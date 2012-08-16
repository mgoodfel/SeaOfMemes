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
  // constructor
  Planet(
    const mgOptionsFile& options);

  // destructor
  virtual ~Planet();

  // create vertex and index buffers
  virtual void createBuffers();

  // delete vertex and index buffers
  virtual void deleteBuffers();

  // render on screen
  virtual void render();

protected:
  mgTextureImage* m_outsideTexture;
  mgTextureImage* m_insideTexture;
  mgTextureImage* m_lavaTexture;

  mgVertexBuffer* m_outsideVertexes;
  mgIndexBuffer* m_outsideIndexes;
  mgVertexBuffer* m_insideVertexes;
  mgIndexBuffer* m_insideIndexes;
  mgVertexBuffer* m_lavaVertexes;
  mgIndexBuffer* m_lavaIndexes;

  mgPoint3 m_origin;
  int m_samples;

  double m_outsideHeight;
  double m_outsideRadius;

  double m_insideHeight;
  double m_insideRadius;

  double m_lavaRadius;

  // return outside diameter of object
  double outsideHt(
    const mgPoint3& pt);

  // return inside diameter of object
  double insideHt(
    const mgPoint3& pt);

  // figure the normal for a point in grid
  void pointNormal(
    mgVertex& v,
    double* heights,
    mgPoint3* points,
    int index,
    int rowSize,
    BOOL outward);

  // add the polygons for a height map
  void addPolygons(
    double* insideHts,
    double* outsideHts,
    mgPoint3* points,
    BOOL* flags);
};
#endif
