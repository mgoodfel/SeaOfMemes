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

#include "SolarSystem.h"
#include "Earth.h"

//--------------------------------------------------------------
// constructor
Earth::Earth(
  double radius)
{
  mgVertex::loadShader("litTexture");

  m_radius = radius;

  m_farTexture = mgDisplay->loadTexture("docs/textures/iceblock.jpg"); // "docs/textures/earth2048.jpg"); 
  m_farVertexes = NULL;
  m_farIndexes = NULL;
}

//--------------------------------------------------------------
// destructor
Earth::~Earth()
{
  deleteBuffers();
}

//--------------------------------------------------------------
// delete display buffers
void Earth::deleteBuffers()
{
  delete m_farVertexes;
  m_farVertexes = NULL;

  delete m_farIndexes;
  m_farIndexes = NULL;
}

//--------------------------------------------------------------
// create buffers ready to send to display
void Earth::createBuffers()
{
  const int HEAD_STRIPS = 16;
  const int HEAD_FACES = 32;

  // three points times two triangles times grid size
  m_farVertexes = mgVertex::newBuffer(3*2*HEAD_STRIPS * HEAD_FACES);

  mgVertex tl, tr, bl, br;

  // create a sphere
  for (int i = 1; i <= HEAD_STRIPS; i++)
  {
    double topAngle = (PI*(i-1))/HEAD_STRIPS;
    double topRadius = sin(topAngle);
    double topY = cos(topAngle);
                                    
    double botAngle = (PI*i)/HEAD_STRIPS;
    double botRadius = sin(botAngle);
    double botY = cos(botAngle);
                                    
    for (int j = 1; j <= HEAD_FACES; j++)
    {
      double leftAngle = (2.0*PI*(j-1))/HEAD_FACES;
      tl.setPoint(topRadius * cos(leftAngle), topY, topRadius * sin(leftAngle));
      bl.setPoint(botRadius * cos(leftAngle), botY, botRadius * sin(leftAngle));

      double rightAngle = (2.0*PI*j)/HEAD_FACES;
      tr.setPoint(topRadius * cos(rightAngle), topY, topRadius * sin(rightAngle));
      br.setPoint(botRadius * cos(rightAngle), botY, botRadius * sin(rightAngle));

      // set texture coordinates
      double topTx = (i-1); // /(double)HEAD_STRIPS;
      double botTx = i; // /(double)HEAD_STRIPS;
      double leftTx = (j-1); // /(double)HEAD_FACES;
      double rightTx = j; // /(double)HEAD_FACES;

      tl.setTexture(leftTx, topTx);
      tr.setTexture(rightTx, topTx);
      bl.setTexture(leftTx, botTx);
      br.setTexture(rightTx, botTx);

      // use vertexes as normals, since they are from origin
      mgPoint3 normal;
      normal = mgPoint3(tl.m_px, tl.m_py, tl.m_pz);
      normal.normalize();
      tl.setNormal(normal.x, normal.y, normal.z);

      normal = mgPoint3(tr.m_px, tr.m_py, tr.m_pz);
      normal.normalize();
      tr.setNormal(normal.x, normal.y, normal.z);

      normal = mgPoint3(bl.m_px, bl.m_py, bl.m_pz);
      normal.normalize();
      bl.setNormal(normal.x, normal.y, normal.z);

      normal = mgPoint3(br.m_px, br.m_py, br.m_pz);
      normal.normalize();
      br.setNormal(normal.x, normal.y, normal.z);

      tl.addTo(m_farVertexes);
      tr.addTo(m_farVertexes);
      bl.addTo(m_farVertexes);

      bl.addTo(m_farVertexes);
      tr.addTo(m_farVertexes);
      br.addTo(m_farVertexes);
    }
  }
}

//--------------------------------------------------------------
// update animation 
BOOL Earth::animate(
  double now,                       // current time (ms)
  double since)                    // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// render the avatar
void Earth::renderFar()
{
  mgMatrix4 model;
  model.rotateYDeg(-50);
  model.scale(m_radius/SYSTEM_FAR_SCALE);
  mgDisplay->setModelTransform(model);

  mgDisplay->setShader("litTexture");
  mgDisplay->setMatColor(1, 1, 1);
  mgDisplay->setTexture(m_farTexture);
  mgDisplay->draw(MG_TRIANGLES, m_farVertexes);
}


//--------------------------------------------------------------
// render the avatar
void Earth::renderNear()
{
}

//--------------------------------------------------------------
// render the avatar
void Earth::renderMedium()
{
}
