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
#ifndef MGOCTREE_H
#define MGOCTREE_H

#include "../Util/mgBlockPool.h"

class mgOctree;

// callback function for sort of node children
typedef void (mgOctSortFunc) (
  void* userData,             // pointer supplied to traverse
  mgOctree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z,
  int *sortOrder);            // order for 8 child nodes

// callback function for traverse of tree leaf nodes
typedef void (mgOctLeafFunc) (
  void* userData,             // pointer supplied to traverse
  mgOctree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z,
  void* leafData);            // type of leaf node

// callback function for traverse of tree interior nodes
typedef bool (mgOctNodeFunc) (
  void* userData,             // pointer supplied to traverse
  mgOctree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z);

// a node in the mgOctree
class mgOctNode 
{
public:
  mgOctNode* m_children[8];
  BYTE m_leaf;              // see isLeaf(cell)

  // destructor
  virtual ~mgOctNode();

  // return true if is a leaf
  inline bool isLeaf(
    int cell)
  {
    return (m_leaf & (1 << cell)) != 0;
  }

  // set a cell to be leaf
  inline void setLeaf(
    int cell,
    void* leafData)
  {
    m_children[cell] = (mgOctNode*) leafData;
    m_leaf |= 1 << cell;
  }

  // set a cell to be a node
  inline void setNode(
    int cell,
    mgOctNode* node)
  {
    m_children[cell] = node;
    m_leaf &= ~(1 << cell);
  }

  // traverse a node of the tree
  void traverse(
    mgOctree* tree,
    void* userData,
    mgOctSortFunc* sortFunc,
    mgOctNodeFunc* nodeFunc,
    mgOctLeafFunc* leafFunc,
    int size,
    int x,
    int y,
    int z);

  // set type of block in tree
  void setLeafData(
    mgOctree* tree,
    int size,
    int x,
    int y,
    int z,
    int leafSize,
    void* leafData);

  // get block containing point
  void getLeaf(
    mgOctree* tree,
    int size,
    int x,
    int y,
    int z,
    int& leafX,
    int& leafY,
    int& leafZ,
    int& leafSize,
    void*& leafData);

  // return count of all nodes
  void countNodes(
    mgOctree* tree,
    int &interiorNodeCount,
    int &leafNodeCount);

protected:
  // constructor
  mgOctNode();

  friend class mgOctNodeBlock;
};

// we manage our own storage for the mgOctree nodes.  There are blocks of
// nodes with the free nodes chained together using the m_children[0] ptr
const int MG_OCT_NODE_BLOCK_SIZE = MG_POOL_BLOCK_SIZE / sizeof(mgOctNode);

class mgOctNodeBlock
{
public:
  mgOctNodeBlock* m_next;
  mgOctNode m_nodes[MG_OCT_NODE_BLOCK_SIZE];

  // initialize node list
  void initNodes();
};

class mgOctree
{
public:
  int m_depth;                  // depth of tree

  // constructor
  mgOctree(
    int depth);

  // destructor
  virtual ~mgOctree();

  // reset the tree
  virtual void reset();

  // set data in leaf in tree
  virtual void setLeafData(
    int x,                      // min point of leaf
    int y,
    int z,
    int leafSize,
    void* leafData);

  // delete leaf node
  virtual void freeLeaf(
    void* leafData);

  // return true if we can combine all leaf children of a node
  virtual bool canCombine(
    void* children[]);

  // combine leaf nodes into a single leaf
  virtual void* combineChildren(
    void* children[]);

  // initialize newly split node from parent leafData
  virtual void splitNode(
    void* leafData,
    void* children[]);

  // get the leaf containing the point
  virtual void getLeaf(
    int x,
    int y,
    int z,
    int& leafX,
    int& leafY,
    int& leafZ,
    int& leafSize,
    void*& leafData);

  // traverse the tree
  virtual void traverse(
    void* userData,
    mgOctSortFunc* sortFunc,
    mgOctNodeFunc* nodeFunc,
    mgOctLeafFunc* leafFunc);

  // return count of all nodes
  virtual void countNodes(
    int &interiorNodeCount,
    int &leafNodeCount);

  // clean up memory allocation
  static void freeMemory();

protected:
  mgOctNode* m_root;                    // node tree

  // create a new node
  mgOctNode* newNode();

  // free an old node
  void freeNode(
    mgOctNode* node);

  mgOctNodeBlock* m_nodeBlocks; 
  mgOctNode* m_freeNode;

  friend class mgOctNode;
};

#endif
