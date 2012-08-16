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
#ifndef OCTTREE_H
#define OCTTREE_H

class OctNode;

// In the generic Octree, we reserve the top bit for the leaf/interior flag.
// The rest of the bits go to the leaf data.  We restrict node index values
// to 20 bits, or a max of 1,048,576 interior nodes.  Subclasses can use
// bits 21 to 31 as flags.
typedef UINT32 OctNodeIndex;

const OctNodeIndex OCT_NODE_INVALID     = 0x7FFFFFFF;

const OctNodeIndex OCT_NODE_LEAF        = 0x80000000;
const OctNodeIndex OCT_NODE_LEAF_DATA   = 0x7FFFFFFF;
const OctNodeIndex OCT_NODE_INDEX_MASK  = 0x000FFFFF;

class Octree;

// callback function for sort of node children
typedef void (OctSortFunc) (
  void* userData,             // pointer supplied to traverse
  Octree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z,
  int *sortOrder);            // order for 8 child nodes

// callback function for traverse of tree leaf nodes
typedef void (OctLeafFunc) (
  void* userData,             // pointer supplied to traverse
  Octree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z,
  int leafData);              // type of leaf node

// callback function for traverse of tree interior nodes
typedef BOOL (OctNodeFunc) (
  void* userData,             // pointer supplied to traverse
  Octree* tree,               // tree being traversed
  int size,                   // size of node
  int x,                      // min point of node
  int y,
  int z);

class Octree
{
public:
  int m_depth;                  // depth of tree

  // constructor
  Octree (
    int depth,
    int leafData);

  // destructor
  virtual ~Octree();

  // reset the tree
  virtual void reset(
    int leafData);

  // set data in leaf in tree
  virtual void setLeafData(
    int x,                      // min point of leaf
    int y,
    int z,
    int leafSize,
    int leafData);

  // return true if we can combine all leaf children of a node
  virtual BOOL canCombine(
    OctNodeIndex* children);

  // combine leaf nodes into a single leaf
  virtual OctNodeIndex combineChildren(
    OctNodeIndex* children);

  // initialize newly split node from parent leafData
  virtual void splitNode(
    int leafData,
    OctNodeIndex* children);

  // get the leaf containing the point
  virtual void getLeaf(
    int x,
    int y,
    int z,
    int& leafX,
    int& leafY,
    int& leafZ,
    int& leafSize,
    int& leafData);

  // traverse the tree
  virtual void traverse(
    void* userData,
    OctSortFunc* sortFunc,
    OctNodeFunc* nodeFunc,
    OctLeafFunc* leafFunc);

  // return count of all nodes
  virtual int countAllNodes();

  // return count of leaf nodes
  virtual int countLeafNodes();

  // return count of interior nodes
  virtual int countInteriorNodes();

  // save the tree to a file
  virtual void saveToFile(
    FILE* outFile);

  // save the tree to a buffer
  virtual void saveToBuffer(
    mgString& version,
    mgString& output);

  // load the tree from a file
  virtual void loadFromFile(
    FILE* inFile);

  // load the tree from a buffer
  virtual void loadFromBuffer(
    const char* version,
    const mgString& buffer);

  // clean up memory allocation
  static void freeMemory();

protected:
  OctNode* m_root;              // node tree

  mgPtrArray g_nodeBlocks;         // OctNodeBlock* pointers 
  OctNodeIndex g_nextIndex;      // next available index
  OctNodeIndex g_freeIndex;      // index of next free node

  // allocate a new node
  OctNodeIndex newNode();

  // free a node
  virtual void freeNode(
    OctNodeIndex index);

  // return a node from the index
  inline OctNode* getNode(
    OctNodeIndex index);
      
  friend class OctNode;
};

#endif
