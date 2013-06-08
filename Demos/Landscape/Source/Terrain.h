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

#ifndef TERRAIN_H
#define TERRAIN_H

const int TERRAIN_SIZE = 64;

const float WATER_LEVEL = 150.0f;

class Terrain
{
public:
  int m_originX;
  int m_originZ;
  int m_extentX;
  int m_extentZ;

  Terrain* m_children[4];

  // stepping of outside edges
  int m_xminStep;
  int m_zminStep;
  int m_xmaxStep;
  int m_zmaxStep;

  BOOL m_rebuildHeightmap;
  BOOL m_rebuildBuffers;
  double m_age;

  // leaf node has vertexes, else null
  mgVertexBuffer* m_vertexes;
  mgIndexBuffer* m_indexes;
  mgVertexBuffer* m_waterVertexes;

  // leaf node has heightmap, else null
  float* m_heights;
  double m_minHeight;
  double m_maxHeight;

  // constructor
  Terrain(
    int originX,
    int originZ,
    int extentX,
    int extentZ);

  // destructor
  virtual ~Terrain();

  // return resolution of terrain at point
  virtual int resolutionAt(
    int x,
    int z);

  // generate heightmap from noise function
  virtual void createHeightmap();

  // mark buffers for rebuild
  virtual void rebuildBuffers();

  // create buffers for this chunk of terrain
  virtual BOOL createBuffers(
    BOOL mapView,
    int gridStyle);

  // delete buffers
  virtual void deleteBuffers();

  // render the chunk
  virtual void render(
    const mgPoint3& renderEyePt,
    mgTextureImage* terrain,
    mgTextureImage* highlight);

  // render the transparent data
  virtual void renderTransparent(
    const mgPoint3& renderEyePt,
    mgTextureImage* water);

  // check resolution of chunk
  virtual void checkResolution(
    const mgPoint3& eyePt);

  // get list of terrain that needs rebuild
  virtual void buildList(
    mgPtrArray& list);

  // return count of terrain chunks
  virtual int count();

  // return height of landscape at point
  static double heightAtPt(
    double tx,
    double tz);

protected:
  // extract heightmap from parent during split
  virtual void extractHeightmapFrom(
    const float* parentHeights,
    int originX,
    int originZ);

  // generate terrain from heightmap
  virtual void createTerrain(
    BOOL mapView,
    int gridStyle);

  // create the water vertexes
  virtual void createWater();

  // return true if resolution too fine
  virtual BOOL tooFine(
    const mgPoint3& eyePt);

  // return true if resolution too coarse
  virtual BOOL tooCoarse(
    const mgPoint3& eyePt);

  // split chunk into four children
  virtual void splitChunk();

  // combine children into one chunk
  virtual void combineChildren();
};

#endif
