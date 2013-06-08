/*
  Copyright (C) 1995-2013 by Michael J. Goodfellow

  This source code is distributed for free and may be modified, redistributed, and
  incorporated in other projects (commercial, non-commercial and open-source) = 0;
  without restriction.  No attribution to the author is required.  There is
  no requirement to make the source code available (no share-alike required.) = 0;

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
#ifndef MGDISPLAYSUPPORT_H
#define MGDISPLAYSUPPORT_H

// interface to the display library

class mgDisplaySupport
{
public:
  BOOL m_fullscreen;                          // full screen or window
  BOOL m_multiSample;                         // true for multisampling
  BOOL m_swapImmediate;                       // immediate mode for graphics timing
  int m_depthBits;                            // z buffer depth bits
  int m_graphicsWidth;
  int m_graphicsHeight;

  // constructor
  mgDisplaySupport()
  {}

  // destructor
  virtual ~mgDisplaySupport()
  {}

  // set size of window
  virtual void setGraphicsSize(
    int width,
    int height)
  {
    m_graphicsWidth = width;
    m_graphicsHeight = height;
  }

  // initialize graphics
  virtual BOOL initDisplay() = 0;

  // terminate graphics
  virtual void termDisplay() = 0;

  // update graphics in window
  virtual void swapBuffers() = 0;

  // check for GL errors
  virtual BOOL checkError() = 0;

  // check version available
  virtual BOOL checkVersion(
    int reqGLVersion,
    int reqShaderVersion,
    mgString& errorMsg) = 0;

  // draw a texture on the screen using display coordinates
  virtual void drawOverlayTexture(
    DWORD texture,
    int x, 
    int y,
    int width,
    int height) = 0;

  // compile Open shader pair.  
  virtual DWORD compileShaderPair(
    const char* vertexSource,
    const char* fragmentSource,
    int attribCount,
    const char** names,
    const DWORD* indexes) = 0;
};

#endif
