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

#include "PlanetTerrain.h"

#include "Landscape.h"

//--------------------------------------------------------------
// rebuild chunks of terrain
void buildThreadProc(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2);

//--------------------------------------------------------------
// constructor
Landscape::Landscape()
{
  m_maxCount = 0;  // number of terrain chunks in use

//  initTerrain();  // =-= needs an initial eye point
//  createBuffers();

  m_buildEvent = NULL;
  m_buildThreads = NULL;

  // init threads
//  m_threadCount = m_options.getInteger("threadCount", 1); 

  m_buildLock = mgOSLock::create();

  m_shutdown = false;
  if (m_threadCount > 0)
  {
    m_buildEvent = mgOSEvent::create();

    m_buildThreads = mgOSThread::create(m_threadCount, buildThreadProc, 
      mgOSThread::PRIORITY_LOW, this, NULL);
  }
}

//--------------------------------------------------------------
// destructor
Landscape::~Landscape()
{
  m_shutdown = true;
  if (m_buildThreads != NULL)
  {
    m_buildEvent->signal();
    m_buildThreads->waitForEnd(200.0);  // wait for end

    delete m_buildThreads;
    m_buildThreads = NULL;

    delete m_buildEvent;
    m_buildEvent = NULL;
  }

  delete m_buildLock;
  m_buildLock = NULL;

  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      delete m_terrain[x][z];
      m_terrain[x][z] = NULL;
    }
  }
}

//--------------------------------------------------------------------
// change resolution of terrain based on distance
void Landscape::checkTerrainResolution(
  const mgPoint3& eyePt)
{
  int count = 0;
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      TerrainCell* cell = m_terrain[x][z];
      checkResolution(cell, eyePt);
      count += countCells(cell);
    }
  }

  m_maxCount = max(m_maxCount, count);
}

//--------------------------------------------------------------
// sort the rebuild array, smallest to largest extent
int rebuildSort(
  const void* a, 
  const void* b)
{
  const TerrainCell* chunkA = *(TerrainCell**) a;
  const TerrainCell* chunkB = *(TerrainCell**) b;

  return chunkB->extent() - chunkA->extent();
}

//--------------------------------------------------------------------
// get list of terrain that needs rebuild
void Landscape::terrainBuildList()
{
  m_buildList.removeAll();

  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      TerrainCell* cell = m_terrain[x][z];
      buildList(cell, m_buildList);
    }
  }
  m_buildList.sort(rebuildSort);
}

//--------------------------------------------------------------
// rebuild a chunk of terrain off the buildList
BOOL Landscape::rebuildTerrain()
{
  BOOL moreWork = false;

  // rebuild a chunk off the build list
  if (m_buildList.length() > 0)
  {
    TerrainCell* chunk = (TerrainCell*) m_buildList.pop();
    chunk->createTerrain();
    chunk->m_rebuildBuffers = true;

    moreWork = m_buildList.length() > 0;
  }

  return moreWork;
}

//-----------------------------------------------------------------------------
// create all terrain buffers
BOOL Landscape::createTerrainBuffers()
{
  BOOL changed = false;
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      TerrainCell* cell = m_terrain[x][z];
      changed |= cell->createBuffers();
    }
  }
  return changed;
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void Landscape::createBuffers()
{
  createTerrainBuffers();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void Landscape::deleteBuffers()
{
  // delete the terrain buffers
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      TerrainCell* cell = m_terrain[x][z];
      cell->deleteBuffers();
    }
  }
}

//-----------------------------------------------------------------------------
// delete all terrain buffers
void Landscape::rebuildTerrainBuffers()
{
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      TerrainCell* cell = m_terrain[x][z];
      rebuildBuffers(cell);
    }
  }
}

//--------------------------------------------------------------------
// update the terrain after movement
BOOL Landscape::updateTerrain(
  const mgPoint3& eyePt)
{
  BOOL changed = false;

  m_buildLock->lock();

  checkTerrainBounds(eyePt);
  checkTerrainResolution(eyePt);

  // check the edges on all chunks
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      TerrainCell* cell = m_terrain[x][z];
      checkTerrainEdges(cell);
    }
  }

  // get list of chunks needing rebuild (finer res height map)
  terrainBuildList();

  // if we requested chunks, activate threads
  if (m_buildList.length() > 0)
  {
    if (m_buildThreads != NULL)
    {
      // signal the threads to look at buildList
      m_buildEvent->signal();
    }
    else rebuildTerrain();
  }

  changed |= createTerrainBuffers();

  m_buildLock->unlock();

  return changed;
}

//--------------------------------------------------------------------
// render the terrain
void Landscape::render(
  const mgPoint3& eyePt)
{
  mgDisplay->setEyePt(mgPoint3(0,0,0));  // we use relative coordinates in render

  if (mgDisplay->getDepthBits() == 32)
  {
    mgDisplay->setFrontAndBack(0.25, 65536*4);

    renderTerrain(eyePt);
  }
  else if (mgDisplay->getDepthBits() >= 24)
  {
    mgDisplay->setFrontAndBack(256, 65536*4);

    renderTerrain(eyePt);

    mgDisplay->setFrontAndBack(0.25, 256);
    mgDisplay->clearBuffer(MG_DEPTH_BUFFER);

    renderTerrain(eyePt);
  }
  else
  {
    mgDisplay->setFrontAndBack(4096, 65536*4);

    renderTerrain(eyePt);

    mgDisplay->setFrontAndBack(32, 4096);
    mgDisplay->clearBuffer(MG_DEPTH_BUFFER);

    renderTerrain(eyePt);

    mgDisplay->setFrontAndBack(0.25, 32);
    mgDisplay->clearBuffer(MG_DEPTH_BUFFER);

    renderTerrain(eyePt);
  }
}


//--------------------------------------------------------------
// check resolution of chunk
void Landscape::checkResolution(
  TerrainCell* cell,
  const mgPoint3& eyePt)
{
  // if this is a leaf node
  if (cell->m_children[0] == NULL)
  {
    if (cell->tooCoarse(eyePt))
    {
      cell->splitChunk();

      // see if children need split
      for (int i = 0; i < 4; i++)
      {
        TerrainCell* child = cell->m_children[i];
        checkResolution(child, eyePt);
      }
    }
  }
  else
  {
    // recurse to children
    BOOL tooFine = true;
    for (int i = 0; i < 4; i++)
    {
      TerrainCell* child = cell->m_children[i];
      checkResolution(child, eyePt);
      if (child->m_children[0] != NULL || !child->tooFine(eyePt))
        tooFine = false;
    }

    // if all four children tooFine, combine them
    if (tooFine)
      cell->combineChildren();
  }
}

//--------------------------------------------------------------
// mark buffers for rebuild
void Landscape::rebuildBuffers(
  TerrainCell* cell)
{
  if (cell->m_children[0] == NULL)
    cell->m_rebuildBuffers = true;
  else
  {
    for (int i = 0; i < 4; i++)
    {
      TerrainCell* child = cell->m_children[i];
      rebuildBuffers(child);
    }
  }
}

//--------------------------------------------------------------
// count the descendents of a cell
int Landscape::countCells(
  TerrainCell* cell)
{
  if (cell->m_children[0] == NULL)
    return 1;

  int total = 0;
  for (int i = 0; i < 4; i++)
  {
    TerrainCell* child = cell->m_children[i];
    total += countCells(child);
  }                          

  return total;
}

//--------------------------------------------------------------
// get list of terrain that needs rebuild
void Landscape::buildList(
  TerrainCell* cell,
  mgPtrArray& list)
{
  if (cell->m_children[0] == NULL)
  {
    if (cell->m_rebuildHeightmap)
      list.add(cell);
  }
  else
  {
    // recurse to children
    for (int i = 0; i < 4; i++)
    {
      TerrainCell* child = cell->m_children[i];
      buildList(child, list);
    }                          
  }
}

//--------------------------------------------------------------
// rebuild chunks of terrain
void buildThreadProc(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2)
{
  Landscape* landscape = (Landscape*) threadArg1;

  while (!landscape->m_shutdown)
  {
    landscape->m_buildEvent->wait(1000.0);  // check shutdown every second

    // while there's work
    while (!landscape->m_shutdown)
    {
      landscape->m_buildLock->lock();
      BOOL moreWork = !landscape->rebuildTerrain();
      landscape->m_buildLock->unlock();

      if (!moreWork)
        break;
    }
  }
}

