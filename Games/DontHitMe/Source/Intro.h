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
#ifndef INTRO_H
#define INTRO_H

class Tube;
class Saucer;

class Intro
{
public:
  // constructor
  Intro(
    const mgOptionsFile& options,
    const mgPoint3& origin);

  // destructor
  virtual ~Intro();

  // create vertex and index buffers
  virtual void createBuffers();

  // delete vertex and index buffers
  virtual void deleteBuffers();

  // animate the scene
  virtual void animate(
    double now,
    double since);

  // render on screen
  virtual void render();

protected:
  mgTextureImage* m_wallTexture;
  mgTextureImage* m_tubeTexture;

  mgPoint3 m_origin;

  Tube* m_wall;
  Tube* m_tube;
  Saucer* m_saucer;

  double m_ballRadius;
  double m_ballPosn;            // from 0 to spline length

  mgMatrix4 m_ballMatrix;       // coordinate space of ball
  mgPoint3 m_ballOrigin;        // current pt along track 
  mgPoint3 m_ballPt;            // position of ball

  mgMatrix4 m_eyeMatrix;
  mgPoint3 m_eyePt;

  // create the intro track
  void initTrack();

  // update position of saucer
  void updateBallPt();
};

#endif
