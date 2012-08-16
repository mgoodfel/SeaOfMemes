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

#include "Util/mgBlockPool.h"

mgBlockPool* mgBlockPool::g_pool = NULL;

//--------------------------------------------------------------
// allocate a new block from the pool
mgBlockPool* mgBlockPool::newBlock()
{
  mgBlockPool* result = NULL;

  mgOSGlobalLock();

  // take a block out of the pool, or create one
  if (g_pool != NULL)
  {
    result = g_pool;
    g_pool = g_pool->m_next;
  }
  else result = new mgBlockPool();

  result->m_next = NULL;

  mgOSGlobalUnlock();
  return result;
}

//--------------------------------------------------------------
// return a block to the pool
void mgBlockPool::freeBlock(
  mgBlockPool* block)
{
  mgOSGlobalLock();

  // put the block at head of pool
  block->m_next = g_pool;
  g_pool = block;

  mgOSGlobalUnlock();
}

//--------------------------------------------------------------------
// free node blocks
void mgBlockPool::freeMemory()
{
  mgOSGlobalLock();

  int count = 0;
  while (g_pool != NULL)
  {
    mgBlockPool* next = g_pool->m_next;
    delete g_pool;
    g_pool = next;
    count++;
  }

  mgOSGlobalUnlock();

  mgDebug("freed %d pool blocks (%d K), total = %g M", count, 
    MG_POOL_BLOCK_SIZE/1024, 
    (count*MG_POOL_BLOCK_SIZE)/(1024*1024.0));
}

//--------------------------------------------------------------------
// constructor
mgBlockPool::mgBlockPool()
{
  m_next = NULL;
}

//--------------------------------------------------------------------
// destructor
mgBlockPool::~mgBlockPool()
{
}
