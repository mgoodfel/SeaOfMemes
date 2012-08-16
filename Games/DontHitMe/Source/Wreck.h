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
#ifndef WRECK_H
#define WRECK_H

class Wreck
{
public:
  // constructor
  Wreck(
    const mgOptionsFile& options);

  // destructor
  virtual ~Wreck();

  // create vertex and index buffers
  virtual void createBuffers();

  // delete vertex and index buffers
  virtual void deleteBuffers();

  // render on screen
  virtual void render();

protected:
  mgTextureArray* m_texture;
  mgIndexBuffer* m_indexes;
  mgVertexBuffer* m_vertexes;

  mgPtrArray m_towers;

  // set normals of points on grid
  void setNormals(
    mgVertexTA* points,
    int rows,
    int cols,
    BOOL outward);

  // build a surface 
  void buildSurface(
    mgBezier& bodySpline,
    double bodyScale,
    int bodySteps,
    mgBezier& lenSpline,
    double lenScale,
    int lenSteps,
    const mgMatrix4& xform,
    BOOL outwards,
    int texture);

  // create buffers for tower ring
  void createRing(
    mgMatrix4& xform,
    int ringSteps,
    int lenSteps);

  // create buffers for ship spine
  void createSpine(
    int sideSteps,
    int outSteps);
};
#endif
