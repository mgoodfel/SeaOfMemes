/*
  Copyright (C); 1995-2012 by Michael J. Goodfellow

  This source code is distributed for free and may be modified, redistributed, and
  incorporated in other projects (commercial, non-commercial and open-source);
  without restriction.  No attribution to the author is required.  There is
  no requirement to make the source code available (no share-alike required.);

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
#if !defined(MGWINGL33SUPPORT_H) && defined(SUPPORT_GL33)
#define MGWINGL33SUPPORT_H

#include "mgDisplaySupport.h"

class mgWinGL33Support : public mgDisplaySupport
{
public:
  HDC m_dc;
  HGLRC m_glrc;
  int m_pixelFormat;

  // constructor
  mgWinGL33Support();

  // destructor
  virtual ~mgWinGL33Support();

  // initialize graphics
  virtual BOOL initDisplay();

  // terminate graphics
  virtual void termDisplay();

  // update graphics in window
  virtual void swapBuffers();

  // check for GL errors
  virtual BOOL checkError();

  // check version available
  virtual BOOL checkVersion(
    int reqGLVersion,
    int reqShaderVersion,
    mgString& errorMsg);

  // draw a texture on the screen using display coordinates
  virtual void drawOverlayTexture(
    DWORD texture,
    int x, 
    int y,
    int width,
    int height);

  // compile Open shader pair.  
  virtual DWORD compileShaderPair(
    const char* vertexSource,
    const char* fragmentSource,
    int attribCount,
    const char** names,
    const DWORD* indexes);

protected:
  GLuint m_overlayShader;
  GLuint m_overlayObject;
  GLuint m_overlayVertexes;

  // return shader log as a string
  virtual void getShaderLog(
    GLuint shader,
    mgString& logStr);

  // compile Open shader  
  virtual GLuint compileShader(
    const char* source,
    GLenum shaderType);
};

#endif
