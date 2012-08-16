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

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgUtil/Include/ImageUtil/mgImageUtil.h"
#include "Planet.h"

const double MOVE_SPEED = 250/1000.0; 

// vertex 
class VertexPlanet
{
public:
  // world coordinates of vertex
  float m_px;
  float m_py;
  float m_pz;

  // model coordinates of vertex
  float m_mx;
  float m_my;
  float m_mz;

  // set the world coordinates
  void setPoint(
    double x,
    double y,
    double z)
  {
    m_px = (float) x;
    m_py = (float) y;
    m_pz = (float) z;
  }

  // set the model coordinates
  void setModelPoint(
    double x,
    double y,
    double z)
  {
    m_mx = (float) x;
    m_my = (float) y;
    m_mz = (float) z;
  }
};

//--------------------------------------------------------------
// constructor
Planet::Planet(
  const mgOptionsFile& options)
{
  // load the planet cubemap texture
  mgString xminName, xmaxName, yminName, ymaxName, zminName, zmaxName;
  options.getFileName("planet-xmin", options.m_sourceFileName, "", xminName);
  options.getFileName("planet-xmax", options.m_sourceFileName, "", xmaxName);
  options.getFileName("planet-ymin", options.m_sourceFileName, "", yminName);
  options.getFileName("planet-ymax", options.m_sourceFileName, "", ymaxName);
  options.getFileName("planet-zmin", options.m_sourceFileName, "", zminName);
  options.getFileName("planet-zmax", options.m_sourceFileName, "", zmaxName);

  m_surfaceTexture = loadCubeMap(xminName, xmaxName, yminName, ymaxName, zminName, zmaxName);

  // load the clouds cubemap texture
  options.getFileName("clouds-xmin", options.m_sourceFileName, "", xminName);
  options.getFileName("clouds-xmax", options.m_sourceFileName, "", xmaxName);
  options.getFileName("clouds-ymin", options.m_sourceFileName, "", yminName);
  options.getFileName("clouds-ymax", options.m_sourceFileName, "", ymaxName);
  options.getFileName("clouds-zmin", options.m_sourceFileName, "", zminName);
  options.getFileName("clouds-zmax", options.m_sourceFileName, "", zmaxName);

  m_cloudsTexture = loadCubeMap(xminName, xmaxName, yminName, ymaxName, zminName, zmaxName);

  compileShader(options);

  // create the vertex buffer
  glGenBuffers(1, &m_vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

  m_eyePt = mgPoint3(3500, 0, 0);
  m_eyeRotX = 0.0;
  m_eyeRotY = 90.0;
  m_eyeMatrix.rotateYDeg(m_eyeRotY);
  m_eyeMatrix.rotateXDeg(m_eyeRotX);

  m_specularColor = mgPoint3(0.4, 0.4, 0.4);
  m_lightColor = mgPoint3(0.6, 0.6, 0.6);
  m_lightAmbient = mgPoint3(0.2, 0.2, 0.2);
  m_matColor = mgPoint4(1, 1, 1, 1);
}

//--------------------------------------------------------------
// destructor
Planet::~Planet()
{
  if (m_surfaceTexture != 0)
    glDeleteTextures(1, &m_surfaceTexture);
  if (m_cloudsTexture != 0)
    glDeleteTextures(1, &m_cloudsTexture);

  if (m_vertexBuffer != 0)
    glDeleteBuffers(1, &m_vertexBuffer);
}

//-----------------------------------------------------------------------------
// compile the shader
void Planet::compileShader(
  const mgOptionsFile& options)
{
  // compile the shader
  mgDebug("compile planet shader:");
  mgString fileName;
  options.getFileName("planetVertexShader", options.m_sourceFileName, "", fileName);
  const char* vertexSource = loadFile(fileName);

  options.getFileName("planetFragmentShader", options.m_sourceFileName, "", fileName);
  const char* fragmentSource = loadFile(fileName);
  int attribCount = 2;
  const char* attribNames[] = {"vertPoint", "modelPoint"};
  const DWORD attribIndexes[] = {0, 1};
  m_planetShader = mgPlatform->compileGLShaderPair(vertexSource, fragmentSource, attribCount, attribNames, attribIndexes);

  delete vertexSource;
  delete fragmentSource;
}

//-----------------------------------------------------------------------------
// turn eye from mouse movement
void Planet::turnEye(
  int dx, 
  int dy)
{
  m_eyeRotX -= dy/25.0;
  m_eyeRotY -= dx/25.0;

  // don't allow head to flip over
  m_eyeRotX = min(m_eyeRotX, 90.0);
  m_eyeRotX = max(m_eyeRotX, -90.0);

  // keep direction within range
  if (m_eyeRotY < -180.0)
    m_eyeRotY += 360.0;
  if (m_eyeRotY > 180.0)
    m_eyeRotY -= 360.0;

  m_eyeMatrix.loadIdentity();
  m_eyeMatrix.rotateYDeg(m_eyeRotY);
  m_eyeMatrix.rotateXDeg(m_eyeRotX);
}

//--------------------------------------------------------------
// move forwards
void Planet::moveForwards(
  double ms)
{
  double headX, headY, headZ;
  m_eyeMatrix.invertPt(0.0, 0.0, 1.0, headX, headY, headZ);

  double dist = MOVE_SPEED*ms;
  m_eyePt.x += dist * headX;
  m_eyePt.y += dist * headY;
  m_eyePt.z += dist * headZ;
}

//--------------------------------------------------------------
// move backwards
void Planet::moveBackwards(
  double ms)
{
  double headX, headY, headZ;
  m_eyeMatrix.invertPt(0.0, 0.0, 1.0, headX, headY, headZ);

  double dist = MOVE_SPEED*ms;
  m_eyePt.x -= dist * headX;
  m_eyePt.y -= dist * headY;
  m_eyePt.z -= dist * headZ;
}

//--------------------------------------------------------------
// move right
void Planet::moveRightwards(
  double ms)
{
  double rightX, rightY, rightZ;
  m_eyeMatrix.invertPt(1.0, 0.0, 0.0, rightX, rightY, rightZ);

  double dist = MOVE_SPEED * ms;
  m_eyePt.x += dist * rightX;
  m_eyePt.y += dist * rightY;
  m_eyePt.z += dist * rightZ;
}

//--------------------------------------------------------------
// move left
void Planet::moveLeftwards(
  double ms)
{
  double rightX, rightY, rightZ;
  m_eyeMatrix.invertPt(1.0, 0.0, 0.0, rightX, rightY, rightZ);

  double dist = MOVE_SPEED * ms;
  m_eyePt.x -= dist * rightX;
  m_eyePt.y -= dist * rightY;
  m_eyePt.z -= dist * rightZ;
}

//--------------------------------------------------------------
// load six image files for cubemap texture
GLuint Planet::loadCubeMap(
  const char* xminFile,
  const char* xmaxFile,
  const char* yminFile,
  const char* ymaxFile,
  const char* zminFile,
  const char* zmaxFile)
{
  GLuint handle;
  glGenTextures(1, &handle);

  glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
// OSX defines routine glGenerateMipmap, not the parameter  
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
  mgLoadRGBAImage(xminFile, imgWidth, imgHeight, hasAlpha, data);

  arrayWidth = imgWidth;            
  arrayHeight = imgHeight;

  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load XMAX image 
  mgLoadRGBAImage(xmaxFile, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgException("file %s size is (%d by %d), expected (%d by %d)", 
      xmaxFile, imgWidth, imgHeight, arrayWidth, arrayHeight);

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load YMIN image 
  mgLoadRGBAImage(yminFile, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgException("file %s size is (%d by %d), expected (%d by %d)", 
      (const char*) yminFile, imgWidth, imgHeight, arrayWidth, arrayHeight);

  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load YMAX image 
  mgLoadRGBAImage(ymaxFile, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgException("file %s size is (%d by %d), expected (%d by %d)", 
      (const char*) ymaxFile, imgWidth, imgHeight, arrayWidth, arrayHeight);

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load ZMIN image 
  mgLoadRGBAImage(zminFile, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgException("file %s size is (%d by %d), expected (%d by %d)", 
      (const char*) zminFile, imgWidth, imgHeight, arrayWidth, arrayHeight);

  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

  // load ZMAX image 
  mgLoadRGBAImage(zmaxFile, imgWidth, imgHeight, hasAlpha, data);

  if (imgWidth != arrayWidth || imgHeight != arrayHeight)
    throw new mgException("file %s size is (%d by %d), expected (%d by %d)", 
      (const char*) zmaxFile, imgWidth, imgHeight, arrayWidth, arrayHeight);

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, 
    arrayWidth, arrayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  delete data;

#ifndef GL_GENERATE_MIPMAP
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
#endif
  
  return handle;
}

//--------------------------------------------------------------
// read a file into memory
const char* Planet::loadFile(
  const char* fileName)
{
  FILE* sourceFile = fopen(fileName, "rb");
  if (sourceFile == NULL)
    return NULL;

  fseek(sourceFile, 0, SEEK_END);
  int fileSize = (int) ftell(sourceFile);
  fseek(sourceFile, 0, SEEK_SET);

  // read shader source
  char* buffer = new char[fileSize+1];
  size_t readLen = fread(buffer, 1, fileSize, sourceFile);
  buffer[readLen] = '\0';
  fclose(sourceFile);

  return buffer;
}

//-----------------------------------------------------------------------------
// return matrix as float[16] array
void matrix4toGL(
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

//--------------------------------------------------------------
// render the planet
void Planet::render(
  int graphicsWidth,
  int graphicsHeight,
  double angle)
{
  // if shader didn't compile, nothing we can do
  if (m_planetShader == 0)
    return;

  double atmos = 20.0;
  double radius = 1200.0;

  double eyeDist = m_eyePt.length()/radius;
  double a = 1.0;
  if (eyeDist < a)
    return;  // below surface, nothing to do 
  double b = sqrt(eyeDist*eyeDist - a*a);
  double h = (a*b)/eyeDist;
  double m = (a*a)/eyeDist;

  h += atmos/radius;

  // x axis from planet center towards eye
  mgPoint3 xaxis(m_eyePt);
  xaxis.normalize();

  // build y axis
  mgPoint3 yaxis(xaxis);
  yaxis.cross(mgPoint3(0.0, 1.0, 0.0));
  yaxis.normalize();

  mgPoint3 zaxis(yaxis);
  zaxis.cross(xaxis);
  zaxis.normalize();

  mgMatrix4 transform;
  transform._11 = xaxis.x;
  transform._12 = xaxis.y;
  transform._13 = xaxis.z;
  transform._21 = yaxis.x;
  transform._22 = yaxis.y;
  transform._23 = yaxis.z;
  transform._31 = zaxis.x;
  transform._32 = zaxis.y;
  transform._33 = zaxis.z;
  
  VertexPlanet tl, tr, bl, br;

  mgPoint3 pt;
  transform.mapPt(m, -h, h, pt.x, pt.y, pt.z);
  tl.setPoint(radius*pt.x, radius*pt.y, radius*pt.z);

  transform.mapPt(m, h, h, pt.x, pt.y, pt.z);
  tr.setPoint(radius*pt.x, radius*pt.y, radius*pt.z);

  transform.mapPt(m, -h, -h, pt.x, pt.y, pt.z);
  bl.setPoint(radius*pt.x, radius*pt.y, radius*pt.z);

  transform.mapPt(m, h, -h, pt.x, pt.y, pt.z);
  br.setPoint(radius*pt.x, radius*pt.y, radius*pt.z);

  // inverse of world transform
  mgMatrix4 model;
  model.rotateYDeg(-angle);

  mgPoint3 lightDir(1.0, 0.25, 0.0);
  lightDir.normalize();

  mgPoint3 modelLightDir;
  model.mapPt(lightDir, modelLightDir);
  transform.multiply(model);

  mgPoint3 modelEye; 
  transform.mapPt(eyeDist, 0.0, 0.0, modelEye.x, modelEye.y, modelEye.z);

  transform.mapPt(m, -h, h, pt.x, pt.y, pt.z);
  tl.setModelPoint(pt.x, pt.y, pt.z);

  transform.mapPt(m, h, h, pt.x, pt.y, pt.z);
  tr.setModelPoint(pt.x, pt.y, pt.z);

  transform.mapPt(m, -h, -h, pt.x, pt.y, pt.z);
  bl.setModelPoint(pt.x, pt.y, pt.z);

  transform.mapPt(m, h, -h, pt.x, pt.y, pt.z);
  br.setModelPoint(pt.x, pt.y, pt.z);

  mgMatrix4 viewMatrix;
  viewMatrix.translate(-m_eyePt.x, -m_eyePt.y, -m_eyePt.z);
  viewMatrix.multiply(m_eyeMatrix);

  mgMatrix4 worldProjection;
  createProjection(worldProjection, graphicsWidth, graphicsHeight);

  mgMatrix4 mvpMatrix(viewMatrix);
  mvpMatrix.multiply(worldProjection);

  setupShader(mvpMatrix, modelEye, modelLightDir);

  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_surfaceTexture); 
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_cloudsTexture); 

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPlanet), (const GLvoid*) offsetof(VertexPlanet, m_px));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPlanet), (const GLvoid*) offsetof(VertexPlanet, m_mx));

  VertexPlanet data[6];
  data[0] = tl;
  data[1] = tr;
  data[2] = bl;
  data[3] = bl;
  data[4] = tr;
  data[5] = br;
  int vertexSize = sizeof(VertexPlanet);
  int count = 6;
  glBufferData(GL_ARRAY_BUFFER, vertexSize * count, data, GL_DYNAMIC_DRAW);

  glDrawArrays(GL_TRIANGLES, 0, count);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

//--------------------------------------------------------------
// create projection matrix
void Planet::createProjection(
  mgMatrix4& projection,
  int graphicsWidth,
  int graphicsHeight)
{
  // create world LH perspective transform  
  double yFOV = 45.0 * PI/180.0;
  GLfloat yScale = (GLfloat) (1.0/tan(yFOV/2.0));
  GLfloat xScale = (yScale*graphicsHeight)/graphicsWidth;
  double viewFront = 1/4.0;
  double viewBack = 16384.0;

  projection._11 = xScale;
  projection._22 = yScale;
  projection._33 = (viewBack + viewFront) / (viewBack - viewFront);
  projection._34 = 1.0;
  projection._43 = -2.0 * (viewFront*viewBack) / (viewBack - viewFront);
  projection._44 = 0.0;
}

//--------------------------------------------------------------
// set up shader
void Planet::setupShader(
  const mgMatrix4& mvpMatrix,
  const mgPoint3& modelEye,
  const mgPoint3& modelLightDir)
{
  glUseProgram(m_planetShader);
  GLint index;

  index = glGetUniformLocation(m_planetShader, "mgMVPMatrix");
  if (index != -1)
  {
    float matrix[16];
    matrix4toGL(mvpMatrix, matrix);
    glUniformMatrix4fv(index, 1, GL_FALSE, matrix);
  }

  index = glGetUniformLocation(m_planetShader, "mgLightColor");
  if (index != -1)
  {
    glUniform3f(index, (GLfloat) m_lightColor.x, (GLfloat) m_lightColor.y, (GLfloat) m_lightColor.z);
  }

  index = glGetUniformLocation(m_planetShader, "mgLightAmbient");
  if (index != -1)
  {
    glUniform3f(index, (GLfloat) m_lightAmbient.x, (GLfloat) m_lightAmbient.y, (GLfloat) m_lightAmbient.z);
  }

  index = glGetUniformLocation(m_planetShader, "mgMatColor");
  if (index != -1)
  {
    glUniform4f(index, (GLfloat) m_matColor.x, (GLfloat) m_matColor.y, (GLfloat) m_matColor.z, (GLfloat) m_matColor.w);
  }

  index = glGetUniformLocation(m_planetShader, "mgTextureUnit0");
  if (index != -1)
    glUniform1i(index, 0);

  index = glGetUniformLocation(m_planetShader, "mgTextureUnit1");
  if (index != -1)
    glUniform1i(index, 1);

  index = glGetUniformLocation(m_planetShader, "modelEye");
  if (index != -1)
    glUniform3f(index, (GLfloat) modelEye.x, (GLfloat) modelEye.y, (GLfloat) modelEye.z);

  index = glGetUniformLocation(m_planetShader, "modelLightDir");
  if (index != -1)
    glUniform3f(index, (GLfloat) modelLightDir.x, (GLfloat) modelLightDir.y, (GLfloat) modelLightDir.z);

  index = glGetUniformLocation(m_planetShader, "specularColor");
  if (index != -1)
    glUniform3f(index, (GLfloat) m_specularColor.x, (GLfloat) m_specularColor.y, (GLfloat) m_specularColor.z);
}
