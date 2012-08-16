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
#ifndef NEBULASKY_H
#define NEBULASKY_H

class NebulaSky
{
public:
  // constructor
  NebulaSky(
    const mgOptionsFile& options);
  
  virtual ~NebulaSky();

  // set sun direction
  virtual void setSunDir(
    const mgPoint3& sunDir)
  {
    m_sunDir = sunDir;
  }
      
  // delete display buffers
  virtual void deleteBuffers();

  // create buffers ready to send to display
  virtual void createBuffers();

  // animate object
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // draw the sky
  virtual void render();

  // set the sky box
  virtual void setSkyBox(
    const char* xminName,
    const char* xmaxName,
    const char* yminName,
    const char* ymaxName,
    const char* zminName,
    const char* zmaxName);

protected:
  mgTextureImage* m_starTexture;
  mgVertexBuffer* m_starTriangles;

  mgTextureImage* m_sunTexture;
  mgVertexBuffer* m_sunTriangles;

  mgTextureCube* m_skyBoxTexture;
  mgVertexBuffer* m_skyBoxTriangles;

  float m_skyDist;
  mgPoint3 m_sunDir;

  // create triangles for skybox
  void createSkyBox();

  // add a star patch
  virtual void addStarPatch(
    const mgPoint3& pt);

  // create triangles for stars
  void createStars();

  // create triangles for sun
  void createSun();
};

#endif
