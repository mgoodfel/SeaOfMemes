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

#include "Bricks/BrickBlob.h"
#include "Bricks/BrickBlobInt.h"
#include "Bricks/BrickBlobFloat.h"
#include "Bricks/BrickSet.h"
#include "Bricks/BrickSetFile.h"
#include "Bricks/ShapeDefn.h"
#include "Bricks/ShapeFile.h"

#include "ChunkObj.h"
#include "ChunkWorld.h"

#include "Minecraft/MinecraftRegion.h"

#ifdef WIN32
#include "shlobj.h"
#endif

// an entry in the viewList
class ChunkOrigin
{
public:
  int x;
  int z;
};

const int MAX_OPEN_REGIONS = 10;
const int HORIZON_DIST = 8192;

//--------------------------------------------------------------
// load chunks from the active list
void chunkThreadProc(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2)
{
  ChunkWorld* world = (ChunkWorld*) threadArg1;
  world->chunkLoader();
}

//--------------------------------------------------------------
// constructor
ChunkWorld::ChunkWorld(
  const mgOptionsFile& options)
{
  BrickBlob::staticInit();
  loadShaders();

  findWorldDir(options);

  // read the brick set to use
  mgString fileName;
  options.getFileName("brickSet", options.m_sourceFileName, "bricks.xml", fileName);

  BrickSetFile brickSetFile(fileName);
  readBrickSet(brickSetFile);
  loadWaterTexture(brickSetFile);

  m_textureArray = mgDisplay->loadTextureArray(m_textureFiles);

  m_displayMemLimit = options.getInteger("displayMemory", 200); 
  m_displayMemLimit *= 1024*1024; // megabytes

  m_systemMemLimit = options.getInteger("systemMemory", 400); 
  m_systemMemLimit *= 1024*1024; // megabytes

  m_threadCount = options.getInteger("threadCount", 1); 
  m_viewDistance = options.getInteger("viewDistance", 300);

  // since the view list goes up as the cube of distance, limit it
  m_viewDistance = min(450, m_viewDistance);

  m_sortCount = options.getInteger("sortCount", 5);

  options.getPoint("torchColor", mgPoint3(0.5, 0.5, 0.1), m_torchColor);

  mgDebug("thread count: %d", m_threadCount);

  m_viewList = NULL;

  createViewList(m_viewDistance);
  createHorizon();
  createWater();

  m_displayMemUsed = 0;
  m_systemMemUsed = 0;
  m_shutdown = false;
  m_chunksChanged = false;

  createWorkers();
}

//--------------------------------------------------------------
// destructor
ChunkWorld::~ChunkWorld()
{
  m_shutdown = true;
  if (m_chunkThreads != NULL)
  {
    m_chunkEvent->signal();
    m_chunkThreads->waitForEnd(2000.0);  // wait for end

    delete m_chunkThreads;
    m_chunkThreads = NULL;

    delete m_chunkEvent;
    m_chunkEvent = NULL;
  }

  delete m_chunkLock;
  m_chunkLock = NULL;

  // free chunk cache
  int posn = m_chunkTable.getStartPosition();
  while (posn != -1)
  {
    const void* value;
    int x, y, z;
    m_chunkTable.getNextAssoc(posn, x, y, z, value);
    ChunkObj* chunk = (ChunkObj*) value;
    delete chunk;
  }
  m_chunkTable.removeAll();

  // free region cache
  posn = m_regionTable.getStartPosition();
  while (posn != -1)
  {
    const void* value;
    int x, y, z;
    m_regionTable.getNextAssoc(posn, x, y, z, value);
    MinecraftRegion* region = (MinecraftRegion*) value;
    delete region;
  }
  m_regionTable.removeAll();

  // free texture files
  m_textureFiles.removeAll();

  // free brick set
  delete m_brickSet;
  m_brickSet = NULL;

  // free shape table
  posn = m_shapes.getStartPosition();
  mgString key;
  while (posn != -1)
  {
    const void* value;
    m_shapes.getNextAssoc(posn, key, value);
    delete (ShapeDefn*) value;
  }
  m_shapes.removeAll();

  delete m_viewList;
  m_viewList = NULL;

  m_requestList.removeAll();
  m_LRUList.removeAll();

  delete m_horizonVertexes;
  m_horizonVertexes = NULL;

  delete m_waterVertexes;
  m_waterVertexes = NULL;
}

//--------------------------------------------------------------
// load shaders
void ChunkWorld::loadShaders()
{
  mgVertexTA::loadShader("unlitTextureArray");
  mgVertexTA::loadShader("litTextureArray");
  mgVertexTA::loadShader("horizon");
  mgVertex::loadShader("unlitTexture");
  mgVertex::loadShader("litTexture");

  if (mgDisplay->supportsIntegerVertex())
    BrickBlobInt::loadShaders();
  else BrickBlobFloat::loadShaders();
}

//--------------------------------------------------------------
// find the minecraft world directory
void ChunkWorld::findWorldDir(
  const mgOptionsFile& options)
{
  mgString worldName;
  options.getString("world", "New World", worldName);

  // initialize the region directory
#ifdef WIN32
  const char* appData = getenv("APPDATA");
  if (appData != NULL)
    m_regionDir = appData;
  m_regionDir += "\\.minecraft\\saves\\";
  m_regionDir += worldName;
#endif

#ifdef __unix__
  const char* appData = getenv("HOME");
  if (appData != NULL)
    m_regionDir = appData;
  m_regionDir += "/.minecraft/saves/";
  m_regionDir += worldName;
#endif

#ifdef __APPLE__
  const char* appData = getenv("HOME");
  if (appData != NULL)
    m_regionDir = appData;
  m_regionDir += "/Library/Application Support/minecraft/saves/";
  m_regionDir += worldName;
#endif

  // look for the level.dat file in this world
  mgString fileName(m_regionDir);
  fileName += "/level.dat";
  mgOSFixFileName(fileName);
  FILE* levelFile = mgOSFileOpen(fileName, "r");
  if (levelFile != NULL)
    fclose(levelFile);
  else
  {
    // try option as a file name instead of world name
    if (worldName.endsWith("/") || worldName.endsWith("\\"))
      worldName.substring(m_regionDir, 0, worldName.length()-1);
    else m_regionDir = worldName;

    fileName = worldName;
    fileName += "/level.dat";
    levelFile = mgOSFileOpen(fileName, "r");
    if (levelFile != NULL)
      fclose(levelFile);

    else throw new mgException("Cannot open minecraft save file %s", (const char*) m_regionDir);
  }

//  m_regionDir += "/DIM-1/region"; // nether regions
  m_regionDir += "/region";
}

//--------------------------------------------------------------
// create lock, event, threads
void ChunkWorld::createWorkers()
{
  // create lock, event, threads
  m_chunkLock = mgOSLock::create();

  m_chunkEvent = NULL;
  m_chunkThreads = NULL;

  if (m_threadCount > 0)
  {
    m_chunkEvent = mgOSEvent::create();

    m_chunkThreads = mgOSThread::create(m_threadCount, chunkThreadProc, 
      mgOSThread::PRIORITY_LOW, this, NULL);
  }
}

//--------------------------------------------------------------
// sort chunk offsets in view list by distance
int offsetSort(
  const void* a,
  const void* b)
{
  ChunkOrigin* originA = (ChunkOrigin*) a;
  int distA = originA->x*originA->x + originA->z*originA->z;

  ChunkOrigin* originB = (ChunkOrigin*) b;
  int distB = originB->x*originB->x + originB->z*originB->z;

  if (distA < distB)
    return -1;
  if (distA > distB)
    return 1;
  if (originA->x < originB->x)
    return -1;
  if (originA->x > originB->x)
    return 1;
  return originA->z - originB->z;
}

//--------------------------------------------------------------
// create view chunk offsets to distance
void ChunkWorld::createViewList(
  int distance)
{
  delete m_viewList;
  m_viewList = NULL;

  // we pull chunks from the cache by taking the current position 
  // and adding offsets from the view list.  So the view list is
  // the surrounding chunks, sorted from near to far to give the
  // correct painting order.  

  // figure number of chunk-sized cubes in distance
  int cubeDistance = (distance+CHUNK_SIZE-1)/CHUNK_SIZE;
  cubeDistance = max(1, cubeDistance);

  int size = 1+2*cubeDistance;
  m_viewListSize = size*size;
  m_viewList = new ChunkOrigin[m_viewListSize];
  m_viewListCount = 0;
  for (int x = -cubeDistance; x <= cubeDistance; x++)
  {
    for (int z = -cubeDistance; z <= cubeDistance; z++)
    {
      ChunkOrigin* origin = &m_viewList[m_viewListCount++];
      origin->x = x;
      origin->z = z;
    }
  }

  qsort(m_viewList, m_viewListCount, sizeof(ChunkOrigin), offsetSort);

  mgDebug("view distance = %d, list size = %d", m_viewDistance, m_viewListCount);

/* =-= debug
  for (int i = 0; i < m_viewListCount; i++)
  {
    ChunkOrigin* origin = &m_viewList[i];
    double x = origin.x;
    double z = origin.z;
    double dist = sqrt(x*x + z*z);
    mgDebug("(%d, %d) dist = %g", origin.x, origin.z, dist);
  }
*/
}

//--------------------------------------------------------------
// set the sky color
void ChunkWorld::setSkyColor(
  const mgPoint3& color)
{
  m_skyColor = color;
  m_fogColor.x = color.x;
  m_fogColor.y = color.y;
  m_fogColor.z = color.z;
  m_fogColor.w = 1.0;
}

//--------------------------------------------------------------
// set the eye point
void ChunkWorld::setEyePt(
  const mgPoint3& eyePt)
{
  // currently, this is called with animate, so it will never
  // conflict with the use there, but just to be safe... get the lock.
  m_chunkLock->lock();

  m_eyePt = eyePt;

  m_chunkLock->unlock();
}

//--------------------------------------------------------------
// delete any display buffers
void ChunkWorld::deleteBuffers()
{
  m_chunkLock->lock();

  int posn = m_chunkTable.getStartPosition();
  while (posn != -1)
  {
    const void* value;
    int x, y, z;
    m_chunkTable.getNextAssoc(posn, x, y, z, value);
    ChunkObj* chunk = (ChunkObj*) value;
    if (chunk->m_status == CHUNK_INDISPLAY ||
        chunk->m_status == CHUNK_NEEDSUPDATE)
      chunk->deleteBuffers();
  }

  m_chunkLock->unlock();
}

//--------------------------------------------------------------
// create buffers, ready to send to display
void ChunkWorld::createBuffers()
{
  m_chunkLock->lock();

  int posn = m_chunkTable.getStartPosition();
  while (posn != -1)
  {
    const void* value;
    int x, y, z;
    m_chunkTable.getNextAssoc(posn, x, y, z, value);
    ChunkObj* chunk = (ChunkObj*) value;
    if (chunk->m_status == CHUNK_INDISPLAY ||
        chunk->m_status == CHUNK_NEEDSUPDATE)
      chunk->createBuffers(chunk->m_eyePt);
  }

  m_chunkLock->unlock();
}

//--------------------------------------------------------------
// update animation 
BOOL ChunkWorld::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  BOOL viewChanged = false;

  m_chunkLock->lock();

  // keep memory use under limits
  checkMemory();

  resetRequestList();

  // figure which chunk we are in
  int viewX = (int) floor(m_eyePt.x / CHUNK_SIZE);
  int viewZ = (int) floor(m_eyePt.z / CHUNK_SIZE);

  // for all coordinates in view list
  for (int i = m_viewListCount-1; i >= 0; i--)
  {
    ChunkOrigin* origin = &m_viewList[i];
    int chunkX = CHUNK_SIZE*(viewX + origin->x);
    int chunkZ = CHUNK_SIZE*(viewZ + origin->z);

    // find/create chunk
    ChunkObj* chunk = createChunk(chunkX, chunkZ);

    // if chunk within view
    if (chunk->withinFrustum())
    {
      switch (chunk->m_status)
      {
        case CHUNK_UNLOADED:
          // request the chunk (loads in memory)
          requestChunk(chunk);

          // request the neighbors
          loadNeighbors(chunk);
          break;

        case CHUNK_INMEMORY:
          // if all neighbors loaded, update chunk edges
          if (loadNeighbors(chunk))
          {
            updateEdges(chunk);

            // request the chunk (loads in display)
            requestChunk(chunk);
          }
          break;

        case CHUNK_INDISPLAY:
          // if we have an update, use it
          if (!chunk->m_active && chunk->m_hasUpdate)
          {
//            mgDebug("use update on (%d, %d)", chunk->m_originX, chunk->m_originZ);
            int oldSize = chunk->getDisplayMemUsed();
            chunk->useUpdate();
            int size = chunk->getDisplayMemUsed();
            m_displayMemUsed += size-oldSize;
            chunk->m_hasUpdate = false;
          }

          // if it needs an update, flag it
          if (!chunk->m_active && chunk->needsUpdate(m_eyePt))
          {
//            mgDebug("wants update on (%d, %d)", chunk->m_originX, chunk->m_originZ);
            chunk->m_status = CHUNK_NEEDSUPDATE;
            chunk->m_eyePt = m_eyePt;
            m_requestList.addToTail(chunk);
          }

          // if still in display, animate it, since we will draw it
          if (chunk->m_status == CHUNK_INDISPLAY ||
              chunk->m_status == CHUNK_NEEDSUPDATE)
            chunk->animate(now, since);
          break;

        case CHUNK_NEEDSUPDATE:
          // if needed update last pass, and not being processed, still needs update
          if (!chunk->m_active)
          {
//            mgDebug("still needs update on (%d, %d)", chunk->m_originX, chunk->m_originZ);
            m_requestList.addToTail(chunk);
            chunk->animate(now, since);
          }
          break;
      }

      // push to bottom of LRU list
      ChunkListNode* node = m_LRUList.find(chunk);
      if (node != NULL)
        m_LRUList.removeNode(node);
      m_LRUList.addToTail(chunk);
    }
  }

  if (m_chunksChanged)
  {
    viewChanged = true;
    m_chunksChanged = false;
  }

  m_chunkLock->unlock();

  // if we requested chunks, activate worker threads
  if (!m_requestList.isEmpty())
  {
    if (m_chunkThreads != NULL)
    {
      // signal the threads to look at requestList
      m_chunkEvent->signal();
    }
    else
    {
      // process one request in this thread (no worker threads)
      ChunkObj* found = dequeueRequest();
      if (found != NULL)
        processRequest(found);
    }
  }

  return viewChanged;
}

//--------------------------------------------------------------
// render the world
void ChunkWorld::render()
{
  m_chunkLock->lock();

  // figure which chunk we are in
  mgPoint3 eyePt;
  mgDisplay->getEyePt(eyePt);
  int viewX = (int) floor(eyePt.x / CHUNK_SIZE);
  int viewZ = (int) floor(eyePt.z / CHUNK_SIZE);

  drawHorizon(viewX, viewZ);

  // render all neighbors in the view list
  for (int i = m_viewListCount-1; i >= 0; i--)
  {
    ChunkOrigin* origin = &m_viewList[i];
    int x = CHUNK_SIZE*(viewX + origin->x);
    int z = CHUNK_SIZE*(viewZ + origin->z);

    const void* value;
    if (!m_chunkTable.lookup(x, 0, z, value))
      continue;  // outside world bounds
    ChunkObj* chunk = (ChunkObj*) value;

    // if chunk loaded and within view, render it
    if (chunk->m_status == CHUNK_INDISPLAY ||
        chunk->m_status == CHUNK_NEEDSUPDATE)
    {
      chunk->render(this);
    }
    else drawWater(x, z);
  }

  m_chunkLock->unlock();
}

//--------------------------------------------------------------
// render the world transparent data
void ChunkWorld::renderTransparent()
{
  m_chunkLock->lock();

  // figure which chunk we are in
  mgPoint3 eyePt;
  mgDisplay->getEyePt(eyePt);
  int viewX = (int) floor(eyePt.x / CHUNK_SIZE);
  int viewZ = (int) floor(eyePt.z / CHUNK_SIZE);

  // render all neighbors in the view list
  for (int i = m_viewListCount-1; i >= 0; i--)
  {
    ChunkOrigin* origin = &m_viewList[i];
    int x = CHUNK_SIZE*(viewX + origin->x);
    int z = CHUNK_SIZE*(viewZ + origin->z);

    const void* value;
    if (!m_chunkTable.lookup(x, 0, z, value))
      continue;  // outside world bounds
    ChunkObj* chunk = (ChunkObj*) value;

    // if chunk loaded and within view, render it
    if (chunk->m_status == CHUNK_INDISPLAY ||
        chunk->m_status == CHUNK_NEEDSUPDATE)
    {
      chunk->renderTransparent(this);
    }
  }

  m_chunkLock->unlock();
}

//--------------------------------------------------------------
// write stats to debug
void ChunkWorld::debugStats(
  mgString& status)
{
  m_chunkLock->lock();

  int unloadedCount = 0;
  int inmemoryCount = 0;
  int indisplayCount = 0;
  int needsUpdateCount = 0;

  ChunkListNode* lruItem = m_LRUList.m_first;
  while (lruItem != NULL)
  {
    ChunkObj* oldest = lruItem->m_chunk;
    switch (oldest->m_status)
    {
      case CHUNK_UNLOADED:
        unloadedCount++;
        break;
      case CHUNK_INMEMORY:
        inmemoryCount++;
        break;
      case CHUNK_INDISPLAY:
        indisplayCount++;
        break;
      case CHUNK_NEEDSUPDATE:
        needsUpdateCount++;
        break;
    }

    lruItem = lruItem->m_next;
  }

  status.format("system memory = %gM (%d chunks), display memory = %gM (%d chunks), update = %d, LRU = %d chunks (%d unloaded)",
        m_systemMemUsed/(1024*1024.0), inmemoryCount,
        m_displayMemUsed/(1024*1024.0), indisplayCount,
        needsUpdateCount,
        m_LRUList.length(), unloadedCount); 

  m_chunkLock->unlock();
}

//--------------------------------------------------------------
// check system and display memory use 
void ChunkWorld::checkMemory()
{
  // drop things out of the LRU list until we're under the limits
//  if (m_displayMemUsed > m_displayMemLimit)
//    mgDebug("display memused = %gM, limit=%gM", m_displayMemUsed/(1024*1024.0), m_displayMemLimit/(1024*1024.0));
//  if (m_systemMemUsed > m_systemMemLimit)
//    mgDebug("system memused = %gM, limit=%gM", m_systemMemUsed/(1024*1024.0), m_systemMemLimit/(1024*1024.0));

  int i = 0;
  ChunkListNode* lruItem = m_LRUList.m_first;
  while (lruItem != NULL && 
         (m_displayMemUsed > m_displayMemLimit || m_systemMemUsed > m_systemMemLimit))
  {
    ChunkObj* chunk = lruItem->m_chunk;
    ChunkListNode* nextItem = lruItem->m_next;
    if (chunk->m_active)
    {
      lruItem = nextItem;
//      mgDebug("active(%d,%d) skipped", chunk->m_originX, chunk->m_originZ);
      i++;
      continue;
    }

    if (chunk->m_status == CHUNK_INDISPLAY ||
        chunk->m_status == CHUNK_NEEDSUPDATE)
    {
      // we're in the loop because we need more display or system memory.
      // in either case, have to unload from display (can't unload from memory
      // until we're unloaded from display)
      m_displayMemUsed -= chunk->getDisplayMemUsed();
//      mgDebug("delete buffers(%d,%d) mem=%gM", chunk->m_originX, chunk->m_originZ, m_displayMemUsed/(1024*1024.0));
      chunk->deleteBuffers();
      chunk->m_status = CHUNK_INMEMORY;
      m_chunksChanged = true;
    }

    if (chunk->m_status == CHUNK_INMEMORY)
    {
      // if need more system memory
      if (m_systemMemUsed > m_systemMemLimit)
      {
        m_systemMemUsed -= chunk->getSystemMemUsed();
        chunk->unloadMemory();
//        mgDebug("unload memory(%d,%d) mem=%gM", chunk->m_originX, chunk->m_originZ, m_systemMemUsed/(1024*1024.0));
        chunk->m_status = CHUNK_UNLOADED;
      }
    }

    if (chunk->m_status == CHUNK_UNLOADED)
    {
      // if chunk has been completely removed, don't keep in LRU list
//      mgDebug("removeList(%d,%d) at %d", chunk->m_originX, chunk->m_originZ, i);
      m_LRUList.removeNode(lruItem);
    }

    // advance to next item in list
    lruItem = nextItem;
    i++;
  }
}

//--------------------------------------------------------------
// add to request list
void ChunkWorld::requestChunk(
  ChunkObj* chunk)
{
  if (!chunk->m_requested)
  {
    chunk->m_requested = true;
    m_requestList.addToTail(chunk);
  }
}

//--------------------------------------------------------------
// reset the request list
void ChunkWorld::resetRequestList()
{
  // reset the request list.  if we are no longer looking at a chunk,
  // don't bother to load it.

  ChunkListNode* request = m_requestList.m_first;
  while (request != NULL)
  {
    request->m_chunk->m_requested = false;
    request = request->m_next;
  }

  m_requestList.removeAll();
}

//--------------------------------------------------------------
// take a request off the queue
ChunkObj* ChunkWorld::dequeueRequest()
{
  m_chunkLock->lock();

  // viewList is sorted nearest to farthest, and render traverses it
  // in reverse order.  animate generates requests for chunks farthest
  // to nearest.  We want to load nearest first, so take entries 
  // from end of request list.

  ChunkObj* found = NULL;
  ChunkListNode* requestItem = m_requestList.m_last;
  while (requestItem != NULL)
  {
    // find an inactive chunk
    ChunkObj* chunk = requestItem->m_chunk;
    if (!chunk->m_active)
    {
      // claim it
      m_requestList.removeNode(requestItem);
      chunk->m_active = true;
      found = chunk;
      break;
    }
    requestItem = requestItem->m_next;
  }
  m_chunkLock->unlock();
  return found;
}

//--------------------------------------------------------------
// process one chunk in request queue
void ChunkWorld::processRequest(
  ChunkObj* chunk)
{
  switch (chunk->m_status)
  {
    case CHUNK_UNLOADED: 
    {
//      mgDebug("chunk_unloaded on (%d,%d)", chunk->m_originX, chunk->m_originZ);
      // load from disk
      chunk->loadMemory(this);

      m_chunkLock->lock();

      // under lock, change status
      chunk->m_status = CHUNK_INMEMORY;
      chunk->m_active = false;
      chunk->m_requested = false;

      int size = chunk->getSystemMemUsed();
      m_systemMemUsed += size;

//      mgDebug("loadMemory(%d,%d) -- sysMem=%gM", chunk->m_originX, chunk->m_originZ, m_systemMemUsed/(1024*1024.0));

      m_chunkLock->unlock();
      break;
    }

    case CHUNK_INMEMORY:
    {
      // create buffers.  ready to send to display
      chunk->createBuffers(chunk->m_eyePt);

      m_chunkLock->lock();
      chunk->m_status = CHUNK_INDISPLAY;
      chunk->m_active = false;
      chunk->m_requested = false;

      int size = chunk->getDisplayMemUsed();
      m_displayMemUsed += size;

//      mgDebug("loadDisplay(%d,%d) -- displayMem=%gM", chunk->m_originX, chunk->m_originZ, m_displayMemUsed/(1024*1024.0));

      m_chunksChanged = true;

      m_chunkLock->unlock();
      break;
    }

    case CHUNK_INDISPLAY:
    {
//      mgDebug("chunk_indisplay ?? on (%d,%d)", chunk->m_originX, chunk->m_originZ);
      // shouldn't happen.  just reset flag
      m_chunkLock->lock();
      chunk->m_active = false;
      chunk->m_requested = false;
      m_chunkLock->unlock();
      break;
    }

    case CHUNK_NEEDSUPDATE:
    {
//      mgDebug("chunk_needsupdate on (%d,%d)", chunk->m_originX, chunk->m_originZ);
      chunk->createUpdate(chunk->m_eyePt);

      m_chunkLock->lock();
      chunk->m_active = false;
      chunk->m_requested = false;
      chunk->m_status = CHUNK_INDISPLAY;
      chunk->m_hasUpdate = true;
      m_chunksChanged = true;
      m_chunkLock->unlock();
      break;
    }
  }
}

//--------------------------------------------------------------
// load chunks from request list
void ChunkWorld::chunkLoader()
{
  while (!m_shutdown)
  {
    m_chunkEvent->wait(1000.0);  // check shutdown every second

    // while there's work
    while (!m_shutdown)
    {
      ChunkObj* found = dequeueRequest();

      // process the request
      if (found != NULL)
        processRequest(found);
      else break;  // back to wait
    }
  }
}

//--------------------------------------------------------------------
// assign an id to a texture
int ChunkWorld::findTexture(
  const char* fileName)
{
  const void* value;
  if (!m_textureIds.lookup(fileName, value))
  {
    int id = m_nextTextureId++;
    if (id == MAX_BRICK_TEXTURES)
      throw new mgException("max of %d textures exceeded.", MAX_BRICK_TEXTURES);

    value = (void*) id;
    m_textureIds.setAt(fileName, value);

    m_textureFiles.setAt(id, fileName);
  }

  return (intptr_t) value;
}

//--------------------------------------------------------------------
// read a brick set for the world
void ChunkWorld::readBrickSet(
  BrickSetFile& brickSetFile)
{
  m_brickSet = new BrickSet();

  // assign index to each texture used
  m_nextTextureId = 0;
  m_textureIds.removeAll();

  // add an entry for air
  BrickDefn* desc = new BrickDefn();
  desc->m_trans[BRICK_FACE_XMIN] = true;
  desc->m_trans[BRICK_FACE_XMAX] = true;
  desc->m_trans[BRICK_FACE_YMIN] = true;
  desc->m_trans[BRICK_FACE_YMAX] = true;
  desc->m_trans[BRICK_FACE_ZMIN] = true;
  desc->m_trans[BRICK_FACE_ZMAX] = true;
  m_brickSet->m_defns[0] = desc;
  m_brickSet->m_defns[0]->m_subDefns[0] = new BrickSubDefn();

  // for each brick type
  for (int i = 0; i < brickSetFile.m_brickTags.length(); i++)
  {
    BrickTag* brick = (BrickTag*) brickSetFile.m_brickTags[i];

    // get existing defn, if any, for this major code
    desc = m_brickSet->m_defns[brick->m_majorCode];
    if (desc == NULL)
    {
      desc = new BrickDefn();
      m_brickSet->m_defns[brick->m_majorCode] = desc;
    }

    // get entry for minor code
    if (brick->m_minorCode < 0 || brick->m_minorCode >= SUBDEFN_COUNT)
      throw new mgException("Invalid brick defn minor code %02x%02x", 
        brick->m_majorCode, brick->m_minorCode);

    // don't allow duplicates
    BrickSubDefn* subDefn = desc->m_subDefns[brick->m_minorCode];
    if (subDefn != NULL)
      throw new mgException("Duplicate brick defn code %02x%02x", 
        brick->m_majorCode, brick->m_minorCode);

    // create new subdefinition
    subDefn = new BrickSubDefn();
    desc->m_subDefns[brick->m_minorCode] = subDefn;

    int shape;
    if (brick->m_shape.equalsIgnoreCase("slab"))
      shape = SHAPE_SLAB;
    else if (brick->m_shape.equalsIgnoreCase("column"))
      shape = SHAPE_COLUMN;
    else if (brick->m_shape.equalsIgnoreCase("cap"))
      shape = SHAPE_CAP;
    else if (brick->m_shape.equalsIgnoreCase("stair"))
      shape = SHAPE_STAIR;
    else if (brick->m_shape.equalsIgnoreCase("cube"))
      shape = SHAPE_CUBE;

    else shape = SHAPE_DEFN;

    // minor code 0 sets shape and transparency of brick
    if (brick->m_minorCode == 0)
    {
      desc->m_shape = shape;
      switch (desc->m_shape)
      {
        case SHAPE_CUBE:
          desc->m_trans[BRICK_FACE_XMIN] = brick->m_xminTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_XMAX] = brick->m_xmaxTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_YMIN] = brick->m_yminTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_YMAX] = brick->m_ymaxTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_ZMIN] = brick->m_zminTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_ZMAX] = brick->m_zmaxTexture.find(0, ';') != -1;
          break;

        case SHAPE_SLAB:
          desc->m_trans[BRICK_FACE_XMIN] = true;
          desc->m_trans[BRICK_FACE_XMAX] = true;
          desc->m_trans[BRICK_FACE_YMIN] = brick->m_yminTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_YMAX] = brick->m_ymaxTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_ZMIN] = true;
          desc->m_trans[BRICK_FACE_ZMAX] = true;
          break;
        
        case SHAPE_CAP:
          desc->m_trans[BRICK_FACE_XMIN] = true;
          desc->m_trans[BRICK_FACE_XMAX] = true;
          desc->m_trans[BRICK_FACE_YMIN] = brick->m_yminTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_YMAX] = brick->m_ymaxTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_ZMIN] = true;
          desc->m_trans[BRICK_FACE_ZMAX] = true;
          break;

        case SHAPE_STAIR:
          desc->m_trans[BRICK_FACE_XMIN] = true;
          desc->m_trans[BRICK_FACE_XMAX] = true;
          desc->m_trans[BRICK_FACE_YMIN] = brick->m_yminTexture.find(0, ';') != -1;
          desc->m_trans[BRICK_FACE_YMAX] = true;
          desc->m_trans[BRICK_FACE_ZMIN] = true;
          desc->m_trans[BRICK_FACE_ZMAX] = true;
          break;

        case SHAPE_COLUMN:
          desc->m_trans[BRICK_FACE_XMIN] = true;
          desc->m_trans[BRICK_FACE_XMAX] = true;
          desc->m_trans[BRICK_FACE_YMIN] = true;
          desc->m_trans[BRICK_FACE_YMAX] = true;
          desc->m_trans[BRICK_FACE_ZMIN] = true;
          desc->m_trans[BRICK_FACE_ZMAX] = true;
          break;
      
        case SHAPE_DEFN:
          desc->m_trans[BRICK_FACE_XMIN] = true;
          desc->m_trans[BRICK_FACE_XMAX] = true;
          desc->m_trans[BRICK_FACE_YMIN] = true;
          desc->m_trans[BRICK_FACE_YMAX] = true;
          desc->m_trans[BRICK_FACE_ZMIN] = true;
          desc->m_trans[BRICK_FACE_ZMAX] = true;
          break;
      }

      // set summary flags
      desc->m_hasTransparent = false;
      desc->m_hasOpaque = false;
    
      for (int j = 0; j < 6; j++)
      {
        if (desc->m_trans[j])
          desc->m_hasTransparent = true;
        else desc->m_hasOpaque = true;
      }
    }

    // find all the textures specified
    if (!brick->m_xminTexture.isEmpty())
      subDefn->m_xmin = findTexture(brick->m_xminTexture);
    if (!brick->m_xmaxTexture.isEmpty())
      subDefn->m_xmax = findTexture(brick->m_xmaxTexture);
    if (!brick->m_yminTexture.isEmpty())
      subDefn->m_ymin = findTexture(brick->m_yminTexture);
    if (!brick->m_ymaxTexture.isEmpty())
      subDefn->m_ymax = findTexture(brick->m_ymaxTexture);
    if (!brick->m_zminTexture.isEmpty())
      subDefn->m_zmin = findTexture(brick->m_zminTexture);
    if (!brick->m_zmaxTexture.isEmpty())
      subDefn->m_zmax = findTexture(brick->m_zmaxTexture);

    if (brick->m_dir < 0 || brick->m_dir >= DIRECTION_COUNT)
      throw new mgException("Invalid direction on brick defn code %02x%02x", 
                brick->m_majorCode, brick->m_minorCode);

    if (shape == SHAPE_DEFN)
      subDefn->m_shapeDefn = readShape(brick->m_shape);

    subDefn->m_dir = brick->m_dir;
    subDefn->m_light = brick->m_light;
    if (brick->m_light)
      mgDebug("light on brick code %02x%02x", brick->m_majorCode, brick->m_minorCode);
  }

  // combine minor and major definitions, check for missing info
  checkBrickSet();
}

//--------------------------------------------------------------------
// combine minor and major definitions, check for missing info
void ChunkWorld::checkBrickSet()
{
  for (int i = 0; i < DEFN_COUNT; i++)
  {
    BrickDefn* defn = m_brickSet->m_defns[i];
    if (defn == NULL)
      continue;

    // there must at least be a subdefn 0
    BrickSubDefn* subDefn0 = defn->m_subDefns[0];
    if (subDefn0 == NULL)
      throw new mgException("no definition for code %04x", i*256);

    for (int j = 0; j < SUBDEFN_COUNT; j++)
    {
      BrickSubDefn* subDefn = defn->m_subDefns[j];
      if (subDefn == NULL)
        continue;

      // textures default to defn 0 textures
      if (subDefn->m_xmin == -1)
        subDefn->m_xmin = subDefn0->m_xmin;
      if (subDefn->m_xmax == -1)
        subDefn->m_xmax = subDefn0->m_xmax;
      if (subDefn->m_ymin == -1)
        subDefn->m_ymin = subDefn0->m_ymin;
      if (subDefn->m_ymax == -1)
        subDefn->m_ymax = subDefn0->m_ymax;
      if (subDefn->m_zmin == -1)
        subDefn->m_zmin = subDefn0->m_zmin;
      if (subDefn->m_zmax == -1)
        subDefn->m_zmax = subDefn0->m_zmax;

      // shape defaults to defn0 shape
      if (subDefn->m_shapeDefn == NULL)
        subDefn->m_shapeDefn = subDefn0->m_shapeDefn;

      // if 0 is lit, they all are
      if (subDefn0->m_light)
        subDefn->m_light = true;
    }
  }
}

//--------------------------------------------------------------------
// read a shape file and convert to ShapeDefn
ShapeDefn* ChunkWorld::readShape(
  const char* fileName)
{
  const void* value;
  if (m_shapes.lookup(fileName, value))
    return (ShapeDefn*) value;

  ShapeFile shapeFile(fileName);

  // convert into a shape defn
  ShapeDefn* shape = new ShapeDefn();
  m_shapes.setAt(fileName, shape);

  // get count of vertexes
  int vertexCount = 0;
  for (int i = 0; i < shapeFile.m_triangles.length(); i++)
  {
    TrianglesTag* tag = (TrianglesTag*) shapeFile.m_triangles[i];
    int inputCount = tag->m_ambientID.isEmpty() ? 3 : 4;
    vertexCount += tag->m_len/inputCount;   // indexes per triangle
  }

  // allocate vertex array for shape
  shape->m_len = vertexCount;
  shape->m_vertexes = new ShapePoint[shape->m_len];

  // copy the vertexes
  int posn = 0;
  for (int i = 0; i < shapeFile.m_triangles.length(); i++)
  {
    TrianglesTag* tag = (TrianglesTag*) shapeFile.m_triangles[i];
    int inputCount = tag->m_ambientID.isEmpty() ? 3 : 4;

    // find the arrays
    if (!shapeFile.m_arrays.lookup(tag->m_positionsID, value))
      continue;
    FloatArrayTag* positions = (FloatArrayTag*) value;

    if (!shapeFile.m_arrays.lookup(tag->m_normalsID, value))
      continue;
    FloatArrayTag* normals = (FloatArrayTag*) value;

    if (!shapeFile.m_arrays.lookup(tag->m_texcoordsID, value))
      continue;
    FloatArrayTag* texcoords = (FloatArrayTag*) value;

    FloatArrayTag* ambient = NULL;
    if (shapeFile.m_arrays.lookup(tag->m_ambientID, value))
      ambient = (FloatArrayTag*) value;

    // scale to 1 unit cube
    double xsize = 1.0/(shapeFile.m_xmax - shapeFile.m_xmin);
    double ysize = 1.0/(shapeFile.m_ymax - shapeFile.m_ymin);
    double zsize = 1.0/(shapeFile.m_zmax - shapeFile.m_zmin);

    for (int j = 0; j < tag->m_len; j+= inputCount)
    {
      int positionIndex = 3*tag->m_values[j];
      int normalIndex = 3*tag->m_values[j+1];
      int texcoordIndex = 2*tag->m_values[j+2];
      int ambientIndex = -1;
      if (inputCount == 4)
        ambientIndex = tag->m_values[j+3];

      ShapePoint* point = &shape->m_vertexes[posn];
      point->m_pt.x = positions->m_values[positionIndex];
      point->m_pt.y = positions->m_values[positionIndex+1];
      point->m_pt.z = positions->m_values[positionIndex+2];

      // adjust position and scale to place shape in unit cube
      point->m_pt.x = (point->m_pt.x - shapeFile.m_xmin) * xsize;
      point->m_pt.y = (point->m_pt.y - shapeFile.m_ymin) * ysize;
      point->m_pt.z = (point->m_pt.z - shapeFile.m_zmin) * zsize;

      point->m_normal.x = normals->m_values[normalIndex];
      point->m_normal.y = normals->m_values[normalIndex+1];
      point->m_normal.z = normals->m_values[normalIndex+2];

      point->m_texcoord.x = texcoords->m_values[texcoordIndex];
      point->m_texcoord.y = texcoords->m_values[texcoordIndex+1];
      point->m_texcoord.z = tag->m_material;

      if (ambientIndex != -1)
        point->m_ambient = ambient->m_values[ambientIndex];
      else point->m_ambient = 1.0;

      posn++;  // next vertex
    }
  }

  return shape;
}

//--------------------------------------------------------------------
// return a minecraft region
MinecraftRegion* ChunkWorld::getRegion(
  int regionX,
  int regionZ)
{
  // this will be called from ChunkObj::loadMemory.  get chunk lock to 
  // support multiple threads
  m_chunkLock->lock();

  // if region seen, return it
  const void* value;
  MinecraftRegion* region = NULL;
  if (m_regionTable.lookup(regionX, 0, regionZ, value))
    region = (MinecraftRegion*) value;
  else
  {
    region = new MinecraftRegion(m_regionDir, regionX, regionZ);
    m_regionTable.setAt(regionX, 0, regionZ, region);

    // trim the number of open region files
    while (m_openRegions.length() > MAX_OPEN_REGIONS)
    {
      MinecraftRegion* openRegion = (MinecraftRegion*) m_openRegions.pop();
      openRegion->closeFile();
    }
  }

  // bring to top of open region list
  int index = m_openRegions.find(region);
  if (index != -1)
    m_openRegions.removeAt(index);
  m_openRegions.insertAt(0, region);

  m_chunkLock->unlock();
  return region;
}

//--------------------------------------------------------------------
// get a chunk of the world
ChunkObj* ChunkWorld::getChunk(
  int chunkX,
  int chunkZ)
{
  const void* value;
  if (!m_chunkTable.lookup(chunkX, 0, chunkZ, value))
    return NULL;
  return (ChunkObj*) value;
}

//--------------------------------------------------------------------
// find/create a chunk of the world
ChunkObj* ChunkWorld::createChunk(
  int chunkX,
  int chunkZ)
{
  ChunkObj* chunk = NULL;
  const void* value;
  if (!m_chunkTable.lookup(chunkX, 0, chunkZ, value))
  {
    // create a new chunk
    chunk = new ChunkObj(chunkX, chunkZ, CHUNK_UNLOADED);
    m_chunkTable.setAt(chunkX, 0, chunkZ, chunk);
  }
  else chunk = (ChunkObj*) value;

  return chunk;
}

//--------------------------------------------------------------------
// return true if all neighbors in memory
BOOL ChunkWorld::loadNeighbors(
  ChunkObj* chunk)
{
  ChunkObj* xmin = createChunk(chunk->m_originX - CHUNK_SIZE, chunk->m_originZ);
  if (xmin->m_status == CHUNK_UNLOADED)
    requestChunk(xmin);

  ChunkObj* xmax = createChunk(chunk->m_originX + CHUNK_SIZE, chunk->m_originZ);
  if (xmax->m_status == CHUNK_UNLOADED)
    requestChunk(xmax);

  ChunkObj* zmin = createChunk(chunk->m_originX, chunk->m_originZ - CHUNK_SIZE);
  if (zmin->m_status == CHUNK_UNLOADED)
    requestChunk(zmin);

  ChunkObj* zmax = createChunk(chunk->m_originX, chunk->m_originZ + CHUNK_SIZE);
  if (zmax->m_status == CHUNK_UNLOADED)
    requestChunk(zmax);

  if (xmin->m_status == CHUNK_UNLOADED ||
      xmax->m_status == CHUNK_UNLOADED ||
      zmin->m_status == CHUNK_UNLOADED ||
      zmax->m_status == CHUNK_UNLOADED)
    return false;

  return true;
}

//--------------------------------------------------------------------
// load edges from neighbors
void ChunkWorld::updateEdges(
  ChunkObj* chunk)
{
  if (chunk->m_blobs[0] == NULL)
    return;  // empty cell

  ChunkObj* other;
  other = getChunk(chunk->m_originX - CHUNK_SIZE, chunk->m_originZ);
  if (other != NULL && other->m_blobs[0] != NULL)
  {
    for (int i = 0; i < CHUNK_COLUMN; i++)
      chunk->m_blobs[i]->updateXMinEdge(other->m_blobs[i]);
  }

  other = getChunk(chunk->m_originX + CHUNK_SIZE, chunk->m_originZ);
  if (other != NULL && other->m_blobs[0] != NULL)
  {
    for (int i = 0; i < CHUNK_COLUMN; i++)
      chunk->m_blobs[i]->updateXMaxEdge(other->m_blobs[i]);
  }

  other = getChunk(chunk->m_originX, chunk->m_originZ - CHUNK_SIZE);
  if (other != NULL && other->m_blobs[0] != NULL)
  {
    for (int i = 0; i < CHUNK_COLUMN; i++)
      chunk->m_blobs[i]->updateZMinEdge(other->m_blobs[i]);
  }

  other = getChunk(chunk->m_originX, chunk->m_originZ + CHUNK_SIZE);
  if (other != NULL && other->m_blobs[0] != NULL)
  {
    for (int i = 0; i < CHUNK_COLUMN; i++)
      chunk->m_blobs[i]->updateZMaxEdge(other->m_blobs[i]);
  }
}

//--------------------------------------------------------------
// create water vertexes
void ChunkWorld::createWater()
{
  m_waterVertexes = mgVertexTA::newBuffer(6);

  mgVertexTA tl, tr, bl, br;

  int waterTexture = m_brickSet->m_defns[0x09]->m_subDefns[0]->m_ymax;

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  // build left edge
  tl.setPoint(0, 64, CHUNK_SIZE);
  tr.setPoint(CHUNK_SIZE, 64, CHUNK_SIZE);
  bl.setPoint(0, 64, 0);
  br.setPoint(CHUNK_SIZE, 64, 0);

  tl.setTexture(tl.m_px, tl.m_pz, waterTexture);
  tr.setTexture(tr.m_px, tr.m_pz, waterTexture);
  bl.setTexture(bl.m_px, bl.m_pz, waterTexture);
  br.setTexture(br.m_px, br.m_pz, waterTexture);

  tl.addTo(m_waterVertexes);
  tr.addTo(m_waterVertexes);
  bl.addTo(m_waterVertexes);
  bl.addTo(m_waterVertexes);
  tr.addTo(m_waterVertexes);
  br.addTo(m_waterVertexes);
}

//--------------------------------------------------------------
// create water vertexes
void ChunkWorld::createHorizon()
{
  // figure number of chunk-sized cubes in distance
  int cubeDistance = (m_viewDistance+CHUNK_SIZE-1)/CHUNK_SIZE;
  cubeDistance *= CHUNK_SIZE;
  int worldMin = -cubeDistance;
  int worldMax = cubeDistance + CHUNK_SIZE;
  m_horizonVertexes = mgVertexTA::newBuffer(4*6);

  mgVertexTA tl, tr, bl, br;

  int waterTexture = m_brickSet->m_defns[0x09]->m_subDefns[0]->m_ymax;

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  // build left edge
  tl.setPoint(-HORIZON_DIST, 64, HORIZON_DIST);
  tr.setPoint(worldMin, 64, HORIZON_DIST);
  bl.setPoint(-HORIZON_DIST, 64, -HORIZON_DIST);
  br.setPoint(worldMin, 64, -HORIZON_DIST);

  tl.setTexture(tl.m_px, tl.m_pz, waterTexture);
  tr.setTexture(tr.m_px, tr.m_pz, waterTexture);
  bl.setTexture(bl.m_px, bl.m_pz, waterTexture);
  br.setTexture(br.m_px, br.m_pz, waterTexture);

  tl.addTo(m_horizonVertexes);
  tr.addTo(m_horizonVertexes);
  bl.addTo(m_horizonVertexes);
  bl.addTo(m_horizonVertexes);
  tr.addTo(m_horizonVertexes);
  br.addTo(m_horizonVertexes);

  // build right edge
  tl.setPoint(worldMax, 64, HORIZON_DIST);
  tr.setPoint(HORIZON_DIST, 64, HORIZON_DIST);
  bl.setPoint(worldMax, 64, -HORIZON_DIST);
  br.setPoint(HORIZON_DIST, 64, -HORIZON_DIST);

  tl.setTexture(tl.m_px, tl.m_pz, waterTexture);
  tr.setTexture(tr.m_px, tr.m_pz, waterTexture);
  bl.setTexture(bl.m_px, bl.m_pz, waterTexture);
  br.setTexture(br.m_px, br.m_pz, waterTexture);

  tl.addTo(m_horizonVertexes);
  tr.addTo(m_horizonVertexes);
  bl.addTo(m_horizonVertexes);
  bl.addTo(m_horizonVertexes);
  tr.addTo(m_horizonVertexes);
  br.addTo(m_horizonVertexes);

  // build top edge
  tl.setPoint(worldMin, 64, HORIZON_DIST);
  tr.setPoint(worldMax, 64, HORIZON_DIST);
  bl.setPoint(worldMin, 64, worldMax);
  br.setPoint(worldMax, 64, worldMax);

  tl.setTexture(tl.m_px, tl.m_pz, waterTexture);
  tr.setTexture(tr.m_px, tr.m_pz, waterTexture);
  bl.setTexture(bl.m_px, bl.m_pz, waterTexture);
  br.setTexture(br.m_px, br.m_pz, waterTexture);

  tl.addTo(m_horizonVertexes);
  tr.addTo(m_horizonVertexes);
  bl.addTo(m_horizonVertexes);
  bl.addTo(m_horizonVertexes);
  tr.addTo(m_horizonVertexes);
  br.addTo(m_horizonVertexes);

  // build bottom edge
  tl.setPoint(worldMin, 64, worldMin);
  tr.setPoint(worldMax, 64, worldMin);
  bl.setPoint(worldMin, 64, -HORIZON_DIST);
  br.setPoint(worldMax, 64, -HORIZON_DIST);

  tl.setTexture(tl.m_px, tl.m_pz, waterTexture);
  tr.setTexture(tr.m_px, tr.m_pz, waterTexture);
  bl.setTexture(bl.m_px, bl.m_pz, waterTexture);
  br.setTexture(br.m_px, br.m_pz, waterTexture);

  tl.addTo(m_horizonVertexes);
  tr.addTo(m_horizonVertexes);
  bl.addTo(m_horizonVertexes);
  bl.addTo(m_horizonVertexes);
  tr.addTo(m_horizonVertexes);
  br.addTo(m_horizonVertexes);
}

//--------------------------------------------------------------
// draw horizon
void ChunkWorld::drawHorizon(
  int viewX,
  int viewZ)
{
  mgMatrix4 model;
  model.translate(viewX*CHUNK_SIZE, 0, viewZ*CHUNK_SIZE);
  mgDisplay->setModelTransform(model);
  mgDisplay->setLightColor(m_skyColor.x, m_skyColor.y, m_skyColor.z);

  mgDisplay->setShader("horizon");
  mgDisplay->setShaderUniform("horizon", "fogColor", m_fogColor);
  mgDisplay->setShaderUniform("horizon", "fogBotHeight", (float) m_fogBotHeight);
  mgDisplay->setShaderUniform("horizon", "fogBotInten", (float) m_fogBotInten);
  mgDisplay->setShaderUniform("horizon", "fogTopHeight", (float) m_fogTopHeight);
  mgDisplay->setShaderUniform("horizon", "fogTopInten", (float) m_fogTopInten);
  mgDisplay->setShaderUniform("horizon", "fogMaxDist", (float) m_fogMaxDist);

  mgDisplay->setTexture(m_waterTexture);
  mgDisplay->draw(MG_TRIANGLES, m_horizonVertexes);
}

//--------------------------------------------------------------
// draw water as standin for chunk
void ChunkWorld::drawWater(
  int chunkX,
  int chunkZ)
{
  mgMatrix4 model;
  model.translate(chunkX, 0, chunkZ);
  mgDisplay->setModelTransform(model);
  mgDisplay->setLightColor(m_skyColor.x, m_skyColor.y, m_skyColor.z);

  mgDisplay->setShader("horizon");
  mgDisplay->setShaderUniform("horizon", "fogColor", m_fogColor);
  mgDisplay->setShaderUniform("horizon", "fogBotHeight", (float) m_fogBotHeight);
  mgDisplay->setShaderUniform("horizon", "fogBotInten", (float) m_fogBotInten);
  mgDisplay->setShaderUniform("horizon", "fogTopHeight", (float) m_fogTopHeight);
  mgDisplay->setShaderUniform("horizon", "fogTopInten", (float) m_fogTopInten);
  mgDisplay->setShaderUniform("horizon", "fogMaxDist", (float) m_fogMaxDist);

  mgDisplay->setTexture(m_waterTexture);
  mgDisplay->draw(MG_TRIANGLES, m_waterVertexes);
}

//--------------------------------------------------------------
// find the water texture in the brick set
void ChunkWorld::loadWaterTexture(
  BrickSetFile& brickSetFile)
{
  m_waterTexture = NULL;

  for (int i = 0; i < brickSetFile.m_brickTags.length(); i++)
  {
    BrickTag* tag = (BrickTag*) brickSetFile.m_brickTags[i];
    if (tag->m_majorCode == 0x09)
    {
      m_waterTexture = mgDisplay->loadTexture(tag->m_ymaxTexture);
      break;
    }
  }
  if (m_waterTexture == NULL)
    throw new mgException("Could not find water texture (code 0x09) in brick set");
}

//--------------------------------------------------------------
// return triangle count for current scene
int ChunkWorld::triangleCount()
{
  int count = 0;
  m_chunkLock->lock();

  int posn = m_chunkTable.getStartPosition();
  while (posn != -1)
  {
    int x, y, z;
    const void* value;
    m_chunkTable.getNextAssoc(posn, x, y, z, value);
    ChunkObj* chunk = (ChunkObj*) value;

    // if chunk loaded and within view, render it
    if (chunk->m_status == CHUNK_INDISPLAY ||
        chunk->m_status == CHUNK_NEEDSUPDATE)
      count += chunk->triangleCount();
  }

  m_chunkLock->unlock();
  return count;
}

