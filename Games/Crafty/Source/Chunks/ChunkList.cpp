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

#include "ChunkList.h"

ChunkListNodeBlock* ChunkList::g_nodeBlocks = NULL;
ChunkListNode* ChunkList::g_freeNode = NULL;

//--------------------------------------------------------------
// initialize block of nodes
void ChunkListNodeBlock::initNodes()
{
  // chain them together 
  for (int i = 0; i < CHUNK_LIST_NODE_BLOCK_SIZE-1; i++)
  {
    m_nodes[i].m_next = &m_nodes[i+1];
  }
  // last node has no next node
  m_nodes[CHUNK_LIST_NODE_BLOCK_SIZE-1].m_next = NULL;

  m_next = NULL;
}

//--------------------------------------------------------------
// allocate a new node from the free list
ChunkListNode* ChunkList::newNode()
{
  if (g_freeNode == NULL)
  {
    // allocate a new block, start free list
    ChunkListNodeBlock* block = (ChunkListNodeBlock*) mgBlockPool::newBlock();
    block->initNodes();

    block->m_next = g_nodeBlocks;
    g_nodeBlocks = block;

    g_freeNode = &block->m_nodes[0];
  }
  
  // take first node off free list
  ChunkListNode* node = g_freeNode;
  g_freeNode = g_freeNode->m_next;

  node->m_next = node->m_prev = NULL;
  node->m_chunk = NULL;

  return node;
}

//--------------------------------------------------------------
// return a node to the free list
void ChunkList::freeNode(
  ChunkListNode* node)
{
  node->m_next = g_freeNode;
  node->m_prev = NULL;
  g_freeNode = node;
}

//--------------------------------------------------------------
// free node blocks
void ChunkList::freeMemory()
{
  // delete the node blocks
  ChunkListNodeBlock* block = g_nodeBlocks;
  while (block != NULL)
  {
    ChunkListNodeBlock* nextBlock = block->m_next;
    mgBlockPool::freeBlock((mgBlockPool*) block);
    block = nextBlock;
  }
  g_nodeBlocks = NULL;
}

//--------------------------------------------------------------
// constructor
ChunkList::ChunkList()
{
  m_first = m_last = NULL;
  m_count = 0;
}

//--------------------------------------------------------------
// destructor
ChunkList::~ChunkList()
{
  removeAll();
}

//--------------------------------------------------------------
// find a chunk in the list
ChunkListNode* ChunkList::find(
  ChunkObj* chunk)
{
  ChunkListNode* node = m_first;
  while (node != NULL)
  {
    if (node->m_chunk == chunk)
      return node;
    node = node->m_next;
  }
  return NULL;
}

//--------------------------------------------------------------
// add item to start
void ChunkList::addToHead(
  ChunkObj* chunk)
{
  // create a new node for the chunk
  ChunkListNode* node = newNode();
  node->m_chunk = chunk;

  // chain it into th elist
  node->m_next = m_first;
  m_first = node;
  if (m_last == NULL)
    m_last = node;

  m_count++;
}

//--------------------------------------------------------------
// add item to end
void ChunkList::addToTail(
  ChunkObj* chunk)
{
  // create a new node for the chunk
  ChunkListNode* node = newNode();
  node->m_chunk = chunk;

  // chain it into the list
  node->m_prev = m_last;
  if (m_last != NULL)
    m_last->m_next = node;
  m_last = node;
  if (m_first == NULL)
    m_first = node;

  m_count++;
}
    
//--------------------------------------------------------------
// remove a node from the list
void ChunkList::removeNode(
  ChunkListNode* node)
{
  // take the node out of the list
  if (node->m_prev != NULL)
    node->m_prev->m_next = node->m_next;
  else m_first = node->m_next;

  if (node->m_next != NULL)
    node->m_next->m_prev = node->m_prev;
  else m_last = node->m_prev;

  // free the node
  freeNode(node);

  m_count--;
}

//--------------------------------------------------------------
// remove all items from list
void ChunkList::removeAll()
{
  while (m_first != NULL)
  {
    ChunkListNode* node = m_first->m_next;
    freeNode(m_first);
    m_first = node;
  }
  m_last = NULL;
  m_count = 0;
}
