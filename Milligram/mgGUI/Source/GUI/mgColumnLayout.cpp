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

#include "GUI/mgFrame.h"
#include "GUI/mgColumnLayout.h"

//--------------------------------------------------------------
// constructor
mgColumnLayout::mgColumnLayout(
  mgControl* parent) 
: mgLayoutManager(parent)
{
  m_frame = NULL;
}

//--------------------------------------------------------------
// destructor
mgColumnLayout::~mgColumnLayout() 
{
}

//--------------------------------------------------------------
// set frame for children
void mgColumnLayout::setFrame(
  const mgFrame* frame)
{
  m_frame = frame;
  controlResized();
}

//--------------------------------------------------------------
// remove child from layout
void mgColumnLayout::removeChild(
  mgControl* child)
{
  // nothing to do -- we don't keep state linked to a child
}
  
//--------------------------------------------------------------
// return minimum size for control
void mgColumnLayout::minimumSize(
  mgDimension& size) 
{
  size.m_width = 0;
  size.m_height = 0;
  int count = m_parent->childCount();
  for (int i = 0; i < count; i++)
  {
    mgControl* child = (mgControl*) m_parent->getChild(i);
    mgDimension childSize;
    child->minimumSize(childSize);
    
    // add the frame size
    if (m_frame != NULL)
    {
      mgDimension extSize;
      m_frame->getOutsideSize(childSize, extSize);
      childSize = extSize;
    }

    size.m_width = max(size.m_width, childSize.m_width);
    size.m_height += childSize.m_height;
  }
}

//--------------------------------------------------------------
// return preferred size for control
void mgColumnLayout::preferredSize(
  mgDimension& size) 
{
  size.m_width = 0;
  size.m_height = 0;
  int count = m_parent->childCount();
  for (int i = 0; i < count; i++)
  {
    mgControl* child = (mgControl*) m_parent->getChild(i);
    mgDimension childSize;
    child->preferredSize(childSize);

    // add the frame size
    if (m_frame != NULL)
    {
      mgDimension extSize;
      m_frame->getOutsideSize(childSize, extSize);
      childSize = extSize;
    }

    size.m_width = max(size.m_width, childSize.m_width);
    size.m_height += childSize.m_height;
  }
}

//--------------------------------------------------------------
// compute size at width.  return false if not implemented
BOOL mgColumnLayout::preferredSizeAtWidth(
  int width,
  mgDimension& size)
{
  size.m_width = 0;
  size.m_height = 0;
  int count = m_parent->childCount();
  for (int i = 0; i < count; i++)
  {
    mgControl* child = (mgControl*) m_parent->getChild(i);
    mgDimension childSize;
    if (!child->preferredSizeAtWidth(width, childSize))
      child->preferredSize(childSize);

    // add the frame size
    if (m_frame != NULL)
    {
      mgDimension extSize;
      m_frame->getOutsideSize(childSize, extSize);
      childSize = extSize;
    }

    size.m_width = max(size.m_width, childSize.m_width);
    size.m_height += childSize.m_height;
  }
  return true;
}

//--------------------------------------------------------------
// update layout after control resized
void mgColumnLayout::controlResized()
{
  mgDimension size;
  m_parent->getSize(size);

  int y = 0;
  // =-= spread in proportion to preferred sizes?
  int count = m_parent->childCount();
  for (int i = 0; i < count; i++)
  {
    mgControl* child = (mgControl*) m_parent->getChild(i);
    
    int childHeight = size.m_height / count;
    
    mgRectangle bounds(0, y, size.m_width, childHeight);
    if (m_frame != NULL)
      m_frame->getInsideRect(bounds);
      
    child->setBounds(bounds);
    y += childHeight;
  }
}

//--------------------------------------------------------------
// repaint background of parent control
void mgColumnLayout::paintBackground(
  mgContext* gc) 
{
  mgDimension size;
  m_parent->getSize(size);

  int y = 0;
  // =-= spread in proportion to preferred sizes?
  int count = m_parent->childCount();
  for (int i = 0; i < count; i++)
  {
    mgControl* child = (mgControl*) m_parent->getChild(i);
    
    int childHeight = size.m_height / count;
    
    if (m_frame != NULL)
      m_frame->paintBackground(gc, 0, y, size.m_width, childHeight);

    y += childHeight;
  }
}

//--------------------------------------------------------------
// repaint foreground of parent control
void mgColumnLayout::paintForeground(
  mgContext* gc) 
{
  mgDimension size;
  m_parent->getSize(size);

  int y = 0;
  // =-= spread in proportion to preferred sizes?
  int count = m_parent->childCount();
  for (int i = 0; i < count; i++)
  {
    mgControl* child = (mgControl*) m_parent->getChild(i);
    
    int childHeight = size.m_height / count;
    
    if (m_frame != NULL)
      m_frame->paintForeground(gc, 0, y, size.m_width, childHeight);

    y += childHeight;
  }
}
