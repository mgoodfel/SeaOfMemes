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

#ifndef TERRAINCELL_H
#define TERRAINCELL_H

class TerrainCell
{
public:
  TerrainCell* m_children[4];

  BOOL m_rebuildHeightmap;
  BOOL m_rebuildBuffers;

  double m_minHeight;
  double m_maxHeight;

  // constructor
  TerrainCell();

  // destructor
  virtual ~TerrainCell();

  // return extent of cell
  virtual int extent() const = 0;

  // generate terrain points
  virtual void createTerrain() = 0;

  // create buffers for this cell
  virtual BOOL createBuffers() = 0;

  // delete buffers
  virtual void deleteBuffers() = 0;

  // render the cell
  virtual void render(
    const mgPoint3& renderEyePt,
    mgTextureImage* terrain) = 0;

  // render the transparent data
  virtual void renderTransparent(
    const mgPoint3& renderEyePt,
    mgTextureImage* water) = 0;

  // split chunk into four children
  virtual void splitChunk() = 0;

  // combine children into one chunk
  virtual void combineChildren() = 0;

  // return true if resolution too fine
  virtual BOOL tooFine(
    const mgPoint3& eyePt) = 0;

  // return true if resolution too coarse
  virtual BOOL tooCoarse(
    const mgPoint3& eyePt) = 0;

  // return resolution of terrain at point
  virtual int resolutionAt(
    int x,
    int z) = 0;
};

#endif
