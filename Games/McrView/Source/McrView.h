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
#ifndef MCRVIEW_H
#define MCRVIEW_H

class StarrySky;
class ChunkWorld;

#include "MovementApp.h"

class McrView : public MovementApp
{
public:
  // constructor
  McrView();

  // destructor
  virtual ~McrView();

  //--- subclass MovementApp

  // key press
  virtual BOOL moveKeyDown(
    int keyCode,
    int modifiers);

  // key char
  virtual BOOL moveKeyChar(
    int keyCode,
    int modifiers);

  //--- end of MovementApp methods

  //---- implement mgApplication interface

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

  // delete any display buffers
  virtual void appDeleteBuffers();

  // create buffers, ready to send to display
  virtual void appCreateBuffers();

  // update animation 
  virtual BOOL appViewAnimate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass
    
  // render the view
  virtual void appViewDraw();

  // report status for debug log
  virtual void appDebugStats(
    mgString& status);

  //--- end of mgApplication interface

protected:
  HelpUI* m_help;

  BOOL m_daylight;
  BOOL m_foggy;
  double m_fogMaxDist;

  // light and material
  mgPoint3 m_matColor;
  mgPoint3 m_lightDir;
  mgPoint3 m_sunlightColor;
  mgPoint3 m_moonlightColor;
  mgPoint3 m_torchlightColor;

  StarrySky* m_sky;
  ChunkWorld* m_world;

  // set day or night
  virtual void setDaylight(
    BOOL daylight);

  // set fog
  virtual void setFoggy(
    BOOL foggy);
};

#endif
