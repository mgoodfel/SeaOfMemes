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

#include "DeathRay.h"

const int RAY_POINTS = 20;
const double RAY_RADIUS = 1.0;

//--------------------------------------------------------------
// constructor
DeathRay::DeathRay(
  const mgOptionsFile& options)
{
  m_vertexes = NULL;
  m_texture = NULL;

  m_shader = mgVertex::loadShader("ray");

  mgString fileName;
  options.getFileName("ray", options.m_sourceFileName, "", fileName);
  m_texture = mgDisplay->loadTexture(fileName);
}

//--------------------------------------------------------------
// destructor
DeathRay::~DeathRay()
{
  deleteBuffers();
}

//--------------------------------------------------------------
// create vertex and index buffers
void DeathRay::createBuffers()
{
  if (m_vertexes != NULL)
    return;

  m_vertexes = mgVertex::newBuffer(3*RAY_POINTS, true);
}

//--------------------------------------------------------------
// aim ray from source to target
void DeathRay::aimRay(
  const mgPoint3& source,
  const mgPoint3& target)
{
  m_vertexes->reset();

  mgPoint3 zaxis(target);
  zaxis.subtract(source);
  double zlen = zaxis.length();
  zaxis.normalize();

  // back the target up a bit to avoid going through the saucer
  mgPoint3 hitPt(zaxis);
  hitPt.scale(zlen-1.5);
  hitPt.add(source);

  mgPoint3 xaxis(0, 1, 0);
  xaxis.cross(zaxis);
  xaxis.normalize();

  mgPoint3 yaxis(zaxis);
  yaxis.cross(xaxis);
  yaxis.normalize();

  // create a cone from source to target
  mgVertex v;
  for (int i = 0; i < RAY_POINTS; i++)
  {
    double angle = (-2*PI*i)/RAY_POINTS;
    double dx = cos(angle);
    double dy = sin(angle);

    // add source as triangle top point
    v.setPoint(source.x, source.y, source.z);
    v.setTexture(i/(double) RAY_POINTS, 0.0);
    m_vertexes->addVertex(&v);

    // add point in circle as triangle bottom-left
    v.m_px = (float) (hitPt.x + RAY_RADIUS * dx*xaxis.x + RAY_RADIUS * dy*yaxis.x);
    v.m_py = (float) (hitPt.y + RAY_RADIUS * dx*xaxis.y + RAY_RADIUS * dy*yaxis.y);
    v.m_pz = (float) (hitPt.z + RAY_RADIUS * dx*xaxis.z + RAY_RADIUS * dy*yaxis.z);
    v.setTexture(i/(double) RAY_POINTS, 1.0);
    m_vertexes->addVertex(&v);

    // add next point in circle as triangle bottom-right
    angle = (-2*PI*(i+1))/RAY_POINTS;
    dx = cos(angle);
    dy = sin(angle);

    v.m_px = (float) (hitPt.x + RAY_RADIUS * dx*xaxis.x + RAY_RADIUS * dy*yaxis.x);
    v.m_py = (float) (hitPt.y + RAY_RADIUS * dx*xaxis.y + RAY_RADIUS * dy*yaxis.y);
    v.m_pz = (float) (hitPt.z + RAY_RADIUS * dx*xaxis.z + RAY_RADIUS * dy*yaxis.z);
    v.setTexture((i+1)/(double) RAY_POINTS, 1.0);
    m_vertexes->addVertex(&v);
  }
}

//--------------------------------------------------------------
// delete vertex and index buffers
void DeathRay::deleteBuffers()
{
  delete m_vertexes;
  m_vertexes = NULL;
}

//--------------------------------------------------------------
// animate the saucer
void DeathRay::animate(
  double now,
  double since)
{
  m_textureOffset.x += since/2000;
  if (m_textureOffset.x > 1.0)
    m_textureOffset.x -= 1.0;

  m_textureOffset.y -= since/5000;
  if (m_textureOffset.y < 0.0)
    m_textureOffset.y += 1.0;
}

//--------------------------------------------------------------
// render on screen
void DeathRay::render()
{
  if (m_vertexes == NULL)
    return;

  mgMatrix4 model;
  mgDisplay->setModelTransform(model);

  mgDisplay->setTransparent(true);

  // draw triangles using texture and shader
  mgDisplay->setShader(m_shader);
  mgDisplay->setShaderUniform(m_shader, "textureOffset", m_textureOffset);
  mgDisplay->setTexture(m_texture);
  mgDisplay->draw(MG_TRIANGLES, m_vertexes);

  mgDisplay->setTransparent(false);
}

