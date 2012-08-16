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
#ifndef PLANETLANDSCAPE_H
#define PLANETLANDSCAPE_H

#include "Landscape.h"

class PlanetLandscape : public Landscape
{
public:
  // constructor
  PlanetLandscape();

  // destructor
  virtual ~PlanetLandscape();

  // generate the initial terrain
  virtual void initTerrain();

  // add terrain if eye moved near edges
  virtual void checkTerrainBounds(
    const mgPoint3& eyePt);

  // update edge stepping
  virtual void checkTerrainEdges(
    TerrainCell* chunk);

  // render a pass of terrain
  virtual void renderTerrain(
    const mgPoint3& eyePt);

protected:
  mgPoint3 m_lightDir;
  mgPoint3 m_lightColor;
  mgPoint3 m_lightAmbient;

  mgPoint3 m_fogColor;
  double m_fogTopHeight;
  double m_fogTopInten;
  double m_fogBotHeight;
  double m_fogBotInten;
  double m_fogMaxDist;

  mgTextureImage* m_waterTexture;
  mgTextureImage* m_terrainTexture;

  // return cell in terrain
  PlanetTerrain* terrainAt(
    int x,
    int z)
  {
    return (PlanetTerrain*) m_terrain[x][z];
  }

  // return resolution of terrain at point
  virtual int resolutionAt(
    int x,
    int z);

};

#endif
