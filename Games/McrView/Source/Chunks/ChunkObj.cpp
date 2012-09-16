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

#include "Bricks/BrickSet.h"
#include "Bricks/BrickBlobInt.h"
#include "Bricks/BrickBlobFloat.h"

#include "Minecraft/MinecraftChunk.h"
#include "Minecraft/MinecraftRegion.h"

#include "ChunkWorld.h"
#include "ChunkObj.h"

const double CREATE_DURATION = 750.0;  // milliseconds

//--------------------------------------------------------------
// constructor
ChunkObj::ChunkObj(
  int x,
  int z,
  int status)
{
  m_originX = x;
  m_originZ = z;
  m_status = status;
  m_active = false;
  m_hasUpdate = false;
  m_requested = false;

  for (int i = 0; i < CHUNK_COLUMN; i++)
    m_blobs[i] = NULL;

  m_createTime = 0.0;  // set at first animate
  m_alpha = 0.0;
}

//--------------------------------------------------------------
// destructor
ChunkObj::~ChunkObj()
{
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    delete m_blobs[i];
    m_blobs[i] = NULL;
  }
}

//--------------------------------------------------------------
// load the chunk file
void ChunkObj::loadMemory(
  ChunkWorld* world)
{
//#define DEBUG_TRANS
#ifdef DEBUG_TRANS
  // debug
  if (m_originX == 0 && m_originZ == 0)
  {
    BrickBlob* blob;
    for (int i = 0; i < CHUNK_COLUMN; i++)
    {
      if (mgDisplay->supportsCompressedVertex())
        blob = new BrickBlobInt();
      else blob = new BrickBlobFloat();

      blob->setBrickSet(world->m_textureArray, world->m_brickSet);
  //    blob->setSelectedTexture(world->m_selectTexture);

      m_blobs[i] = blob;
    }
    blob = m_blobs[0];
    for (int x = 0; x < 32; x++)
      for (int y = 0; y < 32; y++)
        for (int z = 0; z < 32; z++)
          blob->setBrick(x, y, z, (x%4 == 2) && (y%4 == 2) && (z%4 == 2) != 0 ? 0x0900: 0x1400);

    return;
  }
  else return;
#endif

//#define DEBUG_LIGHT
#ifdef DEBUG_LIGHT
  // debug
  if (m_originX == 0 && m_originZ == 0)
  {
    BrickBlob* blob;
    for (int i = 0; i < CHUNK_COLUMN; i++)
    {
      if (mgDisplay->supportsCompressedVertex())
        blob = new BrickBlobInt();
      else blob = new BrickBlobFloat();

      blob->setBrickSet(world->m_textureArray, world->m_brickSet);
  //    blob->setSelectedTexture(world->m_selectTexture);

      m_blobs[i] = blob;
    }
    blob = m_blobs[3];

    for (int x = 10; x < 21; x++)
    {
      blob->setBrick(x, 10, 10, 0x5000);
      blob->setBrick(x, 10, 11, 0x5000);
    }

/*
    for (int x = 10; x < 21; x++)
      for (int y = 10; y < 21; y++)
        for (int z = 10; z < 21; z++)
        {
          blob->setBrick(x, y, z, 0x5000);
        }

    for (int x = 9; x < 22; x++)
      for (int y = 14; y < 17; y++)
        for (int z = 9; z < 22; z++)
        {
          blob->setBrick(x, y, z, 0x5000);
        }

    for (int x = 14; x < 17; x++)
      for (int y = 9; y < 22; y++)
        for (int z = 9; z < 22; z++)
        {
          blob->setBrick(x, y, z, 0x5000);
        }

    for (int x = 9; x < 22; x++)
      for (int y = 9; y < 22; y++)
        for (int z = 14; z < 17; z++)
        {
          blob->setBrick(x, y, z, 0x5000);
        }

    blob->setBrick(9, 17, 10, 0x5000);

    blob->setBrick(9, 10, 10, 0x230B);
*/
    return;
  }
  else return;
#endif

//#define DEBUG_RAYS
#ifdef DEBUG_RAYS
  // debug
  if (m_originX == 0 && m_originZ == 0)
  {
    BrickBlob* blob;
    for (int i = 0; i < CHUNK_COLUMN; i++)
    {
      if (mgDisplay->supportsCompressedVertex())
        blob = new BrickBlobInt();
      else blob = new BrickBlobFloat();

      blob->setBrickSet(world->m_textureArray, world->m_brickSet);
  //    blob->setSelectedTexture(world->m_selectTexture);

      m_blobs[i] = blob;
    }
    blob = m_blobs[3];

    int x = 15;
    int y = 15;
    int z = 15;
    blob->setBrick(x-1, y, z, 0x230A);
    for (int dy = -1; dy < 1; dy++)
    {
      for (int dz = -1; dz < 1; dz++)
      {
        // if the inner blocks around origin are present, ray is blocked
        blob->setBrick(x, y+dy, z+dz, 0x230C);

        int ly, hy;
        if (dy < 0)
        {
          ly = y-1;
          hy = y-2;
        }
        else
        {
          ly = y;
          hy = y+1;
        }
        int lz, hz;
        if (dz < 0)
        {
          lz = z-1;
          hz = z-2;
        }
        else
        {
          lz = z;
          hz = z+1;
        }

        // check outer bricks
        blob->setBrick(x, ly, hz, 0x1400);
        blob->setBrick(x, hy, lz, 0x1400);
        blob->setBrick(x, hy, hz, 0x1400);
        blob->setBrick(x+1, ly, lz, 0x1400);
        blob->setBrick(x+1, ly, hz, 0x1400);
        blob->setBrick(x+1, hy, lz, 0x1400);
        blob->setBrick(x+1, hy, hz, 0x1400);
      }
    }
    return;
  }
  else return;
#endif

  // convert chunk x,z to minecraft chunk indexes.
  // do this in floating point to get negative numbers handled correctly.
  // negate X, since Minecraft x axis is reversed from ours
  int chunkX = (int) floor(-m_originX / 16.0);
  int chunkZ = (int) floor(m_originZ / 16.0);

  // find region from chunk
  int regionX = (int) floor(chunkX / (double) REGION_SIZE);
  int regionZ = (int) floor(chunkZ / (double) REGION_SIZE);

  // get the region from the world
  MinecraftRegion* region = world->getRegion(regionX, regionZ);

  // if not found, this area is not in world.  will be flagged in-memory, with NULL blobs
  if (region == NULL)
    return;

  // create column of brick blobs 32 by 32 by 128
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    BrickBlob* blob;
    if (mgDisplay->supportsIntegerVertex())
      blob = new BrickBlobInt();
    else blob = new BrickBlobFloat();

    blob->setBrickSet(world->m_textureArray, world->m_brickSet);
//    blob->setSelectedTexture(world->m_selectTexture);
//    blob->m_position = mgPoint3(m_originX, i*CHUNK_SIZE, m_originZ);

    m_blobs[i] = blob;
  }

  // calculate offset within region of chunk.
  chunkX -= regionX * REGION_SIZE;
  chunkZ -= regionZ * REGION_SIZE;

  // load four Minecraft chunks, 16 by 16 by 128.  BrickBlobs are actually
  // 34 on a side and duplicate the edge values of neighbors.  This allows
  // transparency to be calculated correctly without asking neighbors.
  // copy from Minecraft duplicates interior values.
  int chunkPosn, chunkLen, compressCode;
  MinecraftChunk chunk;
  if (region->getChunkLoc(chunkX+1, chunkZ, chunkPosn, chunkLen, compressCode))
  {
    chunk.parseRegion(region, chunkPosn, chunkLen);
    copyMinecraftBlocks(world, chunk, 0,  0, 0, m_blobs[0], 0,  0, 0, 16, 33, 16);
    copyMinecraftBlocks(world, chunk, 0, 31, 0, m_blobs[1], 0, -1, 0, 16, 34, 16);
    copyMinecraftBlocks(world, chunk, 0, 63, 0, m_blobs[2], 0, -1, 0, 16, 34, 16);
    copyMinecraftBlocks(world, chunk, 0, 95, 0, m_blobs[3], 0, -1, 0, 16, 33, 16);
    chunk.reset();
  }

  if (region->getChunkLoc(chunkX, chunkZ, chunkPosn, chunkLen, compressCode))
  {
    chunk.parseRegion(region, chunkPosn, chunkLen);
    copyMinecraftBlocks(world, chunk, 0,  0, 0, m_blobs[0], 16,  0, 0, 16, 33, 16);
    copyMinecraftBlocks(world, chunk, 0, 31, 0, m_blobs[1], 16, -1, 0, 16, 34, 16);
    copyMinecraftBlocks(world, chunk, 0, 63, 0, m_blobs[2], 16, -1, 0, 16, 34, 16);
    copyMinecraftBlocks(world, chunk, 0, 95, 0, m_blobs[3], 16, -1, 0, 16, 33, 16);
    chunk.reset();
  }

  if (region->getChunkLoc(chunkX+1, chunkZ+1, chunkPosn, chunkLen, compressCode))
  {
    chunk.parseRegion(region, chunkPosn, chunkLen);
    copyMinecraftBlocks(world, chunk, 0, 0,  0, m_blobs[0], 0,  0, 16, 16, 33, 16);
    copyMinecraftBlocks(world, chunk, 0, 31, 0, m_blobs[1], 0, -1, 16, 16, 34, 16);
    copyMinecraftBlocks(world, chunk, 0, 63, 0, m_blobs[2], 0, -1, 16, 16, 34, 16);
    copyMinecraftBlocks(world, chunk, 0, 95, 0, m_blobs[3], 0, -1, 16, 16, 33, 16);
    chunk.reset();
  }

  if (region->getChunkLoc(chunkX, chunkZ+1, chunkPosn, chunkLen, compressCode))
  {
    chunk.parseRegion(region, chunkPosn, chunkLen);
    copyMinecraftBlocks(world, chunk, 0, 0,  0, m_blobs[0], 16,  0, 16, 16, 33, 16);
    copyMinecraftBlocks(world, chunk, 0, 31, 0, m_blobs[1], 16, -1, 16, 16, 34, 16);
    copyMinecraftBlocks(world, chunk, 0, 63, 0, m_blobs[2], 16, -1, 16, 16, 34, 16);
    copyMinecraftBlocks(world, chunk, 0, 95, 0, m_blobs[3], 16, -1, 16, 16, 33, 16);
    chunk.reset();
  }
}

//--------------------------------------------------------------
// copy and translate mincraft chunk blocks
void ChunkObj::copyMinecraftBlocks(
  ChunkWorld* world,
  MinecraftChunk& chunk,
  int sourceX,
  int sourceY,
  int sourceZ,
  BrickBlob* blob,
  int targetX,
  int targetY,
  int targetZ,
  int xlen,
  int ylen,
  int zlen)
{
  for (int x = 0; x < xlen; x++)
  {
    for (int y = 0; y < ylen; y++)
    {
      for (int z = 0; z < zlen; z++)
      {
        // negate X, since minecraft x is reversed from ours
        BYTE blockType = chunk.getBlock(15-(sourceX+x), sourceY+y, sourceZ+z);
        WORD outType = blockType;
        if (world->m_brickSet->m_defns[outType] == NULL)
          outType = 0xFF00;
        else
        {
          BYTE modifier = chunk.getModifier(15-(sourceX+x), sourceY+y, sourceZ+z);

          // give plants an orientation
          if (blockType >= 0x25 && blockType <= 0x28)
            modifier = rand()%4;

          if (blockType == 0x1F)
            modifier += 4*(rand()%4);

          outType = (outType << 8) | modifier;
        }

        blob->setBrick(targetX + x, targetY + y, targetZ + z, outType);

        int skyInten, blockInten;
        chunk.getLights(15-(sourceX+x), sourceY+y, sourceZ+z, skyInten, blockInten);
        blob->setLight(targetX + x, targetY + y, targetZ + z, (skyInten << 4) | blockInten);
      }
    }
  }
}  

//--------------------------------------------------------------
// unload the chunk from memory
void ChunkObj::unloadMemory()
{
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    delete m_blobs[i];
    m_blobs[i] = NULL;
  }
}

//--------------------------------------------------------------
// create display buffers
void ChunkObj::createBuffers(
  const mgPoint3& eyePt)
{
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
    {
      mgPoint3 blobEyePt(eyePt.x-m_originX, eyePt.y - i*BRICKBLOB_CELLS, eyePt.z-m_originZ);
      m_blobs[i]->createBuffers(blobEyePt);
    }
  }
}

//--------------------------------------------------------------
// delete display buffers
void ChunkObj::deleteBuffers()
{
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
      m_blobs[i]->deleteBuffers();
  }
}

//--------------------------------------------------------------
// return true if needs updateBuffers call
BOOL ChunkObj::needsUpdate(
  const mgPoint3& eyePt)
{
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
    {
      mgPoint3 blobEyePt(eyePt.x-m_originX, eyePt.y - i*BRICKBLOB_CELLS, eyePt.z-m_originZ);
      if (m_blobs[i]->needsUpdate(blobEyePt))
        return true;
    }
  }
  return false;
}

//--------------------------------------------------------------
// create updated display buffers
void ChunkObj::createUpdate(
  const mgPoint3& eyePt)
{
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
    {
      mgPoint3 blobEyePt(eyePt.x-m_originX, eyePt.y - i*BRICKBLOB_CELLS, eyePt.z-m_originZ);
      m_blobs[i]->createUpdate(blobEyePt);
    }
  }
}

//--------------------------------------------------------------
// use updated display buffers
void ChunkObj::useUpdate()
{
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
      m_blobs[i]->useUpdate();
  }
}

//--------------------------------------------------------------
// reset the age of a chunk
void ChunkObj::resetAge()
{
  m_createTime = 0.0;
  m_alpha = 0.0;
}

//--------------------------------------------------------------
// update animation 
BOOL ChunkObj::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  BOOL changed = false;
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
      changed |= m_blobs[i]->animate(now, since);
  }

  if (m_createTime == 0)
    m_createTime = now;

  m_alpha = (now - m_createTime)/CREATE_DURATION;
  m_alpha = max(0.0, min(1.0, m_alpha));

  // if still aging in, return true
  return changed || (now - m_createTime) <= CREATE_DURATION;
}

//--------------------------------------------------------------
// render
void ChunkObj::render(
  ChunkWorld* world)
{
  mgDisplay->setLightColor(world->m_skyColor);

  if (m_alpha < 1.0)
  {
    mgDisplay->setZEnable(false);
    mgDisplay->setMatColor(1.0, 1.0, 1.0, 1.0);
    world->drawWater(m_originX, m_originZ);
    mgDisplay->setZEnable(true);

    mgDisplay->setMatColor(1.0, 1.0, 1.0, m_alpha);
    mgDisplay->setTransparent(true);
  }
  else
  {
    mgDisplay->setMatColor(1.0, 1.0, 1.0, 1.0);
    mgDisplay->setTransparent(false);
  }

  mgMatrix4 model;
  double halfSize = CHUNK_SIZE/2.0;
  double radius = halfSize * MG_CUBE_RADIUS;

  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
    {
      if (mgDisplay->withinFrustum(m_originX + halfSize, i*CHUNK_SIZE + halfSize, m_originZ + halfSize, radius))
      {
        model.loadIdentity();
        model.translate(m_originX, BRICKBLOB_CELLS*i, m_originZ);
        mgDisplay->setModelTransform(model);

        m_blobs[i]->m_torchColor = world->m_torchColor;
        m_blobs[i]->m_fogColor = world->m_fogColor;
        m_blobs[i]->m_fogBotHeight = world->m_fogBotHeight;
        m_blobs[i]->m_fogBotInten = world->m_fogBotInten;
        m_blobs[i]->m_fogTopHeight = world->m_fogTopHeight;
        m_blobs[i]->m_fogTopInten = world->m_fogTopInten;
        m_blobs[i]->m_fogMaxDist = world->m_fogMaxDist;

        m_blobs[i]->render();
      }
    }
  }
}

//--------------------------------------------------------------
// render transparent portion of object
void ChunkObj::renderTransparent(
  ChunkWorld* world)
{
  mgDisplay->setMatColor(1.0, 1.0, 1.0, m_alpha);
  mgDisplay->setLightColor(world->m_skyColor);

  double halfSize = CHUNK_SIZE/2.0;
  double radius = halfSize * MG_CUBE_RADIUS;

  mgPoint3 eyePt;
  mgDisplay->getEyePt(eyePt);
  int level = (int) floor(eyePt.y / CHUNK_SIZE);
  level = max(0, min(CHUNK_COLUMN-1, level));

  mgMatrix4 model;

  // display chunks below eye sorted from bottom to top
  for (int i = 0; i <= level; i++)
  {
    if (m_blobs[i] != NULL)
    {
      if (mgDisplay->withinFrustum(m_originX + halfSize, i*CHUNK_SIZE + halfSize, m_originZ + halfSize, radius))
      {
        model.loadIdentity();
        model.translate(m_originX, BRICKBLOB_CELLS*i, m_originZ);
        mgDisplay->setModelTransform(model);

        m_blobs[i]->m_torchColor = world->m_torchColor;
        m_blobs[i]->m_fogColor = world->m_fogColor;
        m_blobs[i]->m_fogBotHeight = world->m_fogBotHeight;
        m_blobs[i]->m_fogBotInten = world->m_fogBotInten;
        m_blobs[i]->m_fogTopHeight = world->m_fogTopHeight;
        m_blobs[i]->m_fogTopInten = world->m_fogTopInten;
        m_blobs[i]->m_fogMaxDist = world->m_fogMaxDist;

        m_blobs[i]->renderTransparent();
      }
    }
  }

  // display chunks above eye, from top to bottom
  for (int i = CHUNK_COLUMN-1; i > level; i--)
  {
    if (m_blobs[i] != NULL)
    {
      if (mgDisplay->withinFrustum(m_originX + halfSize, i*CHUNK_SIZE + halfSize, m_originZ + halfSize, radius))
      {
        model.loadIdentity();
        model.translate(m_originX, BRICKBLOB_CELLS*i, m_originZ);
        mgDisplay->setModelTransform(model);

        m_blobs[i]->renderTransparent();
      }
    }
  }
}

//--------------------------------------------------------------
// return size in bytes of object in memory
int ChunkObj::getSystemMemUsed()
{
  int memUsed = 0;
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
      memUsed += m_blobs[i]->getSystemMemUsed();
  }
  return memUsed;
}

//--------------------------------------------------------------
// return size in bytes of object in display
int ChunkObj::getDisplayMemUsed()
{
  int memUsed = 0;
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
      memUsed += m_blobs[i]->getDisplayMemUsed();
  }
  return memUsed;
}

//--------------------------------------------------------------
// return true if any brick in view frustum
BOOL ChunkObj::withinFrustum()
{
  double halfSize = CHUNK_SIZE/2.0;
  double radius = halfSize * MG_CUBE_RADIUS;

  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (mgDisplay->withinFrustum(m_originX + halfSize, i*CHUNK_SIZE + halfSize, m_originZ + halfSize, radius))
      return true;
  }
  return false;
}

//--------------------------------------------------------------
// return count of triangles
int ChunkObj::triangleCount()
{
  int count = 0;
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (m_blobs[i] != NULL)
      count += m_blobs[i]->triangleCount();
  }
  return count;
}

