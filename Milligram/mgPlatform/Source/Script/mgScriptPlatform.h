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
#if !defined(MGSCRIPTPLATFORM_H) && defined(EMSCRIPTEN)
#define MGSCRIPTPLATFORM_H

class mgApplication;
class mgDisplaySupport;

int main(int argc, char** argv);
void mgShutdown();
void mgIdle();
void mgViewResized(int width, int height);
void mgMouseMoveAbs(int x, int y, int flags);
void mgMouseMoveRel(int dx, int dy, int flags);
void mgMouseDown(int button, int flags);
void mgMouseUp(int button, int flags);
void mgKeyDown(int keyCode, int flags);
void mgKeyUp(int keyCode, int flags);
void mgKeyChar(int unicode, int flags);

// WebGL implementation of app framework

class mgScriptPlatform: public mgPlatformServices
{
public:
  // constructor
  mgScriptPlatform();
  
  // destructor
  virtual ~mgScriptPlatform();

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

  // process events 
  void processSDLEvents();

protected:
  mgApplication* m_theApp;
  mgErrorTable* m_errorTable;

  // display info
  mgDisplaySupport* m_support;                // display interface
  mgString m_library;                         // display library requested
  int m_libraryFound;                         // display library found

  BOOL m_fullscreen;                          // full screen or window
  BOOL m_multiSample;                         // true for multisampling
  BOOL m_swapImmediate;                       // immediate mode for graphics timing
  int m_depthBits;                            // z buffer depth bits
  int m_DPI;                                  // display DPI

  // window info
  mgString m_windowTitle; 
  int m_windowWidth;
  int m_windowHeight;
  int m_windowX;
  int m_windowY;

  // performance statistics
  double m_startTime;
  double m_timingStart;
  int m_frameCount;

  // mouse tracking
  BOOL m_mouseRelative;
  BOOL m_active;
  BOOL m_initialized;
  int m_lastCursorX;
  int m_lastCursorY;
  BOOL m_mouseInside;
  
  BOOL m_shutdown;
  int m_lastKeyDown;

  // scan directory for font files
  virtual void scanFontDir(
    mgStringArray& fontDirs,
    const char* dirName);

  // reset frame rate timing
  virtual void resetTiming();

  // write frames per second to log
  virtual void logTiming(
    BOOL forceReport = true);

  // initialize the display
  virtual void initDisplay();

  // terminate display
  virtual void termDisplay();

  // initialize view
  virtual void initView();

  // terminate view
  virtual void termView();

  // do idle loop processing
  virtual void idle();

  // handle window resize
  virtual void windowResized(
    int width,
    int height);

  // create window 
  virtual void createWindow();

  // destroy our window 
  virtual void destroyWindow();

  // handle mouse move
  virtual void mouseMoveAbs(
    int x, 
    int y,
    int flags);

  // handle mouse move relative
  virtual void mouseMoveRel(
    int dx, 
    int dy,
    int flags);

  // handle mouse press
  virtual void mouseDown(
    int button,
    int flags);

  // handle mouse release
  virtual void mouseUp(
    int button,
    int flags);

  // mouse entered window
  virtual void mouseEnter(
    int x,
    int y);

  // mouse exited window
  virtual void mouseExit();

  // process key press
  virtual void keyDown(
    int keyCode, 
    int flags);

  // process key release
  virtual void keyUp(
    int keyCode, 
    int flags);

  // process key char
  virtual void keyChar(
    int unicode, 
    int flags);

#ifdef WORKED
  // handle mouse wheel
  void mouseWheel(
    WPARAM  wParam,
    LPARAM  lParam);
#endif

  friend int main(int argc, char** argv);
  friend void mgShutdown();
  friend void mgIdle();
  friend void mgViewResized(int width, int height);
  friend void mgMouseMoveAbs(int x, int y, int flags);
  friend void mgMouseMoveRel(int dx, int dy, int flags);
  friend void mgMouseDown(int button, int flags);
  friend void mgMouseUp(int button, int flags);
  friend void mgMouseEnter(int x, int y);
  friend void mgMouseExit();
  friend void mgKeyDown(int keyCode, int flags);
  friend void mgKeyUp(int keyCode, int flags);
  friend void mgKeyChar(int unicode, int flags);
};

#endif
