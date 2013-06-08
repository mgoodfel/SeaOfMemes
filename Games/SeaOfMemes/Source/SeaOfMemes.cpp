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
const char* mgProgramName = "SeaOfMemes";
const char* mgProgramVersion = "Part 83";

#include "SolarSystem/NebulaSky.h"
#include "SolarSystem/SolarSystem.h"
#include "GUI/HelpUI.h"
#include "GUI/SpeedUI.h"

#include "SeaOfMemes.h"

const double INVALID_TIME = -1.0;
const int BOTH_BUTTONS = MG_EVENT_MOUSE1_DOWN | MG_EVENT_MOUSE2_DOWN;

const double MIN_AVATAR_SPEED = 2.5/1000.0;   // units per ms
const double MAX_AVATAR_SPEED = 128*2500.0/1000.0;   // units per ms

const double LANDING_DISTANCE = 300000.0;

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set the working directory to dir containing options
  mgOSFindWD("options.xml");

  return new SeaOfMemes();
}

//--------------------------------------------------------------
// constructor
SeaOfMemes::SeaOfMemes()
{
  m_moveCursor = NULL;
  m_deskCursor = NULL;
  m_sky = NULL;
  m_world = NULL;
  m_helpUI = NULL;
  m_speedUI = NULL;
  m_alertFont = NULL;

  // read the options.  if fails, exception will be thrown and caught by framework.
  m_options.parseFile("options.xml");
}

//--------------------------------------------------------------
// destructor
SeaOfMemes::~SeaOfMemes()
{
}

//--------------------------------------------------------------
// request display parameters
void SeaOfMemes::appRequestDisplay()
{
  mgString title;
  title.format("SeaOfMemes, %s", mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // set all the window parameters
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
void SeaOfMemes::appInit()
{
  // get the important directories
  mgString shaderDir;
  m_options.getFileName("shaderDir", m_options.m_sourceFileName, "docs/shaders", shaderDir);
  mgString uiDir;
  m_options.getFileName("uiDir", m_options.m_sourceFileName, "docs/ui", uiDir);
  mgString fontDir;
  m_options.getFileName("fontDir", m_options.m_sourceFileName, "docs/fonts", fontDir);

  mgInitDisplayServices(shaderDir, fontDir);

  mgDisplay->setDPI(m_options.getInteger("dpi", 0));
  mgDisplay->setFOV(m_options.getDouble("FOV", 60.0));

  // load cursors
  loadCursor();

  setDeskMode(true);

  // take units from option
  m_unitsMetric = m_options.getBoolean("metricUnits", true);

  // create the sky
  m_matColor = mgPoint3(1, 1, 1);
  m_lightDir = mgPoint3(1.0, 0.25, 0.0);
  m_lightDir.normalize();

  m_sky = new NebulaSky(m_options);
  m_sky->setSunDir(m_lightDir);

  m_world = new SolarSystem(m_options);

  m_helpUI = new HelpUI(m_options);
  m_helpUI->setDebugApp(this);

  m_speedUI = new SpeedUI(m_options);
  m_speedUI->setUnits(m_unitsMetric ? UNITS_KM : UNITS_MILES);

  appCreateBuffers();

  // initialize movement
  initMovement();
  m_lastAnimate = 0.0;
}

//--------------------------------------------------------------
// terminate application
void SeaOfMemes::appTerm()
{
  delete m_deskCursor;
  m_deskCursor = NULL;

  delete m_moveCursor;
  m_moveCursor = NULL;

  delete m_helpUI;
  m_helpUI = NULL;

  delete m_speedUI;
  m_speedUI = NULL;

  delete m_sky;
  m_sky = NULL;

  delete m_world;
  m_world = NULL;

  mgBlockPool::freeMemory();

  mgTermDisplayServices();
}

//--------------------------------------------------------------------
// load cursor pattern from options
void SeaOfMemes::loadCursor()
{
  // load cursor pattern
  mgString fileName;
  try
  {
    m_options.getFileName("deskcursor", m_options.m_sourceFileName, "", fileName);
    m_deskCursor = new mgCursorDefn(fileName);
    m_options.getFileName("movecursor", m_options.m_sourceFileName, "", fileName);
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
void SeaOfMemes::setDeskMode(
  BOOL deskMode)
{
  m_deskMode = deskMode;
  if (m_deskMode)
  {
    // load mmo cursor
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

//--------------------------------------------------------------------
// initialize movement state
void SeaOfMemes::initMovement()
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
  m_landingMode = false;
  m_targetSpeed = MAX_AVATAR_SPEED;
  m_avatarSpeed = m_targetSpeed;

  m_world->setCoordSystem(COORDS_SPACE);
  m_world->setCoordPosn(mgPoint3(PLANET_RADIUS*1.2, 0.0, -PLANET_RADIUS/2));

//  m_world->setCoordSystem(COORDS_RING);
//  m_world->setCoordPosn(mgPoint3(RING_RADIUS-1000, 0.0, 0.0));

//  m_world->setCoordSystem(COORDS_PLANET);
//  double angle = 45*PI/180;
//  double ht = PLANET_RADIUS+1000.0;
//  m_world->setCoordPosn(mgPoint3(ht*sin(angle), ht*cos(angle), 0.0));

//  m_world->setCoordSystem(COORDS_MOON);
//  double angle = 45*PI/180;
//  double ht = MOON_RADIUS+1000.0;
//  m_world->setCoordPosn(mgPoint3(ht*sin(angle), ht*cos(angle), 0.0));

  m_world->setEyeAngle(0.0, 0.0);

  if (m_speedUI != NULL)
  {
    m_speedUI->setSpeed(m_avatarSpeed);
    m_speedUI->setLanding(false);
  }

  m_eyeChanged = true;
}

//--------------------------------------------------------------------
// set landing mode
void SeaOfMemes::setLandingMode(
  BOOL landingMode)
{
  if (m_landingMode == landingMode)
    return;  // nothing to do

  // if already in landing mode
  if (m_landingMode)
  {
    // switch to space speed
    m_landingMode = false;
    m_world->changeCoords(COORDS_SPACE);
    return;
  }

  int obj;
  double dist = m_world->nearestObject(obj);
  if (dist > LANDING_DISTANCE)
  {
    switch (obj)
    {
      case COORDS_PLANET:
        addAlert("Too far from planet -- %g km", dist/1000);
        break;
      
      case COORDS_MOON:
        addAlert("Too far from moon -- %g km", dist/1000);
        break;
      
      case COORDS_RING:
        addAlert("Too far from ring -- %g km", dist/1000);
        break;
    }
    return;  // no change to landing mode
  }

  // switch coordinate system to nearest object
  m_landingMode = true;
  m_world->changeCoords(obj);
}

//--------------------------------------------------------------
// add alert message to the text pane
void SeaOfMemes::addAlert(
  const char* format,
  ...)
{
/*
  if (m_textPane == NULL)
    return;

  va_list args;
  va_start(args, format);
  mgString message;
  message.formatV(format, args);

  m_textPane->addMessage(m_alertColor, m_alertFont, message);
*/
}

//--------------------------------------------------------------
// set units in UI
void SeaOfMemes::setUnits(
  BOOL metric)
{
  m_unitsMetric = metric;
  if (m_speedUI != NULL)
    m_speedUI->setUnits(m_unitsMetric ? UNITS_KM : UNITS_MILES);
}

//--------------------------------------------------------------------
// set view size
void SeaOfMemes::appViewResized(
  int width,
  int height)
{
  mgDisplay->setScreenSize(width, height);

  if (m_helpUI != NULL)
    m_helpUI->viewResized(width, height);

  if (m_speedUI != NULL)
    m_speedUI->viewResized(width, height);

  m_eyeChanged = true;
}

//-----------------------------------------------------------------------------
// handle idle time
void SeaOfMemes::appIdle()
{
  double now = mgOSGetTime();
  appViewAnimate(now, now - m_lastAnimate);
  m_lastAnimate = now;

  mgDisplay->clearView();
  appViewDraw();

  if (m_helpUI != NULL)
    m_helpUI->render();

  if (m_speedUI != NULL)
    m_speedUI->render();

  mgDisplay->drawCursor();
  mgDisplay->swapBuffers();
}

//--------------------------------------------------------------------
// animate the view
BOOL SeaOfMemes::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  // update position
  BOOL changed = false;

  if (m_forwardSince != INVALID_TIME)
  {
    double dist = m_avatarSpeed*(now - m_forwardSince);
    m_world->moveEye(dist, 0.0, 0.0);
    changed = true;
    m_forwardSince = now;
  }

  if (m_backwardSince != INVALID_TIME)
  {
    double dist = m_avatarSpeed*(now - m_backwardSince);
    m_world->moveEye(-dist, 0.0, 0.0);
    changed = true;
    m_backwardSince = now;
  }

  if (m_leftSince != INVALID_TIME)
  {
    double dist = m_avatarSpeed*(now - m_leftSince);
    m_world->moveEye(0.0, -dist, 0.0);
    changed = true;
    m_leftSince = now;
  }

  if (m_rightSince != INVALID_TIME)
  {
    double dist = m_avatarSpeed*(now - m_rightSince);
    m_world->moveEye(0.0, dist, 0.0);
    changed = true;
    m_rightSince = now;
  }

  if (m_upSince != INVALID_TIME)
  {
    double dist = m_avatarSpeed*(now - m_upSince);
    m_world->moveEye(0.0, 0.0, dist);
    changed = true;
    m_upSince = now;
  }

  if (m_downSince != INVALID_TIME)
  {
    double dist = m_avatarSpeed*(now - m_downSince);
    m_world->moveEye(0.0, 0.0, -dist);
    changed = true;
    m_downSince = now;
  }

  // update eye direction
  if (m_turnLeftSince != INVALID_TIME)
  {
    double dist = m_turnSpeed * (now - m_turnLeftSince);
    m_world->turnEye(0.0, dist);

    changed = true;
    m_turnLeftSince = now;
  }

  if (m_turnRightSince != INVALID_TIME)
  {
    double dist = m_turnSpeed * (now - m_turnRightSince);
    m_world->turnEye(0.0, -dist);

    changed = true;
    m_turnRightSince = now;
  }
  
  // mark changed if mouse moved
  changed |= m_eyeChanged;
  m_eyeChanged = false;

  // animate object
  changed |= m_sky->animate(now, since);

  // animate the world
  changed |= m_world->animate(now, since);

  // animate the help UI
  changed |= m_helpUI->animate(now, since);

  if (m_speedUI != NULL)
  {
    int obj;
    if (m_landingMode)
      m_speedUI->setLanding(LANDING_ON);
    else 
    {
      double range = m_world->nearestObject(obj);
      m_speedUI->setRange(range);
      if (range < LANDING_DISTANCE)
        m_speedUI->setLanding(LANDING_RANGE);
      else m_speedUI->setLanding(LANDING_NONE);
    }

    // animate the speed UI
    changed |= m_speedUI->animate(now, since);
  }

  return changed;
}

//--------------------------------------------------------------------
// render the view
void SeaOfMemes::appViewDraw()
{
  // draw the sky
  mgDisplay->setFrontAndBack(0.25, 16384);

  mgPoint3 eyePt;
  m_world->getEyePt(eyePt);
  mgMatrix4 eyeMatrix;
  m_world->getEyeMatrix(eyeMatrix);

  mgDisplay->setEyeMatrix(eyeMatrix);
  mgDisplay->setEyePt(mgPoint3(0,0,0));

  mgMatrix4 identity;
  mgDisplay->setModelTransform(identity);
  mgDisplay->setCulling(true);

  m_sky->render();

  m_world->setSunDir(m_lightDir);

  m_world->render();
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void SeaOfMemes::appCreateBuffers()
{
  m_sky->createBuffers();

  m_world->createBuffers();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void SeaOfMemes::appDeleteBuffers()
{
  m_sky->deleteBuffers();

  m_world->deleteBuffers();
}

//-----------------------------------------------------------------------------
// mouse button pressed
void SeaOfMemes::appMouseDown(
  int pressed,
  int flags)
{
  if (m_deskMode)
  {
    // send the press to the UI if it wants it
    int x, y;
    mgDisplay->getCursorPosn(x, y);
    if (m_helpUI != NULL && m_helpUI->usesMousePoint(x, y))
    {
      m_helpUI->getTop()->surfaceMouseDown(x, y, flags, pressed);
      return;
    }

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

  // if ui didn't take press, lose any key focus
  if (m_helpUI != NULL)
    m_helpUI->resetFocus();
}

//-----------------------------------------------------------------------------
// mouse button released
void SeaOfMemes::appMouseUp(
  int released,
  int flags)
{
  if (m_deskMode)
  {
    int x, y;
    mgDisplay->getCursorPosn(x, y);

    // if ui wants mouse events, send it
    if (m_helpUI != NULL && (m_helpUI->hasMouseFocus() || m_helpUI->usesMousePoint(x, y)))
    {
      m_helpUI->getTop()->surfaceMouseUp(x, y, flags, released);
      return;
    }

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
void SeaOfMemes::appMouseMove(
  int dx,
  int dy, 
  int flags)
{
  if (m_deskMode)
  {
    // neither button down, so move cursor
    mgDisplay->cursorMove(dx, dy);
    m_eyeChanged = true;

    int x, y;
    mgDisplay->getCursorPosn(x, y);

    // if ui wants mouse events, send it
    if (m_helpUI != NULL && (m_helpUI->hasMouseFocus() || m_helpUI->usesMousePoint(x, y)))
      m_helpUI->getTop()->surfaceMouseMove(x, y, flags);
  }
  else
  {
    m_world->turnEye(-dy/25.0, -dx/25.0);

    m_eyeChanged = true;
  }
}

//-----------------------------------------------------------------------------
// mouse dragged
void SeaOfMemes::appMouseDrag(
  int dx,
  int dy, 
  int flags)
{
  if (m_deskMode)
  {
    mgDisplay->cursorMove(dx, dy);

    int x, y;
    mgDisplay->getCursorPosn(x, y);
    // if ui wants mouse events, send it
    if (m_helpUI != NULL && m_helpUI->hasMouseFocus())
    {
      m_helpUI->getTop()->surfaceMouseDrag(x, y, flags);
      return;
    }

    // if either button down (or both)
    if ((flags & BOTH_BUTTONS) != 0)
      m_world->turnEye(-dy/25.0, -dx/25.0);
  }
  else appMouseMove(dx, dy, flags);
}

//-----------------------------------------------------------------------------
// mouse wheel rotate
void SeaOfMemes::appMouseWheel(
  int wheel,
  int flags)
{
}

//-----------------------------------------------------------------------------
// mouse enters window
void SeaOfMemes::appMouseEnter(
  int x,
  int y)
{
  mgDisplay->cursorSetPosn(x, y);
  mgDisplay->cursorEnable(true);
}

//-----------------------------------------------------------------------------
// mouse leaves window
void SeaOfMemes::appMouseExit()
{
  mgDisplay->cursorEnable(false);
}

//-----------------------------------------------------------------------------
// key press
void SeaOfMemes::appKeyDown(
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case MG_EVENT_KEY_F1:
      if (m_helpUI != NULL)
        m_helpUI->toggleHelp();
      break;

    case MG_EVENT_KEY_F2:
      if (m_helpUI != NULL)
        m_helpUI->toggleConsole();
      break;

    case MG_EVENT_KEY_F6:
    {
      mgDebug("debug");
      break;
    }
  }

  // if UI open, give it keys
  if (m_helpUI != NULL && m_helpUI->hasKeyFocus())
  {
    m_helpUI->getTop()->surfaceKeyDown(keyCode, modifiers);
    return;
  }

  switch (keyCode)
  {
    case MG_EVENT_KEY_ESCAPE: 
      setDeskMode(!m_deskMode);
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
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
      if (m_leftSince == INVALID_TIME)
        m_leftSince = mgOSGetTime();
      break;

    case 'D':
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
      if (m_rightSince == INVALID_TIME)
        m_rightSince = mgOSGetTime();
      break;

    case MG_EVENT_KEY_PAGEUP:
    case ' ':
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
      if (m_upSince == INVALID_TIME)
        m_upSince = mgOSGetTime();
      break;

    case MG_EVENT_KEY_PAGEDN:
    case 'X':
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
      if (m_downSince == INVALID_TIME)
        m_downSince = mgOSGetTime();
      break;

    case MG_EVENT_KEY_LEFT:
      // ignore autorepeat keys
      if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
        return;
          
      if (m_turnLeftSince == INVALID_TIME)
        m_turnLeftSince = mgOSGetTime();
      break;

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
void SeaOfMemes::appKeyUp(
  int keyCode,
  int modifiers)
{
  // if UI open, give it keys
  if (m_helpUI != NULL && m_helpUI->hasKeyFocus())
  {
    m_helpUI->getTop()->surfaceKeyUp(keyCode, modifiers);
    return;
  }

  switch (keyCode)
  {
    case MG_EVENT_KEY_LEFT:
      m_turnLeftSince = INVALID_TIME;
      break;

    case MG_EVENT_KEY_RIGHT:
      m_turnRightSince = INVALID_TIME;
      break;

    case MG_EVENT_KEY_UP:
    case 'W':
      m_forwardSince = INVALID_TIME;
      break;

    case MG_EVENT_KEY_DOWN:
    case 'S':
      m_backwardSince = INVALID_TIME;
      break;

    case MG_EVENT_KEY_PAGEUP:
    case ' ':
      m_upSince = INVALID_TIME;
      break;

    case MG_EVENT_KEY_PAGEDN:
    case 'X':
      m_downSince = INVALID_TIME;
      break;

    case 'A':
      m_leftSince = INVALID_TIME;
      break;

    case 'D':
      m_rightSince = INVALID_TIME;
      break;
  }
}

//-----------------------------------------------------------------------------
// key typed
void SeaOfMemes::appKeyChar(
  int keyCode,
  int modifiers)
{
  // if UI open, give it character
  if (m_helpUI != NULL && m_helpUI->hasKeyFocus())
  {
    m_helpUI->getTop()->surfaceKeyChar(keyCode, modifiers);
    return;
  }

  switch (keyCode)
  {
    case 'L':
    case 'l':
      setLandingMode(!m_landingMode);
      break;

    case 'K':
    case 'k':
      setUnits(!m_unitsMetric);
      break;

    case '=': 
    case '+':
    {
      m_avatarSpeed = m_avatarSpeed * 2.0;
      m_avatarSpeed = min(MAX_AVATAR_SPEED, m_avatarSpeed);
      if (m_speedUI != NULL)
        m_speedUI->setSpeed(m_avatarSpeed);
/*
      if (m_landingMode)
        m_avatarSpeed = min(MAX_LAND_SPEED, m_avatarSpeed);
      else m_avatarSpeed = min(MAX_SPACE_SPEED, m_avatarSpeed);
      showSpeed();
*/
      break;
    }
      
    case '-':
    {
      m_avatarSpeed = m_avatarSpeed / 2.0;
      m_avatarSpeed = max(MIN_AVATAR_SPEED, m_avatarSpeed);
      if (m_speedUI != NULL)
        m_speedUI->setSpeed(m_avatarSpeed);
/*
      if (m_landingMode)
        m_avatarSpeed = max(MIN_LAND_SPEED, m_avatarSpeed);
      else m_avatarSpeed = max(MIN_SPACE_SPEED, m_avatarSpeed);
      showSpeed();
*/
      break;
    }
  }
}

//--------------------------------------------------------------------
// report stats to mgDebug
void SeaOfMemes::appDebugStats(
  mgString& status)
{
}

//-----------------------------------------------------------------------------
// intro text
void SeaOfMemes::debugIntro(
  mgString& text) 
{
  text = "Type 'help' for commands.";
}

//-----------------------------------------------------------------------------
// help text
void SeaOfMemes::debugHelp(
  mgString& text) 
{
  text =  "VarName*          // prints variable(s)\n";
  text += "VarName = value   // sets variable\n";
  text += "Function(value)   // calls function\n";
  text += "list              // list variables/functions\n";
}

//-----------------------------------------------------------------------------
// describe variables. 
void SeaOfMemes::debugListVariables(
  mgStringArray& varNames,
  mgStringArray& helpText) 
{
  varNames.add("eyePt.x");
  helpText.add("Eye x coordinate");

  varNames.add("eyePt.y");
  helpText.add("Eye y coordinate");

  varNames.add("eyePt.z");
  helpText.add("Eye z coordinate");

  varNames.add("eyeRot.x");
  helpText.add("Eye x rotation");

  varNames.add("eyeRot.y");
  helpText.add("Eye y rotation");

  varNames.add("eyeRot.z");
  helpText.add("Eye z rotation");

  varNames.add("planetDayLen");
  helpText.add("Planet rotation time (seconds)");

  varNames.add("moonDayLen");
  helpText.add("Moon rotation time (seconds)");

  varNames.add("ringDayLen");
  helpText.add("Ringworld rotation time (seconds)");

  varNames.add("moonMonthLen");
  helpText.add("Moon orbit time (seconds)");

  varNames.add("beltMonthLen");
  helpText.add("Belt orbit time (seconds)");
}

//-----------------------------------------------------------------------------
// describe functions.  
void SeaOfMemes::debugListFunctions(
  mgStringArray& funcNames,
  mgStringArray& funcParms,
  mgStringArray& helpText) 
{
}

//-----------------------------------------------------------------------------
// return value of variable
void SeaOfMemes::debugGetVariable(
  const char* varName,
  mgString& value) 
{
  if (m_world == NULL)
  {
    value = "unknown";
    return;
  }

  mgPoint3 eyePt;
  m_world->getEyePt(eyePt);
  mgPoint3 eyeRot;
  m_world->getEyeRot(eyeRot);

  if (_stricmp(varName, "eyePt.x") == 0)
    value.format("%g", eyePt.x);
  else if (_stricmp(varName, "eyePt.y") == 0)
    value.format("%g", eyePt.y);
  else if (_stricmp(varName, "eyePt.z") == 0)
    value.format("%g", eyePt.z);

  else if (_stricmp(varName, "eyeRot.x") == 0)
    value.format("%g", eyeRot.x);
  else if (_stricmp(varName, "eyeRot.y") == 0)
    value.format("%g", eyeRot.y);
  else if (_stricmp(varName, "eyeRot.z") == 0)
    value.format("%g", eyeRot.z);

  else if (_stricmp(varName, "planetDayLen") == 0)
    value.format("%g", m_world->m_planetDayLen);
  else if (_stricmp(varName, "moonDayLen") == 0)
    value.format("%g", m_world->m_moonDayLen);
  else if (_stricmp(varName, "ringDayLen") == 0)
    value.format("%g", m_world->m_ringDayLen);
  else if (_stricmp(varName, "moonMonthLen") == 0)
    value.format("%g", m_world->m_moonMonthLen);
  else if (_stricmp(varName, "beltMonthLen") == 0)
    value.format("%g", m_world->m_beltMonthLen);

  else value = "unknown variable";
}

//-----------------------------------------------------------------------------
// set a variable
void SeaOfMemes::debugSetVariable(
  const char* varName,
  const char* value,
  mgString& reply) 
{
  if (m_world == NULL)
  {
    reply.format("cannot set %s", varName);
    return;
  }

  double len;
  if (1 != sscanf(value, "%lf", &len))
  {
    reply.format("invalid value %s", value);
    return;
  }
  // keep rotation times in range
  len = max(1, len);

  reply = "ok";

  if (_stricmp(varName, "planetDayLen") == 0)
    m_world->m_planetDayLen = len;
  else if (_stricmp(varName, "moonDayLen") == 0)
    m_world->m_moonDayLen = len;
  else if (_stricmp(varName, "ringDayLen") == 0)
    m_world->m_ringDayLen = len;
  else if (_stricmp(varName, "moonMonthLen") == 0)
    m_world->m_moonMonthLen = len;
  else if (_stricmp(varName, "beltMonthLen") == 0)
    m_world->m_beltMonthLen = len;
  
  else reply.format("cannot set %s" ,varName);
}

//-----------------------------------------------------------------------------
// call a function
void SeaOfMemes::debugCallFunction(
  const char* funcName,
  mgStringArray& args,
  mgString& reply) 
{
}
