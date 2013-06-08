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
#include "VertexPlanet.h"
#include "Planet.h"

//--------------------------------------------------------------
// constructor
Planet::Planet(
  const mgOptionsFile& options,
  double radius)
{
  VertexPlanet::loadShader("planet");

  m_radius = radius / SYSTEM_FAR_SCALE;

  mgString xminFace, xmaxFace, yminFace, ymaxFace, zminFace, zmaxFace;
  options.getFileName("planet-xmin", options.m_sourceFileName, "", xminFace);
  options.getFileName("planet-xmax", options.m_sourceFileName, "", xmaxFace);
  options.getFileName("planet-ymin", options.m_sourceFileName, "", yminFace);
  options.getFileName("planet-ymax", options.m_sourceFileName, "", ymaxFace);
  options.getFileName("planet-zmin", options.m_sourceFileName, "", zminFace);
  options.getFileName("planet-zmax", options.m_sourceFileName, "", zmaxFace);

  m_farTerrain = mgDisplay->loadTextureCube(xminFace, xmaxFace, yminFace, ymaxFace, zminFace, zmaxFace);

  options.getFileName("clouds-xmin", options.m_sourceFileName, "", xminFace);
  options.getFileName("clouds-xmax", options.m_sourceFileName, "", xmaxFace);
  options.getFileName("clouds-ymin", options.m_sourceFileName, "", yminFace);
  options.getFileName("clouds-ymax", options.m_sourceFileName, "", ymaxFace);
  options.getFileName("clouds-zmin", options.m_sourceFileName, "", zminFace);
  options.getFileName("clouds-zmax", options.m_sourceFileName, "", zmaxFace);

  m_farClouds = mgDisplay->loadTextureCube(xminFace, xmaxFace, yminFace, ymaxFace, zminFace, zmaxFace);

  m_farVertexes = NULL;
}

//--------------------------------------------------------------
// destructor
Planet::~Planet()
{
  deleteBuffers();
}
    
//--------------------------------------------------------------
// create buffers ready to send to display
void Planet::createBuffers()
{
  m_shader = VertexPlanet::loadShader("planet");
  m_farVertexes = VertexPlanet::newBuffer(6, true);
}

//--------------------------------------------------------------
// delete display buffers
void Planet::deleteBuffers()
{
  delete m_farVertexes;
  m_farVertexes = NULL;
}

//--------------------------------------------------------------
// animate object
BOOL Planet::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// draw opaque data
void Planet::renderFar(
  double planetDay)
{
  double atmos = 100000.0 / SYSTEM_FAR_SCALE;

  mgPoint3 eyePt;
  mgDisplay->getEyePt(eyePt);

  double eyeDist = eyePt.length()/m_radius;
  double a = 1.0;//+atmos/m_radius;
  if (eyeDist < a)
    return;  // below surface, nothing to do 
  double b = sqrt(eyeDist*eyeDist - a*a);
  double h = (a*b)/eyeDist;
  double m = (a*a)/eyeDist;

  h += atmos/m_radius;

  // x axis from planet center towards eye
  mgPoint3 xaxis(eyePt);
  xaxis.normalize();

  // build y axis
  mgPoint3 yaxis(xaxis);
  yaxis.cross(mgPoint3(0.0, 1.0, 0.0));
  yaxis.normalize();

  mgPoint3 zaxis(yaxis);
  zaxis.cross(xaxis);
  zaxis.normalize();

  mgMatrix4 transform;
  transform._11 = xaxis.x;
  transform._12 = xaxis.y;
  transform._13 = xaxis.z;
  transform._21 = yaxis.x;
  transform._22 = yaxis.y;
  transform._23 = yaxis.z;
  transform._31 = zaxis.x;
  transform._32 = zaxis.y;
  transform._33 = zaxis.z;
  
  VertexPlanet tl, tr, bl, br;

  mgPoint3 pt;
  transform.mapPt(m, -h, h, pt.x, pt.y, pt.z);
  tl.setPoint(m_radius*pt.x, m_radius*pt.y, m_radius*pt.z);

  transform.mapPt(m, h, h, pt.x, pt.y, pt.z);
  tr.setPoint(m_radius*pt.x, m_radius*pt.y, m_radius*pt.z);

  transform.mapPt(m, -h, -h, pt.x, pt.y, pt.z);
  bl.setPoint(m_radius*pt.x, m_radius*pt.y, m_radius*pt.z);

  transform.mapPt(m, h, -h, pt.x, pt.y, pt.z);
  br.setPoint(m_radius*pt.x, m_radius*pt.y, m_radius*pt.z);

  // inverse of world transform
  mgMatrix4 model;
  model.rotateYDeg(-planetDay);

  mgPoint3 lightDir, modelLightDir;
  mgDisplay->getLightDir(lightDir);
  model.mapPt(lightDir, modelLightDir);
  transform.multiply(model);

  mgPoint3 modelEye; 
  transform.mapPt(eyeDist, 0.0, 0.0, modelEye.x, modelEye.y, modelEye.z);

  transform.mapPt(m, -h, h, pt.x, pt.y, pt.z);
  tl.setModelPoint(pt.x, pt.y, pt.z);

  transform.mapPt(m, h, h, pt.x, pt.y, pt.z);
  tr.setModelPoint(pt.x, pt.y, pt.z);

  transform.mapPt(m, -h, -h, pt.x, pt.y, pt.z);
  bl.setModelPoint(pt.x, pt.y, pt.z);

  transform.mapPt(m, h, -h, pt.x, pt.y, pt.z);
  br.setModelPoint(pt.x, pt.y, pt.z);

  m_farVertexes->reset();
  tl.addTo(m_farVertexes);
  tr.addTo(m_farVertexes);
  bl.addTo(m_farVertexes);
  bl.addTo(m_farVertexes);
  tr.addTo(m_farVertexes);
  br.addTo(m_farVertexes);

  mgPoint3 specularColor(0.4, 0.4, 0.4);

/*
  if (writeShaderVars)
  {
    mgDebug("mgPoint3 tlWorld(%g, %g, %g);", tl.m_px, tl.m_py, tl.m_pz);
    mgDebug("mgPoint3 trWorld(%g, %g, %g);", tr.m_px, tr.m_py, tr.m_pz);
    mgDebug("mgPoint3 blWorld(%g, %g, %g);", bl.m_px, bl.m_py, bl.m_pz);
    mgDebug("mgPoint3 brWorld(%g, %g, %g);", br.m_px, br.m_py, br.m_pz);

    mgDebug("mgPoint3 tlModel(%g, %g, %g);", tl.m_mx, tl.m_my, tl.m_mz);
    mgDebug("mgPoint3 trModel(%g, %g, %g);", tr.m_mx, tr.m_my, tr.m_mz);
    mgDebug("mgPoint3 blModel(%g, %g, %g);", bl.m_mx, bl.m_my, bl.m_mz);
    mgDebug("mgPoint3 brModel(%g, %g, %g);", br.m_mx, br.m_my, br.m_mz);

    mgDebug("mgPoint3 modelEye(%g, %g, %g);", modelEye.x, modelEye.y, modelEye.z);
    mgDebug("mgPoint3 modelLightDir(%g, %g, %g);", modelLightDir.x, modelLightDir.y, modelLightDir.z);

    mgMatrix4 transform;
    mgDisplay->getMVTransform(transform);
    mgDebug("mgMatrix4 mvMatrix;");
    mgDebug("mvMatrix._11 = %g; mvMatrix._12 = %g; mvMatrix._13 = %g; mvMatrix._14 = %g;", transform._11, transform._12, transform._13, transform._14);
    mgDebug("mvMatrix._21 = %g; mvMatrix._22 = %g; mvMatrix._23 = %g; mvMatrix._24 = %g;", transform._21, transform._22, transform._23, transform._24);
    mgDebug("mvMatrix._31 = %g; mvMatrix._32 = %g; mvMatrix._33 = %g; mvMatrix._34 = %g;", transform._31, transform._32, transform._33, transform._34);
    mgDebug("mvMatrix._41 = %g; mvMatrix._42 = %g; mvMatrix._43 = %g; mvMatrix._44 = %g;", transform._41, transform._42, transform._43, transform._44);

    writeShaderVars = false;
  }
*/

  mgDisplay->setShader(m_shader);
  mgDisplay->setShaderUniform(m_shader, "modelEye", modelEye);
  mgDisplay->setShaderUniform(m_shader, "modelLightDir", modelLightDir);
  mgDisplay->setShaderUniform(m_shader, "specularColor", specularColor);
  mgDisplay->setTexture(m_farTerrain, 0);
  mgDisplay->setTexture(m_farClouds, 1);
  mgDisplay->draw(MG_TRIANGLES, m_farVertexes);
}

//--------------------------------------------------------------
// render medium version
void Planet::renderMedium()
{
}

//--------------------------------------------------------------
// render near version
void Planet::renderNear()
{
}


