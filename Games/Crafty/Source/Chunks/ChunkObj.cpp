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
  BrickBlob* blob;
  for (int i = 0; i < CHUNK_COLUMN; i++)
  {
    if (mgDisplay->supportsIntegerVertex())
      blob = new BrickBlobInt();
    else blob = new BrickBlobFloat();

    blob->setBrickSet(world->m_textureArray, world->m_brickSet);
    m_blobs[i] = blob;
  }

//  double noiseMin = INT_MAX;
//  double noiseMax = -INT_MAX;
//  double htMin = INT_MAX;
//  double htMax = -INT_MAX;

  for (int x = 0; x < 32; x++)
  {
    for (int z = 0; z < 32; z++)
    {
      // generate height from landscape function
      double tx = m_originX + x;
      double tz = m_originZ + z;
      double ht = mgSimplexNoise::noiseSum(10, tx/10000.0, tz/10000.0);
//      noiseMin = min(noiseMin, ht);
//      noiseMax = max(noiseMax, ht);
      ht = 10.0*exp(2.5*fabs(ht));
      ht += 20*mgSimplexNoise::noiseSum(3, tx/1333.0, tz/1333.0);

//      htMin = min(htMin, ht);
//      htMax = max(htMax, ht);

      int cellHt = max(0, (int) ht);
      cellHt = min(cellHt, CHUNK_COLUMN*BRICKBLOB_CELLS);

      const int WATER_HEIGHT = 64;

      // fill in soil
      for (int y = 0; y < cellHt; y++)
      {
        int brick = 0x0300;  // default to dirt
        if (y > WATER_HEIGHT-3 && y < WATER_HEIGHT+3)
          brick = 0x0C00;  // sand
        else if (y > cellHt-3 && y >= WATER_HEIGHT+3 && y < 90)
          brick = 0x0200;  // grass
        else if (y >= 90)
          brick = 0x0100;  // stone
        m_blobs[y/BRICKBLOB_CELLS]->setBrick(x, y%BRICKBLOB_CELLS, z, brick);
      }

      // fill in water
      for (int y = cellHt; y < WATER_HEIGHT; y++)
      {
        m_blobs[y/BRICKBLOB_CELLS]->setBrick(x, y%BRICKBLOB_CELLS, z, 0x0900);
      }
    }
  }

  // blobs are actually 34 by 34 by 34, with the first and last planes overlapping
  // the neighbors, to simplify visibility tests.  copy top plane of bricks from 
  // next blob to previous blob, etc.  plane 0 is bottom.
  for (int x = -1; x <= BRICKBLOB_CELLS; x++)
  {
    for (int z = -1; z <= BRICKBLOB_CELLS; z++)
    {
      m_blobs[1]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, -1, z)] = 
        m_blobs[0]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 31, z)];

      m_blobs[0]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 32, z)] = 
        m_blobs[1]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 0, z)];

      m_blobs[2]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, -1, z)] = 
        m_blobs[1]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 31, z)];

      m_blobs[1]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 32, z)] = 
        m_blobs[2]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 0, z)];

      m_blobs[3]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, -1, z)] = 
        m_blobs[2]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 31, z)];

      m_blobs[2]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 32, z)] = 
        m_blobs[3]->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, 0, z)];
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

