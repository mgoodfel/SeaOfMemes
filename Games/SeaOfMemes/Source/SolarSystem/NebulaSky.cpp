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

#include "NebulaSky.h"

//--------------------------------------------------------------
// constructor
NebulaSky::NebulaSky(
  const mgOptionsFile& options)
{
  // load shaders
  m_starShader = mgVertex::loadShader("unlitTexture");
  m_sunShader = mgVertex::loadShader("unlitTexture");
  m_skyBoxShader = mgVertexTA::loadShader("unlitTextureCube");

  mgString fileName;

  m_skyBoxTexture = NULL;
  m_skyBoxTriangles = NULL;

  m_starTexture = NULL;
  m_starTriangles = NULL;

  m_sunTexture = NULL;
  m_sunTriangles = NULL;

  options.getFileName("starTexture", options.m_sourceFileName, "", fileName);
  if (!fileName.isEmpty())
    m_starTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("sunTexture", options.m_sourceFileName, "", fileName);
  if (!fileName.isEmpty())
    m_sunTexture = mgDisplay->loadTexture(fileName);

  mgString xminSky, xmaxSky, yminSky, ymaxSky, zminSky, zmaxSky; 
  options.getFileName("sky-xmin", options.m_sourceFileName, "Nebula-xmin.jpg", xminSky);
  options.getFileName("sky-xmax", options.m_sourceFileName, "Nebula-xmax.jpg", xmaxSky);
  options.getFileName("sky-ymin", options.m_sourceFileName, "Nebula-ymin.jpg", yminSky);
  options.getFileName("sky-ymax", options.m_sourceFileName, "Nebula-ymax.jpg", ymaxSky);
  options.getFileName("sky-zmin", options.m_sourceFileName, "Nebula-zmin.jpg", zminSky);
  options.getFileName("sky-zmax", options.m_sourceFileName, "Nebula-zmax.jpg", zmaxSky);
  setSkyBox(xminSky, xmaxSky, yminSky, ymaxSky, zminSky, zmaxSky);

  m_skyDist = 1000.0f;
  m_sunDir = mgPoint3(1, 0, 0);
}

//--------------------------------------------------------------
// destructor
NebulaSky::~NebulaSky()
{
  deleteBuffers();
}

//--------------------------------------------------------------
// set the sky box
void NebulaSky::setSkyBox(
  const char* xminName,
  const char* xmaxName,
  const char* yminName,
  const char* ymaxName,
  const char* zminName,
  const char* zmaxName)
{
  m_skyBoxTexture = mgDisplay->loadTextureCube(xminName, xmaxName, 
                        yminName, ymaxName, zminName, zmaxName);
}

//--------------------------------------------------------------
// animate object
BOOL NebulaSky::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// render in scene
void NebulaSky::render()
{
  mgDisplay->setTransparent(true);
  mgDisplay->setZEnable(false);

  mgMatrix4 xform;
  xform.scale(500.0, 500.0, 500.0);
  mgDisplay->setModelTransform(xform);

  // draw the skybox
  if (m_skyBoxTexture != NULL && m_skyBoxTriangles != NULL)
  {
    mgDisplay->setMatColor(0.5, 0.5, 0.5);
    mgDisplay->setShader(m_skyBoxShader);
    mgDisplay->setTexture(m_skyBoxTexture);
    mgDisplay->draw(MG_TRIANGLES, m_skyBoxTriangles);
  }

  // draw the stars                           
  if (m_starTexture != NULL && m_starTriangles != NULL)
  {
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setShader(m_starShader);
    mgDisplay->setTexture(m_starTexture);
    mgDisplay->draw(MG_TRIANGLES, m_starTriangles);
  }

  // draw the sun
  if (m_sunTexture != NULL && m_sunTriangles != NULL)
  {
    mgPoint3 taxis(m_sunDir.z, 0, -m_sunDir.x);
    mgPoint3 yaxis(m_sunDir);
    yaxis.cross(taxis);
    yaxis.normalize();
    mgPoint3 xaxis(yaxis);
    xaxis.cross(m_sunDir);
    xaxis.normalize();

    mgMatrix4 dir;
    dir._11 = xaxis.x; dir._12 = xaxis.y; dir._13 = xaxis.z; 
    dir._21 = yaxis.x; dir._22 = yaxis.y; dir._23 = yaxis.z; 
    dir._31 = m_sunDir.x; dir._32 = m_sunDir.y; dir._33 = m_sunDir.z; 
    mgDisplay->appendModelTransform(dir);

    mgDisplay->setShader(m_sunShader);
    mgDisplay->setTexture(m_sunTexture);
    mgDisplay->draw(MG_TRIANGLES, m_sunTriangles);

    mgDisplay->setModelTransform(xform);
  }

  mgDisplay->setTransparent(false);
  mgDisplay->setZEnable(true);
}

//--------------------------------------------------------------
// create the skybox triangles
void NebulaSky::createSkyBox()
{
  const int SKY_STRIPS = 5;
  const int SKY_FACES = 10;

  // three points times two triangles times six faces
  m_skyBoxTriangles = mgVertexTA::newBuffer(3*2*SKY_STRIPS * SKY_FACES);

  mgVertexTA tl, tr, bl, br;
  mgPoint3 normal;

  // create a sphere
  for (int i = 1; i <= SKY_STRIPS; i++)
  {
    double topAngle = (PI*(i-1))/SKY_STRIPS;
    double topRadius = sin(topAngle);
    double topY = cos(topAngle);
                                    
    double botAngle = (PI*i)/SKY_STRIPS;
    double botRadius = sin(botAngle);
    double botY = cos(botAngle);
                                    
    for (int j = 1; j <= SKY_FACES; j++)
    {
      double leftAngle = (2.0*PI*(j-1))/SKY_FACES;
      tl.setPoint(topRadius * cos(leftAngle), topY, topRadius * sin(leftAngle));
      bl.setPoint(botRadius * cos(leftAngle), botY, botRadius * sin(leftAngle));

      double rightAngle = (2.0*PI*j)/SKY_FACES;
      tr.setPoint(topRadius * cos(rightAngle), topY, topRadius * sin(rightAngle));
      br.setPoint(botRadius * cos(rightAngle), botY, botRadius * sin(rightAngle));

      normal = mgPoint3(tl.m_px, tl.m_py, tl.m_pz);
      normal.normalize();
      tl.setTexture(normal.x, normal.y, normal.z);
      tl.setNormal(-normal.x, -normal.y, -normal.z);

      normal = mgPoint3(tr.m_px, tr.m_py, tr.m_pz);
      normal.normalize();
      tr.setTexture(normal.x, normal.y, normal.z);
      tr.setNormal(-normal.x, -normal.y, -normal.z);

      normal = mgPoint3(bl.m_px, bl.m_py, bl.m_pz);
      normal.normalize();
      bl.setTexture(normal.x, normal.y, normal.z);
      bl.setNormal(-normal.x, -normal.y, -normal.z);

      normal = mgPoint3(br.m_px, br.m_py, br.m_pz);
      normal.normalize();
      br.setTexture(normal.x, normal.y, normal.z);
      br.setNormal(-normal.x, -normal.y, -normal.z);

      tl.addTo(m_skyBoxTriangles);
      bl.addTo(m_skyBoxTriangles);
      tr.addTo(m_skyBoxTriangles);

      tr.addTo(m_skyBoxTriangles);
      bl.addTo(m_skyBoxTriangles);
      br.addTo(m_skyBoxTriangles);
    }
  }
}

//--------------------------------------------------------------
// add a star patch
void NebulaSky::addStarPatch(
  const mgPoint3& pt)
{
  const double PATCH_SIZE = 0.25;
  const double JITTER = 0.1;

  // jitter the center a bit
  mgPoint3 center(pt);
  center.x += JITTER*(-0.5 + mgRandom());
  center.y += JITTER*(-0.5 + mgRandom());
  center.z += JITTER*(-0.5 + mgRandom());
  center.normalize();

  // get another point as temporary x axis
  mgPoint3 taxis;
  taxis.x = -0.5 + mgRandom();
  taxis.y = -0.5 + mgRandom();
  taxis.z = -0.5 + mgRandom();

  // randomly flip the axis to cut down on visible repetition    
  BOOL xflip = 0.5 > mgRandom();
  BOOL yflip = 0.5 > mgRandom();

  // cross with eye to get two axis
  mgPoint3 yaxis(center);
  yaxis.cross(taxis);
  yaxis.normalize();
  mgPoint3 xaxis(yaxis);
  xaxis.cross(center);
  xaxis.normalize();

  xaxis.scale(PATCH_SIZE);
  yaxis.scale(PATCH_SIZE);

  mgPoint3 tlpt(center);
  tlpt.subtract(xaxis);
  tlpt.add(yaxis);

  mgPoint3 trpt(center);
  trpt.add(xaxis);
  trpt.add(yaxis);

  mgPoint3 blpt(center);
  blpt.subtract(xaxis);
  blpt.subtract(yaxis);

  mgPoint3 brpt(center);
  brpt.add(xaxis);
  brpt.subtract(yaxis);

  mgPoint3 normal(center);
  normal.normalize();
  normal.scale(-1);

  mgVertex tl, tr, bl, br;

  tl.setPoint(tlpt.x, tlpt.y, tlpt.z);
  tl.setNormal(normal.x, normal.y, normal.z);
  tl.setTexture(xflip ? 1.0 : 0.0, yflip ? 1.0 : 0.0);

  tr.setPoint(trpt.x, trpt.y, trpt.z);
  tr.setNormal(normal.x, normal.y, normal.z);
  tr.setTexture(xflip ? 0.0 : 1.0, yflip ? 1.0 : 0.0);

  bl.setPoint(blpt.x, blpt.y, blpt.z);
  bl.setNormal(normal.x, normal.y, normal.z);
  bl.setTexture(xflip ? 1.0 : 0.0, yflip ? 0.0 : 1.0);

  br.setPoint(brpt.x, brpt.y, brpt.z);
  br.setNormal(normal.x, normal.y, normal.z);
  br.setTexture(xflip ? 0.0 : 1.0, yflip ? 0.0 : 1.0);

  tl.addTo(m_starTriangles);
  tr.addTo(m_starTriangles);
  bl.addTo(m_starTriangles);

  bl.addTo(m_starTriangles);
  tr.addTo(m_starTriangles);
  br.addTo(m_starTriangles);
}

//--------------------------------------------------------------
// create the star rectangles.  
void NebulaSky::createStars()
{
  if (m_starTriangles != NULL)
    return;

  int steps = 5;

  // three points times two triangles times six sides times step*step points
  m_starTriangles = mgVertex::newBuffer(3*2*6*steps*steps);

  mgPoint3 ray;

  // zmin face
  for (int i = 0; i < steps; i++)
  {
    for (int j = 0; j < steps; j++)
    {
      // get the vector from the center
      ray.z = -0.5;
      ray.y = 0.5 - (i/(double) steps);
      ray.x = 0.5 - (j/(double) steps);
      ray.normalize();

      addStarPatch(ray);
    }
  }

  // xmin face
  for (int i = 0; i < steps; i++)
  {
    for (int j = 0; j < steps; j++)
    {
      // get the vector from the center
      ray.x = -0.5;
      ray.y = 0.5 - (i/(double) steps);
      ray.z = (j/(double) steps) - 0.5;
      ray.normalize();

      addStarPatch(ray);
    }
  }

  // zmax face
  for (int i = 0; i < steps; i++)
  {
    for (int j = 0; j < steps; j++)
    {
      // get the vector from the center
      ray.z = 0.5;
      ray.y = 0.5 - (i/(double) steps);
      ray.x = (j/(double) steps) - 0.5;
      ray.normalize();

      addStarPatch(ray);
    }
  }

  // xmax face
  for (int i = 0; i < steps; i++)
  {
    for (int j = 0; j < steps; j++)
    {
      // get the vector from the center
      ray.x = 0.5;
      ray.y = 0.5 - (i/(double) steps);
      ray.z = 0.5 - (j/(double) steps);
      ray.normalize();

      addStarPatch(ray);
    }
  }

  // ymin face
  for (int i = 1; i < steps; i++)
  {
    for (int j = 1; j < steps; j++)
    {
      // get the vector from the center
      ray.y = -0.5;
      ray.z = 0.5 - (i/(double) steps);
      ray.x = (j/(double) steps) - 0.5;
      ray.normalize();

      addStarPatch(ray);
    }
  }

  // ymax face
  for (int i = 1; i < steps; i++)
  {
    for (int j = 1; j < steps; j++)
    {
      // get the vector from the center
      ray.y = 0.5;
      ray.z = (i/(double) steps) - 0.5;
      ray.x = (j/(double) steps) - 0.5;
      ray.normalize();

      addStarPatch(ray);
    }
  }
}                             

//--------------------------------------------------------------
// create the sun rectangles.  
void NebulaSky::createSun()
{
  mgVertex tl, tr, bl, br;

  // three points times two triangles
  m_sunTriangles = mgVertex::newBuffer(3*2);

  mgPoint3 sunPoint(0, 0, 1);
  mgPoint3 xaxis(1, 0, 0);
  mgPoint3 yaxis(0, 1, 0);

  double hsize = 0.15; // 0.3

  tl.setNormal(-sunPoint.x, -sunPoint.y, -sunPoint.z);
  tl.setPoint(sunPoint.x - hsize*xaxis.x + hsize*yaxis.x, 
              sunPoint.y - hsize*xaxis.y + hsize*yaxis.y, 
              sunPoint.z - hsize*xaxis.z + hsize*yaxis.z);
  tl.setTexture(0.0, 0.0);

  tr.setNormal(-sunPoint.x, -sunPoint.y, -sunPoint.z);
  tr.setPoint(sunPoint.x + hsize*xaxis.x + hsize*yaxis.x, 
              sunPoint.y + hsize*xaxis.y + hsize*yaxis.y, 
              sunPoint.z + hsize*xaxis.z + hsize*yaxis.z);
  tr.setTexture(1.0, 0.0);

  bl.setNormal(-sunPoint.x, -sunPoint.y, -sunPoint.z);
  bl.setPoint(sunPoint.x - hsize*xaxis.x - hsize*yaxis.x, 
              sunPoint.y - hsize*xaxis.y - hsize*yaxis.y, 
              sunPoint.z - hsize*xaxis.z - hsize*yaxis.z);
  bl.setTexture(0.0, 1.0);

  br.setNormal(-sunPoint.x, -sunPoint.y, -sunPoint.z);
  br.setPoint(sunPoint.x + hsize*xaxis.x - hsize*yaxis.x, 
              sunPoint.y + hsize*xaxis.y - hsize*yaxis.y, 
              sunPoint.z + hsize*xaxis.z - hsize*yaxis.z);
  br.setTexture(1.0, 1.0);

  tl.addTo(m_sunTriangles);
  tr.addTo(m_sunTriangles);
  bl.addTo(m_sunTriangles);

  bl.addTo(m_sunTriangles);
  tr.addTo(m_sunTriangles);
  br.addTo(m_sunTriangles);
}

//--------------------------------------------------------------
// delete display buffers
void NebulaSky::deleteBuffers()
{
  delete m_skyBoxTriangles;
  m_skyBoxTriangles = NULL;

  delete m_starTriangles;
  m_starTriangles = NULL;

  delete m_sunTriangles;
  m_sunTriangles = NULL;
}

//--------------------------------------------------------------
// create buffers ready to send to display
void NebulaSky::createBuffers()
{
  if (m_skyBoxTexture != NULL && m_skyBoxTriangles == NULL)
    createSkyBox();

  if (m_starTexture != NULL && m_starTriangles == NULL)
    createStars();

  if (m_sunTexture != NULL && m_sunTriangles == NULL)
    createSun();
}

