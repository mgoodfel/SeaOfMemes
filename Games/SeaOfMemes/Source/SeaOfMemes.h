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
#ifndef SEAOFMEMES_H
#define SEAOFMEMES_H

class HelpUI;
class SpeedUI;

#include "mgGUI/Include/Util/mgDebugPane.h"

class SeaOfMemes : public mgApplication, public mgDebugInterface
{
public:
  // constructor
  SeaOfMemes();

  // destructor
  virtual ~SeaOfMemes();

  //--- implement mgApplication interface

  // request display parameters
  virtual void appRequestDisplay();

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

  // delete any display buffers
  virtual void appDeleteBuffers();

  // create buffers, ready to send to display
  virtual void appCreateBuffers();

  // resize the view
  virtual void appViewResized(
    int width,
    int height);

  // handle idle time
  virtual void appIdle();

  // update animation 
  virtual BOOL appViewAnimate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass
    
  // render the view
  virtual void appViewDraw();

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

  // report stats to mgDebug
  virtual void appDebugStats(
    mgString& status);

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

protected:
  mgOptionsFile m_options;

  mgCursorDefn* m_moveCursor;
  mgCursorDefn* m_deskCursor;
  BOOL m_deskMode;

  BOOL m_eyeChanged;

  double m_turnSpeed;               // degrees per ms
  double m_targetSpeed;             // meters per second
  double m_avatarSpeed;             // meters per second

  double m_forwardSince;
  double m_backwardSince;
  double m_leftSince;
  double m_rightSince;
  double m_upSince;
  double m_downSince;
  double m_turnLeftSince;
  double m_turnRightSince;

  double m_lastAnimate;

  // light and material
  mgPoint3 m_matColor;
  mgPoint3 m_lightDir;
  mgPoint3 m_sunlightColor;
  mgPoint3 m_moonlightColor;
  mgPoint3 m_torchlightColor;

  NebulaSky* m_sky;
  SolarSystem* m_world;
  HelpUI* m_helpUI;
  SpeedUI* m_speedUI;

  BOOL m_landingMode;
  BOOL m_unitsMetric;

  const mgFont* m_alertFont;
  mgColor m_alertColor;

  // load cursor from options
  virtual void loadCursor();

  // initialize movement state
  virtual void initMovement();

  // set cursor and movement style to desktop
  virtual void setDeskMode(
    BOOL deskMode);

  // set landing mode
  virtual void setLandingMode(
    BOOL landingMode);

  // add alert message to the text pane
  virtual void addAlert(
    const char* format, ...);

  // toggle units 
  virtual void setUnits(
    BOOL metric);
};

#endif
