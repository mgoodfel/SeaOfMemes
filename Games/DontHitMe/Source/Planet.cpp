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

#include "Planet.h"

//--------------------------------------------------------------
// constructor
Planet::Planet(
  const mgOptionsFile& options)
{
  mgVertex::loadShader("cave");

  m_outsideIndexes = NULL;
  m_outsideVertexes = NULL;
  m_insideIndexes = NULL;
  m_insideVertexes = NULL;
  m_lavaIndexes = NULL;
  m_lavaVertexes = NULL;

  mgString fileName;
  options.getFileName("asteroid-outside", options.m_sourceFileName, "terrain.jpg", fileName);
  m_outsideTexture = mgDisplay->loadTexture(fileName);
  options.getFileName("asteroid-inside", options.m_sourceFileName, "terrain.jpg", fileName);
  m_insideTexture = mgDisplay->loadTexture(fileName);
  options.getFileName("asteroid-lava", options.m_sourceFileName, "terrain.jpg", fileName);
  m_lavaTexture = mgDisplay->loadTexture(fileName);

  m_outsideRadius = 200.0;
  m_outsideHeight = 50.0;

  m_insideRadius = 270.0;
  m_insideHeight = 170.0;

  m_lavaRadius = 120.0;

  m_samples = 64;
}

//--------------------------------------------------------------
// destructor
Planet::~Planet()
{
  deleteBuffers();
}

//--------------------------------------------------------------
// return outside diameter of object
double Planet::outsideHt(
  const mgPoint3& pt)
{                                                      
  double ht = m_outsideRadius - m_outsideHeight*mgSimplexNoise::noiseSum(10, pt.x*0.5, pt.y*0.5, pt.z*0.5);
//  double thickness = VARIATION*mgSimplexNoise::noiseSum(3, pt.x*5, pt.y*5, pt.z*5);
//  ht += thickness;
  return ht;
}

//--------------------------------------------------------------
// return inside diameter of object
double Planet::insideHt(
  const mgPoint3& pt)
{                                                      
  double ht = m_insideRadius - m_insideHeight*mgSimplexNoise::noiseSumAbs(10, 1.3*pt.x, 1.3*pt.y, 1.3*pt.z);
  return ht;
}

//--------------------------------------------------------------
// figure the normal for a point in grid
void Planet::pointNormal(
  mgVertex& v,
  double* heights,
  mgPoint3* points,
  int index,
  int rowSize,
  BOOL outward)
{
  double ht = heights[index];
  mgPoint3* npt = &points[index];
  mgPoint3 pt(ht*npt->x, ht*npt->y, ht*npt->z);

  v.m_px = (float) pt.x;
  v.m_py = (float) pt.y;
  v.m_pz = (float) pt.z;

  ht = heights[index-rowSize];
  npt = &points[index-rowSize];
  mgPoint3 westPt(ht*npt->x, ht*npt->y, ht*npt->z);

  ht = heights[index+rowSize];
  npt = &points[index+rowSize];
  mgPoint3 eastPt(ht*npt->x, ht*npt->y, ht*npt->z);

  ht = heights[index-1];
  npt = &points[index-1];
  mgPoint3 northPt(ht*npt->x, ht*npt->y, ht*npt->z);

  ht = heights[index+1];
  npt = &points[index+1];
  mgPoint3 southPt(ht*npt->x, ht*npt->y, ht*npt->z);

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

  float len = sqrt(v.m_nx * v.m_nx + v.m_ny * v.m_ny + v.m_nz * v.m_nz);
  if (!outward)
    len = -len;
  v.m_nx /= len;
  v.m_ny /= len;
  v.m_nz /= len;
}

//--------------------------------------------------------------
// create the polygons for a height map
void Planet::addPolygons(
  double* insideHts,
  double* outsideHts,
  mgPoint3* points,
  BOOL* flags)
{
  // generate the vertex list.
  int depthSize = m_samples+3;  // -1 to size+1
  int vertexSize = m_samples+1;   // 0 to size

  memset(flags, 0, depthSize*depthSize*sizeof(BOOL));

  mgVertex v;
  int vertexBase = m_lavaVertexes->getLength();

  // generate all the lava vertexes
  for (int i = 0; i < vertexSize; i++)
  {
    for (int j = 0; j < vertexSize; j++)
    {
      int index = (i+1)*depthSize + (j+1);
      mgPoint3 pt = points[index];

      v.m_px = (float) (m_lavaRadius * pt.x);
      v.m_py = (float) (m_lavaRadius * pt.y);
      v.m_pz = (float) (m_lavaRadius * pt.z);

      v.m_nx = (float) pt.x;
      v.m_ny = (float) pt.y;
      v.m_nz = (float) pt.z;

      v.m_tx = 4*i/(float) vertexSize;
      v.m_ty = 4*j/(float) vertexSize;

      v.addTo(m_lavaVertexes);
    }
  }

  m_lavaIndexes->addGrid(vertexBase, vertexSize, m_samples, m_samples, true);

  // draw the correct triangles based on inside and outside heights
  for (int i = 0; i < m_samples; i++)
  {
    for (int j = 0; j < m_samples; j++)
    {
      int tlh = (i+1)*depthSize+(j+1);
      int trh = tlh+1;
      int blh = tlh+depthSize;
      int brh = blh+1;

      int tlv = vertexBase + i*vertexSize+j;
      int trv = tlv+1;
      int blv = tlv+vertexSize;
      int brv = blv+1;

      // if any inside ht <= outsideHt
      if (insideHts[tlh] <= outsideHts[tlh] ||
          insideHts[blh] <= outsideHts[blh] ||
          insideHts[trh] <= outsideHts[trh])
      {
        m_insideIndexes->addIndex(tlv);
        m_insideIndexes->addIndex(blv);
        m_insideIndexes->addIndex(trv);

        m_outsideIndexes->addIndex(tlv);
        m_outsideIndexes->addIndex(trv);
        m_outsideIndexes->addIndex(blv);
      }
      else
      {        
        flags[tlh] = flags[blh] = flags[trh] = true;
      }

      // if any inside ht <= outsideHt
      if (insideHts[blh] <= outsideHts[blh] ||
          insideHts[brh] <= outsideHts[brh] ||
          insideHts[trh] <= outsideHts[trh])
      {
        m_insideIndexes->addIndex(blv);
        m_insideIndexes->addIndex(brv);
        m_insideIndexes->addIndex(trv);

        m_outsideIndexes->addIndex(blv);
        m_outsideIndexes->addIndex(trv);
        m_outsideIndexes->addIndex(brv);
      }
      else
      {
        flags[blh] = flags[brh] = flags[trh] = true;
      }
    }
  }

  // generate new heights at points shared between inside and outside
  for (int i = 0; i < depthSize; i++)
  {
    for (int j = 0; j < depthSize; j++)
    {
      //int index = (i+1)*depthSize+(j+1);
      int index = i*depthSize+j;

      double ht = min(insideHts[index], outsideHts[index]);

      // if we're leaving an opening, make sure edges match
      if (flags[index])
        outsideHts[index] = ht;

      // never let inside points extend beyond outside
      insideHts[index] = ht;
    }
  }

  // generate all the inside vertexes
  for (int i = 0; i < vertexSize; i++)
  {
    for (int j = 0; j < vertexSize; j++)
    {
      int index = (i+1)*depthSize + (j+1);

      // calculate normal based on neighbors
      pointNormal(v, insideHts, points, index, depthSize, false);
      v.m_tx = (float) i;
      v.m_ty = (float) j;

      v.addTo(m_insideVertexes);
    }
  }

  // generate all the outside vertexes
  for (int i = 0; i < vertexSize; i++)
  {
    for (int j = 0; j < vertexSize; j++)
    {
      int index = (i+1)*depthSize + (j+1);

      // calculate normal based on neighbors
      pointNormal(v, outsideHts, points, index, depthSize, true);
      v.m_tx = (float) i;
      v.m_ty = (float) j;

      v.addTo(m_outsideVertexes);
    }
  }

}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Planet::createBuffers()
{
  int vertexSize = m_samples+1;
  m_outsideVertexes = mgVertexTA::newBuffer(6*vertexSize*vertexSize);
  m_outsideIndexes = mgDisplay->newIndexBuffer(6*6*m_samples*m_samples, false, true);

  m_insideVertexes = mgVertexTA::newBuffer(6*vertexSize*vertexSize);
  m_insideIndexes = mgDisplay->newIndexBuffer(6*6*m_samples*m_samples, false, true);

  m_lavaVertexes = mgVertexTA::newBuffer(6*vertexSize*vertexSize);
  m_lavaIndexes = mgDisplay->newIndexBuffer(6*6*m_samples*m_samples, false, true);

  // generate the depth map.  extend into neighbors so we can compute normals
  int depthSize = m_samples+3;  // -1 to size+1
  double* outsideHts = new double[depthSize*depthSize];
  double* insideHts = new double[depthSize*depthSize];
  mgPoint3* points = new mgPoint3[depthSize*depthSize];
  BOOL* flags = new BOOL[depthSize*depthSize];
  double px, py, pz;

  // ymin outside
  py = 0.0;
  for (int x = -1; x <= m_samples+1; x++)
  {
    px = x/(double) m_samples;
    for (int z = -1; z <= m_samples+1; z++)
    {
      pz = z/(double) m_samples;

      mgPoint3 pt(px-0.5, py-0.5, pz-0.5);
      pt.normalize();

      double inside = insideHt(pt);
      double outside = outsideHt(pt);

      // find heights of inside and outside edges
      int posn = (m_samples+1-x)*depthSize + (z+1);
      points[posn] = pt;
      insideHts[posn] = inside; // max(m_lavaRadius, min(outside, inside));
      outsideHts[posn] = outside;
    }
  }

  addPolygons(insideHts, outsideHts, points, flags);

  // ymax side
  py = 1.0;
  for (int x = -1; x <= m_samples+1; x++)
  {
    px = x/(double) m_samples;
    for (int z = -1; z <= m_samples+1; z++)
    {
      pz = z/(double) m_samples;

      mgPoint3 pt(px-0.5, py-0.5, pz-0.5);
      pt.normalize();

      // find heights of inside and outside edges
      double inside = insideHt(pt);
      double outside = outsideHt(pt);

      int posn = (x+1)*depthSize + (z+1);
      points[posn] = pt;
      insideHts[posn] = inside; // max(m_lavaRadius, min(outside, inside));
      outsideHts[posn] = outside;
    }
  }

  addPolygons(insideHts, outsideHts, points, flags);

  // xmin side
  px = 0.0;
  for (int z = -1; z <= m_samples+1; z++)
  {
    pz = z/(double) m_samples;
    for (int y = -1; y <= m_samples+1; y++)
    {
      py = y/(double) m_samples;

      mgPoint3 pt(px-0.5, py-0.5, pz-0.5);
      pt.normalize();

      // find heights of inside and outside edges
      double inside = insideHt(pt);
      double outside = outsideHt(pt);

      int posn = (m_samples+1-z)*depthSize + (y+1);
      points[posn] = pt;
      insideHts[posn] = inside;
      outsideHts[posn] = outside;
    }
  }

  addPolygons(insideHts, outsideHts, points, flags);

  // xmax side
  px = 1.0;
  for (int z = -1; z <= m_samples+1; z++)
  {
    pz = z/(double) m_samples;
    for (int y = -1; y <= m_samples+1; y++)
    {
      py = y/(double) m_samples;

      mgPoint3 pt(px-0.5, py-0.5, pz-0.5);
      pt.normalize();

      // find heights of inside and outside edges
      double inside = insideHt(pt);
      double outside = outsideHt(pt);

      int posn = (z+1)*depthSize + (y+1);
      points[posn] = pt;
      insideHts[posn] = inside;
      outsideHts[posn] = outside;
    }
  }
 
  addPolygons(insideHts, outsideHts, points, flags);

  // zmin side
  pz = 0.0;
  for (int y = -1; y <= m_samples+1; y++)
  {
    py = y/(double) m_samples;
    for (int x = -1; x <= m_samples+1; x++)
    {
      px = x/(double) m_samples;

      mgPoint3 pt(px-0.5, py-0.5, pz-0.5);
      pt.normalize();

      // find heights of inside and outside edges
      double inside = insideHt(pt);
      double outside = outsideHt(pt);

      int posn = (m_samples+1-y)*depthSize + (x+1);
      points[posn] = pt;
      insideHts[posn] = inside;
      outsideHts[posn] = outside;
    }
  }

  addPolygons(insideHts, outsideHts, points, flags);

  // zmax side
  pz = 1.0;
  for (int y = -1; y <= m_samples+1; y++)
  {
    py = y/(double) m_samples;
    for (int x = -1; x <= m_samples+1; x++)
    {
      px = x/(double) m_samples;

      mgPoint3 pt(px-0.5, py-0.5, pz-0.5);
      pt.normalize();

      // find heights of inside and outside edges
      double inside = insideHt(pt);
      double outside = outsideHt(pt);

      int posn = (y+1)*depthSize + (x+1);
      points[posn] = pt;
      insideHts[posn] = inside;
      outsideHts[posn] = outside;
    }
  }

  addPolygons(insideHts, outsideHts, points, flags);

  delete insideHts;
  delete outsideHts;
  delete points;
  delete flags;
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Planet::deleteBuffers()
{
  delete m_outsideIndexes;
  m_outsideIndexes = NULL;
  delete m_outsideVertexes;
  m_outsideVertexes = NULL;

  delete m_insideIndexes;
  m_insideIndexes = NULL;
  delete m_insideVertexes;
  m_insideVertexes = NULL;

  delete m_lavaIndexes;
  m_lavaIndexes = NULL;
  delete m_lavaVertexes;
  m_lavaVertexes = NULL;
}

//-----------------------------------------------------------------------------
// render the ball
void Planet::render()
{
  // set drawing parameters
  mgDisplay->setMatColor(1.0, 1.0, 1.0);
//  mgDisplay->setCulling(false);
  mgDisplay->setTransparent(false);

  mgMatrix4 model;
  mgDisplay->setModelTransform(model);

  // draw triangles using texture and shader
  mgDisplay->setShader("litTexture");
  mgDisplay->setLightColor(1, 1, 1);
  mgDisplay->setTexture(m_outsideTexture);
  mgDisplay->draw(MG_TRIANGLES, m_outsideVertexes, m_outsideIndexes); 

  mgDisplay->setShader("cave");
  mgDisplay->setLightColor(204/255.0, 70/255.0, 7/255.0);
  mgDisplay->setShaderUniform("cave", "lightPosn", m_origin);
  mgDisplay->setTexture(m_insideTexture);
  mgDisplay->draw(MG_TRIANGLES, m_insideVertexes, m_insideIndexes); 

  mgDisplay->setShader("unlitTexture");
  mgDisplay->setLightColor(1, 1, 1);
  mgDisplay->setTexture(m_lavaTexture);
  mgDisplay->draw(MG_TRIANGLES, m_lavaVertexes, m_lavaIndexes); 
}
