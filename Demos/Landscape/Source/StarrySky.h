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
#ifndef STARRYSKY_H
#define STARRYSKY_H

class StarrySky
{
public:
  // constructor
  StarrySky(
    const mgOptionsFile& options);
  
  virtual ~StarrySky();
    
  // delete display buffers
  virtual void deleteBuffers();

  // create buffers ready to send to display
  virtual void createBuffers();

  // animate the sky
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // draw the sky
  virtual void render();

  // enable the moon
  virtual void enableMoon(
    BOOL enable)
  {
    m_enableMoon = enable;
  }

  // set the moon direction
  virtual void setMoonDir(
    const mgPoint3& dir);

  // enable the sun
  virtual void enableSun(
    BOOL enable)
  {
    m_enableSun = enable;
  }

  // set the sun direction
  virtual void setSunDir(
    const mgPoint3& dir);

  // set the sky rotation angle
  virtual void setSkyAngle(
    double angle);

  // enable the stars
  virtual void enableStars(
    BOOL enable)
  {
    m_enableStars = enable;
  }

  // enable the skybox
  virtual void enableSkyBox(
    BOOL enable)
  {
    m_enableSkyBox = enable;
  }

  // enable the fog
  virtual void enableFog(
    BOOL enable)
  {
    m_enableFog = enable;
  }

  // set the sky box
  virtual void setSkyBox(
    const char* xminName,
    const char* xmaxName,
    const char* yminName,
    const char* ymaxName,
    const char* zminName,
    const char* zmaxName);

  // set fog height
  virtual void setFogHeight(
    double fogBotHeight,
    double fogTopHeight)
  {
    m_fogBotHeight = fogBotHeight;
    m_fogTopHeight = fogTopHeight;
  }

  // set fog height
  virtual void setFogInten(
    double fogBotInten,
    double fogTopInten)
  {
    m_fogBotInten = fogBotInten;
    m_fogTopInten = fogTopInten;
  }

  // set fog color
  virtual void setFogColor(
    const mgPoint3& color)
  {
    m_fogColor.x = color.x;
    m_fogColor.y = color.y;
    m_fogColor.z = color.z;              
    m_fogColor.w = 1.0;
  }

  // set fog max inten
  virtual void setFogDist(
    double skyDist,
    double fogMaxDist)
  {
    m_skyDist = (float) skyDist;
    m_fogMaxDist = (float) fogMaxDist;
  }

protected:
  mgTextureImage* m_starTexture;
  mgVertexBuffer* m_starTriangles;

  mgTextureImage* m_moonTexture;
  mgVertexBuffer* m_moonTriangles;

  mgTextureImage* m_sunTexture;
  mgVertexBuffer* m_sunTriangles;

  mgTextureCube* m_skyBoxTexture;
  mgVertexBuffer* m_skyBoxTriangles;

  mgVertexBuffer* m_fogVertexes;
  mgIndexBuffer* m_fogIndexes;

  float m_skyDist;
  mgPoint3 m_moonDir;
  mgPoint3 m_sunDir;
  double m_skyAngle;

  BOOL m_enableMoon;
  BOOL m_enableSun;
  BOOL m_enableStars;
  BOOL m_enableSkyBox;
  BOOL m_enableFog;

  mgPoint4 m_fogColor;
  double m_fogTopHeight;
  double m_fogTopInten;
  double m_fogBotHeight;
  double m_fogBotInten;
  double m_fogMaxDist;

  // create triangles for skybox
  void createSkyBox();

  // add a star patch
  virtual void addStarPatch(
    const mgPoint3& pt);

  // create triangles for stars
  void createStars();

  // create triangles for moon
  void createMoon();

  // create triangles for sun
  void createSun();

  // create triangles for fog
  void createFog();
};


#endif
