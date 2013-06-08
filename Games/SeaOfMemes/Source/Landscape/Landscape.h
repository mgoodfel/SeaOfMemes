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
#ifndef LANDSCAPE_H
#define LANDSCAPE_H

class TerrainCell;

class Landscape
{
public:
  // constructor
  Landscape();

  // destructor
  virtual ~Landscape();

  // delete any display buffers
  virtual void deleteBuffers();

  // create buffers, ready to send to display
  virtual void createBuffers();

  // render the view
  virtual void render(
    const mgPoint3& eyePt);

protected:
  int m_threadCount;                      // max thread count

  mgPtrArray m_buildList;                 // list of cells needing rebuild
  mgOSLock* m_buildLock;                  // access to rebuild list
  mgOSEvent* m_buildEvent;                // update to rebuild list
  mgOSThread* m_buildThreads;             // rebuild threads
  BOOL m_shutdown;

  TerrainCell* m_terrain[3][3];
  int m_maxCount;

  // generate the initial terrain
  virtual void initTerrain() = 0;

  // add terrain if eye moved near edges
  virtual void checkTerrainBounds(
    const mgPoint3& eyePt) = 0;


  // update edge stepping
  virtual void checkTerrainEdges(
    TerrainCell* cell) = 0;

  // render a pass of terrain
  virtual void renderTerrain(
    const mgPoint3& eyePt) = 0;

  // change resolution of terrain based on distance
  virtual void checkTerrainResolution(
    const mgPoint3& eyePt);

  // create all terrain buffers
  virtual BOOL createTerrainBuffers();

  // delete all terrain buffers
  virtual void rebuildTerrainBuffers();

  // get list of terrain that needs rebuild
  virtual void terrainBuildList();

  // update the terrain after movement
  virtual BOOL updateTerrain(
    const mgPoint3& eyePt);

  // rebuild a cell of terrain off the buildList
  virtual BOOL rebuildTerrain();

  // check resolution of cell
  virtual void checkResolution(
    TerrainCell* cell,
    const mgPoint3& eyePt);

  // mark buffers for rebuild
  virtual void rebuildBuffers(
    TerrainCell* cell);

  // get list of terrain that needs rebuild
  virtual void buildList(
    TerrainCell* cell,
    mgPtrArray& list);

  // return count of terrain cells
  virtual int countCells(
    TerrainCell* cell);

  friend void buildThreadProc(mgOSThread* thread, void* threadArg1, void* threadArg2);
};

#endif
