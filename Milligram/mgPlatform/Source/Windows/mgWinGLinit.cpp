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

#include <gl/gl.h>								            // Header File For The OpenGL32 Library

HGLRC mg_GLinit_rc = NULL;                     // Permanent Rendering Context
HDC mg_GLinit_dc = NULL;                       // Private GDI Device Context
HWND m_GLinit_window = NULL;                  // Holds Our Window Handle
HINSTANCE m_GLinit_instance = NULL;           // Holds The Instance Of The Application

const WCHAR* INITGL_WINDOW_CLASS = L"initOpenG";

//--------------------------------------------------------------
// Process windows messages
LRESULT CALLBACK InitGLWndProc( 
  HWND  hWnd,                     // Handle For This Window
  UINT  uMsg,                     // Message For This Window
  WPARAM  wParam,                 // Additional Message Information
  LPARAM  lParam)                 // Additional Message Information
{
  // Pass All Unhandled Messages To DefWindowProc
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//--------------------------------------------------------------
// shutdown the anchor OpenGL window
void mgGLTerm()             
{
  if (mg_GLinit_rc == NULL)
    return;

  wglMakeCurrent(mg_GLinit_dc, NULL);
  wglDeleteContext(mg_GLinit_rc);
  mg_GLinit_rc = NULL;

  ReleaseDC(m_GLinit_window, mg_GLinit_dc);
  mg_GLinit_dc = NULL;

  DestroyWindow(m_GLinit_window);
  m_GLinit_window = NULL;

  UnregisterClass(INITGL_WINDOW_CLASS, m_GLinit_instance);
}

//--------------------------------------------------------------
// create window and initialize OpenGL context
void mgGLInit()
{
  m_GLinit_instance = GetModuleHandle(NULL);  // Grab An Instance For Our Window

  WNDCLASS  wc;                         // Windows Class Structure
  memset(&wc, 0, sizeof(wc));
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;   // Redraw On Move, And Own DC For Window
  wc.lpfnWndProc = (WNDPROC) InitGLWndProc;   // WndProc Handles Messages
  wc.cbClsExtra = 0;                    // No Extra Window Data
  wc.cbWndExtra = 0;                    // No Extra Window Data
  wc.hInstance = m_GLinit_instance;           // Set The Instance
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);    // Load The Default Icon
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // Load The Arrow Pointer
  wc.hbrBackground = NULL;              // No Background Required For GL
  wc.lpszMenuName = NULL;               // We Don't Want A Menu
  wc.lpszClassName = INITGL_WINDOW_CLASS;          // Set The Class Name

  // Attempt To Register The Window Class
  if (!RegisterClass(&wc))              
    throw new mgErrorMsg("winBadRegister", "", "");

  DWORD dwExStyle = WS_EX_APPWINDOW; 
  DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP; 

  m_GLinit_window = CreateWindowEx(dwExStyle,
          INITGL_WINDOW_CLASS,                     // Class Name
          L"OpenGL initialization",                // title
          dwStyle,
          -100, -100,                   // window position
          100, 100,                     // window size
          NULL,                         // parent window
          NULL,                         // menu
          m_GLinit_instance,
          NULL);

  // if failed to create window
  if (m_GLinit_window == NULL)
    throw new mgErrorMsg("winBadCreate", "", "");

  // if failed to get a device context
  mg_GLinit_dc = GetDC(m_GLinit_window);
  if (mg_GLinit_dc == NULL)
  {
    mgGLTerm();
    throw new mgErrorMsg("winBadDC", "", "");
  }

  static PIXELFORMATDESCRIPTOR pfd = 
  {
    sizeof(PIXELFORMATDESCRIPTOR),      // Size Of This Pixel Format Descriptor
    1,                                  // Version Number
    PFD_DRAW_TO_WINDOW |                // Format Must Support Window
    PFD_SUPPORT_OPENGL |                // Format Must Support OpenGL
    PFD_GENERIC_ACCELERATED |           // Hardware accelerated
    PFD_DOUBLEBUFFER,                   // Must Support Double Buffering
    PFD_TYPE_RGBA,                      // Request An RGBA Format
    24,                                 // Select Our Color Depth
    0, 0, 0, 0, 0, 0,                   // Color Bits Ignored
    0,                                  // No Alpha Buffer
    0,                                  // Shift Bit Ignored
    0,                                  // No Accumulation Buffer
    0, 0, 0, 0,                         // Accumulation Bits Ignored
    16,                                 // 16Bit Z-Buffer (Depth Buffer)
    0,                                  // No Stencil Buffer
    0,                                  // No Auxiliary Buffer
    PFD_MAIN_PLANE,                     // Main Drawing Layer
    0,                                  // Reserved
    0, 0, 0                             // Layer Masks Ignored
  };

  // Did Windows Find A Matching Pixel Format?
  int nPixelFormat = ChoosePixelFormat(mg_GLinit_dc, &pfd);
  if (nPixelFormat == 0)
    throw new mgErrorMsg("glChoosePixelFormat", "", "");

  // Are We Able To Set The Pixel Format?
  if (!SetPixelFormat(mg_GLinit_dc, nPixelFormat, &pfd))       
  {
    // Reset The Display
    mgGLTerm();             
    throw new mgErrorMsg("glSetPixelFormat", "", "");
  }

  // Are We Able To Get A Rendering Context?
  mg_GLinit_rc = wglCreateContext(mg_GLinit_dc);         
  if (mg_GLinit_rc == NULL)
  {
    // Reset The Display
    mgGLTerm();             
    throw new mgErrorMsg("glCreateContext", "", "");
  }

  // Try To Activate The Rendering Context
  if (!wglMakeCurrent(mg_GLinit_dc, mg_GLinit_rc))            
  {
    // Reset The Display
    mgGLTerm();             
    throw new mgErrorMsg("glMakeCurrent", "", "");
  }

  const char* glVersionStr = (const char*) glGetString(GL_VERSION);
  if (glVersionStr != NULL)
    mgDebug(":OpenGL init version: %s", glVersionStr);
  else mgDebug("mgGLInit finds GL_VERSION = NULL");
}

//--------------------------------------------------------------
// reset context
void mgGLReset()
{
  // Try To Activate The Rendering Context
  if (!wglMakeCurrent(mg_GLinit_dc, mg_GLinit_rc))            
    throw new mgErrorMsg("glMakeCurrent", "", "");
}