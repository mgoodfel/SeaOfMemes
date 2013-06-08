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
#ifndef MGBEZIER_H
#define MGBEZIER_H

/*
  Bezier splines.
*/
class mgBezier
{
public:
  // constructor
  mgBezier(
    int pointSize = 20);

  // destructor
  virtual ~mgBezier();

  // get length of a line segment
  virtual double getSegmentLen(
    int index);

  // get total length (sum of line segments)
  virtual double getLength();

  // add point and control, return index
  virtual int addVertex(
    const mgPoint3& pt,
    const mgPoint3& cntl);

  // remove a point
  virtual void removeVertex(
    int index);

  // remove all vertexes
  virtual void removeAll();

  // get point count
  virtual int getPointCount()
  {
    return m_pointCount;
  }

  // get point and control
  virtual void getVertex(
    int index,
    mgPoint3& pt,
    mgPoint3& cntl);

  // generate point along spline
  virtual void splinePt(
    double dist,
    mgPoint3& pt);

protected:
  mgPoint3* m_points;
  mgPoint3* m_controls;
  int m_pointSize;
  int m_pointCount;

  double m_totalLen;

  int m_lastIndex;
  double m_lastLen;
  double m_nextLen;
  mgPoint4 m_xterm;
  mgPoint4 m_yterm;
  mgPoint4 m_zterm;

  // set lastIndex at segment containing distance
  virtual void findSegment(
    double dist);
};

#endif
