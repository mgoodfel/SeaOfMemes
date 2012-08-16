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
#ifndef MGWINSYSTEM_H
#define MGWINSYSTEM_H

// Windows implementation of app framework

#include "mgPlatformServices.h"

class mgApplication;
class mgFontList;
class mgDisplaySupport;

class mgWinServices: public mgPlatformServices
{
public:
  // constructor
  mgWinServices();
  
  // destructor
  virtual ~mgWinServices();

  // set requested display library ("OpenGL3.3", "OpenGL2.1", "DirectX9") 
  // or NULL for first available in that order.
  virtual void setDisplayLibrary(
    const char* library);

  // query display library initialized
  virtual void getDisplayLibrary(
    mgString& library);

  // set app fullscreen or windowed
  virtual void setFullscreen(
    BOOL fullscreen);

  // query window fullscreen
  virtual BOOL getFullscreen();

  // set the window bounds in windowed mode
  virtual void setWindowBounds(
    int windowX, 
    int windowY, 
    int windowWidth,
    int windowHeight);

  // query window bounds
  virtual void getWindowBounds(
    int& windowX,
    int& windowY,
    int& windowWidth,
    int& windowHeight);

  // SwapBuffers does not wait for vsync
  virtual void setSwapImmediate(
    BOOL graphicsTiming);

  // query swap immediate
  virtual BOOL getSwapImmediate();

  // enable multisample
  virtual void setMultiSample(
    BOOL multiSample);

  // query multisample
  virtual BOOL getMultiSample();

  // set the window title in windowed mode
  virtual void setWindowTitle(
    const char* title);

  // query window title
  virtual void getWindowTitle(
    mgString& windowTitle);

  // set mouse tracking mode
  virtual void setMouseRelative(
    BOOL relative);

  // get mouse tracking mode
  virtual BOOL getMouseRelative();

  // swap display buffers
  virtual void swapBuffers();

  // end the app
  virtual void exitApp();

  // save screen to file
  virtual void screenShot(
    const char* fileName);

  // set error message table
  virtual void setErrorTable(
    mgErrorTable* table);

  // check for display errors
  virtual BOOL checkErrors();

  // return display DPI
  virtual int getDPI();

  // return depth of display
  virtual int getDepthBits();

  // return list of system font directories
  virtual void getFontDirectories(
    mgStringArray& fontDirs);

  // draw a texture on the screen using display coordinates
  virtual void drawOverlayTexture(
    DWORD texture,
    int x, 
    int y,
    int width,
    int height);

  // compile OpenGL shader pair.  noop under DX support
  virtual DWORD compileGLShaderPair(
    const char* vertexSource,             // null-terminated vertex shader source code
    const char* fragmentSource,           // null-termianted fragment shader source code
    int attribCount,
    const char** names,
    const DWORD* indexes);

protected:
  mgApplication* m_theApp;
  mgErrorTable* m_errorTable;

  HINSTANCE m_instance;
  mgString m_windowClass;
  HWND m_window;
  HDC m_dc;

  // display modes
  DEVMODE m_dmInit;                           // initial display mode
  DEVMODE m_dmFull;                           // last fullscreen display mode
  mgString m_monitorDevice;                   // monitor device name

  // mouse tracking
  BOOL m_mouseRelative;
  int m_lastCursorX;
  int m_lastCursorY;
  BOOL m_mouseInside;

  DWORD m_eventFlags;                         // mouse and modifier flags

  // display info
  mgDisplaySupport* m_support;                // display interface
  mgString m_library;                         // display library requested
  int m_libraryFound;                         // display library found

  BOOL m_fullscreen;                          // full screen or window
  BOOL m_multiSample;                         // true for multisampling
  BOOL m_swapImmediate;                       // immediate mode for graphics timing
  int m_depthBits;                            // z buffer depth bits

  // window info
  mgString m_windowTitle; 
  int m_windowWidth;
  int m_windowHeight;
  int m_windowX;
  int m_windowY;

  // performance statistics
  double m_timingStart;
  int m_frameCount;

  BOOL m_active;                              // app taking input
  BOOL m_initialized;                         // app initialized

  // initialize the display
  virtual void initDisplay();

  // terminate display
  virtual void termDisplay();

  // initialize view
  virtual void initView();

  // initialize view
  virtual void termView();

  // find monitor and adapter based on window rect
  virtual void findAdapter();

  // find current display mode in the list
  virtual void findCurrentDisplayMode();

  // restore original display mode
  virtual void restoreDisplayMode();

  // register window class
  virtual void registerWindowClass();

  // create window 
  virtual void createWindow();

  // destroy our window 
  virtual void destroyWindow();

  // remove window class
  virtual void termWindowClass();

  // reset frame rate timing
	virtual void resetTiming();

  // write frames per second to log
	virtual void logTiming(
    BOOL forceReport = true);

  // track mouse exit
  virtual void trackMouse(
    int x,
    int y,
    int& dx,
    int& dy);

  // handle window resize
  virtual void windowResized(
    WPARAM  wParam,
    LPARAM  lParam);

  // handle window move
  virtual void windowMove(
    WPARAM  wParam,
    LPARAM  lParam);

  // translate special keys to platform-independent codes
  virtual int translateKey(
    int key);

  // handle key press
  virtual void keyDown(
    WPARAM  wParam,
    LPARAM  lParam);

  // handle key release
  virtual void keyUp(
    WPARAM  wParam,
    LPARAM  lParam);

  // handle character typed
  virtual void keyChar(
    WPARAM  wParam,
    LPARAM  lParam);

  // handle character typed with alt key down
  virtual void keySys(
    WPARAM  wParam,
    LPARAM  lParam);

  // handle mouse press
  virtual void mouseDown(
    UINT  uMsg,   
    WPARAM  wParam,
    LPARAM  lParam);

  // handle mouse release
  virtual void mouseUp(
    UINT  uMsg,   
    WPARAM  wParam,
    LPARAM  lParam);

  // handle mouse move
  virtual void mouseMove(
    WPARAM  wParam,
    LPARAM  lParam);

  // handle mouse wheel
  virtual void mouseWheel(
    WPARAM  wParam,
    LPARAM  lParam);

  // handle mouse leaving window
  virtual void mouseExit(
    WPARAM  wParam,
    LPARAM  lParam);

  friend int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
  friend LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};

#endif
