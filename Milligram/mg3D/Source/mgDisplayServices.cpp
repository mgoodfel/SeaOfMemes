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

#include "mgDisplayServices.h"

#include "Graphics3D/GL33/mgGL33Services.h"
#include "Graphics3D/GL21/mgGL21Services.h"
#include "Util3D/mgFontList.h"
#include "Util3D/mg3DErrorTable.h"

mgDisplayServices* mgDisplay = NULL;

//--------------------------------------------------------------
// constructor
mgTextureImage::mgTextureImage()
{
  m_width = 0;
  m_height = 0;
  m_transparent = false;
  m_xWrap = MG_TEXTURE_REPEAT;
  m_yWrap = MG_TEXTURE_REPEAT;
  m_filter = MG_TEXTURE_QUALITY;
}

//--------------------------------------------------------------
// destructor
mgTextureImage::~mgTextureImage()
{
}

//--------------------------------------------------------------
// constructor
mgTextureArray::mgTextureArray()
{
  m_imgTransparent = NULL;
  m_width = 0;
  m_height = 0;
  m_xWrap = MG_TEXTURE_REPEAT;
  m_yWrap = MG_TEXTURE_REPEAT;
  m_filter = MG_TEXTURE_QUALITY;
}

//--------------------------------------------------------------
// destructor
mgTextureArray::~mgTextureArray()
{
  delete m_imgTransparent;
  m_imgTransparent = NULL;
}

//--------------------------------------------------------------
// constructor
mgTextureCube::mgTextureCube()
{
  for (int i = 0; i < 6; i++)
    m_imgTransparent[i] = false;
  m_width = 0;
  m_height = 0;
  m_xWrap = MG_TEXTURE_CLAMP;
  m_yWrap = MG_TEXTURE_CLAMP;
  m_filter = MG_TEXTURE_QUALITY;
}

//--------------------------------------------------------------
// destructor
mgTextureCube::~mgTextureCube()
{
}

//--------------------------------------------------------------
// constructor
mgDisplayServices::mgDisplayServices(
  const char* shaderDir,
  const char* fontDir)
{
  m_shaderDir = shaderDir;
  if (!m_shaderDir.endsWith("/") && !m_shaderDir.endsWith("\\"))
    m_shaderDir += "/";

  mgString appFontDir(fontDir);
  if (!appFontDir.endsWith("/") && !appFontDir.endsWith("\\"))
    appFontDir += "/";

  // set up for fonts
  mgPlatform->getFontDirectories(m_fontDirs);
  m_fontDirs.add(appFontDir);

  mgString fileName;
  fileName.format("%sfonts.xml", (const char *) appFontDir);
  m_fontList = new mgFontList(fileName);

  mgPlatform->setErrorTable(new mg3DErrorTable());

  m_matColor = mgPoint4(1, 1, 1, 1);
  m_lightDir = mgPoint3(0.6, 0.8, -0.3); // mgPoint3(-0.6, 1, -0.3); //  mgPoint3(0, 1, 0); // 
  m_lightDir.normalize();
  m_lightColor = mgPoint3(1.0, 1.0, 1.0);
  m_lightAmbient = mgPoint3(0.4, 0.4, 0.4);  // mgPoint3(0.1, 0.1, 0.1);  
  m_updateShaderVars = true;

  // front plane, backplane
  m_viewFront = 1/4.0;
  m_viewBack = 16384.0;
  m_FOV = 45.0; // degrees

  m_cursorX = 0;
  m_cursorY = 0;
  m_cursorHotX = 0;
  m_cursorHotY = 0;
  m_cursorEnabled = false;
  m_cursorTrack = true;

  m_graphicsWidth = 0;
  m_graphicsHeight = 0;
}

//--------------------------------------------------------------
// destructor
mgDisplayServices::~mgDisplayServices()
{
  delete m_fontList;
  m_fontList = NULL;
}

//--------------------------------------------------------------
// set the horizontal field of view
void mgDisplayServices::setFOV(
  double angle)
{
  m_FOV = angle;
  setProjection();
  frustumBuildPlanes();
  m_updateShaderVars = true;
}

//-----------------------------------------------------------------------------
// set the dots per inch of the display
// controls font scaling
void mgDisplayServices::setDPI(
  int dpi)
{
  // ignore 0 dpi from options.  it's there as a default so users
  // know which attribute to set.
  if (dpi > 0)
    m_dpi = dpi;
}
  
//--------------------------------------------------------------
// handle window resize
void mgDisplayServices::setScreenSize(
  int width,
  int height)
{
  // if window is still 0 by 0, it's too early.  wait for real resize
  if (width <= 0 || height <= 0)
    return;

  m_graphicsWidth = width;
  m_graphicsHeight = height;

  mgDebug("view size is %d by %d", m_graphicsWidth, m_graphicsHeight);

  setProjection();
}

//--------------------------------------------------------------
// set the front and back plane distances
void mgDisplayServices::setFrontAndBack(
  double frontDist,
  double backDist)
{
  m_viewFront = frontDist;
  m_viewBack = backDist;
  setProjection();
  frustumBuildPlanes();
  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// set the eye point
void mgDisplayServices::setEyePt(
  const mgPoint3& pt)
{
  m_eyePt = pt;
  frustumBuildPlanes();
  m_updateShaderVars = true;
}

//--------------------------------------------------------------
// set the eye rotation matrix
void mgDisplayServices::setEyeMatrix(
  const mgMatrix4& matrix)
{
  m_eyeMatrix = matrix;
  frustumBuildPlanes();
  m_updateShaderVars = true;
}

//-----------------------------------------------------------------------------
// enable the cursor
void mgDisplayServices::cursorEnable(
  BOOL enabled)
{
  m_cursorEnabled = enabled;
}

//-----------------------------------------------------------------------------
// track the cursor position
void mgDisplayServices::cursorTrack(
  BOOL track)
{
  if (m_cursorTrack == track)
    return;

  // set cursor tracking
  m_cursorTrack = track;
  mgPlatform->setMouseRelative(!m_cursorTrack);
}

//-----------------------------------------------------------------------------
// set cursor position
void mgDisplayServices::cursorSetPosn(
  int x, 
  int y)
{
  m_cursorX = x;
  m_cursorY = y;
}

//-----------------------------------------------------------------------------
// move the cursor based on mouse dx,dy
void mgDisplayServices::cursorMove(
  int dx,
  int dy)
{
  m_cursorX += dx;
  m_cursorY += dy;

  m_cursorX = max(0, m_cursorX);
  m_cursorX = min(m_graphicsWidth, m_cursorX);

  m_cursorY = max(0, m_cursorY);
  m_cursorY = min(m_graphicsHeight, m_cursorY);
}

//-----------------------------------------------------------------------------
// build the view frustum planes
void mgDisplayServices::frustumBuildPlanes()
{
  mgPoint3 up;
  mgPoint3 right;
  mgPoint3 eye;

  m_eyeMatrix.invertPt(mgPoint3(0.0, 0.0, 1.0), eye);
  m_eyeMatrix.invertPt(mgPoint3(0.0, 1.0, 0.0), up);
  m_eyeMatrix.invertPt(mgPoint3(1.0, 0.0, 0.0), right);

  // front plane
  m_viewFrontPt = eye;
  m_viewFrontPt.scale(m_viewFront);
  m_viewFrontPt.add(m_eyePt);
  m_viewFrontNormal = eye;

  // back plane
  m_viewBackPt = m_viewFrontNormal;
  m_viewBackPt.scale(m_viewBack);
  m_viewBackPt.add(m_eyePt);
  m_viewBackNormal = eye;
  m_viewBackNormal.scale(-1.0);

  // get front plane corner vectors
  mgPoint3 ptY(up);
  ptY.scale(m_viewHeight/2);
  mgPoint3 ptX(right);
  ptX.scale(m_viewWidth/2);
  mgPoint3 ptZ(eye);
  ptZ.scale(m_viewFront);

  mgPoint3 ptTL(ptZ);
  ptTL.add(ptY);
  ptTL.subtract(ptX);
  ptTL.normalize();

  mgPoint3 ptTR(ptZ);
  ptTR.add(ptY);
  ptTR.add(ptX);
  ptTR.normalize();

  mgPoint3 ptBL(ptZ);
  ptBL.subtract(ptY);
  ptBL.subtract(ptX);
  ptBL.normalize();

  mgPoint3 ptBR(ptZ);
  ptBR.subtract(ptY);
  ptBR.add(ptX);
  ptBR.normalize();

  // left plane
  m_viewLeftPt = m_eyePt;  // runs through eye
  m_viewLeftNormal = ptTL;
  m_viewLeftNormal.cross(ptBL);

  // right plane
  m_viewRightPt = m_eyePt;  // runs through eye
  m_viewRightNormal = ptBR;
  m_viewRightNormal.cross(ptTR);

  // top plane
  m_viewTopPt = m_eyePt;  // runs through eye
  m_viewTopNormal = ptTR;
  m_viewTopNormal.cross(ptTL);

  // bottom plane
  m_viewBottomPt = m_eyePt;  // runs through eye
  m_viewBottomNormal = ptBL;
  m_viewBottomNormal.cross(ptBR);
}

//-----------------------------------------------------------------------------
// return true if sphere within frustum
BOOL mgDisplayServices::withinFrustum(
  double x, 
  double y, 
  double z,
  double radius)
{
  // get height of sphere center above each frustum plane
  mgPoint3 center(x, y, z);
  mgPoint3 pt;
  double ht;

  pt = center;
  pt.subtract(m_viewLeftPt);
  ht = pt.dot(m_viewLeftNormal);
  if (ht < -radius)
    return false;

  pt = center;
  pt.subtract(m_viewRightPt);
  ht = pt.dot(m_viewRightNormal);
  if (ht < -radius)
    return false;

  pt = center;
  pt.subtract(m_viewTopPt);
  ht = pt.dot(m_viewTopNormal);
  if (ht < -radius)
    return false;

  pt = center;
  pt.subtract(m_viewBottomPt);
  ht = pt.dot(m_viewBottomNormal);
  if (ht < -radius)
    return false;

  pt = center;
  pt.subtract(m_viewFrontPt);
  ht = pt.dot(m_viewFrontNormal);
  if (ht < -radius)
    return false;

  pt = center;
  pt.subtract(m_viewBackPt);
  ht = pt.dot(m_viewBackNormal);
  if (ht < -radius)
    return false;

  return true;
}

//-----------------------------------------------------------------------------
// figure vector corresponding to eye rotations
void mgDisplayServices::eyeVector(
  mgPoint3& vect)
{
  m_eyeMatrix.invertPt(0.0, 0.0, 1.0, vect.x, vect.y, vect.z);
}

//-----------------------------------------------------------------------------
// compute vector from eye to cursor
void mgDisplayServices::cursorVector(
  mgPoint3 &vect)
{
  // figure vector from eye to cursor
  mgPoint3 xAxis, yAxis, zAxis;
  m_eyeMatrix.invertPt(1.0, 0.0, 0.0, xAxis.x, xAxis.y, xAxis.z);
  m_eyeMatrix.invertPt(0.0, 1.0, 0.0, yAxis.x, yAxis.y, yAxis.z);
  m_eyeMatrix.invertPt(0.0, 0.0, 1.0, zAxis.x, zAxis.y, zAxis.z);

  xAxis.scale((m_viewWidth * (m_cursorX - m_graphicsWidth/2))/m_graphicsWidth);
  yAxis.scale((m_viewHeight * (m_graphicsHeight/2 - m_cursorY))/m_graphicsHeight);
  zAxis.scale(m_viewFront);

  vect = zAxis;
  vect.add(xAxis);
  vect.add(yAxis);
  vect.normalize();
}

//-----------------------------------------------------------------------------
// find a font file
BOOL mgDisplayServices::findFont(
  const char* faceName, 
  BOOL bold, 
  BOOL italic, 
  mgString& fontFile)
{
  return m_fontList->findFont(m_fontDirs, faceName, bold, italic, fontFile);
}

//-----------------------------------------------------------------------------
// create an instance of DisplayServices for the display platform in use
void mgInitDisplayServices(
  const char* shaderDir,
  const char* fontDir)
{
  mgString library;
  mgPlatform->getDisplayLibrary(library);

#ifdef SUPPORT_GL33
  if (mgDisplay == NULL && library.equalsIgnoreCase("OpenGL3.3"))
  {
    mgDisplay = new mgGL33Services(shaderDir, fontDir);
  }
#endif

#ifdef SUPPORT_GL21
  if (mgDisplay == NULL && library.equalsIgnoreCase("OpenGL2.1"))
  {
    mgDisplay = new mgGL21Services(shaderDir, fontDir);
  }
#endif

  if (mgDisplay != NULL)
    mgDisplay->initView();
}

//-----------------------------------------------------------------------------
// clean up display services
void mgTermDisplayServices()
{
  delete mgDisplay;
  mgDisplay = NULL;
}
