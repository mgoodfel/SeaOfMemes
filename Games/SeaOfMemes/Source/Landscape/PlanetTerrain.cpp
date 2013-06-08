/*
  Copyright (C) 1995-2013 by Michael J. Goodfellow

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

#include "PlanetTerrain.h"

const int MINIMUM_SIZE = TERRAIN_SIZE;

//--------------------------------------------------------------
// return height of landscape at point
// external version called by Landscape program
double PlanetTerrain::heightAtPt(
  double tx, 
  double tz)
{
  double ht = mgSimplexNoise::noiseSum(10, tx/40000.0, tz/40000.0);
  ht = 10.0*exp(5.0*fabs(ht));
  ht += 20*mgSimplexNoise::noiseSum(3, tx/1333.0, tz/1333.0);
  return ht;
}

//--------------------------------------------------------------
// return height of landscape at point
// internal version as inline function for performance
inline double PlanetTerrain::landscapeHeight(
  double tx, 
  double tz)
{
  double ht = mgSimplexNoise::noiseSum(10, tx/40000.0, tz/40000.0);
  ht = 10.0*exp(5.0*fabs(ht));
  ht += 20*mgSimplexNoise::noiseSum(3, tx/1333.0, tz/1333.0);
  return ht;
}

//--------------------------------------------------------------
// constructor
PlanetTerrain::PlanetTerrain(
  int originX,
  int originZ,
  int extentX,
  int extentZ)
{
  m_waterVertexes = NULL;
  m_vertexes = NULL;
  m_indexes = NULL;
  m_heights = NULL;

  m_minHeight = WATER_LEVEL;
  m_maxHeight = WATER_LEVEL;

  m_originX = originX;
  m_originZ = originZ;
  m_extentX = extentX;
  m_extentZ = extentZ;

  m_xminStep = m_zminStep = m_xmaxStep = m_zmaxStep = 1;
}

//--------------------------------------------------------------
// destructor
PlanetTerrain::~PlanetTerrain()
{
  // delete buffers first.  presence of children used to flag leaf node
  deleteBuffers();

  delete m_heights;
  m_heights = NULL;
}

//--------------------------------------------------------------
// generate terrain points
void PlanetTerrain::createTerrain()
{
//  double start = mgOSGetTime();

  m_minHeight = WATER_LEVEL;
  m_maxHeight = WATER_LEVEL;

  // generate the height map.  extend into neighbors so we can compute normals
  int heightSize = TERRAIN_SIZE+3;  // -1 to size+1

  if (m_heights == NULL)
    m_heights = new float[heightSize*heightSize];

  int step = m_extentX / TERRAIN_SIZE;

  for (int z = -1; z <= TERRAIN_SIZE+1; z++)
  {
    for (int x = -1; x <= TERRAIN_SIZE+1; x++)
    {
      double tx = m_originX+x*step;
      double tz = m_originZ+z*step;

      double ht = landscapeHeight(tx, tz);
      m_heights[(z+1)*heightSize + (x+1)] = (float) ht;

      m_minHeight = min(m_minHeight, ht);
      m_maxHeight = max(m_maxHeight, ht);
    }
  }

  m_rebuildHeightmap = false;

//  double end = mgOSGetTime();
//  mgDebug("create time = %g ms", end-start);

  //mgDebug("%d, %d (%d) heights from %g to %g", m_originX, m_originZ, m_extentX, m_minHeight, m_maxHeight);
}

//--------------------------------------------------------------
// generate terrain from noise function
void PlanetTerrain::createTerrainBuffers()
{
//  mgDebug("createTerrain in (%d, %d) %d", m_originX, m_originZ, m_extentX);

  // create terrain for this chunk
  int step = m_extentX / TERRAIN_SIZE;
  int heightSize = TERRAIN_SIZE+3;  // -1 to size+1

  if (m_heights == NULL)
    createTerrain();

  // generate the vertex list.
  int vertexSize = TERRAIN_SIZE+1;   // 0 to size

  // delete old buffer 
  delete m_vertexes;
  m_vertexes = NULL;

  m_vertexes = VertexTerrain::newBuffer(vertexSize*vertexSize);
  VertexTerrain v;
  for (int z = 0; z < vertexSize; z++)
  {
    for (int x = 0; x < vertexSize; x++)
    {
      int index = (z+1)*heightSize + (x+1);
      float ht;

      // on the outside edges, interpolate the points to match low res neighbors
      // skip the points that are not interpolated
      if (x == 0 && (z % m_xminStep) != 0)
      {
        // first col.  steps are res of m_xminStep
        int topZ = m_xminStep*(z/m_xminStep);
        int topIndex = (topZ+1)*heightSize + (x+1);
        float topHt = m_heights[topIndex];
        float botHt = m_heights[topIndex+m_xminStep*heightSize];

        // interpolate between low res points
        ht = topHt + ((z%m_xminStep) * (botHt-topHt))/m_xminStep;
      }
      else if (x == vertexSize-1 && (z % m_xmaxStep) != 0)
      {
        // last col.  steps are res of m_xmaxStep
        int topZ = m_xmaxStep*(z/m_xmaxStep);
        int topIndex = (topZ+1)*heightSize + (x+1);
        float topHt = m_heights[topIndex];
        float botHt = m_heights[topIndex+m_xmaxStep*heightSize];

        // interpolate between low res points
        ht = topHt + ((z%m_xmaxStep) * (botHt-topHt))/m_xmaxStep;
      }
      else if (z == 0 && (x % m_zminStep) != 0)
      {
        // first row.  steps are res of m_zminStep
        int leftX = m_zminStep*(x/m_zminStep);
        int leftIndex = (z+1)*heightSize + (leftX+1);
        float leftHt = m_heights[leftIndex];
        float rightHt = m_heights[leftIndex+m_zminStep];

        // interpolate between low res points
        ht = leftHt + ((x%m_zminStep) * (rightHt-leftHt))/m_zminStep;
      }
      else if (z == vertexSize-1 && (x % m_zmaxStep) != 0)
      {
        // last row.  steps are res of m_zmaxStep
        int leftX = m_zmaxStep*(x/m_zmaxStep);
        int leftIndex = (z+1)*heightSize + (leftX+1);
        float leftHt = m_heights[leftIndex];
        float rightHt = m_heights[leftIndex+m_zmaxStep];

        // interpolate between low res points
        ht = leftHt + ((x%m_zmaxStep) * (rightHt-leftHt))/m_zmaxStep;
      }
      else
      {
        // interior point or unmodified edge point
        ht = m_heights[index];
      }

      v.m_px = (float) (x*step);
      v.m_py = ht;
      v.m_pz = (float) (z*step);

      v.m_nx = 0.0f;
      v.m_ny = 0.0f;
      v.m_nz = 0.0f;

      float slope, len;
      slope = m_heights[index-1] - ht;
      len = sqrt(slope*slope+step*step);
      v.m_nx += slope/len;
      v.m_ny += step/len;

      slope = m_heights[index+1] - ht;
      len = sqrt(slope*slope+step*step);
      v.m_nx += -slope/len;
      v.m_ny += step/len;

      slope = m_heights[index-heightSize] - ht;
      len = sqrt(slope*slope+step*step);
      v.m_nz += slope/len;
      v.m_ny += step/len;

      slope = m_heights[index+heightSize] - ht;
      len = sqrt(slope*slope+step*step);
      v.m_nz += -slope/len;
      v.m_ny += step/len;

      v.m_nx /= 4.0f;
      v.m_ny /= 4.0f;
      v.m_nz /= 4.0f;

      v.m_tx = (float) (x*step);
      v.m_ty = (float) (z*step);

      if (ht > 3000)
      {
        v.setColor(133, 133, 133);  // gray rock
      }
      else if (ht > 300)  
      {
        v.setColor(62, 124, 85); // green grass
      }
      else if (ht > 100)  
      {
        v.setColor(241, 220, 177); // sand
      }
      else 
      {
        double inten = max(0, ht)/100.0;
        v.setColor((BYTE) (inten*179), (BYTE) (inten*148), (BYTE) (inten*117));   // dirt
      }

      v.addTo(m_vertexes);
    }
  }

  // delete old indexes
  delete m_indexes;
  m_indexes = NULL;

  m_indexes = mgDisplay->newIndexBuffer(6*TERRAIN_SIZE*TERRAIN_SIZE);
  for (int z = 0; z < TERRAIN_SIZE; z++)
  {
    for (int x = 0; x < TERRAIN_SIZE; x++)
    {
      int index = z*vertexSize+x;
      m_indexes->addIndex(index);  // tl
      m_indexes->addIndex(index+1);  // tr
      m_indexes->addIndex(index+vertexSize);  // bl
      m_indexes->addIndex(index+vertexSize);  // bl
      m_indexes->addIndex(index+1);  // tr
      m_indexes->addIndex(index+vertexSize+1);  // br
    }
  }
}

//--------------------------------------------------------------
// create the water vertexes
void PlanetTerrain::createWaterBuffers()
{
  mgVertex tl,tr,bl,br;

  delete m_waterVertexes;
  m_waterVertexes = NULL;

  // three points times two triangles
  m_waterVertexes = mgVertex::newBuffer(3*2);

  // low x, low z
  bl.m_nx = 0.0f;  bl.m_ny = 1.0f;    bl.m_nz = 0.0f;
  bl.m_px = 0.0f;  bl.m_py = WATER_LEVEL;  bl.m_pz = 0.0f;

  bl.m_tx = 0.0f;  bl.m_ty = 0.0f;

  // low x, high z
  tl.m_nx = 0.0f;  tl.m_ny = 1.0f; tl.m_nz = 0.0f;
  tl.m_px = 0.0f;  tl.m_py = WATER_LEVEL;  tl.m_pz = (float) m_extentZ;

  tl.m_tx = 0.0f;  tl.m_ty = (float) m_extentZ;

  // high x, low z
  br.m_nx = 0.0f;  br.m_ny = 1.0f; br.m_nz = 0.0f;
  br.m_px = (float) m_extentX;  br.m_py = WATER_LEVEL;  br.m_pz = 0.0f;

  br.m_tx = (float) m_extentX;  br.m_ty = 0.0f;

  // high x, high z
  tr.m_nx = 0.0f;  tr.m_ny = 1.0f; tr.m_nz = 0.0f;
  tr.m_px = (float) m_extentX;  tr.m_py = WATER_LEVEL;  tr.m_pz = (float) m_extentZ;

  tr.m_tx = (float) m_extentX;  tr.m_ty = (float) m_extentZ;

  tl.addTo(m_waterVertexes);
  tr.addTo(m_waterVertexes);
  bl.addTo(m_waterVertexes);

  bl.addTo(m_waterVertexes);
  tr.addTo(m_waterVertexes);
  br.addTo(m_waterVertexes);
}

//--------------------------------------------------------------
// create buffers for this chunk of terrain
BOOL PlanetTerrain::createBuffers()
{
  BOOL changed = false;

  // if this is a leaf node
  if (m_children[0] == NULL)
  {
    if (m_vertexes == NULL || m_rebuildBuffers)
    {
      createTerrainBuffers();
      createWaterBuffers();
      changed = true;
    }

    // buffers regenerated
    m_rebuildBuffers = false;
  }
  else
  {
    // recurse to children, update min/max heights
    m_minHeight = INT_MAX;
    m_maxHeight = -INT_MAX;
    for (int i = 0; i < 4; i++)
    {
      changed |= m_children[i]->createBuffers();

      m_minHeight = min(m_minHeight, m_children[i]->m_minHeight);
      m_maxHeight = min(m_maxHeight, m_children[i]->m_maxHeight);
    }
  }

  return changed;
}

//--------------------------------------------------------------
// delete buffers
void PlanetTerrain::deleteBuffers()
{
//  mgDebug("deleteBuffers in (%d, %d) %d", m_originX, m_originZ, m_extentX);

  delete m_indexes;
  m_indexes = NULL;
  delete m_vertexes;
  m_vertexes = NULL;

  delete m_waterVertexes;
  m_waterVertexes = NULL;

  // recurse to children
  for (int i = 0; i < 4; i++)
  {
    if (m_children[i] != NULL)
      m_children[i]->deleteBuffers();
  }
}
//--------------------------------------------------------------
// render the buffers
void PlanetTerrain::render(
  const mgPoint3& renderEyePt,
  mgTextureImage* terrain)
{
  // compute bounds of sphere enclosing chunk
  mgPoint3 center;
  center.x = m_originX + m_extentX/2;
  center.z = m_originZ + m_extentZ/2;
  center.y = (m_minHeight + m_maxHeight)/2;
  mgPoint3 corner(m_extentX/2, m_maxHeight - center.y, m_extentZ/2);
  double radius = corner.length();
  center.subtract(renderEyePt);

  if (!mgDisplay->withinFrustum(center.x, center.y, center.z, radius))
    return;

  if (m_children[0] == NULL)
  {
    mgDisplay->setTexture(terrain);

    mgMatrix4 model;
    model.translate(m_originX - renderEyePt.x, -renderEyePt.y, m_originZ-renderEyePt.z);
    mgDisplay->setModelTransform(model);

    mgDisplay->draw(MG_TRIANGLES, m_vertexes, m_indexes);
  }
  else
  {
    // recurse to children
    for (int i = 0; i < 4; i++)
    {
      m_children[i]->render(renderEyePt, terrain);
    }                          
  }
}

//--------------------------------------------------------------
// render the transparent data
void PlanetTerrain::renderTransparent(
  const mgPoint3& renderEyePt,
  mgTextureImage* water)
{
  // compute bounds of sphere enclosing chunk
  mgPoint3 center;
  center.x = m_originX + m_extentX/2;
  center.z = m_originZ + m_extentZ/2;
  center.y = WATER_LEVEL;
  mgPoint3 corner(m_extentX/2, 0.0, m_extentZ/2);
  double radius = corner.length();
  center.subtract(renderEyePt);

  // if not visible 
  if (!mgDisplay->withinFrustum(center.x, center.y, center.z, radius))
    return;

  if (m_children[0] == NULL)
  {
    mgMatrix4 model;
    model.translate(m_originX - renderEyePt.x, -renderEyePt.y, m_originZ-renderEyePt.z);
    mgDisplay->setModelTransform(model);

    mgDisplay->setTexture(water);
    mgDisplay->draw(MG_TRIANGLES, m_waterVertexes);
  }
  else
  {
    // recurse to children
    for (int i = 0; i < 4; i++)
    {
      m_children[i]->renderTransparent(renderEyePt, water);
    }                          
  }
}

//--------------------------------------------------------------
// split chunk into four children
void PlanetTerrain::splitChunk()
{
//  mgDebug("split chunk (%d,%d) %d", m_originX, m_originZ, m_extentX);

  deleteBuffers();

  int sizeX = m_extentX/2;
  int sizeZ = m_extentZ/2;

  PlanetTerrain* chunk = new PlanetTerrain(m_originX, m_originZ, sizeX, sizeZ);
  chunk->extractHeightmapFrom(m_heights, 0, 0);
  m_children[0] = chunk;

  chunk = new PlanetTerrain(m_originX + sizeX, m_originZ, sizeX, sizeZ);
  chunk->extractHeightmapFrom(m_heights, TERRAIN_SIZE/2, 0);
  m_children[1] = chunk;

  chunk = new PlanetTerrain(m_originX, m_originZ + sizeZ, sizeX, sizeZ);
  chunk->extractHeightmapFrom(m_heights, 0, TERRAIN_SIZE/2);
  m_children[2] = chunk;

  chunk = new PlanetTerrain(m_originX + sizeX, m_originZ + sizeZ, sizeX, sizeZ);
  chunk->extractHeightmapFrom(m_heights, TERRAIN_SIZE/2, TERRAIN_SIZE/2);
  m_children[3] = chunk;

  delete m_heights;
  m_heights = NULL;
}

//--------------------------------------------------------------
// extract heightmap from parent during split
void PlanetTerrain::extractHeightmapFrom(
  const float* parentHeights,
  int originX,
  int originZ)
{
  int step = m_extentX / TERRAIN_SIZE;
  int heightSize = TERRAIN_SIZE+3;  // -1 to size+1
  m_heights = new float[heightSize*heightSize];

  m_minHeight = WATER_LEVEL;
  m_maxHeight = WATER_LEVEL;

  // copy height data from parent
  for (int z = 0; z <= TERRAIN_SIZE; z+=2)
  {
    for (int x = 0; x <= TERRAIN_SIZE; x+=2)
    {
      float ht = parentHeights[(originZ + z/2+1)*heightSize + (originX + x/2+1)];
      m_heights[(z+1)*heightSize + (x+1)] = ht;

      m_minHeight = min(m_minHeight, ht);
      m_maxHeight = max(m_maxHeight, ht);
    }
  }

  // interpolate heights on odd cols of even rows
  for (int z = 0; z <= TERRAIN_SIZE; z+=2)
  {
    for (int x = 1; x <= TERRAIN_SIZE; x+=2)
    {
      float ht = m_heights[(z+1)*heightSize + (x+0)];
      ht += m_heights[(z+1)*heightSize + (x+2)];
      ht /= 2.0f;
      m_heights[(z+1)*heightSize + (x+1)] = ht;

      m_minHeight = min(m_minHeight, ht);
      m_maxHeight = max(m_maxHeight, ht);
    }
  }

  // interpolate heights on odd rows of even cols
  for (int z = 1; z <= TERRAIN_SIZE; z+=2)
  {
    for (int x = 0; x <= TERRAIN_SIZE; x+=2)
    {
      float ht = m_heights[(z+0)*heightSize + (x+1)];
      ht += m_heights[(z+2)*heightSize + (x+1)];
      ht /= 2.0f;

      m_heights[(z+1)*heightSize + (x+1)] = ht;

      m_minHeight = min(m_minHeight, ht);
      m_maxHeight = max(m_maxHeight, ht);
    }
  }

  // interpolate heights on odd rows of odd cols
  for (int z = 1; z <= TERRAIN_SIZE; z+=2)
  {
    for (int x = 1; x <= TERRAIN_SIZE; x+=2)
    {
      float ht = m_heights[(z+0)*heightSize + (x+0)];
      ht += m_heights[(z+0)*heightSize + (x+2)];
      ht += m_heights[(z+2)*heightSize + (x+0)];
      ht += m_heights[(z+2)*heightSize + (x+2)];
      ht /= 4.0f;

      m_heights[(z+1)*heightSize + (x+1)] = ht;

      m_minHeight = min(m_minHeight, ht);
      m_maxHeight = max(m_maxHeight, ht);
    }
  }

  // now we need edge values which were not in parent
  int coords[] = {-1, TERRAIN_SIZE, TERRAIN_SIZE+1};
  for (int x = -1; x <= TERRAIN_SIZE+1; x++)
  {
    for (int i = 0; i < 3; i++)
    {
      int z = coords[i];
      double tx = m_originX+x*step;
      double tz = m_originZ+z*step;

      double ht = landscapeHeight(tx, tz);
      m_heights[(z+1)*heightSize + (x+1)] = (float) ht;

      m_minHeight = min(m_minHeight, ht);
      m_maxHeight = max(m_maxHeight, ht);
    }
  }

  for (int z = -1; z <= TERRAIN_SIZE+1; z++)
  {
    for (int i = 0; i < 3; i++)
    {
      int x = coords[i];
      double tx = m_originX+x*step;
      double tz = m_originZ+z*step;

      double ht = landscapeHeight(tx, tz);
      m_heights[(z+1)*heightSize + (x+1)] = (float) ht;

      m_minHeight = min(m_minHeight, ht);
      m_maxHeight = max(m_maxHeight, ht);
    }
  }

  m_rebuildBuffers = true;
}

//--------------------------------------------------------------
// combine children into one chunk
void PlanetTerrain::combineChildren()
{
//  mgDebug("combine chunk (%d,%d) %d", m_originX, m_originZ, m_extentX);
  int step = m_extentX / TERRAIN_SIZE;
  int heightSize = TERRAIN_SIZE+3;  // -1 to size+1
  m_heights = new float[heightSize*heightSize];

  // copy height data from each child
  const int HALF_TERRAIN = TERRAIN_SIZE/2;
  for (int z = 0; z < 2; z++)
  {
    for (int x = 0; x < 2; x++)
    {
      int target = x * HALF_TERRAIN + z * HALF_TERRAIN * heightSize;
      target += 1 + heightSize;  // starting at cell 1,1
      int child = x + z*2;
      float* childHeights = ((PlanetTerrain*) m_children[child])->m_heights;

      // copy every other height from child
      for (int childZ = 0; childZ < TERRAIN_SIZE; childZ += 2)
      {
        for (int childX = 0; childX < TERRAIN_SIZE; childX += 2)
        {
          m_heights[target++] = childHeights[1+childX + (1+childZ)*heightSize];
        }
        target += heightSize - HALF_TERRAIN;
      }
    }
  }

  // now we need edge values which were not in any child
  int coords[] = {-1, TERRAIN_SIZE, TERRAIN_SIZE+1};
  for (int x = -1; x <= TERRAIN_SIZE+1; x++)
  {
    for (int i = 0; i < 3; i++)
    {
      int z = coords[i];
      double tx = m_originX+x*step;
      double tz = m_originZ+z*step;

      double ht = landscapeHeight(tx, tz);
      m_heights[(z+1)*heightSize + (x+1)] = (float) ht;
    }
  }

  for (int z = -1; z <= TERRAIN_SIZE+1; z++)
  {
    for (int i = 0; i < 3; i++)
    {
      int x = coords[i];
      double tx = m_originX+x*step;
      double tz = m_originZ+z*step;

      double ht = landscapeHeight(tx, tz);
      m_heights[(z+1)*heightSize + (x+1)] = (float) ht;
    }
  }

  // max and min heights should already be set from when this was leaf chunk

  // if any child was a temporary heightmap, this chunk needs rebuild.

  // delete the children
  for (int i = 0; i < 4; i++)
  {
    if (m_children[i]->m_rebuildHeightmap)
      m_rebuildHeightmap = true;

    delete m_children[i];
    m_children[i] = NULL;
  }

  m_rebuildBuffers = true;
}

//--------------------------------------------------------------------
// return resolution of terrain at point
int PlanetTerrain::resolutionAt(
  int ptx,
  int ptz)
{
  if (m_children[0] == NULL)
    return m_extentX;

  int index = (ptx - m_originX) >= (m_extentX/2) ? 1 : 0;
  index += (ptz - m_originZ) >= (m_extentZ/2) ? 2 : 0;

  return m_children[index]->resolutionAt(ptx, ptz);
}

//BOOL tracing = false;

//--------------------------------------------------------------
// return true if resolution too fine
BOOL PlanetTerrain::tooFine(
  const mgPoint3& eyePt)
{
  double dist;

  // figure distance from eye to edges
  double ldx = eyePt.x - m_originX;
  double hdx = (m_originX + m_extentX) - eyePt.x;
  double ldz = eyePt.z - m_originZ;
  double hdz = (m_originZ + m_extentZ) - eyePt.z;

  // if inside this chunk
  if (ldx > 0 && hdx > 0 && ldz > 0 && hdz > 0)
    return false;  // don't drop resolution

//  // get average distance to chunk
//  double dx = (fabs(ldx) + fabs(hdx))/2;
//  double dz = (fabs(ldz) + fabs(hdz))/2;

  // get min distance to chunk
  double dx, dz; 

  // if we're nearest edge, no x component
  if (ldx > 0 && hdx > 0)
    dx = 0.0;
  else dx = min(fabs(ldx), fabs(hdx));

  // if we're nearest edge, no z component
  if (ldz > 0 && hdz > 0)
    dz = 0.0;
  else dz = min(fabs(ldz), fabs(hdz));

  dist = sqrt(dx*dx+dz*dz);

  // don't divide by small distance if near edge
  dist = max(1.0, dist);

  // want unit of landscape to stay roughly same size with distance
  BOOL result = (m_extentX / dist) < 1.0;

/*
  if (tracing)
    mgDebug("(%d, %d) %d dist = %g, extent/dist = %g, tooFine = %s", m_originX, m_originZ, m_extentX, 
      dist, m_extentX/dist, result ? "true" : "false");
*/
  
  return result;
}

//--------------------------------------------------------------
// return true if resolution too coarse
BOOL PlanetTerrain::tooCoarse(
  const mgPoint3& eyePt)
{
  // can't subdivide below 1 unit per cell
  if (m_extentX <= MINIMUM_SIZE)
    return false;

  double dist;

  // figure distance from eye to edges
  double ldx = eyePt.x - m_originX;
  double hdx = (m_originX + m_extentX) - eyePt.x;
  double ldz = eyePt.z - m_originZ;
  double hdz = (m_originZ + m_extentZ) - eyePt.z;

  // if inside this chunk
  if (ldx > 0 && hdx > 0 && ldz > 0 && hdz > 0)
    return true;  // go to max resolution

  // get average distance to chunk
//  double dx = (fabs(ldx) + fabs(hdx))/2;
//  double dz = (fabs(ldz) + fabs(hdz))/2;

  // get min distance to chunk
  double dx, dz; 

  // if we're nearest edge, no x component
  if (ldx > 0 && hdx > 0)
    dx = 0.0;
  else dx = min(fabs(ldx), fabs(hdx));

  // if we're nearest edge, no z component
  if (ldz > 0 && hdz > 0)
    dz = 0.0;
  else dz = min(fabs(ldz), fabs(hdz));

  dist = sqrt(dx*dx+dz*dz);

  // don't divide by small distance if near edge
  dist = max(1.0, dist);

  // want unit of landscape to stay roughly same size with distance
  BOOL result = (m_extentX / dist) > 3.0;

/*
  if (tracing)
    mgDebug("(%d, %d) %d dist = %g, extent/dist = %g, tooCoarse = %s", m_originX, m_originZ, m_extentX, 
        dist, m_extentX/dist, result ? "true" : "false");
*/
  return result;
}

