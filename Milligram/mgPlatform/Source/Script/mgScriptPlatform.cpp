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

#ifdef EMSCRIPTEN

#include "emscripten.h"
#include "SDL/SDL.h"
#include <dirent.h>

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgWebGLSupport.h"
#include "mgScriptPlatform.h"

extern const char* mgProgramName;
extern const char* mgProgramVersion;

const int MG_LIBRARY_NONE = 0;
const int MG_LIBRARY_OPENGL21 = 1;
const int MG_LIBRARY_OPENGL33 = 2;
const int MG_LIBRARY_WEBGL = 3;

const double FPS_REPORT_INTERVAL = 30000.0;  // milliseconds

mgPlatformServices* mgPlatform = NULL;

static SDL_Surface* m_screen = NULL;
static int m_DPI = 0;
static int m_eventFlags = 0;
static int m_lastMouseX = 0;
static int m_lastMouseY = 0;

//--------------------------------------------------------------
// constructor
mgScriptPlatform::mgScriptPlatform()
{
  // set global pointer to framework
  mgPlatform = this;

  m_windowTitle = "Debug Framework";

  m_windowWidth = 800;
  m_windowHeight = 600;
  m_windowX = 100;
  m_windowY = 100;

  m_fullscreen = false;
  m_multiSample = false;
  m_swapImmediate = false;
  m_library = "";
  m_libraryFound = MG_LIBRARY_NONE;

  m_mouseRelative = false;
  m_lastCursorX = 0;
  m_lastCursorY = 0;
  m_mouseInside = false;
  m_lastKeyDown = 0;

  m_shutdown = false;

  m_errorTable = new mgPlatformErrorTable();

  m_active = false;
  m_initialized = false;
  resetTiming();
}

//--------------------------------------------------------------
// destructor
mgScriptPlatform::~mgScriptPlatform()
{
  delete m_errorTable;
  m_errorTable = NULL;

  mgPlatform = NULL;
}

//--------------------------------------------------------------
// set error message table
void mgScriptPlatform::setErrorTable(
  mgErrorTable* table)
{
  delete m_errorTable;
  m_errorTable = NULL;

  m_errorTable = table;
}

//--------------------------------------------------------------
// return display DPI
int mgScriptPlatform::getDPI()
{
  return m_DPI;
}

//--------------------------------------------------------------
// return depth of display
int mgScriptPlatform::getDepthBits()
{
  if (m_support != NULL)
    return m_support->m_depthBits;
  else return 0;
}

//--------------------------------------------------------------
// scan directory for font files
void mgScriptPlatform::scanFontDir(
  mgStringArray& fontDirs,
  const char* dirName)
{
  // list contents of directory
  DIR* directory = opendir(dirName);
  if (directory == NULL)
    return;

  // for each item in directory
  BOOL foundFont = false;
  while (true)
  {
    struct dirent* entry = readdir(directory);
    if (entry == NULL)
      break;
    if (entry->d_type == DT_DIR)
    {
      mgString fileName;
      fileName.format("%s/%s", dirName, entry->d_name);
      // scan subdirectory
      if (entry->d_name[0] != '.')
        scanFontDir(fontDirs, fileName);
    }
    else if (!foundFont)
    {
      // if a font file found, add directory to list
      char* dot = strrchr(entry->d_name, '.');
      if (dot != NULL && strcmp(dot, ".ttf") == 0)
      {
        mgString saveName(dirName);
        if (!saveName.endsWith("/"))
          saveName += "/";
        fontDirs.add(saveName);
        foundFont = true;
      }
    }
  }
  closedir(directory);
}

//--------------------------------------------------------------
// return list of system font directories
void mgScriptPlatform::getFontDirectories(
  mgStringArray& fontDirs)
{
/*
  // parse /etc/fonts/fonts.conf for directory list
  try
  {
    mgFontConfParser parser;
    parser.parseFile("/etc/fonts/fonts.conf");

    // for each directory, scan recursively for .ttf files
    int posn = parser.m_dirList.getStartPosition();
    while (posn != -1)
    {
      mgString dirName;
      DWORD value;
      parser.m_dirList.getNextAssoc(posn, dirName, value);
      scanFontDir(fontDirs, dirName);
    }
  }
  catch (...)
  {
    // if we can't parse font.conf, use XGetFontPath
    int pathCount;
    char**paths = XGetFontPath(m_display, &pathCount);
    for (int i = 0; i < pathCount; i++)
    {
      if (_stricmp(paths[i], "built-ins") == 0)
        continue;
      mgString dirName(paths[i]);
      if (!dirName.endsWith("/"))
        dirName += "/";
      fontDirs.add(dirName);
    }
    XFreeFontPath(paths);
  }
*/
}

//--------------------------------------------------------------
// set requested display library ("OpenGL3.3", "OpenGL2.1") 
// or NULL for first available in that order.
void mgScriptPlatform::setDisplayLibrary(
  const char* library)
{
  m_library = library;
}

//--------------------------------------------------------------
// query display library initialized
void mgScriptPlatform::getDisplayLibrary(
  mgString& library)
{
  switch (m_libraryFound)
  {
    case MG_LIBRARY_OPENGL33: 
      library = "OpenGL3.3";
      break;
    case MG_LIBRARY_OPENGL21: 
      library = "OpenGL2.1";
      break;
    case MG_LIBRARY_WEBGL: 
      library = "WebGL";
      break;

    case MG_LIBRARY_NONE: 
    default:
      library = "";
      break;
  }
}

//--------------------------------------------------------------
// create the window
void mgScriptPlatform::createWindow()
{
  int flags = SDL_OPENGL | SDL_RESIZABLE;
  m_screen = SDL_SetVideoMode(m_windowWidth, m_windowHeight, 32, flags); 
  if (m_screen == NULL)
    throw new mgException("SDL_SetVideoMode failed.");
}

//--------------------------------------------------------------
// initialize display.  
void mgScriptPlatform::initDisplay()
{
  createWindow();

  // build error string if library is unknown choice
  mgString libraryChoices;

  // for each supported library, try to initialize it
  m_support = NULL;
  BOOL triedLibrary = false;

#ifdef SUPPORT_GL33
  libraryChoices += "\"OpenGL3.3\" ";
  if (m_support == NULL && (m_library.equalsIgnoreCase("OpenGL3.3") || m_library.isEmpty()))
  {
    triedLibrary = true;
    mgLinuxGL33Support* gl33 = new mgLinuxGL33Support();

    // requested gl33 attributes
    gl33->m_fullscreen = m_fullscreen;
    gl33->m_multiSample = m_multiSample;
    gl33->m_swapImmediate = m_swapImmediate;

    if (gl33->initDisplay())
    {
      m_libraryFound = MG_LIBRARY_OPENGL33;
      m_support = gl33;
    }
    else delete gl33;
  }
#endif

#ifdef SUPPORT_GL21
  libraryChoices += "\"OpenGL2.1\" ";
  if (m_support == NULL && (m_library.equalsIgnoreCase("OpenGL2.1") || m_library.isEmpty()))
  {
    triedLibrary = true;
    mgLinuxGL21Support* gl21 = new mgLinuxGL21Support();

    // requested gl21 attributes
    gl21->m_fullscreen = m_fullscreen;
    gl21->m_multiSample = m_multiSample;
    gl21->m_swapImmediate = m_swapImmediate;

    if (gl21->initDisplay())
    {
      m_libraryFound = MG_LIBRARY_OPENGL21;
      m_support = gl21;
    }
    else delete gl21;
  }
#endif

#ifdef SUPPORT_WEBGL
  libraryChoices += "\"WebGL\" ";
  if (m_support == NULL && (m_library.equalsIgnoreCase("WebGL") || m_library.isEmpty()))
  {
    triedLibrary = true;
    mgWebGLSupport* webgl = new mgWebGLSupport();

    // requested gl21 attributes
    webgl->m_fullscreen = m_fullscreen;
    webgl->m_multiSample = m_multiSample;
    webgl->m_swapImmediate = m_swapImmediate;

    if (webgl->initDisplay())
    {
      m_libraryFound = MG_LIBRARY_WEBGL;
      m_support = webgl;
    }
    else delete webgl;
  }
#endif

  // if display initialization failed, throw exception
  if (m_support == NULL)
  {
    // if no library specified
    if (m_library.isEmpty())
      throw new mgErrorMsg("winNoLibrary", "libraries", "%s", (const char*) libraryChoices);

    // library specified could not be initialized
    if (triedLibrary)
      throw new mgErrorMsg("winBadLibrary", "library", "%s", (const char*) m_library);

    // library specified was not supported
    throw new mgErrorMsg("winWrongLibrary", "library,libraries", "%s,%s", 
      (const char*) m_library, (const char*) libraryChoices);
  }

  // save actual attributes
  m_depthBits = m_support->m_depthBits;
  m_fullscreen = m_support->m_fullscreen;
  m_multiSample = m_support->m_multiSample;
  m_swapImmediate = m_support->m_swapImmediate;

  m_DPI = 110;  // guess at web resolution

  mgDebug("");
  mgDebug("------ display initialized.  Return to app: %s", (const char *) m_windowTitle);
}

//--------------------------------------------------------------
// terminate the display
void mgScriptPlatform::termDisplay()
{
  if (m_support != NULL)
  {
    m_support->termDisplay();
    delete m_support;
    m_support = NULL;
  }
}

//--------------------------------------------------------------
// swap display buffers
void mgScriptPlatform::swapBuffers()
{
  if (m_support == NULL)
    return;  // no display support
  
  m_support->swapBuffers();

  // if swap immediate, report graphics timing
  m_frameCount++;
  if (m_frameCount > 60 && m_swapImmediate)
    logTiming(false);
}

//--------------------------------------------------------------
// reset frame rate timing
void mgScriptPlatform::resetTiming()
{
  m_frameCount = 0;
  m_timingStart = mgOSGetTime();
}

//--------------------------------------------------------------
// write frames per second to log
void mgScriptPlatform::logTiming(
  BOOL forceReport)
{
  if (m_frameCount == 0)
    return;

  // get elapsed time since last report
  double interval = mgOSGetTime() - m_timingStart;

  // don't report more than once per minute
  if (!forceReport && interval < FPS_REPORT_INTERVAL)
    return;

  double frameTime = interval / m_frameCount;
  int fps = (int) (1000.0 / frameTime);
  
  mgDebug(":Session fps: %d fps, %.2f ms/frame, over %d frames, %.2f seconds", fps, frameTime, m_frameCount, interval/1000.0);
  resetTiming();  // don't count log time
}

//--------------------------------------------------------------
// compile a shader pair
DWORD mgScriptPlatform::compileGLShaderPair(
  const char* vertexSource,             // null-terminated vertex shader source code
  const char* fragmentSource,           // null-termianted fragment shader source code
  int attribCount,
  const char** names,
  const DWORD* indexes)
{
  if (m_support != NULL)
    return m_support->compileShaderPair(vertexSource, fragmentSource, attribCount, names, indexes);

  return 0;
}

//--------------------------------------------------------------
// set app fullscreen or windowed
void mgScriptPlatform::setFullscreen(
  BOOL fullscreen)
{
  m_fullscreen = fullscreen;
}

//--------------------------------------------------------------
// set app fullscreen or windowed
BOOL mgScriptPlatform::getFullscreen()
{
  return m_fullscreen;
}

//--------------------------------------------------------------
// set the window title
void mgScriptPlatform::setWindowTitle(
  const char* title)
{
  m_windowTitle = title;

/*
  if (m_window != 0)
    XStoreName(m_display, m_window, m_windowTitle);
*/
}

//--------------------------------------------------------------
// get the window title
void mgScriptPlatform::getWindowTitle(
  mgString& title)
{
  title = m_windowTitle;
}

//--------------------------------------------------------------
// set the window bounds in windowed mode
void mgScriptPlatform::setWindowBounds(
  int windowX, 
  int windowY, 
  int windowWidth,
  int windowHeight)
{
  m_windowX = windowX;
  m_windowY = windowY;
  m_windowWidth = windowWidth;
  m_windowHeight = windowHeight;

  // =-= set the window position and size
}

//--------------------------------------------------------------
// get the window bounds in windowed mode
void mgScriptPlatform::getWindowBounds(
  int& windowX, 
  int& windowY, 
  int& windowWidth,
  int& windowHeight)
{
  windowX = m_windowX;
  windowY = m_windowY;
  windowWidth = m_windowWidth;
  windowHeight = m_windowHeight;
}

//--------------------------------------------------------------
// set swap mode immediate
void mgScriptPlatform::setSwapImmediate(
  BOOL swapImmediate)
{
  m_swapImmediate = swapImmediate;
}

//--------------------------------------------------------------
// get swap mode
BOOL mgScriptPlatform::getSwapImmediate()
{
  return m_swapImmediate;
}

//--------------------------------------------------------------
// enable multisample
void mgScriptPlatform::setMultiSample(
  BOOL multiSample)
{
  m_multiSample = multiSample;
}

//--------------------------------------------------------------
// get multisample
BOOL mgScriptPlatform::getMultiSample()
{
  return m_multiSample;
}

//-----------------------------------------------------------------------------
// save screen to file
void mgScriptPlatform::screenShot(
  const char* fileName)
{
}

//--------------------------------------------------------------
// check for display errors
BOOL mgScriptPlatform::checkErrors()
{
  if (m_support != NULL)
    return m_support->checkError();

  return false;
}

//--------------------------------------------------------------
// draw a texture on the screen using display coordinates
void mgScriptPlatform::drawOverlayTexture(
  DWORD texture,
  int x, 
  int y,
  int width,
  int height)
{
  if (m_support != NULL)
    m_support->drawOverlayTexture(texture, x, y, width, height);
}

//--------------------------------------------------------------
// initialize view
void mgScriptPlatform::initView()
{
#ifdef STUFF
  // do the initial resize
  mgPlatform->windowResized(m_windowWidth, m_windowHeight);

  // create the display services
  mgPlatform->cursorInitState(m_lastCursorX, m_lastCursorY);

  // start timing
  resetTiming();
#endif
}

//--------------------------------------------------------------
// terminate view
void mgScriptPlatform::termView()
{
}

//--------------------------------------------------------------
// handle window resize
void mgScriptPlatform::windowResized(
  int width,
  int height)
{
  if (!m_fullscreen)
  {
    m_windowWidth = width;
    m_windowHeight = height;
  }

  if (m_support != NULL)
    m_support->setGraphicsSize(width, height);

  if (m_theApp != NULL && m_initialized)
    m_theApp->appViewResized(width, height);
}

//--------------------------------------------------------------
// set mouse tracking mode
void mgScriptPlatform::setMouseRelative(
  BOOL relative)
{
  // if unchanged, do nothing
  if (m_mouseRelative == relative)
    return;
  m_mouseRelative = relative;

#ifdef WORKED
  if (m_mouseRelative)
  {
    // capture the mouse, force to center of window
    int x = m_windowWidth/2;
    int y = m_windowHeight/2;
    if (!m_mouseInside)
    {
      m_lastCursorX = x;
      m_lastCursorY = y;
    }
//    XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, x, y);
  }
  else 
  {
    // restore old mouse position and release capture
//    XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, 
//      m_lastCursorX, m_lastCursorY);

    // synch with real cursor again
    if (m_theApp != NULL)
      m_theApp->appMouseEnter(m_lastCursorX, m_lastCursorY);
  }
#endif
}

//--------------------------------------------------------------
// get mouse tracking mode
BOOL mgScriptPlatform::getMouseRelative()
{
  return m_mouseRelative;
}

//--------------------------------------------------------------
// end the app
void mgScriptPlatform::exitApp()
{
  m_shutdown = true;
}

//--------------------------------------------------------------
// do idle loop processing
void mgScriptPlatform::idle()
{
  if (m_theApp != NULL)
    m_theApp->appIdle();
}

//--------------------------------------------------------------
// handle mouse movement in screen coordinates
void mgScriptPlatform::mouseMoveAbs(
  int x, 
  int y,
  int flags)
{
  // if this is first point after mouse left, do setup
  if (!m_mouseInside)
    mouseEnter(x, y);

  // figure delta as different from last coord
  int dx = x - m_lastCursorX;
  int dy = y - m_lastCursorY;
  m_lastCursorX = x;
  m_lastCursorY = y;

  if (m_theApp != NULL)
  {
    if ((flags & MG_EVENT_MOUSE_BUTTONS) != 0)
      m_theApp->appMouseDrag(dx, dy, flags);
    else m_theApp->appMouseMove(dx, dy, flags);
  }
}

//--------------------------------------------------------------
// handle mouse movement in screen coordinates
void mgScriptPlatform::mouseMoveRel(
  int dx, 
  int dy,
  int flags)
{
  // relative moves should only happen with mouse captured,
  // and there should be no mouseExit.  But just in case...
  if (!m_mouseInside)
    mouseEnter(m_lastCursorX, m_lastCursorY);

  if (m_theApp != NULL)
  {
    if ((flags & MG_EVENT_MOUSE_BUTTONS) != 0)
      m_theApp->appMouseDrag(dx, dy, flags);
    else m_theApp->appMouseMove(dx, dy, flags);
  }
}

//--------------------------------------------------------------
// mouse pressed
void mgScriptPlatform::mouseDown(
  int button,
  int flags)
{
  if (m_theApp != NULL)
    m_theApp->appMouseDown(button, flags);
}

//--------------------------------------------------------------
// mouse released
void mgScriptPlatform::mouseUp(
  int button,
  int flags)
{
  if (m_theApp != NULL)
    m_theApp->appMouseUp(button, flags);
}

//--------------------------------------------------------------
// mouse entered window
void mgScriptPlatform::mouseEnter(
  int x,
  int y)
{
  // synch with real cursor again
  m_lastCursorX = x;
  m_lastCursorY = y;
  if (m_theApp != NULL)
    m_theApp->appMouseEnter(m_lastCursorX, m_lastCursorY);

  m_mouseInside = true;
}

//--------------------------------------------------------------
// mouse exited window
void mgScriptPlatform::mouseExit()
{
  if (m_theApp != NULL)
    m_theApp->appMouseExit();

  m_mouseInside = false;
}

//--------------------------------------------------------------
// process key press
void mgScriptPlatform::keyDown(
  int keyCode, 
  int flags)
{
  if (m_theApp != NULL)
    m_theApp->appKeyDown(keyCode, flags);
}

//--------------------------------------------------------------
// process key release
void mgScriptPlatform::keyUp(
  int keyCode, 
  int flags)
{
  if (m_theApp != NULL)
    m_theApp->appKeyUp(keyCode, flags);
}

//--------------------------------------------------------------
// process key char
void mgScriptPlatform::keyChar(
  int unicode, 
  int flags)
{
  if (m_theApp != NULL)
    m_theApp->appKeyChar(unicode, flags);
}

//--------------------------------------------------------------
// report program/os/machine information
void systemReport()
{
  mgDebug(":Program: %s", mgProgramName != NULL ? mgProgramName : "unknown");
  mgDebug(":Program Version: %s", mgProgramVersion != NULL ? mgProgramVersion : "unknown");
  mgDebug(":OS: Javascript");
}

//--------------------------------------------------------------
// main entry
int main(
  int argc,
  char** argv)
{
//  mgOSLaunchSendLog();

//	setlocale(LC_ALL, "");

  mgDebugReset(true);         // reset trace file
  
  if (SDL_Init (SDL_INIT_EVERYTHING) != 0) 
  {
    mgDebug("SDL_Init failed");
    return 0;
  }

  mgOSInitTimer();       // performance timer
  double startTime = mgOSGetTime();

  // initialize random numbers
  time_t seed;
  time(&seed);
  srand(12123123); // srand(seed & 0xFFFF);

  systemReport();

  mgScriptPlatform* platform = NULL;
  try
  {
    // initialize platform services
    platform = new mgScriptPlatform();
    platform->m_startTime = startTime;

    // create the application
    platform->m_theApp = mgCreateApplication();

    // tell application to set up display
    platform->m_theApp->appRequestDisplay();

    // initialize the display 
    platform->initDisplay();

    // initialize the application
    platform->m_theApp->appInit();

    // create buffers
    platform->m_theApp->appCreateBuffers();

    platform->initView();
    platform->m_theApp->appViewResized(platform->m_windowWidth, platform->m_windowHeight);

    platform->m_active = true;
    platform->m_initialized = true;
  }
  catch (mgErrorMsg* e)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug(":Session exit: %s", (const char*) msg);
    fprintf(stderr, "error: %s\n", (const char*) msg);
    return 0;
  }
  catch (mgException* e)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    mgDebug(":Session exit: %s", (const char*) e->m_message);
    fprintf(stderr, "error: %s\n", (const char*) e->m_message);
    return 0;
  }
  catch (...)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    mgDebug(":Session exit: \"...\" exception");
    return -4;
  }

  return 0;
}

//--------------------------------------------------------------
// shutdown messages
void mgShutdown()
{
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    mgDebug("------ shutdown");
    platform->logTiming(true);

    // shutdown
    platform->termView();

    // delete buffers
    platform->m_theApp->appDeleteBuffers();

    platform->m_theApp->appTerm();

    delete platform->m_theApp;
    platform->m_theApp = NULL;

    platform->termDisplay();

    platform->destroyWindow();

    delete platform;
    platform = NULL;
  }
  catch (mgErrorMsg* e)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - platform->m_startTime)/1000.0);

    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug(":Session exit: %s", (const char*) msg);
    fprintf(stderr, "error: %s\n", (const char*) msg);
    return;
  }
  catch (mgException* e)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - platform->m_startTime)/1000.0);

    mgDebug(":Session exit: %s", (const char*) e->m_message);
    fprintf(stderr, "error: %s\n", (const char*) e->m_message);
    return;
  }
  catch (...)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - platform->m_startTime)/1000.0);

    mgDebug(":Session exit: \"...\" exception");
    return;
  }

  double endTime = mgOSGetTime();
  mgDebug(":Session time: %.2f seconds", (endTime - platform->m_startTime)/1000.0);

  mgDebug(":Session exit: clean");

//  mgBlockPool::freeMemory();

  // display all memory leaks
#ifdef DEBUG_MEMORY
  mgDebugMemory();
#endif
}

//--------------------------------------------------------------
// pass idle loop call to platform
void mgIdle()
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->idle();
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass window resize to platform
void mgViewResized(
  int width,
  int height)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->windowResized(width, height);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass relative mouse movement to platform
void mgMouseMoveRel(
  int dx,
  int dy,
  int flags)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->mouseMoveRel(dx, dy, flags);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass absolute mouse movement to platform
void mgMouseMoveAbs(
  int x,
  int y,
  int flags)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->mouseMoveAbs(x, y, flags);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass mouse down to platform
void mgMouseDown(
  int button,
  int flags)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->mouseDown(button, flags);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass mouse up to platform
void mgMouseUp(
  int button,
  int flags)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->mouseUp(button, flags);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass mouse enter to platform
void mgMouseEnter(
  int x,
  int y)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->mouseEnter(x, y);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass mouse exit to platform
void mgMouseExit()
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->mouseExit();
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass key down to platform
void mgKeyDown(
  int keyCode,
  int flags)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->keyDown(keyCode, flags);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass key up to platform
void mgKeyUp(
  int keyCode,
  int flags)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->keyUp(keyCode, flags);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

//--------------------------------------------------------------
// pass key character to platform
void mgKeyChar(
  int unicode,
  int flags)
{ 
  mgScriptPlatform* platform = (mgScriptPlatform*) mgPlatform;
  try
  {
    platform->keyChar(unicode, flags);
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug("Exception: %s", (const char*) msg);
  }
  catch (mgException* e)
  {
    mgDebug("Exception: %s", (const char*) e->m_message);
  }
  catch (...)
  {
    mgDebug("Exception \"...\"");
  }
}

#endif

