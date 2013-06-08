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

#include "Intro.h"
#include "Saucer.h"
#include "Tube.h"

const double WALL_RADIUS = 20.0;
const int WALL_STEPS = 40;

const double TUBE_RADIUS = 1.0;
const int TUBE_STEPS = 15;

const double BALL_RADIUS = 0.5;  // height of saucer base

const double EYE_OFFSET_Z = -15.0;
const double EYE_OFFSET_Y = 7.0;

//--------------------------------------------------------------
// constructor
Intro::Intro(
  const mgOptionsFile& options,
  const mgPoint3& origin)
{
  m_origin = origin;

  // load wall texture
  mgString fileName;
  options.getFileName("tube", options.m_sourceFileName, "tube.jpg", fileName);
  m_tubeTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("intro-wall", options.m_sourceFileName, "tube.jpg", fileName);
  m_wallTexture = mgDisplay->loadTexture(fileName);

  m_saucer = new Saucer(options);
  m_tube = new Tube(TUBE_RADIUS, TUBE_STEPS);
  m_wall = new Tube(WALL_RADIUS, WALL_STEPS);

  initTrack();
  m_ballPosn = 0.0;
  updateBallPt();
}

//--------------------------------------------------------------
// destructor
Intro::~Intro()
{
  deleteBuffers();

  delete m_tube;
  m_tube = NULL;

  delete m_wall;
  m_wall = NULL;

  delete m_saucer;
  m_saucer = NULL;
}

//--------------------------------------------------------------
// create the intro track
void Intro::initTrack()
{
  const double TRACK_RADIUS = 50.0;
  const int TRACK_POINTS = 12;
  const double CNTL_LEN = 2*TRACK_RADIUS/TRACK_POINTS;

  mgPoint3 points[TRACK_POINTS+2];

  // create simple circular track
  for (int i = 0; i <= TRACK_POINTS+1; i++)
  {
    double lenAngle = (2*PI*i)/TRACK_POINTS;

    mgPoint3* pt = &points[i];
    pt->x = m_origin.x + TRACK_RADIUS * cos(lenAngle);
    pt->x -= TRACK_RADIUS;  // 0,0,0 is start position
    pt->y = m_origin.y + 2*sin(5*(2*PI*i)/TRACK_POINTS);
    pt->z = m_origin.z + TRACK_RADIUS * sin(lenAngle);
  }

  // build first point of spline
  mgPoint3 pt, lastPt, nextPt;
  pt = points[0];
  lastPt = points[TRACK_POINTS-1];
  nextPt = points[1];

  mgPoint3 cntlPt(pt);
  cntlPt.subtract(lastPt);
  cntlPt.add(nextPt);
  cntlPt.subtract(pt);
  cntlPt.scale(0.125);
  cntlPt.add(pt);

  mgBezier* wallTrack = new mgBezier();
  mgBezier* tubeTrack = new mgBezier();
  wallTrack->addVertex(pt, cntlPt);
  tubeTrack->addVertex(pt, cntlPt);

  // build remaining points of spline
  for (int i = 1; i <= TRACK_POINTS; i++)
  {
    mgPoint3 pt = points[i];
    mgPoint3 lastPt = points[i-1];
    mgPoint3 nextPt = points[i+1];

    mgPoint3 cntlPt(pt);
    cntlPt.subtract(lastPt);
    cntlPt.add(nextPt);
    cntlPt.subtract(pt);
    cntlPt.scale(-0.125);
    cntlPt.add(pt);

    wallTrack->addVertex(pt, cntlPt);
    tubeTrack->addVertex(pt, cntlPt);
  }

  m_tube->setTrack(tubeTrack);
  m_wall->setTrack(wallTrack);
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Intro::createBuffers()
{
  m_saucer->createBuffers();
  m_tube->createBuffers();
  m_wall->createBuffers();
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Intro::deleteBuffers()
{
  m_saucer->deleteBuffers();
  m_tube->deleteBuffers();
  m_wall->deleteBuffers();
}

//-----------------------------------------------------------------------------
// animate the scene
void Intro::animate(
  double now,
  double since)
{
  m_saucer->animate(now, since);
  updateBallPt();
}

//-----------------------------------------------------------------------------
// update position of saucer
void Intro::updateBallPt()
{
  m_tube->getBallView(m_ballPosn, 0.0, m_ballOrigin, m_ballMatrix);

  // compute position of ball
  m_ballMatrix.mapPt(0.0, BALL_RADIUS + TUBE_RADIUS, 0.0, m_ballPt.x, m_ballPt.y, m_ballPt.z);
  m_ballPt.add(m_ballOrigin);

  // compute position of eye
  m_eyeMatrix = m_ballMatrix;
  m_eyeMatrix.mapPt(0.0, EYE_OFFSET_Y, EYE_OFFSET_Z, m_eyePt.x, m_eyePt.y, m_eyePt.z);
  m_eyePt.add(m_ballOrigin);

  // eye matrix is inverse transform.  rotate down to center ball in view
  m_eyeMatrix.transpose();
  m_eyeMatrix.rotateXDeg(-22);
}

//-----------------------------------------------------------------------------
// render the intro scene
void Intro::render()
{
  // draw the saucer
  if (m_saucer != NULL)
  {
    double saucerScale = 0.5;
    double ballRadius = 0.5*saucerScale;
    mgMatrix4 ballModel;
    ballModel.scale(saucerScale);
    ballModel.translate(0.0, ballRadius + TUBE_RADIUS, 0.0);
    ballModel.multiply(m_ballMatrix);
    ballModel.translate(m_ballOrigin);

    mgDisplay->setModelTransform(ballModel);

    m_saucer->render();
  }

  // draw the walls
  if (m_wall != NULL)
  {
    mgDisplay->setCulling(false);
    mgMatrix4 wallModel;
    mgDisplay->setModelTransform(wallModel);
    mgDisplay->setTexture(m_wallTexture);
    m_wall->render();
    mgDisplay->setCulling(true);
  }

  // draw the track
  if (m_tube != NULL)
  {
    mgDisplay->setTransparent(true);
    mgMatrix4 tubeModel;
    mgDisplay->setModelTransform(tubeModel);
    mgDisplay->setTexture(m_tubeTexture);
    m_tube->render();
    mgDisplay->setTransparent(false);
  }
}
