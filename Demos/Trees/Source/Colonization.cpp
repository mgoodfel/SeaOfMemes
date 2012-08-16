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

#include "Colonization.h"

//--------------------------------------------------------------
// constructor
Colonization::Colonization()
{
}

//--------------------------------------------------------------
// destructor
Colonization::~Colonization()
{
  for (int i = 0; i < m_branches.length(); i++)
  {
    Branch* branch = getBranch(i);
    delete branch;
  }
  m_branches.removeAll();

  for (int i = 0; i < m_leaves.length(); i++)
  {
    Leaf* leaf = getLeaf(i);
    delete leaf;
  }
  m_leaves.removeAll();
}

//--------------------------------------------------------------
// add a leaf
void Colonization::addLeaf(
  const mgPoint3& pt)
{
  Leaf* leaf = new Leaf();
  leaf->m_active = true;
  leaf->m_pt = pt;
  leaf->m_closest = -1;
  m_leaves.add(leaf);
}

//--------------------------------------------------------------
// add a branch
void Colonization::addBranch(
  const mgPoint3& pt,
  int parent)
{
  Branch* branch = new Branch();
  branch->m_pt = pt;
  branch->m_parent = parent;
  branch->m_growDir = mgPoint3(0, 0, 0);
  branch->m_growCount = 0;
  branch->m_area = TWIG_AREA;

  m_branches.add(branch);
}

//--------------------------------------------------------------
// do a step of colonization.  return true if added branch
BOOL Colonization::colonize()
{
  BOOL active = false;

  int branchCount = m_branches.length();

  // for each leaf, find the closest branch point within the cutoff radius
  for (int i = m_leaves.length()-1; i >= 0; i--)
  {
    Leaf* leaf = getLeaf(i);
    if (!leaf->m_active)
      continue;

    leaf->m_closest = -1;
    double closestDist = INT_MAX;

    for (int j = 0; j < branchCount; j++)
    {
      Branch* branch = getBranch(j);
      // calculate distance
      mgPoint3 dir(leaf->m_pt);
      dir.subtract(branch->m_pt);
      double dist = dir.length();
      if (dist < MIN_DISTANCE)
      {
        leaf->m_active = false;
        break; 
      }
      if (dist > MAX_DISTANCE)
        continue;

      if (dist < closestDist)
      {
        leaf->m_closest = j;
        closestDist = dist;
      }
    }

    // update growth of closest branch
    if (leaf->m_closest != -1)
    {
      Branch* branch = getBranch(leaf->m_closest);

      // calculate vector to branch
      mgPoint3 dir(leaf->m_pt);
      dir.subtract(branch->m_pt);
      dir.normalize();
//      double dist = dir.length();
//      dist = pow(dist, 1.5);

      // add normalized vector
      branch->m_growDir.add(dir);
      branch->m_growCount++;
    }
  }

  // for each branch
  for (int j = 0; j < branchCount; j++)
  {
    Branch* branch = getBranch(j);
    // if there's growth, add a new branch
    if (branch->m_growCount != 0)
    {
      // normalize the growth direction
      branch->m_growDir.scale(1.0/branch->m_growCount);
      branch->m_growDir.normalize();
      branch->m_growDir.scale(GROW_DISTANCE);

      // create a new branch
      Branch* twig = new Branch();
      twig->m_pt = branch->m_pt;
      twig->m_pt.add(branch->m_growDir);
      twig->m_parent = j;
      twig->m_growDir = mgPoint3(0,0,0);
      twig->m_growCount = 0;
      twig->m_area = TWIG_AREA;

      m_branches.add(twig);

      // reset growth vector on branch
      branch->m_growDir = mgPoint3(0,0,0);
      branch->m_growCount = 0;
      active = true;
    }
  }

  return active;
}

//--------------------------------------------------------------
// calculate sizes of branches
void Colonization::branchSizes(
  BOOL partial,               // true to scale nodes by growth
  int lastCount,              // only modify branches > lastCount
  double growth)              // growth factor
{
  // zero all the branch areas
  for (int j = m_branches.length()-1; j >= 0; j--)
  {
    Branch* branch = getBranch(j);
    branch->m_area = 0.0;
    branch->m_main = -1;
    branch->m_mainAngle = -INT_MAX;
  }

  // set the area of all the branches
  for (int j = m_branches.length()-1; j >= 0; j--)
  {
    Branch* branch = getBranch(j);
    if (branch->m_area < TWIG_AREA)
    {
      branch->m_area = TWIG_AREA;
      if (partial && j > lastCount)
        branch->m_area *= growth;
    }

    if (branch->m_parent != -1)
    {
      Branch* parent = getBranch(branch->m_parent);
      parent->m_area += branch->m_area;
    }
  }

  // set the area of all the branches
  for (int j = m_branches.length()-1; j >= 0; j--)
  {
    Branch* branch = getBranch(j);

    if (branch->m_parent != -1)
    {
      Branch* parent = getBranch(branch->m_parent);
      if (branch->m_area > parent->m_mainAngle)
      {
        parent->m_mainAngle = branch->m_area;
        parent->m_main = j;
      }
    }
  }
}

