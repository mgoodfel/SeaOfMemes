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

#include "GraphicsUtil/mgBezier.h"

//--------------------------------------------------------------
// constructor
mgBezier::mgBezier(
  int pointSize)
{
  m_pointSize = pointSize;
  m_pointCount = 0;
  m_points = new mgPoint3[m_pointSize];
  m_controls = new mgPoint3[m_pointSize];

  m_nextLen = 0.0;
  m_totalLen = 0.0;
}

//--------------------------------------------------------------
// destructor
mgBezier::~mgBezier()
{
  delete m_points;
  m_points = NULL;
  delete m_controls;
  m_controls = NULL;
}

//--------------------------------------------------------------
// get length of a line segment
double mgBezier::getSegmentLen(
  int index)
{
  if (index < 0 || index > m_pointCount-2)
    return 0.0;

  // get length of segment from index to index+1
  mgPoint3 segment(m_points[index+1]);
  segment.subtract(m_points[index]);
  return segment.length();
}

//--------------------------------------------------------------
// return total length (sum of segments)
double mgBezier::getLength()
{
  if (m_totalLen > 0.0)
    return m_totalLen;

  if (m_pointCount < 2)
    return 0.0;

  for (int i = 1; i < m_pointCount; i++)
  {
    mgPoint3 segment(m_points[i]);
    segment.subtract(m_points[i-1]);
    m_totalLen += segment.length();
  }

  return m_totalLen;
}

//--------------------------------------------------------------
// add point and control, return index
int mgBezier::addVertex(
  const mgPoint3& pt,
  const mgPoint3& cntl)
{
  // if out of space, reallocate arrays
  if (m_pointCount >= m_pointSize)
  {
    m_pointSize *= 2;
    mgPoint3* newPoints = new mgPoint3[m_pointSize];
    memcpy(newPoints, m_points, sizeof(mgPoint3)*m_pointCount);
    mgPoint3* newControls = new mgPoint3[m_pointSize];
    memcpy(newControls, m_controls, sizeof(mgPoint3)*m_pointCount);
    
    delete m_points;
    m_points = newPoints;
    delete m_controls;
    m_controls = newControls;
  }

  m_points[m_pointCount] = pt;
  m_controls[m_pointCount] = cntl;
  m_pointCount++;

  // reset cached length and path position
  m_nextLen = 0.0;
  m_totalLen = 0.0;

  return m_pointCount-1;
}

//--------------------------------------------------------------
// get point and control
void mgBezier::getVertex(
  int index,
  mgPoint3& pt,
  mgPoint3& cntl)
{
  if (index < 0 || index >= m_pointCount)
    return;
  pt = m_points[index];
  cntl = m_controls[index];
}

//--------------------------------------------------------------
// remove a point
void mgBezier::removeVertex(
  int index)
{
  if (index < 0 || index >= m_pointCount)
    return;

  for (int i = index; i < m_pointCount; i++)
  {
    m_points[i] = m_points[i+1];
    m_controls[i] = m_controls[i+1];
  }
  m_pointCount--;

  // reset cached length and path position
  m_nextLen = 0.0;
  m_totalLen = 0.0;
}

//--------------------------------------------------------------
// remove all vertexes
void mgBezier::removeAll()
{
  m_pointCount = 0;
  // reset cached length and path position
  m_nextLen = 0.0;
  m_totalLen = 0.0;
}

//--------------------------------------------------------------
// set lastIndex at segment containing distance
void mgBezier::findSegment(
  double dist)
{
  BOOL changed = false;
  mgPoint3 startPt, segment;
  // if cached position reset, start from beginning
  if (m_nextLen == 0.0 || dist == 0.0)
  {
    m_lastIndex = 0;
    m_lastLen = 0.0;
    segment = m_points[1];
    segment.subtract(m_points[0]);
    m_nextLen = segment.length();
    changed = true;
  }

  startPt = m_points[m_lastIndex];

  // move backwards until we find correct segment
  while (dist < m_lastLen && m_lastIndex > 0)
  {
    // prev segment
    m_lastIndex--;
    startPt = m_points[m_lastIndex];
    segment = m_points[m_lastIndex+1];
    segment.subtract(startPt);
    m_nextLen = segment.length();
    m_lastLen -= m_nextLen;
    changed = true;
  }

  // move forwards until we find correct segment
  while (dist > m_lastLen+m_nextLen && m_lastIndex < m_pointCount-1)
  {
    // next segment
    m_lastIndex++;
    m_lastLen += m_nextLen;
    startPt = m_points[m_lastIndex];
    segment = m_points[m_lastIndex+1];
    segment.subtract(startPt);
    m_nextLen = segment.length();
    changed = true;
  }

  if (!changed)
    return;

  mgPoint3 startCntl = m_controls[m_lastIndex];
  // reverse previous segment ending control point for 
  // start of this segment
  if (m_lastIndex > 0)
  {
    startCntl.x = startPt.x + (startPt.x - startCntl.x);
    startCntl.y = startPt.y + (startPt.y - startCntl.y);
    startCntl.z = startPt.z + (startPt.z - startCntl.z);
  }

  mgPoint3 nextPt = m_points[m_lastIndex+1];
  mgPoint3 nextCntl = m_controls[m_lastIndex+1];

  // compute bezier terms for each coordinate
  m_xterm.w = startPt.x;
  m_xterm.z = 3.0*(startCntl.x - startPt.x);
  m_xterm.y = 3.0*nextCntl.x - 6.0*startCntl.x + 3.0*startPt.x;
  m_xterm.x = nextPt.x - 3.0*nextCntl.x + 3.0*startCntl.x - startPt.x;

  m_yterm.w = startPt.y;
  m_yterm.z = 3.0*(startCntl.y - startPt.y);
  m_yterm.y = 3.0*nextCntl.y - 6.0*startCntl.y + 3.0*startPt.y;
  m_yterm.x = nextPt.y - 3.0*nextCntl.y + 3.0*startCntl.y - startPt.y;

  m_zterm.w = startPt.z;
  m_zterm.z = 3.0*(startCntl.z - startPt.z);
  m_zterm.y = 3.0*nextCntl.z - 6.0*startCntl.z + 3.0*startPt.z;
  m_zterm.x = nextPt.z - 3.0*nextCntl.z + 3.0*startCntl.z - startPt.z;
}

//--------------------------------------------------------------
// generate point along spline
void mgBezier::splinePt(
  double dist,
  mgPoint3& pt)
{
  if (m_pointCount < 2)
    return;  // =-= not enough points

  // no negative distances
  dist = max(0.0, dist);

  findSegment(dist);
  double t = (dist-m_lastLen)/m_nextLen;

  pt.x = ((m_xterm.x*t + m_xterm.y)*t + m_xterm.z)*t + m_xterm.w;
  pt.y = ((m_yterm.x*t + m_yterm.y)*t + m_yterm.z)*t + m_yterm.w;
  pt.z = ((m_zterm.x*t + m_zterm.y)*t + m_zterm.z)*t + m_zterm.w;
}
