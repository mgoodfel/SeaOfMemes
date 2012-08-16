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
#ifndef SAUCER_H
#define SAUCER_H

class SaucerSegment;

class Saucer
{
public:
  mgPoint3 m_origin;
  double m_xAngle;
  double m_yAngle;
  double m_zAngle;

  int m_sound;

  // constructor
  Saucer(
    const mgOptionsFile& options);

  // destructor
  virtual ~Saucer();

  // create vertex and index buffers
  virtual void createBuffers();

  // delete vertex and index buffers
  virtual void deleteBuffers();

  // animate the saucer
  virtual void animate(
    double now,
    double since);

  // render on screen
  virtual void render();

  // stop texture cycling
  virtual void stopTexture();

protected:
  SaucerSegment* m_outline;
  int m_outlineCount;
  int m_outlineSize;

  double m_textureCycle;
  double m_wobble;
  double m_wobbleRate;

  static mgTextureArray* m_texture;
  static mgVertexBuffer* m_vertexes;
  static mgIndexBuffer* m_indexes;

  // build outline of saucer
  virtual void buildOutline();
};
#endif
