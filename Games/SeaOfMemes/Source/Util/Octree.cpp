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

#include "Octree.h"

const int DEFAULT_SORT_ORDER[] = {0, 1, 2, 3, 4, 5, 6, 7};

// a node in the Octree
class OctNode 
{
public:
  OctNodeIndex m_children[8];

  // initialize a node
  void init(
    int leafData);

  // destructor
  virtual ~OctNode();

  // traverse a node of the tree
  void traverse(
    Octree* tree,
    void* userData,
    OctSortFunc* sortFunc,
    OctNodeFunc* nodeFunc,
    OctLeafFunc* leafFunc,
    int size,
    int x,
    int y,
    int z);

  // set type of block in tree
  void setLeafData(
    Octree* tree,
    int size,
    int x,
    int y,
    int z,
    int leafSize,
    int leafData);

  // get block containing point
  void getLeaf(
    Octree* tree,
    int size,
    int x,
    int y,
    int z,
    int& leafX,
    int& leafY,
    int& leafZ,
    int& leafSize,
    int& leafData);

  // return count of all nodes
  int countAllNodes(
    Octree* tree);

  // return count of leaf nodes
  int countLeafNodes(
    Octree* tree);

  // return count of interior nodes
  int countInteriorNodes(
    Octree* tree);

  // save the node to a file
  void saveToFile(
    Octree* tree,
    FILE* outFile);

  // save the node to a buffer
  void saveToBuffer(
    Octree* tree,
    mgString& output);

  // load the tree from a file
  void loadFromFile(
    Octree* tree,
    FILE* inFile);

  // load the tree from a buffer
  void loadFromBuffer(
    Octree* tree,
    const mgString& buffer,
    int &posn);

protected:
  // constructor
  OctNode();

  friend class OctNodeBlock;
};

// we manage our own storage for the Octree nodes.  There are blocks of
// nodes with the free nodes chained together (first child slot used for
// "next free" index.  All allocated blocks are in Octree::g_nodeBlocks and
// the head of the free list is in Octree::g_freeIndex.
const int OCT_NODE_BLOCK_SHIFT = 14;  // 16384 nodes per block
const int OCT_NODE_BLOCK_SIZE = 1 << OCT_NODE_BLOCK_SHIFT;
const int OCT_NODE_BLOCK_MASK = OCT_NODE_BLOCK_SIZE-1;

class OctNodeBlock
{
public:
  OctNode m_nodes[OCT_NODE_BLOCK_SIZE];

  // constructor
  OctNodeBlock(
    OctNodeIndex baseIndex);

  // destructor
  virtual ~OctNodeBlock();
};

//--------------------------------------------------------------
// constructor
OctNodeBlock::OctNodeBlock(
  OctNodeIndex baseIndex)
{
  // chain them together with first node at baseIndex, points to baseIndex+1;
  baseIndex++;
  for (int i = 0; i < OCT_NODE_BLOCK_SIZE-1; i++)
  {
    m_nodes[i].m_children[0] = baseIndex++; 
  }
  // last node has no next node
  m_nodes[OCT_NODE_BLOCK_SIZE-1].m_children[0] = OCT_NODE_INVALID;
}

//--------------------------------------------------------------
// destructor
OctNodeBlock::~OctNodeBlock()
{
}

//--------------------------------------------------------------
// constructor
Octree::Octree(
  int depth,
  int leafData)
{
  g_nextIndex = 0;  // next available index
  g_freeIndex = OCT_NODE_INVALID;      // index of next free node

  m_depth = depth;

  // we never free the root node, so we can keep it as a pointer
  // for slightly faster access.
  OctNodeIndex rootIndex = newNode();
  m_root = getNode(rootIndex);
  m_root->init(leafData);
}

//--------------------------------------------------------------
// destructor
Octree::~Octree()
{
  // delete the node blocks
  for (int i = 0; i < g_nodeBlocks.length(); i++)
  {
    OctNodeBlock* block = (OctNodeBlock*) g_nodeBlocks[i];
    delete block;
  }
  g_nodeBlocks.reset();

  m_root = NULL;
}

//--------------------------------------------------------------
// free all the octnode memory
void Octree::freeMemory()
{
}

//--------------------------------------------------------------
// reset the tree
void Octree::reset(
  int leafData)
{
  // free all children of root
  for (int i = 0; i < 8; i++)
  {
    // if not a leaf node, free it
    if ((m_root->m_children[i] & OCT_NODE_LEAF) == 0)
      freeNode(m_root->m_children[i]);
    m_root->m_children[i] = leafData | OCT_NODE_LEAF;
  }
}

//--------------------------------------------------------------
// return a node from the index
inline OctNode* Octree::getNode(
  OctNodeIndex index) 
{
  OctNodeBlock* block = (OctNodeBlock*) g_nodeBlocks[index >> OCT_NODE_BLOCK_SHIFT];
  return &block->m_nodes[index & OCT_NODE_BLOCK_MASK];
}
      
//--------------------------------------------------------------
// allocate a new node
OctNodeIndex Octree::newNode()
{
  // if there's no free node
  if (g_freeIndex == OCT_NODE_INVALID)
  {
    // allocate a new block, start free list
    OctNodeBlock* block = new OctNodeBlock(g_nextIndex);
    g_freeIndex = g_nextIndex;
    g_nextIndex += OCT_NODE_BLOCK_SIZE;
    g_nodeBlocks.add(block);
  }
  
  // result is first free node
  OctNodeIndex result = g_freeIndex;

  // advance free node to next in free list
  OctNode* freeNode = getNode(g_freeIndex);
  g_freeIndex = freeNode->m_children[0];

  return result;
}

//--------------------------------------------------------------
// free a node
void Octree::freeNode(
  OctNodeIndex index)
{
  OctNode* node = getNode(index);

  // for each child node
  for (int i = 0; i < 8; i++)
  {
    // if not a leaf node, free it
    if ((node->m_children[i] & OCT_NODE_LEAF) == 0)
      freeNode(node->m_children[i]);
    node->m_children[i] = 0;  // reset
  }

  // put the node at the head of the free list
  node->m_children[0] = g_freeIndex;
  g_freeIndex = index;
}

//--------------------------------------------------------------------
// set a block in the tree
void Octree::setLeafData(
  int x,
  int y,
  int z,
  int leafSize,
  int leafData)
{
  int size = 1 << m_depth;
  m_root->setLeafData(this, size, x, y, z, leafSize, leafData);
}

//--------------------------------------------------------------------
// get the leaf containing the point
void Octree::getLeaf(
  int x,
  int y,
  int z,
  int& leafX,
  int& leafY,
  int& leafZ,
  int& leafSize,
  int& leafData)
{
  leafX = 0;
  leafY = 0;
  leafZ = 0;
  int size = 1 << m_depth;
  m_root->getLeaf(this, size, x, y, z, leafX, leafY, leafZ, leafSize, leafData);
}

//--------------------------------------------------------------------
// traverse the tree
void Octree::traverse(
  void* userData,
  OctSortFunc* sortFunc,
  OctNodeFunc* nodeFunc,
  OctLeafFunc* leafFunc)
{
  int size = 1 << m_depth;
  m_root->traverse(this, userData, sortFunc, nodeFunc, leafFunc, size, 0, 0, 0);
}

//--------------------------------------------------------------------
// return true if we can combine all leaf children of a node
BOOL Octree::canCombine(
  OctNodeIndex* children)
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
OctNodeIndex Octree::combineChildren(
  OctNodeIndex* children)
{
  // in generic tree, combine identical leaves into a single larger leaf node
  return children[0];
}

//--------------------------------------------------------------------
// initialize newly split node from parent leafData
void Octree::splitNode(
  int leafData,
  OctNodeIndex* children)
{
  for (int i = 0; i < 8; i++)
  {
    children[i] = OCT_NODE_LEAF | leafData;
  }
}

//--------------------------------------------------------------------
// return count of all nodes
int Octree::countAllNodes()
{
  return m_root->countAllNodes(this);
}

//--------------------------------------------------------------------
// return count of leaf nodes
int Octree::countLeafNodes()
{
  return m_root->countLeafNodes(this);
}

//--------------------------------------------------------------------
// return count of interior nodes
int Octree::countInteriorNodes()
{
  return m_root->countInteriorNodes(this);
}

//--------------------------------------------------------------
// save the tree to a file
void Octree::saveToFile(
  FILE* outFile)
{
  // write the node values in traverse order
  fputs("VER000 ", outFile);
  m_root->saveToFile(this, outFile);
  fputs(".", outFile);
}

//--------------------------------------------------------------
// save the tree to a buffer
void Octree::saveToBuffer(
  mgString& version,
  mgString& output)
{
  version = "VER000";
  // write the node values in traverse order
  m_root->saveToBuffer(this, output);
  output += ".";
}

//--------------------------------------------------------------
// load the tree from a file
void Octree::loadFromFile(
  FILE* inFile)
{
  char version[7];
  int len = fread(version, 1, sizeof(version), inFile);
  if (len != sizeof(version) ||
      memcmp(version, "VER000 ", sizeof(version)) != 0)
    throw new mgException("invalid version on Octree file");

  reset(0);  // reset the tree

  m_root->loadFromFile(this, inFile);
}

//--------------------------------------------------------------
// load the tree from a buffer
void Octree::loadFromBuffer(
  const char* version,
  const mgString& buffer)
{
  if (_stricmp(version, "VER000") != 0)
    throw new mgException("invalid version on Octree file");

  reset(0);  // reset the tree

  int posn = 0;
  m_root->loadFromBuffer(this, buffer, posn);
}

//--------------------------------------------------------------
// constructor
OctNode::OctNode()
{
  // do nothing.  only used when OctNodeBlock allocated
}

//--------------------------------------------------------------
// initialize a node
void OctNode::init(
  int leafData)
{
  // initialize all leaves of indicated type
  for (int i = 0; i < 8; i++)
    m_children[i] = leafData | OCT_NODE_LEAF;
}

//--------------------------------------------------------------
// destructor
OctNode::~OctNode()
{
}

//--------------------------------------------------------------------
// change a block in the tree
void OctNode::setLeafData(
  Octree* tree,
  int size,
  int x,
  int y,
  int z,
  int leafSize,
  int leafData)
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
    if ((m_children[cell] & OCT_NODE_LEAF) == 0)
      tree->freeNode(m_children[cell]);

    // set the node type
    m_children[cell] = leafData | OCT_NODE_LEAF;
  }
  else
  {
    OctNode* child = NULL;

    // if this is a leaf cell
    if ((m_children[cell] & OCT_NODE_LEAF) != 0)
    {
      // make a new node, with all leaves initialized at old data
      int oldLeafData = m_children[cell] & OCT_NODE_LEAF_DATA;
      m_children[cell] = tree->newNode();
      child = tree->getNode(m_children[cell]); 
      tree->splitNode(oldLeafData, child->m_children);
    }
    else child = tree->getNode(m_children[cell]); 

    // recurse to child
    child->setLeafData(tree, halfSize, x, y, z, leafSize, leafData);

    // if all nodes of child are leaves, see if we can combine them
    if ((OCT_NODE_LEAF & child->m_children[0] & child->m_children[1] &
        child->m_children[2] & child->m_children[3] & child->m_children[4] &
        child->m_children[5] & child->m_children[6] & child->m_children[7]) != 0)
    {
      if (tree->canCombine(child->m_children))
      {
        int newLeafData = tree->combineChildren(child->m_children);
        tree->freeNode(m_children[cell]);
        m_children[cell] = OCT_NODE_LEAF | newLeafData;
      }
    }
  }
}

//--------------------------------------------------------------------
// get the leaf containing the point
void OctNode::getLeaf(
  Octree* tree,
  int size,
  int x,
  int y,
  int z,
  int& leafX,
  int& leafY,
  int& leafZ,
  int& leafSize,
  int& leafData)
{
  // figure which child this block goes into
  int halfSize = size >> 1;
  int cell = (x%size < halfSize) ? 0 : 4;
  cell += (y%size < halfSize) ? 0 : 2;
  cell += (z%size < halfSize) ? 0 : 1;

  // if this is a leaf cell
  if ((m_children[cell] & OCT_NODE_LEAF) != 0)
  {
    leafX = x - x % halfSize;
    leafY = y - y % halfSize;
    leafZ = z - z % halfSize;
    leafSize = halfSize;
    leafData = m_children[cell] & OCT_NODE_LEAF_DATA;
  }
  else
  {
    OctNode* child = tree->getNode(m_children[cell]); 

    // recurse to child
    child->getLeaf(tree, halfSize, x, y, z, leafX, leafY, leafZ, leafSize, leafData);
  }
}

//--------------------------------------------------------------
// traverse a node of the tree, draw block
void OctNode::traverse(
  Octree* tree,
  void* userData,
  OctSortFunc* sortFunc,
  OctNodeFunc* nodeFunc,
  OctLeafFunc* leafFunc,
  int size,
  int x,
  int y,
  int z)
{
  int halfSize = size >> 1;

  // get the sort order
  const int* sortOrder = DEFAULT_SORT_ORDER;
  int sortResult[8];
  if (sortFunc != NULL)
  {
    (*sortFunc)(userData, tree, size, x, y, z, sortResult);
    sortOrder = sortResult;
  }

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
    if ((m_children[index] & OCT_NODE_LEAF) != 0)
    {
      // call callback function
      int leafData = (int) m_children[index] & OCT_NODE_LEAF_DATA;
      if (leafFunc != NULL)
        (*leafFunc) (userData, tree, halfSize, childX, childY, childZ, leafData);
    }
    else
    {
      // traverse child node
      OctNode* child = tree->getNode(m_children[index]);

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
int OctNode::countAllNodes(
  Octree* tree)
{
  int count = 1;  // this is a node

  // for each child
  for (int i = 0; i < 8; i++)
  {
    // if not a leaf, count children
    if ((m_children[i] & OCT_NODE_LEAF) == 0)
    {
      OctNode* child = tree->getNode(m_children[i]);
      count += child->countAllNodes(tree);
    }
    else count++;
  }
  return count;
}

//--------------------------------------------------------------
// return count of leaf nodes
int OctNode::countLeafNodes(
  Octree* tree)
{
  int count = 0;

  // for each child
  for (int i = 0; i < 8; i++)
  {
    // if not a leaf, count children
    if ((m_children[i] & OCT_NODE_LEAF) == 0)
    {
      OctNode* child = tree->getNode(m_children[i]);
      count += child->countLeafNodes(tree);
    }
    else count++;
  }
  return count;
}

//--------------------------------------------------------------
// return count of interior nodes
int OctNode::countInteriorNodes(
  Octree* tree)
{
  int count = 1;  // this is an interior node

  // for each child
  for (int i = 0; i < 8; i++)
  {
    // if not a leaf, count children
    if ((m_children[i] & OCT_NODE_LEAF) == 0)
    {
      OctNode* child = tree->getNode(m_children[i]);
      count += child->countInteriorNodes(tree);
    }
  }
  return count;
}

//--------------------------------------------------------------
// save a node to a file
void OctNode::saveToFile(
  Octree* tree,
  FILE* outFile)
{
  // for each child
  for (int i = 0; i < 8; i++)
  {
    // if a leaf
    if ((m_children[i] & OCT_NODE_LEAF) != 0)
    {
      // write the child
      fprintf(outFile, " %x", m_children[i] & ~OCT_NODE_LEAF);
    }
    else
    {
      // write child node
      OctNode* child = tree->getNode(m_children[i]);
      fputs("{", outFile);
      child->saveToFile(tree, outFile);
      fputs("}", outFile);
    }
  }
}

//--------------------------------------------------------------
// save a node to a buffer
void OctNode::saveToBuffer(
  Octree* tree,
  mgString& output)
{
  // for each child
  for (int i = 0; i < 8; i++)
  {
    // if a leaf
    if ((m_children[i] & OCT_NODE_LEAF) != 0)
    {
      // write the child
      char leafNode[32];
      sprintf(leafNode, " %x", m_children[i] & ~OCT_NODE_LEAF);
      output += leafNode;
    }
    else
    {
      // write child node
      OctNode* child = tree->getNode(m_children[i]);
      output += "{";
      child->saveToBuffer(tree, output);
      output += "}";
    }
  }
}

//--------------------------------------------------------------
// load node from a file
void OctNode::loadFromFile(
  Octree* tree,
  FILE* inFile)
{
  int cell = 0;
  int lastChar = -1;
  int c;
  BOOL done = false;
  while (!done)
  {
    // read pushed back character
    if (lastChar != -1)
    {
      c = lastChar;
      lastChar = -1;
    }
    else c = fgetc(inFile);

    if (c == '{')
    {
      // read interior node
      OctNodeIndex newNode = tree->newNode();
      m_children[cell++] = newNode;
      OctNode* child = tree->getNode(newNode);
      child->loadFromFile(tree, inFile);
    }
    else if (c == ' ')
    {
      int value = 0;
      // read leaf node
      while (true)
      {
        c = fgetc(inFile);
        if (isxdigit(c))
        {
          // =-= there's a routine for this, i know it...
          int digit = c - '0';
          if (digit < 0 || digit > 9)
            digit = 10+c-'a';
          if (digit < 0 || digit > 15)
            digit = 10+c-'A';
          value = (value << 4) | digit;
        }
        else 
        {
          // we're done.  push back delimiter
          lastChar = c;

          m_children[cell++] = value | OCT_NODE_LEAF;
          break;
        }
      }
    }
    else if (c == '}' || c == '.')
    {
      done = true;
    }
    else throw new mgException("unexpected char '%c'(%02x) reading Octree from file", c, c);
  }
}

//--------------------------------------------------------------
// load node from a buffer
void OctNode::loadFromBuffer(
  Octree* tree,
  const mgString& buffer,
  int& posn)
{
  int cell = 0;
  int lastChar = -1;
  int c;
  BOOL done = false;
  while (!done)
  {
    // read pushed back character
    if (lastChar != -1)
    {
      c = lastChar;
      lastChar = -1;
    }
    else c = buffer[posn++];

    if (c == '{')
    {
      // read interior node
      OctNodeIndex newNode = tree->newNode();
      m_children[cell++] = newNode;
      OctNode* child = tree->getNode(newNode);
      child->loadFromBuffer(tree, buffer, posn);
    }
    else if (c == '}' || c == '.')
    {
      done = true;
    }
    else if (isxdigit(c))
    {
      int value = 0;
      // read leaf node
      while (true)
      {
        if (isxdigit(c))
        {
          // =-= there's a routine for this, i know it...
          int digit = c - '0';
          if (digit < 0 || digit > 9)
            digit = 10+c-'a';
          if (digit < 0 || digit > 15)
            digit = 10+c-'A';
          value = (value << 4) | digit;
          c = buffer[posn++];
        }
        else 
        {
          // we're done.  push back delimiter
          lastChar = c;

          m_children[cell++] = value | OCT_NODE_LEAF;
          break;
        }
      }
    }
    else if (!isspace(c))
      throw new mgException("unexpected char '%c'(%02x) reading Octree from buffer", c, c);
  }
}
