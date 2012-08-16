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

/* TODO:
  - scroll wheel
  - video sync?
*/

#include "stdafx.h"

#ifdef __APPLE__
#include <unistd.h>
#include <sys/utsname.h>

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include <ApplicationServices/ApplicationServices.h>
//#include <ApplicationServices/CGDirectDisplay.h>

#include "mgOSXServices.h"

#include "mgPlatformErrorTable.h"
#include "mgOSXGL33Support.h"
#include "mgOSXGL21Support.h"
#include "mgOSXServices.h"

#include "mgApplication.h"

extern const char* mgProgram;
extern const char* mgProgramVersion;

const int MG_LIBRARY_NONE = 0;
const int MG_LIBRARY_OPENGL21 = 1;
const int MG_LIBRARY_OPENGL33 = 2;

const double FPS_REPORT_INTERVAL = 30000.0;  // milliseconds

void termFramework(void);

mgPlatformServices* mgPlatform = NULL;

const char* mgOpenGLVersion(void);

// call into MyOpenGLView
void updateCursorTracksMouse(void);

//--------------------------------------------------------------
// constructor
mgOSXServices::mgOSXServices()
{
  // set global pointer to framework
  mgPlatform = this;
  
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
  m_active = false;
  m_lastCursorX = 0;
  m_lastCursorY = 0;
  m_mouseInside = false;
  
  m_errorTable = new mgPlatformErrorTable();

  m_eventFlags = 0;

  m_sessionStart = mgOSGetTime();

  resetTiming();
}

//--------------------------------------------------------------
// destructor
mgOSXServices::~mgOSXServices()
{
  delete m_errorTable;
  m_errorTable = NULL;

  mgPlatform = NULL;
}

//--------------------------------------------------------------
// set error message table
void mgOSXServices::setErrorTable(
  mgErrorTable* table)
{
  delete m_errorTable;
  m_errorTable = NULL;

  m_errorTable = table;
}

//--------------------------------------------------------------
// return display DPI
int mgOSXServices::getDPI()
{
  return m_DPI;
}

//--------------------------------------------------------------
// set depth of display
void mgOSXServices::setDepthBits(
  int depth)
{
  m_depthBits = depth;
}

//--------------------------------------------------------------
// return depth of display
int mgOSXServices::getDepthBits()
{
  return m_depthBits;
}

//--------------------------------------------------------------
// return list of system font directories
void mgOSXServices::getFontDirectories(
  mgStringArray& fontDirs)
{
  fontDirs.add("/Library/Fonts/");
}

//--------------------------------------------------------------
// set requested display library ("OpenGL3.3", "OpenGL2.1") 
// or NULL for first available in that order.
void mgOSXServices::setDisplayLibrary(
  const char* library)
{
  m_library = library;
}

//--------------------------------------------------------------
// query display library initialized
void mgOSXServices::getDisplayLibrary(
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
void mgOSXServices::initDisplay()
{
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
    mgOSXGL33Support* gl33 = new mgOSXGL33Support();

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
    mgOSXGL21Support* gl21 = new mgOSXGL21Support();

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
  size_t width = CGDisplayPixelsWide(CGMainDisplayID());
  CGSize size = CGDisplayScreenSize(CGMainDisplayID());
  m_DPI = (int) ((width * 25.4) / size.width);
  mgDebug("screen is %d pixels/%d mm = %d dpi", (int) width, (int) size.width, m_DPI);
  m_DPI = max(m_DPI, 72);  // in case monitor info wrong

  mgDebug("");
  mgDebug("------ display initialized.  Return to app: %s", (const char *) m_windowTitle);
}

//--------------------------------------------------------------
// terminate the display
void mgOSXServices::termDisplay()
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
void mgOSXServices::swapBuffers()
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
void mgOSXServices::resetTiming()
{
  m_frameCount = 0;
  m_timingStart = mgOSGetTime();
}

//--------------------------------------------------------------
// write frames per second to log
void mgOSXServices::logTiming(
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
DWORD mgOSXServices::compileGLShaderPair(
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
void mgOSXServices::setFullscreen(
  BOOL fullscreen)
{
  m_fullscreen = fullscreen;
}

//--------------------------------------------------------------
// set app fullscreen or windowed
BOOL mgOSXServices::getFullscreen()
{
  return m_fullscreen;
}

//--------------------------------------------------------------
// set the window title
void mgOSXServices::setWindowTitle(
  const char* title)
{
  m_windowTitle = title;
}

//--------------------------------------------------------------
// get the window title
void mgOSXServices::getWindowTitle(
  mgString& title)
{
  title = m_windowTitle;
}

//--------------------------------------------------------------
// set the window bounds in windowed mode
void mgOSXServices::setWindowBounds(
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
void mgOSXServices::getWindowBounds(
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
void mgOSXServices::setSwapImmediate(
  BOOL swapImmediate)
{
  m_swapImmediate = swapImmediate;
}

//--------------------------------------------------------------
// get swap mode
BOOL mgOSXServices::getSwapImmediate()
{
  return m_swapImmediate;
}

//--------------------------------------------------------------
// enable multisample
void mgOSXServices::setMultiSample(
  BOOL multiSample)
{
  m_multiSample = multiSample;
}

//--------------------------------------------------------------
// get multisample
BOOL mgOSXServices::getMultiSample()
{
  return m_multiSample;
}

//-----------------------------------------------------------------------------
// save screen to file
void mgOSXServices::screenShot(
  const char* fileName)
{
}

//--------------------------------------------------------------
// check for display errors
BOOL mgOSXServices::checkErrors()
{
  if (m_support != NULL)
    return m_support->checkError();

  return false;
}

//--------------------------------------------------------------
// draw a texture on the screen using display coordinates
void mgOSXServices::drawOverlayTexture(
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
// display area resized
void mgOSXServices::displayResized(
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
void mgOSXServices::setMouseRelative(
  BOOL relative)
{
  // if unchanged, do nothing
  if (m_mouseRelative == relative)
    return;
  m_mouseRelative = relative;

  // detach cursor from mouse if relative
  updateCursorTracksMouse();
}

//--------------------------------------------------------------
// set mouse tracking mode
BOOL mgOSXServices::getMouseRelative()
{
  return m_mouseRelative;
}

//--------------------------------------------------------------
// mouse entered window
void mgOSXServices::mouseEntered(
  int x,
  int y)
{
  m_lastCursorX = x;
  m_lastCursorY = y;

  // synch with real cursor again
  if (m_theApp != NULL)
    m_theApp->appMouseEnter(m_lastCursorX, m_lastCursorY);

  m_mouseInside = true;
}

//--------------------------------------------------------------
// mouse exited window
void mgOSXServices::mouseExited()
{
  if (m_theApp != NULL)
    m_theApp->appMouseExit();

  m_mouseInside = false;
}

//--------------------------------------------------------------
// press of mouse button
void mgOSXServices::mouseDown(
  int button)
{
  if (m_theApp == NULL)
    return;
  
  m_eventFlags |= button;
  m_theApp->appMouseDown(button, m_eventFlags);
}

//--------------------------------------------------------------
// release of mouse button
void mgOSXServices::mouseUp(
  int button)
{
  if (m_theApp == NULL)
    return;
  
  m_eventFlags &= ~button;
  m_theApp->appMouseUp(button, m_eventFlags);
}

//--------------------------------------------------------------
// mouse moved, no buttons down
void mgOSXServices::mouseMove(
  int x,
  int y,
  int dx,
  int dy)
{
  if (m_theApp == NULL)
    return;
  
  // we don't always get the first entered message
  if (!m_mouseInside)
    mouseEntered(x, y);
  
  if (m_mouseRelative)
  {
    m_theApp->appMouseMove(dx, dy, m_eventFlags);
  }
  else 
  {
    m_theApp->appMouseMove(x - m_lastCursorX, y - m_lastCursorY, m_eventFlags);
    m_lastCursorX = x;
    m_lastCursorY = y;
  }
}

//--------------------------------------------------------------
// mouse dragged (buttons down)
void mgOSXServices::mouseDrag(
  int x,
  int y,
  int dx,
  int dy)
{
  if (m_theApp == NULL)
    return;
    
  // we don't always get the first entered message
  if (!m_mouseInside)
    mouseEntered(x, y);
  
  if (m_mouseRelative)
  {
    m_theApp->appMouseDrag(dx, dy, m_eventFlags);
  }
  else 
  {
    m_theApp->appMouseDrag(x - m_lastCursorX, y - m_lastCursorY, m_eventFlags);
    m_lastCursorX = x;
    m_lastCursorY = y;
  }
}

//--------------------------------------------------------------
// mouse wheel spin
void mgOSXServices::mouseWheel(
  int delta)
{
  if (m_theApp == NULL)
    return;
  m_theApp->appMouseWheel(delta, m_eventFlags);
}

//--------------------------------------------------------------
// key pressed
void mgOSXServices::keyDown(
  int specialKey,
  int modifiers,
  int charCode)
{
  if (m_theApp == NULL)
    return;

  int flags = modifiers | m_eventFlags;
  if (specialKey != 0)
  {
    m_theApp->appKeyDown(specialKey, flags);
  }
  else    
  {
    m_theApp->appKeyDown(toupper(charCode), flags);
    m_theApp->appKeyChar(charCode, flags);
  }
}

//--------------------------------------------------------------
// key released
void mgOSXServices::keyUp(
  int specialKey,
  int modifiers,
  int charCode)
{
  if (m_theApp == NULL)
    return;
  
  int flags = m_eventFlags | modifiers;
  if (specialKey != 0)
  {
    m_theApp->appKeyUp(specialKey, flags);
  }
  else
  {
    m_theApp->appKeyUp(toupper(charCode), flags);
  }
}

//--------------------------------------------------------------
// initialize application
void mgOSXServices::initView()
{
/*
  // do the initial resize
  mgDisplay->windowResized(m_windowWidth, m_windowHeight);

  // start cursor tracking
  mgDisplay->cursorInitState(m_lastCursorX, m_lastCursorY);

  // initialize view rendering
  mgDisplay->initView();

  // start timing
  resetTiming();
*/
}

//--------------------------------------------------------------
// terminate application
void mgOSXServices::termView()
{
}
        
//--------------------------------------------------------------
// end the app
void mgOSXServices::exitApp()
{
  // =-= close the window?
}

//--------------------------------------------------------------
// get a unique machine id
void getPlatformUUID(
  char* buf, 
  int bufSize) 
{
  io_registry_entry_t ioRegistryRoot = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/");
  
  CFStringRef uuidCf = (CFStringRef) IORegistryEntryCreateCFProperty(ioRegistryRoot, 
    CFSTR(kIOPlatformUUIDKey), kCFAllocatorDefault, 0);
  IOObjectRelease(ioRegistryRoot);
  
  CFStringGetCString(uuidCf, buf, bufSize, kCFStringEncodingMacRoman);
  CFRelease(uuidCf);    
}

//--------------------------------------------------------------
// global initialization called in main.m
void initFramework(
  const char* resourceDir)
{
  chdir(resourceDir);

  mgOSLaunchSendLog();
  
  // initialize random numbers
  time_t seed;
  time(&seed);
  srand(12123123); // srand(seed & 0xFFFF);
  
  mgDebugReset();         // reset trace file

  mgDebug(":Program: %s", mgProgram != NULL ? mgProgram : "unknown");
  mgDebug(":Program Version: %s", mgProgramVersion != NULL ? mgProgramVersion : "unknown");
  
  char machineID[256];
  getPlatformUUID(machineID, sizeof(machineID)-1);
  machineID[sizeof(machineID)-1] = '\0';
  mgDebug(":Machine Id: %s", machineID);
    
  struct utsname name;
  if (uname(&name) != -1)
  {
    mgDebug(":OS: %s", name.sysname);
    mgDebug(":Machine 64-bit: %s", strstr(name.machine, "_64") != NULL ? "true" : "false");
  }
  else mgDebug(":OS: OSX");

  const int MAX_DISPLAYS = 20;
  uint32_t displayCount;
  CGGetActiveDisplayList(MAX_DISPLAYS, NULL, &displayCount);
  mgDebug(":Machine Monitor count: %d", displayCount);

  CGDirectDisplayID mainDisplay = CGMainDisplayID();
  mgDebug(":Machine Monitor res: %d by %d", CGDisplayPixelsWide(mainDisplay), CGDisplayPixelsHigh(mainDisplay));

  mgOSInitTimer();       // performance timer
  
  atexit(termFramework);
}

//--------------------------------------------------------------
// terminate framework
void termFramework(void)
{
  // shutdown
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  mgDebug("------ shutdown");
  platform->logTiming(true);

  double sessionEnd = mgOSGetTime();
  mgDebug(":Session time: %.2f seconds", (sessionEnd - platform->m_sessionStart)/1000.0);
  mgDebug(":Session exit: clean");

/* 
  // too much of the window has already been destroyed by Cocoa.  skip this

  // shutdown
  platform->termView();

  // delete buffers
  platform->m_theApp->appDeleteBuffers();

  platform->m_theApp->appTerm();

  delete platform->m_theApp;
  platform->m_theApp = NULL;

  platform->termDisplay();
*/

  mgBlockPool::freeMemory();
}

#endif
