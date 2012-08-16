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

#ifdef __unix__

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include <locale.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <dirent.h>

#include "mgFontConfParser.h"

#include "mgPlatformErrorTable.h"
#include "mgLinuxGL33Support.h"
#include "mgLinuxGL21Support.h"
#include "mgLinuxServices.h"

#include "mgApplication.h"

extern const char* mgProgram;
extern const char* mgProgramVersion;

const int MG_LIBRARY_NONE = 0;
const int MG_LIBRARY_OPENGL21 = 1;
const int MG_LIBRARY_OPENGL33 = 2;

const double FPS_REPORT_INTERVAL = 30000.0;  // milliseconds

mgPlatformServices* mgPlatform = NULL;

//--------------------------------------------------------------
// constructor
mgLinuxServices::mgLinuxServices()
{
  // set global pointer to framework
  mgPlatform = this;

  m_glrc = NULL;
  m_display = NULL;
  m_window = 0;
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
mgLinuxServices::~mgLinuxServices()
{
  delete m_errorTable;
  m_errorTable = NULL;

  mgPlatform = NULL;
}

//--------------------------------------------------------------
// set error message table
void mgLinuxServices::setErrorTable(
  mgErrorTable* table)
{
  delete m_errorTable;
  m_errorTable = NULL;

  m_errorTable = table;
}

//--------------------------------------------------------------
// return display DPI
int mgLinuxServices::getDPI()
{
  return m_DPI;
}

//--------------------------------------------------------------
// return depth of display
int mgLinuxServices::getDepthBits()
{
  if (m_support != NULL)
    return m_support->m_depthBits;
  else return 0;
}

//--------------------------------------------------------------
// scan directory for font files
void mgLinuxServices::scanFontDir(
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
void mgLinuxServices::getFontDirectories(
  mgStringArray& fontDirs)
{
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
}

//--------------------------------------------------------------
// set requested display library ("OpenGL3.3", "OpenGL2.1") 
// or NULL for first available in that order.
void mgLinuxServices::setDisplayLibrary(
  const char* library)
{
  m_library = library;
}

//--------------------------------------------------------------
// query display library initialized
void mgLinuxServices::getDisplayLibrary(
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

    case MG_LIBRARY_NONE: 
    default:
      library = "";
      break;
  }
}

//--------------------------------------------------------------
// initialize display.  
void mgLinuxServices::initDisplay()
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

  // save actual gl33 attributes
  m_depthBits = m_support->m_depthBits;
  m_fullscreen = m_support->m_fullscreen;
  m_multiSample = m_support->m_multiSample;
  m_swapImmediate = m_support->m_swapImmediate;

  m_support->setGraphicsSize(m_windowWidth, m_windowHeight);

  // finish display init from system state
  m_DPI = (int) ((25.4 * DisplayWidth(m_display, 0)) / DisplayWidthMM(m_display, 0));
  mgDebug("display is %d pixels/%d mm = %d dpi", DisplayWidth(m_display, 0), 
			DisplayWidthMM(m_display, 0), m_DPI);  
  m_DPI = max(m_DPI, 72);  // in case monitor info wrong

  mgDebug("");
  mgDebug("------ display initialized.  Return to app: %s", (const char *) m_windowTitle);
}

//--------------------------------------------------------------
// terminate the display
void mgLinuxServices::termDisplay()
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
void mgLinuxServices::swapBuffers()
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
void mgLinuxServices::resetTiming()
{
  m_frameCount = 0;
  m_timingStart = mgOSGetTime();
}

//--------------------------------------------------------------
// write frames per second to log
void mgLinuxServices::logTiming(
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
DWORD mgLinuxServices::compileGLShaderPair(
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
void mgLinuxServices::setFullscreen(
  BOOL fullscreen)
{
  m_fullscreen = fullscreen;
}

//--------------------------------------------------------------
// set app fullscreen or windowed
BOOL mgLinuxServices::getFullscreen()
{
  return m_fullscreen;
}

//--------------------------------------------------------------
// set the window title
void mgLinuxServices::setWindowTitle(
  const char* title)
{
  m_windowTitle = title;

  if (m_window != 0)
    XStoreName(m_display, m_window, m_windowTitle);
}

//--------------------------------------------------------------
// get the window title
void mgLinuxServices::getWindowTitle(
  mgString& title)
{
  title = m_windowTitle;
}

//--------------------------------------------------------------
// set the window bounds in windowed mode
void mgLinuxServices::setWindowBounds(
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
void mgLinuxServices::getWindowBounds(
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
void mgLinuxServices::setSwapImmediate(
  BOOL swapImmediate)
{
  m_swapImmediate = swapImmediate;
}

//--------------------------------------------------------------
// get swap mode
BOOL mgLinuxServices::getSwapImmediate()
{
  return m_swapImmediate;
}

//--------------------------------------------------------------
// enable multisample
void mgLinuxServices::setMultiSample(
  BOOL multiSample)
{
  m_multiSample = multiSample;
}

//--------------------------------------------------------------
// get multisample
BOOL mgLinuxServices::getMultiSample()
{
  return m_multiSample;
}

//-----------------------------------------------------------------------------
// save screen to file
void mgLinuxServices::screenShot(
  const char* fileName)
{
}

//--------------------------------------------------------------
// check for display errors
BOOL mgLinuxServices::checkErrors()
{
  if (m_support != NULL)
    return m_support->checkError();

  return false;
}

//--------------------------------------------------------------
// draw a texture on the screen using display coordinates
void mgLinuxServices::drawOverlayTexture(
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
void mgLinuxServices::initView()
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
void mgLinuxServices::termView()
{
}

//--------------------------------------------------------------
// create window 
void mgLinuxServices::createWindow()
{
  // Get a matching FB config
  static int visual_attribs[] =
    {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_DOUBLEBUFFER    , 1,
			GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
//      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 1,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      0
    };
 
  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig(m_display, DefaultScreen(m_display),
                                       visual_attribs, &fbcount);
  if (fbc == NULL || fbcount == 0)
    throw new mgErrorMsg("linuxNoConfig", "", "");

  mgDebug("Found %d matching FB configs.", fbcount);
  int bestIndex = 0;
  int bestSamples = 0;
  for (int i = 0; i < fbcount; i++)
  {
    int value = 0;
    glXGetFBConfigAttrib(m_display, fbc[i], GLX_SAMPLE_BUFFERS, &value);
    if (value)
    {
      glXGetFBConfigAttrib(m_display, fbc[i], GLX_SAMPLES, &value);
      if (value > bestSamples && value <= 8)
      {
        bestSamples = value;
        bestIndex = i;
      }
    }
  }
  mgDebug("best multisample is %d, config=%d", bestSamples, bestIndex);
  m_bestFbc = fbc[bestIndex];
 
  // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
  XFree(fbc);
 
  // Get a visual
  m_vi = glXGetVisualFromFBConfig(m_display, m_bestFbc);
  if (m_vi == NULL)
    throw new mgErrorMsg("linuxNullConfig", "", "");

  m_colormap = XCreateColormap(m_display, RootWindow(m_display, m_vi->screen),
                               m_vi->visual, AllocNone);

  XSetWindowAttributes swa;
  swa.colormap = m_colormap;
  swa.background_pixel  = BlackPixel(m_display, m_vi->screen);
  swa.border_pixel      = WhitePixel(m_display, m_vi->screen);

  swa.event_mask        = KeyPressMask | KeyReleaseMask | KeymapStateMask |
                          ButtonPressMask | ButtonReleaseMask |
                          PointerMotionMask | ButtonMotionMask |
                          EnterWindowMask | LeaveWindowMask |
                          StructureNotifyMask;

  unsigned long swaMask = CWColormap | CWBackPixel | CWBorderPixel | CWEventMask;
 
  mgDebug("Creating window, %d by %d, depth=%d", m_windowWidth, m_windowHeight, m_vi->depth);
  m_window = XCreateWindow(m_display, RootWindow(m_display, m_vi->screen), 
                              m_windowX, m_windowY, m_windowWidth, m_windowHeight, 
                              4, m_vi->depth, InputOutput, 
                              m_vi->visual, 
                              swaMask, &swa);
  if (m_window == 0)
    throw new mgErrorMsg("linuxBadCreate", "", "");

  XColor color;
  memset(&color, 0, sizeof(color));
  Pixmap image = XCreatePixmap(m_display, m_window, 1, 1, 1);
// =-= clear the image
  Cursor cursor = XCreatePixmapCursor(m_display, image, image, &color, &color, 0, 0);
  XDefineCursor(m_display, m_window, cursor);

  XStoreName(m_display, m_window, m_windowTitle);
 
  // register interest in the delete window message
  m_wmDeleteMessage = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(m_display, m_window, &m_wmDeleteMessage, 1);

  XMapWindow(m_display, m_window);

  m_mouseInside = false;
    
  if (m_mouseRelative)
  {
    m_lastCursorX = m_windowWidth/2;
    m_lastCursorY = m_windowHeight/2;
    XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, 
        m_lastCursorX, m_lastCursorY);

    /* Make the warp visible immediately. */
    XFlush(m_display);
  }
}

//--------------------------------------------------------------
// destroy our window 
void mgLinuxServices::destroyWindow()
{
  // Done with the visual info data
  XFree(m_vi);
  m_vi = NULL;
 
  XDestroyWindow(m_display, m_window);
  m_window = 0;

  XFreeColormap(m_display, m_colormap);
  m_colormap = NULL;

	XCloseDisplay(m_display);
  m_display = NULL;
}

//--------------------------------------------------------------
// handle window resize
void mgLinuxServices::windowResized(
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
void mgLinuxServices::setMouseRelative(
  BOOL relative)
{
  // if unchanged, do nothing
  if (m_mouseRelative == relative)
    return;
  m_mouseRelative = relative;

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
    XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, x, y);
  }
  else 
  {
    // restore old mouse position and release capture
    XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, 
      m_lastCursorX, m_lastCursorY);

    // synch with real cursor again
    if (m_theApp != NULL)
      m_theApp->appMouseEnter(m_lastCursorX, m_lastCursorY);
  }
}

//--------------------------------------------------------------
// get mouse tracking mode
BOOL mgLinuxServices::getMouseRelative()
{
  return m_mouseRelative;
}

//--------------------------------------------------------------
// end the app
void mgLinuxServices::exitApp()
{
  m_shutdown = true;
}

//--------------------------------------------------------------
// track mouse exit
void mgLinuxServices::trackMouse(
  int x,
  int y,
  int& dx,
  int& dy)
{
  // if this is first point after mouse left, do setup
  if (!m_mouseInside)
    mouseEnter();
  
  if (m_mouseRelative)
  {
    // keep cursor in center of window, figure delta from motion
    dx = x - m_windowWidth/2;
    dy = y - m_windowHeight/2;
    if (dx != 0 || dy != 0)
    {
      XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, 
        m_windowWidth/2, m_windowHeight/2);
    }
  }
  else
  {
    // figure delta as different from last coord
    dx = x - m_lastCursorX;
    dy = y - m_lastCursorY;
    m_lastCursorX = x;
    m_lastCursorY = y;
  }
}

//--------------------------------------------------------------
// handle mouse movement
void mgLinuxServices::mouseMove(
  int x, 
  int y,
  int state)
{
  int dx, dy;
  trackMouse(x, y, dx, dy);

  if (m_theApp != NULL)
  {
    int flags = translateState(state);

    if ((flags & MG_EVENT_MOUSE_BUTTONS) != 0)
      m_theApp->appMouseDrag(dx, dy, flags);
    else m_theApp->appMouseMove(dx, dy, flags);
  }
}

//--------------------------------------------------------------
// handle mouse press
void mgLinuxServices::mouseDown(
  XButtonEvent& ev)   
{
  int dx, dy;
  trackMouse(ev.x, ev.y, dx, dy);

  int flags = translateState(ev.state);

  // button2 and button3 are reversed from Windows meaning
  int pressed = 0;
  if (ev.button == Button1)
    pressed = MG_EVENT_MOUSE1_DOWN;
  else if (ev.button == Button3)
    pressed = MG_EVENT_MOUSE2_DOWN;
  else if (ev.button == Button2)
    pressed = MG_EVENT_MOUSE3_DOWN;

  flags |= pressed;

  // =-= if another button already down, should this be a dragged, not moved?
  if (dx != 0 || dy != 0 && m_theApp != NULL)
    m_theApp->appMouseMove(dx, dy, flags);

  if (m_theApp != NULL)
    m_theApp->appMouseDown(pressed, flags);

}

//--------------------------------------------------------------
// handle mouse release
void mgLinuxServices::mouseUp(
  XButtonEvent& ev)   
{
  int dx, dy;
  trackMouse(ev.x, ev.y, dx, dy);

  int flags = translateState(ev.state);

  // button2 and button3 are reversed from Windows meaning
  int released = 0;
  if (ev.button == Button1)
    released = MG_EVENT_MOUSE1_DOWN;
  else if (ev.button == Button3)
    released = MG_EVENT_MOUSE2_DOWN;
  else if (ev.button == Button2)
    released = MG_EVENT_MOUSE3_DOWN;

  flags &= ~released;

  if (m_theApp != NULL)
  {
    // if button released away from last coords, it was dragged
    if (dx != 0 || dy != 0)
      m_theApp->appMouseDrag(dx, dy, flags);

    m_theApp->appMouseUp(released, flags);
  }
}

//--------------------------------------------------------------
// mouse entered window
void mgLinuxServices::mouseEnter()
{
  // synch with real cursor again
  if (m_theApp != NULL)
    m_theApp->appMouseEnter(m_lastCursorX, m_lastCursorY);

  m_mouseInside = true;
}

//--------------------------------------------------------------
// mouse exited window
void mgLinuxServices::mouseExit()
{
  if (m_theApp != NULL)
    m_theApp->appMouseExit();

  m_mouseInside = false;
}

//--------------------------------------------------------------
// translate special keys to platform-independent codes
int mgLinuxServices::translateKey(
  int key)
{
  switch (key)
  {
    case XK_F1: return MG_EVENT_KEY_F1;
    case XK_F2: return MG_EVENT_KEY_F2;
    case XK_F3: return MG_EVENT_KEY_F3;
    case XK_F4: return MG_EVENT_KEY_F4;
    case XK_F5: return MG_EVENT_KEY_F5;
    case XK_F6: return MG_EVENT_KEY_F6;
    case XK_F7: return MG_EVENT_KEY_F7;
    case XK_F8: return MG_EVENT_KEY_F8;
    case XK_F9: return MG_EVENT_KEY_F9;
    case XK_F10: return MG_EVENT_KEY_F10;
    case XK_F11: return MG_EVENT_KEY_F11;
    case XK_F12: return MG_EVENT_KEY_F12;

    case XK_Left: return MG_EVENT_KEY_LEFT;
    case XK_Right: return MG_EVENT_KEY_RIGHT;
    case XK_Up: return MG_EVENT_KEY_UP;
    case XK_Down: return MG_EVENT_KEY_DOWN;

    case XK_BackSpace: return MG_EVENT_KEY_BACKSPACE;
    case XK_Insert: return MG_EVENT_KEY_INSERT;
    case XK_Delete: return MG_EVENT_KEY_DELETE;
    case XK_Home: return MG_EVENT_KEY_HOME;
    case XK_End: return MG_EVENT_KEY_END;
    case XK_Prior: return MG_EVENT_KEY_PAGEUP;
    case XK_Next: return MG_EVENT_KEY_PAGEDN;

    case XK_Return: return MG_EVENT_KEY_RETURN;
    case XK_Tab: return MG_EVENT_KEY_TAB;
    case XK_Escape: return MG_EVENT_KEY_ESCAPE;

    case XK_KP_0: return MG_EVENT_KEY_INSERT;
    case XK_KP_1: return MG_EVENT_KEY_END;
    case XK_KP_2: return MG_EVENT_KEY_DOWN;
    case XK_KP_3: return MG_EVENT_KEY_PAGEDN;
    case XK_KP_4: return MG_EVENT_KEY_LEFT;
    case XK_KP_5: return ' ';
    case XK_KP_6: return MG_EVENT_KEY_RIGHT;
    case XK_KP_7: return MG_EVENT_KEY_HOME;
    case XK_KP_8: return MG_EVENT_KEY_UP;
    case XK_KP_9: return MG_EVENT_KEY_PAGEUP;

    case XK_KP_Multiply: return '*';
    case XK_KP_Add: return '+';
    case XK_KP_Separator: return MG_EVENT_KEY_RETURN;
    case XK_KP_Subtract: return '-';
    case XK_KP_Decimal: return MG_EVENT_KEY_DELETE;
    case XK_KP_Divide: return '/';

/*
    case VK_OEM_1: return ';';
    case VK_OEM_PLUS: return '+';
    case VK_OEM_COMMA: return ',';
    case VK_OEM_MINUS: return '-';
    case VK_OEM_PERIOD: return '.'; 
    case VK_OEM_2: return '/';
    case VK_OEM_3: return '`';
    case VK_OEM_4: return '[';
    case VK_OEM_5: return '\\';
    case VK_OEM_6: return ']';
    case VK_OEM_7: return '\'';
*/

    default: return key;
  }
}

//--------------------------------------------------------------
// translate X key event state bits
int mgLinuxServices::translateState(
  int state)
{
  int flags = 0;

  // button2 and button3 are reversed from Windows meaning
  if ((state & Button1Mask) != 0)
    flags |= MG_EVENT_MOUSE1_DOWN;

  if ((state & Button3Mask) != 0)
    flags |= MG_EVENT_MOUSE2_DOWN;

  if ((state & Button2Mask) != 0)
    flags |= MG_EVENT_MOUSE3_DOWN;

  if ((state & ShiftMask) != 0)
    flags |= MG_EVENT_SHIFT_DOWN;
  if ((state & ControlMask) != 0)
    flags |= MG_EVENT_CNTL_DOWN;
  if ((state & Mod1Mask) != 0)
    flags |= MG_EVENT_META_DOWN;

  return flags;
}

//--------------------------------------------------------------
// handle key press
void mgLinuxServices::keyDown(
  XKeyEvent& ev)
{
  XComposeStatus composeStatus;
  char asciiCode[32];
  KeySym keySym;

  /* Check for the ASCII/KeySym codes associated with the event: */
  int keyLen = XLookupString(&ev, asciiCode, sizeof(asciiCode),
                          &keySym, &composeStatus);
  int keyCode;
  if (keyLen > 0)
  {
//  mgDebug("key asciicode = %02x", asciiCode[0]);
    if (asciiCode[0] == 0x1B)
      keyCode = XK_Escape;
    else if (asciiCode[0] == 0x0D)
      keyCode = XK_Return;
    else if (asciiCode[0] == 0x08)
      keyCode = XK_BackSpace;
    else if (asciiCode[0] == '\t')
      keyCode = XK_Tab;
    else if (asciiCode[0] == 0x7F)
      keyCode = XK_Delete;
    else keyCode = asciiCode[0];

    if (!isprint(asciiCode[0]))
      keyLen = 0;
  }
  else 
  {
    keyCode = (int) keySym;
//    mgDebug("key sym = %0x", keyCode);
  }

  int repeat = (ev.keycode == m_lastKeyDown) ? MG_EVENT_KEYREPEAT : 0;
  m_lastKeyDown = ev.keycode;

  // translate special keys
  int key = translateKey(keyCode);
  int flags = translateState(ev.state);


  if (m_theApp != NULL)
  {
    // send down key in upper-case to match windows
    m_theApp->appKeyDown(key < 256 ? toupper(key) : key, flags);

    // if alt key pressed
    if ((flags & MG_EVENT_META_DOWN) != 0)
    {
//      if (key == MG_EVENT_KEY_RETURN)
//        mgDisplay->toggleFullscreen();
//      else m_theApp->appKeySys(key, flags);
    }
    else 
    {
      // try to send letter keys only
      if (keyLen > 0)
        m_theApp->appKeyChar(key, flags | repeat);
    }
  }
}

//--------------------------------------------------------------
// handle key release
void mgLinuxServices::keyUp(
  XKeyEvent& ev)
{
  // query the keyboard state 
  char keyState[32];
  XQueryKeymap(m_display, keyState);

  // if key is still down, ignore key up message
  if (ev.keycode < 256)
  {
    if ((keyState[ev.keycode/8] & (1 << (ev.keycode % 8))) != 0)
      return;
  }

  m_lastKeyDown = 0;

  XComposeStatus composeStatus;
  char asciiCode[32];
  KeySym keySym;

  /* Check for the ASCII/KeySym codes associated with the event: */
  int len = XLookupString(&ev, asciiCode, sizeof(asciiCode),
                      &keySym, &composeStatus);

  int keyCode;
  if (len > 0)
  {
    if (asciiCode[0] == 0x1B)
      keyCode = XK_Escape;
    else if (asciiCode[0] == 0x0D)
      keyCode = XK_Return;
    else keyCode = toupper(asciiCode[0]);
  }
  else keyCode = (int) keySym;

  // translate special keys
  int key = translateKey(keyCode);
  int flags = translateState(ev.state);

  if (m_theApp != NULL)
  {
    // send up key in upper-case to match windows
    m_theApp->appKeyUp(key < 256 ? toupper(key) : key, flags);
  }
}

//--------------------------------------------------------------
// parse the lspci output for display device
BOOL getDisplayDevice(
  mgString& device)
{
  device.empty();
  mgString report;

  FILE* reportFile = popen("lspci -vmm", "r");
  if (reportFile == NULL)
    return false;

  char buffer[2048];
  while (true)
  {
    int readLen = (int) fread(buffer, 1, sizeof(buffer), reportFile);
    if (readLen == 0)
      break;
    report.write(buffer, readLen);
  }
  pclose(reportFile);

  // do a sloppy parse to find display device
  mgString line;
  int classPosn = 0;
  while (true)
  {
    classPosn = report.find(classPosn, "Class:");
    if (classPosn == -1)
      break;
    classPosn += strlen("Class:");

    int end = report.find(classPosn, '\n');
    if (end == -1)
      break;
    report.substring(line, classPosn, end-classPosn);
    line.trim();
    if (line.startsWith("VGA "))
    {
      int devicePosn = report.find(end, "Device:");
      if (devicePosn != -1)
      {
        devicePosn += strlen("Device:");
        end = report.find(devicePosn, '\n');
        if (end == -1)
          break;
        report.substring(line, devicePosn, end-devicePosn);
        line.trim();
        device = line;

        // if text in [], take only that
        int bracket = line.find(0, '[');
        if (bracket != -1)
        {
          bracket++;
          end = line.find(bracket, ']');
          if (end != -1)
            line.substring(device, bracket, end-bracket);
        }
        break;  // we have device
      }
    }
  }
  return !device.isEmpty();
}

//--------------------------------------------------------------
// report program/os/machine information
void systemReport()
{
  mgDebug(":Program: %s", mgProgram != NULL ? mgProgram : "unknown");
  mgDebug(":Program Version: %s", mgProgramVersion != NULL ? mgProgramVersion : "unknown");
  struct utsname name;
  if (uname(&name) != -1)
  {
    mgDebug(":OS: %s", name.sysname);
    mgDebug(":OS Linux Release: %s", name.release);
    mgDebug(":OS Linux Version: %s", name.version);
    mgDebug(":Machine 64-bit: %s", strstr(name.machine, "_64") != NULL ? "true" : "false");
  }
  else mgDebug(":OS: unix, no uname");

  mgDebug(":Machine CPU count: %d", sysconf(_SC_NPROCESSORS_ONLN));

  // get the display device
  mgString device;
  if (getDisplayDevice(device))
    mgDebug(":Machine Display: %s", (const char*) device);

  // get a unique machine id
  FILE* idFile = fopen("/var/lib/dbus/machine-id", "rb");
  if (idFile != NULL)
  {
    char buffer[256];
    int readLen = fread(buffer, 1, sizeof(buffer)-1, idFile);
    fclose(idFile);

    if (readLen > 0)
    {
      mgString id(buffer, readLen);
      id.trim();
      mgDebug(":Machine Id: %s", (const char*) id);
    }
  }
}

//--------------------------------------------------------------
// list the system font directories
void listFontDirs(
  mgLinuxServices* platform)
{
  mgStringArray fontDirs;
  platform->getFontDirectories(fontDirs);
  mgDebug("Font directories:");
  for (int i = 0; i < fontDirs.length(); i++)
  {
    mgDebug("  %s", (const char*) fontDirs.getAt(i));
  }
}

//--------------------------------------------------------------
// main entry
int main(
  int argc,
  char** argv)
{
  mgOSLaunchSendLog();

	setlocale(LC_ALL, "");

  mgDebugReset();         // reset trace file
  
  mgOSInitTimer();       // performance timer
  double startTime = mgOSGetTime();

  // initialize random numbers
  time_t seed;
  time(&seed);
  srand(12123123); // srand(seed & 0xFFFF);

  systemReport();

  mgLinuxServices* platform = NULL;
  try
  {
    // initialize platform services
    platform = new mgLinuxServices();

    // First connect to the display server
    platform->m_display = XOpenDisplay(NULL);
    if (platform->m_display == NULL) 
      throw new mgErrorMsg("linuxBadDisplay", "", "");

    Screen* screen = XDefaultScreenOfDisplay(platform->m_display);
    
    mgDebug(":Machine Monitor count: %d", ScreenCount(platform->m_display));
    mgDebug(":Machine Monitor res: %d by %d", WidthOfScreen(screen), HeightOfScreen(screen));

    listFontDirs(platform);

    // create the application
    platform->m_theApp = mgCreateApplication();

    // tell application to set up display
    platform->m_theApp->appRequestDisplay();

    // initialize the display 
    platform->initDisplay();

    // initialize the application
    platform->m_theApp->appInit();
    platform->m_theApp->appViewResized(platform->m_windowWidth, platform->m_windowHeight);

    // create buffers
    platform->m_theApp->appCreateBuffers();

    platform->initView();

    platform->m_active = true;
    platform->m_initialized = true;

    // process X11 messages until done
    int width = -1;
    int height = -1;
    while (!platform->m_shutdown)
    {
      XEvent ev;
      if (platform->m_active)
      {
        while (XPending(platform->m_display) == 0)
        {
          platform->m_theApp->appIdle();
        }
      }

      XNextEvent(platform->m_display, &ev);
      switch (ev.type)
      {
        case ConfigureNotify:
				  if (width != ev.xconfigure.width || height != ev.xconfigure.height) 
	        {
					  width = ev.xconfigure.width;
					  height = ev.xconfigure.height;
            if (width != 0 && height != 0)
              platform->windowResized(width, height);
				  }
				  break;

        case ClientMessage:
          if (ev.xclient.data.l[0] == platform->m_wmDeleteMessage)
            platform->m_shutdown = true;
          break;

        case MapNotify:
          // window shown
          platform->m_active = true;
          break;

        case UnmapNotify:
          // window hidden
          platform->m_active = false;
          break;

        case MappingNotify:
          // update keyboard mapping
          XRefreshKeyboardMapping((XMappingEvent*) &ev);
          break;

#ifdef STUFF
        case EnterNotify:
          mouseEnter();
          break;
#endif

        case LeaveNotify:
          platform->mouseExit();
          break;

			  case KeyPress:
          platform->keyDown(ev.xkey);
          break;

        case KeyRelease:
          platform->keyUp(ev.xkey);
          break;

        case ButtonPress:
          platform->mouseDown(ev.xbutton);
          break;

        case ButtonRelease:
          platform->mouseUp(ev.xbutton);
          break;

        case MotionNotify:
          platform->mouseMove(ev.xmotion.x, ev.xmotion.y, ev.xmotion.state);
          break;
      }
    }

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
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    mgString msg;
    platform->m_errorTable->msgText(msg, e);

    mgDebug(":Session exit: %s", (const char*) msg);
    fprintf(stderr, "error: %s\n", (const char*) msg);
//    MessageBox(NULL, (const char *) e->m_message, "Error", MB_OK | MB_ICONINFORMATION);
    return 0;
  }
  catch (mgException* e)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    mgDebug(":Session exit: %s", (const char*) e->m_message);
    fprintf(stderr, "error: %s\n", (const char*) e->m_message);
//    MessageBox(NULL, (const char *) e->m_message, "Error", MB_OK | MB_ICONINFORMATION);
    return 0;
  }
  catch (...)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    mgDebug(":Session exit: \"...\" exception");
//    MessageBox(platform->m_window, TEXT("Unhandled exception"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
    return -4;
  }

  double endTime = mgOSGetTime();
  mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

  mgDebug(":Session exit: clean");

  mgBlockPool::freeMemory();

  // display all memory leaks
#ifdef DEBUG_MEMORY
  mgDebugMemory();
#endif

  return 0;
}

#endif
