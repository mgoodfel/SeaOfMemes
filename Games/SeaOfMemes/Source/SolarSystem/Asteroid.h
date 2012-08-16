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
#ifndef ASTEROID_H
#define ASTEROID_H

class Belt;
class Asteroid;

class AsteroidChunk
{
public:
  mgPoint3 m_origin;
  double m_radius;
  double m_height;
  int m_samples;
  BOOL m_addWater;

  // constructor
  AsteroidChunk();

  // destructor
  virtual ~AsteroidChunk();

  // create buffers for this chunk of surface
  virtual void createBuffers(
    Asteroid* world);

  // delete buffers
  virtual void deleteBuffers();

  // update animation 
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // render the world
  virtual void render(
    Asteroid* world);

  // render the world transparent data
  virtual void renderTransparent(
    Asteroid* world);

protected:
  mgVertexBuffer* m_vertexes;
  mgIndexBuffer* m_indexes;
  mgVertexBuffer* m_waterVertexes;
  mgIndexBuffer* m_waterIndexes;

  // return outside diameter of object
  void outsidePt(
    mgPoint3& pt,
    double x,
    double y,
    double z);

  // add the polygons for a height map
  void addPolygons(
    mgPoint3* points,
    BOOL outward);

  // create outside surface for this chunk
  virtual void addOutsideSurface(
    Asteroid* world);

  // create water surface for this chunk
  virtual void addWaterSurface();
};

class Asteroid
{
public:
  // constructor
  Asteroid(
    const mgOptionsFile& options,
    const mgPoint3& center,
    double radius,
    BOOL addWater);
  
  virtual ~Asteroid();
    
  // delete display buffers
  virtual void deleteBuffers();

  // create buffers ready to send to display
  virtual void createBuffers();

  // animate object
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // draw opaque data
  virtual void render();

  // draw transparent data
  virtual void renderTransparent();

protected:
  mgPoint3 m_center;

  mgTextureImage* m_terrainTexture;
  mgTextureImage* m_waterTexture;

  mgTextureCube* m_planetTexture;
  mgVertexBuffer* m_planetVertexes;
  mgIndexBuffer* m_planetIndexes;

  mgPtrArray m_chunks;      // of AsteroidChunk* 

  // create a sphere
  void createSphere(
    mgVertexBuffer*& vertexes,
    mgIndexBuffer*& indexes,
    const mgPoint3& center,
    double radius,
    int steps);

  friend class AsteroidChunk;
  friend class Belt;
};

#endif
