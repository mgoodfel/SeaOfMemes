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
#ifndef TOWER_H
#define TOWER_H

class Tower
{
public:
  // constructor
  Tower(
    const mgOptionsFile& options,
    BOOL lights);

  // destructor
  virtual ~Tower();

  // create vertex and index buffers
  virtual void createBuffers();

  // delete vertex and index buffers
  virtual void deleteBuffers();

  // render on screen
  virtual void render();

protected:
  BOOL m_lights;

  mgTextureImage* m_shellTexture;
  mgIndexBuffer* m_shellIndexes;
  mgVertexBuffer* m_shellVertexes;

  mgTextureArray* m_officeTextures;
  mgIndexBuffer* m_officeIndexes;
  mgVertexBuffer* m_officeVertexes;

  mgTextureImage* m_glassTexture;
  mgIndexBuffer* m_glassIndexes;

  mgBezier m_floorSpline;
  double m_floorLen;
  mgBezier m_aptSpline;
  double m_aptLen;

  double* m_floorDists;

  // set normals of points on grid
  void setNormals(
    mgVertex* points,
    int rows,
    int cols,
    BOOL outward);

  // find x value on spline
  double findSplineX(
    mgBezier& spline,
    double target,
    int count,
    double dist);

  // return a shell point
  void shellPoint(
    int i,
    int j,
    mgPoint3& pt);

  // create the outside shell
  void createShell();

  // add an office to the tower
  void addOffice(
    int floor,
    int start,
    int len);

  // create offices
  void createOffices();
};
#endif
