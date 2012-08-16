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
#ifndef PLANET_H
#define PLANET_H

class Planet
{
public:
  mgPoint3 m_eyePt;

  // constructor
  Planet(
    const mgOptionsFile& options);

  // destructor
  virtual ~Planet();

  // turn eye from mouse movement
  virtual void turnEye(
    int dx, 
    int dy);

  // move forwards
  virtual void moveForwards(
    double ms);

  // move backwards
  virtual void moveBackwards(
    double ms);

  // move right
  virtual void moveRightwards(
    double ms);

  // move left
  virtual void moveLeftwards(
    double ms);

  // render the planet
  void render(
    int graphicsWidth,
    int graphicsHeight,
    double angle);

protected:
  GLuint m_cloudsTexture;
  GLuint m_surfaceTexture;
  GLuint m_planetShader;
  GLuint m_vertexBuffer;

  double m_eyeRotX;
  double m_eyeRotY;
  mgMatrix4 m_eyeMatrix;

  mgPoint3 m_specularColor;
  mgPoint3 m_lightColor;
  mgPoint3 m_lightAmbient;
  mgPoint4 m_matColor;

  // compile the shader
  void compileShader(
    const mgOptionsFile& options);

  // load six image files for cubemap texture
  GLuint loadCubeMap(
    const char* xminFile,
    const char* xmaxFile,
    const char* yminFile,
    const char* ymaxFile,
    const char* zminFile,
    const char* zmaxFile);

  // load a shader source file
  const char* loadFile(
    const char* fileName);

  // create projection matrix
  void createProjection(
    mgMatrix4& projection,
    int graphicsHeight,
    int graphicsWidth);

  // set up shader
  void setupShader(
    const mgMatrix4& mvpMatrix,
    const mgPoint3& modelEye,
    const mgPoint3& modelLightDir);
};

#endif
