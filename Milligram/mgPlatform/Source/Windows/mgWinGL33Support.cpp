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
#ifdef SUPPORT_GL33

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

// implement the initialization of OpenGL on Windows

#include "wglew.h"
#include "mgWinGLinit.h"
#include "mgWinGL33Support.h"

static int GL33_MULTISAMPLE_ATTRIBUTES[] = {
  WGL_SUPPORT_OPENGL_ARB, 1,    // support OpenGL rendering
  WGL_DRAW_TO_WINDOW_ARB, 1,    // can run in a window
  WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, 
  WGL_DOUBLE_BUFFER_ARB, 1,     // double buffered
  WGL_SAMPLE_BUFFERS_ARB, 1,    // multisampling
  WGL_SAMPLES_ARB, 8,           // sample count
  WGL_COLOR_BITS_ARB, 24,       // rgb colors
  WGL_DEPTH_BITS_ARB, 16,       // z-buffer bits
  WGL_STENCIL_BITS_ARB, 1,       // stencil buffer bits
  WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, 
  0 
};

static int GL33_DEFAULT_ATTRIBUTES[] = {
  WGL_SUPPORT_OPENGL_ARB, 1,    // support OpenGL rendering
  WGL_DRAW_TO_WINDOW_ARB, 1,    // can run in a window
  WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, 
  WGL_DOUBLE_BUFFER_ARB, 1,     // double buffered
  WGL_COLOR_BITS_ARB, 24,       // rgb colors
  WGL_DEPTH_BITS_ARB, 16,       // z-buffer bits
  WGL_STENCIL_BITS_ARB, 1,       // stencil buffer bits
  WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, 
  0 
};

static int GL33_CREATE_ATTRIBUTES[] = {
  WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
  WGL_CONTEXT_MINOR_VERSION_ARB, 3,
  WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
  0 };

static int GL32_CREATE_ATTRIBUTES[] = {
  WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
  WGL_CONTEXT_MINOR_VERSION_ARB, 2,
  WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
  0 };


extern "C" GLubyte* glExtensions;

//--------------------------------------------------------------
// constructor
mgWinGL33Support::mgWinGL33Support()
{
  m_glrc = NULL;
  m_overlayShader = 0;
  m_overlayObject = 0;
  m_overlayVertexes = 0;
  m_pixelFormat = 0;
  m_depthBits = 0;
}

//--------------------------------------------------------------
// destructor
mgWinGL33Support::~mgWinGL33Support()
{
  if (m_overlayObject != 0)
  {
    glDeleteVertexArrays(1, &m_overlayObject);
    m_overlayObject = 0;
  }

  if (m_overlayVertexes != 0)
  {
    glDeleteBuffers(1, &m_overlayVertexes);
    m_overlayVertexes = 0;
  }

  if (m_overlayShader != 0)
  {
    glDeleteProgram(m_overlayShader);
    m_overlayShader = 0;
  }
}

//--------------------------------------------------------------
// initialize OpenGL 3.3 graphics
BOOL mgWinGL33Support::initDisplay()
{
  mgDebug("------ try to create OpenGL 3.3 context, fullscreen=%s, multiSample=%s", 
    m_fullscreen?"true":"false", 
    m_multiSample?"true":"false");

  mgGLInit();

  // save the GL_EXTENSIONS from the base driver as backup
  const GLubyte* defaultExtensions = glGetString(GL_EXTENSIONS);

  PFNWGLCHOOSEPIXELFORMATARBPROC fnChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
  PFNWGLCREATECONTEXTATTRIBSARBPROC fnCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
  PFNGLGETSTRINGIPROC fnGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");
  PFNWGLGETPIXELFORMATATTRIBIVARBPROC fnGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC) wglGetProcAddress("wglGetPixelFormatAttribivARB");

  if (fnChoosePixelFormatARB == NULL || 
      fnCreateContextAttribsARB == NULL || 
      fnGetStringi == NULL ||
      fnGetPixelFormatAttribivARB == NULL)
  {
    mgDebug("GLCreate unable to locate OpenGL methods");
    termDisplay();
    return false;
  }

  int* formatAttribs;
  if (m_multiSample)
    formatAttribs = GL33_MULTISAMPLE_ATTRIBUTES;
  else formatAttribs = GL33_DEFAULT_ATTRIBUTES;
  
  float floatFormatAttribs[] = {
    0.0f
  };

  int formats[1];
  UINT formatCount;
  (*fnChoosePixelFormatARB) (m_dc, formatAttribs, floatFormatAttribs, 1, formats, &formatCount);
  if (formatCount < 1)
  {
    // fall back to default, not multisample
    (*fnChoosePixelFormatARB) (m_dc, GL33_DEFAULT_ATTRIBUTES, floatFormatAttribs, 1, formats, &formatCount);
    if (formatCount < 1)
    {
      mgDebug("OpenGL: No available pixel formats");
      termDisplay();
      return false;
    }
  }

  if (fnGetPixelFormatAttribivARB != NULL)
  {
    // HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int* piAttributes, int *piValues
    int attribs[] = {WGL_RED_BITS_ARB, WGL_GREEN_BITS_ARB, WGL_BLUE_BITS_ARB, WGL_ALPHA_BITS_ARB, WGL_DEPTH_BITS_ARB, WGL_SAMPLES_ARB};
    int values[6];
    if ((*fnGetPixelFormatAttribivARB) (m_dc, formats[0], 0, 6, attribs, values))
      mgDebug("wglGetPixelFormatARB = R%dG%dB%dA%d, %d depth, %d multisample", values[0], values[1], values[2], values[3], values[4], values[5]);
    else mgDebug("wglGetPixelFormatARB returns false");
  }

  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof(pfd));
  m_pixelFormat = formats[0];
  SetPixelFormat(m_dc, m_pixelFormat, &pfd);
  DescribePixelFormat(m_dc, m_pixelFormat, sizeof(pfd), &pfd);

  m_depthBits = 16;  // assumed, since that's what we requested
  if (pfd.cDepthBits != 0)
    m_depthBits = pfd.cDepthBits;
  mgDebug("stencil bits = %d", pfd.cStencilBits);

  m_glrc = (*fnCreateContextAttribsARB)(m_dc, NULL, GL33_CREATE_ATTRIBUTES);
  if (m_glrc == NULL)
  {
    mgDebug("OpenGL wglCreateContextAttribsARB 3.3 returns NULL.  Trying 3.2");
    m_glrc = (*fnCreateContextAttribsARB)(m_dc, NULL, GL32_CREATE_ATTRIBUTES);
    if (m_glrc == NULL)
    {
      mgDebug("OpenGL wglCreateContextAttribsARB 3.2 returns NULL.  I give up.");
      termDisplay();
      return false;
    }
  }

  if (!wglMakeCurrent(m_dc, m_glrc))
  {
    mgDebug("OpenGL: cannot make rc current.");
    termDisplay();
    return false;
  }

  // build an extension list for glew with glGetStringi
  mgString extList;
  int i = 0;
  for (; ; i++)
  {
    const GLubyte* ext = (*fnGetStringi)(GL_EXTENSIONS, i);
    if (ext == NULL)
      break;
    extList += (const char*) ext;
    extList += " ";
  }
  mgDebug("glGetStringi returns %d extensions", i);
  checkError();  // clear error from requesting last extension

  // if there were no individual extensions, use default list
  if (i != 0)
  {
    glExtensions = (GLubyte*) new char[1+extList.length()];
    strcpy((char*) glExtensions, (const char*) extList);
  }
  else 
  {
    // copy default extensions so we can delete this string in termDisplay.
    glExtensions = (GLubyte*) new char[1+strlen((const char*) defaultExtensions)];
    strcpy((char*) glExtensions, (const char*) defaultExtensions);
  }

  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    mgDebug("glewInit failed.");
    termDisplay();
    return false;
  }

  // check version.  accept OpenGL 3.2, but need 330 shader.
  mgString errorMsg;
  if (!checkVersion(302, 303, errorMsg))
  {
    mgDebug("%s", (const char*) errorMsg);
    termDisplay();
    return false;
  }

  if (mgPlatform->getMultiSample())
    glEnable(GL_MULTISAMPLE);

  if (m_swapImmediate)
    wglSwapIntervalEXT(0);

  if (!builtinShaders())
  {
    termDisplay();
    return false;
  }

  const char* vendor = (const char*) glGetString(GL_VENDOR);
  if (vendor != NULL)
    mgDebug(":OpenGL device vendor: %s", (const char*) vendor);
  const char* renderer = (const char*) glGetString(GL_RENDERER);
  if (renderer != NULL)
    mgDebug(":OpenGL device renderer: %s", (const char*) renderer);
  const char* versionStr = (const char*) glGetString(GL_VERSION);
  if (versionStr != NULL)
    mgDebug(":OpenGL version: %s", (const char*) versionStr);
  const char* shaderVersionStr = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (shaderVersionStr != NULL)
    mgDebug(":OpenGL shader version: %s", (const char*) shaderVersionStr);

// report all the GL limits
  GLint value;
#define GLREPORT(sym) value = INT_MAX; glGetIntegerv(sym, &value); if (value != INT_MAX) mgDebug(":%s: %d", #sym, value);
  GLREPORT(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
  GLREPORT(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
  GLREPORT(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
  GLREPORT(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS);
  GLREPORT(GL_MAX_VARYING_COMPONENTS);
  GLREPORT(GL_MAX_COMBINED_UNIFORM_BLOCKS);
  GLREPORT(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
  GLREPORT(GL_MAX_DRAW_BUFFERS);
  GLREPORT(GL_MAX_ELEMENTS_INDICES);
  GLREPORT(GL_MAX_ELEMENTS_VERTICES);
  GLREPORT(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
  GLREPORT(GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
  GLREPORT(GL_MAX_FRAGMENT_INPUT_COMPONENTS);
  GLREPORT(GL_MIN_PROGRAM_TEXEL_OFFSET);
  GLREPORT(GL_MAX_PROGRAM_TEXEL_OFFSET);
  GLREPORT(GL_MAX_RECTANGLE_TEXTURE_SIZE);
  GLREPORT(GL_MAX_TEXTURE_IMAGE_UNITS);
  GLREPORT(GL_MAX_TEXTURE_LOD_BIAS);
  GLREPORT(GL_MAX_TEXTURE_SIZE);
  GLREPORT(GL_MAX_RENDERBUFFER_SIZE);
  GLREPORT(GL_MAX_ARRAY_TEXTURE_LAYERS);
  GLREPORT(GL_MAX_TEXTURE_BUFFER_SIZE);
  GLREPORT(GL_MAX_UNIFORM_BLOCK_SIZE);
  GLREPORT(GL_MAX_VARYING_FLOATS);
  GLREPORT(GL_MAX_VERTEX_ATTRIBS);
  GLREPORT(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
  GLREPORT(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
  GLREPORT(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
  GLREPORT(GL_MAX_VERTEX_OUTPUT_COMPONENTS);
  GLREPORT(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS);
  GLREPORT(GL_MAX_SAMPLE_MASK_WORDS);
  GLREPORT(GL_MAX_COLOR_TEXTURE_SAMPLES);
  GLREPORT(GL_MAX_DEPTH_TEXTURE_SAMPLES);
  GLREPORT(GL_MAX_DEPTH_TEXTURE_SAMPLES);
  GLREPORT(GL_MAX_INTEGER_SAMPLES);
  GLREPORT(GL_MAX_UNIFORM_BUFFER_BINDINGS);
  GLREPORT(GL_MAX_UNIFORM_BLOCK_SIZE);
  GLREPORT(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT);
  GLREPORT(GL_MAX_VERTEX_UNIFORM_BLOCKS);
  GLREPORT(GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
  GLREPORT(GL_MAX_GEOMETRY_INPUT_COMPONENTS);
  GLREPORT(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS);

#undef GLREPORT
  return true;
}

//--------------------------------------------------------------
// terminate graphics
void mgWinGL33Support::termDisplay()
{
  mgDebug("------ destroy OpenGL context");

  if (m_dc != NULL)
    wglMakeCurrent(m_dc, NULL);

  if (m_glrc != NULL)
    wglDeleteContext(m_glrc);
  m_glrc = NULL;

  delete glExtensions;
  glExtensions = NULL;

  mgGLReset();

  mgGLTerm();
}

//-----------------------------------------------------------------------------
// update graphics in window
void mgWinGL33Support::swapBuffers()
{
  SwapBuffers(m_dc);
}

//--------------------------------------------------------------
// check for GL errors
BOOL mgWinGL33Support::checkError()
{
  BOOL hasErrors = false;
  BOOL done = false;
  mgString msg;
  for (int i = 0; i < 10 && !done; i++)
  {
    GLenum err = glGetError();
    switch (err)
    {
      case GL_NO_ERROR:
        if (msg.isEmpty())
          msg += "GL no error.  ";
        done = true;
        break;

      case GL_INVALID_ENUM:
        msg += "GL invalid enum.  ";
        hasErrors = true;
        break;

      case GL_INVALID_VALUE:
        msg += "GL invalid value.  ";
        hasErrors = true;
        break;

      case GL_INVALID_OPERATION:
        msg += "GL invalid operation.  ";
        hasErrors = true;
        break;

#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        msg += "GL invalid framebuffer operation.  ";
        hasErrors = true;
        break;
#endif

      case GL_OUT_OF_MEMORY:
        msg += "GL out of memory.  ";
        hasErrors = true;
        break;
    }
  }
  mgDebug("%s", (const char*) msg);

  return hasErrors;
}

//--------------------------------------------------------------
// check version of OpenGL available
BOOL mgWinGL33Support::checkVersion(
  int reqGLVersion,
  int reqShaderVersion,
  mgString& errorMsg)
{
  const char* vendor = (const char*) glGetString(GL_VENDOR);
  const char* renderer = (const char*) glGetString(GL_RENDERER);
  const char* glVersionStr = (const char*) glGetString(GL_VERSION);
  if (glVersionStr == NULL)
  {
    errorMsg = "cannot read OpenGL version";
    return false;
  }

  int major, minor, glVersion;
  if (2 != sscanf(glVersionStr, "%d.%d", &major, &minor))
  {
    errorMsg.format("cannot parse OpenGL version %s", (const char*) glVersionStr);
    return false;
  }
  // trim trailing zeros.  3.3 and 3.30 are the same
  while (minor%10 == 0)
    minor /= 10;

  glVersion = major*100 + minor;

  const char* shaderVersionStr = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (shaderVersionStr == NULL)
  {
    errorMsg = "cannot read OpenGL Shader Language version";
    return false;
  }

  int shaderVersion;
  if (2 != sscanf(shaderVersionStr, "%d.%d", &major, &minor))
  {
    errorMsg.format("cannot parse OpenGL version %s", (const char*) shaderVersionStr);
    return false;
  }
  // trim trailing zeros.  3.3 and 3.30 are the same
  while (minor%10 == 0)
    minor /= 10;

  shaderVersion = major*100 + minor;

  if (glVersion < reqGLVersion || shaderVersion < reqShaderVersion)
  {
    errorMsg.format("App requested OpenGl %d.%d, shader %d.%d\nSystem has OpenGL %d.%d, shader %d.%d", 
      reqGLVersion/100, reqGLVersion%100, reqShaderVersion/100, reqShaderVersion%100,
      glVersion/100, glVersion%100, shaderVersion/100, shaderVersion%100);
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
// compile the builtin shaders
BOOL mgWinGL33Support::builtinShaders()
{
  mgString vs;
  vs += "#version 330\n";
  vs += "uniform vec2 mgSize;\n";
  vs += "uniform vec2 mgOrigin;\n";
  vs += "in vec2 vertPoint;\n";
  vs += "in vec2 vertTexCoord0;\n";
  vs += "smooth out vec2 vTex;\n";
  vs += "void main(void) \n";
  vs += "{ \n";
  vs += "  vTex = vertTexCoord0;\n";
  vs += "  gl_Position.x = (2.0 * (vertPoint.x+mgOrigin.x)) / mgSize.x - 1;\n";
  vs += "  gl_Position.y = (-2.0 * (vertPoint.y+mgOrigin.y)) / mgSize.y + 1;\n";
  vs += "  gl_Position.z = 0.5;\n";
  vs += "  gl_Position.w = 1.0;\n";
  vs += "}\n";

  mgString fs;
  fs += "#version 330\n";
  fs += "uniform sampler2D mgTextureUnit0;\n";
  fs += "smooth in vec2 vTex;\n";
  fs += "out vec4 outFragColor;\n";
  fs += "void main(void) \n";
  fs += "{\n"; 
  fs += "  outFragColor = texture(mgTextureUnit0, vTex);\n";
  fs += "}\n";

  // compile the overlay shader
  mgDebug("compile overlay shader:");
  const char* attrNames[] = {"vertPoint", "vertTexCoord0"};
  const DWORD attrIndexes[] = {0, 1};
  m_overlayShader = compileShaderPair(vs, fs, 2, attrNames, attrIndexes);

  if (m_overlayShader == 0)
  {
    mgDebug("compile overlay shader failed.");
    return false;
  }

  m_overlayTextureUnitIndex = glGetUniformLocation(m_overlayShader, "mgTextureUnit0");
  m_overlaySizeIndex = glGetUniformLocation(m_overlayShader, "mgSize");
  m_overlayOriginIndex = glGetUniformLocation(m_overlayShader, "mgOrigin");

  return true;
}

//--------------------------------------------------------------------------
// return shader log error messages as a string
void mgWinGL33Support::getShaderLog(
  GLuint shader,
  mgString& logStr)
{
  GLint logLen, retLen;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
  char* logText = new char[logLen];
  glGetShaderInfoLog(shader, logLen, &retLen, logText);
  logStr.empty();
  logStr.write((const char*) logText, retLen);
  delete logText;
}

//--------------------------------------------------------------
// compile OpenGL33 shader  
GLuint mgWinGL33Support::compileShader(
  const char* source,
  GLenum shaderType)
{
  // Create shader objects
  GLuint shader = glCreateShader(shaderType);
  const GLchar* strings[1];
  strings[0] = (GLchar *) source;
  glShaderSource(shader, 1, strings, NULL);

  const char* typeStr = (shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment";
  mgString log;
  glCompileShader(shader);
  getShaderLog(shader, log);
  log.trim();
  if (log.isEmpty())
    mgDebug("%s shader compiled.", typeStr);
  else mgDebug("%s shader log:\n%s\n", typeStr, (const char*) log);

  // Check for errors
  GLint testVal;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &testVal);
  if (testVal == GL_FALSE)
  {
    mgDebug("%s shader compilation failed.", typeStr);
    return 0;
  }

  return shader;
}

//--------------------------------------------------------------
// compile OpenGL33 shader pair.  
DWORD mgWinGL33Support::compileShaderPair(
  const char* vertexSource,
  const char* fragmentSource,
  int attribCount,
  const char** names,
  const DWORD* indexes)
{
  GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
  GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
  if (vertexShader == 0 || fragmentShader == 0)
    return 0;
  
  // create the shader program
  GLuint shader = glCreateProgram();
  glAttachShader(shader, vertexShader);
  glAttachShader(shader, fragmentShader);

  // bind attributes
  for (int i = 0; i < attribCount; i++)
  {
    glBindAttribLocation(shader, indexes[i], names[i]);
  }

  glLinkProgram(shader);
  
  // These are no longer needed
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);  
    
  // Make sure link worked too
  GLint testVal;
  glGetProgramiv(shader, GL_LINK_STATUS, &testVal);
  if (testVal == GL_FALSE)
  {
    // get the linker log
    GLsizei msgSize, msgLen;
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &msgSize);
    GLchar* message = new char[msgSize];
    glGetProgramInfoLog(shader, msgSize, &msgLen, message);
    mgDebug("%s", (const char*) message);
    delete message;

    glDeleteProgram(shader);
    shader = 0;
    mgDebug("shader link failed.");
  }

  return shader;
}

//--------------------------------------------------------------
// draw a texture on the screen using display coordinates
void mgWinGL33Support::drawOverlayTexture(
  DWORD texture,
  int x, 
  int y,
  int width,
  int height)
{
  // if shader did not compile, nothing we can do
  if (m_overlayShader == 0)
    return;

  // vertex format for overlay graphics
  typedef struct _OverlayVertex
  {
    GLfloat m_px;
    GLfloat m_py;
    GLfloat m_tx;
    GLfloat m_ty;
  } OverlayVertex;

  const size_t OVERLAY_POINT_OFFSET = offsetof(OverlayVertex, m_px);
  const size_t OVERLAY_TXCOORDS_OFFSET = offsetof(OverlayVertex, m_tx);

  glUseProgram(m_overlayShader);

  glUniform1i(m_overlayTextureUnitIndex, 0);
  glUniform2f(m_overlaySizeIndex, (GLfloat) m_graphicsWidth, (GLfloat) m_graphicsHeight);
  glUniform2f(m_overlayOriginIndex, (GLfloat) x, (GLfloat) y);

  // create the vertex array and buffer for the overlay if necessary
  if (m_overlayObject == 0)
    glGenVertexArrays(1, &m_overlayObject);
  glBindVertexArray(m_overlayObject);

  if (m_overlayVertexes == 0)
    glGenBuffers(1, &m_overlayVertexes);
  glBindBuffer(GL_ARRAY_BUFFER, m_overlayVertexes);

  // create the vertex list for the overlay rectangle
  OverlayVertex vertexes[6];
  GLfloat w = (GLfloat) width;
  GLfloat h = (GLfloat) height;

  // tl
  vertexes[0].m_px = 0.0f; vertexes[0].m_py = 0.0f;
  vertexes[0].m_tx = 0.0f; vertexes[0].m_ty = 0.0f; 

  // tr
  vertexes[1].m_px = w;    vertexes[1].m_py = 0.0f;
  vertexes[1].m_tx = 1.0f; vertexes[1].m_ty = 0.0f; 

  // bl
  vertexes[2].m_px = 0.0f; vertexes[2].m_py = h;   
  vertexes[2].m_tx = 0.0f; vertexes[2].m_ty = 1.0f; 

  // bl
  vertexes[3].m_px = 0.0f; vertexes[3].m_py = h;   
  vertexes[3].m_tx = 0.0f; vertexes[3].m_ty = 1.0f; 

  // tr
  vertexes[4].m_px = w;    vertexes[4].m_py = 0.0f;
  vertexes[4].m_tx = 1.0f; vertexes[4].m_ty = 0.0f; 

  // br
  vertexes[5].m_px = w;    vertexes[5].m_py = h;   
  vertexes[5].m_tx = 1.0f; vertexes[5].m_ty = 1.0f; 

  glBufferData(GL_ARRAY_BUFFER, sizeof(OverlayVertex) * 6, vertexes, GL_DYNAMIC_DRAW);

  // bind arrays to attribute slots
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(OverlayVertex), (const GLvoid*) OVERLAY_POINT_OFFSET);

  glEnableVertexAttribArray(1),
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OverlayVertex), (const GLvoid*) OVERLAY_TXCOORDS_OFFSET);

  // draw the texture
  glBindTexture(GL_TEXTURE_2D, (GLuint) texture);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
