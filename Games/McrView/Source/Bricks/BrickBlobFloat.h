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
#ifndef BRICKBLOBFLOAT_H
#define BRICKBLOBFLOAT_H

#include "BrickBlob.h"

// world object for drawing landscape
class BrickBlobFloat : public BrickBlob
{
public:
  // load the shaders we use
  static void loadShaders();

  // constructor
  BrickBlobFloat();

  // destructor
  virtual ~BrickBlobFloat();

  // create the solid brick buffers
  virtual void createSolidBrickBuffers(
    BrickBuffers& buffers);

  // create the transparent brick buffers
  virtual void createTransBrickBuffers(
    BrickBuffers& buffers);

  // render the world
  virtual void render();

  // render the world transparent data
  virtual void renderTransparent();

  // return size in bytes of object in memory
  virtual int getSystemMemUsed();

  // return size in bytes of object in display
  virtual int getDisplayMemUsed();

protected:
  // add vertexes and indexes for cube 
  void cubeTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for slab
  void slabTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for cap
  void capTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for column
  void columnTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for stair
  void stairTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for stair dir 0
  void stairTriangles_0(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for stair dir 1
  void stairTriangles_1(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for stair dir 2
  void stairTriangles_2(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for stair dir 3
  void stairTriangles_3(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // add vertexes and indexes for shape
  virtual void shapeTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z);

  // return ambient intensity for vertex
  float ambientIntenXMin(
    int x,
    int y,
    int z);

  // return ambient intensity for vertex
  float ambientIntenXMax(
    int x,
    int y,
    int z);

  // return ambient intensity for vertex
  float ambientIntenYMin(
    int x,
    int y,
    int z);

  // return ambient intensity for vertex
  float ambientIntenYMax(
    int x,
    int y,
    int z);

  // return ambient intensity for vertex
  float ambientIntenZMin(
    int x,
    int y,
    int z);

  // return ambient intensity for vertex
  float ambientIntenZMax(
    int x,
    int y,
    int z);

};

#endif
