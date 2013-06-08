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
#ifndef CHUNKLIST_H
#define CHUNKLIST_H

class ChunkObj;

// an entry in the requestList and LRULists
class ChunkListNode
{
public:
  ChunkListNode* m_next;
  ChunkListNode* m_prev;
  ChunkObj* m_chunk;

  // constructor
  ChunkListNode()
  {
    m_next = m_prev = NULL;
    m_chunk = NULL;
  }

  // destructor
  virtual ~ChunkListNode()
  {}
};

// we manage our own storage for the ChunkListNodes.  There are blocks of
// nodes with the free nodes chained together using the m_next ptr
const int CHUNK_LIST_NODE_BLOCK_SIZE = MG_POOL_BLOCK_SIZE / sizeof(ChunkListNode);

class ChunkListNodeBlock
{
public:
  ChunkListNode m_nodes[CHUNK_LIST_NODE_BLOCK_SIZE];
  ChunkListNodeBlock* m_next;

  // initialize node list
  void initNodes();
};


// a list of chunks
class ChunkList
{
public:
  ChunkListNode* m_first;
  ChunkListNode* m_last;
  int m_count;

  // constructor
  ChunkList();

  // destructor
  virtual ~ChunkList();

  // return true if empty list
  BOOL isEmpty()
  {
    return m_first == NULL;
  }

  // return number of nodes
  int length()
  {
    return m_count;
  }

  // find a chunk in the list
  ChunkListNode* find(
    ChunkObj* chunk);

  // add item to start
  void addToHead(
    ChunkObj* chunk);

  // add item to end
  void addToTail(
    ChunkObj* chunk);
    
  // remove a node from the list
  void removeNode(
    ChunkListNode* node);

  // remove all items from list
  void removeAll();

  // clean up memory
  static void freeMemory();

protected:
  static ChunkListNodeBlock* g_nodeBlocks;
  static ChunkListNode* g_freeNode;

  // allocate a new node from the free list
  ChunkListNode* newNode();

  // return a node to the free list
  void freeNode(
    ChunkListNode* node);
};

#endif
