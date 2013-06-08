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
  reset();
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
  branch->m_childCount = 0;
  branch->m_area = TWIG_AREA;
  branch->m_path = -1;

  m_branches.add(branch);
  int index = m_branches.length();  // branch index +1

  int x = (int) floor(pt.x / MAX_DISTANCE);
  int y = (int) floor(pt.y / MAX_DISTANCE);
  int z = (int) floor(pt.z / MAX_DISTANCE);
  DWORD key = (x << 20) ^ (y << 10) ^ z;
  DWORD value;
  if (m_branchLocs.lookup(key, value))
    branch->m_next = (int) value;
  else branch->m_next = 0;

  m_branchLocs.setAt(key, (DWORD) index);
}

//--------------------------------------------------------------
// find closest branch
BOOL Colonization::findClosestBranch(
  const mgPoint3& leafPt,
  int& closest)
{
  closest = -1;
  double closestDist = INT_MAX;

  int checkCount = 0;
  for (int ix = -1; ix <= 1; ix++)
  {
    for (int iy = -1; iy <= 1; iy++)
    {
      for (int iz = -1; iz <= 1; iz++)
      {
        int x = (int) floor(leafPt.x / MAX_DISTANCE);
        int y = (int) floor(leafPt.y / MAX_DISTANCE);
        int z = (int) floor(leafPt.z / MAX_DISTANCE);
        DWORD key = ((x+ix) << 20) ^ ((y+iy) << 10) ^ (z+iz);
        DWORD value;
        if (!m_branchLocs.lookup(key, value))
          continue;
        int index = (int) value;

        while (index != 0)
        {
          Branch* branch = getBranch(index-1);

          // calculate distance
          mgPoint3 dir(leafPt);
          dir.subtract(branch->m_pt);
          double dist = dir.length();
          if (dist < MIN_DISTANCE)
          {
            // leaf has reached a branch point.  deactivate it.
            return false;
          }
          
          if (dist <= MAX_DISTANCE)
          {
            if (dist < closestDist)
            {
              closest = index-1;
              closestDist = dist;
            }
          }
          
          index = branch->m_next;
          checkCount++;
        }
      }
    }
  }

/*
  int branchCount = m_branches.length();
  for (int j = 0; j < branchCount; j++)
  {
    Branch* branch = getBranch(j);
    // calculate distance
    mgPoint3 dir(leafPt);
    dir.subtract(branch->m_pt);
    double dist = dir.length();
    if (dist < MIN_DISTANCE)
    {
      // leaf has reached a branch point.  deactivate it.
      return false;
    }

    if (dist > MAX_DISTANCE)
      continue;

    if (dist < closestDist)
    {
      closest = j;
      closestDist = dist;
    }
  }
*/

  // leaf is still active
  return true;
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

    leaf->m_active = findClosestBranch(leaf->m_pt, leaf->m_closest);

    // update growth of closest branch
    if (leaf->m_closest != -1)
    {
      Branch* branch = getBranch(leaf->m_closest);

      // calculate vector to branch
      mgPoint3 dir(leaf->m_pt);
      dir.subtract(branch->m_pt);
      dir.normalize();

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
    if (branch->m_growCount != 0 && branch->m_childCount < 3)
    {
      // normalize direction
      branch->m_growDir.normalize();
      branch->m_growDir.scale(GROW_DISTANCE);

      // create a new branch
      mgPoint3 pt(branch->m_pt);
      pt.add(branch->m_growDir);
      addBranch(pt, j);

      branch->m_childCount++;
      active = true;
    }
      
    // reset growth vector on branch
    branch->m_growDir.set(0, 0, 0);
    branch->m_growCount = 0;
  }

  return active;
}

//--------------------------------------------------------------
// calculate sizes of branches
void Colonization::computeBranchSizes(
  BOOL partial,               // true to scale nodes by growth
  int lastCount,              // only modify branches > lastCount
  double growth)              // growth factor
{
  // zero all the branch areas
  for (int j = m_branches.length()-1; j >= 0; j--)
  {
    Branch* branch = getBranch(j);
    branch->m_area = 0.0;
    branch->m_path = -1;
    branch->m_pathArea = -1;
  }

  // starting from tips, set parent area to sum of child areas.
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

  // starting from tips, find the thickest child for each parent.
  for (int j = m_branches.length()-1; j >= 0; j--)
  {
    Branch* branch = getBranch(j);

    if (branch->m_parent != -1)
    {
      Branch* parent = getBranch(branch->m_parent);
      if (branch->m_area > parent->m_pathArea)
      {
        parent->m_pathArea = branch->m_area;
        parent->m_path = j;
      }
    }
  }
}

//--------------------------------------------------------------
// reset list of branches and leaves
void Colonization::reset()
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

  m_branchLocs.removeAll();
}

#ifdef STUFF

  Point3 startPosn(m_lastPosn);
  Point3 endPosn(m_avatarPt);
  bool hit = false;
  while (true)
  {
    if (!boxHitsWorld(startPosn, endPosn, avaMin, avaMax, dist, hitPosn, hitPt, hitNormal))
      break;  // no hit

    hit = true;

    // get distance target extends past hit pt
    Point3 targetPt(endPosn);
    targetPt.subtract(hitPosn);
    double targetDist = targetPt.dot(hitNormal);

    // project this along sliding plane
    Point3 slideDist(hitNormal);
    slideDist.scale(targetDist);

    // add to original target pt
    Point3 slidePt(endPosn);
    slidePt.subtract(slideDist);

    // test collisions on slide movement
    startPosn = hitPosn;
    endPosn = slidePt;
  }
  m_avatarPt = endPosn;
#endif
