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

// identify the program for the framework log
const char* mgProgramName = "DontHitMe";
const char* mgProgramVersion = "Part 83";

#include "StarrySky.h"
#include "Tube.h"
#include "Saucer.h"
#include "Planet.h"
#include "Wreck.h"
#include "Tower.h"
#include "Prickle.h"
#include "Intro.h"
#include "DontHitMe.h"

const double INVALID_TIME = -1.0;
const int BOTH_BUTTONS = MG_EVENT_MOUSE1_DOWN | MG_EVENT_MOUSE2_DOWN;

const double TUBE_RADIUS = 1.0;
const int TUBE_STEPS = 15;
const double BALL_RADIUS = 0.5;  // height of saucer base

const double EYE_OFFSET_Z = -15.0;
const double EYE_OFFSET_Y = 7.0;

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set the working directory to dir containing options
  mgOSFindWD("options.xml");

  return new DontHitMe();
}

//--------------------------------------------------------------
// constructor
DontHitMe::DontHitMe()
{
  m_deskCursor = NULL;
  m_moveCursor = NULL;
  m_intro = NULL;
  m_sky = NULL;
  m_saucer = NULL;
  m_tube = NULL;
  m_planet = NULL;
  m_wreck = NULL;
  m_tower1 = NULL;
  m_tower2 = NULL;
  m_tubeTexture = NULL;

  // parse the options file
  m_options.parseFile("options.xml");
}

//--------------------------------------------------------------
// destructor
DontHitMe::~DontHitMe()
{
}

//--------------------------------------------------------------
// request display 
void DontHitMe::appRequestDisplay()
{
  mgString title;
  title.format("DontHitMe, %s", mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  mgPlatform->setFullscreen(m_options.getBoolean("fullscreen", false));
  mgPlatform->setWindowBounds(
    m_options.getInteger("windowX", 100), 
    m_options.getInteger("windowY", 100),
    m_options.getInteger("windowWidth", 800),
    m_options.getInteger("windowHeight", 600));

  // set graphics parameters
  mgPlatform->setMultiSample(m_options.getBoolean("multiSample", true));
  mgPlatform->setSwapImmediate(m_options.getBoolean("graphicsTiming", false));

  mgString platform;
  m_options.getString("platform", "", platform);
  mgPlatform->setDisplayLibrary(platform);
}

//--------------------------------------------------------------
// initialize application
void DontHitMe::appInit()
{
  // get the important directories
  mgString shaderDir;
  m_options.getFileName("shaderDir", m_options.m_sourceFileName, "docs/shaders", shaderDir);
  mgString uiDir;
  m_options.getFileName("uiDir", m_options.m_sourceFileName, "docs/ui", uiDir);
  mgString fontDir;
  m_options.getFileName("fontDir", m_options.m_sourceFileName, "docs/fonts", fontDir);

  mgInitDisplayServices(shaderDir, fontDir);

  mgDisplay->setFOV(m_options.getDouble("FOV", 60.0));
  mgDisplay->setDPI(m_options.getInteger("DPI", 0));

  // load cursor pattern
  loadCursor();
  setDeskMode(true);

  initMovement();

  m_sky = new StarrySky(m_options);
  m_sky->enableSkyBox(true);
  m_sky->enableStars(true);
  m_sky->enableSun(true);

  mgPoint3 lightDir(0, 1, 1);
  lightDir.normalize();
  m_sky->setSunDir(lightDir);

  mgDisplay->setLightDir(lightDir);

  m_saucer = new Saucer(m_options);
  m_planet = new Planet(m_options);

  m_wreck = new Wreck(m_options);
  m_tower1 = new Tower(m_options, false);
  m_tower2 = new Tower(m_options, false);

  // create prickle flock
  Prickle* prickle = new Prickle(m_options);
  prickle->m_origin = mgPoint3(300, 0, 0);
  m_prickles.add(prickle);

  mgString fileName;
  m_options.getFileName("tube", m_options.m_sourceFileName, "tube.jpg", fileName);
  m_tubeTexture = mgDisplay->loadTexture(fileName);

  initTrack();
  initBall();
  updateBallPt();

  m_intro = new Intro(m_options, m_ballOrigin);
  m_showingIntro = false;

  m_lastAnimate = mgOSGetTime();  // now!
}

//--------------------------------------------------------------
// terminate application
void DontHitMe::appTerm()
{
  delete m_moveCursor;
  m_moveCursor = NULL;

  delete m_deskCursor;
  m_deskCursor = NULL;

  delete m_intro;
  m_intro = NULL;

  delete m_saucer;
  m_saucer = NULL;

  delete m_tube;
  m_tube = NULL;

  delete m_planet;
  m_planet = NULL;

  delete m_wreck;
  m_wreck = NULL;

  delete m_tower1;
  m_tower1 = NULL;

  delete m_tower2;
  m_tower2 = NULL;

  delete m_sky;
  m_sky = NULL;

  for (int i = 0; i < m_prickles.length(); i++)
  {
    Prickle* prickle = (Prickle*) m_prickles[i];
    delete prickle;
  }
  m_prickles.removeAll();

  mgTermDisplayServices();
}

//--------------------------------------------------------------------
// initialize the track
void DontHitMe::initTrack()
{
  m_tube = new Tube(TUBE_RADIUS, TUBE_STEPS);

  mgPoint3 points[100];
  int pointCount = 0;

  points[pointCount++] = mgPoint3(-228.416445, -41.358831, -117.116098);
  points[pointCount++] = mgPoint3(-243.142015, -13.272057, -78.736983);
  points[pointCount++] = mgPoint3(-250.652838, 13.582563, -41.237106);
  points[pointCount++] = mgPoint3(-246.772827, 42.275564, -9.971116);
  points[pointCount++] = mgPoint3(-220.172371, 77.741417, -1.910027);
  points[pointCount++] = mgPoint3(-186.696810, 93.162267, 27.003141);
  points[pointCount++] = mgPoint3(-162.644748, 91.834728, 49.839272);
  points[pointCount++] = mgPoint3(-135.575121, 83.329043, 65.969549);
  points[pointCount++] = mgPoint3(-95.004644, 67.156703, 86.108004);
  points[pointCount++] = mgPoint3(-63.820451, 51.180720, 101.277010);
  points[pointCount++] = mgPoint3(-34.210671, 32.013787, 115.049674);
  points[pointCount++] = mgPoint3(-5.953715, 12.834811, 122.929728);
  points[pointCount++] = mgPoint3(27.007515, -6.686469, 121.968658);
  points[pointCount++] = mgPoint3(61.645948, -7.969721, 120.272704);
  points[pointCount++] = mgPoint3(94.041745, -1.391364, 121.005816);
  points[pointCount++] = mgPoint3(129.733907, 10.565194, 122.912350);
  points[pointCount++] = mgPoint3(161.957111, 16.065340, 108.947959);
  points[pointCount++] = mgPoint3(207.644826, 16.679817, 70.219127);
  points[pointCount++] = mgPoint3(219.624802, 18.653035, 47.242515);
  points[pointCount++] = mgPoint3(223.891977, 9.090362, 27.537427);
  points[pointCount++] = mgPoint3(222.537937, -4.518158, 5.098732);
  points[pointCount++] = mgPoint3(213.729281, -14.378990, -22.287487);
  points[pointCount++] = mgPoint3(208.807850, -27.577125, -32.607589);
  points[pointCount++] = mgPoint3(208.981896, -54.885413, -33.944260);
  points[pointCount++] = mgPoint3(201.786616, -85.248045, -39.267186);
  points[pointCount++] = mgPoint3(173.388444, -106.360454, -61.914524);
  points[pointCount++] = mgPoint3(148.180797, -107.036729, -80.996889);
  points[pointCount++] = mgPoint3(119.912835, -79.744013, -94.359016);
  points[pointCount++] = mgPoint3(84.907808, -64.429394, -102.021798);
  points[pointCount++] = mgPoint3(28.167741, -68.077039, -117.682767);
  points[pointCount++] = mgPoint3(-6.985498, -80.490656, -130.833510);
  points[pointCount++] = mgPoint3(-43.262429, -96.783243, -145.603375);
  points[pointCount++] = mgPoint3(-74.684409, -110.848949, -158.145489);
  points[pointCount++] = mgPoint3(-110.273785, -110.509320, -167.411397);
  points[pointCount++] = mgPoint3(-150.329849, -96.364764, -168.732019);
  points[pointCount++] = mgPoint3(-182.235273, -76.109089, -152.612426);
  points[pointCount++] = mgPoint3(-220.564855, -57.127795, -133.041037);

  points[pointCount++] = mgPoint3(-228.416445, -41.358831, -117.116098);
  points[pointCount++] = mgPoint3(-243.142015, -13.272057, -78.736983);
  points[pointCount++] = mgPoint3(-250.652838, 13.582563, -41.237106);
/*
  const double TRACK_RADIUS = 350.0;

  // create simple circular track
  for (int i = 0; i <= TRACK_POINTS+1; i++)
  {
    double lenAngle = (2*PI*i)/TRACK_POINTS;

    mgPoint3* pt = &points[i];
    pt->x = TRACK_RADIUS * cos(lenAngle);
    pt->y = 60*sin(5*(2*PI*i)/TRACK_POINTS);
    pt->z = TRACK_RADIUS * sin(lenAngle);
  }
*/

  // build first point of spline
  mgBezier* track = new mgBezier();

  // build first point of spline
  mgPoint3 lastPt(points[0]);
  mgPoint3 nowPt(points[1]);
  mgPoint3 nextPt(points[2]);

  mgPoint3 cntlPt(nowPt);
  cntlPt.subtract(lastPt);
  cntlPt.add(nextPt);
  cntlPt.subtract(nowPt);
  cntlPt.scale(0.125);
  cntlPt.add(nowPt);

  track->addVertex(nowPt, cntlPt);

  // build remaining points of spline
  for (int i = 2; i < pointCount-1; i++)
  {
    mgPoint3 pt = points[i];
    lastPt = points[i-1];
    nextPt = points[i+1];

    cntlPt = pt;
    cntlPt.subtract(lastPt);
    cntlPt.add(nextPt);
    cntlPt.subtract(pt);
    cntlPt.scale(-0.125);
    cntlPt.add(pt);

    track->addVertex(pt, cntlPt);
  }

  m_tube->setTrack(track);
}

//--------------------------------------------------------------------
// initialize ball state
void DontHitMe::initBall()
{
  m_ballPosn = 0.0;
  m_ballRotate = 0.0;
  m_ballRoll = 0.0;

  m_rotateSpeed = 0.0;
  m_rollSpeed = 0.0;
}

//--------------------------------------------------------------------
// compute ball pt
void DontHitMe::updateBallPt()
{
  m_tube->getBallView(m_ballPosn, m_ballRotate, m_ballOrigin, m_ballMatrix);

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
// handle idle time
void DontHitMe::appIdle()
{
  double now = mgOSGetTime();
  appViewAnimate(now, now - m_lastAnimate);
  m_lastAnimate = now;

  mgDisplay->clearView();
  appViewDraw();
/*
  if (m_ui != NULL)
    m_ui->render();
*/
  mgDisplay->drawCursor();
  mgDisplay->swapBuffers();
}

//--------------------------------------------------------------------
// initialize movement state
void DontHitMe::initMovement()
{
  m_forwardSince = INVALID_TIME;
  m_backwardSince = INVALID_TIME;
  m_leftSince = INVALID_TIME;
  m_rightSince = INVALID_TIME;
  m_upSince = INVALID_TIME;
  m_downSince = INVALID_TIME;
  m_turnLeftSince = INVALID_TIME;
  m_turnRightSince = INVALID_TIME;

  m_turnSpeed = 120.0/1000;             // degrees per ms
  m_moveSpeed = 20.0/1000;             // units per ms // 100.0/1000;

  m_rollRate = 0.02/1000;
  m_rollDrag = m_rollRate/2;
  m_rollMax = 40.0/1000;

  m_rotateRate = 0.8/1000;
  m_rotateDrag = m_rotateRate/3;
  m_rotateMax = 220.0/1000;         // degrees per ms

  m_flying = false;
  m_eyeRotX = 0.0;
  m_eyeRotY = 0.0;
  m_eyeRotZ = 0.0;

  m_eyeChanged = true;
}

//--------------------------------------------------------------------
// update position of eye when controlling rolling ball
BOOL DontHitMe::updateRollingMovement(
  double now,
  double since)
{
  // update position
  BOOL changed = false;

  // update rotate speed
  if (m_turnLeftSince != INVALID_TIME)
  {
    m_rotateSpeed += m_rotateRate * (now - m_turnLeftSince);
    m_rotateSpeed = min(m_rotateSpeed, m_rotateMax);

    changed = true;
    m_turnLeftSince = now;
  }

  if (m_turnRightSince != INVALID_TIME)
  {
    m_rotateSpeed -= m_rotateRate * (now - m_turnRightSince);
    m_rotateSpeed = max(m_rotateSpeed, -m_rotateMax);

    changed = true;
    m_turnRightSince = now;
  }
  
  if (m_forwardSince != INVALID_TIME)
  {
    m_rollSpeed += m_rollRate*(now - m_forwardSince);
    m_rollSpeed = min(m_rollSpeed, m_rollMax);

    changed = true;
    m_forwardSince = now;
  }

  if (m_backwardSince != INVALID_TIME)
  {
    m_rollSpeed -= m_rollRate*(now - m_backwardSince);
    m_rollSpeed = max(m_rollSpeed, -m_rollMax);

    changed = true;
    m_backwardSince = now;
  }

  return changed;
}

//--------------------------------------------------------------------
// start flying mode
void DontHitMe::startFlight()
{
  m_flightMatrix = m_ballMatrix;
  m_flightOrigin = m_ballOrigin;
}

//--------------------------------------------------------------------
// update saucer position during flying
void DontHitMe::updateFlyingView()
{
  // compute position of eye
  m_eyeMatrix = m_flightMatrix;
  m_eyeMatrix.mapPt(0.0, EYE_OFFSET_Y, EYE_OFFSET_Z, m_eyePt.x, m_eyePt.y, m_eyePt.z);
  m_eyePt.add(m_flightOrigin);

  // eye matrix is inverse transform.  rotate down to center ball in view
  m_eyeMatrix.transpose();
  m_eyeMatrix.rotateXDeg(-22);
}

//--------------------------------------------------------------------
// update position of eye when flying
BOOL DontHitMe::updateFlyingMovement(
  double now,
  double since)
{
  mgPoint3 forward;
  m_flightMatrix.mapPt(0, 0, 1, forward.x, forward.y, forward.z);

  // update position
  BOOL changed = false;

  // update rotate speed
  if (m_turnLeftSince != INVALID_TIME)
  {
    mgMatrix4 rotate;
    rotate.rotateZDeg(m_rotateRate*200.0  * (now - m_turnLeftSince));
    m_flightMatrix.leftMultiply(rotate);

    changed = true;
    m_turnLeftSince = now;
  }

  if (m_turnRightSince != INVALID_TIME)
  {
    mgMatrix4 rotate;
    rotate.rotateZDeg(-m_rotateRate*200.0 * (now - m_turnRightSince));
    m_flightMatrix.leftMultiply(rotate);

    changed = true;
    m_turnRightSince = now;
  }
  
  if (m_forwardSince != INVALID_TIME)
  {
    double dist = m_moveSpeed*(now - m_forwardSince);
    m_flightOrigin.x += dist * forward.x;
    m_flightOrigin.y += dist * forward.y;
    m_flightOrigin.z += dist * forward.z;
    changed = true;
    m_forwardSince = now;
  }

  if (m_backwardSince != INVALID_TIME)
  {
    double dist = m_moveSpeed*(now - m_backwardSince);
    m_flightOrigin.x -= dist * forward.x;
    m_flightOrigin.y -= dist * forward.y;
    m_flightOrigin.z -= dist * forward.z;
    changed = true;
    m_backwardSince = now;
  }

  return changed;
}

//--------------------------------------------------------------------
// update rotation of ball
void DontHitMe::updateBallRotate(
  double since)
{
  // change rotation of ball
  double angle = since * m_rotateSpeed;
  m_ballRotate += angle;
  if (m_ballRotate > 180.0)
    m_ballRotate -= 360.0;
  if (m_ballRotate < -180.0)
    m_ballRotate += 360.0;

  if (m_rotateSpeed > 0)
    m_rotateSpeed = max(0, m_rotateSpeed - m_rotateDrag*since);
  else m_rotateSpeed = min(0, m_rotateSpeed + m_rotateDrag*since);
}

//--------------------------------------------------------------------
// animate the view
BOOL DontHitMe::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  m_saucer->animate(now, since);

  // update move/turn 
  BOOL changed = true;

  if (m_flying)
  {
    updateFlyingMovement(now, since);
    updateFlyingView();
  }
  else 
  {
    updateRollingMovement(now, since);

    // change position of ball
    double dist = since * m_rollSpeed;
    m_ballPosn += dist;
    while (m_ballPosn > m_tube->m_trackLen)
    {
      m_ballPosn -= m_tube->m_trackLen;
    }
    while (m_ballPosn < 0)
    {
      m_ballPosn += m_tube->m_trackLen;
    }

    if (m_rollSpeed > 0)
      m_rollSpeed = max(0, m_rollSpeed - m_rollDrag*since);
    else m_rollSpeed = min(0, m_rollSpeed + m_rollDrag*since);

    updateBallRotate(since);
    updateBallPt();
  }

  return m_eyeChanged || changed;
}

//--------------------------------------------------------------------
// render the view
void DontHitMe::appViewDraw()
{
  // draw the sky
  if (m_sky != NULL)
  {
    mgDisplay->setTransparent(true);
    mgDisplay->setZEnable(false);

    mgDisplay->setEyeMatrix(m_eyeMatrix);
    mgDisplay->setEyePt(mgPoint3(0,0,0));

    mgMatrix4 identity;
    mgDisplay->setModelTransform(identity);

    m_sky->render();

    mgDisplay->setTransparent(false);
    mgDisplay->setZEnable(true);
  }

  mgDisplay->setEyeMatrix(m_eyeMatrix);
  mgDisplay->setEyePt(m_eyePt);

  mgDisplay->setLightColor(0.8, 0.8, 0.8);

  // draw the planet
  if (m_planet != NULL)
    m_planet->render();

  // draw the wreck
  if (m_wreck != NULL)
  {
    mgMatrix4 model;
    model.translate(0, 0, -120);  // center of ship
    model.rotateYDeg(20);
    model.rotateZDeg(10);
    model.translate(215, 0, 30);  // near asteroid
    mgDisplay->setModelTransform(model);

    m_wreck->render();
  }

  // draw the fallen towers
  if (m_tower1 != NULL)
  {
    mgMatrix4 model;
    model.rotateYDeg(-80);
    model.rotateXDeg(10);
    model.rotateZDeg(-60);
    model.translate(190, 40, 40);
    mgDisplay->setModelTransform(model);

    m_tower1->render();
  }

  if (m_tower2 != NULL)
  {
    mgMatrix4 model;
    model.rotateYDeg(90);
    model.rotateXDeg(-5);
    model.rotateZDeg(-105);
    model.translate(205, -20, 20);
    mgDisplay->setModelTransform(model);

    m_tower2->render();
  }

  // draw the prickles
  for (int i = 0; i < m_prickles.length(); i++)
  {
    Prickle* prickle = (Prickle*) m_prickles[i];
    prickle->render();
  }

  if (m_showingIntro)
  {
    if (m_intro != NULL)
      m_intro->render();
  }

  else
  {
    // draw the saucer
    if (m_saucer != NULL)
    {
      double saucerScale = 0.5;
      double ballRadius = 0.5*saucerScale;
      mgMatrix4 ballModel;
      ballModel.scale(saucerScale);
      ballModel.translate(0.0, ballRadius + TUBE_RADIUS, 0.0);
      if (m_flying)
      {
        ballModel.multiply(m_flightMatrix);
        ballModel.translate(m_flightOrigin);
      }
      else
      {
        ballModel.multiply(m_ballMatrix);
        ballModel.translate(m_ballOrigin);
      }
      mgDisplay->setModelTransform(ballModel);
      m_saucer->render();
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
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void DontHitMe::appCreateBuffers()
{
  m_sky->createBuffers();
  m_intro->createBuffers();
  m_tube->createBuffers();
  m_saucer->createBuffers();
  m_planet->createBuffers();
  m_wreck->createBuffers();
  m_tower1->createBuffers();
  m_tower2->createBuffers();

  for (int i = 0; i < m_prickles.length(); i++)
  {
    Prickle* prickle = (Prickle*) m_prickles[i];
    prickle->createBuffers();
  }
}

//-----------------------------------------------------------------------------
// delete any display buffers
void DontHitMe::appDeleteBuffers()
{
  m_intro->deleteBuffers();
  m_tube->deleteBuffers();
  m_sky->deleteBuffers();
  m_saucer->deleteBuffers();
  m_planet->deleteBuffers();
  m_wreck->deleteBuffers();
  m_tower1->deleteBuffers();
  m_tower2->deleteBuffers();

  for (int i = 0; i < m_prickles.length(); i++)
  {
    Prickle* prickle = (Prickle*) m_prickles[i];
    prickle->deleteBuffers();
  }
}

//--------------------------------------------------------------------
// load cursor pattern from options
void DontHitMe::loadCursor()
{
  // load cursor pattern
  mgString fileName;
  try
  {
    m_options.getFileName("deskcursor", m_options.m_sourceFileName, "deskcursor", fileName);
    m_deskCursor = new mgCursorDefn(fileName);
    m_options.getFileName("movecursor", m_options.m_sourceFileName, "movecursor", fileName);
    m_moveCursor = new mgCursorDefn(fileName);
  }
  catch (mgErrorMsg* e)
  {
    delete e;
    throw new mgException("Could not open cursor file '%s'", (const char*) fileName);
  }
}

//--------------------------------------------------------------------
// set cursor and movement style to desktop
void DontHitMe::setDeskMode(
  BOOL deskMode)
{
  m_deskMode = deskMode;
  if (m_deskMode)
  {
    // load desk cursor
    mgDisplay->setCursorTexture(m_deskCursor->m_texture, m_deskCursor->m_hotX, m_deskCursor->m_hotY);

    // track cursor position
    mgDisplay->cursorTrack(true);
  }
  else
  {
    // load movement cursor
    mgDisplay->setCursorTexture(m_moveCursor->m_texture, m_moveCursor->m_hotX, m_moveCursor->m_hotY);

    // don't track cursor
    mgDisplay->cursorTrack(false);
  }
}

//-----------------------------------------------------------------------------
// turn eye from mouse movement
void DontHitMe::turnEye(
  int dx, 
  int dy)
{
  mgMatrix4 rotate;
  rotate.rotateXDeg(dy/25.0);
  rotate.rotateYDeg(dx/25.0);
  m_flightMatrix.leftMultiply(rotate);

  updateFlyingView();
}

//--------------------------------------------------------------------
// set view size
void DontHitMe::appViewResized(
  int width,
  int height)
{
  mgDisplay->setScreenSize(width, height);
}

//-----------------------------------------------------------------------------
// mouse button pressed
void DontHitMe::appMouseDown(
  int pressed,
  int flags)
{
  if (m_deskMode)
  {
    // if both buttons pressed
    if ((pressed == MG_EVENT_MOUSE1_DOWN || pressed == MG_EVENT_MOUSE2_DOWN) &&
        (flags & BOTH_BUTTONS) == BOTH_BUTTONS)
    {
      if (m_forwardSince == INVALID_TIME)
        m_forwardSince = mgOSGetTime();
//      avatarMoveTowardsCamera(true);
    }
  
    // any button pressed, turn off cursor
    mgDisplay->cursorEnable(false);
    mgDisplay->setMouseRelative(true);
  }
}

//-----------------------------------------------------------------------------
// mouse button released
void DontHitMe::appMouseUp(
  int released,
  int flags)
{
  if (m_deskMode)
  {
    int oldFlags = flags | released;  // flags before release

    // if was both buttons (move towards camera) but isn't now
    if ((oldFlags & BOTH_BUTTONS) == BOTH_BUTTONS &&
        (released == MG_EVENT_MOUSE1_DOWN || released == MG_EVENT_MOUSE2_DOWN))
    {
      m_forwardSince = INVALID_TIME;
    }
  
    // if both buttons released, enable cursor
    if ((flags & BOTH_BUTTONS) == 0)
    {
      mgDisplay->cursorEnable(true);
      mgDisplay->setMouseRelative(false);
    }
  }
}

//-----------------------------------------------------------------------------
// mouse moved
void DontHitMe::appMouseMove(
  int dx,
  int dy, 
  int flags)
{
  if (m_deskMode)
  {
    // neither button down, so move cursor
    mgDisplay->cursorMove(dx, dy);
    m_eyeChanged = true;
  }
  else turnEye(dx, dy);
}

//-----------------------------------------------------------------------------
// mouse dragged
void DontHitMe::appMouseDrag(
  int dx,
  int dy, 
  int flags)
{
  if (m_deskMode)
  {
    // if either button down (or both)
    if ((flags & BOTH_BUTTONS) != 0)
    {
      turnEye(dx, dy);
    }
  }
  else appMouseMove(dx, dy, flags);
}

//-----------------------------------------------------------------------------
// mouse wheel rotate
void DontHitMe::appMouseWheel(
  int wheel,
  int flags)
{
}

//-----------------------------------------------------------------------------
// mouse enters window
void DontHitMe::appMouseEnter(
  int x,
  int y)
{
  mgDisplay->cursorSetPosn(x, y);
  mgDisplay->cursorEnable(true);
}

//-----------------------------------------------------------------------------
// mouse leaves window
void DontHitMe::appMouseExit()
{
  mgDisplay->cursorEnable(false);
}

//-----------------------------------------------------------------------------
// key press
void DontHitMe::appKeyDown(
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case MG_EVENT_KEY_ESCAPE: 
      setDeskMode(!m_deskMode);
      break;

    case MG_EVENT_KEY_F1:
//      showHelp();
      if (m_flying)
        mgDebug("  points[pointCount++] = new mgPoint3(%f, %f, %f);", m_flightOrigin.x, m_flightOrigin.y, m_flightOrigin.z);
      else mgDebug("  points[pointCount++] = new mgPoint3(%f, %f, %f);", m_ballOrigin.x, m_ballOrigin.y, m_ballOrigin.z);
      break;

    case MG_EVENT_KEY_F5:
      mgDebug("break");
      break;

    case 'F':
      m_flying = !m_flying;
      if (m_flying)
        startFlight();
      m_eyeChanged = true;
      break;

    case 'W':
    case MG_EVENT_KEY_UP:
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
        if (m_forwardSince == INVALID_TIME)
          m_forwardSince = mgOSGetTime();
      break;

    case 'S':
    case MG_EVENT_KEY_DOWN:
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
      if (m_backwardSince == INVALID_TIME)
        m_backwardSince = mgOSGetTime();
      break;

    case 'A':
    case MG_EVENT_KEY_LEFT:
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
      if (m_turnLeftSince == INVALID_TIME)
        m_turnLeftSince = mgOSGetTime();
      break;

    case 'D':
    case MG_EVENT_KEY_RIGHT:
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
      if (m_turnRightSince == INVALID_TIME)
        m_turnRightSince = mgOSGetTime();
      break;
  }
}

//-----------------------------------------------------------------------------
// key release
void DontHitMe::appKeyUp(
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case 'A':
    case MG_EVENT_KEY_LEFT:
      m_turnLeftSince = INVALID_TIME;
      break;

    case 'D':
    case MG_EVENT_KEY_RIGHT:
      m_turnRightSince = INVALID_TIME;
      break;

    case 'W':
    case MG_EVENT_KEY_UP:
      m_forwardSince = INVALID_TIME;
      break;

    case 'S':
    case MG_EVENT_KEY_DOWN:
      m_backwardSince = INVALID_TIME;
      break;

    case ' ':
    case MG_EVENT_KEY_PAGEUP:
      m_upSince = INVALID_TIME;
      break;

    case 'X':
    case MG_EVENT_KEY_PAGEDN:
      m_downSince = INVALID_TIME;
      break;
  }
}

//-----------------------------------------------------------------------------
// key typed
void DontHitMe::appKeyChar(
  int keyCode,
  int modifiers)
{
}

//-----------------------------------------------------------------------------
// report status for debug log
void DontHitMe::appDebugStats(
  mgString& status)
{
  // format program status into this string.  displayed when graphicsTiming option is true.
}
