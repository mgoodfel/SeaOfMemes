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
#ifndef RING_H
#define RING_H

class Ring
{
public:
  // constructor
  Ring(
    const mgOptionsFile& options,
    double radius,
    double wallHeight,
    double wallWidth);
  
  virtual ~Ring();
    
  // delete display buffers
  virtual void deleteBuffers();

  // create buffers ready to send to display
  virtual void createBuffers();

  // animate object
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // draw back (away from eye) portion of ring
  virtual void renderBackFar();

  // draw front (towards eye) portion of ring
  virtual void renderFrontFar();

  // render medium distance version
  virtual void renderMedium();

  // render near distance version
  virtual void renderNear();

protected:
  double m_radius;
  double m_ringWidth;
  double m_ringThick;
  double m_wallHeight;
  double m_wallWidth;

  mgTextureArray* m_farTexture;
  mgVertexBuffer* m_farVertexes;
  mgIndexBuffer* m_farIndexes;
  mgShader* m_shader;

  // create far mode ring surface
  virtual void createSurfaceFar(
    int j);

  // create far mode ring shell
  virtual void createShellFar(
    int j);

  // create ring wall top
  virtual void createWallTopFar(
    int j,
    double edgeY);

  // create ring wall min side
  virtual void createWallMinFar(
    int j,
    double edgeY);

  // create ring wall max side
  virtual void createWallMaxFar(
    int j,
    double edgeY);
};

#endif
