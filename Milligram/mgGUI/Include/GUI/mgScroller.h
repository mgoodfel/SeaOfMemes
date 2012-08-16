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
#ifndef MGSCROLLER_H
#define MGSCROLLER_H

class mgScrollListener;

class mgScroller
{
public:
  // add a scroll listener
  virtual void addScrollListener(
    mgScrollListener* listener) = 0;

  // remove a scroll listener
  virtual void removeScrollListener(
    mgScrollListener* listener) = 0;

  // set the range of the scroller
  virtual void setRange(
    int minRange,
    int lenRange) = 0;

  // get the range of the scroller
  virtual void getRange(
    int& minRange,
    int& lenRange) = 0;

  // set the view of the scroller
  virtual void setView(
    int minView,
    int lenView) = 0;

  // get the view position
  virtual void getView(
    int& minView,
    int& lenView) = 0;

  // return true if view is entire range
  virtual BOOL isFullView() = 0;
};

#endif
