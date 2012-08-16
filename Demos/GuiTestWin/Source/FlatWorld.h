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
#ifndef FLATWORLD_H
#define FLATWORLD_H

class FlatWorld
{
public: 
  double m_playerX;
  double m_playerY;
  double m_playerAngle;
  double m_terrainScale;

  // constructor
  FlatWorld();

  // destructor
  virtual ~FlatWorld();

  // set map size
  virtual void resize(
    int width,
    int height);

  // update map
  virtual void update(
    HDC dc,
    RECT& damage);

  // player forward
  virtual void playerForward(
    double ms);

  // player backward
  virtual void playerBackward(
    double ms);

  // player turn right
  virtual void playerRight(
    double ms);

  // player turn left
  virtual void playerLeft(
    double ms);

  // zoom in
  virtual void zoomIn();

  // zoom out
  virtual void zoomOut();

  // adjust terrain after player movement
  virtual void recenterTerrain();

  // set new player position
  virtual void setPlayerPosn(
    double x,
    double y);

protected:
  int m_mapWidth;
  int m_mapHeight;

  HDC m_mapDC;
  HBITMAP m_mapBitmap;
  BYTE* m_mapData;
  int m_mapSpan;
  int m_mapDepth;

  int m_lastTerrainX;
  int m_lastTerrainY;

  HBRUSH m_redBrush;
  HPEN m_redPen;

  // generate new terrain in region
  virtual void rebuildTerrain(
    int left,
    int top,
    int right,
    int bottom);

  // create the bitmap
  virtual void createBitmap();

  // destroy the bitmap
  virtual void destroyBitmap();
};

#endif
