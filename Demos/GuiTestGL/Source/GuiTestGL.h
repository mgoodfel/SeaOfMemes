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
#ifndef GUITESTGL_H
#define GUITESTGL_H

class mgFontList;
class TextureSurface;
class SampleUI;
class Planet;

class GuiTestGL : public mgApplication
{
public:
  // constructor
    // constructor must not use any display services.  
    // appRequestDisplay is called, and you should set requested format.
    // appInit is called after display has been initialized.
  GuiTestGL();

  // destructor
  virtual ~GuiTestGL();

  // set the display attributes
  virtual void appRequestDisplay();

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

  // delete any display buffers
    // If you wish to support DirectX, you must be prepared for the graphics support
    // to reset itself, losing all display state.  This method call informs the app it
    // should delete display objects before reset.  It is also called at shutdown.
  virtual void appDeleteBuffers();

  // create buffers, ready to send to display
    // this method is called after DirectX graphics support is reinitialized.  
    // It is also called at startup, after the window is created.
  virtual void appCreateBuffers();

  // set the size of the view
  virtual void appViewResized(
    int width,
    int height);

  // no input pending.  
  // Typically, build the scene and call mgPlatform->swapBuffers().
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

  // find a font file
  virtual BOOL findFont(
    const char* faceName, 
    BOOL bold, 
    BOOL italic, 
    mgString& fontFile);
 
protected:
  mgOptionsFile m_options;

  mgStringArray m_fontDirs;
  mgFontList* m_fontList;

  int m_windowWidth;
  int m_windowHeight;
  int m_cursorX;
  int m_cursorY;
  BOOL m_cursorEnabled;
  GLuint m_cursorTexture;
  int m_cursorWidth;
  int m_cursorHeight;
  int m_cursorHotX;
  int m_cursorHotY;

  TextureSurface* m_surface;
  SampleUI* m_ui;
  Planet* m_planet;

  double m_forwardSince;
  double m_backwardSince;
  double m_leftSince;
  double m_rightSince;

  // create a simple cursor
  virtual void createCursor();

  // create the user interface panel
  virtual void createUI();

  // destroy the user interface panel
  virtual void destroyUI();

  // update position based on pressed keys
  virtual void updateMovement();
};

#endif
