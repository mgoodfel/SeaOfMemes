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
const char* mgProgram = "GuiTestGL";
const char* mgProgramVersion = "Part 60";

#include "mgFontList.h"
#include "TextureSurface.h"
#include "mgCursorDefn.h"
#include "mgUtil/Include/ImageUtil/mgImageUtil.h"
#include "SampleUI.h"
#include "Planet.h"

#include "GuiTestGL.h"

const double INVALID_TIME = -1.0;

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set the working directory to dir containing options
  mgOSFindWD("options.xml");

  return new GuiTestGL();
}

//--------------------------------------------------------------
// constructor
GuiTestGL::GuiTestGL()
{
  m_options.parseFile("options.xml");
  mgPlatform->getFontDirectories(m_fontDirs);

  mgString fileName;
  m_options.getFileName("fontList", m_options.m_sourceFileName, "", fileName);
  m_fontList = new mgFontList(fileName);

  m_ui = NULL;
  m_planet = NULL;
  m_surface = NULL;

  m_forwardSince = INVALID_TIME;
  m_backwardSince = INVALID_TIME;
  m_leftSince = INVALID_TIME;
  m_rightSince = INVALID_TIME;
}

//--------------------------------------------------------------
// destructor
GuiTestGL::~GuiTestGL()
{
  delete m_fontList;
  m_fontList = NULL;
}

//--------------------------------------------------------------
// set the display attributes
void GuiTestGL::appRequestDisplay()
{
  mgString title;
  title.format("%s, %s", mgProgram, mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // use GL2.1 so we can have only one shader
  mgPlatform->setDisplayLibrary("OpenGL2.1");

  mgPlatform->setWindowBounds(
    m_options.getInteger("windowX", 100), 
    m_options.getInteger("windowY", 100),
    m_options.getInteger("windowWidth", 800),
    m_options.getInteger("windowHeight", 600));

  // set graphics parameters
  mgPlatform->setMultiSample(m_options.getBoolean("multiSample", true));
  mgPlatform->setSwapImmediate(m_options.getBoolean("graphicsTiming", false));
}

//--------------------------------------------------------------
// create the user interface
void GuiTestGL::createUI()
{
  mgString fileName;
  m_options.getFileName("helpFile", m_options.m_sourceFileName, "", fileName);

  m_surface = new TextureSurface(this);
  m_ui = new SampleUI(m_surface, fileName);
}

//--------------------------------------------------------------
// destroy the UI
void GuiTestGL::destroyUI()
{
  delete m_ui;
  m_ui = NULL;

  delete m_surface;
  m_surface = NULL;
}

//-----------------------------------------------------------------------------
// find a font file.  called by BitmapSurface
BOOL GuiTestGL::findFont(
  const char* faceName, 
  BOOL bold, 
  BOOL italic, 
  mgString& fontFile)
{
  return m_fontList->findFont(m_fontDirs, faceName, bold, italic, fontFile);
}

//--------------------------------------------------------------
// create a simple cursor
void GuiTestGL::createCursor()
{
  // parse the cursor description, which gives texture filename and hot spot offsets
  mgString fileName;
  m_options.getFileName("cursor", m_options.m_sourceFileName, "", fileName);

  mgCursorDefn arrow(fileName);
  m_cursorHotX = arrow.m_hotX;
  m_cursorHotY = arrow.m_hotY;

  // load the texture image
  BYTE* cursorData;
  BOOL hasAlpha;
  mgLoadRGBAImage(arrow.m_texture, m_cursorWidth, m_cursorHeight, hasAlpha, cursorData);

  // create the OpenGL texture and load the cursor pattern
  glGenTextures(1, &m_cursorTexture);

  glBindTexture(GL_TEXTURE_2D, m_cursorTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_cursorWidth, m_cursorHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) cursorData);

  delete cursorData;
}

//--------------------------------------------------------------
// initialize application
void GuiTestGL::appInit()
{
  createCursor();
  m_cursorX = m_cursorY = 0;
  m_cursorEnabled = false;

  createUI();

  m_planet = new Planet(m_options);
}

//--------------------------------------------------------------
// terminate application
void GuiTestGL::appTerm()
{
  destroyUI();

  delete m_planet;
  m_planet = NULL;
}

//--------------------------------------------------------------
// update position based on pressed keys
void GuiTestGL::updateMovement()
{
  if (m_planet == NULL)
    return; // too early

  double now = mgOSGetTime();

  if (m_forwardSince != INVALID_TIME)
  {
    m_planet->moveForwards(now - m_forwardSince);
    m_forwardSince = now;
  }

  if (m_backwardSince != INVALID_TIME)
  {
    m_planet->moveBackwards(now - m_backwardSince);
    m_backwardSince = now;
  }

  if (m_leftSince != INVALID_TIME)
  {
    m_planet->moveLeftwards(now - m_leftSince);
    m_leftSince = now;
  }

  if (m_rightSince != INVALID_TIME)
  {
    m_planet->moveRightwards(now - m_rightSince);
    m_rightSince = now;
  }
}

//--------------------------------------------------------------
// delete any display buffers
  // If you wish to support DirectX, you must be prepared for the graphics support
  // to reset itself, losing all display state.  This method call informs the app it
  // should delete display objects before reset.  It is also called at shutdown.
void GuiTestGL::appDeleteBuffers()
{
}

//--------------------------------------------------------------
// create buffers, ready to send to display
  // this method is called after DirectX graphics support is reinitialized.  
  // It is also called at startup, after the window is created.
void GuiTestGL::appCreateBuffers()
{
}

//--------------------------------------------------------------
// set the size of the view
void GuiTestGL::appViewResized(
  int width,
  int height)
{
  m_windowWidth = width;
  m_windowHeight = height;

  if (m_ui != NULL)
    m_ui->resize(m_windowWidth, m_windowHeight);

  glViewport(0, 0, m_windowWidth, m_windowHeight);
}

//--------------------------------------------------------------
// no input pending.  
// Typically, build the scene and call mgPlatform->swapBuffers().
void GuiTestGL::appIdle()
{
  // update movement based on pressed keys
  updateMovement();

  // animate the ui
  if (m_ui != NULL)
    m_ui->m_top->animate();

  // set an example "statistic" 
  if (m_ui != NULL)
    m_ui->setValue(m_planet->m_eyePt.x, m_planet->m_eyePt.z);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glFrontFace(GL_CW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);

  // draw the scene
  double angle = mgOSGetTime()/40000;
  angle -= (int)angle;
  if (m_planet != NULL)
    m_planet->render(m_windowWidth, m_windowHeight, angle*360);

  // if the ui needs an update
  if (m_surface != NULL && m_surface->isDamaged())
  {
    mgRectangle bounds;
    m_surface->getDamage(bounds);
    m_ui->m_top->surfacePaint(bounds);
    m_surface->repair(bounds);
  }

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  // paint the UI
  mgPlatform->drawOverlayTexture(m_surface->getTexture(), 0, 0, m_windowWidth, m_windowHeight);

  // paint the cursor
  if (m_cursorEnabled)
  {
    mgPlatform->drawOverlayTexture(m_cursorTexture, m_cursorX - m_cursorHotX, m_cursorY - m_cursorHotY, 
                                   m_cursorWidth, m_cursorHeight);
  }

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  mgPlatform->swapBuffers();
}

//--------------------------------------------------------------
// key press
void GuiTestGL::appKeyDown(
  int keyCode,
  int modifiers)
{
  if (m_ui == NULL)
    return; // too early

  // handle the help key and console key
  if (keyCode == '`' || keyCode == '~')
    m_ui->toggleConsole();

  else if (keyCode == MG_EVENT_KEY_F1)
    m_ui->toggleHelp();

  else if (m_ui->hasKeyFocus())
    m_ui->m_top->surfaceKeyDown(keyCode, modifiers);

  else
  {
    // pass keystroke to application code.

    // start movement
    switch (keyCode)
    {
      case 'W':
        // ignore autorepeat keys
        if ((modifiers & MG_EVENT_KEYREPEAT) != 0)
          return;
          
          if (m_forwardSince == INVALID_TIME)
            m_forwardSince = mgOSGetTime();
        break;

      case 'S':
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
    }
  }
}

//--------------------------------------------------------------
// key release
void GuiTestGL::appKeyUp(
  int keyCode,
  int modifiers)
{
  if (m_ui == NULL)
    return;  // too early

  if (m_ui->hasKeyFocus())
    m_ui->m_top->surfaceKeyUp(keyCode, modifiers);
  else
  {
    switch (keyCode)
    {
      case 'W':
        m_forwardSince = INVALID_TIME;
        break;

      case 'S':
        m_backwardSince = INVALID_TIME;
        break;

      case 'A':
        m_leftSince = INVALID_TIME;
        break;

      case 'D':
        m_rightSince = INVALID_TIME;
        break;
    }
  }
}

//--------------------------------------------------------------
// key typed
void GuiTestGL::appKeyChar(
  int keyCode,
  int modifiers)
{
  if (m_ui != NULL && m_ui->hasKeyFocus())
    m_ui->m_top->surfaceKeyChar(keyCode, modifiers);
}

//--------------------------------------------------------------
// mouse button pressed
void GuiTestGL::appMouseDown(
  int pressed,
  int flags)
{
  if (m_ui != NULL && m_ui->useMousePoint(m_cursorX, m_cursorY))
    m_ui->m_top->surfaceMouseDown(m_cursorX, m_cursorY, flags, pressed);
  else
  {
    // any button pressed, turn off cursor
    m_cursorEnabled = false;
    mgPlatform->setMouseRelative(true);
  }
}

//--------------------------------------------------------------
// mouse button released
void GuiTestGL::appMouseUp(
  int released,
  int flags)
{
  if (m_ui != NULL && m_ui->hasMouseFocus())
    m_ui->m_top->surfaceMouseUp(m_cursorX, m_cursorY, flags, released);
  else
  {
    // if both buttons released, enable cursor
    if ((flags & (MG_EVENT_MOUSE1_DOWN | MG_EVENT_MOUSE2_DOWN)) == 0)
    {
      m_cursorEnabled = true;
      mgPlatform->setMouseRelative(false);
    }
  }
}

//--------------------------------------------------------------
// mouse moved
void GuiTestGL::appMouseMove(
  int dx,
  int dy, 
  int flags)
{
  m_cursorX += dx;
  m_cursorY += dy;

  if (m_ui != NULL)
    m_ui->m_top->surfaceMouseMove(m_cursorX, m_cursorY, flags);
}

//--------------------------------------------------------------
// mouse dragged
void GuiTestGL::appMouseDrag(
  int dx,
  int dy, 
  int flags)
{
  m_cursorX += dx;
  m_cursorY += dy;

  if (m_ui != NULL && m_ui->hasMouseFocus())
    m_ui->m_top->surfaceMouseDrag(m_cursorX, m_cursorY, flags);
  else if (m_planet != NULL)
    m_planet->turnEye(dx, dy);
}

//--------------------------------------------------------------
// mouse wheel rotated
void GuiTestGL::appMouseWheel(
  int wheel,
  int flags)
{
}

//--------------------------------------------------------------
// mouse enters window
void GuiTestGL::appMouseEnter(
  int x,
  int y)
{
  m_cursorX = x;
  m_cursorY = y;
  m_cursorEnabled = true;
}

//--------------------------------------------------------------
// mouse leaves window
void GuiTestGL::appMouseExit()
{
  m_cursorEnabled = false;
}
