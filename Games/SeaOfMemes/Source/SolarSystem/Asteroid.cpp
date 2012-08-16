/*
  Copyright (C) 1995-2012 by Michael J. Goodfellow

  This source code is distributed for free and may be modified, redistributed, and
  incorporated in other projects (commercial, non-commercial and open-source)
  without restriction.  No attribution to the author is required.  There is
  no requirement to make the source code available (no share-alike required.)

  This source code is distributed "AS IS", with no warranty expressed or implied.
  The user assumes all risks related to quality, accuracy and fitness of use.

  Except where noted, this source code is the sole work of the author, but it has 
  not been checked for any intellectual property infringements such as copyrights, 
  trademarks or patents.  The user assumes all legal risks.  The original version 
  may be found at "http://www.sea-of-memes.com".  The author is not responsible 
  for subsequent alterations.

  Retain this copyright notice and add your own copyrights and revisions above
  this notice.
*/
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Util/VertexTerrain.h"

#include "Asteroid.h"

const double SCALE = 1000.0;
const double WATER_LEVEL = -48.0/SCALE;
const double VARIATION = 10.0/SCALE;

//--------------------------------------------------------------
// AsteroidChunk constructor
AsteroidChunk::AsteroidChunk()
{
  m_indexes = NULL;
  m_vertexes = NULL;
  m_waterIndexes = NULL;
  m_waterVertexes = NULL;
}

//--------------------------------------------------------------
// AsteroidChunk destructor
AsteroidChunk::~AsteroidChunk()
{
  delete m_indexes;
  m_indexes = NULL;

  delete m_vertexes;
  m_vertexes = NULL;

  delete m_waterIndexes;
  m_waterIndexes = NULL;

  delete m_waterVertexes;
  m_waterVertexes = NULL;
}

//--------------------------------------------------------------
// return outside diameter of object
void AsteroidChunk::outsidePt(
  mgPoint3& pt,
  double x,
  double y,
  double z)
{                                                      
  double ox = m_origin.x / SCALE;
  double oy = m_origin.y / SCALE;
  double oz = m_origin.z / SCALE;

  double dist = sqrt(x*x + y*y + z*z);
  double nx = x/dist;
  double ny = y/dist;
  double nz = z/dist;
  double ht = m_radius - m_height*mgSimplexNoise::noiseSum(10, nx+ox, ny+oy, nz+oz);
  double thickness = VARIATION*mgSimplexNoise::noise(nx*5+ox, ny*5+oy, nz*5+oz);
  ht += thickness;
  pt.x = nx*ht;
  pt.y = ny*ht;
  pt.z = nz*ht;
}

//--------------------------------------------------------------
// create the polygons for a height map
void AsteroidChunk::addPolygons(
  mgPoint3* points,
  BOOL outward)
{
  // generate the vertex list.
  int depthSize = m_samples+3;  // -1 to size+1
  int vertexSize = m_samples+1;   // 0 to size

  int vertexBase = m_vertexes->getLength();

  VertexTerrain v;
  v.setColor(133, 133, 133);  // gray rock
  for (int i = 0; i < vertexSize; i++)
  {
    for (int j = 0; j < vertexSize; j++)
    {
      int index = (i+1)*depthSize + (j+1);
      mgPoint3 pt = points[index];
      double ht = pt.length() - m_radius;
      v.m_px = (float) (pt.x + m_origin.x);
      v.m_py = (float) (pt.y + m_origin.y);
      v.m_pz = (float) (pt.z + m_origin.z);

      mgPoint3 westPt = points[index-depthSize];
      mgPoint3 eastPt = points[index+depthSize];
      mgPoint3 northPt = points[index-1];
      mgPoint3 southPt = points[index+1];
      northPt.subtract(pt);
      southPt.subtract(pt);
      westPt.subtract(pt);
      eastPt.subtract(pt);

      mgPoint3 normal(northPt);
      normal.cross(westPt);
      normal.normalize();

      v.m_nx = (float) normal.x;
      v.m_ny = (float) normal.y;
      v.m_nz = (float) normal.z;

      normal = westPt;
      normal.cross(southPt);
      normal.normalize();

      v.m_nx += (float) normal.x;
      v.m_ny += (float) normal.y;
      v.m_nz += (float) normal.z;

      normal = southPt;
      normal.cross(eastPt);
      normal.normalize();

      v.m_nx += (float) normal.x;
      v.m_ny += (float) normal.y;
      v.m_nz += (float) normal.z;

      normal = eastPt;
      normal.cross(northPt);
      normal.normalize();

      v.m_nx += (float) normal.x;
      v.m_ny += (float) normal.y;
      v.m_nz += (float) normal.z;

      if (outward)
      {
        v.m_nx /= 4.0f;
        v.m_ny /= 4.0f;
        v.m_nz /= 4.0f;
      }
      else
      {
        v.m_nx /= -4.0f;
        v.m_ny /= -4.0f;
        v.m_nz /= -4.0f;
      }

      v.m_tx = (float) i;
      v.m_ty = (float) j;

      if (m_addWater)
      {
        if (ht > 200/SCALE)
        {
          v.setColor(133, 133, 133);  // gray rock
        }
        else if (ht > -40/SCALE)  
        {
          v.setColor(62, 124, 85); // green grass
        }
        else if (ht > -50/SCALE)    
        {
          v.setColor(255, 204, 161);   // brown sand
        }
        else v.setColor(135, 104, 73);  // brown dirt
      }

      v.addTo(m_vertexes);
    }
  }

  m_indexes->addGrid(vertexBase, vertexSize, m_samples, m_samples, outward);
}

//--------------------------------------------------------------
// create outside surface for chunk
void AsteroidChunk::addOutsideSurface(
  Asteroid* world)
{
  // generate the depth map.  extend into neighbors so we can compute normals
  int depthSize = m_samples+3;  // -1 to size+1
  mgPoint3* points = new mgPoint3[depthSize*depthSize];
  mgPoint3 hit;
  double px, py, pz;

  // ymin outside
  py = 0.0;
  for (int x = -1; x <= m_samples+1; x++)
  {
    px = x/(double) m_samples;
    for (int z = -1; z <= m_samples+1; z++)
    {
      pz = z/(double) m_samples;

      // find depth of object
      outsidePt(hit, px-0.5, py-0.5, pz-0.5);
      points[(m_samples+1-x)*depthSize + (z+1)] = hit;
    }
  }

  addPolygons(points, true);

  // ymax side
  py = 1.0;
  for (int x = -1; x <= m_samples+1; x++)
  {
    px = x/(double) m_samples;
    for (int z = -1; z <= m_samples+1; z++)
    {
      pz = z/(double) m_samples;

      // find depth of object
      outsidePt(hit, px-0.5, py-0.5, pz-0.5);
      points[(x+1)*depthSize + (z+1)] = hit;
    }
  }

  addPolygons(points, true);

  // xmin side
  px = 0.0;
  for (int z = -1; z <= m_samples+1; z++)
  {
    pz = z/(double) m_samples;
    for (int y = -1; y <= m_samples+1; y++)
    {
      py = y/(double) m_samples;

      // find depth of object
      outsidePt(hit, px-0.5, py-0.5, pz-0.5);
      points[(m_samples+1-z)*depthSize + (y+1)] = hit;
    }
  }

  addPolygons(points, true);

  // xmax side
  px = 1.0;
  for (int z = -1; z <= m_samples+1; z++)
  {
    pz = z/(double) m_samples;
    for (int y = -1; y <= m_samples+1; y++)
    {
      py = y/(double) m_samples;

      // find depth of object
      outsidePt(hit, px-0.5, py-0.5, pz-0.5);
      points[(z+1)*depthSize + (y+1)] = hit;
    }
  }
 
  addPolygons(points, true);

  // zmin side
  pz = 0.0;
  for (int y = -1; y <= m_samples+1; y++)
  {
    py = y/(double) m_samples;
    for (int x = -1; x <= m_samples+1; x++)
    {
      px = x/(double) m_samples;

      // find depth of object
      outsidePt(hit, px-0.5, py-0.5, pz-0.5);
      points[(m_samples+1-y)*depthSize + (x+1)] = hit;
    }
  }

  addPolygons(points, true);

  // zmax side
  pz = 1.0;
  for (int y = -1; y <= m_samples+1; y++)
  {
    py = y/(double) m_samples;
    for (int x = -1; x <= m_samples+1; x++)
    {
      px = x/(double) m_samples;

      // find depth of object
      outsidePt(hit, px-0.5, py-0.5, pz-0.5);
      points[(y+1)*depthSize + (x+1)] = hit;
    }
  }

  addPolygons(points, true);

  delete points;
}

//--------------------------------------------------------------
// add water level surface
void AsteroidChunk::addWaterSurface()
{
  mgPoint3 hit;
  double px, py, pz;
  mgVertex v;

  int steps = m_samples/2;
  int vertexSize = 1+steps;

  m_waterVertexes = mgVertex::newBuffer(6*vertexSize*vertexSize);
  m_waterIndexes = mgDisplay->newIndexBuffer(6*6*steps*steps);

  int vertexBase = m_waterVertexes->getLength();

  double tile = (2*PI*m_radius*SCALE)/4;

  // ymin outside
  py = 0.0;
  for (int x = 0; x <= steps; x++)
  {
    px = x/(double) steps;
    for (int z = 0; z <= steps; z++)
    {
      pz = z/(double) steps;

      hit.x = px-0.5;
      hit.y = py-0.5;
      hit.z = 0.5-pz;
      hit.normalize();

      v.m_nx = (float) hit.x;
      v.m_ny = (float) hit.y;
      v.m_nz = (float) hit.z;
      hit.scale(m_radius + WATER_LEVEL);

      v.m_px = (float) (hit.x + m_origin.x);
      v.m_py = (float) (hit.y + m_origin.y);
      v.m_pz = (float) (hit.z + m_origin.z);

      v.m_tx = (float) (px*tile);
      v.m_ty = (float) (pz*tile);

      v.addTo(m_waterVertexes);
    }
  }

  // add indexes
  m_waterIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  vertexBase = m_waterVertexes->getLength();

  // ymax side
  py = 1.0;
  for (int x = 0; x <= steps; x++)
  {
    px = x/(double) steps;
    for (int z = 0; z <= steps; z++)
    {
      pz = z/(double) steps;

      hit.x = px-0.5;
      hit.y = py-0.5;
      hit.z = pz-0.5;
      hit.normalize();

      v.m_nx = (float) hit.x;
      v.m_ny = (float) hit.y;
      v.m_nz = (float) hit.z;
      hit.scale(m_radius + WATER_LEVEL);

      v.m_px = (float) (hit.x + m_origin.x);
      v.m_py = (float) (hit.y + m_origin.y);
      v.m_pz = (float) (hit.z + m_origin.z);

      v.m_tx = (float) (px*tile);
      v.m_ty = (float) (pz*tile);

      v.addTo(m_waterVertexes);
    }
  }

  // add indexes
  m_waterIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  vertexBase = m_waterVertexes->getLength();

  // xmin side
  px = 0.0;
  for (int z = 0; z <= steps; z++)
  {
    pz = z/(double) steps;
    for (int y = 0; y <= steps; y++)
    {
      py = y/(double) steps;

      hit.x = px-0.5;
      hit.y = py-0.5;
      hit.z = 0.5-pz;
      hit.normalize();

      v.m_nx = (float) hit.x;
      v.m_ny = (float) hit.y;
      v.m_nz = (float) hit.z;
      hit.scale(m_radius + WATER_LEVEL);

      v.m_px = (float) (hit.x + m_origin.x);
      v.m_py = (float) (hit.y + m_origin.y);
      v.m_pz = (float) (hit.z + m_origin.z);

      v.m_tx = (float) (pz*tile);
      v.m_ty = (float) (py*tile);

      v.addTo(m_waterVertexes);
    }
  }

  // add indexes
  m_waterIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  vertexBase = m_waterVertexes->getLength();

  // zmin side
  pz = 0.0;
  for (int y = 0; y <= steps; y++)
  {
    py = y/(double) steps;
    for (int x = 0; x <= steps; x++)
    {
      px = x/(double) steps;

      hit.x = 0.5-px;
      hit.y = py-0.5;
      hit.z = pz-0.5;
      hit.normalize();

      v.m_nx = (float) hit.x;
      v.m_ny = (float) hit.y;
      v.m_nz = (float) hit.z;
      hit.scale(m_radius + WATER_LEVEL);

      v.m_px = (float) (hit.x + m_origin.x);
      v.m_py = (float) (hit.y + m_origin.y);
      v.m_pz = (float) (hit.z + m_origin.z);

      v.m_tx = (float) (py*tile);
      v.m_ty = (float) (px*tile);

      v.addTo(m_waterVertexes);
    }
  }

  // add indexes
  m_waterIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  vertexBase = m_waterVertexes->getLength();

  // xmax side
  px = 1.0;
  for (int z = 0; z <= steps; z++)
  {
    pz = z/(double) steps;
    for (int y = 0; y <= steps; y++)
    {
      py = y/(double) steps;

      hit.x = px-0.5;
      hit.y = py-0.5;
      hit.z = pz-0.5;
      hit.normalize();

      v.m_nx = (float) hit.x;
      v.m_ny = (float) hit.y;
      v.m_nz = (float) hit.z;
      hit.scale(m_radius + WATER_LEVEL);

      v.m_px = (float) (hit.x + m_origin.x);
      v.m_py = (float) (hit.y + m_origin.y);
      v.m_pz = (float) (hit.z + m_origin.z);

      v.m_tx = (float) (pz*tile);
      v.m_ty = (float) (py*tile);

      v.addTo(m_waterVertexes);
    }
  }
 
  // add indexes
  m_waterIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  vertexBase = m_waterVertexes->getLength();

  // zmax side
  pz = 1.0;
  for (int y = 0; y <= steps; y++)
  {
    py = y/(double) steps;
    for (int x = 0; x <= steps; x++)
    {
      px = x/(double) steps;

      hit.x = px-0.5;
      hit.y = py-0.5;
      hit.z = pz-0.5;
      hit.normalize();

      v.m_nx = (float) hit.x;
      v.m_ny = (float) hit.y;
      v.m_nz = (float) hit.z;
      hit.scale(m_radius + WATER_LEVEL);

      v.m_px = (float) (hit.x + m_origin.x);
      v.m_py = (float) (hit.y + m_origin.y);
      v.m_pz = (float) (hit.z + m_origin.z);

      v.m_tx = (float) (y*tile);
      v.m_ty = (float) (x*tile);

      v.addTo(m_waterVertexes);
    }
  }

  // add indexes
  m_waterIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);
}

//--------------------------------------------------------------
// AsteroidChunk create buffers
void AsteroidChunk::createBuffers(
  Asteroid* world)
{
  // if buffer already created, nothing to do
  if (m_indexes != NULL)
    return;

  int vertexSize = m_samples+1;   // 0 to size

  // inside and outside times six sides
  int vertexCount = 6*vertexSize*vertexSize;
  m_vertexes = VertexTerrain::newBuffer(vertexCount);

  // inside and outside times six sides times six points per cell
  m_indexes = mgDisplay->newIndexBuffer(6*6*m_samples*m_samples, false, vertexCount > 65535);  

  // create surface for this chunk
  addOutsideSurface(world);

  if (m_addWater)
    addWaterSurface();
}

//--------------------------------------------------------------
// AsteroidChunk delete buffers
void AsteroidChunk::deleteBuffers()
{
  delete m_indexes;
  m_indexes = NULL;

  delete m_vertexes;
  m_vertexes = NULL;

  delete m_waterIndexes;
  m_waterIndexes = NULL;

  delete m_waterVertexes;
  m_waterVertexes = NULL;
}

//--------------------------------------------------------------
// update animation 
BOOL AsteroidChunk::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// render the chunk
void AsteroidChunk::render(
  Asteroid* world)
{
  mgDisplay->setShader("terrain");
  mgDisplay->setTexture(world->m_terrainTexture);
  mgDisplay->draw(MG_TRIANGLES, m_vertexes, m_indexes); 
}

//--------------------------------------------------------------
// render the transparent data
void AsteroidChunk::renderTransparent(
  Asteroid* world)
{
  if (m_waterVertexes != NULL)
  {
    mgDisplay->setShader("litTexture");
    mgDisplay->setTexture(world->m_waterTexture);
    mgDisplay->draw(MG_TRIANGLES, m_waterVertexes, m_waterIndexes);
  }
}

//--------------------------------------------------------------
// constructor
Asteroid::Asteroid(
  const mgOptionsFile& options,
  const mgPoint3& center,
  double radius,
  BOOL addWater)
{
  VertexTerrain::loadShader("terrain");
  mgVertex::loadShader("litTexture");

  mgString fileName;

  options.getFileName("waterTexture", options.m_sourceFileName, "", fileName);
  m_waterTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("terrainTexture", options.m_sourceFileName, "", fileName);
  m_terrainTexture = mgDisplay->loadTexture(fileName);

  m_center = center;

  AsteroidChunk* chunk = new AsteroidChunk();
  chunk->m_addWater = addWater;
  chunk->m_origin = m_center; // mgPoint3(0, 0, 0);
  chunk->m_radius = radius;
  chunk->m_height = 0.8;
  chunk->m_samples = 32;//100;
  m_chunks.add(chunk);
}

//--------------------------------------------------------------
// destructor
Asteroid::~Asteroid()
{
  deleteBuffers();
}
    
//--------------------------------------------------------------
// create buffers ready to send to display
void Asteroid::createBuffers()
{
  for (int i = 0; i < m_chunks.length(); i++)
  {
    AsteroidChunk* chunk = (AsteroidChunk*) m_chunks[i];
    chunk->createBuffers(this);
  }
}

//--------------------------------------------------------------
// delete display buffers
void Asteroid::deleteBuffers()
{
  for (int i = 0; i < m_chunks.length(); i++)
  {
    AsteroidChunk* chunk = (AsteroidChunk*) m_chunks[i];
    delete chunk;
  }
  m_chunks.removeAll();
}

//--------------------------------------------------------------
// animate object
BOOL Asteroid::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  BOOL changed = false;
  for (int i = 0; i < m_chunks.length(); i++)
  {
    AsteroidChunk* chunk = (AsteroidChunk*) m_chunks[i];
    changed |= chunk->animate(now, since);
  }
  return changed;
}

//--------------------------------------------------------------
// draw opaque data
void Asteroid::render()
{
  for (int i = 0; i < m_chunks.length(); i++)
  {
    AsteroidChunk* chunk = (AsteroidChunk*) m_chunks[i];
    chunk->render(this);
  }
}

//--------------------------------------------------------------
// draw transparent data
void Asteroid::renderTransparent()
{
  for (int i = 0; i < m_chunks.length(); i++)
  {
    AsteroidChunk* chunk = (AsteroidChunk*) m_chunks[i];
    chunk->renderTransparent(this);
  }
}
