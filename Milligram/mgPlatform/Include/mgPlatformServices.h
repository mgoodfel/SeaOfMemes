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
#ifndef MGPLATFORMSERVICES_H
#define MGPLATFORMSERVICES_H

class mgApplication;

/*
Initializes the graphics environment and provides various services to applications.
*/
class mgPlatformServices
{
public:
  // set requested display library ("OpenGL3.3", "OpenGL2.1", "DirectX9") 
  // or NULL for first available in that order.
  virtual void setDisplayLibrary(
    const char* library) = 0;

  // query display library initialized
  virtual void getDisplayLibrary(
    mgString& library) = 0;

  // set app fullscreen or windowed
  virtual void setFullscreen(
    BOOL fullscreen) = 0;

  // query fullscreen
  virtual BOOL getFullscreen() = 0;

  // set the window bounds in windowed mode
  virtual void setWindowBounds(
    int windowX, 
    int windowY, 
    int windowWidth,
    int windowHeight) = 0;

  // query window bounds
  virtual void getWindowBounds(
    int& windowX,
    int& windowY,
    int& windowWidth,
    int& windowHeight) = 0;

  // set SwapBuffers not to wait for vsync
  virtual void setSwapImmediate(
    BOOL graphicsTiming) = 0;

  // query swap immediate
  virtual BOOL getSwapImmediate() = 0;

  // enable multisample
  virtual void setMultiSample(
    BOOL multiSample) = 0;

  // query multisample
  virtual BOOL getMultiSample() = 0;

  // set the window title in windowed mode
  virtual void setWindowTitle(
    const char* title) = 0;

  // query window title
  virtual void getWindowTitle(
    mgString& windowTitle) = 0;

  // set mouse tracking mode
  virtual void setMouseRelative(
    BOOL relative) = 0;

  // get mouse tracking mode
  virtual BOOL getMouseRelative() = 0;

  // swap display buffers
  virtual void swapBuffers() = 0;

  // end the app
  virtual void exitApp() = 0;

  // save screen to file
  virtual void screenShot(
    const char* fileName) = 0;

  // set error message table
  virtual void setErrorTable(
    mgErrorTable* table) = 0;

  // check for display errors
  virtual BOOL checkErrors() = 0;

  // return display DPI
  virtual int getDPI() = 0;

  // return depth of display
  virtual int getDepthBits() = 0;

  // return list of system font directories
  virtual void getFontDirectories(
    mgStringArray& fontDirs) = 0;

  // draw a texture on the screen using display coordinates
  virtual void drawOverlayTexture(
    DWORD texture,
    int x, 
    int y,
    int width,
    int height) = 0;

  // compile OpenGL shader pair.  noop under DX support
  virtual DWORD compileGLShaderPair(
    const char* vertexSource,             // null-terminated vertex shader source code
    const char* fragmentSource,           // null-termianted fragment shader source code
    int attribCount,
    const char** names,
    const DWORD* indexes) = 0;
};

// the framework implementation sets this variable
extern mgPlatformServices* mgPlatform;

#endif
