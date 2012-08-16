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

// identify the program for the framework log
const char* mgProgram = "GuiTestAll";
const char* mgProgramVersion = "Part 60";

#include "mgGUI/Include/mgGUI.h"
#include "SampleUI.h"
#include "GuiTestAll.h"

const double INVALID_TIME = -1.0;
const int BOTH_BUTTONS = MG_EVENT_MOUSE1_DOWN | MG_EVENT_MOUSE2_DOWN;

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set the working directory to dir containing options
  mgOSFindWD("options.xml");

  return new GuiTestAll();
}

//--------------------------------------------------------------
// constructor
GuiTestAll::GuiTestAll()
{
  m_cubeTexture = NULL;
  m_cubeIndexes = NULL;
  m_cubeVertexes = NULL;
  m_floorTexture = NULL;
  m_floorVertexes = NULL;
  m_surface = NULL;
  m_ui = NULL;

  // parse the options file
  m_options.parseFile("options.xml");

  m_deskCursor = NULL;
  m_moveCursor = NULL;
}

//--------------------------------------------------------------
// destructor
GuiTestAll::~GuiTestAll()
{
}

//--------------------------------------------------------------------
// initialize application
void GuiTestAll::appInit()
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

  mgString title;
  title.format("%s, %s", mgProgram, mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // load the shaders we use
  mgVertex::loadShader("litTexture");
  mgVertexTA::loadShader("litTextureArray");

  // load cursor pattern
  loadCursor();
  setDeskMode(true);

  initMovement();
  m_lastAnimate = mgOSGetTime();  // now!

  // rotation angle for cube
  m_angle = 0.0;

  // load texture patterns
  loadTextures();

  // create vertex and index buffers
  m_cubeIndexes = NULL;
  m_cubeVertexes = NULL;
  m_floorVertexes = NULL;

  m_surface = mgDisplay->createOverlaySurface();

  mgString fileName;
  m_options.getFileName("helpFile", m_options.m_sourceFileName, "help.xml", fileName);
  m_ui = new SampleUI(m_surface, fileName);
}

//--------------------------------------------------------------------
// terminate application
void GuiTestAll::appTerm()
{
  delete m_moveCursor;
  m_moveCursor = NULL;

  delete m_deskCursor;
  m_deskCursor = NULL;

  // delete the overlay UI
  delete m_ui;
  m_ui = NULL;

  delete m_surface;
  m_surface = NULL;

  mgTermDisplayServices();
}

//--------------------------------------------------------------
// request display parameters
void GuiTestAll::appRequestDisplay()
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

//--------------------------------------------------------------------
// load cursor pattern from options
void GuiTestAll::loadCursor()
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
void GuiTestAll::setDeskMode(
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
void GuiTestAll::initMovement()
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
BOOL GuiTestAll::updateMovement(
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
void GuiTestAll::turnEye(
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
// load texture patterns from options
void GuiTestAll::loadTextures()
{
  mgStringArray faceNames;
  mgString fileName;

  // load cube face textures.  must all be same size.
  m_options.getFileName("xminface", m_options.m_sourceFileName, "face-xmin.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("xmaxface", m_options.m_sourceFileName, "face-xmax.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("yminface", m_options.m_sourceFileName, "face-ymin.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("ymaxface", m_options.m_sourceFileName, "face-ymax.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("zminface", m_options.m_sourceFileName, "face-zmin.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("zmaxface", m_options.m_sourceFileName, "face-zmax.jpg", fileName);
  faceNames.add(fileName);

  m_cubeTexture = mgDisplay->loadTextureArray(faceNames);

  // load floor texture
  m_options.getFileName("floor", m_options.m_sourceFileName, "docs/images/floor.jpg", fileName);
  m_floorTexture = mgDisplay->loadTexture(fileName);
}

//--------------------------------------------------------------------
// set view size
void GuiTestAll::appViewResized(
  int width,
  int height)
{
  mgDisplay->setScreenSize(width, height);
  if (m_ui != NULL)
    m_ui->resize(width, height);
}

//-----------------------------------------------------------------------------
// handle idle time
void GuiTestAll::appIdle()
{
  double now = mgOSGetTime();
  animate(now, now - m_lastAnimate);
  m_lastAnimate = now;

  mgDisplay->clearView();

  viewDraw();

  if (m_ui != NULL)
  {
    // if the ui needs an update
    if (m_surface->isDamaged())
    {
      mgRectangle bounds;
      m_surface->getDamage(bounds);
      m_ui->m_top->surfacePaint(bounds);
      m_surface->repair(bounds);
    }

    mgDisplay->setTransparent(true);
    mgDisplay->setZEnable(false);
    mgDisplay->drawOverlaySurface(m_surface, 0, 0);
  }

  mgDisplay->drawCursor();
  mgDisplay->swapBuffers();
}

//--------------------------------------------------------------------
// animate the view
BOOL GuiTestAll::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  // rotate the cube
  m_angle += since/100.0;

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
  {
    m_ui->setValue(m_eyePt.x, m_eyePt.z);
    m_ui->animate(now, since);
  }
  
  return true;  // cube is rotating, so it's always updating
}

//--------------------------------------------------------------------
// nothing to do
void GuiTestAll::viewDraw()
{
  mgDisplay->setEyeMatrix(m_eyeMatrix);

  // set eye point above and in front of cube
  mgDisplay->setEyePt(m_eyePt);

  // draw the floor
  if (m_floorVertexes != NULL)
  {
    mgMatrix4 floorModel;
    mgDisplay->setModelTransform(floorModel);

    mgDisplay->setShader("litTexture");
    mgDisplay->setTexture(m_floorTexture);
    mgDisplay->draw(MG_TRIANGLES, m_floorVertexes);
  }

  if (m_cubeIndexes != NULL)
  {
    mgMatrix4 cubeModel;
    // stand cube on point
    cubeModel.rotateXDeg(45);
    cubeModel.rotateZDeg(45);
    // rotate the cube based on angle set in animateView
    cubeModel.rotateYDeg(m_angle);
    cubeModel.translate(0, sqrt(3.0), 0);
    mgDisplay->setModelTransform(cubeModel);

    // set drawing parameters
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setCulling(true);
    mgDisplay->setTransparent(false);

    // draw triangles using texture and shader
    mgDisplay->setShader("litTextureArray");
    mgDisplay->setTexture(m_cubeTexture);
    mgDisplay->draw(MG_TRIANGLES, m_cubeVertexes, m_cubeIndexes);
  }
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void GuiTestAll::appCreateBuffers()
{
  createCube();
  createFloor();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void GuiTestAll::appDeleteBuffers()
{
  delete m_cubeIndexes;
  m_cubeIndexes = NULL;
  delete m_cubeVertexes;
  m_cubeVertexes = NULL;

  delete m_floorVertexes;
  m_floorVertexes = NULL;
}

//-----------------------------------------------------------------------------
// create vertex and index buffers for cube
void GuiTestAll::createCube()
{
  // create vertexes for cube.  six sides * four vertexes
  m_cubeVertexes = mgVertexTA::newBuffer(6*4);

  // create indexes for cube triangles.  six sides times two triangles times three points
  m_cubeIndexes = mgDisplay->newIndexBuffer(6*2*3);

  mgVertexTA tl, tr, bl, br;
  int index;

  // all sides have same texture coordinate for u,v
  tl.setTexture(0, 0);
  tr.setTexture(1, 0);
  bl.setTexture(0, 1);
  br.setTexture(1, 1);

  // x min side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 0;  // face-xmin image

  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(-1,  1,  1);
  tr.setPoint(-1,  1, -1);
  bl.setPoint(-1, -1,  1);
  br.setPoint(-1, -1, -1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // x max side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 1;  // face-xmax image

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setPoint(1,  1, -1);
  tr.setPoint(1,  1,  1);
  bl.setPoint(1, -1, -1);
  br.setPoint(1, -1,  1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // y min side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 2;  // face-ymin image

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  tl.setPoint(-1, -1, -1);
  tr.setPoint( 1, -1, -1);
  bl.setPoint(-1, -1,  1);
  br.setPoint( 1, -1,  1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // y max side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 3;  // face-ymax image

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(-1, 1,  1);
  tr.setPoint( 1, 1,  1);
  bl.setPoint(-1, 1, -1);
  br.setPoint( 1, 1, -1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // z min side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 4;  // face-zmin image

  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setPoint(-1,  1, -1);
  tr.setPoint( 1,  1, -1);
  bl.setPoint(-1, -1, -1);
  br.setPoint( 1, -1, -1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // z max side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 5;  // face-zmax image

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setPoint( 1,  1, 1);
  tr.setPoint(-1,  1, 1);
  bl.setPoint( 1, -1, 1);
  br.setPoint(-1, -1, 1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);
}

//-----------------------------------------------------------------------------
// create vertex buffer for floor
void GuiTestAll::createFloor()
{
  // create vertexes for floor.  six vertexes for two triangles
  m_floorVertexes = mgVertex::newBuffer(6);

  mgVertex tl, tr, bl, br;

  tl.setTexture(0, 0);
  tr.setTexture(20, 0);
  bl.setTexture(0, 20);
  br.setTexture(20, 20);

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(-10, 0,  10);
  tr.setPoint( 10, 0,  10);
  bl.setPoint(-10, 0, -10);
  br.setPoint( 10, 0, -10);

  tl.addTo(m_floorVertexes);
  tr.addTo(m_floorVertexes);
  bl.addTo(m_floorVertexes);

  bl.addTo(m_floorVertexes);
  tr.addTo(m_floorVertexes);
  br.addTo(m_floorVertexes);
}

//-----------------------------------------------------------------------------
// mouse button pressed
void GuiTestAll::appMouseDown(
  int pressed,
  int flags)
{
  if (m_deskMode)
  {
    // send the press to the UI if it wants it
    int x, y;
    mgDisplay->getCursorPosn(x, y);
    if (m_ui->useMousePoint(x, y))
    {
      m_ui->m_top->surfaceMouseDown(x, y, flags, pressed);
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
}

//-----------------------------------------------------------------------------
// mouse button released
void GuiTestAll::appMouseUp(
  int released,
  int flags)
{
  if (m_deskMode)
  {
    int x, y;
    mgDisplay->getCursorPosn(x, y);

    // if ui wants mouse events, send it
    if (m_ui != NULL && (m_ui->hasMouseFocus() || m_ui->useMousePoint(x, y)))
    {
      m_ui->m_top->surfaceMouseUp(x, y, flags, released);
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
void GuiTestAll::appMouseMove(
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
    if (m_ui != NULL && (m_ui->hasMouseFocus() || m_ui->useMousePoint(x, y)))
      m_ui->m_top->surfaceMouseMove(x, y, flags);
  }
  else turnEye(dx, dy);
}

//-----------------------------------------------------------------------------
// mouse dragged
void GuiTestAll::appMouseDrag(
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
      m_ui->m_top->surfaceMouseDrag(x, y, flags);
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
void GuiTestAll::appMouseWheel(
  int wheel,
  int flags)
{
}

//-----------------------------------------------------------------------------
// mouse enters window
void GuiTestAll::appMouseEnter(
  int x,
  int y)
{
  mgDisplay->cursorSetPosn(x, y);
  mgDisplay->cursorEnable(true);
}

//-----------------------------------------------------------------------------
// mouse leaves window
void GuiTestAll::appMouseExit()
{
  mgDisplay->cursorEnable(false);
}

//-----------------------------------------------------------------------------
// key press
void GuiTestAll::appKeyDown(
  int keyCode,
  int modifiers)
{
  if (m_ui != NULL && m_ui->hasKeyFocus())
  {
    m_ui->m_top->surfaceKeyDown(keyCode, modifiers);
    return;
  }

  switch (keyCode)
  {
    case MG_EVENT_KEY_F1:
      m_ui->toggleHelp();
      break;

    case '~':
    case '`':
      m_ui->toggleConsole();
      break;

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
void GuiTestAll::appKeyUp(
  int keyCode,
  int modifiers)
{
  if (m_ui != NULL && m_ui->hasKeyFocus())
  {
    m_ui->m_top->surfaceKeyUp(keyCode, modifiers);
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
// key typed
void GuiTestAll::appKeyChar(
  int keyCode,
  int modifiers)
{
  if (m_ui != NULL && m_ui->hasKeyFocus())
  {
    m_ui->m_top->surfaceKeyChar(keyCode, modifiers);
    return;
  }
}
