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

#include "FlatWorld.h"

const double TURN_SPEED = 120.0/1000;             // degrees per ms
const double MOVE_SPEED = 250.0/1000;               // units per ms

//--------------------------------------------------------------
// constructor
FlatWorld::FlatWorld()
{
  m_mapWidth = 0;
  m_mapHeight = 0;

  m_mapDC = NULL;
  m_mapBitmap = NULL;
  m_mapData = NULL;
  m_mapSpan = 0;
  m_mapDepth = 24;

  m_playerX = 0.0;
  m_playerY = 0.0;
  m_playerAngle = 90.0;
  m_terrainScale = 20.0;

  // build initial terrain view
  m_lastTerrainX = (int) floor(m_playerX / m_terrainScale);
  m_lastTerrainY = (int) floor(m_playerY / m_terrainScale);

  m_mapDC = CreateCompatibleDC(NULL);
  SetGraphicsMode(m_mapDC, GM_ADVANCED);

  m_redBrush = CreateSolidBrush(RGB(255, 0, 0));
  m_redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
}

//--------------------------------------------------------------
// destructor
FlatWorld::~FlatWorld()
{
  destroyBitmap();
  DeleteDC(m_mapDC);

  DeleteObject(m_redBrush);
  DeleteObject(m_redPen);
}

//--------------------------------------------------------------
// set map size
void FlatWorld::resize(
  int width,
  int height)
{
  if (width == m_mapWidth && height == m_mapHeight)
    return;

  m_mapWidth = width;
  m_mapHeight = height;

  destroyBitmap();
  createBitmap();

  rebuildTerrain(0, 0, m_mapWidth, m_mapHeight);
}

//--------------------------------------------------------------
// player forward
void FlatWorld::playerForward(
  double ms)
{
  double dist = MOVE_SPEED * ms;
  double angle = m_playerAngle*PI/180.0;
  m_playerX += dist * cos(angle);
  m_playerY += dist * sin(angle);
}

//--------------------------------------------------------------
// player backward
void FlatWorld::playerBackward(
  double ms)
{
  double dist = MOVE_SPEED * ms;
  double angle = m_playerAngle*PI/180.0;
  m_playerX -= dist * cos(angle);
  m_playerY -= dist * sin(angle);
}

//--------------------------------------------------------------
// player turn left
void FlatWorld::playerLeft(
  double ms)
{
  m_playerAngle -= TURN_SPEED * ms;
  if (m_playerAngle < 0)
    m_playerAngle += 360;
}

//--------------------------------------------------------------
// player turn right
void FlatWorld::playerRight(
  double ms)
{
  m_playerAngle += TURN_SPEED * ms;
  if (m_playerAngle > 360)
    m_playerAngle -= 360;
}

//--------------------------------------------------------------
// zoom in
void FlatWorld::zoomIn()
{
  m_terrainScale /= 1.5;
  rebuildTerrain(0, 0, m_mapWidth, m_mapHeight);
  m_lastTerrainX = (int) floor(m_playerX / m_terrainScale);
  m_lastTerrainY = (int) floor(m_playerY / m_terrainScale);
}

//--------------------------------------------------------------
// zoom out
void FlatWorld::zoomOut()
{
  m_terrainScale *= 1.5;
  rebuildTerrain(0, 0, m_mapWidth, m_mapHeight);
  m_lastTerrainX = (int) floor(m_playerX / m_terrainScale);
  m_lastTerrainY = (int) floor(m_playerY / m_terrainScale);
}

//--------------------------------------------------------------
// set new player position
void FlatWorld::setPlayerPosn(
  double x,
  double y)
{
  m_playerX = x;
  m_playerY = y;
}

//--------------------------------------------------------------
// update screen
void FlatWorld::update(
  HDC dc,
  RECT& damage)
{
  int width = damage.right - damage.left;
  int height = damage.bottom - damage.top;

  // paint the terrain
  BitBlt(dc, damage.left, damage.top, width, height, 
              m_mapDC, damage.left, damage.top, SRCCOPY);

  // paint player 
  SelectObject(dc, m_redBrush);
  SelectObject(dc, m_redPen);

  int cx = m_mapWidth/2 + (int) (m_playerX/m_terrainScale) - m_lastTerrainX;
  int cy = m_mapHeight/2 + (int) (m_playerY/m_terrainScale) - m_lastTerrainY;

  double angle = m_playerAngle*PI/180.0;
  int ax = cx + (int) (20*cos(angle));
  int ay = cy + (int) (20*sin(angle));
  Ellipse(dc, cx-4, cy-4, cx+4, cy+4);
  MoveToEx(dc, cx, cy, NULL);
  LineTo(dc, ax, ay);
}

//--------------------------------------------------------------
// rebuild a stretch of terrain
void FlatWorld::rebuildTerrain(
  int left,
  int top,
  int right,
  int bottom)
{
  // layer colors
  const int ROCK[3] = {133, 133, 133};
  const int GRASS[3] = {62, 124, 85};
  const int SAND[3] = {241, 220, 177};
  const int DIRT[3] = {179, 148, 117};
  const int WATER[3] = {16, 84, 255};
  const double WATER_TRANS = 194/255.0; 

  // snap player position to multiple of scale so that we can shift terrain
  // by integer number of pixels.
  double px = m_terrainScale * floor(m_playerX/m_terrainScale);
  double py = m_terrainScale * floor(m_playerY/m_terrainScale);

  // figure top left based on player position
  double xmin = px - (m_mapWidth/2) * m_terrainScale;
  double ymin = py - (m_mapHeight/2) * m_terrainScale;

  for (int y = top; y < bottom; y++)
  {
    double ty = ymin + y*m_terrainScale;
    for (int x = left; x < right; x++)
    {
      double tx = xmin + x*m_terrainScale;

      double ht = mgSimplexNoise::noiseSum(7, tx/40000.0, ty/40000.0);
      ht = 10.0*exp(4.0*fabs(ht));
//      ht += 0.2*mgSimplexNoise::noiseSum(3, tx/100, ty/100);

      int r, g, b;
      if (ht > 1500)
      {
        r = ROCK[0]; g = ROCK[1]; b = ROCK[2];
      }
      else if (ht > 300)  
      {
        r = GRASS[0]; g = GRASS[1]; b = GRASS[2];
      }
      else if (ht > 100)  
      {
        r = SAND[0]; g = SAND[1]; b = SAND[2];
      }
      else 
      {
        double inten = max(0, ht)/100.0;
        r = (int) (inten*DIRT[0]);
        g = (int) (inten*DIRT[1]);
        b = (int) (inten*DIRT[2]);
      }

      if (ht < 150) // water level
      {
        r = (int) (r*(1-WATER_TRANS) + WATER[0]*WATER_TRANS);
        g = (int) (g*(1-WATER_TRANS) + WATER[1]*WATER_TRANS);
        b = (int) (b*(1-WATER_TRANS) + WATER[2]*WATER_TRANS);
      }

      int posn = x*3+m_mapSpan*y;
      m_mapData[posn+0] = b;
      m_mapData[posn+1] = g;
      m_mapData[posn+2] = r;
    }
  }
}

//--------------------------------------------------------------
// recenter terrain
void FlatWorld::recenterTerrain()
{
  // get player position in pixels
  int px = (int) floor(m_playerX/m_terrainScale);
  int py = (int) floor(m_playerY/m_terrainScale);

  // figure amount to shift terrain
  int shiftX = m_lastTerrainX - px;
  int shiftY = m_lastTerrainY - py;

  // if nothing shift, no update
  if (shiftX == 0 && shiftY == 0)
    return;

  // if shifted more than width or height of screen, just rebuild
  if (abs(shiftX) >= m_mapWidth || abs(shiftY) >= m_mapHeight)
  {
    rebuildTerrain(0, 0, m_mapWidth, m_mapHeight);
    m_lastTerrainX += shiftX;
    m_lastTerrainY += shiftY;
    return;
  }

  // copy the bits we're keeping
  BitBlt(m_mapDC, shiftX, shiftY, m_mapWidth, m_mapHeight, 
         m_mapDC, 0, 0, SRCCOPY);
  GdiFlush();

  // rebuild revealed horizontal area
  if (shiftX < 0)
    rebuildTerrain(m_mapWidth + shiftX, 0, m_mapWidth, m_mapHeight);
  else if (shiftX > 0)
    rebuildTerrain(0, 0, shiftX, m_mapHeight);

  // rebuild revealed vertical area
  if (shiftY < 0)
    rebuildTerrain(0, m_mapHeight + shiftY, m_mapWidth, m_mapHeight);
  else if (shiftY > 0)
    rebuildTerrain(0, 0, m_mapWidth, shiftY);

  // update terrain position
  m_lastTerrainX -= shiftX;
  m_lastTerrainY -= shiftY;
}

//--------------------------------------------------------------
// create bitmap for rendering
void FlatWorld::createBitmap()
{
  // delete the old bitmaps, different size
  if (m_mapBitmap != NULL)
  {
    DeleteObject(m_mapBitmap);
    m_mapBitmap = NULL;
  }

  // create bitmap info header 
  BITMAPINFO *info = (BITMAPINFO *) new char[sizeof(BITMAPINFOHEADER)];

  info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info->bmiHeader.biPlanes = 1;
  info->bmiHeader.biBitCount = m_mapDepth;
  info->bmiHeader.biCompression = BI_RGB;
  info->bmiHeader.biSizeImage = 0;
  info->bmiHeader.biClrUsed = 0;
  info->bmiHeader.biClrImportant = 0;

  info->bmiHeader.biWidth = m_mapWidth;
  info->bmiHeader.biHeight = -m_mapHeight;

  if (m_mapDepth == 32)
    m_mapSpan = m_mapWidth*4;  // 4 bytes per pixel
  else
  {
    m_mapSpan = m_mapWidth*3;  // 3 bytes per pixel
    m_mapSpan = 4*((m_mapSpan+3)/4);  // round up to next word
  }

  // create the bitmap
  void *pData;
  m_mapBitmap = CreateDIBSection(m_mapDC, info, DIB_PAL_COLORS, &pData, NULL, 0);
  m_mapData = (BYTE*) pData;
  SelectObject(m_mapDC, m_mapBitmap);
  delete info;

  // clear the image
  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = m_mapWidth;
  rect.bottom = m_mapHeight;
  FillRect(m_mapDC, &rect, (HBRUSH) GetStockObject(BLACK_BRUSH));
}

//--------------------------------------------------------------
// destroy bitmap
void FlatWorld::destroyBitmap()
{
  // delete the surface DC
  SelectObject(m_mapDC, (HBITMAP) NULL);
  SelectObject(m_mapDC, (HFONT) NULL);
  SelectObject(m_mapDC, (HBRUSH) NULL);
  SelectObject(m_mapDC, (HPEN) NULL);

  if (m_mapBitmap != NULL)
  {
    DeleteObject(m_mapBitmap);
    m_mapBitmap = NULL;
  }
}

