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
#ifndef SAMPLEUI_H
#define SAMPLEUI_H

class mgFormPane;

#include "mgGUI/Include/mgGUI.h"

class SampleUI : public mgActionListener, public mgSelectListener, public mgChangeListener
{
public:
  mgSurface* m_surface;
  mgStyle* m_style;
  mgTopControl* m_top;

  // constructor
  SampleUI(
    mgSurface* surface,
    const char* helpFile);

  // destructor
  virtual ~SampleUI();

  // animate the UI
  virtual void animate(
    double now,
    double since);

  // resize the UI
  virtual void resize(
    int width,
    int height);

  // return true if we have a key focus
  virtual BOOL hasKeyFocus();

  // return true if we have a mouse focus
  virtual BOOL hasMouseFocus();

  // return true if mouse point is for UI
  virtual BOOL useMousePoint(
    int x,
    int y);

  // set sample statistics
  virtual void setValue(
    double x,
    double y);

  // toggle console visibility
  virtual void toggleConsole();

  // toggle help visibility
  virtual void toggleHelp();

protected:
  mgStackControl* m_stack;
  mgControl* m_controlPanel;
  mgConsoleControl* m_console;
  mgFieldControl* m_input;
  mgFormPane* m_help;
  mgLabelControl* m_xvalue;
  mgLabelControl* m_yvalue;

  mgRectangle m_consoleBounds;
  mgRectangle m_controlBounds;
  
  // create the left side of the UI 
  virtual mgStackControl* createLeftSide(
    mgControl* parent,
    const char* helpFile);

  // create the control panel
  virtual mgControl* createControlPanel(
    mgControl* parent);

  // handle action from ui control
  virtual void guiAction(
    void* source,
    const char* name);
    
  // handle selection from ui control
  virtual void guiSelection(
    void* source,
    const char* name,
    BOOL state);
    
  // handle change from ui control
  virtual void guiChange(
    void* source,
    const char* name);
    
};

#endif
