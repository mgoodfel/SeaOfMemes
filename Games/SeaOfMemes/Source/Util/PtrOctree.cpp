/*
  Copyright (C) 1995-2011 by Michael J. Goodfellow

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

#include "PtrOctree.h"

const int DEFAULT_SORT_ORDER[] = {0, 1, 2, 3, 4, 5, 6, 7};

//--------------------------------------------------------------
// initialize node list for block
void PtrOctNodeBlock::initNodes()
{
  // chain them together 
  for (int i = 0; i < PTR_OCT_NODE_BLOCK_SIZE-1; i++)
  {
    m_nodes[i].m_children[0] = &m_nodes[i+1];
  }
  // last node has no next node
  m_nodes[PTR_OCT_NODE_BLOCK_SIZE-1].m_children[0] = NULL;

  m_next = NULL;
}

//--------------------------------------------------------------
// constructor
PtrOctree::PtrOctree(
  int depth)
{
  m_nodeBlocks = NULL; 
  m_freeNode = NULL;

  m_depth = depth;

  m_root = newNode();
}

//--------------------------------------------------------------
// destructor
PtrOctree::~PtrOctree()
{
  // delete the node blocks
  PtrOctNodeBlock* block = m_nodeBlocks;
  while (block != NULL)
  {
    PtrOctNodeBlock* nextBlock = block->m_next;
    mgBlockPool::freeBlock((mgBlockPool*) block);
    block = nextBlock;
  }
  m_nodeBlocks = NULL;

  m_root = NULL;
}

//--------------------------------------------------------------
// allocate a new node
PtrOctNode* PtrOctree::newNode()
{
  // if there's no free node
  if (m_freeNode == NULL)
  {
    // allocate a new block, start free list
    PtrOctNodeBlock* block = (PtrOctNodeBlock*) mgBlockPool::newBlock();
    block->initNodes();

    block->m_next = m_nodeBlocks;
    m_nodeBlocks = block;

    m_freeNode = &block->m_nodes[0];
  }
  
  // result is first free node
  PtrOctNode* result = m_freeNode;
  m_freeNode = m_freeNode->m_children[0];

  // initialize result
  result->m_leaf = 0xFF;
  memset(result->m_children, 0, sizeof(result->m_children));

  return result;
}

//--------------------------------------------------------------
// free a node
void PtrOctree::freeNode(
  PtrOctNode* node)
{
  // for each child node
  for (int i = 0; i < 8; i++)
  {
    // if not a leaf node, free it
    if (!node->isLeaf(i))
      freeNode(node->m_children[i]);
    else freeLeaf(node->m_children[i]);

    node->m_children[i] = NULL;  // reset
  }

  // put the node at the head of the free list
  node->m_children[0] = m_freeNode;
  node->m_leaf = 0xFF;
  m_freeNode = node;
//  mgDebug("free node(%08x)", node);
}

//--------------------------------------------------------------
// free a leaf node
void PtrOctree::freeLeaf(
  void* leafData)
{
  // must be implemented in subclass
}

//--------------------------------------------------------------
// reset the tree
void PtrOctree::reset()
{
  // free all children of root
  for (int i = 0; i < 8; i++)
  {
    // if not a leaf node, free it
    if (!m_root->isLeaf(i))
      freeNode(m_root->m_children[i]);
    else freeLeaf(m_root->m_children[i]);

    m_root->m_children[i] = NULL;
  }
}

//--------------------------------------------------------------------
// set a block in the tree
void PtrOctree::setLeafData(
  int x,
  int y,
  int z,
  int leafSize,
  void* leafData)
{
  int size = 1 << m_depth;
  m_root->setLeafData(this, size, x, y, z, leafSize, leafData);
}

//--------------------------------------------------------------------
// get the leaf containing the point
void PtrOctree::getLeaf(
  int x,
  int y,
  int z,
  int& leafX,
  int& leafY,
  int& leafZ,
  int& leafSize,
  void*& leafData)
{
  leafX = 0;
  leafY = 0;
  leafZ = 0;
  int size = 1 << m_depth;
  m_root->getLeaf(this, size, x, y, z, leafX, leafY, leafZ, leafSize, leafData);
}

//--------------------------------------------------------------------
// traverse the tree
void PtrOctree::traverse(
  void* userData,
  PtrOctSortFunc* sortFunc,
  PtrOctNodeFunc* nodeFunc,
  PtrOctLeafFunc* leafFunc)
{
  int size = 1 << m_depth;
  m_root->traverse(this, userData, sortFunc, nodeFunc, leafFunc, size, 0, 0, 0);
}

//--------------------------------------------------------------------
// traverse the tree starting at subnode
void PtrOctree::traverseFrom(
  int nodeX,
  int nodeY,
  int nodeZ,
  int nodeSize,
  void* userData,
  PtrOctSortFunc* sortFunc,
  PtrOctNodeFunc* nodeFunc,
  PtrOctLeafFunc* leafFunc)
{
  int foundX, foundY, foundZ, foundSize;
  BOOL foundLeaf;
  PtrOctNode* foundNode;

  int size = 1 << m_depth;
  m_root->getNode(this, size, nodeX, nodeY, nodeZ, nodeSize, 
                  foundX, foundY, foundZ, foundSize, foundLeaf, foundNode);

  if (foundLeaf)
  {
    (*leafFunc)(userData, this, foundSize, foundX, foundY, foundZ, (void*) foundNode);
  }
  else
  {
    foundNode->traverse(this, userData, sortFunc, nodeFunc, leafFunc, foundSize, foundX, foundY, foundZ);
  }
}

//--------------------------------------------------------------------
// return true if we can combine all leaf children of a node
BOOL PtrOctree::canCombine(
  void* children[])
{
  // in generic tree, we can combine leaf nodes if they are identical.
  // subclass with leaf flags will process this differently.
  return children[1] == children[0] && 
         children[2] == children[0] && 
         children[3] == children[0] && 
         children[4] == children[0] && 
         children[5] == children[0] && 
         children[6] == children[0] && 
         children[7] == children[0];
}

//--------------------------------------------------------------------
// combine leaf nodes into a single leaf
void* PtrOctree::combineChildren(
  void* children[])
{
  // will never happen in generic tree, since nodes are never identical
  return children[0];
}

//--------------------------------------------------------------------
// initialize newly split node from parent leafData
void PtrOctree::splitNode(
  void* leafData,
  void* children[])
{
  for (int i = 0; i < 8; i++)
  {
    children[i] = leafData;
  }
}

//--------------------------------------------------------------------
// return count of all nodes
void PtrOctree::countNodes(
  int &interiorNodeCount,
  int &leafNodeCount)
{
  interiorNodeCount = 0;
  leafNodeCount = 0;
  return m_root->countNodes(this, interiorNodeCount, leafNodeCount);
}

//--------------------------------------------------------------
// constructor
PtrOctNode::PtrOctNode()
{
  // do nothing.  only used when PtrOctNodeBlock allocated
}

//--------------------------------------------------------------
// destructor
PtrOctNode::~PtrOctNode()
{
}

//--------------------------------------------------------------------
// change a block in the tree
void PtrOctNode::setLeafData(
  PtrOctree* tree,
  int size,
  int x,
  int y,
  int z,
  int leafSize,
  void* leafData)
{
  // figure which child this block goes into
  int halfSize = size >> 1;
  int cell = (x%size < halfSize) ? 0 : 4;
  cell += (y%size < halfSize) ? 0 : 2;
  cell += (z%size < halfSize) ? 0 : 1;

  // if at target size
  if (halfSize == leafSize)
  {
    // if this is not a leaf, free the node
    if (!isLeaf(cell))
      tree->freeNode(m_children[cell]);
    else tree->freeLeaf(m_children[cell]);

    // set the leaf node
    setLeaf(cell, leafData);
  }
  else
  {
    PtrOctNode* child = NULL;

    // if this is a leaf cell
    if (isLeaf(cell))
    {
      // make a new node, with all leaves initialized at old data
      void* oldLeafData = m_children[cell];
      child = tree->newNode();
      setNode(cell, child);
      tree->splitNode(oldLeafData, (void**) child->m_children);
      child->m_leaf = 0xFF;  // all children are leaves
    }
    else child = m_children[cell]; 

    // recurse to child
    child->setLeafData(tree, halfSize, x, y, z, leafSize, leafData);

    // if all nodes of child are leaves, see if we can combine them
    if (m_leaf == 0xFF)
    {
      if (tree->canCombine((void**) child->m_children))
      {
        void* newLeafData = tree->combineChildren((void**) child->m_children);
        tree->freeNode(child);
        setLeaf(cell, newLeafData);
      }
    }
  }
}

//--------------------------------------------------------------------
// get the leaf containing the point
void PtrOctNode::getLeaf(
  PtrOctree* tree,
  int size,
  int x,
  int y,
  int z,
  int& leafX,
  int& leafY,
  int& leafZ,
  int& leafSize,
  void*& leafData)
{
  // figure which child this block goes into
  int halfSize = size >> 1;
  int cell = (x%size < halfSize) ? 0 : 4;
  cell += (y%size < halfSize) ? 0 : 2;
  cell += (z%size < halfSize) ? 0 : 1;

  // if this is a leaf cell
  if (isLeaf(cell))
  {
    leafX = x - x % halfSize;
    leafY = y - y % halfSize;
    leafZ = z - z % halfSize;
    leafSize = halfSize;
    leafData = m_children[cell];
  }
  else
  {
    PtrOctNode* child = m_children[cell]; 

    // recurse to child
    child->getLeaf(tree, halfSize, x, y, z, leafX, leafY, leafZ, leafSize, leafData);
  }
}

//--------------------------------------------------------------------
// get the node with origin and size
void PtrOctNode::getNode(
  PtrOctree* tree,
  int size,
  int nodeX,
  int nodeY,
  int nodeZ,
  int nodeSize,
  int& foundX,                   // target/found position
  int& foundY,
  int& foundZ,
  int& foundSize,                // target/found size
  BOOL& foundLeaf,               // true if found is leaf
  PtrOctNode*& foundNode)
{
  // figure which child this block goes into
  int halfSize = size >> 1;
  int cell = (nodeX%size < halfSize) ? 0 : 4;
  cell += (nodeY%size < halfSize) ? 0 : 2;
  cell += (nodeZ%size < halfSize) ? 0 : 1;

  PtrOctNode* child = m_children[cell];

  foundX = nodeX - nodeX % halfSize;
  foundY = nodeY - nodeY % halfSize;
  foundZ = nodeZ - nodeZ % halfSize;
  foundSize = halfSize;
  foundNode = child;

  // if this is a leaf cell
  if (isLeaf(cell))
  {
    // can't recurse farther.  this may be larger than requested node.
    foundLeaf = true;
    return;
  }

  // if this is requested node
  if (foundSize == nodeSize)
  {
    foundLeaf = false;
    return;
  }

  // recurse to child
  child->getNode(tree, halfSize, nodeX, nodeY, nodeZ, nodeSize, 
                 foundX, foundY, foundZ, foundSize, foundLeaf, foundNode);
}

//--------------------------------------------------------------
// traverse a node of the tree, draw block
void PtrOctNode::traverse(
  PtrOctree* tree,
  void* userData,
  PtrOctSortFunc* sortFunc,
  PtrOctNodeFunc* nodeFunc,
  PtrOctLeafFunc* leafFunc,
  int size,
  int x,
  int y,
  int z)
{
  int halfSize = size >> 1;

  // get the sort order
  const int* sortOrder = DEFAULT_SORT_ORDER;
  if (sortFunc != NULL)
    (*sortFunc)(userData, tree, size, x, y, z, sortOrder);

  // for each child
  for (int i = 0; i < 8; i++)
  {
    int index = sortOrder[i];

    // figure child position
    int childX = x;
    if ((index & 4) != 0)
      childX += halfSize;
    int childY = y;
    if ((index & 2) != 0)
      childY += halfSize;
    int childZ = z;
    if ((index & 1) != 0)
      childZ += halfSize;

    // if a leaf
    if (isLeaf(index))
    {
      // call callback function
      void* leafData = m_children[index];
      if (leafData != NULL && leafFunc != NULL)
        (*leafFunc) (userData, tree, halfSize, childX, childY, childZ, leafData);
    }
    else
    {
      // traverse child node
      PtrOctNode* child = m_children[index];

      // if there's a node function, only traverse child if returns true.
      if (nodeFunc == NULL || (*nodeFunc) (userData, tree, halfSize, childX, childY, childZ))
      {
        child->traverse(tree, userData, sortFunc, nodeFunc, leafFunc, halfSize, childX, childY, childZ);
      }
    }
  }
}

//--------------------------------------------------------------
// return count of child and interior nodes
void PtrOctNode::countNodes(
  PtrOctree* tree,
  int &interiorNodeCount,
  int &leafNodeCount)
{
  interiorNodeCount++;

  // for each child
  for (int i = 0; i < 8; i++)
  {
    // if not a leaf, count children
    if (!isLeaf(i))
    {
      m_children[i]->countNodes(tree, interiorNodeCount, leafNodeCount);
    }
    else leafNodeCount++;
  }
}

