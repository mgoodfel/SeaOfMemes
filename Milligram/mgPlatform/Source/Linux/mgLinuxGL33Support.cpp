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

#if defined(SUPPORT_GL33) && defined(__unix__)

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgLinuxServices.h"
#include "mgLinuxGL33Support.h"

static int GL33_CREATE_ATTRIBUTES[] = {
  GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
  GLX_CONTEXT_MINOR_VERSION_ARB, 3,
  GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
  0 };

static int GL32_CREATE_ATTRIBUTES[] = {
  GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
  GLX_CONTEXT_MINOR_VERSION_ARB, 2,
  GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
  0 };

const char* GL33_OVERLAY_VERTEX_SHADER = "        \n\
#version 330                                      \n\
uniform mat4 mgProjMatrix;                        \n\
uniform vec2 mgOrigin;                            \n\
in vec2 vertPoint;                                \n\
in vec2 vertTexCoord0;                            \n\
smooth out vec2 vTex;                             \n\
void main(void)                                   \n\
{                                                 \n\
  vTex = vertTexCoord0;                           \n\
  vec4 position;                                  \n\
  position.xy = vertPoint + mgOrigin;             \n\
  position.z = 0.0;                               \n\
  position.w = 1.0;                               \n\
  gl_Position = mgProjMatrix * position;          \n\
}                                                 \n\
";

const char* GL33_OVERLAY_FRAGMENT_SHADER = "      \n\
#version 330                                      \n\
uniform sampler2D mgTextureUnit0;                 \n\
smooth in vec2 vTex;                              \n\
out vec4 outFragColor;                            \n\
void main(void)                                   \n\
{                                                 \n\
  outFragColor = texture(mgTextureUnit0, vTex);   \n\
}                                                 \n\
";

extern "C" GLubyte* glExtensions;

//--------------------------------------------------------------
// constructor
mgLinuxGL33Support::mgLinuxGL33Support()
{
  m_overlayShader = 0;
  m_overlayObject = 0;
  m_overlayVertexes = 0;
}

//--------------------------------------------------------------
// destructor
mgLinuxGL33Support::~mgLinuxGL33Support()
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
// initialize graphics
BOOL mgLinuxGL33Support::initDisplay()
{
  mgDebug("------ try to create OpenGL 3.3 context, fullscreen=%s, multiSample=%s", 
    m_fullscreen?"true":"false", 
    m_multiSample?"true":"false");

  mgLinuxServices* platform = (mgLinuxServices*) mgPlatform;

  // =-= some drivers are failing this test.  not clear we need it
#ifdef WORKED
  int glxMajor, glxMinor;
  glXQueryVersion(platform->m_display, &glxMajor, &glxMinor);
  mgDebug("GLX version = %d.%d", glxMajor, glxMinor);

  int glxVersion = glxMajor * 100 + glxMinor;
  if (glxVersion < 103)
  {
    mgDebug("GLX version < 1.3");
    termDisplay();
    return false;
  }
#endif

  PFNGLXCREATECONTEXTATTRIBSARBPROC fnCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte*) "glXCreateContextAttribsARB");
  PFNGLGETSTRINGIPROC fnGetStringi = (PFNGLGETSTRINGIPROC)glXGetProcAddress((const GLubyte*) "glGetStringi");
  if (fnCreateContextAttribsARB == NULL || fnGetStringi == NULL)
  {
    mgDebug("Could not locate OpenGL functions");
    termDisplay();
    return false;
  }

  platform->m_glrc = (*fnCreateContextAttribsARB) (platform->m_display, platform->m_bestFbc, NULL, true, GL33_CREATE_ATTRIBUTES);
  if (platform->m_glrc == NULL)
  {
    mgDebug("glXCreateContextAttribsARB 3.3 returns NULL.  Trying 3.2");
    platform->m_glrc = (*fnCreateContextAttribsARB) (platform->m_display, platform->m_bestFbc, NULL, true, GL32_CREATE_ATTRIBUTES);
    if (platform->m_glrc == NULL)
    {
      mgDebug("glXCreateContextAttribsARB 3.2 returns NULL.  I give up.");
      termDisplay();
      return false;
    }
  }

  glXMakeCurrent(platform->m_display, platform->m_window, platform->m_glrc);

  m_depthBits = 16; // default
  glXGetConfig(platform->m_display, platform->m_vi, GLX_DEPTH_SIZE, &m_depthBits);
  mgDebug("%d depth bits", m_depthBits);

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

  // save extensions list for glew
  glExtensions = (GLubyte*) new char[1+extList.length()];
  strcpy((char*) glExtensions, (const char*) extList);

	GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    mgDebug("Cannot initialize OpenGL - glewInit failed.");
    termDisplay();
    return false;
  }

  // accept OpenGL 3.2, but need #version 330 shader
  mgString errorMsg;
  if (!checkVersion(302, 303, errorMsg))
  {
    mgDebug("%s", (const char*) errorMsg);
    termDisplay();
    return false;
  }

  if (m_swapImmediate)
  {
    PFNGLXSWAPINTERVALMESAPROC fnSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC)glXGetProcAddress((const GLubyte*) "glXSwapIntervalMESA");
    if (fnSwapIntervalMESA != NULL)
      (*fnSwapIntervalMESA) (0);
    else 
    {
      PFNGLXSWAPINTERVALSGIPROC fnSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress((const GLubyte*) "glXSwapIntervalSGI");
      if (fnSwapIntervalSGI != NULL)
        (*fnSwapIntervalSGI) (0);
      else mgDebug("Cannot find glXSwapInterval");
    }
  }

  if (m_multiSample)
    glEnable(GL_MULTISAMPLE);

  // compile the overlay shader
  mgDebug("compile overlay shader:");
  const char* attrNames[] = {"vertPoint", "vertTexCoord0"};
  const DWORD attrIndexes[] = {0, 1};
  m_overlayShader = compileShaderPair(GL33_OVERLAY_VERTEX_SHADER, GL33_OVERLAY_FRAGMENT_SHADER,
    2, attrNames, attrIndexes);

  if (m_overlayShader == 0)
  {
    mgDebug("Cannot compile overlay shader.");
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
void mgLinuxGL33Support::termDisplay()
{
  mgLinuxServices* platform = (mgLinuxServices*) mgPlatform;

  glXMakeCurrent(platform->m_display, 0, 0);
  if (platform->m_glrc != NULL)
    glXDestroyContext(platform->m_display, platform->m_glrc);
  platform->m_glrc = NULL;

  delete glExtensions;
  glExtensions = NULL;
}

//-----------------------------------------------------------------------------
// update graphics in window
void mgLinuxGL33Support::swapBuffers()
{
  mgLinuxServices* platform = (mgLinuxServices*) mgPlatform;

  glXSwapBuffers(platform->m_display, platform->m_window);
}

//--------------------------------------------------------------
// check for GL errors
BOOL mgLinuxGL33Support::checkError()
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
BOOL mgLinuxGL33Support::checkVersion(
  int reqGLVersion,
  int reqShaderVersion,
  mgString& errorMsg)
{
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
// return shader log error messages as a string
void mgLinuxGL33Support::getShaderLog(
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
// compile OpenGL33 shader  
GLuint mgLinuxGL33Support::compileShader(
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
// compile OpenGL33 shader pair.  
DWORD mgLinuxGL33Support::compileShaderPair(
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
void mgLinuxGL33Support::drawOverlayTexture(
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

  // create overlay LH orthographic projection with top left at 0,0
  const GLfloat zNear = -1.0;
  const GLfloat zFar = 1.0;
  GLfloat projection[16];
  memset(projection, 0, sizeof(projection));
  projection[0*4+0] = 2.0f/m_graphicsWidth;
  projection[1*4+1] = -2.0f/m_graphicsHeight;
  projection[2*4+2] = 1.0f/(zFar - zNear);
  projection[3*4+0] = -1.0f;   // (left+right)/(left-right)
  projection[3*4+1] = 1.0f;   // (bot+top)/(bot-top)
  projection[3*4+2] = -zNear/(zFar - zNear);
  projection[3*4+3] = 1.0f;

  glUseProgram(m_overlayShader);

  GLint iProjMatrix = glGetUniformLocation(m_overlayShader, "mgProjMatrix");
  glUniformMatrix4fv(iProjMatrix, 1, GL_FALSE, projection);

  GLint iTextureUnit = glGetUniformLocation(m_overlayShader, "mgTextureUnit0");
  glUniform1i(iTextureUnit, 0);
  
  GLint iOrigin = glGetUniformLocation(m_overlayShader, "mgOrigin");
  glUniform2f(iOrigin, (GLfloat) x, (GLfloat) y);

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OverlayVertex), (const GLvoid*) OVERLAY_POINT_OFFSET);

  glEnableVertexAttribArray(1),
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OverlayVertex), (const GLvoid*) OVERLAY_TXCOORDS_OFFSET);

  // draw the texture
  glBindTexture(GL_TEXTURE_2D, (GLuint) texture);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
#endif
