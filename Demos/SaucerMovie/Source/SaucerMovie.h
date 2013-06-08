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
#ifndef SAUCERMOVIE_H
#define SACUERMOVIE_H

class StarrySky;
class Saucer;
class Earth;
class HelpUI;

#include "MovementApp.h"

class SaucerMovie : public MovementApp
{
public:
  // constructor
  SaucerMovie();

  // destructor
  virtual ~SaucerMovie();

  //--- implement MovementApp subclass

  // key press
  virtual BOOL moveKeyDown(
    int keyCode,
    int modifiers);

  // key typed
  virtual BOOL moveKeyChar(
    int keyCode,
    int modifiers);

  //--- implement mgApplication interface

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

  // create buffers, ready to send to display
  virtual void appCreateBuffers();

  // delete any display buffers
  virtual void appDeleteBuffers();

  // update animation 
  virtual BOOL appViewAnimate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass
    
  // render the view
  virtual void appViewDraw();

  // report status for debug log
  virtual void appDebugStats(
    mgString& status);

  //--- end of mgApplication

protected:
  HelpUI* m_help;

  mgTextureImage* m_earthTexture;
  mgVertexBuffer* m_earthVertexes;
  mgShader* m_earthShader;

  BOOL m_nebula;
  StarrySky* m_sky;

  mgString m_raySound;
  mgString m_saucerSound;
  mgString m_musicSound;

  mgAudio* m_audio;
  int m_music;

  mgPtrArray m_saucers;
  mgPtrArray m_rays;

  int m_movieScene;
  double m_movieTime;
  Saucer* m_badSaucer;
  Saucer* m_killer1;
  Saucer* m_killer2;
  mgBezier m_cameraTrack;
  double m_trackLen;

  double m_randomPosn;

  // generate random number from simplex noise
  double rand();

  // place a saucer in the cloud
  virtual void placeSaucer(
    Saucer* saucer);

  // create earth vertexes
  virtual void createEarth();

  // do an animation step of the movie
  virtual void animateMovie(
    double now,
    double since);

};

#endif
