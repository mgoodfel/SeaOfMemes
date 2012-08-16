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
#ifndef MGUGLYSTACK_H
#define MGUGLYSTACK_H

#include "GUI/mgStackControl.h"

class mgUglyStack : public mgStackControl
{
public:
  // constructor
  mgUglyStack(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgUglyStack();
  
  // force all panes to same size
  virtual void setSameSize(
    BOOL enable);
      
  // add a pane to the stack
  virtual mgControl* addPane(
    const char* name);
    
  // remove a pane from the stack
  virtual void removePane(
    const char* name);
    
  // select pane to show
  virtual void selectPane(
    const char* name);
    
  // return selected pane
  virtual void getSelected(
    mgString& name);

protected:
  mgMapStringToPtr m_panes;
  mgString m_selected;
  BOOL m_sameSize;

  // set control size
  virtual void setSize(
    int width,
    int height);

  // return minimum size
  virtual void minimumSize(
    mgDimension& size);

  // return preferred size
  virtual void preferredSize(
    mgDimension& size);
};

#endif
