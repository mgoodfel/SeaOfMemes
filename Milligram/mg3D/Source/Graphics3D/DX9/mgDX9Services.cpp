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

#ifdef SUPPORT_DX9
#include "mgDX9State.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "System/Windows/mgWinSystem.h"
#include "mgPlatform/Include/mgApplication.h"
#include "mgDX9Textures.h"
#include "mgDX9Triangles.h"
#include "mgDX9Indexes.h"
#include "mgDX9Vertexes.h"
#include "mgDX9VertexesTA.h"

#include "mgDX9Services.h"
#include "DX9DevMode.h"

#include "Util/ImageUtil/ImageUtil.h"

//--------------------------------------------------------------
// constructor
mgDX9Services::mgDX9Services()
{
  m_state = new mgDX9State();
  m_theApp = NULL;

  m_cursorTexture = NULL;
  m_graphicsWidth = 0;
  m_graphicsHeight = 0;

  m_lighting = false;

  m_textures = new mgDX9Textures();

  m_deviceActive = true;
  m_deviceResized = false;
}

//--------------------------------------------------------------
// constructor
mgDX9Services::~mgDX9Services()
{
  delete m_textures;
  m_textures = NULL;

  delete m_state;
  m_state = NULL;
}

//--------------------------------------------------------------
// create a 2D graphics surface
mgSurface* mgDX9Services::createOverlaySurface()
{
  return mgPlatform->createOverlaySurface();
}

//--------------------------------------------------------------
// create a 2D graphics surface
mgSurface* mgDX9Services::createTextureSurface()
{
  return mgPlatform->createTextureSurface();
}

//--------------------------------------------------------------
// can the display repeat textures
BOOL mgDX9Services::canRepeatTextures()
{
  return false;
}

//--------------------------------------------------------------
// end the app
void mgDX9Services::exitApp()
{
  mgPlatform->exitApp();
}

//--------------------------------------------------------------
// initialize for scene rendering
void mgDX9Services::initView()
{
  mgWinSystem* framework = (mgWinSystem*) mgPlatform;

  mgDebug("initView");
  // =-= turn off culling
//  mg_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  mg_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

//  mg_d3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
  mg_d3dDevice->SetRenderState(D3DRS_ZENABLE, true);

  mg_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  mg_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

  mg_d3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE); 

  mg_d3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

  mg_d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
  mg_d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  mg_d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  mg_d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE); 

  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 2); // m_state->m_deviceCaps.MaxAnisotropy);

  mg_d3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  mg_d3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
  mg_d3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
  mg_d3dDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 2); // m_state->m_deviceCaps.MaxAnisotropy);

  mgDebug("view size is %d by %d", m_graphicsWidth, m_graphicsHeight);

  setProjection();

  if (m_theApp != NULL)
    m_theApp->viewResized(m_graphicsWidth, m_graphicsHeight);
}
                
//--------------------------------------------------------------
// set projection
void mgDX9Services::setProjection()
{
  mgWinSystem* framework = (mgWinSystem*) mgPlatform;
 mgDebug("view size is %d by %d", m_graphicsWidth, m_graphicsHeight);

  // set the projection
  FLOAT yFOV = (FLOAT) (framework->m_FOV*D3DX_PI/180.0f);
  D3DXMatrixPerspectiveFovLH(&m_state->m_projMatrix, yFOV, m_graphicsWidth/(FLOAT) m_graphicsHeight, 
    (FLOAT) m_viewFront, (FLOAT) m_viewBack);

  mg_d3dDevice->SetTransform(D3DTS_PROJECTION, &m_state->m_projMatrix);

  m_viewWidth = 2*m_viewFront / m_state->m_projMatrix.m[0][0];
  m_viewHeight = 2*m_viewFront / m_state->m_projMatrix.m[1][1];

  m_state->m_viewport.X = 0;
  m_state->m_viewport.Y = 0;
  m_state->m_viewport.Width = m_graphicsWidth;
  m_state->m_viewport.Height = m_graphicsHeight;
  m_state->m_viewport.MinZ = 0.0f;
  m_state->m_viewport.MaxZ = 1.0f;
}

//--------------------------------------------------------------
// release device resources
void mgDX9Services::deleteBuffers()
{
  m_textures->deleteBuffers();
  if (m_theApp != NULL)
    m_theApp->deleteBuffers();
}

//--------------------------------------------------------------
// restore device resources
void mgDX9Services::createBuffers()
{
  initView();

  m_textures->createBuffers();
  if (m_theApp != NULL)
    m_theApp->createBuffers();
}

//--------------------------------------------------------------
// reset the display
void mgDX9Services::displayReset()
{
  mgDebug("reset display");
  // reset the D3D Device, losing all graphics state
  deleteBuffers();

  // set current size of window
  m_state->m_presentParms.BackBufferWidth = m_graphicsWidth;
  m_state->m_presentParms.BackBufferHeight = m_graphicsHeight;

  HRESULT hr = mg_d3dDevice->Reset(&m_state->m_presentParms);
  if (hr == S_OK)
    createBuffers();
}

//--------------------------------------------------------------
// animate the view, return true if changed
BOOL mgDX9Services::animateView(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  if (m_theApp != NULL)
    return m_theApp->animate(now, since);
  else return false;  // if no app, no changes to scene
}

//-----------------------------------------------------------------------------
// assign from DirectX format
void fromD3DMatrix(
  mgMatrix4 &m,
  D3DXMATRIX& other)
{
  m._11 = (double) other._11;
  m._12 = (double) other._12;
  m._13 = (double) other._13;
  m._14 = (double) other._14;
  m._21 = (double) other._21;
  m._22 = (double) other._22;
  m._23 = (double) other._23;
  m._24 = (double) other._24;
  m._31 = (double) other._31;
  m._32 = (double) other._32;
  m._33 = (double) other._33;
  m._34 = (double) other._34;
  m._41 = (double) other._41;
  m._42 = (double) other._42;
  m._43 = (double) other._43;
  m._44 = (double) other._44;
}


//-----------------------------------------------------------------------------
// convert to DirectX format
void toD3DMatrix(
  const mgMatrix4& m,
  D3DXMATRIX& trans)
{
  trans._11 = (FLOAT) m._11;
  trans._12 = (FLOAT) m._12;
  trans._13 = (FLOAT) m._13;
  trans._14 = (FLOAT) m._14;
  trans._21 = (FLOAT) m._21;
  trans._22 = (FLOAT) m._22;
  trans._23 = (FLOAT) m._23;
  trans._24 = (FLOAT) m._24;
  trans._31 = (FLOAT) m._31;
  trans._32 = (FLOAT) m._32;
  trans._33 = (FLOAT) m._33;
  trans._34 = (FLOAT) m._34;
  trans._41 = (FLOAT) m._41;
  trans._42 = (FLOAT) m._42;
  trans._43 = (FLOAT) m._43;
  trans._44 = (FLOAT) m._44;
}

//--------------------------------------------------------------
// draw the view
void mgDX9Services::drawView()
{
  mg_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
      D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

  // if too early, do nothing
  if (m_theApp == NULL)
    return;

  mg_d3dDevice->BeginScene();

  D3DXMatrixIdentity(&m_state->m_worldMatrix);

  // render sky (before light and fog, or we can't see sky)
  D3DXMATRIX eyeMatrix;
  toD3DMatrix(m_eyeMatrix, eyeMatrix);
  m_state->m_viewMatrix = eyeMatrix;

  mg_d3dDevice->SetTransform(D3DTS_PROJECTION, &m_state->m_projMatrix);
  mg_d3dDevice->SetTransform(D3DTS_WORLD, &m_state->m_worldMatrix);
  mg_d3dDevice->SetTransform(D3DTS_VIEW, &m_state->m_viewMatrix);

  // set defaults
  setMatColor(0.5, 0.5, 0.5);
  setLighting(false);

  m_theApp->drawSky();

  // set up for world object rendering
  D3DXMatrixIdentity(&m_state->m_worldMatrix);
  mg_d3dDevice->SetTransform(D3DTS_PROJECTION, &m_state->m_projMatrix);
  mg_d3dDevice->SetTransform(D3DTS_WORLD, &m_state->m_worldMatrix);

  // set up view transform
  D3DXMATRIX translate;
  D3DXMatrixTranslation(&translate, (FLOAT) -m_eyePt.x, (FLOAT) -m_eyePt.y, (FLOAT) -m_eyePt.z);
  D3DXMatrixMultiply(&m_state->m_viewMatrix, &translate, &eyeMatrix);

  mg_d3dDevice->SetTransform(D3DTS_VIEW, &m_state->m_viewMatrix);

  frustumBuildPlanes();

  setLightAndFog();

  // set defaults
  setMatColor(0.5, 0.5, 0.5);
  setLighting(true);
  setZEnable(true);
  setTransparent(false);

  // tell the app the draw the view
  m_theApp->drawView();

  // set up for front plane rendering.  screen coordinates.
  D3DXMATRIX identity;
  D3DXMatrixIdentity(&identity);
  mg_d3dDevice->SetTransform(D3DTS_PROJECTION, &identity);
  mg_d3dDevice->SetTransform(D3DTS_WORLD, &identity);

  D3DXMATRIX scaling;
  D3DXMatrixScaling(&scaling, 2.0f/(FLOAT) m_graphicsWidth, -2.0f/(FLOAT) m_graphicsHeight, 1.0f);
  D3DXMatrixTranslation(&translate, -0.5f * (FLOAT) m_graphicsWidth, -0.5f * (FLOAT) m_graphicsHeight, 0.0f);
  D3DXMatrixMultiply(&m_state->m_viewMatrix, &translate, &scaling);
  mg_d3dDevice->SetTransform(D3DTS_VIEW, &m_state->m_viewMatrix);

  // turn off quality texture filtering.  for overlay, we want the actual pixels
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

  // set defaults for overlay
  setMatColor(1.0, 1.0, 1.0);
  setLighting(false);
  setZEnable(false);

  // tell the app to draw its overlay
  m_theApp->drawOverlay();

  // draw the cursor, if enabled
  if (m_cursorEnabled)
    drawCursor();

  // turn texture filtering back on
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
  mg_d3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 2); // m_state->m_deviceCaps.MaxAnisotropy);

  mg_d3dDevice->EndScene();
}

//--------------------------------------------------------------
// set up light and fog
void mgDX9Services::setLightAndFog()
{
  HRESULT hr;

  D3DLIGHT9 lightSpec;
  memset(&lightSpec, 0, sizeof(lightSpec));

  lightSpec.Type = D3DLIGHT_DIRECTIONAL;

  lightSpec.Direction = D3DXVECTOR3((FLOAT) m_lightDir.x, (FLOAT) m_lightDir.y, (FLOAT) m_lightDir.z);
  lightSpec.Diffuse.r = (FLOAT) m_lightColor.x;
  lightSpec.Diffuse.g = (FLOAT) m_lightColor.y;
  lightSpec.Diffuse.b = (FLOAT) m_lightColor.z;
  lightSpec.Diffuse.a = 1.0f;
  lightSpec.Ambient.r = (FLOAT) m_lightAmbient.x;
  lightSpec.Ambient.g = (FLOAT) m_lightAmbient.y;
  lightSpec.Ambient.b = (FLOAT) m_lightAmbient.z;
  lightSpec.Ambient.a = 1.0f;
  lightSpec.Specular.r = 1.0f;
  lightSpec.Specular.g = 1.0f;
  lightSpec.Specular.b = 1.0f;
  lightSpec.Specular.a = 1.0f;
  lightSpec.Range = 1000000.0f;

  hr = mg_d3dDevice->SetLight(0, &lightSpec);
  hr = mg_d3dDevice->LightEnable(0, true);
}

//--------------------------------------------------------------
// draw a texture to the overlay
void mgDX9Services::drawOverlayTexture(
  const mgTextureImage* texture,
  int x,
  int y,
  int width,
  int height)
{
  struct CUSTOMVERTEX
  {
    D3DXVECTOR3 position; // The 3-D position for the vertex.
    D3DXVECTOR3 normal;   // The surface normal for the vertex.
    FLOAT       tu, tv;
  };
  // Custom FVF.
  #define CUSTOM_FVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0))

  FLOAT left = (FLOAT) x;
  FLOAT top = (FLOAT) y;

  // we are displaying this in screen coordinates, so make sure they are 
  // off by 0.5 to prevent sampling errors.  
  if (width % 2 == 0)
    left += -0.5f;
  if (height % 2 == 0)
    top += -0.5f;

  FLOAT right = left + (FLOAT) width;
  FLOAT bottom = top + (FLOAT) height;

  CUSTOMVERTEX vertices[4];
  int i = 0;
  vertices[i].position = D3DXVECTOR3(left, bottom, 0.0f);  // bl
  vertices[i].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
  vertices[i].tu = 0.0f;   vertices[i].tv = 1.0f;
  i++;
  vertices[i].position = D3DXVECTOR3(left, top, 0.0f);  // tl
  vertices[i].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
  vertices[i].tu = 0.0f;   vertices[i].tv = 0.0f;
  i++;
  vertices[i].position = D3DXVECTOR3(right, bottom, 0.0f);  // br
  vertices[i].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
  vertices[i].tu = 1.0f;   vertices[i].tv = 1.0f;
  i++;
  vertices[i].position = D3DXVECTOR3(right, top, 0.0f);  // tr
  vertices[i].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
  vertices[i].tu = 1.0f;   vertices[i].tv = 0.0f;

  mg_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

  mg_d3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

  mgDX9TextureImage* ourTexture = (mgDX9TextureImage*) texture;
  mg_d3dDevice->SetTexture(0, ourTexture->m_handle);
  mg_d3dDevice->SetFVF(CUSTOM_FVF);

  mg_d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(CUSTOMVERTEX));

  mg_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

//--------------------------------------------------------------
// draw the cursor image
void mgDX9Services::drawCursor()
{
  int x = m_cursorX;
  int y = m_cursorY;
  if (!m_cursorTrack)
  {
    x = m_graphicsWidth/2;
    y = m_graphicsHeight/2;
  }
  drawOverlayTexture(m_cursorTexture, x - m_cursorHotX, y - m_cursorHotY, 
    m_cursorTexture->m_width, m_cursorTexture->m_height);
}

//--------------------------------------------------------------
// allocate an index array
mgIndexBuffer* mgDX9Services::newIndexBuffer(
  int size,
  BOOL dynamic)
{
  return new mgDX9Indexes(size, dynamic);
}

//--------------------------------------------------------------
// set world transform
void mgDX9Services::setModelTransform(
  mgMatrix4& xform)
{
  toD3DMatrix(xform, m_state->m_worldMatrix);
  mg_d3dDevice->SetTransform(D3DTS_WORLD, &m_state->m_worldMatrix);
}

//--------------------------------------------------------------
// set light position
void mgDX9Services::setLightDir(
  double x,
  double y,
  double z)
{
  m_lightDir.x = x;
  m_lightDir.y = y;
  m_lightDir.z = z;
  m_lightDir.normalize();
  setLightAndFog();
}

//--------------------------------------------------------------
// set light color
void mgDX9Services::setLightColor(
  double r,
  double g,
  double b)
{
  m_lightColor.x = r;
  m_lightColor.y = g;
  m_lightColor.z = b;
  setLightAndFog();
}

//--------------------------------------------------------------
// set light ambient
void mgDX9Services::setLightAmbient(
  double r,
  double g,
  double b)
{
  m_lightAmbient.x = r;
  m_lightAmbient.y = g;
  m_lightAmbient.z = b;
  setLightAndFog();
}

//-----------------------------------------------------------------------------
// set material color
void mgDX9Services::setMatColor(
  double r,
  double g,
  double b)
{
  m_matColor.x = r;
  m_matColor.y = g;
  m_matColor.z = b;
  m_matColor.w = 1.0;

  D3DMATERIAL9 mtrl;
  ZeroMemory( &mtrl, sizeof(mtrl) );
  mtrl.Diffuse.r = (FLOAT) m_matColor.x;
  mtrl.Diffuse.g = (FLOAT) m_matColor.y;
  mtrl.Diffuse.b = (FLOAT) m_matColor.z;
  mtrl.Diffuse.a = (FLOAT) m_matColor.w;
  mtrl.Ambient.r = 1.0f;
  mtrl.Ambient.g = 1.0f;
  mtrl.Ambient.b = 1.0f;
  mtrl.Ambient.a = 1.0f;
  mg_d3dDevice->SetMaterial( &mtrl );
}

//--------------------------------------------------------------
// turn lighting on 
void mgDX9Services::setLighting(
  BOOL enable)
{
  m_lighting = enable;
  mg_d3dDevice->SetRenderState(D3DRS_LIGHTING, enable);
}

//--------------------------------------------------------------
// set zenable
void mgDX9Services::setZEnable(
  BOOL enable)
{
  mg_d3dDevice->SetRenderState(D3DRS_ZENABLE, enable ? D3DZB_TRUE : D3DZB_FALSE);
}

//--------------------------------------------------------------
// set culling 
void mgDX9Services::setCulling(
  BOOL enable)
{
  mg_d3dDevice->SetRenderState(D3DRS_CULLMODE, enable ? D3DCULL_CCW : D3DCULL_NONE);
}

//--------------------------------------------------------------
// set transparent
void mgDX9Services::setTransparent(
  BOOL enable)
{
  mg_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, enable);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  mg_d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  mg_d3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
}

//--------------------------------------------------------------
// set mouse relative mode
void mgDX9Services::setMouseRelative(
  BOOL relative)
{
  mgWinSystem* framework = (mgWinSystem*) mgPlatform;
  framework->setMouseRelative(relative);
}

//-----------------------------------------------------------------------------
// set cursor pattern
void mgDX9Services::setCursorTexture(
  const char* fileName,
  int hotX,
  int hotY)
{
  m_cursorTexture = (mgDX9TextureImage *) m_textures->loadTexture(fileName);
  if (m_cursorTexture == NULL)
    throw new mgException("Could not open cursor files %s", (const char*) fileName);

  m_cursorHotX = hotX;
  m_cursorHotY = hotY;
}

//-----------------------------------------------------------------------------
// save screen to file
void mgDX9Services::screenShot(
  const char* fileName)
{
  HRESULT hr;

  IDirect3DSurface9* targetSurface;

  // reads the entire screen
  hr = mg_d3dDevice->CreateOffscreenPlainSurface(
    m_graphicsWidth, m_graphicsHeight, 
    D3DFMT_X8R8G8B8, 
    D3DPOOL_SYSTEMMEM, 
    &targetSurface, 
    NULL);

  if (FAILED(hr))
    throw new mgException("Failed to create offscreen surface.");

  IDirect3DSurface9* backBuffer;
  hr = mg_d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
  if (FAILED(hr))
    throw new mgException("Failed to GetBackBuffer.");

  hr = mg_d3dDevice->GetRenderTargetData(backBuffer, targetSurface);
  if (FAILED(hr))
    throw new mgException("Failed to GetFrontBufferData.");

  D3DLOCKED_RECT lockedRect;
  hr = targetSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
  if (FAILED(hr))
    throw new mgException("Failed to lock surface.");

  BYTE* source = (BYTE*) lockedRect.pBits;
  
  if (!mgWriteBGRAtoJPGFile(fileName, m_graphicsWidth, m_graphicsHeight, source))
    MessageBox(NULL, "Unable to write jpg file.", "Save Scene", MB_OK | MB_ICONINFORMATION);

  hr = targetSurface->UnlockRect();
  targetSurface->Release();
}

//-----------------------------------------------------------------------------
// enable graphics timing with immediate mode
void mgDX9Services::setGraphicsTiming(
  BOOL enabled)
{
  mgWinSystem* framework = (mgWinSystem*) mgPlatform;
  framework->m_graphicsTiming = enabled;
}

//-----------------------------------------------------------------------------
// enable multisampling
void mgDX9Services::setMultiSample(
  BOOL enabled)
{
}

//-----------------------------------------------------------------------------
// set initial window position
void mgDX9Services::setWindowPosn(
  int x,
  int y,
  int width,
  int height)
{
  mgWinSystem* framework = (mgWinSystem*) mgPlatform;

  framework->m_windowX = x;
  framework->m_windowY = y;
  framework->m_windowWidth = width;
  framework->m_windowHeight = height;
}

#endif
