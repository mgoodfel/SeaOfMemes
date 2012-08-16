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

#include "StarrySky.h"
#include "VertexSky.h"

//--------------------------------------------------------------
// constructor
StarrySky::StarrySky(
  const mgOptionsFile& options)
{
  // load shaders
  mgVertex::loadShader("unlitTexture");
  mgVertexTA::loadShader("unlitTextureCube");
  VertexSky::loadShader("skyfog");

  mgString fileName;

  m_skyBoxTexture = NULL;
  m_skyBoxTriangles = NULL;

  m_starTexture = NULL;
  m_starTriangles = NULL;

  m_moonTexture = NULL;
  m_moonTriangles = NULL;

  m_sunTexture = NULL;
  m_sunTriangles = NULL;

  m_fogVertexes = NULL;
  m_fogIndexes = NULL;

  mgString xminName, xmaxName, yminName, ymaxName, zminName, zmaxName;
  options.getFileName("skybox-xmin", options.m_sourceFileName, "", xminName);
  options.getFileName("skybox-xmax", options.m_sourceFileName, "", xmaxName);
  options.getFileName("skybox-ymin", options.m_sourceFileName, "", yminName);
  options.getFileName("skybox-ymax", options.m_sourceFileName, "", ymaxName);
  options.getFileName("skybox-zmin", options.m_sourceFileName, "", zminName);
  options.getFileName("skybox-zmax", options.m_sourceFileName, "", zmaxName);

  if (!xminName.isEmpty() && !xmaxName.isEmpty() && 
      !yminName.isEmpty() && !ymaxName.isEmpty() && 
      !zminName.isEmpty() && !zmaxName.isEmpty())
  {
    m_skyBoxTexture = mgDisplay->loadTextureCube(xminName, xmaxName, 
                         yminName, ymaxName, zminName, zmaxName);
  }

  options.getFileName("starTexture", options.m_sourceFileName, "", fileName);
  if (!fileName.isEmpty())
    m_starTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("moonTexture", options.m_sourceFileName, "", fileName);
  if (!fileName.isEmpty())
    m_moonTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("sunTexture", options.m_sourceFileName, "", fileName);
  if (!fileName.isEmpty())
    m_sunTexture = mgDisplay->loadTexture(fileName);

  setSkyAngle(0.0);
  setMoonDir(mgPoint3(0, 0, 1));
  setSunDir(mgPoint3(0, 0, -1));
  m_skyDist = 1000.0f;
  m_fogMaxDist = 100000.0f;
  m_fogBotHeight = 0.0f;
  m_fogBotInten = 1.0f;
  m_fogTopHeight = 10000.0f;
  m_fogTopInten = 1.0f;
  m_fogColor = mgPoint4(0.8, 0.8, 0.8, 1.0);

  m_enableStars = false;
  m_enableSkyBox = false;
  m_enableMoon = false;
  m_enableSun = false;
  m_enableFog = false;
}

//--------------------------------------------------------------
// destructor
StarrySky::~StarrySky()
{
  deleteBuffers();
}

//--------------------------------------------------------------
// set moon direction
void StarrySky::setMoonDir(
  const mgPoint3& dir)
{
  m_moonDir = dir;
  m_moonDir.normalize();
}

//--------------------------------------------------------------
// set sun direction
void StarrySky::setSunDir(
  const mgPoint3& dir)
{
  m_sunDir = dir;
  m_sunDir.normalize();
}

//--------------------------------------------------------------
// set sky angle
void StarrySky::setSkyAngle(
  double angle)
{
  m_skyAngle = angle;
}

//--------------------------------------------------------------
// set the sky box
void StarrySky::setSkyBox(
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
// animate the sky
BOOL StarrySky::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// render in scene
void StarrySky::render()
{
  mgDisplay->setTransparent(true);
  mgDisplay->setZEnable(false);

  mgMatrix4 xform;
  xform.rotateXDeg(m_skyAngle);
  xform.scale(500.0, 500.0, 500.0);
  mgDisplay->setModelTransform(xform);

  // draw the skybox
  if (m_enableSkyBox && m_skyBoxTexture != NULL && m_skyBoxTriangles != NULL)
  {
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setShader("unlitTextureCube");
    mgDisplay->setTexture(m_skyBoxTexture);
    mgDisplay->draw(MG_TRIANGLES, m_skyBoxTriangles);
  }

  // draw the stars                           
  if (m_enableStars && m_starTexture != NULL && m_starTriangles != NULL)
  {
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setShader("unlitTexture");
    mgDisplay->setTexture(m_starTexture);
    mgDisplay->draw(MG_TRIANGLES, m_starTriangles);
  }

  // draw the moon
  if (m_enableMoon && m_moonTexture != NULL && m_moonTriangles != NULL)
  {
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgPoint3 taxis(m_moonDir.z, 0, -m_moonDir.x);
    mgPoint3 yaxis(m_moonDir);
    yaxis.cross(taxis);
    yaxis.normalize();
    mgPoint3 xaxis(yaxis);
    xaxis.cross(m_moonDir);
    xaxis.normalize();

    mgMatrix4 dir;
    dir._11 = xaxis.x; dir._12 = xaxis.y; dir._13 = xaxis.z; 
    dir._21 = yaxis.x; dir._22 = yaxis.y; dir._23 = yaxis.z; 
    dir._31 = m_moonDir.x; dir._32 = m_moonDir.y; dir._33 = m_moonDir.z; 
    mgDisplay->appendModelTransform(dir);

    mgDisplay->setShader("unlitTexture");
    mgDisplay->setTexture(m_moonTexture);
    mgDisplay->draw(MG_TRIANGLES, m_moonTriangles);

    mgDisplay->setModelTransform(xform);
  }

  // draw the sun
  if (m_enableSun && m_sunTexture != NULL && m_sunTriangles != NULL)
  {
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
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

    mgDisplay->setShader("unlitTexture");
    mgDisplay->setTexture(m_sunTexture);
    mgDisplay->draw(MG_TRIANGLES, m_sunTriangles);

    mgDisplay->setModelTransform(xform);
  }

  // draw the fog
  if (m_enableFog && m_fogVertexes != NULL)
  {
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setShader("skyfog");
    mgDisplay->setShaderUniform("skyfog", "fogColor", m_fogColor);
    mgDisplay->setShaderUniform("skyfog", "fogBotHeight", (float) (m_fogBotHeight/m_skyDist));
    mgDisplay->setShaderUniform("skyfog", "fogBotInten", (float) m_fogBotInten);
    mgDisplay->setShaderUniform("skyfog", "fogTopHeight", (float) (m_fogTopHeight/m_skyDist));
    mgDisplay->setShaderUniform("skyfog", "fogTopInten", (float) m_fogTopInten);
    mgDisplay->setShaderUniform("skyfog", "fogMaxDist", (float) (m_fogMaxDist/m_skyDist));
    mgDisplay->draw(MG_TRIANGLES, m_fogVertexes, m_fogIndexes);
  }

  mgDisplay->setTransparent(false);
  mgDisplay->setZEnable(true);
}

//--------------------------------------------------------------
// create the skybox triangles
void StarrySky::createSkyBox()
{
  // three points times two triangles times six faces
  m_skyBoxTriangles = mgVertexTA::newBuffer(3*2*6);

  mgVertexTA tl, tr, bl, br;

  // draw xmin
  tl.setNormal( 1, 0, 0);
  tr.setNormal( 1, 0, 0);
  bl.setNormal( 1, 0, 0);
  br.setNormal( 1, 0, 0);

  tl.setPoint(-1,  1, -1);      
  tr.setPoint(-1,  1,  1); 
  bl.setPoint(-1, -1, -1);
  br.setPoint(-1, -1,  1); 

  tl.setTexture(-1,  1, -1);      
  tr.setTexture(-1,  1,  1); 
  bl.setTexture(-1, -1, -1);
  br.setTexture(-1, -1,  1); 

  tl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  bl.addTo(m_skyBoxTriangles);

  bl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  br.addTo(m_skyBoxTriangles);

  // draw xmax
  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint( 1,  1,  1);      
  tr.setPoint( 1,  1, -1); 
  bl.setPoint( 1, -1,  1);      
  br.setPoint( 1, -1, -1); 

  tl.setTexture( 1,  1,  1);      
  tr.setTexture( 1,  1, -1); 
  bl.setTexture( 1, -1,  1);      
  br.setTexture( 1, -1, -1); 

  tl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  bl.addTo(m_skyBoxTriangles);

  bl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  br.addTo(m_skyBoxTriangles);

  // ymin
  tl.setNormal(0,  1, 0);
  tr.setNormal(0,  1, 0);
  bl.setNormal(0,  1, 0);
  br.setNormal(0,  1, 0);

  tl.setPoint( 1, -1, -1); 
  tr.setPoint(-1, -1, -1); 
  bl.setPoint( 1, -1,  1);
  br.setPoint(-1, -1,  1);

  tl.setTexture( 1, -1, -1); 
  tr.setTexture(-1, -1, -1); 
  bl.setTexture( 1, -1,  1);
  br.setTexture(-1, -1,  1);

  tl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  bl.addTo(m_skyBoxTriangles);

  bl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  br.addTo(m_skyBoxTriangles);

  // ymax
  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  tl.setPoint( 1,  1,  1);        
  tr.setPoint(-1,  1,  1);        
  bl.setPoint( 1,  1, -1);             
  br.setPoint(-1,  1, -1);

  tl.setTexture( 1,  1,  1);        
  tr.setTexture(-1,  1,  1);        
  bl.setTexture( 1,  1, -1);             
  br.setTexture(-1,  1, -1);

  tl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  bl.addTo(m_skyBoxTriangles);

  bl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  br.addTo(m_skyBoxTriangles);

  // zmin
  tl.setNormal(0, 0,  1);
  tr.setNormal(0, 0,  1);
  bl.setNormal(0, 0,  1);
  br.setNormal(0, 0,  1);

  tl.setPoint( 1,  1, -1);      
  tr.setPoint(-1,  1, -1);      
  bl.setPoint( 1, -1, -1);      
  br.setPoint(-1, -1, -1);      

  tl.setTexture( 1,  1, -1);      
  tr.setTexture(-1,  1, -1);      
  bl.setTexture( 1, -1, -1);      
  br.setTexture(-1, -1, -1);      

  tl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  bl.addTo(m_skyBoxTriangles);

  bl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  br.addTo(m_skyBoxTriangles);

  // zmax
  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setPoint(-1,  1,  1);
  tr.setPoint( 1,  1,  1);
  bl.setPoint(-1, -1,  1);
  br.setPoint( 1, -1,  1);

  tl.setTexture(-1,  1,  1);
  tr.setTexture( 1,  1,  1);
  bl.setTexture(-1, -1,  1);
  br.setTexture( 1, -1,  1);

  tl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  bl.addTo(m_skyBoxTriangles);

  bl.addTo(m_skyBoxTriangles);
  tr.addTo(m_skyBoxTriangles);
  br.addTo(m_skyBoxTriangles);
}

//--------------------------------------------------------------
// add a star patch
void StarrySky::addStarPatch(
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
void StarrySky::createStars()
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
// create the moon rectangles.  
void StarrySky::createMoon()
{
  mgVertex tl, tr, bl, br;

  // three points times two triangles
  m_moonTriangles = mgVertex::newBuffer(3*2);

  mgPoint3 moonPoint(0, 0, 1);
  mgPoint3 xaxis(1, 0, 0);
  mgPoint3 yaxis(0, 1, 0);

  double hsize = 0.15; // 0.3

  tl.setNormal(-moonPoint.x, -moonPoint.y, -moonPoint.z);
  tl.setPoint(moonPoint.x - hsize*xaxis.x + hsize*yaxis.x, 
              moonPoint.y - hsize*xaxis.y + hsize*yaxis.y, 
              moonPoint.z - hsize*xaxis.z + hsize*yaxis.z);
  tl.setTexture(0.0, 1.0);

  tr.setNormal(-moonPoint.x, -moonPoint.y, -moonPoint.z);
  tr.setPoint(moonPoint.x + hsize*xaxis.x + hsize*yaxis.x, 
              moonPoint.y + hsize*xaxis.y + hsize*yaxis.y, 
              moonPoint.z + hsize*xaxis.z + hsize*yaxis.z);
  tr.setTexture(1.0, 1.0);

  bl.setNormal(-moonPoint.x, -moonPoint.y, -moonPoint.z);
  bl.setPoint(moonPoint.x - hsize*xaxis.x - hsize*yaxis.x, 
              moonPoint.y - hsize*xaxis.y - hsize*yaxis.y, 
              moonPoint.z - hsize*xaxis.z - hsize*yaxis.z);
  bl.setTexture(0.0, 0.0);

  br.setNormal(-moonPoint.x, -moonPoint.y, -moonPoint.z);
  br.setPoint(moonPoint.x + hsize*xaxis.x - hsize*yaxis.x, 
              moonPoint.y + hsize*xaxis.y - hsize*yaxis.y, 
              moonPoint.z + hsize*xaxis.z - hsize*yaxis.z);
  br.setTexture(1.0, 0.0);

  tl.addTo(m_moonTriangles);
  tr.addTo(m_moonTriangles);
  bl.addTo(m_moonTriangles);

  bl.addTo(m_moonTriangles);
  tr.addTo(m_moonTriangles);
  br.addTo(m_moonTriangles);
}

//--------------------------------------------------------------
// create the sun rectangles.  
void StarrySky::createSun()
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
  tl.setTexture(0.0, 1.0);

  tr.setNormal(-sunPoint.x, -sunPoint.y, -sunPoint.z);
  tr.setPoint(sunPoint.x + hsize*xaxis.x + hsize*yaxis.x, 
              sunPoint.y + hsize*xaxis.y + hsize*yaxis.y, 
              sunPoint.z + hsize*xaxis.z + hsize*yaxis.z);
  tr.setTexture(1.0, 1.0);

  bl.setNormal(-sunPoint.x, -sunPoint.y, -sunPoint.z);
  bl.setPoint(sunPoint.x - hsize*xaxis.x - hsize*yaxis.x, 
              sunPoint.y - hsize*xaxis.y - hsize*yaxis.y, 
              sunPoint.z - hsize*xaxis.z - hsize*yaxis.z);
  bl.setTexture(0.0, 0.0);

  br.setNormal(-sunPoint.x, -sunPoint.y, -sunPoint.z);
  br.setPoint(sunPoint.x + hsize*xaxis.x - hsize*yaxis.x, 
              sunPoint.y + hsize*xaxis.y - hsize*yaxis.y, 
              sunPoint.z + hsize*xaxis.z - hsize*yaxis.z);
  br.setTexture(1.0, 0.0);

  tl.addTo(m_sunTriangles);
  tr.addTo(m_sunTriangles);
  bl.addTo(m_sunTriangles);

  bl.addTo(m_sunTriangles);
  tr.addTo(m_sunTriangles);
  br.addTo(m_sunTriangles);
}

//--------------------------------------------------------------
// create fog triangles
void StarrySky::createFog()
{
  mgPoint3 hit;
  double px, py, pz;
  mgVertex v;

  int steps = 32;
  int vertexSize = 1+steps;

  m_fogVertexes = VertexSky::newBuffer(6*vertexSize*vertexSize);
  m_fogIndexes = mgDisplay->newIndexBuffer(6*6*steps*steps);

  int vertexBase = m_fogVertexes->getLength();

  // ymin outside
  py = 0.0;
  for (int x = 0; x <= steps; x++)
  {
    px = x/(double) steps;
    for (int z = 0; z <= steps; z++)
    {
      pz = z/(double) steps;

      hit.x = 0.5-px;
      hit.y = py-0.5;
      hit.z = pz-0.5;
      hit.normalize();

      v.m_px = (float) hit.x;
      v.m_py = (float) hit.y;
      v.m_pz = (float) hit.z;

      v.addTo(m_fogVertexes);
    }
  }

  // add indexes
  m_fogIndexes->addGrid(vertexBase, vertexSize, steps, steps, false);

  vertexBase = m_fogVertexes->getLength();

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

      v.m_px = (float) hit.x;
      v.m_py = (float) hit.y;
      v.m_pz = (float) hit.z;

      v.addTo(m_fogVertexes);
    }
  }

  // add indexes
  m_fogIndexes->addGrid(vertexBase, vertexSize, steps, steps, false);

  vertexBase = m_fogVertexes->getLength();

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

      v.m_px = (float) hit.x;
      v.m_py = (float) hit.y;
      v.m_pz = (float) hit.z;

      v.addTo(m_fogVertexes);
    }
  }

  // add indexes
  m_fogIndexes->addGrid(vertexBase, vertexSize, steps, steps, false);

  vertexBase = m_fogVertexes->getLength();

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

      v.m_px = (float) hit.x;
      v.m_py = (float) hit.y;
      v.m_pz = (float) hit.z;

      v.addTo(m_fogVertexes);
    }
  }
 
  // add indexes
  m_fogIndexes->addGrid(vertexBase, vertexSize, steps, steps, false);

  vertexBase = m_fogVertexes->getLength();

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

      v.m_px = (float) hit.x;
      v.m_py = (float) hit.y;
      v.m_pz = (float) hit.z;

      v.addTo(m_fogVertexes);
    }
  }

  // add indexes
  m_fogIndexes->addGrid(vertexBase, vertexSize, steps, steps, false);

  vertexBase = m_fogVertexes->getLength();

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

      v.m_px = (float) hit.x;
      v.m_py = (float) hit.y;
      v.m_pz = (float) hit.z;

      v.addTo(m_fogVertexes);
    }
  }

  // add indexes
  m_fogIndexes->addGrid(vertexBase, vertexSize, steps, steps, false);
}

//--------------------------------------------------------------
// delete display buffers
void StarrySky::deleteBuffers()
{
  delete m_skyBoxTriangles;
  m_skyBoxTriangles = NULL;

  delete m_starTriangles;
  m_starTriangles = NULL;

  delete m_moonTriangles;
  m_moonTriangles = NULL;

  delete m_sunTriangles;
  m_sunTriangles = NULL;

  delete m_fogVertexes;
  m_fogVertexes = NULL;
  delete m_fogIndexes;
  m_fogIndexes = NULL;
}

//--------------------------------------------------------------
// create buffers ready to send to display
void StarrySky::createBuffers()
{
  if (m_skyBoxTexture != NULL && m_skyBoxTriangles == NULL)
    createSkyBox();

  if (m_starTexture != NULL && m_starTriangles == NULL)
    createStars();

  if (m_moonTexture != NULL && m_moonTriangles == NULL)
    createMoon();

  if (m_sunTexture != NULL && m_sunTriangles == NULL)
    createSun();

  if (m_fogVertexes == NULL)
    createFog();
}

