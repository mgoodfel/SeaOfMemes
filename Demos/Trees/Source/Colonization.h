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
#ifndef COLONIZATION_H
#define COLONIZATION_H

class BranchLocs;

class Leaf
{
public:
  BOOL m_active;
  mgPoint3 m_pt;
  int m_closest;

  Leaf(){}
};

class Branch
{
public:
  mgPoint3 m_pt;
  int m_parent;

  mgPoint3 m_growDir;
  int m_growCount;
  int m_childCount;

  double m_area;

  double m_pathArea;
  int m_path;

  int m_next;   // chain of branches in octree cell

  Branch() {}
};

const double GROW_DISTANCE = 3.0;
const double MIN_GROW = 1.0;
const double MIN_DISTANCE = 2 /*4 */ * GROW_DISTANCE;
const double MAX_DISTANCE = 10 * GROW_DISTANCE;
const double TWIG_AREA = 0.1*0.1;
const int OCT_DEPTH = 16;

class Colonization
{
public:
  // constructor
  Colonization();

  // destructor
  virtual ~Colonization();

  // return count of leaves
  int getLeafCount()
  {
    return m_leaves.length();
  }

  // return a leaf
  Leaf* getLeaf(
    int i)
  {
    return (Leaf*) m_leaves[i];
  }

  // return count of branches
  int getBranchCount()
  {
    return m_branches.length();
  }

  // return a branch
  Branch* getBranch(
    int i)
  {
    return (Branch*) m_branches[i];
  }

  // add a leaf
  virtual void addLeaf(
    const mgPoint3& pt);

  // add a branch
  virtual void addBranch(
    const mgPoint3& pt,
    int parent);

  // do a step of colonization.  return true if added branch
  virtual BOOL colonize();

  // calculate sizes of branches
  virtual void computeBranchSizes(
    BOOL partial,               // true to scale nodes by growth
    int lastCount,              // only modify branches > lastCount
    double growth);             // growth factor

  // reset list of branches and leaves
  virtual void reset();

protected:
  mgPtrArray m_leaves;
  mgPtrArray m_branches;
  mgMapDWordToDWord m_branchLocs;

  // find closest branch
  BOOL findClosestBranch(
    const mgPoint3& leafPt,
    int& closest);
};

#endif
