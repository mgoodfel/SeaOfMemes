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

#include "Tube.h"

//--------------------------------------------------------------
// constructor
Tube::Tube(
  double radius,
  int widthSteps)
{
  m_radius = radius;
  m_widthSteps = widthSteps;

  m_indexes = NULL;
  m_vertexes = NULL;
  m_track = NULL;
  m_trackLen = 0.0;
  m_trackSteps = 0;
}

//--------------------------------------------------------------
// destructor
Tube::~Tube()
{
  deleteBuffers();

  delete m_track;
  m_track = NULL;
}

//--------------------------------------------------------------------
// set the track to follow (tube owns storage)
void Tube::setTrack(
  mgBezier*track)
{
  m_track = track;
  m_trackLen = track->getLength();
  m_trackSteps = (int) (m_trackLen/10.0);
}

//--------------------------------------------------------------------
// compute ball position and eye matrix
void Tube::getBallView(
  double ballPosn,
  double ballRotate,
  mgPoint3& ballOrigin,
  mgMatrix4& ballMatrix)
{
  // get track point at ball position
  m_track->splinePt(ballPosn, ballOrigin);

  // get track point slightly behind
  double backPosn = ballPosn-1.0;
  if (backPosn < 0)
    backPosn += m_trackLen;

  mgPoint3 lastPt;
  m_track->splinePt(backPosn, lastPt);
  mgPoint3 segment(ballOrigin);
  segment.subtract(lastPt);

  // build the rotation matrix for point on track
  mgPoint3 zaxis(segment);
  zaxis.normalize();

  mgPoint3 xaxis(0, 1, 0);
  xaxis.cross(segment);
  xaxis.normalize();

  mgPoint3 yaxis(segment);
  yaxis.cross(xaxis);
  yaxis.normalize();

  ballMatrix._11 = xaxis.x;
  ballMatrix._12 = xaxis.y;
  ballMatrix._13 = xaxis.z;
  ballMatrix._21 = yaxis.x;
  ballMatrix._22 = yaxis.y;
  ballMatrix._23 = yaxis.z;
  ballMatrix._31 = zaxis.x;
  ballMatrix._32 = zaxis.y;
  ballMatrix._33 = zaxis.z;

  mgMatrix4 rotation;
  rotation.rotateZDeg(ballRotate);
  ballMatrix.leftMultiply(rotation);
}   

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Tube::createBuffers()
{
  m_vertexes = mgVertex::newBuffer((m_widthSteps+1)*(m_trackSteps+1));
  m_indexes = mgDisplay->newIndexBuffer(m_widthSteps*m_trackSteps*6);

  double lenScale = m_radius * 2 * PI * 32;
  double tubeLen = 0.0;

  mgVertex v;
  mgMatrix4 tubeMatrix;

  mgPoint3 lastPt;
  m_track->splinePt((m_trackSteps-1)*m_trackLen/m_trackSteps, lastPt);

  for (int h = 0; h <= m_trackSteps; h++)
  {
    mgPoint3 tubePt;
    m_track->splinePt(h*m_trackLen/m_trackSteps, tubePt);

    // build coordinate system at point
    mgPoint3 segment(tubePt);
    segment.subtract(lastPt);
    double segmentLen = segment.length();
    segment.normalize();

    mgPoint3 xaxis(0, 1, 0);
    xaxis.cross(segment);
    xaxis.normalize();

    mgPoint3 yaxis(segment);
    yaxis.cross(xaxis);
    yaxis.normalize();

    tubeMatrix._11 = xaxis.x;
    tubeMatrix._12 = xaxis.y;
    tubeMatrix._13 = xaxis.z;
    tubeMatrix._21 = yaxis.x;
    tubeMatrix._22 = yaxis.y;
    tubeMatrix._23 = yaxis.z;
    tubeMatrix._31 = segment.x;
    tubeMatrix._32 = segment.y;
    tubeMatrix._33 = segment.z;

    for (int w = 0; w <= m_widthSteps; w++)
    {
      double widAngle = (2*PI*w)/m_widthSteps;
      double widX = cos(widAngle);
      double widY = sin(widAngle);

      double nx, ny, nz;
      tubeMatrix.mapPt(widX, widY, 0.0, nx, ny, nz);

      v.setNormal(nx, ny, nz);
      v.setPoint(nx*m_radius + tubePt.x, ny*m_radius + tubePt.y, nz*m_radius + tubePt.z);
      
      v.setTexture(w/(double) m_widthSteps, tubeLen/lenScale);

      v.addTo(m_vertexes);
    }

    tubeLen += segmentLen;
    lastPt = tubePt;
  }

  m_indexes->addGrid(0, m_widthSteps+1, m_trackSteps, m_widthSteps, true);
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Tube::deleteBuffers()
{
  delete m_indexes;
  m_indexes = NULL;
  delete m_vertexes;
  m_vertexes = NULL;
}

//-----------------------------------------------------------------------------
// animate the scene
void Tube::animate(
  double now,
  double since)
{
}

//-----------------------------------------------------------------------------
// render the intro scene
void Tube::render()
{
  // draw triangles using texture and shader
  mgDisplay->setShader("unlitTexture");
  mgDisplay->draw(MG_TRIANGLES, m_vertexes, m_indexes);
}
