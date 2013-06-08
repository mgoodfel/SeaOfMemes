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
#ifdef SUPPORT_GL21

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

// implement the initialization of OpenGL on Windows

#include "wglew.h"
#include "mgWinGLinit.h"
#include "mgWinGL21Support.h"

static int GL21_MULTISAMPLE_ATTRIBUTES[] = {
  WGL_SUPPORT_OPENGL_ARB, 1,    // support OpenGL rendering
  WGL_DRAW_TO_WINDOW_ARB, 1,    // can run in a window
  WGL_DOUBLE_BUFFER_ARB, 1,     // double buffered
  WGL_SAMPLE_BUFFERS_ARB, 1,    // multisampling
  WGL_SAMPLES_ARB, 8,           // sample count
  WGL_COLOR_BITS_ARB, 24,       // rgb colors
  WGL_DEPTH_BITS_ARB, 16,       // z-buffer bits
  WGL_STENCIL_BITS_ARB, 1,       // stencil buffer bits
  WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, 
  WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, 
  0 
};

static int GL21_DEFAULT_ATTRIBUTES[] = {
  WGL_SUPPORT_OPENGL_ARB, 1,    // support OpenGL rendering
  WGL_DRAW_TO_WINDOW_ARB, 1,    // can run in a window
  WGL_DOUBLE_BUFFER_ARB, 1,     // double buffered
  WGL_COLOR_BITS_ARB, 24,       // rgb colors
  WGL_DEPTH_BITS_ARB, 16,       // z-buffer bits
  WGL_STENCIL_BITS_ARB, 1,       // stencil buffer bits
  WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, 
  WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, 
  0 
};

static int GL21_CREATE_ATTRIBUTES[] = {
  WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
  WGL_CONTEXT_MINOR_VERSION_ARB, 1,
  0 };

extern "C" GLubyte* glExtensions;

//--------------------------------------------------------------
// constructor
mgWinGL21Support::mgWinGL21Support()
{
  m_glrc = NULL;
  m_overlayShader = 0;
  m_overlayVertexes = 0;
  m_pixelFormat = 0;
  m_depthBits = 0;
}

//--------------------------------------------------------------
// destructor
mgWinGL21Support::~mgWinGL21Support()
{
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
// initialize graphics
BOOL mgWinGL21Support::initDisplay()
{
  mgDebug("------ try to create OpenGL 2.1 context, fullscreen=%s, multiSample=%s", 
    m_fullscreen?"true":"false", 
    m_multiSample?"true":"false");

  mgGLInit();

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

  // SetPixelFormat can only be called once on the window.  The OpenGL 3.3
  // support might get as far as creating a pixel format, but then not find
  // the OpenGL version or shader version it requires.  If this happens,
  // don't choose a pixel format again.
  if (m_pixelFormat == 0)
  {
    // Did Windows Find A Matching Pixel Format?
    m_pixelFormat = ChoosePixelFormat(m_dc, &pfd);
    if (m_pixelFormat == 0)
    {
      mgDebug("OpenGL: No available pixel formats");
      termDisplay();
      return false;
    }
    mgDebug("ChoosePixelFormat returned %d", m_pixelFormat);

    // set the pixel format
    if (!SetPixelFormat(m_dc, m_pixelFormat, &pfd))       
    {
      mgDebug("OpenGL: Cannot SetPixelFormat, GetLastError = %d", GetLastError());
      termDisplay();
      return false;
    }

    m_depthBits = 16;  // assumed, since that's what we requested

    PIXELFORMATDESCRIPTOR found_pfd;
    memset(&found_pfd, 0, sizeof(found_pfd));
    if (DescribePixelFormat(m_dc, m_pixelFormat, sizeof(pfd), &found_pfd) != 0)
    {
      mgDebug("depth bits = %d", found_pfd.cDepthBits);
      if (found_pfd.cDepthBits != 0)
        m_depthBits = found_pfd.cDepthBits;
    }
  }
  else mgDebug("pixelFormat=%d and depthBits=%d inherited from earlier initialization.", m_pixelFormat, m_depthBits);

  // Are We Able To Get A Rendering Context?
  m_glrc = wglCreateContext(m_dc);         
  if (m_glrc == NULL)
  {
    mgDebug("OpenGL: Cannot wglCreateContext");
    termDisplay();
    return false;
  }

  if (!wglMakeCurrent(m_dc, m_glrc))
  {
    mgDebug("OpenGL: cannot make rc current.");
    termDisplay();
    return false;
  }

  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    mgDebug("Cannot initialize OpenGL - glewInit failed.");
    termDisplay();
    return false;
  }

  mgString errorMsg;
  if (!checkVersion(201, 102, errorMsg))
  {
    mgDebug("%s", (const char*) errorMsg);
    termDisplay();
    return false;
  }

  if (m_multiSample)
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
void mgWinGL21Support::termDisplay()
{
  mgDebug("------ destroy OpenGL context");

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
void mgWinGL21Support::swapBuffers()
{
  SwapBuffers(m_dc);   
}

//--------------------------------------------------------------
// check for GL errors
BOOL mgWinGL21Support::checkError()
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
BOOL mgWinGL21Support::checkVersion(
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
BOOL mgWinGL21Support::builtinShaders()
{
  mgString vs;
  vs += "#version 120\n";
  vs += "uniform vec2 mgSize;\n";
  vs += "uniform vec2 mgOrigin;\n";
  vs += "attribute vec2 vertPoint;\n";
  vs += "attribute vec2 vertTexCoord0;\n";
  vs += "varying vec2 vTex;\n";
  vs += "void main(void) \n";
  vs += "{ \n";
  vs += "  vTex = vertTexCoord0;\n";
  vs += "  gl_Position.x = (2.0 * (vertPoint.x+mgOrigin.x)) / mgSize.x - 1.0;\n";
  vs += "  gl_Position.y = (-2.0 * (vertPoint.y+mgOrigin.y)) / mgSize.y + 1.0;\n";
  vs += "  gl_Position.z = 0.5;\n";
  vs += "  gl_Position.w = 1.0;\n";
  vs += "}\n";

  mgString fs;
  fs += "#version 120\n";
  fs += "uniform sampler2D mgTextureUnit0;\n";
  fs += "varying vec2 vTex;\n";
  fs += "void main(void) \n";
  fs += "{\n"; 
  fs += "  gl_FragColor = texture2D(mgTextureUnit0, vTex);\n";
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
void mgWinGL21Support::getShaderLog(
  GLuint shader,
  mgString& log)
{
  GLint logLen, retLen;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
  char* logText = new char[logLen];
  glGetShaderInfoLog(shader, logLen, &retLen, logText);
  log.empty();
  log.write((const char*) logText, retLen);
  delete logText;
}

//--------------------------------------------------------------
// compile OpenGL21 shader  
GLuint mgWinGL21Support::compileShader(
  const char* source,
  GLenum shaderType)
{
  // Create shader objects
  GLuint shader = glCreateShader(shaderType);
  const GLchar* strings[1];
  strings[0] = (GLchar *) source;
  glShaderSource(shader, 1, strings, NULL);

  const char* typeStr = (shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment";
  glCompileShader(shader);
  mgString log;
  getShaderLog(shader, log);
  log.trim();
  if (log.isEmpty())
    mgDebug("%s shader compiled.", typeStr);
  else mgDebug("%s shader log:\n%s", typeStr, (const char*) log);

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
// compile OpenGL21 shader pair.  
DWORD mgWinGL21Support::compileShaderPair(
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
void mgWinGL21Support::drawOverlayTexture(
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

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
