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
#ifndef TUBE_H
#define TUBE_H

class Tube
{
public:
  double m_trackLen;

  // constructor
  Tube(
    double radius,
    int widthSteps);

  // destructor
  virtual ~Tube();

  // set track to follow (tube owns storage)
  virtual void setTrack(
    mgBezier* track);

  // compute ball position and transform matrix
  virtual void getBallView(
    double ballPosn,
    double ballRotate,
    mgPoint3& ballOrigin,
    mgMatrix4& segmentMatrix);

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
  mgTextureImage* m_texture;
  mgIndexBuffer* m_indexes;
  mgVertexBuffer* m_vertexes;
  mgShader* m_shader;

  double m_radius;
  mgBezier* m_track;
  int m_trackSteps;
  int m_widthSteps;
};

#endif
