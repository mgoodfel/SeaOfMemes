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
#ifndef GUITESTALL_H
#define GUITESTALL_H

class GuiTestAll : public mgApplication
{
public:
  // constructor
  GuiTestAll();

  // destructor
  virtual ~GuiTestAll();

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

  // delete any display buffers
  virtual void appDeleteBuffers();

  // create buffers, ready to send to display
  virtual void appCreateBuffers();

  // request display
  virtual void appRequestDisplay();

  // resize the view
  virtual void appViewResized(
    int width,
    int height);

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
  double m_lastAnimate;

  double m_angle;
  mgTextureArray* m_cubeTexture;
  mgIndexBuffer* m_cubeIndexes;
  mgVertexBuffer* m_cubeVertexes;
  mgShader* m_cubeShader;

  mgTextureImage* m_floorTexture;
  mgVertexBuffer* m_floorVertexes;
  mgShader* m_floorShader;

  mgTextureSurface* m_overlay;
  SampleUI* m_ui;

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

  // load texture patterns from options
  virtual void loadTextures();

  // create vertex and index buffers for cube
  virtual void createCube();

  // create vertex buffer for floor
  virtual void createFloor();

  // update animation 
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass
    
  // draw the view
  virtual void viewDraw();
};

#endif
