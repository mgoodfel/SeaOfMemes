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

#include "SolarSystem.h"
#include "Ring.h"

const int RING_LENGTH_STEPS = 256;

//--------------------------------------------------------------
// constructor
Ring::Ring(
  const mgOptionsFile& options,
  double radius,
  double wallHeight,
  double wallWidth)
{
  mgVertexTA::loadShader("litTextureArray");

  m_radius = radius;
  m_wallHeight = wallHeight;
  m_wallWidth = wallWidth;
  m_ringWidth = (m_radius*2*PI*32)/2048;
  m_ringThick = m_wallWidth;

  mgString ringFront;
  options.getFileName("ring-front", options.m_sourceFileName, "", ringFront);
  mgString ringBack;
  options.getFileName("ring-back", options.m_sourceFileName, "", ringBack);

  mgStringArray filenames;
  filenames.add(ringFront);
  filenames.add(ringBack);
  m_farTexture = mgDisplay->loadTextureArray(filenames);

  m_farVertexes = NULL;
  m_farIndexes = NULL;
}

//--------------------------------------------------------------
// destructor
Ring::~Ring()
{
  deleteBuffers();
}
    
//--------------------------------------------------------------
// create far mode ring surface
void Ring::createSurfaceFar(
  int j)
{
  // get scaled ring dimensions
  double scaleRingWidth = m_ringWidth / SYSTEM_FAR_SCALE;
  double scaleRadius = m_radius / SYSTEM_FAR_SCALE;

  double left = (j+1)/(double) RING_LENGTH_STEPS;
  double right = j/(double) RING_LENGTH_STEPS;

  double langle = 2*PI*left;
  double rangle = 2*PI*right;

  double lx = cos(langle);
  double lz = sin(langle);
  double rx = cos(rangle);
  double rz = sin(rangle);

  // create the surface
  mgVertexTA tl, tr, bl, br;

  // left points
  tl.setNormal(-lx, 0.0, -lz);
  tl.setPoint(scaleRadius * lx, scaleRingWidth * 0.5, scaleRadius * lz);
  tl.setTexture(left, 0.0, 0);   // surface texture

  bl.setNormal(-lx, 0.0, -lz);
  bl.setPoint(scaleRadius * lx, scaleRingWidth * -0.5, scaleRadius * lz);
  bl.setTexture(left, 1.0, 0);  // surface texture

  // right points
  tr.setNormal(-rx, 0.0, -rz);
  tr.setPoint(scaleRadius * rx, scaleRingWidth * 0.5, scaleRadius * rz);
  tr.setTexture(right, 0.0, 0); // surface texture

  br.setNormal(-rx, 0.0, -rz);
  br.setPoint(scaleRadius * rx, scaleRingWidth * -0.5, scaleRadius * rz);
  br.setTexture(right, 1.0, 0); // surface texture

  tl.addTo(m_farVertexes);
  tr.addTo(m_farVertexes);
  bl.addTo(m_farVertexes);
  br.addTo(m_farVertexes);
}

//--------------------------------------------------------------
// create far mode ring shell
void Ring::createShellFar(
  int j)
{
  // get scaled shell dimensions.
  double scaleShellWidth = (m_ringWidth+2*m_wallWidth) / SYSTEM_FAR_SCALE;
  double scaleRadius = (m_radius + m_ringThick) / SYSTEM_FAR_SCALE;

  // shell points outwards, so left and right are reversed
  double left = j/(double) RING_LENGTH_STEPS;
  double right = (j+1)/(double) RING_LENGTH_STEPS;

  double langle = 2*PI*left;
  double rangle = 2*PI*right;

  double lx = cos(langle);
  double lz = sin(langle);
  double rx = cos(rangle);
  double rz = sin(rangle);

  // create the shell
  mgVertexTA tl, tr, bl, br;

  // left points
  tl.setNormal(lx, 0.0, lz);
  tl.setPoint(scaleRadius * lx, scaleShellWidth * 0.5, scaleRadius * lz);
  tl.setTexture(left, 0.0, 1);  // shell texture

  bl.setNormal(lx, 0.0, lz);
  bl.setPoint(scaleRadius * lx, scaleShellWidth * -0.5, scaleRadius * lz);
  bl.setTexture(left, 1.0, 1);  // shell texture

  // right points
  tr.setNormal(rx, 0.0, rz);
  tr.setPoint(scaleRadius * rx, scaleShellWidth * 0.5, scaleRadius * rz);
  tr.setTexture(right, 0.0, 1);  // shell texture

  br.setNormal(rx, 0.0, rz);
  br.setPoint(scaleRadius * rx, scaleShellWidth * -0.5, scaleRadius * rz);
  br.setTexture(right, 1.0, 1);  // shell texture

  tl.addTo(m_farVertexes);
  tr.addTo(m_farVertexes);
  bl.addTo(m_farVertexes);
  br.addTo(m_farVertexes);
}

//--------------------------------------------------------------
// create ring wall max side 
void Ring::createWallMaxFar(
  int j,
  double edgeY)
{
  double outsideRadius = (m_radius + m_ringThick)/SYSTEM_FAR_SCALE;
  double insideRadius = (m_radius + m_ringThick - m_wallHeight)/SYSTEM_FAR_SCALE;
  double totalHeight = (m_ringWidth + m_wallWidth*2)/SYSTEM_FAR_SCALE;
  double wallHeight = m_wallHeight/SYSTEM_FAR_SCALE;
  edgeY = edgeY/SYSTEM_FAR_SCALE;

  double left = j/(double) RING_LENGTH_STEPS;
  double right = (j+1)/(double) RING_LENGTH_STEPS;
  double langle = 2*PI*left;
  double rangle = 2*PI*right;

  double lx = cos(langle);
  double lz = sin(langle);
  double rx = cos(rangle);
  double rz = sin(rangle);

  mgVertexTA tl, tr, bl, br;

  // create max side of wall, left edge
  tl.setNormal(0.0, 1.0, 0.0);
  tl.setPoint(insideRadius * lx, edgeY, insideRadius * lz);
  tl.setTexture(left, 0.0, 1);  // shell texture

  bl.setNormal(0.0, 1.0, 0.0);
  bl.setPoint(outsideRadius * lx, edgeY, outsideRadius * lz);
  bl.setTexture(left, wallHeight / totalHeight, 1); // shell texture

  // create max side of wall, right edge
  tr.setNormal(0.0, 1.0, 0.0);
  tr.setPoint(insideRadius * rx, edgeY, insideRadius * rz);
  tr.setTexture(right, 0.0, 1);  // shell texture

  br.setNormal(0.0, 1.0, 0.0);
  br.setPoint(outsideRadius * rx, edgeY, outsideRadius * rz);
  br.setTexture(right, wallHeight / totalHeight, 1); // shell texture

  tl.addTo(m_farVertexes);
  tr.addTo(m_farVertexes);
  bl.addTo(m_farVertexes);
  br.addTo(m_farVertexes);
}

//--------------------------------------------------------------
// create ring wall top
void Ring::createWallTopFar(
  int j,
  double edgeY)
{
  double outsideRadius = (m_radius + m_ringThick)/SYSTEM_FAR_SCALE;
  double insideRadius = (m_radius + m_ringThick - m_wallHeight)/SYSTEM_FAR_SCALE;
  double totalHeight = (m_ringWidth + m_wallWidth*2)/SYSTEM_FAR_SCALE;
  double wallWidth = m_wallWidth/SYSTEM_FAR_SCALE;
  double wallHeight = m_wallHeight/SYSTEM_FAR_SCALE;

  double scaleTopY = (edgeY - m_wallWidth) / SYSTEM_FAR_SCALE;
  double scaleBotY = edgeY / SYSTEM_FAR_SCALE;

  double left = j/(double) RING_LENGTH_STEPS;
  double right = (j+1)/(double) RING_LENGTH_STEPS;
  double langle = 2*PI*left;
  double rangle = 2*PI*right;

  double lx = cos(langle);
  double lz = sin(langle);
  double rx = cos(rangle);
  double rz = sin(rangle);

  mgVertexTA tl, tr, bl, br;

  // create top of wall, left edge
  tl.setNormal(-lx, 0.0, -lz);
  tl.setPoint(insideRadius * lx, scaleTopY, insideRadius * lz);
  tl.setTexture(left, 0.0, 1); // shell texture

  bl.setNormal(-lx, 0.0, -lz);
  bl.setPoint(insideRadius * lx, scaleBotY, insideRadius * lz);
  bl.setTexture(left, wallWidth / totalHeight, 1); // shell texture

  // create top of wall, right edge
  tr.setNormal(-rx, 0.0, -rz);
  tr.setPoint(insideRadius * rx, scaleTopY, insideRadius * rz);
  tr.setTexture(right, 0.0, 1); // shell texture

  br.setNormal(-rx, 0.0, -rz);
  br.setPoint(insideRadius * rx, scaleBotY, insideRadius * rz);
  br.setTexture(right, wallWidth / totalHeight, 1); // shell texture

  tl.addTo(m_farVertexes);
  tr.addTo(m_farVertexes);
  bl.addTo(m_farVertexes);
  br.addTo(m_farVertexes);
}

//--------------------------------------------------------------
// create ring wall min side
void Ring::createWallMinFar(
  int j,
  double edgeY)
{
  double outsideRadius = (m_radius + m_ringThick)/SYSTEM_FAR_SCALE;
  double insideRadius = (m_radius + m_ringThick - m_wallHeight)/SYSTEM_FAR_SCALE;
  double totalHeight = (m_ringWidth + m_wallWidth*2)/SYSTEM_FAR_SCALE;
  double wallHeight = m_wallHeight/SYSTEM_FAR_SCALE;
  edgeY = (edgeY - m_wallWidth) / SYSTEM_FAR_SCALE;

  double left = j/(double) RING_LENGTH_STEPS;
  double right = (j+1)/(double) RING_LENGTH_STEPS;
  double langle = 2*PI*left;
  double rangle = 2*PI*right;

  double lx = cos(langle);
  double lz = sin(langle);
  double rx = cos(rangle);
  double rz = sin(rangle);

  mgVertexTA tl, tr, bl, br;

  // create min side of wall, left edge
  tl.setNormal(0.0, -1.0, 0.0);
  tl.setPoint(outsideRadius * lx, edgeY, outsideRadius * lz);
  tl.setTexture(left, 0.0, 1);  // shell texture

  bl.setNormal(0.0, -1.0, 0.0);
  bl.setPoint(insideRadius * lx, edgeY, insideRadius * lz);
  bl.setTexture(left, wallHeight / totalHeight, 1); // shell texture

  // create min side of wall, right edge
  tr.setNormal(0.0, -1.0, 0.0);
  tr.setPoint(outsideRadius * rx, edgeY, outsideRadius * rz);
  tr.setTexture(right, 0.0, 1);  // shell texture

  br.setNormal(0.0, -1.0, 0.0);
  br.setPoint(insideRadius * rx, edgeY, insideRadius * rz);
  br.setTexture(right, wallHeight / totalHeight, 1); // shell texture

  tl.addTo(m_farVertexes);
  tr.addTo(m_farVertexes);
  bl.addTo(m_farVertexes);
  br.addTo(m_farVertexes);
}

//--------------------------------------------------------------
// delete display buffers
void Ring::deleteBuffers()
{
  delete m_farVertexes;
  m_farVertexes = NULL;

  delete m_farIndexes;
  m_farIndexes = NULL;
}

//--------------------------------------------------------------
// animate object
BOOL Ring::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// create buffers ready to send to display
void Ring::createBuffers()
{
  m_shader = mgVertexTA::loadShader("litTextureArray");

  int ringSegments = 8;
  m_farVertexes = mgVertexTA::newBuffer(ringSegments*4*RING_LENGTH_STEPS);
  m_farIndexes = mgDisplay->newIndexBuffer(ringSegments*6*RING_LENGTH_STEPS);

  // create the max wall
  for (int j = 0; j < RING_LENGTH_STEPS; j++)
  {
    double edgeY = m_ringWidth/2 + m_wallWidth;

    m_farIndexes->addRectIndex(m_farVertexes->getLength());
    createWallMaxFar(j, edgeY);

    m_farIndexes->addRectIndex(m_farVertexes->getLength());
    createWallTopFar(j, edgeY);

    m_farIndexes->addRectIndex(m_farVertexes->getLength());
    createWallMinFar(j, edgeY);

    // create the ring surface and shell
    m_farIndexes->addRectIndex(m_farVertexes->getLength());
    createSurfaceFar(j);

    m_farIndexes->addRectIndex(m_farVertexes->getLength());
    createShellFar(j);

    edgeY = -m_ringWidth/2;
    m_farIndexes->addRectIndex(m_farVertexes->getLength());
    createWallMaxFar(j, edgeY);

    m_farIndexes->addRectIndex(m_farVertexes->getLength());
    createWallTopFar(j, edgeY);

    m_farIndexes->addRectIndex(m_farVertexes->getLength());
    createWallMinFar(j, edgeY);
  }
}

//--------------------------------------------------------------
// draw back (away from eye) portion of ring
void Ring::renderBackFar()
{
  mgDisplay->setShader(m_shader);
  mgDisplay->setTexture(m_farTexture);

  mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farIndexes);
}

//--------------------------------------------------------------
// draw back (towards from eye) portion of ring
void Ring::renderFrontFar()
{
}

//--------------------------------------------------------------
// render medium version
void Ring::renderMedium()
{
}

//--------------------------------------------------------------
// render near version
void Ring::renderNear()
{
}

#ifdef WORKED
//--------------------------------------------------------------
// create buffers ready to send to display
void Ring::createBuffers()
{
  int ringSegments = 8;
  int vertexCount = ringSegments*4*RING_LENGTH_STEPS;

  m_farVertexes = mgVertexTA::newBuffer(vertexCount);
  m_farMaxWallIndexes = mgDisplay->newIndexBuffer(3*6*RING_LENGTH_STEPS);
  m_farMinWallIndexes = mgDisplay->newIndexBuffer(3*6*RING_LENGTH_STEPS);
  m_farRingIndexes = mgDisplay->newIndexBuffer(2*6*RING_LENGTH_STEPS);

  // create the max wall
  for (int j = 0; j < RING_LENGTH_STEPS; j++)
  {
    double edgeY = m_ringWidth/2 + m_wallWidth;

    m_farMaxWallIndexes->addRectIndex(m_farVertexes->getLength());
    createWallMaxFar(j, edgeY);

    m_farMaxWallIndexes->addRectIndex(m_farVertexes->getLength());
    createWallTopFar(j, edgeY);

    m_farMaxWallIndexes->addRectIndex(m_farVertexes->getLength());
    createWallMinFar(j, edgeY);
  }

  // create the surface and shell
  for (int j = 0; j < RING_LENGTH_STEPS; j++)
  {
    // create the ring surface and shell
    m_farRingIndexes->addRectIndex(m_farVertexes->getLength());
    createSurfaceFar(j);

    m_farRingIndexes->addRectIndex(m_farVertexes->getLength());
    createShellFar(j);
  }

  // create the min wall
  for (int j = 0; j < RING_LENGTH_STEPS; j++)
  {
    double edgeY = -m_ringWidth/2;
    m_farMinWallIndexes->addRectIndex(m_farVertexes->getLength());
    createWallMaxFar(j, edgeY);

    m_farMinWallIndexes->addRectIndex(m_farVertexes->getLength());
    createWallTopFar(j, edgeY);

    m_farMinWallIndexes->addRectIndex(m_farVertexes->getLength());
    createWallMinFar(j, edgeY);
  }
}

//--------------------------------------------------------------
// draw back (away from eye) portion of ring
void Ring::renderBackFar()
{
return;
  mgDisplay->setShader("litTextureArray");
  mgDisplay->setTexture(m_farTexture);

  // get the inverse transform matrix
  mgMatrix4 transform;
  mgDisplay->getModelTransform(transform);
  mgMatrix4 invtrans;
  invtrans._11 = transform._11;
  invtrans._12 = transform._21;
  invtrans._13 = transform._31;
  invtrans._21 = transform._12;
  invtrans._22 = transform._22;
  invtrans._23 = transform._32;
  invtrans._31 = transform._13;
  invtrans._32 = transform._23;
  invtrans._33 = transform._33;

  // get height of eye in local coordinates
  mgPoint3 eyePt;
  mgDisplay->getEyePt(eyePt);
  eyePt.x -= transform._41;
  eyePt.y -= transform._42;
  eyePt.z -= transform._43;
  double ex, ey, ez;
  invtrans.mapPt(eyePt.x, eyePt.y, eyePt.z, ex, ey, ez);

  // get the index of the nearest segment to the eye
  double dist = sqrt(ex*ex+ez*ez);
  double angle = asin(ez/dist);
  if (ex < 0)
    angle = PI - angle;
  int posn = (int) floor(RING_LENGTH_STEPS*angle/(2*PI));

  // get indexes of back arc
  int start = posn + RING_LENGTH_STEPS/4;
  int end = start + RING_LENGTH_STEPS/2;
  start = (start+RING_LENGTH_STEPS)%RING_LENGTH_STEPS;
  end = (end+RING_LENGTH_STEPS)%RING_LENGTH_STEPS;

  if (start < end)
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes, start*2*6, end*2*6);
  else 
  {
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes, start*2*6, RING_LENGTH_STEPS*2*6);
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes, 0, end*2*6);
  }

/*
  // paint elements in sorted order
  if (ey > m_ringWidth/2)
  {
    // above ring, paint min wall first
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farMinWallIndexes);
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes);
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farMaxWallIndexes);
  }
  else if (ey < -m_ringWidth/2)
  {
    // below ring, paint max wall first
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farMaxWallIndexes);
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes);
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farMinWallIndexes);
  }
  else
  {
    // within ring, paint surface, then walls
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes);
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farMaxWallIndexes);
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farMinWallIndexes);
  }
*/
}

//--------------------------------------------------------------
// draw front (towards eye) portion of ring
void Ring::renderFrontFar()
{
  mgDisplay->setShader("litTextureArray");
  mgDisplay->setTexture(m_farTexture);

  // get the inverse transform matrix
  mgMatrix4 transform;
  mgDisplay->getModelTransform(transform);
  mgMatrix4 invtrans;
  invtrans._11 = transform._11;
  invtrans._12 = transform._21;
  invtrans._13 = transform._31;
  invtrans._21 = transform._12;
  invtrans._22 = transform._22;
  invtrans._23 = transform._32;
  invtrans._31 = transform._13;
  invtrans._32 = transform._23;
  invtrans._33 = transform._33;

  // get height of eye in local coordinates
  mgPoint3 eyePt;
  mgDisplay->getEyePt(eyePt);
  eyePt.x -= transform._41;
  eyePt.y -= transform._42;
  eyePt.z -= transform._43;
  double ex, ey, ez;
  invtrans.mapPt(eyePt.x, eyePt.y, eyePt.z, ex, ey, ez);

  // get the index of the nearest segment to the eye
  double dist = sqrt(ex*ex+ez*ez);
  double angle = asin(ez/dist);
  if (ex < 0)
    angle = PI - angle;
  int posn = (int) floor(RING_LENGTH_STEPS*angle/(2*PI));

  // get indexes of front arc
  int start = posn - RING_LENGTH_STEPS/4;
  int end = start + RING_LENGTH_STEPS/2;
  start = (start+RING_LENGTH_STEPS)%RING_LENGTH_STEPS;
  end = (end+RING_LENGTH_STEPS)%RING_LENGTH_STEPS;

  if (start < end)
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes, start*2*6, end*2*6);
  else 
  {
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes, start*2*6, RING_LENGTH_STEPS*2*6);
    mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farRingIndexes, 0, end*2*6);
  }
}

#endif
