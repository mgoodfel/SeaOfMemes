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

#include "MovementApp.h"
#include "mgGUI/Include/mgGUI.h"

const double INVALID_TIME = -1.0;
const int BOTH_BUTTONS = MG_EVENT_MOUSE1_DOWN | MG_EVENT_MOUSE2_DOWN;

//--------------------------------------------------------------
// constructor
MovementApp::MovementApp()
{
  // parse the options file
  m_options.parseFile("options.xml");

  m_ui = NULL;
  m_deskCursor = NULL;
  m_moveCursor = NULL;
}

//--------------------------------------------------------------
// destructor
MovementApp::~MovementApp()
{
}

//--------------------------------------------------------------
// request display parameters
void MovementApp::appRequestDisplay()
{
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
void MovementApp::appInit()
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

  // load cursor pattern
  loadCursor();
  setDeskMode(true);

  initMovement();
  m_lastAnimate = mgOSGetTime();  // now!
}

//--------------------------------------------------------------
// terminate application
void MovementApp::appTerm()
{
  delete m_moveCursor;
  m_moveCursor = NULL;

  delete m_deskCursor;
  m_deskCursor = NULL;

  mgTermDisplayServices();
}

//--------------------------------------------------------------------
// load cursor pattern from options
void MovementApp::loadCursor()
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
void MovementApp::setDeskMode(
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

//--------------------------------------------------------------------
// initialize movement state
void MovementApp::initMovement()
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
  m_moveSpeed = 2.5/1000;               // units per ms // 100.0/1000;

  m_eyePt = mgPoint3(0, 2.5, -3.5);
  m_eyeRotX = 0.0;
  m_eyeRotY = 0.0;
  m_eyeRotZ = 0.0;

  m_eyeChanged = true;
}

//--------------------------------------------------------------------
// update position of eye
BOOL MovementApp::updateMovement(
  double now,
  double since)
{
  // get direction of head and direction of right
  double headX, headY, headZ;
  m_eyeMatrix.invertPt(0.0, 0.0, 1.0, headX, headY, headZ);
  double rightX, rightY, rightZ;
  m_eyeMatrix.invertPt(1.0, 0.0, 0.0, rightX, rightY, rightZ);

  // update position
  BOOL changed = false;

  if (m_forwardSince != INVALID_TIME)
  {
    double dist = m_moveSpeed*(now - m_forwardSince);
    m_eyePt.x += dist * headX;
    m_eyePt.y += dist * headY;
    m_eyePt.z += dist * headZ;
    changed = true;
    m_forwardSince = now;
  }

  if (m_backwardSince != INVALID_TIME)
  {
    double dist = m_moveSpeed*(now - m_backwardSince);
    m_eyePt.x -= dist * headX;
    m_eyePt.y -= dist * headY;
    m_eyePt.z -= dist * headZ;
    changed = true;
    m_backwardSince = now;
  }

  if (m_leftSince != INVALID_TIME)
  {
    double dist = m_moveSpeed*(now - m_leftSince);
    m_eyePt.x -= dist * rightX;
    m_eyePt.y -= dist * rightY;
    m_eyePt.z -= dist * rightZ;
    changed = true;
    m_leftSince = now;
  }

  if (m_rightSince != INVALID_TIME)
  {
    double dist = m_moveSpeed*(now - m_rightSince);
    m_eyePt.x += dist * rightX;
    m_eyePt.y += dist * rightY;
    m_eyePt.z += dist * rightZ;
    changed = true;
    m_rightSince = now;
  }

  if (m_upSince != INVALID_TIME)
  {
    double fDist = m_moveSpeed*(now - m_upSince);
    m_eyePt.y += fDist;
    changed = true;
    m_upSince = now;
  }

  if (m_downSince != INVALID_TIME)
  {
    double fDist = m_moveSpeed*(now - m_downSince);
    m_eyePt.y -= fDist;
    changed = true;
    m_downSince = now;
  }

  // update eye direction
  if (m_turnLeftSince != INVALID_TIME)
  {
    m_eyeRotY += m_turnSpeed * (now - m_turnLeftSince);

    if (m_eyeRotY > 180.0)
      m_eyeRotY -= 360.0;

    changed = true;
    m_turnLeftSince = now;
  }

  if (m_turnRightSince != INVALID_TIME)
  {
    m_eyeRotY -= m_turnSpeed * (now - m_turnRightSince);

    if (m_eyeRotY < -180.0)
      m_eyeRotY += 360.0;

    changed = true;
    m_turnRightSince = now;
  }
  
  return changed;
}

//-----------------------------------------------------------------------------
// turn eye from mouse movement
void MovementApp::turnEye(
  int dx, 
  int dy)
{
  m_eyeRotX -= dy/25.0;
  m_eyeRotY -= dx/25.0;

  // don't allow head to flip over
  m_eyeRotX = min(m_eyeRotX, 90.0);
  m_eyeRotX = max(m_eyeRotX, -90.0);

  // keep direction within range
  if (m_eyeRotY < -180.0)
    m_eyeRotY += 360.0;
  if (m_eyeRotY > 180.0)
    m_eyeRotY -= 360.0;

  m_eyeChanged = true;
}

//--------------------------------------------------------------------
// set view size
void MovementApp::appViewResized(
  int width,
  int height)
{
  mgDisplay->setScreenSize(width, height);
  if (m_ui != NULL)
    m_ui->viewResized(width, height);
}

//--------------------------------------------------------------------
// animate the view
BOOL MovementApp::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  // construct eye matrix
  m_eyeMatrix.loadIdentity();
  m_eyeMatrix.rotateZDeg(m_eyeRotZ);
  m_eyeMatrix.rotateYDeg(m_eyeRotY);
  m_eyeMatrix.rotateXDeg(m_eyeRotX);

  // update move/turn 
  BOOL changed = updateMovement(now, since);
  changed |= m_eyeChanged;
  m_eyeChanged = false;

  if (m_ui != NULL)
    changed |= m_ui->animate(now, since);

  return changed;
}

//--------------------------------------------------------------------
// render the view
void MovementApp::appViewDraw()
{
}

//-----------------------------------------------------------------------------
// handle idle time
void MovementApp::appIdle()
{
  double now = mgOSGetTime();
  appViewAnimate(now, now - m_lastAnimate);
  m_lastAnimate = now;

  mgDisplay->clearView();
  appViewDraw();

  if (m_ui != NULL)
    m_ui->render();

  mgDisplay->drawCursor();
  mgDisplay->swapBuffers();
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void MovementApp::appCreateBuffers()
{
}

//-----------------------------------------------------------------------------
// delete any display buffers
void MovementApp::appDeleteBuffers()
{
}

//--------------------------------------------------------------------
// set overlay UI
void MovementApp::setUI(
  MovementUI* ui)
{
  m_ui = ui;
}

//-----------------------------------------------------------------------------
// mouse button pressed
void MovementApp::appMouseDown(
  int pressed,
  int flags)
{
  if (m_deskMode)
  {
    // send the press to the UI if it wants it
    int x, y;
    mgDisplay->getCursorPosn(x, y);
    if (m_ui != NULL && m_ui->usesMousePoint(x, y))
    {
      m_ui->getTop()->surfaceMouseDown(x, y, flags, pressed);
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
  if (m_ui != NULL)
    m_ui->resetFocus();
}

//-----------------------------------------------------------------------------
// mouse button released
void MovementApp::appMouseUp(
  int released,
  int flags)
{
  if (m_deskMode)
  {
    int x, y;
    mgDisplay->getCursorPosn(x, y);

    // if ui wants mouse events, send it
    if (m_ui != NULL && (m_ui->hasMouseFocus() || m_ui->usesMousePoint(x, y)))
    {
      m_ui->getTop()->surfaceMouseUp(x, y, flags, released);
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
void MovementApp::appMouseMove(
  int dx,
  int dy, 
  int flags)
{
  if (m_deskMode)
  {
    // neither button down, so move cursor
    mgDisplay->cursorMove(dx, dy);

    int x, y;
    mgDisplay->getCursorPosn(x, y);

    // if ui wants mouse events, send it
    if (m_ui != NULL && (m_ui->hasMouseFocus() || m_ui->usesMousePoint(x, y)))
      m_ui->getTop()->surfaceMouseMove(x, y, flags);
  }
  else turnEye(dx, dy);
}

//-----------------------------------------------------------------------------
// mouse dragged
void MovementApp::appMouseDrag(
  int dx,
  int dy, 
  int flags)
{
  if (m_deskMode)
  {
    // neither button down, so move cursor
    mgDisplay->cursorMove(dx, dy);

    int x, y;
    mgDisplay->getCursorPosn(x, y);
    // if ui wants mouse events, send it
    if (m_ui != NULL && m_ui->hasMouseFocus())
    {
      m_ui->getTop()->surfaceMouseDrag(x, y, flags);
      return;
    }

    // if either button down (or both)
    if ((flags & BOTH_BUTTONS) != 0)
      turnEye(dx, dy);
  }
  else appMouseMove(dx, dy, flags);
}

//-----------------------------------------------------------------------------
// mouse wheel rotate
void MovementApp::appMouseWheel(
  int wheel,
  int flags)
{
}

//-----------------------------------------------------------------------------
// mouse enters window
void MovementApp::appMouseEnter(
  int x,
  int y)
{
  mgDisplay->cursorSetPosn(x, y);
  mgDisplay->cursorEnable(true);
}

//-----------------------------------------------------------------------------
// mouse leaves window
void MovementApp::appMouseExit()
{
  mgDisplay->cursorEnable(false);
}

//-----------------------------------------------------------------------------
// key press
void MovementApp::appKeyDown(
  int keyCode,
  int modifiers)
{
  // process subclass key handling
  if (moveKeyDown(keyCode, modifiers))
    return;

  // if UI open, give it keys
  if (m_ui != NULL && m_ui->hasKeyFocus())
  {
    m_ui->getTop()->surfaceKeyDown(keyCode, modifiers);
    return;
  }

  // process default MovementApp keys
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
void MovementApp::appKeyUp(
  int keyCode,
  int modifiers)
{
  // process subclass key handling
  if (moveKeyUp(keyCode, modifiers))
    return;

  // if UI open, give it keys
  if (m_ui != NULL && m_ui->hasKeyFocus())
  {
    m_ui->getTop()->surfaceKeyUp(keyCode, modifiers);
    return;
  }

  // process movementApp key handling
  switch (keyCode)
  {
    case MG_EVENT_KEY_LEFT:
      m_turnLeftSince = INVALID_TIME;
      break;

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

    case 'A':
      m_leftSince = INVALID_TIME;
      break;

    case 'D':
      m_rightSince = INVALID_TIME;
      break;
  }
}

//-----------------------------------------------------------------------------
// character typed
void MovementApp::appKeyChar(
  int keyCode,
  int modifiers)
{
  // if UI open, give it character
  if (m_ui != NULL && m_ui->hasKeyFocus())
  {
    m_ui->getTop()->surfaceKeyChar(keyCode, modifiers);
    return;
  }

  // process subclass character handling
  moveKeyChar(keyCode, modifiers);
}

//-----------------------------------------------------------------------------
// report status for debug log
void MovementApp::appDebugStats(
  mgString& status)
{
  // format program status into this string.  displayed when graphicsTiming option is true.
}

//-----------------------------------------------------------------------------
// intro text
void MovementApp::debugIntro(
  mgString& text) 
{
  text = "Type 'help' for commands.";
}

//-----------------------------------------------------------------------------
// help text
void MovementApp::debugHelp(
  mgString& text) 
{
  text =  "VarName*          // prints variable(s)\n";
  text += "VarName = value   // sets variable\n";
  text += "Function(value)   // calls function\n";
  text += "list              // list variables/functions\n";
}

//-----------------------------------------------------------------------------
// describe variables. 
void MovementApp::debugListVariables(
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
}

//-----------------------------------------------------------------------------
// describe functions.  
void MovementApp::debugListFunctions(
  mgStringArray& funcNames,
  mgStringArray& funcParms,
  mgStringArray& helpText) 
{
}

//-----------------------------------------------------------------------------
// return value of variable
void MovementApp::debugGetVariable(
  const char* varName,
  mgString& value) 
{
  if (_stricmp(varName, "eyePt.x") == 0)
    value.format("%g", m_eyePt.x);
  else if (_stricmp(varName, "eyePt.y") == 0)
    value.format("%g", m_eyePt.y);
  else if (_stricmp(varName, "eyePt.z") == 0)
    value.format("%g", m_eyePt.z);

  else if (_stricmp(varName, "eyeRot.x") == 0)
    value.format("%g", m_eyeRotX);
  else if (_stricmp(varName, "eyeRot.y") == 0)
    value.format("%g", m_eyeRotY);
  else if (_stricmp(varName, "eyeRot.z") == 0)
    value.format("%g", m_eyeRotZ);

  else value = "unknown variable";
}

//-----------------------------------------------------------------------------
// set a variable
void MovementApp::debugSetVariable(
  const char* varName,
  const char* value,
  mgString& reply) 
{
  BOOL error = false;
  if (_stricmp(varName, "eyePt.x") == 0)
  {
    if (sscanf(value, "%lf", &m_eyePt.x) == 1)
      m_eyeChanged = true;
    else error = true;
  }
  else if (_stricmp(varName, "eyePt.y") == 0)
  {
    if (sscanf(value, "%lf", &m_eyePt.y) == 1)
      m_eyeChanged = true;
    else error = true;
  }
  else if (_stricmp(varName, "eyePt.z") == 0)
  {
    if (sscanf(value, "%lf", &m_eyePt.z) == 1)
      m_eyeChanged = true;
    else error = true;
  }

  else if (_stricmp(varName, "eyeRot.x") == 0)
  {
    if (sscanf(value, "%lf", &m_eyeRotX) == 1)
      m_eyeChanged = true;
    else error = true;
  }
  else if (_stricmp(varName, "eyeRot.y") == 0)
  {
    if (sscanf(value, "%lf", &m_eyeRotY) == 1)
      m_eyeChanged = true;
    else error = true;
  }
  else if (_stricmp(varName, "eyeRot.z") == 0)
  {
    if (sscanf(value, "%lf", &m_eyeRotZ) == 1)
      m_eyeChanged = true;
    else error = true;
  }

  if (error)
    reply.format("Could not read %s", value);
  else reply = "ok";
}

//-----------------------------------------------------------------------------
// call a function
void MovementApp::debugCallFunction(
  const char* funcName,
  mgStringArray& args,
  mgString& reply) 
{
}
