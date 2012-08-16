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
#ifndef PTROCTTREE_H
#define PTROCTTREE_H

class PtrOctree;

// callback function for sort of node children
typedef void (PtrOctSortFunc) (
  void* userData,             // pointer supplied to traverse
  PtrOctree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z,
  const int* &sortOrder);     // order for 8 child nodes

// callback function for traverse of tree leaf nodes
typedef void (PtrOctLeafFunc) (
  void* userData,             // pointer supplied to traverse
  PtrOctree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z,
  void* leafData);            // type of leaf node

// callback function for traverse of tree interior nodes
typedef BOOL (PtrOctNodeFunc) (
  void* userData,             // pointer supplied to traverse
  PtrOctree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z);

// a node in the PtrOctree
class PtrOctNode 
{
public:
  PtrOctNode* m_children[8];
  BYTE m_leaf;              // see isLeaf(cell)

  // destructor
  virtual ~PtrOctNode();

  // return true if is a leaf
  inline BOOL isLeaf(
    int cell)
  {
    return (m_leaf & (1 << cell)) != 0;
  }

  // set a cell to be leaf
  inline void setLeaf(
    int cell,
    void* leafData)
  {
    m_children[cell] = (PtrOctNode*) leafData;
    m_leaf |= 1 << cell;
  }

  // set a cell to be a node
  inline void setNode(
    int cell,
    PtrOctNode* node)
  {
    m_children[cell] = node;
    m_leaf &= ~(1 << cell);
  }

  // traverse a node of the tree
  void traverse(
    PtrOctree* tree,
    void* userData,
    PtrOctSortFunc* sortFunc,
    PtrOctNodeFunc* nodeFunc,
    PtrOctLeafFunc* leafFunc,
    int size,
    int x,
    int y,
    int z);

  // set type of block in tree
  void setLeafData(
    PtrOctree* tree,
    int size,
    int x,
    int y,
    int z,
    int leafSize,
    void* leafData);

  // get block containing point
  void getLeaf(
    PtrOctree* tree,
    int size,
    int x,
    int y,
    int z,
    int& leafX,
    int& leafY,
    int& leafZ,
    int& leafSize,
    void*& leafData);

  // get the node with origin and size
  void getNode(
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
    PtrOctNode*& foundNode);

  // return count of all nodes
  void countNodes(
    PtrOctree* tree,
    int &interiorNodeCount,
    int &leafNodeCount);

protected:
  // constructor
  PtrOctNode();

  friend class PtrOctNodeBlock;
};

// we manage our own storage for the PtrOctree nodes.  There are blocks of
// nodes with the free nodes chained together using the m_children[0] ptr
const int PTR_OCT_NODE_BLOCK_SIZE = MG_POOL_BLOCK_SIZE / sizeof(PtrOctNode);

class PtrOctNodeBlock
{
public:
  PtrOctNodeBlock* m_next;
  PtrOctNode m_nodes[PTR_OCT_NODE_BLOCK_SIZE];

  // initialize node list
  void initNodes();
};

class PtrOctree
{
public:
  int m_depth;                  // depth of tree

  // constructor
  PtrOctree(
    int depth);

  // destructor
  virtual ~PtrOctree();

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
  virtual BOOL canCombine(
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
    PtrOctSortFunc* sortFunc,
    PtrOctNodeFunc* nodeFunc,
    PtrOctLeafFunc* leafFunc);

  // traverse the tree starting at subnode
  virtual void traverseFrom(
    int x,
    int y,
    int z,
    int size,
    void* userData,
    PtrOctSortFunc* sortFunc,
    PtrOctNodeFunc* nodeFunc,
    PtrOctLeafFunc* leafFunc);

  // return count of all nodes
  virtual void countNodes(
    int &interiorNodeCount,
    int &leafNodeCount);

  // clean up memory allocation
  static void freeMemory();

protected:
  PtrOctNode* m_root;                    // node tree

  // create a new node
  PtrOctNode* newNode();

  // free an old node
  void freeNode(
    PtrOctNode* node);

  PtrOctNodeBlock* m_nodeBlocks; 
  PtrOctNode* m_freeNode;

  friend class PtrOctNode;
};

#endif
