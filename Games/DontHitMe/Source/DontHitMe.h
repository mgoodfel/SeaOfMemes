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
#ifndef DONTHITME_H
#define DONTHITME_H

class StarrySky;
class Tube;
class Saucer;
class Planet;
class Wreck;
class Tower;
class Intro;

class DontHitMe : public mgApplication
{
public:
  // constructor
  DontHitMe();

  // destructor
  virtual ~DontHitMe();

  // set display parameters
  virtual void appRequestDisplay();

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

protected:
  mgOptionsFile m_options;

  mgCursorDefn* m_deskCursor;
  mgCursorDefn* m_moveCursor;
  BOOL m_deskMode;

  BOOL m_eyeChanged;
  BOOL m_flying;

  mgPoint3 m_eyePt;                   // current eye point

  double m_eyeRotX;
  double m_eyeRotY;
  double m_eyeRotZ;
  mgMatrix4 m_eyeMatrix;               // current eye matrix
  
  // flying speed
  double m_turnSpeed;
  double m_moveSpeed;

  // rolling speed
  double m_rollRate;            // acceleration
  double m_rollDrag; 
  double m_rollMax;
  double m_rollSpeed;

  // rotation speed
  double m_rotateRate;
  double m_rotateDrag; 
  double m_rotateMax;  
  double m_rotateSpeed; 

  // movement key timing
  double m_forwardSince;
  double m_backwardSince;
  double m_leftSince;
  double m_rightSince;
  double m_upSince;
  double m_downSince;
  double m_turnLeftSince;
  double m_turnRightSince;

  double m_lastAnimate;

  BOOL m_showingIntro;
  Intro* m_intro;

  StarrySky* m_sky;
  Saucer* m_saucer;
  Tube* m_tube;
  Planet* m_planet;
  Wreck* m_wreck;
  Tower* m_tower1;
  Tower* m_tower2;

  mgTextureImage* m_tubeTexture;

  // ball state
  double m_ballRadius;
  double m_ballPosn;            // from 0 to spline length
  double m_ballRotate;          // rotation angle of ball around track
  double m_ballRoll;            // rolling position of ball

  mgMatrix4 m_ballMatrix;       // coordinate space of ball
  mgPoint3 m_ballOrigin;        // current pt along track 
  mgPoint3 m_ballPt;            // position of ball

  mgPoint3 m_flightOrigin;       // direction when flying
  mgMatrix4 m_flightMatrix;

  // initialize the track
  virtual void initTrack();

  // initialize ball state
  virtual void initBall();

  // compute ball pt
  virtual void updateBallPt();

  // create buffers, ready to send to display
  virtual void appCreateBuffers();

  // delete any display buffers
  virtual void appDeleteBuffers();

  // resize the view
  virtual void appViewResized(
    int width,
    int height);

  // handle idle time
  virtual void appIdle();

  // update rotation of ball
  virtual void updateBallRotate(
    double since);

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

  // report status for debug log
  virtual void appDebugStats(
    mgString& status);

  // initialize movement state
  virtual void initMovement();

  // start flying mode
  virtual void startFlight();

  // update saucer position during flying
  virtual void updateFlyingView();

  // update movement when flying
  virtual BOOL updateFlyingMovement(
    double now,
    double since);

  // update movement when rolling ball
  virtual BOOL updateRollingMovement(
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
