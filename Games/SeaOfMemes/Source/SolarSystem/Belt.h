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
#ifndef BELT_H
#define BELT_H

class Rock;

class Belt
{
public:
  // constructor
  Belt(
    const mgOptionsFile& options,
    double radius);
  
  virtual ~Belt();
    
  // set the eye point
  virtual void setEyePt(
    const mgPoint3& eyePt)
  {
    m_eyePt = eyePt;
  }

  // delete display buffers
  virtual void deleteBuffers();

  // create buffers ready to send to display
  virtual void createBuffers();

  // animate object
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // draw back (away from eye) portion of belt
  virtual void renderBackFar();

  // draw front (towards eye) portion of belt
  virtual void renderFrontFar();

  // draw transparent data
  virtual void renderTransparent();

  // render medium distance version
  virtual void renderMedium();

  // render near distance version
  virtual void renderNear();

protected:
  mgPoint3 m_eyePt;
  double m_radius;

  mgTextureImage* m_rockTexture;

  mgTextureImage* m_farTexture;
  mgVertexBuffer* m_farVertexes;
  mgIndexBuffer* m_farIndexes;
  mgShader* m_shader;

  mgVertexBuffer* m_mediumVertexes;
  mgIndexBuffer* m_mediumIndexes;

  mgPoint3 m_lastEyePt;
  mgPtrArray m_rocks;

  // create buffers for far belt
  virtual void createFarBelt();

  // create buffers for medium belt
  virtual void addMediumRock(
    const Rock* rock);

  // return true if asteroid present at location
  virtual BOOL asteroidPresent(
    double x,
    double y,
    double z,
    double& radius);

  // check a volume of space for new asteroids
  BOOL checkVolume(
    int lx, 
    int ly,
    int lz,
    int hx, 
    int hy,
    int hz);
};

#endif
