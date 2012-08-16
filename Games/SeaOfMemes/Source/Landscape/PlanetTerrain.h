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

#ifndef PLANETTERRAIN_H
#define PLANETTERRAIN_H

#include "TerrainCell.h"

const int TERRAIN_SIZE = 64;
const float WATER_LEVEL = 150.0f;

class PlanetTerrain : public TerrainCell
{
public:
  // return height of landscape at point
  static double heightAtPt(
    double tx,
    double tz);

  int m_originX;
  int m_originZ;
  int m_extentX;
  int m_extentZ;

  // stepping of outside edges
  int m_xminStep;
  int m_zminStep;
  int m_xmaxStep;
  int m_zmaxStep;

  PlanetTerrain(
    int originX,
    int originZ,
    int extentX,
    int extentZ);

  virtual ~PlanetTerrain();

  // return extent of cell
  virtual int extent() const
  { 
    return m_extentX;
  }

  // generate terrain points
  virtual void createTerrain();

  // create buffers for this cell
  virtual BOOL createBuffers();

  // delete buffers
  virtual void deleteBuffers();

  // render the cell
  virtual void render(
    const mgPoint3& renderEyePt,
    mgTextureImage* terrain);

  // render the transparent data
  virtual void renderTransparent(
    const mgPoint3& renderEyePt,
    mgTextureImage* water);

  // return resolution of terrain at point
  virtual int resolutionAt(
    int x,
    int z);

  // return true if resolution too fine
  virtual BOOL tooFine(
    const mgPoint3& eyePt);

  // return true if resolution too coarse
  virtual BOOL tooCoarse(
    const mgPoint3& eyePt);

protected:
  // leaf node has vertexes, else null
  mgVertexBuffer* m_vertexes;
  mgIndexBuffer* m_indexes;
  mgVertexBuffer* m_waterVertexes;

  // leaf node has heightmap, else null
  float* m_heights;

  // generate noise
  static double noise(
    double x,
    double y);

  // return height of landscape at point
  inline double landscapeHeight(
    double tx, 
    double tz);

  // generate terrain from heightmap
  virtual void createTerrainBuffers();

  // create the water vertexes
  virtual void createWaterBuffers();

  // extract heightmap from parent during split
  virtual void extractHeightmapFrom(
    const float* parentHeights,
    int originX,
    int originZ);

  // split chunk into four children
  virtual void splitChunk();

  // combine children into one chunk
  virtual void combineChildren();
};

#endif
