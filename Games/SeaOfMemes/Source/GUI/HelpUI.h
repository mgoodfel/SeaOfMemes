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
#ifndef HELPUI_H
#define HELPUI_H

class mgFormPane;
class mgDebugPane;
class mgDebugInterface;

#include "mgGUI/Include/GUI/mgToolkit.h"

class HelpUI
{
public:
  // constructor
  HelpUI(
    const mgOptionsFile& options);

  // destructor
  virtual ~HelpUI();

  // view resized
  virtual void viewResized(
    int width,
    int height);

  // update ui graphics.  return true if changed
  virtual BOOL animate(
    double now,
    double since);

  // draw the ui
  virtual void render();

  // return top control
  virtual mgTopControl* getTop()
  { 
    return m_top;
  }

  // reset focus
  virtual void resetFocus();

  // set application for debug console
  virtual void setDebugApp(
    mgDebugInterface* app);

  // add a line to the debug console
  virtual void addLine(
    const char* format,
    ...);

  // toggle console visibility
  virtual void toggleConsole();

  // toggle help visibility
  virtual void toggleHelp();

  // true if ui has key focus
  virtual BOOL hasKeyFocus();

  // true if has mouse focus
  virtual BOOL hasMouseFocus();

  // true if ui uses mouse point
  virtual BOOL usesMousePoint(
    int x, 
    int y);

protected:
  mgTextureSurface* m_overlay;
  mgStyle* m_style;
  mgTopControl* m_top;

  mgStackControl* m_stack;
  mgFormPane* m_help;
  mgDebugPane* m_debug;

  mgRectangle m_debugBounds;

};

#endif
