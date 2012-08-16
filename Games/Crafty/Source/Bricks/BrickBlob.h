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
#ifndef BRICKBLOB_H
#define BRICKBLOB_H

class BrickSet;
class BrickDefn;
class Point3Array;
class ChunkObj; // =-= debug

const int BRICKBLOB_CELLS = 32;
const int BRICKBLOB_SIZE = BRICKBLOB_CELLS+2;

// build indexes for array.  This could be done as an inline function,
// but compiler does not seem to inline them under debug, making the
// code VERY slow.
#define BRICK_CELL(size,x,y,z) (((y)+1)+(size)*((x)+1+(size)*((z)+1)))

#define MAJOR_CODE(brick) ((brick) >> 8)
#define MINOR_CODE(brick) ((brick) & 0xF)

class BrickBuffers
{
public:
  int m_cubeCount;        // rectangles
  int m_shapeCount;       // triangles
  int m_transCount;       // rectangles

  mgVertexBuffer* m_cubeVertexes;
  mgIndexBuffer* m_cubeIndexes;
  mgVertexBuffer* m_shapeVertexes;
  mgIndexBuffer* m_shapeIndexes;
  mgVertexBuffer* m_transVertexes;
  mgIndexBuffer* m_transIndexes;

  mgPoint3 m_sortEyePt;

  // constructor
  BrickBuffers();

  // destructor
  virtual ~BrickBuffers();

  // delete all buffers
  virtual void deleteBuffers();
};

// an array of bricks
class BrickBlob 
{
public:
  mgPoint3 m_torchColor;
  mgPoint4 m_fogColor;
  double m_fogBotHeight;
  double m_fogBotInten;
  double m_fogTopHeight;
  double m_fogTopInten;
  double m_fogMaxDist;

  // constructor
  BrickBlob();

  // destructor
  virtual ~BrickBlob();

  // set selected texture
  virtual void setSelectedTexture(
    const mgTextureImage* selectedTexture);

  // set the definition for a brickType
  virtual void setBrickSet(
    const mgTextureArray* textureArray,
    const BrickSet* brickSet);

  // set type of brick
  virtual void setBrick(
    int x,                      // min point of leaf
    int y,
    int z,
    WORD brickType);

  // set lighting of brick
  virtual void setLight(
    int x,                      // min point of leaf
    int y,
    int z,
    int inten);

  // return type of brick 
  virtual WORD getBrick(
    int x,
    int y,
    int z,
    int& flags);

  // create the block blob from a array of cube bytes
  virtual void setBricksFromArray(
    const WORD* data,
    int size);

  // load array from tree
  virtual void setArrayFromBricks(
    WORD* data,
    int size);

  // set the selected brick
  virtual void setSelected(
    int x,
    int y,
    int z);

  // reset selected brick
  virtual void resetSelected();

  // create buffers, ready to send to display
  virtual void createBuffers(
    const mgPoint3& eyePt);

  // delete any display buffers
  virtual void deleteBuffers();

  // return true if needs update
  virtual BOOL needsUpdate(
    const mgPoint3& eyePt);

  // create updated display buffers
  virtual void createUpdate(
    const mgPoint3& eyePt);

  // use updated buffers
  virtual void useUpdate();

  // update animation 
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // return true if can render (buffers created)
  virtual BOOL canRender();

  // render the world
  virtual void render() = 0;

  // render the world transparent data
  virtual void renderTransparent() = 0;

  // create the solid brick buffers
  virtual void createSolidBrickBuffers(
    BrickBuffers& buffers) = 0;

  // create the transparent brick buffers
  virtual void createTransBrickBuffers(
    BrickBuffers& buffers) = 0;

  // return size in bytes of object in memory
  virtual int getSystemMemUsed() = 0;

  // return size in bytes of object in display
  virtual int getDisplayMemUsed() = 0;

  // return first brick at ray.  return false if none
  virtual BOOL rayIntersect(
    const mgPoint3& origin,
    const mgPoint3& ray,
    double limit,
    double& dist,
    mgPoint3& hitPt,
    int& face,
    int& cellX,
    int& cellY,
    int& cellZ);

  // find first brick under point
  virtual int findGround(
    int x,
    int z);

  // update xmin edges from neighbor brick
  virtual void updateXMinEdge(
    BrickBlob* other);

  // update xmax edges from neighbor brick
  virtual void updateXMaxEdge(
    BrickBlob* other);

  // update zmin edges from neighbor brick
  virtual void updateZMinEdge(
    BrickBlob* other);

  // update zmax edges from neighbor brick
  virtual void updateZMaxEdge(
    BrickBlob* other);

  // do static initialization
  static void staticInit();

  // step to next cell on ray
  static void cellStep(
    mgPoint3& pt, 
    const mgPoint3& ray, 
    double& dist, 
    int& cellX,
    int& cellY, 
    int& cellZ,
    int& sideHit);

protected:
  const BrickSet* m_brickSet;                 // brick definitions
  const mgTextureArray* m_textureArray;       // loaded texture array

  // eye position at last sort
  mgPoint3 m_sortEyePt;

  // selected brick
  int m_selectedX;
  int m_selectedY;
  int m_selectedZ;
  const mgTextureImage* m_selectedTexture;
  mgVertexBuffer* m_selectedVertexes;

  BrickBuffers m_buffers;
  BrickBuffers m_buffers2;

  WORD* m_bricks;
  BYTE* m_lights;
  BOOL m_bricksChanged;
  BOOL m_needsSort; // =-= debug

  // static initialization
  static BOOL m_staticInitialized;
  static float m_intenLevels[16];

  // return true if block obscured by block in direction
  // not virtual.  needs to be as fast as possible
  BOOL isObscuredBy(
    int brickType,
    const BrickDefn* defn,
    int otherType,
    int dir);

  // return true if object needs a sort call
  virtual BOOL sortRequired(
    const mgPoint3& eyePt);

  // count triangles in the blob
  virtual void countTriangles(
    BrickBuffers& buffers);

  // create solid brick vertexes and indexes
  virtual void createSolidTriangles(
    BrickBuffers& buffers);

  // create sorted transparent brick vertexes and indexes
  virtual void createTransTriangles(
    BrickBuffers& buffers);

  // create transparent bricks for subarray of chunk
  virtual void sortTransTriangles(
    mgVertexBuffer* transVertexes,
    mgIndexBuffer* transIndexes,
    int startX,
    int startY,
    int startZ,
    int endX,
    int endY,
    int endZ);

  // add vertexes and indexes for cube 
  virtual void cubeTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z) = 0;

  // add vertexes and indexes for slab
  virtual void slabTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z) = 0;

  // add vertexes and indexes for cap
  virtual void capTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z) = 0;

  // add vertexes and indexes for column
  virtual void columnTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z) = 0;

  // add vertexes and indexes for stair
  virtual void stairTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z) = 0;

  // add vertexes and indexes for shape
  virtual void shapeTriangles(
    mgVertexBuffer* vertexes,
    mgIndexBuffer* indexes,
    int brickType,
    const BrickDefn* defnDefn,
    int x,
    int y,
    int z) = 0;


  // return lighting for vertex
  void lightXMin(
    int x,
    int y,
    int z,
    float& ambientLight,
    float& skyLight,
    float& blockLight);

  // return lighting for vertex
  void lightXMax(
    int x,
    int y,
    int z,
    float& ambientLight,
    float& skyLight,
    float& blockLight);

  // return lighting for vertex
  void lightYMin(
    int x,
    int y,
    int z,
    float& ambientLight,
    float& skyLight,
    float& blockLight);

  // return lighting for vertex
  void lightYMax(
    int x,
    int y,
    int z,
    float& ambientLight,
    float& skyLight,
    float& blockLight);

  // return lighting for vertex
  void lightZMin(
    int x,
    int y,
    int z,
    float& ambientLight,
    float& skyLight,
    float& blockLight);

  // return lighting for vertex
  void lightZMax(
    int x,
    int y,
    int z,
    float& ambientLight,
    float& skyLight,
    float& blockLight);

  friend class ChunkObj; // =-= debug
};

#endif
