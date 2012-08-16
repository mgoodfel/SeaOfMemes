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
#ifndef MOVEMENTAPP_H
#define MOVEMENTAPP_H

// A simple application supporting
// - head turned on mouse
// - movement on WASD and cursor keys
// - ESC to exit to desktop cursor
// - parsing of options

#include "mgGUI/Include/Util/mgDebugPane.h"

class mgTopControl;

// abstract interface to ui
class MovementUI
{
public:
  // constructor
  MovementUI()
  {}

  // destructor
  virtual ~MovementUI()
  {}

  // view resized
  virtual void viewResized(
    int width,
    int height) = 0;

  // update ui graphics.  return true if changed
  virtual BOOL animate(
    double now,
    double since) = 0;

  // draw the ui
  virtual void render() = 0;

  // return top control
  virtual mgTopControl* getTop() = 0;

  // reset focus
  virtual void resetFocus() = 0;

  // true if ui has key focus
  virtual BOOL hasKeyFocus()
  {
    return false;
  }

  // true if has mouse focus
  virtual BOOL hasMouseFocus()
  {
    return false;
  }

  // true if ui uses mouse point
  virtual BOOL usesMousePoint(
    int x, 
    int y)
  {
    return false;
  }
};


class MovementApp : public mgApplication, public mgDebugInterface
{
public:
  // constructor
  MovementApp();

  // destructor
  virtual ~MovementApp();

  // set overlay UI
  virtual void setUI(
    MovementUI* ui);

  // subclass to add key processing.  return true if used key
  virtual BOOL moveKeyDown(
    int keyCode,
    int modifiers)
  {
    return false;
  }
    
  // subclass to add key processing.  return true if used key
  virtual BOOL moveKeyUp(
    int keyCode,
    int modifiers)
  {
    return false;
  }
    
  // subclass to add key processing.  return true if used key
  virtual BOOL moveKeyChar(
    int keyCode,
    int modifiers)
  {
    return false;
  }
    
  //---- implement mgDebugInterface 

  // intro text
  virtual void debugIntro(
    mgString& text);

  // help text
  virtual void debugHelp(
    mgString& text);

  // describe variables.  
  virtual void debugListVariables(
    mgStringArray& varNames,
    mgStringArray& helpText);

  // describe functions.
  virtual void debugListFunctions(
    mgStringArray& funcNames,
    mgStringArray& funcParms,
    mgStringArray& helpText);

  // return value of variable
  virtual void debugGetVariable(
    const char* varName,
    mgString& value);

  // set a variable
  virtual void debugSetVariable(
    const char* varName,
    const char* value,
    mgString& reply);

  // end of mgDebugInterface interface

  // call a function
  virtual void debugCallFunction(
    const char* funcName,
    mgStringArray& args,
    mgString& reply);

  //--------- implement mgApplication interface

  // request display
  virtual void appRequestDisplay();

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

  // create buffers, ready to send to display
  virtual void appCreateBuffers();

  // delete any display buffers
  virtual void appDeleteBuffers();

  // resize the view
  virtual void appViewResized(
    int width,
    int height);

  // update animation 
  virtual BOOL appViewAnimate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass
    
  // render the view
  virtual void appViewDraw();

  // idle time
  virtual void appIdle();

  // key press
  virtual void appKeyDown(
    int keyCode,
    int modifiers);

  // key release
  virtual void appKeyUp(
    int keyCode,
    int modifiers);

  // key typed
  virtual void appKeyChar(
    int keyCode,
    int modifiers);

  // mouse button pressed
  virtual void appMouseDown(
    int pressed,
    int flags);

  // mouse button released
  virtual void appMouseUp(
    int released,
    int flags);

  // mouse moved
  virtual void appMouseMove(
    int dx,
    int dy, 
    int flags);

  // mouse dragged
  virtual void appMouseDrag(
    int dx,
    int dy, 
    int flags);

  // mouse wheel rotated
  virtual void appMouseWheel(
    int wheel,
    int flags);

  // mouse enters window
  virtual void appMouseEnter(
    int x,
    int y);

  // mouse leaves window
  virtual void appMouseExit();

  // report status for debug log
  virtual void appDebugStats(
    mgString& status);

  //--------- end of mgApplication interface

protected:
  mgOptionsFile m_options;

  mgCursorDefn* m_deskCursor;
  mgCursorDefn* m_moveCursor;
  BOOL m_deskMode;

  BOOL m_eyeChanged;

  mgPoint3 m_eyePt;                     // current eye point

  double m_eyeRotX;
  double m_eyeRotY;
  double m_eyeRotZ;
  mgMatrix4 m_eyeMatrix;                // current eye matrix
  
  double m_turnSpeed;                   // degrees per ms
  double m_moveSpeed;                   // units per second

  double m_forwardSince;
  double m_backwardSince;
  double m_leftSince;
  double m_rightSince;
  double m_upSince;
  double m_downSince;
  double m_turnLeftSince;
  double m_turnRightSince;

  // performance statistics
  double m_timingStart;
  double m_lastAnimate;
  double m_drawTime;
  int m_frameCount;

  MovementUI* m_ui;

  // initialize movement state
  virtual void initMovement();

  // change position based on movement keys
  virtual BOOL updateMovement(
    double now,
    double since);

  // load cursor from options
  virtual void loadCursor();

  // set cursor and movement style to desktop
  virtual void setDeskMode(
    BOOL deskMode);

  // turn eye from mouse movement
  virtual void turnEye(
    int dx, 
    int dy);
};

#endif
