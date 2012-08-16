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
#ifdef SUPPORT_GL33

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgUtil/Include/ImageUtil/mgImageUtil.h"

#include "mgGL33Types.h"

#include "mgGL33Misc.h"
#include "mgGL33Services.h"
#include "mgGL33VertexBuffer.h"
#include "mgGL33IndexBuffer.h"

#include "mgPlatform/Include/mgPlatformServices.h"
#include "mgPlatform/Include/mgApplication.h"

#if defined(_DEBUG) && defined(WIN32)
#define CHECK_THREAD() mgServicesCheckThread()

void mgServicesCheckThread()
{
  if (GetCurrentThreadId() != ((mgGL33Services*) mgDisplay)->m_createThreadId)
    throw new mgErrorMsg("glWrongThread", "", ""); 
}
#endif

//--------------------------------------------------------------
// constructor
mgGL33TextureImage::mgGL33TextureImage()
{
  m_handle = mgNullHandle;
  m_format = 0;
  m_mipmap = TRUE;
}

//--------------------------------------------------------------
// destructor
mgGL33TextureImage::~mgGL33TextureImage()
{
  if (m_handle != mgNullHandle)
  {
    glDeleteTextures(1, &m_handle);
    m_handle = mgNullHandle;
  }
}

//--------------------------------------------------------------
// convert framework wrap constants to GL
inline GLuint mgGL33TextureWrap(
  int mode)
{
  switch (mode)
  {
    case MG_TEXTURE_REPEAT: return GL_REPEAT;
    case MG_TEXTURE_CLAMP: return GL_CLAMP_TO_EDGE; 
    default: return GL_REPEAT;// throw exception?
  }
}

//--------------------------------------------------------------
// set texture wrap mode in x and y
void mgGL33TextureImage::setWrap(
  int xWrap,
  int yWrap)
{
  CHECK_THREAD();

  m_xWrap = xWrap;
  m_yWrap = yWrap;

  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mgGL33TextureWrap(m_xWrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mgGL33TextureWrap(m_yWrap));
}

//--------------------------------------------------------------
// update memory texture
void mgGL33TextureImage::updateMemory(
  int x,
  int y,
  int width,
  int height,
  const BYTE* data)
{
  CHECK_THREAD();

  glBindTexture(GL_TEXTURE_2D, m_handle);

  GLint texFormat = (m_format == MG_MEMORY_FORMAT_RGBA) ? GL_RGBA : GL_BGRA;
  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, texFormat, GL_UNSIGNED_BYTE, data);

  if (m_mipmap)
    glGenerateMipmap(GL_TEXTURE_2D);
}

//--------------------------------------------------------------
// constructor
mgGL33TextureArray::mgGL33TextureArray()
{
  m_handle = mgNullHandle;
}

//--------------------------------------------------------------
// destructor
mgGL33TextureArray::~mgGL33TextureArray()
{
  if (m_handle != mgNullHandle)
  {
    glDeleteTextures(1, &m_handle);
    m_handle = mgNullHandle;
  }
}

//--------------------------------------------------------------
// set texture wrap mode in x and y
void mgGL33TextureArray::setWrap(
  int xWrap,
  int yWrap)
{
  CHECK_THREAD();
  m_xWrap = xWrap;
  m_yWrap = yWrap;

  glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, mgGL33TextureWrap(m_xWrap));
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, mgGL33TextureWrap(m_yWrap));
}

//--------------------------------------------------------------
// constructor
mgGL33TextureCube::mgGL33TextureCube()
{
  m_handle = mgNullHandle;
}

//--------------------------------------------------------------
// destructor
mgGL33TextureCube::~mgGL33TextureCube()
{
  if (m_handle != mgNullHandle)
  {
    glDeleteTextures(1, &m_handle);
    m_handle = mgNullHandle;
  }
}

//--------------------------------------------------------------
// set texture wrap mode in x and y
void mgGL33TextureCube::setWrap(
  int xWrap,
  int yWrap)
{
  CHECK_THREAD();
  m_xWrap = xWrap;
  m_yWrap = yWrap;

  glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, mgGL33TextureWrap(m_xWrap));
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, mgGL33TextureWrap(m_yWrap));
}

//--------------------------------------------------------------
// constructor
mgGL33Services::mgGL33Services(
  const char* shaderDir,
  const char* fontDir)
: mgDisplayServices(shaderDir, fontDir)
{
#if defined(_DEBUG) && defined(WIN32)
  m_createThreadId = GetCurrentThreadId();
#endif

  m_cursorTexture = mgNullHandle;
  m_shader = mgNullHandle;
}

//--------------------------------------------------------------
// constructor
mgGL33Services::~mgGL33Services()
{
  CHECK_THREAD();

  // free all the textures
  const void* value;
  mgString key;
  int posn = m_textureImages.getStartPosition();
  while (posn != -1)
  {
    m_textureImages.getNextAssoc(posn, key, value);
    mgGL33TextureImage* texture = (mgGL33TextureImage*) value;
    delete texture;
  }

  m_textureImages.removeAll();

  // free all the texture arrays
  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgGL33TextureArray* texture = (mgGL33TextureArray*) m_textureArrays[i];
    delete texture;
  }
  m_textureArrays.removeAll();

  // free all the texture arrays
  for (int i = 0; i < m_textureCubes.length(); i++)
  {
    mgGL33TextureCube* texture = (mgGL33TextureCube*) m_textureCubes[i];
    delete texture;
  }
  m_textureCubes.removeAll();
}

//--------------------------------------------------------------
// create a 2D graphics surface
mgTextureSurface* mgGL33Services::createOverlaySurface()
{
  return new mgTextureSurface(false, false);
}

//--------------------------------------------------------------
// create a 2D graphics surface
mgTextureSurface* mgGL33Services::createTextureSurface()
{
  return new mgTextureSurface(true, true);
}

//--------------------------------------------------------------
// supports non-float shader arguments, bit operations
BOOL mgGL33Services::supportsIntegerVertex()
{
  return true;
}

//--------------------------------------------------------------
// can the display repeat textures
BOOL mgGL33Services::canRepeatTextures()
{
  return true;
}

//--------------------------------------------------------------
// release device resources
void mgGL33Services::deleteBuffers()
{
  unloadTextures();
}

//--------------------------------------------------------------
// restore device resources
void mgGL33Services::createBuffers()
{
  initView();

  reloadTextures();
  setCursorTexture(m_cursorFileName, m_cursorHotX, m_cursorHotY);
}

//--------------------------------------------------------------
// initialize for scene rendering
void mgGL33Services::initView()
{
  int windowX, windowY, windowWidth, windowHeight;
  mgPlatform->getWindowBounds(windowX, windowY, windowWidth, windowHeight);
  setScreenSize(windowWidth, windowHeight);

  setDPI(mgPlatform->getDPI());

  // if window is still 0 by 0, it's too early.  wait for real resize
  if (m_graphicsWidth <= 0 || m_graphicsHeight <= 0)
    return;

  mgDebug("view size is %d by %d", m_graphicsWidth, m_graphicsHeight);

  setProjection();
}
                
//--------------------------------------------------------------
// set projection
void mgGL33Services::setProjection()
{
  CHECK_THREAD();
  glViewport(0, 0, m_graphicsWidth, m_graphicsHeight);

  // create world LH perspective transform  
  double yFOV = m_FOV * PI/180.0;
  GLfloat yScale = (GLfloat) (1.0/tan(yFOV/2.0));
  GLfloat xScale = (yScale*m_graphicsHeight)/m_graphicsWidth;

  m_worldProjection.loadIdentity();
  m_worldProjection._11 = xScale;
  m_worldProjection._22 = yScale;
  m_worldProjection._33 = (m_viewBack + m_viewFront) / (m_viewBack - m_viewFront);
  m_worldProjection._34 = 1.0;
  m_worldProjection._43 = -2.0 * (m_viewFront*m_viewBack) / (m_viewBack - m_viewFront);
  m_worldProjection._44 = 0.0;

  m_viewWidth = 2*m_viewFront / xScale;
  m_viewHeight = 2*m_viewFront / yScale;

  m_updateShaderVars = true;
}

//-----------------------------------------------------------------------------
// load shader
void mgGL33Services::loadShader(
  const char* shaderName,             // name of shader
  const mgVertexAttrib* attribs)      // vertex attributes
{
  DWORD value;
  if (m_shaders.lookup(shaderName, value))
    return;  // already loaded

  mgString vertexShader;
  vertexShader.format("%s.vs", (const char*) shaderName);

  mgString fragmentShader;
  fragmentShader.format("%s.fs", (const char*) shaderName);

  mgShaderHandle shader = mgGL33loadShaderPair(m_shaderDir, vertexShader, fragmentShader, attribs);
  m_shaders.setAt(shaderName, (DWORD) shader);
}

//-----------------------------------------------------------------------------
// delete shader
void mgGL33Services::deleteShader(
  const char* shaderName)             // name of shader
{
  DWORD value;
  if (!m_shaders.lookup(shaderName, value))
    throw new mgErrorMsg("glShader", "shaderName", "%s", (const char*) shaderName);

  CHECK_THREAD();
  mgShaderHandle shader = (mgShaderHandle) value;
  glDeleteProgram(shader);
}

//-----------------------------------------------------------------------------
// set standard variables for shader
void mgGL33Services::setShaderStdUniforms(
  mgShaderHandle shader)
{
  mgMatrix4 mvpMatrix(m_worldMatrix);
  mvpMatrix.multiply(m_worldProjection);

  GLint index;
  GLfloat matrix[16];

  CHECK_THREAD();
  index = glGetUniformLocation(shader, "mgMVPMatrix");
  if (index != -1)
  {
    matrix4toGL(mvpMatrix, matrix);
    glUniformMatrix4fv(index, 1, GL_FALSE, matrix);
  }

  index = glGetUniformLocation(shader, "mgMVMatrix");
  if (index != -1)
  {
    matrix4toGL(m_worldMatrix, matrix);
    glUniformMatrix4fv(index, 1, GL_FALSE, matrix);
  }

  index = glGetUniformLocation(shader, "mgModelMatrix");
  if (index != -1)
  {
    matrix4toGL(m_modelMatrix, matrix);
    glUniformMatrix4fv(index, 1, GL_FALSE, matrix);
  }

  index = glGetUniformLocation(shader, "mgNormalMatrix");
  if (index != -1)
  {
    normalMatrix(m_worldMatrix, matrix);
    glUniformMatrix3fv(index, 1, GL_FALSE, matrix);
  }

  index = glGetUniformLocation(shader, "mgEyePt");
  if (index != -1)
  {
    glUniform3f(index, (GLfloat) m_eyePt.x, (GLfloat) m_eyePt.y, (GLfloat) m_eyePt.z);
  }

  // transform the light dir by eye matrix.  negate, since we want vector from point to light
  index = glGetUniformLocation(shader, "mgLightDir");
  if (index != -1)
  {
    mgPoint3 eyeLightDir;
    m_eyeMatrix.mapPt(m_lightDir, eyeLightDir);
    glUniform3f(index, (GLfloat) eyeLightDir.x, (GLfloat) eyeLightDir.y, (GLfloat) eyeLightDir.z);
  }

  index = glGetUniformLocation(shader, "mgLightColor");
  if (index != -1)
  {
    glUniform3f(index, (GLfloat) m_lightColor.x, (GLfloat) m_lightColor.y, (GLfloat) m_lightColor.z);
  }

  index = glGetUniformLocation(shader, "mgLightAmbient");
  if (index != -1)
  {
    glUniform3f(index, (GLfloat) m_lightAmbient.x, (GLfloat) m_lightAmbient.y, (GLfloat) m_lightAmbient.z);
  }

  index = glGetUniformLocation(shader, "mgMatColor");
  if (index != -1)
  {
    glUniform4f(index, (GLfloat) m_matColor.x, (GLfloat) m_matColor.y, (GLfloat) m_matColor.z, (GLfloat) m_matColor.w);
  }

  char varName[128];
  for (int unit = 0; ; unit++)
  {
    sprintf(varName, "mgTextureUnit%d", unit);
    index = glGetUniformLocation(shader, varName);
    if (index != -1)
      glUniform1i(index, unit);
    else break;
  }

  // vars up to date
  m_updateShaderVars = false;
}

//-----------------------------------------------------------------------------
// set current shader
void mgGL33Services::setShader(
  const char* shaderName)             // name of shader
{
  DWORD value;
  if (!m_shaders.lookup(shaderName, value))
    throw new mgErrorMsg("glShader", "shaderName", "%s", (const char*) shaderName);

  CHECK_THREAD();
  m_shader = (mgShaderHandle) value;
  glUseProgram(m_shader);

  m_updateShaderVars = true;
}

//-----------------------------------------------------------------------------
// set shader uniform value to matrix
void mgGL33Services::setShaderUniform(
  const char* shaderName,             // name of shader
  const char* varName,                // variable name
  const mgMatrix3& matrix)             // value
{
  DWORD value;
  if (!m_shaders.lookup(shaderName, value))
    throw new mgErrorMsg("glShader", "shaderName", "%s", (const char*) shaderName);

  CHECK_THREAD();
  mgShaderHandle shader = (mgShaderHandle) value;

  GLint index = glGetUniformLocation(shader, (const GLchar*) varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  GLfloat glmatrix[9];
  matrix3toGL(matrix, glmatrix);
  glUniformMatrix3fv(index, 1, GL_FALSE, glmatrix);
}

//-----------------------------------------------------------------------------
// set shader uniform value to matrix
void mgGL33Services::setShaderUniform(
  const char* shaderName,             // name of shader
  const char* varName,                // variable name
  const mgMatrix4& matrix)             // value
{
  DWORD value;
  if (!m_shaders.lookup(shaderName, value))
    throw new mgErrorMsg("glShader", "shaderName", "%s", (const char*) shaderName);

  CHECK_THREAD();
  mgShaderHandle shader = (mgShaderHandle) value;

  GLint index = glGetUniformLocation(shader, (const GLchar*) varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  GLfloat glmatrix[16];
  matrix4toGL(matrix, glmatrix);
  glUniformMatrix4fv(index, 1, GL_FALSE, glmatrix);
}

//-----------------------------------------------------------------------------
// set shader uniform value to Point3
void mgGL33Services::setShaderUniform(
  const char* shaderName,             // name of shader
  const char* varName,                // variable name
  const mgPoint3& point)              // value
{
  DWORD value;
  if (!m_shaders.lookup(shaderName, value))
    throw new mgErrorMsg("glShader", "shaderName", "%s", (const char*) shaderName);

  CHECK_THREAD();
  mgShaderHandle shader = (mgShaderHandle) value;

  GLint index = glGetUniformLocation(shader, (const GLchar*) varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform3f(index, (GLfloat) point.x, (GLfloat) point.y, (GLfloat) point.z);
}

//-----------------------------------------------------------------------------
// set shader uniform value to Point4
void mgGL33Services::setShaderUniform(
  const char* shaderName,             // name of shader
  const char* varName,                // variable name
  const mgPoint4& point)              // value
{
  DWORD value;
  if (!m_shaders.lookup(shaderName, value))
    throw new mgErrorMsg("glShader", "shaderName", "%s", (const char*) shaderName);

  CHECK_THREAD();
  mgShaderHandle shader = (mgShaderHandle) value;

  GLint index = glGetUniformLocation(shader, (const GLchar*) varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform4f(index, (GLfloat) point.x, (GLfloat) point.y, (GLfloat) point.z, (GLfloat) point.w);
}

//-----------------------------------------------------------------------------
// set shader uniform value to Point4
void mgGL33Services::setShaderUniform(
  const char* shaderName,             // name of shader
  const char* varName,                // variable name
  int intvalue)                       // value
{
  DWORD value;
  if (!m_shaders.lookup(shaderName, value))
    throw new mgErrorMsg("glShader", "shaderName", "%s", (const char*) shaderName);

  CHECK_THREAD();
  mgShaderHandle shader = (mgShaderHandle) value;

  GLint index = glGetUniformLocation(shader, (const GLchar*) varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform1i(index, (GLint) intvalue);
}

//-----------------------------------------------------------------------------
// set shader uniform value to Point4
void mgGL33Services::setShaderUniform(
  const char* shaderName,             // name of shader
  const char* varName,                // variable name
  float floatvalue)                   // value
{
  DWORD value;
  if (!m_shaders.lookup(shaderName, value))
    throw new mgErrorMsg("glShader", "shaderName", "%s", (const char*) shaderName);

  CHECK_THREAD();
  mgShaderHandle shader = (mgShaderHandle) value;

  GLint index = glGetUniformLocation(shader, (const GLchar*) varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform1f(index, (GLfloat) floatvalue);
}

//-----------------------------------------------------------------------------
// return matrix as float[16] array
void mgGL33Services::matrix4toGL(
  const mgMatrix4& m,
  float* values)
{
  values[ 0] = (float) m._11;
  values[ 1] = (float) m._12;
  values[ 2] = (float) m._13;
  values[ 3] = (float) m._14;
  values[ 4] = (float) m._21;
  values[ 5] = (float) m._22;
  values[ 6] = (float) m._23;
  values[ 7] = (float) m._24;
  values[ 8] = (float) m._31;
  values[ 9] = (float) m._32;
  values[10] = (float) m._33;
  values[11] = (float) m._34;
  values[12] = (float) m._41;
  values[13] = (float) m._42;
  values[14] = (float) m._43;
  values[15] = (float) m._44;
}

//-----------------------------------------------------------------------------
// return matrix as float[9] array
void mgGL33Services::matrix3toGL(
  const mgMatrix3& m,
  float* values)
{
  values[0] = (float) m._11;
  values[1] = (float) m._12;
  values[2] = (float) m._13;
  values[3] = (float) m._21;
  values[4] = (float) m._22;
  values[5] = (float) m._23;
  values[6] = (float) m._31;
  values[7] = (float) m._32;
  values[8] = (float) m._33;
}         

//--------------------------------------------------------------
// extract a normals matrix from world matrix
void mgGL33Services::normalMatrix(
  const mgMatrix4& matrix,
  float* result)
{
  double len1 = sqrt(matrix._11*matrix._11 + matrix._12*matrix._12 + matrix._13*matrix._13);
  double len2 = sqrt(matrix._21*matrix._21 + matrix._22*matrix._22 + matrix._23*matrix._23);
  double len3 = sqrt(matrix._31*matrix._31 + matrix._32*matrix._32 + matrix._33*matrix._33);
  result[0] = (float) (matrix._11 / len1);
  result[1] = (float) (matrix._12 / len1);
  result[2] = (float) (matrix._13 / len1);
  result[3] = (float) (matrix._21 / len2);
  result[4] = (float) (matrix._22 / len2);
  result[5] = (float) (matrix._23 / len2);
  result[6] = (float) (matrix._31 / len3);
  result[7] = (float) (matrix._32 / len3);
  result[8] = (float) (matrix._33 / len3);
}

//--------------------------------------------------------------
// clear the view
void mgGL33Services::clearView()
{
  CHECK_THREAD();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glFrontFace(GL_CW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
}

//--------------------------------------------------------------
// draw a texture to the overlay
void mgGL33Services::drawOverlayTexture(
  const mgTextureImage* texture,
  int x,
  int y,
  int width,
  int height)
{
  CHECK_THREAD();

  mgGL33TextureImage* ourTexture = (mgGL33TextureImage*) texture;
  mgPlatform->drawOverlayTexture(ourTexture->m_handle, x, y, width, height);
  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// draw a surface to the overlay
void mgGL33Services::drawOverlaySurface(
  const mgTextureSurface* surface,
  int x,
  int y)
{
  surface->drawOverlay(x, y);
}

//--------------------------------------------------------------
// draw the cursor image
void mgGL33Services::drawCursor()
{
  // draw the cursor, if enabled
  if (!m_cursorEnabled || m_cursorTexture == mgNullHandle)
    return;

  int x = m_cursorX;
  int y = m_cursorY;
  if (!m_cursorTrack)
  {
    x = m_graphicsWidth/2;
    y = m_graphicsHeight/2;
  }

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  drawOverlayTexture(m_cursorTexture, x - m_cursorHotX, y - m_cursorHotY, 
    m_cursorTexture->m_width, m_cursorTexture->m_height);
}

//--------------------------------------------------------------
// draw the view
void mgGL33Services::clearBuffer(
  int flags)
{
  CHECK_THREAD();

  GLbitfield mask = 0;
  if ((flags & MG_COLOR_BUFFER) != 0)
    mask |= GL_COLOR_BUFFER_BIT;

  if ((flags & MG_DEPTH_BUFFER) != 0)
    mask |= GL_DEPTH_BUFFER_BIT;

  glClear(mask);
}

//--------------------------------------------------------------
// allocate a vertex array
mgVertexBuffer* mgGL33Services::newVertexBuffer(
  int vertexSize,                   // size of vertex in bytes
  const mgVertexAttrib* attribs,    // array of attributes.  end with offset=-1
  int maxVertexes,                  // max number of vertexes
  BOOL dynamic)                     // support reset and reuse
{
  return new mgGL33VertexBuffer(vertexSize, attribs, maxVertexes, dynamic);
}

//--------------------------------------------------------------
// allocate an index array
mgIndexBuffer* mgGL33Services::newIndexBuffer(
  int size,                         // max number of indexes
  BOOL dynamic,                     // support reset and reuse
  BOOL longIndexes)                 // support 32-bit indexes
{
  return new mgGL33IndexBuffer(size, dynamic, longIndexes);
}

//--------------------------------------------------------------
// set the texture to use
void mgGL33Services::setTexture(
  const mgTextureImage* texture,
  int unit)
{
  CHECK_THREAD();
  mgGL33TextureImage* ourTexture = (mgGL33TextureImage*) texture;

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, ourTexture->m_handle); 
  glActiveTexture(GL_TEXTURE0);
}
  
//--------------------------------------------------------------
// set the texture to use
void mgGL33Services::setTexture(
  const mgTextureArray* textureArray,
  int unit)
{
  CHECK_THREAD();
  mgGL33TextureArray* ourTextureArray = (mgGL33TextureArray*) textureArray;

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D_ARRAY, ourTextureArray->m_handle); 
  glActiveTexture(GL_TEXTURE0);
}
  
//--------------------------------------------------------------
// set the texture to use
void mgGL33Services::setTexture(
  const mgTextureCube* textureCube,
  int unit)
{
  CHECK_THREAD();
  mgGL33TextureCube* ourTextureCube = (mgGL33TextureCube*) textureCube;

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, ourTextureCube->m_handle); 
  glActiveTexture(GL_TEXTURE0);
}
  
//--------------------------------------------------------------
// convert MG_ primitive types to GL types
inline GLenum mgGL33primType(
  int primType)
{
  switch (primType)
  {
    case MG_TRIANGLES: return GL_TRIANGLES;
    case MG_TRIANGLE_FAN: return GL_TRIANGLE_FAN;
    case MG_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case MG_LINES: return GL_LINES;
    case MG_LINE_STRIP: return GL_LINE_STRIP;
    case MG_POINTS: return GL_POINTS;

    default: return (GLenum) -1;
  }
}

//--------------------------------------------------------------
// draw the vertices
void mgGL33Services::draw(
  int primType,
  mgVertexBuffer* triangles)
{
  CHECK_THREAD();
  if (m_updateShaderVars)
    setShaderStdUniforms(m_shader);

  mgGL33VertexBuffer* outVertexes = (mgGL33VertexBuffer*) triangles;

  outVertexes->loadDisplay();

  glBindVertexArray(outVertexes->m_array);
  glBindBuffer(GL_ARRAY_BUFFER, outVertexes->m_buffer);

  glDrawArrays(mgGL33primType(primType), 0, outVertexes->m_count);

  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
  glBindVertexArray(mgNullHandle);
}

//--------------------------------------------------------------
// draw from separate vertex and index buffers
void mgGL33Services::draw(
  int primType,
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes)
{
  CHECK_THREAD();
  if (m_updateShaderVars)
    setShaderStdUniforms(m_shader);

  mgGL33VertexBuffer* ourVertexes = (mgGL33VertexBuffer*) vertexes;
  mgGL33IndexBuffer* ourIndexes = (mgGL33IndexBuffer*) indexes;

  ourVertexes->loadDisplay();
  ourIndexes->loadDisplay();

  int indexSize = ourIndexes->m_longIndexes ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;

  glBindVertexArray(ourVertexes->m_array);
  glBindBuffer(GL_ARRAY_BUFFER, ourVertexes->m_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourIndexes->m_buffer);

  glDrawElements(mgGL33primType(primType), ourIndexes->m_count, indexSize, (const GLvoid*) 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mgNullHandle);
  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
  glBindVertexArray(mgNullHandle);
}

//--------------------------------------------------------------
// draw from separate vertex and index buffers
void mgGL33Services::draw(
  int primType,
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int startIndex,
  int endIndex)
{
  CHECK_THREAD();
  if (m_updateShaderVars)
    setShaderStdUniforms(m_shader);

  mgGL33VertexBuffer* ourVertexes = (mgGL33VertexBuffer*) vertexes;
  mgGL33IndexBuffer* ourIndexes = (mgGL33IndexBuffer*) indexes;

  ourVertexes->loadDisplay();
  ourIndexes->loadDisplay();

  int indexType = ourIndexes->m_longIndexes ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
  int indexSize = ourIndexes->m_longIndexes ? sizeof(unsigned int) : sizeof(unsigned short);

  glBindVertexArray(ourVertexes->m_array);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourIndexes->m_buffer);
  int startOffset = startIndex*indexSize;

  glDrawElements(mgGL33primType(primType), endIndex-startIndex, indexType, (const GLvoid*) startOffset);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mgNullHandle);
  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
  glBindVertexArray(mgNullHandle);
}

//--------------------------------------------------------------
// start drawing decal background.  draw=false just sets depth
void mgGL33Services::decalBackground(
  BOOL draw)
{
  CHECK_THREAD();
  // set the stencil buffer based on depth test
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 1);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_ZERO, GL_REPLACE);
  glStencilOpSeparate(GL_BACK, GL_ZERO, GL_ZERO, GL_ZERO);

  // if we shouldn't draw the background, turn off write of color
  if (!draw)
  {
    glDepthFunc(GL_LEQUAL);
    glColorMask(false, false, false, false);
  }
}

//--------------------------------------------------------------
// start drawing decals
void mgGL33Services::decalStart()
{
  CHECK_THREAD();
  glDepthFunc(GL_LESS);
  glColorMask(true, true, true, true);

  // draw the decals masked by the stencil buffer
  glStencilFunc(GL_EQUAL, 1, 1);
  glDisable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
// end decals
void mgGL33Services::decalEnd()
{
  CHECK_THREAD();
  // restore old state
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
// set world transform
void mgGL33Services::setModelTransform(
  const mgMatrix4& xform)
{
  m_modelMatrix = xform;

  mgMatrix4 viewMatrix;
  viewMatrix.translate(-m_eyePt.x, -m_eyePt.y, -m_eyePt.z);
  viewMatrix.multiply(m_eyeMatrix);

  m_worldMatrix = m_modelMatrix;
  m_worldMatrix.multiply(viewMatrix);
  frustumBuildPlanes();

  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// append world transform
void mgGL33Services::appendModelTransform(
  const mgMatrix4& xform)
{
  m_modelMatrix.leftMultiply(xform);

  mgMatrix4 viewMatrix;
  viewMatrix.translate(-m_eyePt.x, -m_eyePt.y, -m_eyePt.z);
  viewMatrix.multiply(m_eyeMatrix);

  m_worldMatrix = m_modelMatrix;
  m_worldMatrix.multiply(viewMatrix);
  frustumBuildPlanes();

  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// get world transform
void mgGL33Services::getModelTransform(
  mgMatrix4& xform)
{
  xform = m_modelMatrix;
}

//--------------------------------------------------------------
// get MVP transform
void mgGL33Services::getMVPTransform(
  mgMatrix4& xform)
{
  xform = m_worldMatrix;
  xform.multiply(m_worldProjection);
}

//--------------------------------------------------------------
// get MV transform
void mgGL33Services::getMVTransform(
  mgMatrix4& xform)
{
  xform = m_worldMatrix;
}

//--------------------------------------------------------------
// set light position
void mgGL33Services::setLightDir(
  double x,
  double y,
  double z)
{
  m_lightDir.x = x;
  m_lightDir.y = y;
  m_lightDir.z = z;
  m_lightDir.normalize();

  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// set light color
void mgGL33Services::setLightColor(
  double r,
  double g,
  double b)
{
  m_lightColor.x = r;
  m_lightColor.y = g;
  m_lightColor.z = b;

  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// set light ambient
void mgGL33Services::setLightAmbient(
  double r,
  double g,
  double b)
{
  m_lightAmbient.x = r;
  m_lightAmbient.y = g;
  m_lightAmbient.z = b;

  m_updateShaderVars = true;
}

//-----------------------------------------------------------------------------
// set material color
void mgGL33Services::setMatColor(
  double r,
  double g,
  double b,
  double a)
{
  m_matColor.x = r;
  m_matColor.y = g;
  m_matColor.z = b;
  m_matColor.w = a;

  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// set zenable
void mgGL33Services::setZEnable(
  BOOL enable)
{
  CHECK_THREAD();
  if (enable)
    glEnable(GL_DEPTH_TEST);
  else glDisable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
// set culling 
void mgGL33Services::setCulling(
  BOOL enable)
{
  CHECK_THREAD();
  if (enable)
    glEnable(GL_CULL_FACE);
  else glDisable(GL_CULL_FACE);
}

//--------------------------------------------------------------
// set front face clockwise
void mgGL33Services::setFrontCW(
  BOOL enable)
{
  CHECK_THREAD();
  if (enable)
    glFrontFace(GL_CW);
  else glFrontFace(GL_CCW);
}

//--------------------------------------------------------------
// set transparent
void mgGL33Services::setTransparent(
  BOOL enable)
{
  CHECK_THREAD();
  if (enable)
    glEnable(GL_BLEND);
  else glDisable(GL_BLEND);
}

//-----------------------------------------------------------------------------
// set cursor pattern
void mgGL33Services::setCursorTexture(
  const char* fileName,
  int hotX,
  int hotY)
{
  m_cursorFileName = fileName;
  m_cursorHotX = hotX;
  m_cursorHotY = hotY;

  m_cursorTexture = (mgGL33TextureImage*) loadTexture(fileName);
}

//--------------------------------------------------------------
// load texture from image file
mgTextureImage* mgGL33Services::loadTexture(
  const char* fileName)
{
  const void *value;
  // if we've already created the texture
  if (m_textureImages.lookup(fileName, value))
    return (mgTextureImage*) value;

  mgGL33TextureImage* texture = new mgGL33TextureImage();
  texture->m_fileName = fileName;
  mgOSFixFileName(texture->m_fileName);

  reloadTextureImage(texture);

  // save the texture id (under original name.  copy in texture is fixed)
  m_textureImages.setAt(fileName, texture);

  return texture;
}

//--------------------------------------------------------------
// load texture array from file list
mgTextureArray* mgGL33Services::loadTextureArray(
  const mgStringArray& fileList) 
{
  mgGL33TextureArray* texture = new mgGL33TextureArray();

  // flag for each transparent texture
  texture->m_imgTransparent = new BOOL[fileList.length()];
  memset(texture->m_imgTransparent, 0, sizeof(BOOL)*fileList.length());

  // copy the file list into the texture
  mgString fileName;
  for (int i = 0; i < fileList.length(); i++)
  {
    fileName = fileList[i];
    mgOSFixFileName(fileName);
    texture->m_fileList.add(fileName);
  }

  reloadTextureArray(texture);

  m_textureArrays.add(texture);
  return texture;
}

//--------------------------------------------------------------
// load texture cube from file list
mgTextureCube* mgGL33Services::loadTextureCube(
  const char* xminImage,
  const char* xmaxImage,
  const char* yminImage,
  const char* ymaxImage,
  const char* zminImage,
  const char* zmaxImage)
{
  mgGL33TextureCube* texture = new mgGL33TextureCube();
  memset(texture->m_imgTransparent, 0, sizeof(texture->m_imgTransparent));

  // copy the file list into the texture
  texture->m_xminImage = xminImage;
  mgOSFixFileName(texture->m_xminImage);

  texture->m_xmaxImage = xmaxImage;
  mgOSFixFileName(texture->m_xminImage);

  texture->m_yminImage = yminImage;
  mgOSFixFileName(texture->m_yminImage);

  texture->m_ymaxImage = ymaxImage;
  mgOSFixFileName(texture->m_ymaxImage);

  texture->m_zminImage = zminImage;
  mgOSFixFileName(texture->m_zminImage);

  texture->m_zmaxImage = zmaxImage;
  mgOSFixFileName(texture->m_zmaxImage);

  reloadTextureCube(texture);

  m_textureCubes.add(texture);
  return texture;
}

//--------------------------------------------------------------
// create texture to be updated from memory
mgTextureImage* mgGL33Services::createTextureMemory(
  int width,
  int height,
  int format,
  BOOL mipmap)
{
  CHECK_THREAD();
  GLuint handle;
  glGenTextures(1, &handle);

  glBindTexture(GL_TEXTURE_2D, handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // turn off mipmapping if not requested
  if (!mipmap)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  GLint texFormat = (format == MG_MEMORY_FORMAT_RGBA) ? GL_RGBA : GL_BGRA;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, texFormat, GL_UNSIGNED_BYTE, NULL);

  mgGL33TextureImage* texture = new mgGL33TextureImage();
  texture->m_width = width;
  texture->m_height = height;
  texture->m_format = format;
  texture->m_mipmap = mipmap;

  texture->m_handle = handle;

  return texture;
}

//--------------------------------------------------------------
// reload texture image from file
void mgGL33Services::reloadTextureImage(
  mgGL33TextureImage* texture)
{
  CHECK_THREAD();
  glGenTextures(1, &texture->m_handle);

  glBindTexture(GL_TEXTURE_2D, texture->m_handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mgGL33TextureWrap(texture->m_xWrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mgGL33TextureWrap(texture->m_yWrap));

  int imgWidth, imgHeight;
  BYTE* data;
  mgLoadRGBAImage(texture->m_fileName, imgWidth, imgHeight, texture->m_transparent, data);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, 
    GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  texture->m_width = imgWidth;
  texture->m_height = imgHeight;

  glGenerateMipmap(GL_TEXTURE_2D);
}

//--------------------------------------------------------------
// reload texture array from file list
void mgGL33Services::reloadTextureArray(
  mgGL33TextureArray* texture)
{
  CHECK_THREAD();
  glGenTextures(1, &texture->m_handle);

  glBindTexture(GL_TEXTURE_2D_ARRAY, texture->m_handle);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, mgGL33TextureWrap(texture->m_xWrap));
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, mgGL33TextureWrap(texture->m_yWrap));

  // load the images in the file list into the array
  int arrayWidth = 0;
  int arrayHeight = 0;
  mgString fileName;
  for (int i = 0; i < texture->m_fileList.length(); i++)
  {
    fileName = texture->m_fileList[i];

    int imgWidth, imgHeight;
    BOOL hasAlpha;
    BYTE* data;
    mgLoadRGBAImage(fileName, imgWidth, imgHeight, hasAlpha, data);

		texture->m_imgTransparent[i] = hasAlpha;

    // with first image, we have size.  create the texture array
    if (i == 0)
    {
      arrayWidth = imgWidth;
      arrayHeight = imgHeight;
      glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 
        arrayWidth, arrayHeight, texture->m_fileList.length(), 
        0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
      if (imgWidth != arrayWidth || imgHeight != arrayHeight)
      throw new mgErrorMsg("glTextureArray", "fileName,wd,ht,arraywd,arrayht", 
        (const char*) fileName, imgWidth, imgHeight, arrayWidth, arrayHeight);
    }

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 
      0, 0, i, arrayWidth, arrayHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    delete data;
  }

  texture->m_width = arrayWidth;
  texture->m_height = arrayHeight;

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

//--------------------------------------------------------------
// reload texture cube from file list
void mgGL33Services::reloadTextureCube(
  mgGL33TextureCube* texture)
{
  CHECK_THREAD();
  glGenTextures(1, &texture->m_handle);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture->m_handle);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, mgGL33TextureWrap(texture->m_xWrap));
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, mgGL33TextureWrap(texture->m_yWrap));

  // load the images in the file list into the array
  int arrayWidth = 0;
  int arrayHeight = 0;

  // load XMIN image
  int imgWidth, imgHeight;
  BOOL hasAlpha;
  BYTE* data;
  mgLoadRGBAImage(texture->m_xminImage, imgWidth, imgHeight, hasAlpha, data);

	texture->m_imgTransparent[0] = hasAlpha;
  arrayWidth = imgWidth;            
  arrayHeight = imgHeight;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load XMAX image 
  mgLoadRGBAImage(texture->m_xmaxImage, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgErrorMsg("glTextureArray", "fileName,wd,ht,arraywd,arrayht", 
      (const char*) texture->m_xmaxImage, imgWidth, imgHeight, arrayWidth, arrayHeight);

	texture->m_imgTransparent[1] = hasAlpha;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load YMIN image 
  mgLoadRGBAImage(texture->m_yminImage, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgErrorMsg("glTextureArray", "fileName,wd,ht,arraywd,arrayht", 
      (const char*) texture->m_yminImage, imgWidth, imgHeight, arrayWidth, arrayHeight);

	texture->m_imgTransparent[2] = hasAlpha;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load YMAX image 
  mgLoadRGBAImage(texture->m_ymaxImage, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgErrorMsg("glTextureArray", "fileName,wd,ht,arraywd,arrayht", 
      (const char*) texture->m_ymaxImage, imgWidth, imgHeight, arrayWidth, arrayHeight);

	texture->m_imgTransparent[3] = hasAlpha;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load ZMIN image 
  mgLoadRGBAImage(texture->m_zminImage, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgErrorMsg("glTextureArray", "fileName,wd,ht,arraywd,arrayht", 
      (const char*) texture->m_zminImage, imgWidth, imgHeight, arrayWidth, arrayHeight);

	texture->m_imgTransparent[4] = hasAlpha;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load ZMAX image 
  mgLoadRGBAImage(texture->m_zmaxImage, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgErrorMsg("glTextureArray", "fileName,wd,ht,arraywd,arrayht", 
      (const char*) texture->m_zmaxImage, imgWidth, imgHeight, arrayWidth, arrayHeight);

	texture->m_imgTransparent[5] = hasAlpha;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  texture->m_width = arrayWidth;
  texture->m_height = arrayHeight;

  // generate mipmap
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

//--------------------------------------------------------------
// release device resources
void mgGL33Services::unloadTextures()
{
  CHECK_THREAD();
  // release texture data
  const void* value;
  mgString key;
  int posn = m_textureImages.getStartPosition();
  while (posn != -1)
  {
    m_textureImages.getNextAssoc(posn, key, value);
    mgGL33TextureImage* texture = (mgGL33TextureImage*) value;
    if (texture->m_handle != mgNullHandle)
    {
      glDeleteTextures(1, &texture->m_handle);
      texture->m_handle = mgNullHandle;
    }
  }

  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgGL33TextureArray* texture = (mgGL33TextureArray*) m_textureArrays[i];
    glDeleteTextures(1, &texture->m_handle);
    texture->m_handle = mgNullHandle;
  }

  for (int i = 0; i < m_textureCubes.length(); i++)
  {
    mgGL33TextureCube* texture = (mgGL33TextureCube*) m_textureCubes[i];
    glDeleteTextures(1, &texture->m_handle);
    texture->m_handle = mgNullHandle;
  }
}

//--------------------------------------------------------------
// restore device resources
void mgGL33Services::reloadTextures()
{
  CHECK_THREAD();
  // restore textures from source file
  const void* value;
  mgString key;
  int posn = m_textureImages.getStartPosition();
  while (posn != -1)
  {
    m_textureImages.getNextAssoc(posn, key, value);
    mgGL33TextureImage* texture = (mgGL33TextureImage*) value;
    
    reloadTextureImage(texture);
  }

  // reload texture arrays
  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgGL33TextureArray* texture = (mgGL33TextureArray*) m_textureArrays[i];
    reloadTextureArray(texture);
  }

  // reload texture cubes
  for (int i = 0; i < m_textureCubes.length(); i++)
  {
    mgGL33TextureCube* texture = (mgGL33TextureCube*) m_textureCubes[i];
    reloadTextureCube(texture);
  }
}

#endif
