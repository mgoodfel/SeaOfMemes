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

#include "mgInputEvents.h"
#include "mgUtil/Include/mgUtil.h"
#include "BitmapSurface.h"
#include "mgFontList.h"
#include "SampleUI.h"

#include "FlatWorld.h"

const char* WINDOW_TITLE = "GuiTestWin, Part 60";
const char* WINDOW_CLASS = "GUITESTWIN";

HWND m_window = NULL;
HINSTANCE m_instance;
HDC m_dc;

// saved window size and position
int m_windowWidth = 800;
int m_windowHeight = 600;
int m_windowX = CW_USEDEFAULT;
int m_windowY = CW_USEDEFAULT;

DWORD m_eventFlags = 0;                // mouse and modifier flags
int m_lastMouseX = 0;
int m_lastMouseY = 0;

// forward dcls
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

mgErrorTable* m_errorTable = NULL;
mgFontList* m_fontList = NULL;
mgStringArray m_fontDirs;

// terrain browser
FlatWorld* m_flatWorld = NULL;

// ui state
BitmapSurface* m_surface = NULL;      // surface to render ui into
SampleUI* m_ui = NULL;

const double TURN_SPEED = 120.0/1000;             // degrees per ms
const double MOVE_SPEED = 250.0/1000;               // units per ms

// key begin times
const double INVALID_TIME = -1.0;

double m_leftPress = INVALID_TIME;
double m_rightPress = INVALID_TIME;
double m_upPress = INVALID_TIME;
double m_downPress = INVALID_TIME;
int m_dragMouseX;
int m_dragMouseY;
double m_dragPlayerX;
double m_dragPlayerY;
BOOL m_dragClick;

// to combine application graphics and UI overlay without flicker,
// we need an intermediate bitmap.
HDC m_outputDC;
HBITMAP m_outputBitmap;

//--------------------------------------------------------------
// read the font list
void readFontList()
{
  char winDir[MAX_PATH];
  GetWindowsDirectory(winDir, sizeof(winDir));

  mgString fontPath;
  fontPath.format("%s\\Fonts\\", winDir);
  mgDebug("windows font dir is %s", (const char*) fontPath);

  m_fontDirs.add(fontPath);
  m_fontList = new mgFontList("docs\\fonts.xml");
}

//-----------------------------------------------------------------------------
// find a font file.  called by BitmapSurface
BOOL sysFindFont(
  const char* faceName, 
  BOOL bold, 
  BOOL italic, 
  mgString& fontFile)
{
  return m_fontList->findFont(m_fontDirs, faceName, bold, italic, fontFile);
}

//--------------------------------------------------------------
// create the user interface
void createUI()
{
  readFontList();

  m_surface = new BitmapSurface();
  m_ui = new SampleUI(m_surface, "docs\\help.xml");

  // resize the UI to the window, since window create and initial resize
  // have already happened.
  m_ui->resize(m_windowWidth, m_windowHeight);
}

//--------------------------------------------------------------
// destroy the UI
void destroyUI()
{
  delete m_ui;
  m_ui = NULL;

  delete m_surface;
  m_surface = NULL;

  delete m_fontList;
  m_fontList = NULL;
}

//--------------------------------------------------------------
// map movement key down
void movementKeyDown(
  WORD wParam)
{
  switch (wParam)
  {
    case VK_NEXT:
      m_flatWorld->zoomOut();
      InvalidateRect(m_window, NULL, false);
      break;

    case VK_PRIOR:
      m_flatWorld->zoomIn();
      InvalidateRect(m_window, NULL, false);
      break;

    case VK_LEFT:
      if (m_leftPress == -1)
        m_leftPress = mgOSGetTime();
      break;

    case VK_RIGHT:
      if (m_rightPress = -1)
        m_rightPress = mgOSGetTime();
      break;

    case VK_UP:
      if (m_upPress == -1)
        m_upPress = mgOSGetTime();
      break;

    case VK_DOWN:
      if (m_downPress == -1)
        m_downPress = mgOSGetTime();
      break;
  }
}

//--------------------------------------------------------------
// movement key up
void movementKeyUp(
  WORD wParam)
{
  switch (wParam)
  {
    case VK_LEFT:
      m_leftPress = -1;
      break;

    case VK_RIGHT:
      m_rightPress = -1;
      break;

    case VK_UP:
      m_upPress = -1;
      break;

    case VK_DOWN:
      m_downPress = -1;
      break;
  }
}

//--------------------------------------------------------------
// mouse drag
void movementDrag(
  int x,
  int y)
{
  if (x != m_dragMouseX || y != m_dragMouseY)
    m_dragClick = false;

  double scale = m_flatWorld->m_terrainScale;

  m_flatWorld->setPlayerPosn(
    m_dragPlayerX - (x-m_dragMouseX)*scale, 
    m_dragPlayerY - (y-m_dragMouseY)*scale);

  m_flatWorld->recenterTerrain();
  m_ui->setValue(m_flatWorld->m_playerX, m_flatWorld->m_playerY);
  InvalidateRect(m_window, NULL, false);
}

//--------------------------------------------------------------
// press of mouse button recenters map
void movementPress(
  int x,
  int y)
{
  if (m_flatWorld == NULL)
    return;  // too early

  // start drag
  m_dragMouseX = x;
  m_dragMouseY = y;
  m_dragPlayerX = m_flatWorld->m_playerX;
  m_dragPlayerY = m_flatWorld->m_playerY;

  m_dragClick = true;  // assume click until mouse moves
}

//--------------------------------------------------------------
// release of mouse button 
void movementRelease(
  int x,
  int y)
{
  if (!m_dragClick || x != m_dragMouseX || y != m_dragMouseY)
    return;  // not a click

  // recenter the map at the click point
  double scale = m_flatWorld->m_terrainScale;

  m_flatWorld->setPlayerPosn(
    m_dragPlayerX + (x-m_windowWidth/2)*scale, 
    m_dragPlayerY + (y-m_windowHeight/2)*scale);

  m_flatWorld->recenterTerrain();
  m_ui->setValue(m_flatWorld->m_playerX, m_flatWorld->m_playerY);
  InvalidateRect(m_window, NULL, false);
}

//--------------------------------------------------------------
// update movement 
void updateMovement()
{
  if (m_flatWorld == NULL)
    return;  // too early

  double now = mgOSGetTime();
  BOOL moved = false;
  BOOL turned = false;

  // turn left
  if (m_leftPress != -1)
  {
    m_flatWorld->playerLeft(now-m_leftPress);
    m_leftPress = now;
    turned = true;
  }

  // turn right
  if (m_rightPress != -1)
  {
    m_flatWorld->playerRight(now-m_rightPress);
    m_rightPress = now;
    turned = true;
  }

  // move forwards
  if (m_upPress != -1)
  {
    m_flatWorld->playerForward(now-m_upPress);
    m_upPress = now;
    moved = true;
  }

  // move backwards
  if (m_downPress != -1)
  {
    m_flatWorld->playerBackward(now-m_downPress);
    m_downPress = now;
    moved = true;
  }

  if (moved)
  {
    m_flatWorld->recenterTerrain();
    m_ui->setValue(m_flatWorld->m_playerX, m_flatWorld->m_playerY);
  }

  if (moved || turned)
    InvalidateRect(m_window, NULL, false);
}

//--------------------------------------------------------------
// create window 
void initWindow()
{
  // get application instance
  m_instance = GetModuleHandle(NULL);

  // create and register window class
  WNDCLASS wc;
  memset(&wc, 0, sizeof(wc));
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = (WNDPROC) WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_instance;
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = WINDOW_CLASS;

  // Register the window class
  if (!RegisterClass(&wc))              
    throw new mgException("Failed to register window class.");
}

//--------------------------------------------------------------
// create window 
void createWindow()
{
  DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

  // Adjust Window To true Requested Size
  RECT rect;             
  rect.left = 0;         
  rect.right = m_windowWidth;
  rect.top = 0;          
  rect.bottom = m_windowHeight;
  AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);

  // create the window
  m_window = CreateWindowEx(dwExStyle,
          WINDOW_CLASS, WINDOW_TITLE, dwStyle,
          m_windowX, m_windowY,           // Window Position
          rect.right-rect.left,         // Calculate Adjusted Window Width
          rect.bottom-rect.top,         // Calculate Adjusted Window Height
          NULL,                         // No Parent Window
          NULL,                         // No Menu
          m_instance, NULL);

  // if failed to create window
  if (m_window == NULL)
    throw new mgException("Could not create window.");

  ShowWindow(m_window, SW_SHOW);
  SetFocus(m_window);               // Sets Keyboard Focus To The Window

  m_dc = GetDC(m_window);
  m_outputDC = CreateCompatibleDC(m_dc);
}

//--------------------------------------------------------------
// destroy our window 
void destroyWindow()
{
  if (m_window != NULL)
  {
    DeleteDC(m_outputDC);
    ReleaseDC(m_window, m_dc);
    if (!DestroyWindow(m_window))
      throw new mgException("Failed to destroy window.");
    m_window = NULL;
  }
}

//--------------------------------------------------------------
// terminate windows
void termWindow()
{
  if (!UnregisterClass(WINDOW_CLASS, m_instance))
    throw new mgException("Failed to unregister class.");
}

//--------------------------------------------------------------
// create bitmap for rendering
void createBitmap()
{
  SetGraphicsMode(m_outputDC, GM_ADVANCED);
  m_outputBitmap = CreateCompatibleBitmap(m_dc, m_windowWidth, m_windowHeight);
  SelectObject(m_outputDC, m_outputBitmap);
}

//--------------------------------------------------------------
// destroy bitmap
void destroyBitmap()
{
  // delete the surface DC
  SelectObject(m_outputDC, (HBITMAP) NULL);
  SelectObject(m_outputDC, (HFONT) NULL);
  SelectObject(m_outputDC, (HBRUSH) NULL);
  SelectObject(m_outputDC, (HPEN) NULL);

  if (m_outputBitmap != NULL)
  {
    DeleteObject(m_outputBitmap);
    m_outputBitmap = NULL;
  }
}

//--------------------------------------------------------------
// translate special keys to platform-independent codes
int translateKey(
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
// Process windows messages
LRESULT CALLBACK WndProc(
  HWND  hWnd,                     // Handle For This Window
  UINT  uMsg,                     // Message For This Window
  WPARAM  wParam,                 // Additional Message Information
  LPARAM  lParam)                 // Additional Message Information
{
  /*
    Windows input events are translated into GUI events (see uiInputEvents.h) and
    passed to the SampleUI instance.  If SampleUI does not want an event (mouse or
    keyboard), we can pass it to the application (movementKeyDown, etc.)

    There's nothing in the GUI that decides when an event should go to the GUI
    vs. the application.  This policy is implemented in SampleUI.
  */

  switch (uMsg)                 
  {
    case WM_CLOSE: 
    {
      PostQuitMessage(0); 
      return 0; 
    }

    case WM_SIZE:             
    {
      if (m_window != NULL)
      {
        RECT rect;
        GetClientRect(m_window, &rect);
        m_windowWidth = rect.right - rect.left;
        m_windowHeight = rect.bottom - rect.top;

        // resize the ui
        if (m_ui != NULL)
          m_ui->resize(m_windowWidth, m_windowHeight);

        // resize the terrain browser
        if (m_flatWorld != NULL)
          m_flatWorld->resize(m_windowWidth, m_windowHeight);

        // recreate output bitmap
        destroyBitmap();
        createBitmap();
      }

      return 0;
    }

    case WM_MOVE:
    {
      m_windowX = LOWORD(lParam);
      m_windowY = HIWORD(lParam);
      return 0;
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(m_window, &ps);

      int copyLeft = ps.rcPaint.left;
      int copyTop = ps.rcPaint.top;
      int copyWidth = ps.rcPaint.right - ps.rcPaint.left;
      int copyHeight = ps.rcPaint.bottom - ps.rcPaint.top;
      
      if (m_flatWorld != NULL)
        m_flatWorld->update(m_outputDC, ps.rcPaint);

      // if the UI has been created
      if (m_surface != NULL)
      {
        // if UI has been damaged, merge bitmap into window
        if (copyWidth > 0 && copyHeight > 0)
        {
          BLENDFUNCTION blend;
          blend.BlendFlags = 0;
          blend.BlendOp = AC_SRC_OVER;
          blend.SourceConstantAlpha = 255;
          blend.AlphaFormat = AC_SRC_ALPHA;

          AlphaBlend(m_outputDC, copyLeft, copyTop, copyWidth, copyHeight,
                     m_surface->m_bitmapDC, copyLeft, copyTop, 
                     copyWidth, copyHeight, blend);
        }
      }
      BitBlt(ps.hdc, copyLeft, copyTop, copyWidth, copyHeight, m_outputDC, copyLeft, copyTop, SRCCOPY);
      EndPaint(m_window, &ps);
      return 0;
    }

    case WM_CHAR:
    {
      if (m_ui != NULL)
        m_ui->m_top->surfaceKeyChar((int) wParam, 0);  // =-= modifiers

      return 0;
    }

    case WM_KEYDOWN:
    {
      // set autorepeat flag
      int repeat = (lParam & (1 << 30)) != 0 ? MG_EVENT_KEYREPEAT : 0;
     
      if (wParam == VK_SHIFT)
        m_eventFlags |= MG_EVENT_SHIFT_DOWN;
      else if (wParam == VK_CONTROL)
        m_eventFlags |= MG_EVENT_CNTL_DOWN;
      else if (wParam == VK_MENU)
        m_eventFlags |= MG_EVENT_ALT_DOWN;

      // translate special keys
      int key = translateKey((int) wParam);

      if (m_ui != NULL)
      {
        // handle the help key and console key
        if (key == MG_EVENT_KEY_F1)
          m_ui->toggleHelp();
    
        else if (key == MG_EVENT_KEY_F2)
          m_ui->toggleConsole();

        // if ui is taking input
        else if (m_ui->hasKeyFocus())
          m_ui->m_top->surfaceKeyDown(key, m_eventFlags | repeat);
  
        // otherwise, process as application key
        else movementKeyDown(wParam);
      }

      return 0;
    }

    case WM_KEYUP:
    {
      if (m_ui != NULL && m_ui->hasKeyFocus())
      {
        if (wParam == VK_SHIFT)
          m_eventFlags &= ~MG_EVENT_SHIFT_DOWN;
        else if (wParam == VK_CONTROL)
          m_eventFlags &= ~MG_EVENT_CNTL_DOWN;
        else if (wParam == VK_MENU)
          m_eventFlags &= ~MG_EVENT_ALT_DOWN;

        // translate special keys
        int key = translateKey((int) wParam);

        m_ui->m_top->surfaceKeyUp(key, m_eventFlags);
      }
      else movementKeyUp(wParam);
      return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
      int x = LOWORD(lParam);
      int y = HIWORD(lParam);
        
      int pressed = 0;
      if (uMsg == WM_LBUTTONDOWN)
        pressed = MG_EVENT_MOUSE1_DOWN;
      else if (uMsg == WM_RBUTTONDOWN)
        pressed = MG_EVENT_MOUSE2_DOWN;
      else if (uMsg == WM_MBUTTONDOWN)
        pressed = MG_EVENT_MOUSE3_DOWN;

      m_eventFlags |= pressed;

      if (m_ui != NULL && m_ui->useMousePoint(x, y))
      {
        // =-= if another button already down, should this be a dragged, not moved?
        m_ui->m_top->surfaceMouseMove(x, y, m_eventFlags);
        m_ui->m_top->surfaceMouseDown(x, y, m_eventFlags, pressed);
      }
      else movementPress(x, y);

      m_lastMouseX = x;
      m_lastMouseY = y;
      return 0;
    }

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    {
      int x = LOWORD(lParam);
      int y = HIWORD(lParam);
      
      int released = 0;
      if (uMsg == WM_LBUTTONUP)
        released = MG_EVENT_MOUSE1_DOWN;
      else if (uMsg == WM_RBUTTONUP)
        released = MG_EVENT_MOUSE2_DOWN;
      else if (uMsg == WM_MBUTTONUP)
        released = MG_EVENT_MOUSE3_DOWN;

      m_eventFlags &= ~released;

      if (m_ui != NULL && m_ui->hasMouseFocus())
      {

        // if button released away from last coords, it was dragged
        if (x != m_lastMouseX || y != m_lastMouseY)
          m_ui->m_top->surfaceMouseDrag(x, y, m_eventFlags);

        m_ui->m_top->surfaceMouseUp(x, y, m_eventFlags, released);
      }
      else movementRelease(x, y);

      m_lastMouseX = x;
      m_lastMouseY = y;
      return 0;
    }

    case WM_MOUSEMOVE:
    {
      int x = LOWORD(lParam);
      int y = HIWORD(lParam);

      if (m_ui != NULL && (m_ui->useMousePoint(x, y) || m_ui->hasMouseFocus()))
      {
        if ((m_eventFlags & MG_EVENT_MOUSE_BUTTONS) != 0)
          m_ui->m_top->surfaceMouseDrag(x, y, m_eventFlags);
        else m_ui->m_top->surfaceMouseMove(x, y, m_eventFlags);
      }
      else if ((m_eventFlags & MG_EVENT_MOUSE_BUTTONS) != 0)
        movementDrag(x, y);

      m_lastMouseX = x;
      m_lastMouseY = y;
      return 0;
    }

    case WM_TIMER:
    {
      // do UI animation
      if (m_ui != NULL)
        m_ui->m_top->animate();
      return 0;
    }
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
  mgOSInitTimer();       // performance timer
  
  // initialize random numbers
  time_t seed;
  time(&seed);
  srand(12123123); // srand(seed & 0xFFFF);

  mgOSFindWD("docs");

  // handle utility error messages
  m_errorTable = new mgUtilErrorTable();

  try
  {
    initWindow();
    createWindow();
    createBitmap();
    createUI();

    // create the terrain
    m_flatWorld = new FlatWorld();
    m_flatWorld->resize(m_windowWidth, m_windowHeight);
    m_ui->setValue(m_flatWorld->m_playerX, m_flatWorld->m_playerY);

    // check for screen update every 25 ms
    SetTimer(m_window, 123, 25, NULL);

    while (true)
    {
      MSG msg;      

      // if there is no input pending
      if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
      {
        // update any movement keys still down
        updateMovement();

        // if the ui needs an update
        if (m_surface != NULL && m_surface->isDamaged())
        {
          // redraw ui at damaged area
          mgRectangle bounds;
          m_surface->getDamage(bounds);
          m_ui->m_top->surfacePaint(bounds);

          // copy bits from surface into bitmap
          m_surface->repair(bounds);

          // tell windows to redraw the updated area
          RECT rect;
          rect.left = bounds.m_x;
          rect.right = rect.left + bounds.m_width;
          rect.top = bounds.m_y;
          rect.bottom = rect.top + bounds.m_height;
          InvalidateRect(m_window, &rect, false);
        }
      }

      GetMessage(&msg, NULL, 0, 0);     

      // end on quit
      if (msg.message == WM_QUIT)       
        break;

      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // shutdown
    destroyUI();
    destroyBitmap();
    destroyWindow();
    termWindow();
  }
  catch (mgErrorMsg* e)
  {
    mgString msg;
    m_errorTable->msgText(msg, e);
    mgDebug("%s", (const char*) msg);

    MessageBox(m_window, msg, "Error", MB_OK | MB_ICONINFORMATION);
    return 0;
  }
  catch (mgException* e)
  {
    mgDebug("%s", (const char*) e->m_message);

    MessageBox(m_window, e->m_message, "Error", MB_OK | MB_ICONINFORMATION);
    return 0;
  }

  delete m_errorTable;
  m_errorTable = NULL;

#ifdef DEBUG_MEMORY
  // display all memory leaks
  mgDebugMemory();
#endif

  return 0;
}
