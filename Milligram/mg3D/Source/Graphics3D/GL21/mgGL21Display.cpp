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

#include "mgUtil/Include/ImageUtil/mgImageUtil.h"

#include "mgGL21Types.h"

#include "mgGL21Misc.h"
#include "mgGL21Display.h"
#include "mgGL21VertexBuffer.h"
#include "mgGL21IndexBuffer.h"
#include "../GLTextureSurface/mgGLTextureSurface.h"

#include "mgPlatform/Include/mgPlatformServices.h"
#include "mgPlatform/Include/mgApplication.h"

#include "../mgVertexOverlaySolid.h"
#include "../mgVertexOverlayImage.h"
#include "../mgVertexOverlayText.h"

#ifdef SUPPORT_FTFONT
#include "mg2D/Include/Surfaces/mgFTFont.h"
#endif

//--------------------------------------------------------------
// constructor
mgGL21TextureImage::mgGL21TextureImage()
{
  m_handle = mgNullHandle;
  m_format = 0;
  m_mipmap = true;
}

//--------------------------------------------------------------
// destructor
mgGL21TextureImage::~mgGL21TextureImage()
{
  if (m_handle != mgNullHandle)
  {
    glDeleteTextures(1, &m_handle);
    m_handle = mgNullHandle;
  }
}

//--------------------------------------------------------------
// convert framework wrap constants to GL
inline GLuint mgGL21TextureWrap(
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
// convert framework texture constants to GL
inline GLuint mgGL21TextureFormat(
  int format)
{
  switch (format)
  {
    case MG_MEMORY_FORMAT_RGBA: return GL_RGBA;
    case MG_MEMORY_FORMAT_BGRA: return GL_BGRA;
    case MG_MEMORY_FORMAT_GRAY: return GL_RED;
    default: return GL_RGBA;
  }
}

//--------------------------------------------------------------
// set texture wrap mode in x and y
void mgGL21TextureImage::setWrap(
  int xWrap,
  int yWrap)
{
  m_xWrap = xWrap;
  m_yWrap = yWrap;

  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mgGL21TextureWrap(m_xWrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mgGL21TextureWrap(m_yWrap));
}

//--------------------------------------------------------------
// set texture filter
void mgGL21TextureImage::setFilter(
  int filter)
{
}

//--------------------------------------------------------------
// update memory texture
void mgGL21TextureImage::updateMemory(
  int x,
  int y,
  int width,
  int height,
  const BYTE* data)
{
  glBindTexture(GL_TEXTURE_2D, m_handle);

  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, mgGL21TextureFormat(m_format), GL_UNSIGNED_BYTE, data);

#ifndef GL_GENERATE_MIPMAP
  if (m_mipmap)
    glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

//--------------------------------------------------------------
// constructor
mgGL21TextureArray::mgGL21TextureArray()
{
  m_handle = mgNullHandle;
}

//--------------------------------------------------------------
// destructor
mgGL21TextureArray::~mgGL21TextureArray()
{
  if (m_handle != mgNullHandle)
  {
    glDeleteTextures(1, &m_handle);
    m_handle = mgNullHandle;
  }
}

//--------------------------------------------------------------
// set texture wrap mode in x and y
void mgGL21TextureArray::setWrap(
  int xWrap,
  int yWrap)
{
  m_xWrap = xWrap;
  m_yWrap = yWrap;

// =-= not supported under OpenGL 2.1  Do in shader?
//  glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle);

//  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, mgGL21TextureWrap(m_xWrap));
//  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, mgGL21TextureWrap(m_yWrap));
}

//--------------------------------------------------------------
// set texture filter
void mgGL21TextureArray::setFilter(
  int filter)
{
}

//--------------------------------------------------------------
// constructor
mgGL21TextureCube::mgGL21TextureCube()
{
  m_handle = mgNullHandle;
}

//--------------------------------------------------------------
// destructor
mgGL21TextureCube::~mgGL21TextureCube()
{
  if (m_handle != mgNullHandle)
  {
    glDeleteTextures(1, &m_handle);
    m_handle = mgNullHandle;
  }
}

//--------------------------------------------------------------
// set texture wrap mode in x and y
void mgGL21TextureCube::setWrap(
  int xWrap,
  int yWrap)
{
  m_xWrap = xWrap;
  m_yWrap = yWrap;

  glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, mgGL21TextureWrap(m_xWrap));
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, mgGL21TextureWrap(m_yWrap));
}

//--------------------------------------------------------------
// set texture filter
void mgGL21TextureCube::setFilter(
  int filter)
{
}

//--------------------------------------------------------------
// constructor
mgGL21Shader::mgGL21Shader(
  const char* name)
: mgShader(name)
{
}

//--------------------------------------------------------------
// destructor
mgGL21Shader::~mgGL21Shader()
{
}

//--------------------------------------------------------------
// find index of uniform
GLint mgGL21Shader::uniformIndex(
  const char* name)
{
  DWORD value;
  if (m_uniforms.lookup(name, value))
    return (GLint) value;

  GLint index = glGetUniformLocation(m_handle, name);
  m_uniforms.setAt(name, (DWORD) index);
  return index;
}

//--------------------------------------------------------------
// constructor
mgGL21Display::mgGL21Display(
  const char* shaderDir,
  const char* fontDir)
: mgDisplayServices(shaderDir, fontDir)
{
  m_textureArray = NULL;
  m_shader = NULL;
  m_maxVertexAttrib = 0;

  m_framebuffer = 0;
  loadOverlayShaders();

#ifdef SUPPORT_FTFONT
  // initialize FTFont library
  mgFTInit();
#endif
}

//--------------------------------------------------------------
// constructor
mgGL21Display::~mgGL21Display()
{
#ifdef SUPPORT_FTFONT
  // terminate FTFont library
  mgFTTerm();
#endif

  // free all the textures
  const void* value;
  mgString key;
  int posn = m_textureImages.getStartPosition();
  while (posn != -1)
  {
    m_textureImages.getNextAssoc(posn, key, value);
    mgGL21TextureImage* texture = (mgGL21TextureImage*) value;
    delete texture;
  }

  m_textureImages.removeAll();

  // free all the texture arrays
  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgGL21TextureArray* texture = (mgGL21TextureArray*) m_textureArrays[i];
    delete texture;
  }
  m_textureArrays.removeAll();

  // free all the texture arrays
  for (int i = 0; i < m_textureCubes.length(); i++)
  {
    mgGL21TextureCube* texture = (mgGL21TextureCube*) m_textureCubes[i];
    delete texture;
  }
  m_textureCubes.removeAll();

  // free all the shaders
  posn = m_shaders.getStartPosition();
  while (posn != -1)
  {
    m_shaders.getNextAssoc(posn, key, value);
    mgGL21Shader* shader = (mgGL21Shader*) value;
    delete shader;
  }
  m_shaders.removeAll();

  if (m_framebuffer != 0)
  {
    glDeleteFramebuffers(1, &m_framebuffer);
    m_framebuffer = 0;
  }
}

//--------------------------------------------------------------
// create a 2D graphics surface
mgTextureSurface* mgGL21Display::createOverlaySurface()
{
  return new mgGLTextureSurface(false, false);
}

//--------------------------------------------------------------
// create a 2D graphics surface
mgTextureSurface* mgGL21Display::createTextureSurface()
{
  return new mgGLTextureSurface(true, true);
}

//--------------------------------------------------------------
// render to a texture
void mgGL21Display::renderToTexture(
  mgTextureImage* target,
  BOOL withDepth)
{
  mgGL21TextureImage* ourTexture = (mgGL21TextureImage*) target;

  if (m_framebuffer == 0)
  glGenFramebuffers(1, &m_framebuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  // set texture as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ourTexture->m_handle, 0);
 
  // set the list of draw buffers.
  GLenum DRAW_BUFFERS[2] = {GL_COLOR_ATTACHMENT0, 0};
  glDrawBuffers(1, DRAW_BUFFERS); // "1" is the size of DrawBuffers

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    mgDebug("CheckFramebufferStatus failed.");

  setProjection(ourTexture->m_width, ourTexture->m_height);

  // set default graphics state
  glFrontFace(GL_CW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glEnable(GL_SCISSOR_TEST);
}

//--------------------------------------------------------------
// return to rendering to display
void mgGL21Display::renderToDisplay()
{
  // set render target back to display
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  setProjection(m_graphicsWidth, m_graphicsHeight);

  // set default graphics state
  glFrontFace(GL_CW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glDisable(GL_SCISSOR_TEST);
}

//--------------------------------------------------------------
// supports non-float shader arguments, bit operations
BOOL mgGL21Display::supportsIntegerVertex()
{
  return false;
}

//--------------------------------------------------------------
// can the display repeat textures
BOOL mgGL21Display::canRepeatTextures()
{
  return false;
}

//--------------------------------------------------------------
// release device resources
void mgGL21Display::deleteBuffers()
{
  unloadTextures();
}

//--------------------------------------------------------------
// restore device resources
void mgGL21Display::createBuffers()
{
  initView();

  reloadTextures();

  setCursorTexture(m_cursorFileName, m_cursorHotX, m_cursorHotY);
}

//--------------------------------------------------------------
// initialize for scene rendering
void mgGL21Display::initView()
{
  int windowX, windowY, windowWidth, windowHeight;
  mgPlatform->getWindowBounds(windowX, windowY, windowWidth, windowHeight);
  setScreenSize(windowWidth, windowHeight);

  setDPI(mgPlatform->getDPI());

  // if window is still 0 by 0, it's too early.  wait for real resize
  if (m_graphicsWidth <= 0 || m_graphicsHeight <= 0)
    return;

  mgDebug("view size is %d by %d", m_graphicsWidth, m_graphicsHeight);

  setProjection(m_graphicsWidth, m_graphicsHeight);
}
                
//--------------------------------------------------------------
// set projection
void mgGL21Display::setProjection(
  int width,
  int height)
{
  glViewport(0, 0, width, height);
  glScissor(0, 0, width, height);

  // create world LH perspective transform  
  double yFOV = m_FOV * PI/180.0;
  GLfloat yScale = (GLfloat) (1.0/tan(yFOV/2.0));
  GLfloat xScale = (yScale*height)/width;

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
mgShader* mgGL21Display::loadShader(
  const char* shaderName,             // name of shader
  const mgVertexAttrib* attribs)      // vertex attributes
{
  const void* value;
  if (m_shaders.lookup(shaderName, value))
    return (mgShader*) value; 

  mgString vertexShader;
  vertexShader.format("%s.vs", (const char*) shaderName);

  mgString fragmentShader;
  fragmentShader.format("%s.fs", (const char*) shaderName);

  mgShaderHandle handle = mgGL21loadShaderPair(m_shaderDir, vertexShader, fragmentShader, attribs);

  mgGL21Shader* shader = new mgGL21Shader(shaderName);
  shader->m_handle = handle;
  m_shaders.setAt(shaderName, shader);

  return shader;
}

//-----------------------------------------------------------------------------
// load shader
mgShader* mgGL21Display::loadShaderSource(
  const char* shaderName,             // name of shader
  const char* vertexSource,           // vertex shader source
  const char* fragmentSource,         // fragment shader source
  const mgVertexAttrib* attribs)      // vertex attributes
{
  const void* value;
  if (m_shaders.lookup(shaderName, value))
    return (mgShader*) value;

  mgShaderHandle handle = mgGL21loadShaderPairSource(shaderName, vertexSource, fragmentSource, attribs);

  mgGL21Shader* shader = new mgGL21Shader(shaderName);
  shader->m_handle = handle;
  m_shaders.setAt(shaderName, shader);

  return shader;
}

//-----------------------------------------------------------------------------
// delete shader
void mgGL21Display::deleteShader(
  mgShader* shader)                           // shader instance
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;
  m_shaders.removeKey(ourShader->m_name);
  glDeleteProgram(ourShader->m_handle);
  delete ourShader;
}

//-----------------------------------------------------------------------------
// set current shader
void mgGL21Display::setShader(
  mgShader* shader)                             // shader instance
{
  m_shader = (mgGL21Shader*) shader;
  glUseProgram(m_shader->m_handle);

  m_updateShaderVars = true;
}

//-----------------------------------------------------------------------------
// set shader uniform value to matrix
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  const mgMatrix3& matrix)             // value
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  GLfloat glmatrix[9];
  matrix3toGL(matrix, glmatrix);
  glUniformMatrix3fv(index, 1, GL_FALSE, glmatrix);
}

//-----------------------------------------------------------------------------
// set shader uniform value to matrix
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  const mgMatrix4& matrix)             // value
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  GLfloat glmatrix[16];
  matrix4toGL(matrix, glmatrix);
  glUniformMatrix4fv(index, 1, GL_FALSE, glmatrix);
}

//-----------------------------------------------------------------------------
// set shader uniform value to x,y
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  float x,                            // value
  float y)
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform2f(index, (GLfloat) x, (GLfloat) y);
}

//-----------------------------------------------------------------------------
// set shader uniform value to Point3
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  const mgPoint3& point)              // value
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform3f(index, (GLfloat) point.x, (GLfloat) point.y, (GLfloat) point.z);
}

//-----------------------------------------------------------------------------
// set shader uniform value to array of points
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  int count,                          // array size
  const mgPoint3* points)             // point array
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  GLfloat* values = new GLfloat[count*3];
  for (int i = 0; i < count; i++)
  {
    values[3*i] = (GLfloat) points[i].x;
    values[3*i+1] = (GLfloat) points[i].y;
    values[3*i+2] = (GLfloat) points[i].z;
  }

  glUniform3fv(index, count, values);
  delete values;
}

//-----------------------------------------------------------------------------
// set shader uniform value to Point4
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  const mgPoint4& point)              // value
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform4f(index, (GLfloat) point.x, (GLfloat) point.y, (GLfloat) point.z, (GLfloat) point.w);
}

//-----------------------------------------------------------------------------
// set shader uniform value to integer
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  int intvalue)                       // value
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform1i(index, (GLint) intvalue);
}

//-----------------------------------------------------------------------------
// set shader uniform value to float
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  float floatvalue)                       // value
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform1f(index, (GLfloat) floatvalue);
}

//-----------------------------------------------------------------------------
// set shader uniform value to float array
void mgGL21Display::setShaderUniform(
  mgShader* shader,                             // shader instance
  const char* varName,                // variable name
  int count,                          // array size
  const float* values)                // float array
{
  mgGL21Shader* ourShader = (mgGL21Shader*) shader;

  GLint index = ourShader->uniformIndex(varName);
  if (index == -1)
    return; // throw new mgException("setShaderUniform %s.%s not found", (const char*) shaderName, (const char*) varName);

  glUniform1fv(index, count, (const GLfloat*) values);
}

//-----------------------------------------------------------------------------
// set standard variables for shader
void mgGL21Display::setShaderStdUniforms(
  mgGL21Shader* shader)                            // shader instance
{
  mgMatrix4 mvpMatrix(m_worldMatrix);
  mvpMatrix.multiply(m_worldProjection);

  GLint index;
  GLfloat matrix[16];

  index = shader->uniformIndex("mgMVPMatrix");
  if (index != -1)
  {
    matrix4toGL(mvpMatrix, matrix);
    glUniformMatrix4fv(index, 1, GL_FALSE, matrix);
  }

  index = shader->uniformIndex("mgMVMatrix");
  if (index != -1)
  {
    matrix4toGL(m_worldMatrix, matrix);
    glUniformMatrix4fv(index, 1, GL_FALSE, matrix);
  }

  index = shader->uniformIndex("mgModelMatrix");
  if (index != -1)
  {
    matrix4toGL(m_modelMatrix, matrix);
    glUniformMatrix4fv(index, 1, GL_FALSE, matrix);
  }

  index = shader->uniformIndex("mgNormalMatrix");
  if (index != -1)
  {
    normalMatrix(m_worldMatrix, matrix);
    glUniformMatrix3fv(index, 1, GL_FALSE, matrix);
  }

  index = shader->uniformIndex("mgEyePt");
  if (index != -1)
  {
    glUniform3f(index, (GLfloat) m_eyePt.x, (GLfloat) m_eyePt.y, (GLfloat) m_eyePt.z);
  }

  // transform the light dir by eye matrix.  negate, since we want vector from point to light
  index = shader->uniformIndex("mgLightDir");
  if (index != -1)
  {
    mgPoint3 eyeLightDir;
    m_eyeMatrix.mapPt(m_lightDir, eyeLightDir);
    glUniform3f(index, (GLfloat) eyeLightDir.x, (GLfloat) eyeLightDir.y, (GLfloat) eyeLightDir.z);
  }

  index = shader->uniformIndex("mgLightColor");
  if (index != -1)
  {
    glUniform3f(index, (GLfloat) m_lightColor.x, (GLfloat) m_lightColor.y, (GLfloat) m_lightColor.z);
  }

  index = shader->uniformIndex("mgLightAmbient");
  if (index != -1)
  {
    glUniform3f(index, (GLfloat) m_lightAmbient.x, (GLfloat) m_lightAmbient.y, (GLfloat) m_lightAmbient.z);
  }

  index = shader->uniformIndex("mgMatColor");
  if (index != -1)
  {
    glUniform4f(index, (GLfloat) m_matColor.x, (GLfloat) m_matColor.y, (GLfloat) m_matColor.z, (GLfloat) m_matColor.w);
  }

  char varName[128];
  for (int unit = 0; ; unit++)
  {
    sprintf(varName, "mgTextureUnit%d", unit);
    index = shader->uniformIndex(varName);
    if (index != -1)
      glUniform1i(index, unit);
    else break;
  }

  // if a texture array is in use, tell shader dimensions
  if (m_textureArray != NULL)
  {
    index = shader->uniformIndex("mgAtlasSize");
    if (index != -1)
    {
      glUniform2f(index, (GLfloat) m_textureArray->m_atlasWidth, (GLfloat) m_textureArray->m_atlasHeight);
    }

    index = shader->uniformIndex("mgTextureSize");
    if (index != -1)
    {
      glUniform2f(index, (GLfloat) m_textureArray->m_width, (GLfloat) m_textureArray->m_height);
    }
  }

  // vars are up to date
  m_updateShaderVars = false;
}

//-----------------------------------------------------------------------------
// return matrix as float[16] array
void mgGL21Display::matrix4toGL(
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
void mgGL21Display::matrix3toGL(
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
void mgGL21Display::normalMatrix(
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
void mgGL21Display::clearView()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glFrontFace(GL_CW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);

  m_textureArray = NULL;
}

//--------------------------------------------------------------
// draw a texture to the overlay
void mgGL21Display::drawOverlayTexture(
  const mgTextureImage* texture,
  int x,
  int y,
  int width,
  int height)
{
  mgGL21TextureImage* ourTexture = (mgGL21TextureImage*) texture;
  if (mgPlatform != NULL)
    mgPlatform->drawOverlayTexture(ourTexture->m_handle, x, y, width, height);

  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// draw a surface to the overlay
void mgGL21Display::drawOverlaySurface(
  const mgTextureSurface* surface,
  int x,
  int y)
{
  surface->drawOverlay(x, y);
}

//--------------------------------------------------------------
// draw the cursor image
void mgGL21Display::drawCursor()
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
void mgGL21Display::clearBuffer(
  int flags)
{
  GLbitfield mask = 0;
  if ((flags & MG_COLOR_BUFFER) != 0)
    mask |= GL_COLOR_BUFFER_BIT;

  if ((flags & MG_DEPTH_BUFFER) != 0)
    mask |= GL_DEPTH_BUFFER_BIT;

  glClear(mask);
}

//--------------------------------------------------------------
// allocate a vertex array
mgVertexBuffer* mgGL21Display::newVertexBuffer(
  int vertexSize,                   // size of vertex in bytes
  const mgVertexAttrib* attribs,    // array of attributes.  end with offset=-1
  int maxVertexes,                  // max number of vertexes
  BOOL dynamic)                     // support reset and reuse
{
  return new mgGL21VertexBuffer(vertexSize, attribs, maxVertexes, dynamic);
}

//--------------------------------------------------------------
// allocate an index array
mgIndexBuffer* mgGL21Display::newIndexBuffer(
  int size,                         // max number of indexes
  BOOL dynamic,                     // support reset and reuse
  BOOL longIndexes)                 // support 32-bit indexes
{
  return new mgGL21IndexBuffer(size, dynamic, longIndexes);
}

//--------------------------------------------------------------
// set the texture to use
void mgGL21Display::setTexture(
  const mgTextureImage* texture,
  int unit)
{
  mgGL21TextureImage* ourTexture = (mgGL21TextureImage*) texture;

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, ourTexture->m_handle); 
  glActiveTexture(GL_TEXTURE0);

  m_textureArray = NULL;
  m_updateShaderVars = true;
}
  
//--------------------------------------------------------------
// set the texture to use
void mgGL21Display::setTexture(
  const mgTextureArray* textureArray,
  int unit)
{
  mgGL21TextureArray* ourTextureArray = (mgGL21TextureArray*) textureArray;

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, ourTextureArray->m_handle); 
  glActiveTexture(GL_TEXTURE0);

  m_textureArray = ourTextureArray;
  m_updateShaderVars = true;
}
  
//--------------------------------------------------------------
// set the texture to use
void mgGL21Display::setTexture(
  const mgTextureCube* textureCube,
  int unit)
{
  mgGL21TextureCube* ourTextureCube = (mgGL21TextureCube*) textureCube;

  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, ourTextureCube->m_handle); 
  glActiveTexture(GL_TEXTURE0);

  m_textureArray = NULL;
  m_updateShaderVars = true;
}
  
//--------------------------------------------------------------
// convert MG_ primitive types to GL types
inline GLenum mgGL21primType(
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
// set vertex buffer up for use
void mgGL21Display::useVertexBuffer(
  mgGL21VertexBuffer* vertexes)
{
  const mgVertexAttrib* attribs = vertexes->m_attribs;
  int vertexSize = vertexes->m_vertexSize;

  // bind arrays to attribute slots
  int i;
  for (i = 0; ; i++)
  {
    if (attribs[i].m_name == NULL)
      break;

    glEnableVertexAttribArray(i);
    switch (attribs[i].m_type)
    {
      case MG_VERTEX_UBYTE1:
        glVertexAttribPointer(i, 1, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
      break;
      case MG_VERTEX_UBYTE2:
        glVertexAttribPointer(i, 2, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
      break;
      case MG_VERTEX_UBYTE3:
        glVertexAttribPointer(i, 3, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
      break;
      case MG_VERTEX_UBYTE4:
        glVertexAttribPointer(i, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
      break;

      case MG_VERTEX_UINT1:
        glVertexAttribPointer(i, 2, GL_UNSIGNED_SHORT, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
      break;

      case MG_VERTEX_UINT2:
        glVertexAttribPointer(i, 4, GL_UNSIGNED_SHORT, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
      break;

      case MG_VERTEX_UINT3:
        // =-= cast these as vec4 and vec2?
        glVertexAttribPointer(i, 3, GL_UNSIGNED_INT, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
      break;

      case MG_VERTEX_FLOAT1:
        glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
        break;
      case MG_VERTEX_FLOAT2:
        glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
        break;
      case MG_VERTEX_FLOAT3:
        glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
        break;
      case MG_VERTEX_FLOAT4:
        glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, vertexSize, (const GLvoid*) attribs[i].m_offset);
        break;
    }
  }
  int newMaxVertexAttrib = i;

  // turn off any attributes left on from last use
  for (; i < m_maxVertexAttrib; i++)
  {
    glDisableVertexAttribArray(i);
  }
  m_maxVertexAttrib = newMaxVertexAttrib;
}

//--------------------------------------------------------------
// draw the vertices
void mgGL21Display::draw(
  int primType,
  mgVertexBuffer* triangles)
{
  if (m_updateShaderVars)
    setShaderStdUniforms(m_shader);

  mgGL21VertexBuffer* ourVertexes = (mgGL21VertexBuffer*) triangles;

  ourVertexes->loadDisplay();

  // draw the triangles
  glBindBuffer(GL_ARRAY_BUFFER, ourVertexes->m_buffer);
  useVertexBuffer(ourVertexes);

  glDrawArrays(mgGL21primType(primType), 0, ourVertexes->m_count);

  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
}

//--------------------------------------------------------------
// draw the vertices
void mgGL21Display::draw(
  int primType,
  mgVertexBuffer* triangles,
  int startIndex,
  int endIndex)
{
  if (m_updateShaderVars)
    setShaderStdUniforms(m_shader);

  mgGL21VertexBuffer* ourVertexes = (mgGL21VertexBuffer*) triangles;

  ourVertexes->loadDisplay();

  // draw the triangles
  glBindBuffer(GL_ARRAY_BUFFER, ourVertexes->m_buffer);
  useVertexBuffer(ourVertexes);

  glDrawArrays(mgGL21primType(primType), startIndex, endIndex-startIndex);

  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
}

//--------------------------------------------------------------
// draw from separate vertex and index buffers
void mgGL21Display::draw(
  int primType,
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes)
{
  if (m_updateShaderVars)
    setShaderStdUniforms(m_shader);

  mgGL21VertexBuffer* ourVertexes = (mgGL21VertexBuffer*) vertexes;
  mgGL21IndexBuffer* ourIndexes = (mgGL21IndexBuffer*) indexes;

  ourVertexes->loadDisplay();
  ourIndexes->loadDisplay();

  int indexSize = ourIndexes->m_longIndexes ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;

  glBindBuffer(GL_ARRAY_BUFFER, ourVertexes->m_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourIndexes->m_buffer);
  useVertexBuffer(ourVertexes);

  glDrawElements(mgGL21primType(primType), ourIndexes->m_count, indexSize, (const GLvoid*) 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mgNullHandle);
  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
}

//--------------------------------------------------------------
// draw from separate vertex and index buffers
void mgGL21Display::draw(
  int primType,
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int startIndex,
  int endIndex)
{
  if (m_updateShaderVars)
    setShaderStdUniforms(m_shader);

  mgGL21VertexBuffer* ourVertexes = (mgGL21VertexBuffer*) vertexes;
  mgGL21IndexBuffer* ourIndexes = (mgGL21IndexBuffer*) indexes;

  ourVertexes->loadDisplay();
  ourIndexes->loadDisplay();

  int indexType = ourIndexes->m_longIndexes ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
  int indexSize = ourIndexes->m_longIndexes ? sizeof(unsigned int) : sizeof(unsigned short);

  glBindBuffer(GL_ARRAY_BUFFER, ourVertexes->m_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourIndexes->m_buffer);
  useVertexBuffer(ourVertexes);

  int startOffset = startIndex*indexSize;
  glDrawElements(mgGL21primType(primType), endIndex-startIndex, indexType, (const GLvoid*) startOffset);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mgNullHandle);
  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
}

//--------------------------------------------------------------
// start drawing decal background.  draw=false just sets depth
void mgGL21Display::decalBackground(
  BOOL draw)
{
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
void mgGL21Display::decalStart()
{
  glDepthFunc(GL_LESS);
  glColorMask(true, true, true, true);

  // draw the decals masked by the stencil buffer
  glStencilFunc(GL_EQUAL, 1, 1);
  glDisable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
// end decals
void mgGL21Display::decalEnd()
{
  // restore old state
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
// set world transform
void mgGL21Display::setModelTransform(
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
void mgGL21Display::appendModelTransform(
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
void mgGL21Display::getModelTransform(
  mgMatrix4& xform)
{
  xform = m_modelMatrix;
}

//--------------------------------------------------------------
// get mvp transform
void mgGL21Display::getMVPTransform(
  mgMatrix4& xform)
{
  xform = m_worldMatrix;
  xform.multiply(m_worldProjection);
}

//--------------------------------------------------------------
// get mv transform
void mgGL21Display::getMVTransform(
  mgMatrix4& xform)
{
  xform = m_worldMatrix;
}

//--------------------------------------------------------------
// set light position
void mgGL21Display::setLightDir(
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
void mgGL21Display::setLightColor(
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
void mgGL21Display::setLightAmbient(
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
void mgGL21Display::setMatColor(
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
void mgGL21Display::setZEnable(
  BOOL enable)
{
  if (enable)
    glEnable(GL_DEPTH_TEST);
  else glDisable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
// set culling 
void mgGL21Display::setCulling(
  BOOL enable)
{
  if (enable)
    glEnable(GL_CULL_FACE);
  else glDisable(GL_CULL_FACE);
}

//--------------------------------------------------------------
// set front face clockwise
void mgGL21Display::setFrontCW(
  BOOL enable)
{
  if (enable)
    glFrontFace(GL_CW);
  else glFrontFace(GL_CCW);
}

//--------------------------------------------------------------
// set transparent
void mgGL21Display::setTransparent(
  BOOL enable)
{
  if (enable)
    glEnable(GL_BLEND);
  else glDisable(GL_BLEND);
}

//-----------------------------------------------------------------------------
// set cursor pattern
void mgGL21Display::setCursorTexture(
  const char* fileName,
  int hotX,
  int hotY)
{
  m_cursorFileName = fileName;
  m_cursorHotX = hotX;
  m_cursorHotY = hotY;

  m_cursorTexture = (mgGL21TextureImage*) loadTexture(fileName);
}

//--------------------------------------------------------------
// scale texture array down by factor of 2.
void mgGL21Display::scaleTextureImage(
  int width,
  int height,
  BYTE* data)
{
  // just sum up 2 by 2 pixels.  Not the best scaling... :-)
  int sourceLineSize = width*4;
  BYTE* source = data;
  BYTE* target = data;
  for (int r = 0; r < height; r+=2)
  {
    BYTE* nextLine = source + sourceLineSize;
    for (int c = 0; c < width; c+= 2)
    {
      int avgR = (source[0] + source[4] + nextLine[0] + nextLine[4])/4;
      int avgG = (source[1] + source[5] + nextLine[1] + nextLine[5])/4;
      int avgB = (source[2] + source[6] + nextLine[2] + nextLine[6])/4;
      int avgA = (source[3] + source[7] + nextLine[3] + nextLine[7])/4;

      // advance source by two pixels
      source += 2*4;
      nextLine += 2*4;

      // write target
      target[0] = (BYTE) avgR;
      target[1] = (BYTE) avgG;
      target[2] = (BYTE) avgB;
      target[3] = (BYTE) avgA;
      target += 4;  // one pixel
    }
    // we've advanced one line in loop, skip second line
    source += sourceLineSize;  
  }
}

//--------------------------------------------------------------
// load texture from image file
mgTextureImage* mgGL21Display::loadTexture(
  const char* fileName)
{
  const void *value;
  // if we've already created the texture
  if (m_textureImages.lookup(fileName, value))
    return (mgTextureImage*) value;

  mgGL21TextureImage* texture = new mgGL21TextureImage();
  texture->m_fileName = fileName;
  mgOSFixFileName(texture->m_fileName);

  reloadTextureImage(texture);

  // save the texture id (under original name.  copy in texture is fixed)
  m_textureImages.setAt(fileName, texture);

  return texture;
}

//--------------------------------------------------------------
// load texture array from file list
mgTextureArray* mgGL21Display::loadTextureArray(
  const mgStringArray& fileList)   
{
  mgGL21TextureArray* texture = new mgGL21TextureArray();

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
mgTextureCube* mgGL21Display::loadTextureCube(
  const char* xminImage,
  const char* xmaxImage,
  const char* yminImage,
  const char* ymaxImage,
  const char* zminImage,
  const char* zmaxImage)
{
  mgGL21TextureCube* texture = new mgGL21TextureCube();
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
mgTextureImage* mgGL21Display::createTextureMemory(
  int width,
  int height,
  int format,
  BOOL mipmap)
{
  GLuint handle;
  glGenTextures(1, &handle);

  glBindTexture(GL_TEXTURE_2D, handle);

  // to use texture as a render target, filters must be "GL_NEAREST"
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
       mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

// OSX appears not to define GL_GENERATE_MIPMAP parameter, but glGenerateMipmap routine instead.
#ifdef GL_GENERATE_MIPMAP
  // turn off mipmapping if not requested
  if (mipmap)
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#else
  if (!mipmap)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#endif

  GLint texFormat = mgGL21TextureFormat(format);
  glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0, texFormat, GL_UNSIGNED_BYTE, NULL);

  mgGL21TextureImage* texture = new mgGL21TextureImage();
  texture->m_width = width;
  texture->m_height = height;
  texture->m_format = format;
  texture->m_mipmap = mipmap;

  texture->m_handle = handle;

  return texture;
}

//--------------------------------------------------------------
// reload texture image from file
void mgGL21Display::reloadTextureImage(
  mgGL21TextureImage* texture)
{
  glGenTextures(1, &texture->m_handle);

  glBindTexture(GL_TEXTURE_2D, texture->m_handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
       texture->m_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR); // NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mgGL21TextureWrap(texture->m_xWrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mgGL21TextureWrap(texture->m_yWrap));

// OSX appears not to define GL_GENERATE_MIPMAP parameter, but glGenerateMipmap routine instead.
#ifdef GL_GENERATE_MIPMAP
  // turn off mipmapping if not requested
  if (texture->m_mipmap)
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#else
  if (!texture->m_mipmap)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#endif
    
  int imgWidth, imgHeight;
  BYTE* data;
  mgLoadRGBAImage(texture->m_fileName, imgWidth, imgHeight, texture->m_transparent, data);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, 
    GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

#ifndef GL_GENERATE_MIPMAP
  glGenerateMipmap(GL_TEXTURE_2D);
#endif

  texture->m_width = imgWidth;
  texture->m_height = imgHeight;
}

//--------------------------------------------------------------
// reload texture array from file list
void mgGL21Display::reloadTextureArray(
  mgGL21TextureArray* texture)
{
  // get the first texture file, for size
  mgString fileName = texture->m_fileList[0];
  int imgWidth, imgHeight;
  BOOL hasAlpha;
  BYTE* data;
  mgLoadRGBAImage(fileName, imgWidth, imgHeight, hasAlpha, data);
    
  int cellWidth = imgWidth;
  int cellHeight = imgHeight;

  // make the texture square-ish
  int colCount = (int) (0.5+sqrt((double) texture->m_fileList.length()));
  colCount = min(colCount, 2048/imgWidth);   // =-= query max texture width
  int width = colCount*imgWidth;

  // enlarge width to next power of two
  int atlasWidth = 1;
  while (atlasWidth < width)
    atlasWidth *= 2;
  colCount = atlasWidth / imgWidth;

  int rowCount = (texture->m_fileList.length()+colCount-1) / colCount;
  int height = rowCount*imgHeight;
  
  // enlarge height to next power of two
  int atlasHeight = 1;
  while (atlasHeight < height)
    atlasHeight *= 2;
  
  // allocate data for the complete texture atlas
  int atlasLineSize = 4*atlasWidth;
  BYTE* atlas = new BYTE[atlasLineSize * atlasHeight];
  memset(atlas, 0, atlasLineSize * atlasHeight);

  // load the images in the file list into the array
  int col = 0;
  int row = 0;
  for (int i = 0; i < texture->m_fileList.length(); i++)
  {
    mgString fileName = (mgString) texture->m_fileList[i];

    // first texture's data already loaded, so skip i == 0
    if (i != 0)
      mgLoadRGBAImage(fileName, imgWidth, imgHeight, hasAlpha, data);

    if (imgWidth != cellWidth || imgHeight != cellHeight)
      throw new mgErrorMsg("glTextureArray", "fileName,wd,ht,arraywd,arrayht", 
        (const char*) fileName, imgWidth, imgHeight, cellWidth, cellHeight);

    texture->m_imgTransparent[i] = hasAlpha;

    // copy the data into the altas
    BYTE* source = data;
    BYTE* target = atlas + atlasLineSize * row + 4*col;
    int imgLineSize = cellWidth * 4;

    int tgtOffset = 0;
    for (int r = 0; r < cellHeight; r++)
    {
      // copy image
      memcpy(target + tgtOffset, source, imgLineSize);

      // advance target to next scanline
      tgtOffset += atlasLineSize;

      // source to next scanline
      source += imgLineSize;
    }

    delete data;

    col += cellWidth;
    if (col >= atlasWidth)
    {
      col = 0;
      row += cellHeight;
    }
  }

  texture->m_width = cellWidth;
  texture->m_height = cellHeight;

  texture->m_atlasWidth = atlasWidth;
  texture->m_atlasHeight = atlasHeight;

  // create the texture
  glGenTextures(1, &texture->m_handle);

  glBindTexture(GL_TEXTURE_2D, texture->m_handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mgGL21TextureWrap(texture->m_xWrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mgGL21TextureWrap(texture->m_yWrap));

  // construct mipmap levels
  int mipSize = min(cellWidth, cellHeight);
  int mipWidth = atlasWidth;
  int mipHeight = atlasHeight;
  int level = 0;
  while (mipSize > 1)
  {
    glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, 
      mipWidth, mipHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas);

    scaleTextureImage(mipWidth, mipHeight, atlas);
    mipWidth /= 2;
    mipHeight /= 2;
    mipSize /= 2;
    level++;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level-1);
  delete atlas;
}

//--------------------------------------------------------------
// reload texture cube from file list
void mgGL21Display::reloadTextureCube(
  mgGL21TextureCube* texture)
{
  glGenTextures(1, &texture->m_handle);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture->m_handle);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, mgGL21TextureWrap(texture->m_xWrap));
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, mgGL21TextureWrap(texture->m_yWrap));

// OSX appears not to define GL_GENERATE_MIPMAP parameter, but glGenerateMipmap routine instead.
#ifdef GL_GENERATE_MIPMAP
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
#endif
    
  // load the images in the file list into the array
  int arrayWidth = 0;
  int arrayHeight = 0;

  // load XMIN image
  int imgWidth, imgHeight;
  BOOL hasAlpha;
  BYTE* data;
  mgLoadRGBAImage(texture->m_xminImage, imgWidth, imgHeight, hasAlpha, data);

  arrayWidth = imgWidth;            
  arrayHeight = imgHeight;

  texture->m_imgTransparent[0] = hasAlpha;

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

// OSX appears not to define GL_GENERATE_MIPMAP parameter, but glGenerateMipmap routine instead.
#ifndef GL_GENERATE_MIPMAP
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
#endif

  texture->m_width = arrayWidth;
  texture->m_height = arrayHeight;
}

//--------------------------------------------------------------
// release textures from display memory
void mgGL21Display::unloadTextures()
{
  // release texture data
  const void* value;
  mgString key;
  int posn = m_textureImages.getStartPosition();
  while (posn != -1)
  {
    m_textureImages.getNextAssoc(posn, key, value);
    mgGL21TextureImage* texture = (mgGL21TextureImage*) value;
    if (texture->m_handle != mgNullHandle)
    {
      glDeleteTextures(1, &texture->m_handle);
      texture->m_handle = mgNullHandle;
    }
  }

  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgGL21TextureArray* texture = (mgGL21TextureArray*) m_textureArrays[i];
    glDeleteTextures(1, &texture->m_handle);
    texture->m_handle = mgNullHandle;
  }

  for (int i = 0; i < m_textureCubes.length(); i++)
  {
    mgGL21TextureCube* texture = (mgGL21TextureCube*) m_textureCubes[i];
    glDeleteTextures(1, &texture->m_handle);
    texture->m_handle = mgNullHandle;
  }
}

//--------------------------------------------------------------
// reload textures into display memory
void mgGL21Display::reloadTextures()
{
  // restore textures from source file
  const void* value;
  mgString key;
  int posn = m_textureImages.getStartPosition();
  while (posn != -1)
  {
    m_textureImages.getNextAssoc(posn, key, value);
    mgGL21TextureImage* texture = (mgGL21TextureImage*) value;

    reloadTextureImage(texture);    
  }

  // reload texture arrays
  for (int i = 0; i < m_textureArrays.length(); i++)
  {
    mgGL21TextureArray* texture = (mgGL21TextureArray*) m_textureArrays[i];
    reloadTextureArray(texture);
  }

  // reload texture cubes
  for (int i = 0; i < m_textureCubes.length(); i++)
  {
    mgGL21TextureCube* texture = (mgGL21TextureCube*) m_textureCubes[i];
    reloadTextureCube(texture);
  }
}

//--------------------------------------------------------------------
// initialize overlay shaders
void mgGL21Display::loadOverlayShaders()
{
  mgString vs;
  vs += "#version 120\n";
  vs += "uniform vec2 mgSize;\n";
  vs += "uniform vec2 mgOrigin;\n";
  vs += "attribute vec2 vertPoint;\n";
  vs += "attribute vec4 vertColor;\n";
  vs += "varying vec4 vColor;\n";
  vs += "void main(void) \n";
  vs += "{ \n";
  vs += "  vColor = vertColor;\n";
  vs += "  gl_Position.x = (2.0 * (vertPoint.x+mgOrigin.x)) / mgSize.x - 1.0;\n";
  vs += "  gl_Position.y = (2.0 * (vertPoint.y+mgOrigin.y)) / mgSize.y - 1.0;\n";
  vs += "  gl_Position.z = 0.5;\n";
  vs += "  gl_Position.w = 1.0;\n";
  vs += "}\n";

  mgString fs;
  fs += "#version 120\n";
  fs += "varying vec4 vColor;\n";
  fs += "void main(void) \n";
  fs += "{ \n";
  fs += "  gl_FragColor = vColor;\n";
  fs += "}\n";

  m_overlaySolidShader = loadShaderSource("mgOverlaySolid", vs, fs, MGVERTEX_OVERLAYSOLID_ATTRIBS);

  vs.empty();
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
  vs += "  gl_Position.y = (2.0 * (vertPoint.y+mgOrigin.y)) / mgSize.y - 1.0;\n";
  vs += "  gl_Position.z = 0.5;\n";
  vs += "  gl_Position.w = 1.0;\n";
  vs += "}\n";

  fs.empty();  
  fs += "#version 120\n";
  fs += "uniform sampler2D mgTextureUnit0;\n";
  fs += "varying vec2 vTex;\n";
  fs += "void main(void) \n";
  fs += "{ \n";
  fs += "  gl_FragColor = texture2D(mgTextureUnit0, vTex);\n";
  fs += "}\n";

  m_overlayImageShader = loadShaderSource("mgOverlayImage", vs, fs, MGVERTEX_OVERLAYIMAGE_ATTRIBS);

  vs.empty();
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
  vs += "  gl_Position.y = (2.0 * (vertPoint.y+mgOrigin.y)) / mgSize.y - 1.0;\n";
  vs += "  gl_Position.z = 0.5;\n";
  vs += "  gl_Position.w = 1.0;\n";
  vs += "}\n";

  fs.empty();  
  fs += "#version 120\n";
  fs += "uniform sampler2D mgTextureUnit0;\n";
  fs += "uniform vec4 textColor;\n";
  fs += "varying vec2 vTex;\n";
  fs += "void main(void) \n";
  fs += "{ \n";
  // font texture is a grayscale, use as alpha component
  fs += "  vec4 pixel = texture2D(mgTextureUnit0, vTex);\n";
  fs += "  if (pixel.r == 0.0)\n";
  fs += "    discard;\n";
  fs += "  gl_FragColor = vec4(textColor.r, textColor.g, textColor.b, textColor.a * pixel.r);\n";
  fs += "}\n";

  m_overlayTextShader = loadShaderSource("mgOverlayText", vs, fs, MGVERTEX_OVERLAYTEXT_ATTRIBS);

  vs.empty();
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
  vs += "  gl_Position.y = (2.0 * (vertPoint.y+mgOrigin.y)) / mgSize.y - 1.0;\n";
  vs += "  gl_Position.z = 0.5;\n";
  vs += "  gl_Position.w = 1.0;\n";
  vs += "}\n";

  fs.empty();  
  fs += "#version 120\n";
  fs += "uniform sampler2D mgTextureUnit0;\n";
  fs += "uniform vec4 textColor;\n";
  fs += "varying vec2 vTex;\n";
  fs += "void main(void) \n";
  fs += "{ \n";
  // font texture is a grayscale, use as alpha component
  fs += "  vec4 pixel = texture2D(mgTextureUnit0, vTex);\n";
  fs += "  if (pixel.r == 0.0)\n";
  fs += "    discard;\n";
  fs += "  gl_FragColor = vec4(textColor.r, textColor.g, textColor.b, pixel.r);\n";
  fs += "}\n";

  m_overlayTextM1Shader = loadShaderSource("mgOverlayTextM1", vs, fs, MGVERTEX_OVERLAYTEXT_ATTRIBS);

  vs.empty();
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
  vs += "  gl_Position.y = (2.0 * (vertPoint.y+mgOrigin.y)) / mgSize.y - 1.0;\n";
  vs += "  gl_Position.z = 0.5;\n";
  vs += "  gl_Position.w = 1.0;\n";
  vs += "}\n";

  fs.empty();  
  fs += "#version 120\n";
  fs += "uniform sampler2D mgTextureUnit0;\n";
  fs += "uniform vec4 textColor;\n";
  fs += "varying vec2 vTex;\n";
  fs += "void main(void) \n";
  fs += "{ \n";
  // font texture is a grayscale, use as alpha component
  fs += "  vec4 pixel = texture2D(mgTextureUnit0, vTex);\n";
  fs += "  if (pixel.r == 0.0)\n";
  fs += "    discard;\n";
  // second pass.  dst a already set to dst.a*(1-pixel.r))
  // now add in color.a * pixel.r
  fs += "  gl_FragColor = vec4(0.0, 0.0, 0.0, textColor.a * pixel.r);\n";
  fs += "}\n";

  m_overlayTextM2Shader = loadShaderSource("mgOverlayTextM2", vs, fs, MGVERTEX_OVERLAYTEXT_ATTRIBS);
}

//--------------------------------------------------------------------
// return overlay shaders
void mgGL21Display::getOverlayShaders(
  mgShader*& solidShader, 
  mgShader*& imageShader, 
  mgShader*& textShader, 
  mgShader*& textM1Shader, 
  mgShader*& textM2Shader)
{
  solidShader = m_overlaySolidShader;
  imageShader = m_overlayImageShader;
  textShader = m_overlayTextShader;
  textM1Shader = m_overlayTextM1Shader;
  textM2Shader = m_overlayTextM2Shader;
}

#endif
