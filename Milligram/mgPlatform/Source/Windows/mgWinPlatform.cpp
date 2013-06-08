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

#include "mgApplication.h"
#include "mgUtil/Include/Util/mgBlockPool.h"
#include "mgPlatformErrorTable.h"
#include "mgWinGL33Support.h"
#include "mgWinGL21Support.h"
#include "mgWinPlatform.h"

extern const char* mgProgramName;
extern const char* mgProgramVersion;

const int MG_LIBRARY_NONE = 0;
const int MG_LIBRARY_OPENGL21 = 1;
const int MG_LIBRARY_OPENGL33 = 2;
const int MG_LIBRARY_DIRECTX9 = 3;

const double FPS_REPORT_INTERVAL = 30000.0;  // milliseconds

// instance of the display and services
mgPlatformServices* mgPlatform = NULL;   

// main window handle for any Windows code that needs it (DirectSound)
HWND mgWindow;  

// forward dcls
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);       

//--------------------------------------------------------------
// constructor
mgWinPlatform::mgWinPlatform()
{
  // set global pointer to platform services
  mgPlatform = this;

  m_theApp = NULL;  
  m_instance = NULL;
  m_window = NULL;
  m_dc = NULL;
  m_support = NULL;

  m_windowTitle = "mgFramework";
  m_windowClass = tmpnam(NULL);

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
  m_eventFlags = 0;                // mouse and modifier flags

  m_active = false;
  m_initialized = false;

  resetTiming();

  m_errorTable = new mgPlatformErrorTable();
}

//--------------------------------------------------------------
// destructor
mgWinPlatform::~mgWinPlatform()
{
  delete m_errorTable;
  m_errorTable = NULL;

  mgPlatform = NULL;
}

//--------------------------------------------------------------
// set error message table
void mgWinPlatform::setErrorTable(
  mgErrorTable* table)
{
  delete m_errorTable;
  m_errorTable = NULL;

  m_errorTable = table;
}

//--------------------------------------------------------------
// return display DPI
int mgWinPlatform::getDPI()
{
  if (m_dc != NULL)
  {
    int dpi = GetDeviceCaps(m_dc, LOGPIXELSY);
  //  mgDebug("GetDeviceCaps returns %d dpi", dpi);
    return max(dpi, 72);  // in case monitor info wrong
  }
  else return 72;
}

//--------------------------------------------------------------
// return depth of display
int mgWinPlatform::getDepthBits()
{
  if (m_support != NULL)
    return m_support->m_depthBits;
  else return 0;
}

//--------------------------------------------------------------
// return list of system font directories
void mgWinPlatform::getFontDirectories(
  mgStringArray& fontDirs)
{
  WCHAR winDir[MAX_PATH];
  GetWindowsDirectory(winDir, MAX_PATH);

  mgString fontPath;
  fontPath.format("%ls\\Fonts\\", winDir);

  fontDirs.add(fontPath);
}

//--------------------------------------------------------------
// set requested display library ("OpenGL3.3", "OpenGL2.1", "DirectX9") 
// or NULL for first available in that order.
void mgWinPlatform::setDisplayLibrary(
  const char* library)
{
  m_library = library;
}

//--------------------------------------------------------------
// query display library initialized
void mgWinPlatform::getDisplayLibrary(
  mgString& library)
{
  switch (m_libraryFound)
  {
    case MG_LIBRARY_DIRECTX9: 
      library = "DirectX9";
      break;
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
// initialize display
void mgWinPlatform::initDisplay()
{
  // find and save current display mode
  findCurrentDisplayMode();

  // register the window class and create window
  registerWindowClass();
  createWindow();

  // build error string if library is unknown choice
  mgString libraryChoices;

  // for each supported library, try to initialize it
  m_support = NULL;
  BOOL triedLibrary = false;

  int pixelFormat = 0;
  int depthBits = 0;

#ifdef SUPPORT_GL33
  libraryChoices += "\"OpenGL3.3\" ";
  if (m_support == NULL && (m_library.equalsIgnoreCase("OpenGL3.3") || m_library.isEmpty()))
  {
    triedLibrary = true;
    mgWinGL33Support* gl33 = new mgWinGL33Support();

    // requested gl33 attributes
    gl33->m_dc = m_dc;
    gl33->m_fullscreen = m_fullscreen;
    gl33->m_multiSample = m_multiSample;
    gl33->m_swapImmediate = m_swapImmediate;

    if (gl33->initDisplay())
    {
      m_libraryFound = MG_LIBRARY_OPENGL33;
      m_support = gl33;
    }
    else 
    {
      // if we got through SetPixelFormat, we cannot do it again for OpenGL 2.1
      pixelFormat = gl33->m_pixelFormat;
      depthBits = gl33->m_depthBits;
      delete gl33;
    }
  }
#endif

#ifdef SUPPORT_GL21
  libraryChoices += "\"OpenGL2.1\" ";
  if (m_support == NULL && (m_library.equalsIgnoreCase("OpenGL2.1") || m_library.isEmpty()))
  {
    triedLibrary = true;
    mgWinGL21Support* gl21 = new mgWinGL21Support();

    // requested gl21 attributes
    gl21->m_dc = m_dc;
    gl21->m_fullscreen = m_fullscreen;
    gl21->m_multiSample = m_multiSample;
    gl21->m_swapImmediate = m_swapImmediate;
    gl21->m_pixelFormat = pixelFormat;
    gl21->m_depthBits = depthBits;

    if (gl21->initDisplay())
    {
      m_libraryFound = MG_LIBRARY_OPENGL21;
      m_support = gl21;
    }
    else delete gl21;
  }
#endif

#ifdef SUPPORT_DX9
  libraryChoices += "\"DirectX9\" ";
  if (m_support == NULL && (m_library.equalsIgnoreCase("DirectX9") || m_library.isEmpty()))
  {
    triedLibrary = true;
    mgDX9Support* dx9 = new mgDX9Support();

    // requested display attributes
    dx9->m_dc = m_dc;
    dx9->m_fullscreen = m_fullscreen;
    dx9->m_multiSample = m_multiSample;
    dx9->m_swapImmediate = m_swapImmediate;

    if (dx9->initDisplay())
    {
      m_libraryFound = MG_LIBRARY_DIRECTX9;
      m_support = dx9;
    }
    else delete dx9;
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

  m_support->setGraphicsSize(m_windowWidth, m_windowHeight);

  mgDebug("");
  mgDebug("------ display initialized.  Return to app: %s", (const char*) m_windowTitle);
}

//--------------------------------------------------------------
// terminate the display
void mgWinPlatform::termDisplay()
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
void mgWinPlatform::swapBuffers()
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
void mgWinPlatform::resetTiming()
{
  m_frameCount = 0;
  m_timingStart = mgOSGetTime();
}

//--------------------------------------------------------------
// write frames per second to log
void mgWinPlatform::logTiming(
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
DWORD mgWinPlatform::compileGLShaderPair(
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
// initialize view
void mgWinPlatform::initView()
{
  // initialize cursor position
  POINT posn;
  GetCursorPos(&posn);
  ScreenToClient(m_window, &posn);
  m_lastCursorX = posn.x;
  m_lastCursorY = posn.y;
}

//--------------------------------------------------------------
// terminate view
void mgWinPlatform::termView()
{
}

//--------------------------------------------------------------
// set app fullscreen or windowed
void mgWinPlatform::setFullscreen(
  BOOL fullscreen)
{
  m_fullscreen = fullscreen;
}

//--------------------------------------------------------------
// set app fullscreen or windowed
BOOL mgWinPlatform::getFullscreen()
{
  return m_fullscreen;
}

//--------------------------------------------------------------
// set the window title
void mgWinPlatform::setWindowTitle(
  const char* title)
{
  m_windowTitle = title;

  if (m_window != NULL)
  {
    WCHAR* outText;
    int outLen;
    m_windowTitle.toWCHAR(outText, outLen);

    SetWindowText(m_window, outText);
    delete outText;
  }
}

//--------------------------------------------------------------
// get the window title
void mgWinPlatform::getWindowTitle(
  mgString& title)
{
  title = m_windowTitle;
}

//--------------------------------------------------------------
// set the window bounds in windowed mode
void mgWinPlatform::setWindowBounds(
  int windowX, 
  int windowY, 
  int windowWidth,
  int windowHeight)
{
  m_windowX = windowX;
  m_windowY = windowY;
  m_windowWidth = windowWidth;
  m_windowHeight = windowHeight;
  if (m_window != NULL)
  {
    // if not fullscreen
      // adjust window bounds
  }
}

//--------------------------------------------------------------
// get the window bounds in windowed mode
void mgWinPlatform::getWindowBounds(
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
void mgWinPlatform::setSwapImmediate(
  BOOL swapImmediate)
{
  m_swapImmediate = swapImmediate;
}

//--------------------------------------------------------------
// get swap mode
BOOL mgWinPlatform::getSwapImmediate()
{
  return m_swapImmediate;
}

//--------------------------------------------------------------
// enable multisample
void mgWinPlatform::setMultiSample(
  BOOL multiSample)
{
  m_multiSample = multiSample;
}

//--------------------------------------------------------------
// get multisample
BOOL mgWinPlatform::getMultiSample()
{
  return m_multiSample;
}

//-----------------------------------------------------------------------------
// save screen to file
void mgWinPlatform::screenShot(
  const char* fileName)
{
}

//--------------------------------------------------------------
// check for display errors
BOOL mgWinPlatform::checkErrors()
{
  if (m_support != NULL)
    return m_support->checkError();

  return false;
}

//--------------------------------------------------------------
// draw a texture on the screen using display coordinates
void mgWinPlatform::drawOverlayTexture(
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
// find monitor and adapter based on window rect
void mgWinPlatform::findAdapter()
{
  // fiond the monitor containing the window
  RECT windowRect;
  windowRect.left = m_windowX != CW_USEDEFAULT ? m_windowX : 0;
  windowRect.top = m_windowY != CW_USEDEFAULT ? m_windowY : 0;
  windowRect.right = windowRect.left + m_windowWidth;
  windowRect.bottom = windowRect.top + m_windowHeight;

  HMONITOR monitor = MonitorFromRect(&windowRect, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEX monitorInfo;
  memset(&monitorInfo, 0, sizeof(monitorInfo));
  monitorInfo.cbSize = sizeof(monitorInfo);
  GetMonitorInfo(monitor, &monitorInfo);
  m_monitorDevice = monitorInfo.szDevice;

  mgDebug("running on monitor=%s", (const char*) m_monitorDevice);
}

//--------------------------------------------------------------
// find current display mode in the list
void mgWinPlatform::findCurrentDisplayMode()
{
  mgDebug("------ initialize display mode");

  // find the adapter to use
  findAdapter();

  // Save The Current Display State 
  memset(&m_dmInit, 0, sizeof(m_dmInit));
  m_dmInit.dmSize = sizeof(DEVMODE);

  WCHAR* wideMonitor; int textLen;
  m_monitorDevice.toWCHAR(wideMonitor, textLen);
  EnumDisplaySettings(wideMonitor, ENUM_CURRENT_SETTINGS, &m_dmInit); 

  // set fullscreen display mode to current state
  memset(&m_dmFull, 0, sizeof(m_dmFull));
  m_dmFull.dmSize = sizeof(DEVMODE);
  EnumDisplaySettings(wideMonitor, ENUM_CURRENT_SETTINGS, &m_dmFull); 

  mgDebug("current display mode: %d by %d by %d, %d hz, flags=%x", 
      m_dmFull.dmPelsWidth, m_dmFull.dmPelsHeight, m_dmFull.dmBitsPerPel, m_dmFull.dmDisplayFrequency,
      m_dmFull.dmDisplayFlags);

  // don't set all fields when we ChangeDisplayMode
  m_dmInit.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
  m_dmFull.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

  // find the current display mode in the list
  DEVMODE dm;
  memset(&dm, 0, sizeof(dm));
  dm.dmSize = sizeof(DEVMODE);
  BOOL found = false;
  for (int i = 0; ; i++)
  {
    if (!EnumDisplaySettings(wideMonitor, i, &dm))
      break;

    if (dm.dmBitsPerPel == m_dmFull.dmBitsPerPel &&
        dm.dmPelsWidth == m_dmFull.dmPelsWidth &&
        dm.dmPelsHeight == m_dmFull.dmPelsHeight)
    {
      found = true;
      break;
    }

  }
  // fail if we can't find display mode!
  if (!found)
  {
    throw new mgErrorMsg("winDisplayMode", "width,height,depth,freq,flags", "%d,%d,%d,%d,%d", 
      m_dmFull.dmPelsWidth, m_dmFull.dmPelsHeight, m_dmFull.dmBitsPerPel, m_dmFull.dmDisplayFrequency,
      m_dmFull.dmDisplayFlags);
  }

  delete wideMonitor;
}

//--------------------------------------------------------------
// restore original display mode
void mgWinPlatform::restoreDisplayMode()
{
  // reset display mode to original settings
  if (m_fullscreen)
  {
    WCHAR* wideMonitor; int wideLen;
    m_monitorDevice.toWCHAR(wideMonitor, wideLen);

    ChangeDisplaySettingsEx(wideMonitor, &m_dmInit, NULL, 0, NULL);
    delete wideMonitor;
  }
}

//--------------------------------------------------------------
// register window class
void mgWinPlatform::registerWindowClass()
{
  // get application instance
  m_instance = GetModuleHandle(NULL);

  // create and register window class
  WNDCLASS wc;
  memset(&wc, 0, sizeof(wc));
  wc.style = CS_HREDRAW | CS_VREDRAW;// | CS_OWNDC;
  wc.lpfnWndProc = (WNDPROC) WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_instance;
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wc.hCursor = NULL; // LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;

  WCHAR* outText; int outLen;
  m_windowClass.toWCHAR(outText, outLen);

  wc.lpszClassName = outText;

  // Register the window class
  BOOL ok = RegisterClass(&wc);
  delete outText;

  if (!ok)
    throw new mgErrorMsg("winBadRegister", "", "");
}

//--------------------------------------------------------------
// create window 
void mgWinPlatform::createWindow()
{
  DWORD dwExStyle = 0;
  DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  if (m_fullscreen)    
  {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST; 
    dwStyle |= WS_POPUP; 
  }
  else
  {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; 
    dwStyle |= WS_OVERLAPPEDWINDOW; 
  }

  int createX, createY, createWidth, createHeight;

  if (m_fullscreen)
  {
    // set window size and position to match display mode
    createX = m_dmFull.dmPosition.x;
    createY = m_dmFull.dmPosition.y;
    createWidth = m_dmFull.dmPelsWidth;
    createHeight = m_dmFull.dmPelsHeight;

    // Try to set selected display mode NOTE: CDS_FULLSCREEN gets rid of task bar.
    WCHAR* wideMonitor; int wideLen;
    m_monitorDevice.toWCHAR(wideMonitor, wideLen);
    int rc = ChangeDisplaySettingsEx(wideMonitor, &m_dmFull, NULL, CDS_FULLSCREEN, NULL);
    delete wideMonitor;

    if (rc != DISP_CHANGE_SUCCESSFUL)
      throw new mgErrorMsg("winFullscreen", "", "");
  }
  else
  {
    // use last window position and adjust window size
    RECT rect;             
    rect.left = m_windowX;         
    rect.top = m_windowY;
    rect.right = m_windowX + m_windowWidth;   
    rect.bottom = m_windowY + m_windowHeight;  

    AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);   
    createX = rect.left;
    createY = rect.top;
    createWidth = rect.right - rect.left;
    createHeight = rect.bottom - rect.top;
  }

  mgDebug("create window at (%d, %d) (%d by %d)", createX, createY, createWidth, createHeight);

  WCHAR* wideWindowClass; int outLen;
  m_windowClass.toWCHAR(wideWindowClass, outLen);
  WCHAR* wideWindowTitle;
  m_windowTitle.toWCHAR(wideWindowTitle, outLen);

  // create the window
  m_window = CreateWindowEx(dwExStyle,
          wideWindowClass, 
          wideWindowTitle,
          dwStyle,
          createX, createY,           // Window Position
          createWidth, createHeight, 
          NULL,                         // No Parent Window
          NULL,                         // No Menu
          m_instance, NULL);

  mgWindow = m_window;  // =-= debug

  delete wideWindowClass;
  delete wideWindowTitle;

  // if failed to create window
  if (m_window == NULL)
    throw new mgErrorMsg("winBadCreate", "", "");

  mgDebug("window created");
  m_dc = GetDC(m_window);

  // display the window and take the keyboard focus
  ShowWindow(m_window, SW_SHOW);
  SetFocus(m_window);

  m_mouseInside = false;
    
  if (m_mouseRelative)
  {
    // capture the mouse, force to center of window
    SetCapture(m_window);
    
    POINT posn;
    posn.x = m_windowWidth/2;
    posn.y = m_windowHeight/2;
    m_lastCursorX = posn.x;
    m_lastCursorY = posn.y;
    ClientToScreen(m_window, &posn);
    SetCursorPos(posn.x, posn.y);
  }
}

//--------------------------------------------------------------
// destroy our window 
void mgWinPlatform::destroyWindow()
{
  if (m_window != NULL)
  {
    ReleaseDC(m_window, m_dc);
    m_dc = NULL;

    DestroyWindow(m_window);
    m_window = NULL;
  }
}

//--------------------------------------------------------------
// terminate windows
void mgWinPlatform::termWindowClass()
{
  WCHAR* wideWindowClass; int wideLen;
  m_windowClass.toWCHAR(wideWindowClass, wideLen);
  BOOL ok = UnregisterClass(wideWindowClass, m_instance);
  delete wideWindowClass;

  if (!ok)
    throw new mgErrorMsg("winUnregister", "", "");
}

//--------------------------------------------------------------
// set mouse tracking mode
void mgWinPlatform::setMouseRelative(
  BOOL relative)
{
  // if unchanged, do nothing
  if (m_mouseRelative == relative)
    return;
  m_mouseRelative = relative;

  if (m_mouseRelative)
  {
    // capture the mouse, force to center of window
    SetCapture(m_window);
    
    POINT posn;
    posn.x = m_windowWidth/2;
    posn.y = m_windowHeight/2;
    if (!m_mouseInside)
    {
      m_lastCursorX = posn.x;
      m_lastCursorY = posn.y;
    }
    ClientToScreen(m_window, &posn);
    SetCursorPos(posn.x, posn.y);
  }
  else 
  {
    // restore old mouse position and release capture
    POINT posn;
    posn.x = m_lastCursorX;
    posn.y = m_lastCursorY;
    ClientToScreen(m_window, &posn);
    SetCursorPos(posn.x, posn.y);

    // synch with real cursor again
    if (m_theApp != NULL)
      m_theApp->appMouseEnter(m_lastCursorX, m_lastCursorY);
    ReleaseCapture();
  }
}

//--------------------------------------------------------------
// get mouse tracking mode
BOOL mgWinPlatform::getMouseRelative()
{
  return m_mouseRelative;
}

//--------------------------------------------------------------
// track mouse exit
void mgWinPlatform::trackMouse(
  int x,
  int y,
  int& dx,
  int& dy)
{
  // if this is first point after WM_MOUSELEAVE, do setup
  if (!m_mouseInside)
  {
    m_mouseInside = true;

    SetCursor(NULL);

    if (m_theApp != NULL)
      m_theApp->appMouseEnter(m_lastCursorX, m_lastCursorY);
        
    TRACKMOUSEEVENT track;
    memset(&track, 0, sizeof(track));
    track.cbSize = sizeof(track);
    track.dwFlags = TME_LEAVE;
    track.hwndTrack = m_window;
    TrackMouseEvent(&track);
  }
  
  if (m_mouseRelative)
  {
    // keep cursor in center of window, figure delta from motion
    POINT posn;
    GetCursorPos(&posn);
    ScreenToClient(m_window, &posn);
    dx = posn.x - m_windowWidth/2;
    dy = posn.y - m_windowHeight/2;
    if (dx != 0 || dy != 0)
    {
      posn.x = m_windowWidth/2;
      posn.y = m_windowHeight/2;
      ClientToScreen(m_window, &posn);
      SetCursorPos(posn.x, posn.y);
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
// handle window resize
void mgWinPlatform::windowResized(
  WPARAM  wParam,
  LPARAM  lParam)
{
  if (wParam == SIZE_MINIMIZED)
  {
    m_active = false;
    return;   // ignore size
  }

  m_active = true;        
  int width = LOWORD(lParam);
  int height = HIWORD(lParam);

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
// handle window move
void mgWinPlatform::windowMove(
  WPARAM  wParam,
  LPARAM  lParam)
{
  if (!m_fullscreen)
  {
    m_windowX = LOWORD(lParam);
    m_windowY = HIWORD(lParam);
  }
}

//--------------------------------------------------------------
// translate special keys to platform-independent codes
int mgWinPlatform::translateKey(
  int key)
{
  switch (key)
  {
    case VK_F1: return MG_EVENT_KEY_F1;
    case VK_F2: return MG_EVENT_KEY_F2;
    case VK_F3: return MG_EVENT_KEY_F3;
    case VK_F4: return MG_EVENT_KEY_F4;
    case VK_F5: return MG_EVENT_KEY_F5;
    case VK_F6: return MG_EVENT_KEY_F6;
    case VK_F7: return MG_EVENT_KEY_F7;
    case VK_F8: return MG_EVENT_KEY_F8;
    case VK_F9: return MG_EVENT_KEY_F9;
    case VK_F10: return MG_EVENT_KEY_F10;
    case VK_F11: return MG_EVENT_KEY_F11;
    case VK_F12: return MG_EVENT_KEY_F12;

    case VK_LEFT: return MG_EVENT_KEY_LEFT;
    case VK_RIGHT: return MG_EVENT_KEY_RIGHT;
    case VK_UP: return MG_EVENT_KEY_UP;
    case VK_DOWN: return MG_EVENT_KEY_DOWN;

    case VK_BACK: return MG_EVENT_KEY_BACKSPACE;
    case VK_INSERT: return MG_EVENT_KEY_INSERT;
    case VK_DELETE: return MG_EVENT_KEY_DELETE;
    case VK_HOME: return MG_EVENT_KEY_HOME;
    case VK_END: return MG_EVENT_KEY_END;
    case VK_PRIOR: return MG_EVENT_KEY_PAGEUP;
    case VK_NEXT: return MG_EVENT_KEY_PAGEDN;

    case VK_RETURN: return MG_EVENT_KEY_RETURN;
    case VK_TAB: return MG_EVENT_KEY_TAB;
    case VK_ESCAPE: return MG_EVENT_KEY_ESCAPE;

    case VK_NUMPAD0: return MG_EVENT_KEY_INSERT;
    case VK_NUMPAD1: return MG_EVENT_KEY_END;
    case VK_NUMPAD2: return MG_EVENT_KEY_DOWN;
    case VK_NUMPAD3: return MG_EVENT_KEY_PAGEDN;
    case VK_NUMPAD4: return MG_EVENT_KEY_LEFT;
    case VK_NUMPAD5: return ' ';
    case VK_NUMPAD6: return MG_EVENT_KEY_RIGHT;
    case VK_NUMPAD7: return MG_EVENT_KEY_HOME;
    case VK_NUMPAD8: return MG_EVENT_KEY_UP;
    case VK_NUMPAD9: return MG_EVENT_KEY_PAGEUP;
    case VK_MULTIPLY: return '*';
    case VK_ADD: return '+';
    case VK_SEPARATOR: return MG_EVENT_KEY_RETURN;
    case VK_SUBTRACT: return '-';
    case VK_DECIMAL: return MG_EVENT_KEY_DELETE;
    case VK_DIVIDE: return '/';

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

    default: return key;
  }
}

//--------------------------------------------------------------
// handle key press
void mgWinPlatform::keyDown(
  WPARAM  wParam,
  LPARAM  lParam)
{
  // set autorepeat flag
  int repeat = (lParam & (1 << 30)) != 0 ? MG_EVENT_KEYREPEAT : 0;
//  mgDebug("keydown %d repeat=%s", wParam, repeat?"true":"false");
     
  if (wParam == VK_SHIFT)
    m_eventFlags |= MG_EVENT_SHIFT_DOWN;
  else if (wParam == VK_CONTROL)
    m_eventFlags |= MG_EVENT_CNTL_DOWN;
  else if (wParam == VK_MENU)
    m_eventFlags |= MG_EVENT_ALT_DOWN;

  // translate special keys
  int key = translateKey((int) wParam);

  if (m_theApp != NULL)
  {
    m_theApp->appKeyDown(key, m_eventFlags | repeat);
  }
}

//--------------------------------------------------------------
// handle key release
void mgWinPlatform::keyUp(
  WPARAM  wParam,
  LPARAM  lParam)
{
  if (wParam == VK_SHIFT)
    m_eventFlags &= ~MG_EVENT_SHIFT_DOWN;
  else if (wParam == VK_CONTROL)
    m_eventFlags &= ~MG_EVENT_CNTL_DOWN;
  else if (wParam == VK_MENU)
    m_eventFlags &= ~MG_EVENT_ALT_DOWN;

  // translate special keys
  int key = translateKey((int) wParam);

  if (m_theApp != NULL)
  {
    m_theApp->appKeyUp(key, m_eventFlags);
  }
}

//--------------------------------------------------------------
// handle character typed
void mgWinPlatform::keyChar(
  WPARAM  wParam,
  LPARAM  lParam)
{
// mgDebug("char %d", wParam);
  if (m_theApp != NULL)
  {
    m_theApp->appKeyChar((int) wParam, 0);  // =-= modifiers
  }
}

//--------------------------------------------------------------
// handle character typed with alt-key down
void mgWinPlatform::keySys(
  WPARAM  wParam,
  LPARAM  lParam)
{
// mgDebug("char %d", wParam);
//  if (m_theApp != NULL)
//  {
//    m_theApp->keyChar((int) wParam, 0);  // =-= with alt modifier
//  }
}

//--------------------------------------------------------------
// handle mouse press
void mgWinPlatform::mouseDown(
  UINT  uMsg,   
  WPARAM  wParam,
  LPARAM  lParam)
{
  int dx, dy;
  trackMouse(LOWORD(lParam), HIWORD(lParam), dx, dy);

  int pressed = 0;
  if (uMsg == WM_LBUTTONDOWN)
    pressed = MG_EVENT_MOUSE1_DOWN;
  else if (uMsg == WM_RBUTTONDOWN)
    pressed = MG_EVENT_MOUSE2_DOWN;
  else if (uMsg == WM_MBUTTONDOWN)
    pressed = MG_EVENT_MOUSE3_DOWN;

  m_eventFlags |= pressed;

  // =-= if another button already down, should this be a dragged, not moved?
  if (dx != 0 || dy != 0 && m_theApp != NULL)
    m_theApp->appMouseMove(dx, dy, m_eventFlags);

  if (m_theApp != NULL)
    m_theApp->appMouseDown(pressed, m_eventFlags);

}

//--------------------------------------------------------------
// handle mouse release
void mgWinPlatform::mouseUp(
  UINT  uMsg,   
  WPARAM  wParam,
  LPARAM  lParam)
{
  int dx, dy;
  trackMouse(LOWORD(lParam), HIWORD(lParam), dx, dy);

  int released = 0;
  if (uMsg == WM_LBUTTONUP)
    released = MG_EVENT_MOUSE1_DOWN;
  else if (uMsg == WM_RBUTTONUP)
    released = MG_EVENT_MOUSE2_DOWN;
  else if (uMsg == WM_MBUTTONUP)
    released = MG_EVENT_MOUSE3_DOWN;

  m_eventFlags &= ~released;

  if (m_theApp != NULL)
  {
    // if button released away from last coords, it was dragged
    if (dx != 0 || dy != 0)
      m_theApp->appMouseDrag(dx, dy, m_eventFlags);

    m_theApp->appMouseUp(released, m_eventFlags);
  }
}

//--------------------------------------------------------------
// handle mouse movement
void mgWinPlatform::mouseMove(
  WPARAM  wParam,
  LPARAM  lParam)
{
  int dx, dy;
  trackMouse(LOWORD(lParam), HIWORD(lParam), dx, dy);

  if (m_theApp != NULL)
  {
    if ((m_eventFlags & MG_EVENT_MOUSE_BUTTONS) != 0)
      m_theApp->appMouseDrag(dx, dy, m_eventFlags);
    else m_theApp->appMouseMove(dx, dy, m_eventFlags);
  }
}

//--------------------------------------------------------------
// handle mouse wheel
void mgWinPlatform::mouseWheel(
  WPARAM  wParam,
  LPARAM  lParam)
{
  if (m_theApp != NULL)
    m_theApp->appMouseWheel((short) HIWORD(wParam), m_eventFlags);
}

//--------------------------------------------------------------
// handle mouse leaving the window
void mgWinPlatform::mouseExit(
  WPARAM  wParam,
  LPARAM  lParam)
{
  if (m_theApp != NULL)
    m_theApp->appMouseExit();

  m_mouseInside = false;
}

//--------------------------------------------------------------
// end the app
void mgWinPlatform::exitApp()
{
  PostQuitMessage(0);
}

//--------------------------------------------------------------
// Process windows messages
LRESULT CALLBACK WndProc(
  HWND  hWnd,                     // Handle For This Window
  UINT  uMsg,                     // Message For This Window
  WPARAM  wParam,                 // Additional Message Information
  LPARAM  lParam)                 // Additional Message Information
{
  mgWinPlatform* platform = (mgWinPlatform*) mgPlatform;

  switch (uMsg)                 
  {
    case WM_CLOSE:   // Window has closed
    {
      PostQuitMessage(0); 
      return 0; 
    }

    case WM_SIZE:    // Window has resized
    {
      platform->windowResized(wParam, lParam);
      return 0;
    }

    case WM_MOVE:   // Window has moved
    {
      platform->windowMove(wParam, lParam);
      return 0;
    }

    case WM_PAINT:  // Window repaint
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);

      if (!platform->m_active)
        FillRect(ps.hdc, &ps.rcPaint, (HBRUSH) GetStockObject(WHITE_BRUSH));

      EndPaint(hWnd, &ps);
      return 0;
    }

    case WM_SYSCOMMAND:
    {
      // Intercept System Commands
      switch (wParam)           
      {
        case SC_SCREENSAVE: 
          // Screensaver Trying To Start?
        case SC_MONITORPOWER:       
          // Monitor Trying To Enter Powersave?
          return 0;         // Prevent From Happening
      }
      break; 
    }

    case WM_SYSKEYDOWN:
    {
      platform->keySys(wParam, lParam);
      return 0;
    }

    case WM_CHAR:
      platform->keyChar(wParam, lParam);
      return 0;

    case WM_KEYDOWN:
      platform->keyDown(wParam, lParam);
      return 0;

    case WM_KEYUP:
      platform->keyUp(wParam, lParam);
      return 0;

    case WM_MOUSELEAVE:
      platform->mouseExit(wParam, lParam);
      return 0;
      
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      platform->mouseDown(uMsg, wParam, lParam);
      return 0;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      platform->mouseUp(uMsg, wParam, lParam);
      return 0;

    case WM_MOUSEMOVE:
      platform->mouseMove(wParam, lParam);
      return 0;

    case WM_MOUSEWHEEL:
      platform->mouseWheel(wParam, lParam);
      return 0;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//--------------------------------------------------------------
// convert unhandled exceptions into C++ exceptions
LONG WINAPI MyUnhandledExceptionFilter(
  struct _EXCEPTION_POINTERS *info)
{
  PEXCEPTION_RECORD32 record = (PEXCEPTION_RECORD32) info->ExceptionRecord;

// don't feel like retyping all these codes
#define EXCASE(name) case name: throw #name;

  switch (record->ExceptionCode)
  {
    EXCASE(EXCEPTION_ACCESS_VIOLATION)
    EXCASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED)
    EXCASE(EXCEPTION_BREAKPOINT)
    EXCASE(EXCEPTION_DATATYPE_MISALIGNMENT)
    EXCASE(EXCEPTION_FLT_DENORMAL_OPERAND)
    EXCASE(EXCEPTION_FLT_DIVIDE_BY_ZERO)
    EXCASE(EXCEPTION_FLT_INEXACT_RESULT)
    EXCASE(EXCEPTION_FLT_INVALID_OPERATION)
    EXCASE(EXCEPTION_FLT_OVERFLOW)
    EXCASE(EXCEPTION_FLT_STACK_CHECK)
    EXCASE(EXCEPTION_FLT_UNDERFLOW)
    EXCASE(EXCEPTION_ILLEGAL_INSTRUCTION)
    EXCASE(EXCEPTION_IN_PAGE_ERROR)
    EXCASE(EXCEPTION_INT_DIVIDE_BY_ZERO)
    EXCASE(EXCEPTION_INT_OVERFLOW)
    EXCASE(EXCEPTION_INVALID_DISPOSITION)
    EXCASE(EXCEPTION_NONCONTINUABLE_EXCEPTION)
    EXCASE(EXCEPTION_PRIV_INSTRUCTION)
    EXCASE(EXCEPTION_SINGLE_STEP)
    EXCASE(EXCEPTION_STACK_OVERFLOW)
    default: throw "Unknown ExceptionCode";
  }
#undef EXCASE

  return EXCEPTION_EXECUTE_HANDLER;
}

//--------------------------------------------------------------
// report program/os/machine information
void systemReport()
{
  mgDebug(":Program: %s", mgProgramName != NULL ? mgProgramName : "unknown");
  mgDebug(":Program Version: %s", mgProgramVersion != NULL ? mgProgramVersion : "unknown");

  OSVERSIONINFOEX osVersion;
  memset(&osVersion, 0, sizeof(osVersion));
  osVersion.dwOSVersionInfoSize = sizeof(osVersion);
  GetVersionEx((OSVERSIONINFO*) &osVersion);
  mgDebug(":OS: Windows");
  mgDebug(":OS Windows version: %d.%d", osVersion.dwMajorVersion, osVersion.dwMinorVersion);
  mgDebug(":OS Windows build: %d", osVersion.dwBuildNumber);
  mgDebug(":OS Windows service pack: %s %d.%d", osVersion.szCSDVersion, osVersion.wServicePackMajor, osVersion.wServicePackMinor);

  // get a unique machine id
  HKEY hKey;
  LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ | KEY_WOW64_64KEY, &hKey);
  if (lRes == ERROR_SUCCESS)
  {
    WCHAR machineGUID[256];
    DWORD len = sizeof(machineGUID)-1;
    DWORD valueType;
    lRes = RegQueryValueEx(hKey, L"MachineGuid", 0, &valueType, (LPBYTE)machineGUID, &len);
    if (lRes == ERROR_SUCCESS && valueType == REG_SZ)
    {
      mgDebug(":Machine Id: %ls", machineGUID);
    }
  }
  
#ifdef _WIN64
  mgDebug(":Machine 64-bit: true");
#else
  BOOL is64Bit;
  if (IsWow64Process(GetCurrentProcess(), &is64Bit))
    mgDebug(":Machine 64-bit: %s", is64Bit?"true":"false");
#endif

  SYSTEM_INFO si;
  memset(&si, 0, sizeof(si));
  GetSystemInfo(&si);
  mgDebug(":Machine CPU count: %d", si.dwNumberOfProcessors);

  mgDebug(":Machine Monitor count: %d", GetSystemMetrics(SM_CMONITORS));
  mgDebug(":Machine Monitor res: %d by %d", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

  DISPLAY_DEVICE device;
  memset(&device, 0, sizeof(device));
  device.cb = sizeof(device);
  if (EnumDisplayDevices(NULL, 0, &device, 0))
    mgDebug(":Machine Display: %ls", device.DeviceString);
}

//--------------------------------------------------------------
// list the system font directories
void listFontDirs(
  mgWinPlatform* platform)
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
int WINAPI WinMain( 
  HINSTANCE hInstance,            // Instance
  HINSTANCE hPrevInstance,        // Previous Instance
  LPSTR lpCmdLine,                // Command Line Parameters
  int nCmdShow)                   // Window Show State
{
#ifdef DEBUG_MEMORY
  mgDebugMemoryInit();
#endif

  mgDebugReset();         // reset trace file

  // catch all exceptions
  SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

  // start the sendlog program if available
  mgOSLaunchSendLog();

  systemReport();
  mgOSInitTimer();       // performance timer
  double startTime = mgOSGetTime();

  mgDebug("");
    
  // initialize random numbers
  time_t seed;
  time(&seed);
  srand(12123123); // srand(seed & 0xFFFF);
  
  mgWinPlatform* platform = NULL;  // defined for internal use
  try
  {
    // initialize platform services
    platform = new mgWinPlatform();

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

    // process Windows messages until done
    while (true)
    {
      MSG msg;

      if (platform->m_active)
      {
        while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
        {
          platform->m_theApp->appIdle();
        }
      }

      GetMessage(&msg, NULL, 0, 0);     
    
      // end on quit
      if (msg.message == WM_QUIT)       
        break;

      TranslateMessage(&msg);
      DispatchMessage(&msg);
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

    platform->termWindowClass();

    platform->restoreDisplayMode();

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

    WCHAR* outText;
    int outLen;
    msg.toWCHAR(outText, outLen);

    MessageBox(platform->m_window, outText, L"Error", MB_OK | MB_ICONINFORMATION);
    delete outText;
    return -1;
  }
  catch (mgException* e)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    mgDebug(":Session exit: %s", (const char*) e->m_message);

    WCHAR* outText;
    int outLen;
    e->m_message.toWCHAR(outText, outLen);

    MessageBox(platform->m_window, outText, TEXT("Error"), MB_OK | MB_ICONINFORMATION);
    delete outText;
    return -2;
  }
  catch (const char* msg)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    if (msg == NULL)  
      msg = "null exception";
    mgDebug(":Session exit: %s", msg);

    mgString msgStr;
    msgStr.format("Low-level exception: %s", msg);

    WCHAR* outText;
    int outLen;
    msgStr.toWCHAR(outText, outLen);

    MessageBox(platform->m_window, outText, TEXT("Error"), MB_OK | MB_ICONINFORMATION);
    delete outText;
    return -3;
  }
  catch (...)
  {
    double endTime = mgOSGetTime();
    mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

    mgDebug(":Session exit: \"...\" exception");
    MessageBox(platform->m_window, TEXT("Unhandled exception"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
    return -4;
  }

  double endTime = mgOSGetTime();
  mgDebug(":Session time: %.2f seconds", (endTime - startTime)/1000.0);

  mgDebug(":Session exit: clean");

#ifdef DEBUG_MEMORY
  // display all memory leaks
  mgDebugMemory();
#endif

  return 0;
}
