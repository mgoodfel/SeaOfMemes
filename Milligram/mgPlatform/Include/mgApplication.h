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
#ifndef MGAPPLICATION_H
#define MGAPPLICATION_H

#include "mgInputEvents.h"

class mgPlatformServices;

/*
  The abstract interface to an application.  All programs using this library must
  be subclasses of mgApplication.
*/
class mgApplication
{
public:
  // constructor
  mgApplication()
  {
    // constructor must not use any display services.  
    // appRequestDisplay is called, and you should set requested format.
    // appInit is called after display has been initialized.
  }

  // destructor
  virtual ~mgApplication()
  {
  }

  // set the display attributes
  virtual void appRequestDisplay() = 0;

  // initialize the application
  virtual void appInit()
  {}

  // terminate the application
  virtual void appTerm()
  {}

  // If you wish to support DirectX, you must be prepared for the graphics support
  // to reset itself, losing all display state.  This method call informs the app it
  // should delete display objects before reset.  It is also called at shutdown.
  // Ignore if you are not supporting DirectX.

  // delete any display buffers
  virtual void appDeleteBuffers()
  {}

  // this method is called after DirectX graphics support is reinitialized.  
  // It is also called at startup, after the window is created.
  // Ignore if you are not supporting DirectX.

  // create buffers, ready to send to display
  virtual void appCreateBuffers()
  {}

  // set the size of the view
  virtual void appViewResized(
    int width,
    int height)
  {}

  // no input pending.  
  // Typically, build the scene and call mgPlatform->swapBuffers().
  virtual void appIdle()
  {}

  // key press
  virtual void appKeyDown(
    int keyCode,
    int modifiers)
  {}

  // key release
  virtual void appKeyUp(
    int keyCode,
    int modifiers)
  {}

  // key typed
  virtual void appKeyChar(
    int keyCode,
    int modifiers)
  {}

  // mouse button pressed
  virtual void appMouseDown(
    int pressed,
    int flags)
  {}

  // mouse button released
  virtual void appMouseUp(
    int released,
    int flags)
  {}

  // mouse moved
  virtual void appMouseMove(
    int dx,
    int dy, 
    int flags)
  {}

  // mouse dragged
  virtual void appMouseDrag(
    int dx,
    int dy, 
    int flags)
  {}

  // mouse wheel rotated
  virtual void appMouseWheel(
    int wheel,
    int flags)
  {}

  // mouse enters window
  virtual void appMouseEnter(
    int x,
    int y)
  {}

  // mouse leaves window
  virtual void appMouseExit()
  {}
};

// create and return an instance of your application.  Do no graphics until 
// appInit has been called.
mgApplication *mgCreateApplication();

#endif
