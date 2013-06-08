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

#ifndef MGDESKTOPCONTROL_H
#define MGDESKTOPCONTROL_H

/*
  The superclass of all window-manager controls
*/
#include "mgControl.h"

class mgDesktopListener;

// window attribute flags
const DWORD MG_WINDOW_HAS_TITLE     = 0x0001;
const DWORD MG_WINDOW_HAS_FRAME     = 0x0002;
const DWORD MG_WINDOW_CAN_MOVE      = 0x0004;
const DWORD MG_WINDOW_CAN_RESIZE    = 0x0008;
const DWORD MG_WINDOW_CAN_CLOSE     = 0x0010;
const DWORD MG_WINDOW_CAN_MAXIMIZE  = 0x0020;
const DWORD MG_WINDOW_CAN_MINIMIZE  = 0x0040;
const DWORD MG_WINDOW_ON_TOP        = 0x0080;
const DWORD MG_WINDOW_ON_BOTTOM     = 0x0100;

const DWORD MG_WINDOW_APP = 
      MG_WINDOW_HAS_TITLE | 
      MG_WINDOW_HAS_FRAME |
      MG_WINDOW_CAN_MOVE | 
      MG_WINDOW_CAN_RESIZE |
      MG_WINDOW_CAN_MAXIMIZE | 
      MG_WINDOW_CAN_MINIMIZE |
      MG_WINDOW_CAN_CLOSE;

const int MG_WINDOW_ORIGIN_TL       = 0;
const int MG_WINDOW_ORIGIN_TC       = 1;
const int MG_WINDOW_ORIGIN_TR       = 2;
const int MG_WINDOW_ORIGIN_BL       = 3;
const int MG_WINDOW_ORIGIN_BC       = 4;
const int MG_WINDOW_ORIGIN_BR       = 5;
const int MG_WINDOW_ORIGIN_CL       = 6;
const int MG_WINDOW_ORIGIN_CR       = 7;
const int MG_WINDOW_ORIGIN_CC       = 8;

class mgDesktopControl : public mgControl
{
public:
  // constructor
  mgDesktopControl(
    mgControl* parent,
    const char* cntlName)
  : mgControl(parent, cntlName)
  {}

  // destructor
  virtual ~mgDesktopControl()
  {}

  // add a window, return content pane control
  virtual mgControl* addWindow(
    const char* windowName = NULL) = 0;

  // remove and delete a window
  virtual void removeWindow(
    mgControl* contentPane) = 0;

  // show the window
  virtual void showWindow(
    mgControl* contentPane) = 0;

  // hide the window without deleting it
  virtual void hideWindow(
    mgControl* contentPane) = 0;

  // surface window
  virtual void surfaceWindow(
    mgControl* contentPane) = 0;

  // layout the window to preferred size
  virtual void layoutWindow(
    mgControl* contentPane) = 0;

  // set window flags
  virtual void enableWindowFeatures(
    mgControl* contentPane,
    DWORD flags) = 0;

  // set window title
  virtual void setWindowTitle(
    mgControl* contentPane,
    const char* title) = 0;

  // =-= set window icon

  // set window location 
  virtual void setWindowLocation(
    mgControl* contentPane,
    DWORD origin,
    int offsetX,
    int offsetY) = 0;

  // set content location 
  virtual void setContentLocation(
    mgControl* contentPane,
    DWORD origin,
    int offsetX,
    int offsetY) = 0;

  // set content pane size
  virtual void setContentSize(
    mgControl* contentPane,
    int width,
    int height) = 0;
              
  // add a desktop listener
  virtual void addDesktopListener(
    mgDesktopListener* listener) = 0;

  // remove a desktop listener
  virtual void removeDesktopListener(
    mgDesktopListener* listener) = 0;
};

#endif
