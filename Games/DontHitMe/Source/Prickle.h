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
#ifndef PRICKLE_H
#define PRICKLE_H

class Prickle
{
public:
  mgPoint3 m_origin;
  double m_xAngle;
  double m_yAngle;
  double m_zAngle;

  // constructor
  Prickle(
    const mgOptionsFile& options);

  // destructor
  virtual ~Prickle();

  // create vertex and index buffers
  virtual void createBuffers();

  // delete vertex and index buffers
  virtual void deleteBuffers();

  // animate
  virtual void animate(
    double now,
    double since);

  // render on screen
  virtual void render();

protected:
  double m_xTumble;
  double m_yTumble;

  int m_samples;
  double m_height;
  double m_radius;

  static mgTextureArray* m_texture;
  static mgVertexBuffer* m_vertexes;
  static mgIndexBuffer* m_indexes;
  static mgShader* m_shader;

  // generate noise
  double noise(
    double x,
    double y,
    double z);

  // create shell vertexes
  void createShell(
    int steps,
    int vertexSize);

  // create spines
  void createSpines(
    int latCount,
    int lonCount,
    int spinePoints,
    int spineSteps,
    double spineRadius,
    double spineLen);

  // create a spine
  void createSpine(
    int index,
    int points,
    int steps,
    double spineLen,
    double spineRadius,
    const mgPoint3& origin,
    const mgPoint3& xaxis,
    const mgPoint3& yaxis,
    const mgPoint3& zaxis);
};
#endif
