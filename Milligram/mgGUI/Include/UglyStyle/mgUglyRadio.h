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
#ifndef MGRADIO_H
#define MGRADIO_H

class mgFrame;

#include "UIToolkit/mgControl.h"

#include "UIEvents/mgMouseListener.h"
#include "UIEvents/mgSelectListener.h"

class mgRadio : public mgControl, public mgMouseListener
{
public:
  // constructor
  mgRadio(
    mgControl* parent);
    
  // destructor
  virtual ~mgRadio();
  
  // set text
  virtual void setText(
    const WCHAR* text);
    
  // set state
  virtual void setState(
    BOOL state);
    
  // get state
  virtual BOOL getState();
  
  // set radio button group
  virtual void setGroupLead(
    mgRadio* lead);
    
  // paint content of control
  virtual void paint(
    mgContext* gc);

  virtual void minimumSize(
    mgDimension& size);

  virtual void preferredSize(
    mgDimension& size);

  // add a select listener
  virtual void addSelectListener(
    mgSelectListener* listener);

  // remove a select listener
  virtual void removeSelectListener(
    mgSelectListener* listener);

protected:
  const mgFont* m_font;
  mgColor m_upColor;
  mgColor m_hoverColor;
  mgColor m_downColor;
  mgColor m_disColor;
  
  const mgFrame* m_upFrame;
  const mgFrame* m_hoverFrame;
  const mgFrame* m_downFrame;
  const mgFrame* m_disFrame;

  const mgIcon* m_offIcon;
  const mgIcon* m_onIcon;

  mgString m_text;
  BOOL m_state;
  int m_mode;
  
  mgRadio* m_groupLead;
  mgPtrArray m_group;

  mgPtrArray* m_selectListeners;             // select listeners

  // send event to select listeners
  virtual void sendSelectEvent(
    mgSelectEventType type,
    const WCHAR* value);

  // mouse entered
  virtual void mouseEnter(
    mgMouseEvent& ev);

  // mouse exited
  virtual void mouseExit(
    mgMouseEvent& ev);

  // mouse pressed
  virtual void mouseDown(
    mgMouseEvent& ev);

  // mouse released
  virtual void mouseUp(
    mgMouseEvent& ev);

  // mouse clicked
  virtual void mouseClick(
    mgMouseEvent& ev);

  // mouse dragged
  virtual void mouseDrag(
    mgMouseEvent& ev);

  // mouse moved
  virtual void mouseMove(
    mgMouseEvent& ev);
};

#endif
