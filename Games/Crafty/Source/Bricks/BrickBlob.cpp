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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "BrickSet.h"
#include "ShapeDefn.h"
#include "BrickBlob.h"

const float TRANSPARENT_AMBIENT = 0.05f;

//--------------------------------------------------------------
// constructor
BrickBuffers::BrickBuffers()
{
  m_cubeVertexes = NULL;
  m_cubeIndexes = NULL;
  m_shapeVertexes = NULL;
  m_shapeIndexes = NULL;
  m_transVertexes = NULL;
  m_transIndexes = NULL;
  
  m_cubeCount = 0;
  m_shapeCount = 0;
  m_transCount = 0;
}

//--------------------------------------------------------------
// destructor
BrickBuffers::~BrickBuffers()
{
  deleteBuffers();
}

//--------------------------------------------------------------
// delete all buffers
void BrickBuffers::deleteBuffers()
{
  delete m_cubeVertexes;
  m_cubeVertexes = NULL;
  delete m_cubeIndexes;
  m_cubeIndexes = NULL;

  delete m_shapeVertexes;
  m_shapeVertexes = NULL;
  delete m_shapeIndexes;
  m_shapeIndexes = NULL;

  delete m_transVertexes;
  m_transVertexes = NULL;
  delete m_transIndexes;
  m_transIndexes = NULL;

  m_cubeCount = 0;
  m_shapeCount = 0;
  m_transCount = 0;
}

BOOL BrickBlob::m_staticInitialized = false;
float BrickBlob::m_intenLevels[16];

//--------------------------------------------------------------
// static initialization
void BrickBlob::staticInit()
{
  // build the light levels for each intensity
  m_intenLevels[15] = 1.0f;
  for (int i = 14; i >= 0; i--)
  {
    m_intenLevels[i] = 0.8f * m_intenLevels[i+1];
  }

  m_staticInitialized = true;
}

//--------------------------------------------------------------
// constructor
BrickBlob::BrickBlob()
{
  resetSelected();
  m_selectedTexture = NULL;
  m_textureArray = NULL;
  m_selectedVertexes = NULL;

  // initialize the brick array
  m_bricks = new WORD[BRICKBLOB_SIZE*BRICKBLOB_SIZE*BRICKBLOB_SIZE];
  memset(m_bricks, 0, sizeof(WORD)*BRICKBLOB_SIZE*BRICKBLOB_SIZE*BRICKBLOB_SIZE);

  // initialize the lights array
  m_lights = new BYTE[BRICKBLOB_SIZE*BRICKBLOB_SIZE*BRICKBLOB_SIZE];
  memset(m_lights, 0xF0, BRICKBLOB_SIZE*BRICKBLOB_SIZE*BRICKBLOB_SIZE);

  m_bricksChanged = true;
  m_needsSort = true;
}

//--------------------------------------------------------------
// destructor
BrickBlob::~BrickBlob()
{
  delete m_bricks;
  m_bricks = NULL;

  delete m_lights;
  m_lights = NULL;

  m_selectedTexture = NULL;

  deleteBuffers();
}

//--------------------------------------------------------------
// set selected texture
void BrickBlob::setSelectedTexture(
  const mgTextureImage* selectedTexture)
{
  m_selectedTexture = selectedTexture;
}

//--------------------------------------------------------------
// set the textures for a brickType
void BrickBlob::setBrickSet(
  const mgTextureArray* textureArray,
  const BrickSet* brickSet)
{
  m_textureArray = textureArray;
  m_brickSet = brickSet;
}

//--------------------------------------------------------------
// set brick in the blob
void BrickBlob::setBrick(
  int x,                      // min point of leaf
  int y,
  int z,
  WORD newType)
{
  m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z)] = newType;
  m_bricksChanged = true;
}

//--------------------------------------------------------------
// return type of brick 
WORD BrickBlob::getBrick(
  int x,
  int y,
  int z,
  int& flags)
{
  flags = 0;
  return m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z)];
}

//--------------------------------------------------------------
// create the block blob from a array of cube bytes
void BrickBlob::setBricksFromArray(
  const WORD* data,
  int size)
{  
  if (size != BRICKBLOB_SIZE)
    return;
  memcpy(m_bricks, data, sizeof(WORD)*BRICKBLOB_SIZE*BRICKBLOB_SIZE*BRICKBLOB_SIZE);
  m_bricksChanged = true;
}

//--------------------------------------------------------------
// load array from tree
void BrickBlob::setArrayFromBricks(
  WORD* data,
  int size)
{
}

//--------------------------------------------------------------
// set lighting of a brick
void BrickBlob::setLight(
  int x,                      // min point of leaf
  int y,
  int z,
  int inten)
{
  m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y, z)] = (BYTE) inten;
  m_bricksChanged = true;
}

//--------------------------------------------------------------
// set the selected brick
void BrickBlob::setSelected(
  int x,
  int y,
  int z)
{
  m_selectedX = x;
  m_selectedY = y;
  m_selectedZ = z;
}

//--------------------------------------------------------------
// reset selected brick
void BrickBlob::resetSelected()
{
  m_selectedX = m_selectedY = m_selectedZ = -1;
}

//--------------------------------------------------------------
// find first brick under point
int BrickBlob::findGround(
  int x,
  int z)
{
  return 0;
}

//--------------------------------------------------------------
// return true if block obscured by block in direction
BOOL BrickBlob::isObscuredBy(
  int brickType,
  const BrickDefn* defn,
  int otherType,
  int dir)
{
  BrickDefn* otherDefn = m_brickSet->m_defns[MAJOR_CODE(otherType)];

  BOOL brickTrans = defn->m_trans[dir];
  BOOL otherTrans = otherDefn->m_trans[dir^1];

  if (brickTrans)
  {
    if (otherTrans)
    {
      // transparent brick is obscured by transparent brick
      // of the same type.  otherwise, no
      return MAJOR_CODE(brickType) == MAJOR_CODE(otherType);
    }
    else
    {
      // transparent brick is obscured by opaque brick
      return true;
    }
  }
  else
  {
    if (otherTrans)
    {
      // opaque brick is not obscured by transparent
      return false;
    }
    else
    {
      // opaque brick is obscured by opaque
      return true;
    }
  }
}

//--------------------------------------------------------------
// count triangles in the blob
void BrickBlob::countTriangles(
  BrickBuffers& buffers)
{
  int cubeCount = 0;
  int shapeCount = 0;
  int transCount = 0;

  for (int x = 0; x < BRICKBLOB_CELLS; x++)
  {
    for (int y = 0; y < BRICKBLOB_CELLS; y++)
    {
      for (int z = 0; z < BRICKBLOB_CELLS; z++)
      {
        WORD brickType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z)];
        if (brickType == 0)
          continue;
        BrickDefn* brickDefn = m_brickSet->m_defns[MAJOR_CODE(brickType)];
        WORD otherType;

        switch (brickDefn->m_shape)
        {
          case SHAPE_CUBE:
            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x-1, y, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMIN))
            {
              if (brickDefn->m_hasTransparent)
                transCount++;  // rectangles
              else cubeCount++;  // rectangles
            }
            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+1, y, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMAX))
            {
              if (brickDefn->m_hasTransparent)
                transCount++;  // rectangles
              else cubeCount++;  // rectangles
            }
            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
            {
              if (brickDefn->m_hasTransparent)
                transCount++;  // rectangles
              else cubeCount++;  // rectangles
            }
            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y+1, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMAX))
            {
              if (brickDefn->m_hasTransparent)
                transCount++;  // rectangles
              else cubeCount++;  // rectangles
            }
            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z-1)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMIN))
            {
              if (brickDefn->m_hasTransparent)
                transCount++;  // rectangles
              else cubeCount++;  // rectangles
            }
            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z+1)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMAX))
            {
              if (brickDefn->m_hasTransparent)
                transCount++;  // rectangles
              else cubeCount++;  // rectangles
            }
            break;

          case SHAPE_SLAB:
          case SHAPE_CAP:
            // brickDefn for slabs and caps have sides set transparent so they will be
            // obscured only by another brick of the same type
            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x-1, y, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMIN))
              cubeCount++;  // rectangles

            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+1, y, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMAX))
              cubeCount++;  // rectangles

            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
              cubeCount++;  // rectangles

            // cap and slab are never obscured on top
            cubeCount++;  // rectangles

            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z-1)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMIN))
              cubeCount++;  // rectangles

            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z+1)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMAX))
              cubeCount++;  // rectangles
            break;

          case SHAPE_STAIR:
            // bottom of stair can be obscured, but sides and top depend
            // on orientation of stairs, so just draw them always
            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
              cubeCount++;  // rectangles

            // always draw top (3 rects), left and right sides (2 rects each) and 
            // front and back (1 rect each)
            cubeCount += 3 + 2*2 + 2*1;  // rectangles
            break;

          case SHAPE_COLUMN:
            // sides of a column are never obscured
            cubeCount += 4;  // outside faces

            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
              cubeCount++;  // rectangles

            otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y+1, z)];
            if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMAX))
              cubeCount++;  // rectangles
            break;

          case SHAPE_DEFN:
          {
            BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
            if (subDefn == NULL)
              subDefn = brickDefn->m_subDefns[0];
            shapeCount += subDefn->m_shapeDefn->m_len;
            break;
          }
        }
      }
    }
  }

  buffers.m_cubeCount = cubeCount;
  buffers.m_shapeCount = shapeCount;
  buffers.m_transCount = transCount;
}

//--------------------------------------------------------------
// create the vertexes and indexes for solid bricks
void BrickBlob::createSolidTriangles(
  BrickBuffers& buffers)
{
  for (int x = 0; x < BRICKBLOB_CELLS; x++)
  {
    for (int y = 0; y < BRICKBLOB_CELLS; y++)
    {
      for (int z = 0; z < BRICKBLOB_CELLS; z++)
      {
        WORD brickType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z)];
        if (brickType == 0)
          continue;

        BrickDefn* brickDefn = m_brickSet->m_defns[MAJOR_CODE(brickType)];
        switch (brickDefn->m_shape)
        {
          case SHAPE_CUBE:
            if (!brickDefn->m_hasTransparent)
              cubeTriangles(buffers.m_cubeVertexes, buffers.m_cubeIndexes, brickType, brickDefn, x, y, z);
            break;

          case SHAPE_SLAB:
            slabTriangles(buffers.m_cubeVertexes, buffers.m_cubeIndexes, brickType, brickDefn, x, y, z);
            break;

          case SHAPE_CAP:
            capTriangles(buffers.m_cubeVertexes, buffers.m_cubeIndexes, brickType, brickDefn, x, y, z);
            break;

          case SHAPE_COLUMN:
            columnTriangles(buffers.m_cubeVertexes, buffers.m_cubeIndexes, brickType, brickDefn, x, y, z);
            break;

          case SHAPE_STAIR:
            stairTriangles(buffers.m_cubeVertexes, buffers.m_cubeIndexes, brickType, brickDefn, x, y, z);
            break;

          case SHAPE_DEFN:
            shapeTriangles(buffers.m_shapeVertexes, buffers.m_shapeIndexes, brickType, brickDefn, x, y, z);
            break;
        }
      }
    }
  }
}

//--------------------------------------------------------------
// create sorted transparent cube vertexes and indexes
void BrickBlob::createTransTriangles(
  BrickBuffers& buffers)
{
//  mgDebug("createTrans(%d), sortEye=(%g,%g,%g)", buffers.m_transCount, 
//    buffers.m_sortEyePt.x, buffers.m_sortEyePt.y, buffers.m_sortEyePt.z);

  if (buffers.m_transCount == 0)
    return;

  // the eye position split the blob into 8 non-overlapping pieces
  // which can be drawn in any order.  generate each piece in sorted order
  int eyeX = (int) floor(buffers.m_sortEyePt.x);
  int eyeY = (int) floor(buffers.m_sortEyePt.y);
  int eyeZ = (int) floor(buffers.m_sortEyePt.z);

  // constrain to within blob
  int maxCell = BRICKBLOB_CELLS-1;
  eyeX = max(0, min(maxCell, eyeX));
  eyeY = max(0, min(maxCell, eyeY));
  eyeZ = max(0, min(maxCell, eyeZ));

  // traverse each block from corner point to eye.  
  // sortTransTriangles does not include end point in step
  mgVertexBuffer* transVertexes = buffers.m_transVertexes;
  mgIndexBuffer* transIndexes = buffers.m_transIndexes;

  sortTransTriangles(transVertexes, transIndexes, 0, 0, 0, eyeX, eyeY, eyeZ);
  sortTransTriangles(transVertexes, transIndexes, maxCell, 0, 0, eyeX-1, eyeY, eyeZ);
  sortTransTriangles(transVertexes, transIndexes, 0, 0, maxCell, eyeX, eyeY, eyeZ-1);
  sortTransTriangles(transVertexes, transIndexes, maxCell, 0, maxCell, eyeX-1, eyeY, eyeZ-1);

  sortTransTriangles(transVertexes, transIndexes, 0, maxCell, 0, eyeX, eyeY-1, eyeZ);
  sortTransTriangles(transVertexes, transIndexes, maxCell, maxCell, 0, eyeX-1, eyeY-1, eyeZ);
  sortTransTriangles(transVertexes, transIndexes, 0, maxCell, maxCell, eyeX, eyeY-1, eyeZ-1);
  sortTransTriangles(transVertexes, transIndexes, maxCell, maxCell, maxCell, eyeX-1, eyeY-1, eyeZ-1);
}

//--------------------------------------------------------------
// create the vertexes and indexes for transparent bricks
void BrickBlob::sortTransTriangles(
  mgVertexBuffer* transVertexes,
  mgIndexBuffer* transIndexes,
  int startX,
  int startY,
  int startZ,
  int endX,
  int endY,
  int endZ)
{
  int incX = (startX < endX) ? 1 : -1;
  int incY = (startY < endY) ? 1 : -1;
  int incZ = (startZ < endZ) ? 1 : -1;

  for (int x = startX; x != endX; x+= incX)
  {
    for (int y = startY; y != endY; y+= incY)
    {
      for (int z = startZ; z != endZ; z+= incZ)
      {
        WORD brickType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z)];
        if (brickType == 0)
          continue;

        BrickDefn* brickDefn = m_brickSet->m_defns[MAJOR_CODE(brickType)];
        if (brickDefn->m_shape == SHAPE_CUBE && brickDefn->m_hasTransparent)
        {
          cubeTriangles(transVertexes, transIndexes, brickType, brickDefn, x, y, z);
        }
      }
    }
  }
}

//--------------------------------------------------------------
// return true if needs update
BOOL BrickBlob::needsUpdate(
  const mgPoint3& eyePt)
{
  m_needsSort = sortRequired(eyePt);
  return m_bricksChanged || m_needsSort;
}

//--------------------------------------------------------------
// create updated display buffers
void BrickBlob::createUpdate(
  const mgPoint3& eyePt)
{
  if (m_bricksChanged)
    countTriangles(m_buffers2);
  else 
  {
    m_buffers2.m_cubeCount = m_buffers.m_cubeCount;
    m_buffers2.m_shapeCount = m_buffers.m_shapeCount;
    m_buffers2.m_transCount = m_buffers.m_transCount;
  }

  if (m_bricksChanged || sortRequired(eyePt))
  {
    m_buffers2.m_sortEyePt = eyePt;
    createTransBrickBuffers(m_buffers2);
    createTransTriangles(m_buffers2);
  }

  if (m_bricksChanged)
  {
    createSolidBrickBuffers(m_buffers2);
    createSolidTriangles(m_buffers2);
  }
  m_bricksChanged = false;
}

//--------------------------------------------------------------
// use updated buffers
void BrickBlob::useUpdate()
{
  // copy any new buffers in buffers2 into buffers

  if (m_buffers2.m_cubeVertexes != NULL)
  {
    m_buffers.m_cubeCount = m_buffers2.m_cubeCount;

    delete m_buffers.m_cubeVertexes;
    m_buffers.m_cubeVertexes = m_buffers2.m_cubeVertexes;
    m_buffers2.m_cubeVertexes = NULL;

    delete m_buffers.m_cubeIndexes;
    m_buffers.m_cubeIndexes = m_buffers2.m_cubeIndexes;
    m_buffers2.m_cubeIndexes = NULL;
  }

  if (m_buffers2.m_shapeVertexes != NULL)
  {
    m_buffers.m_shapeCount = m_buffers2.m_shapeCount;

    delete m_buffers.m_shapeVertexes;
    m_buffers.m_shapeVertexes = m_buffers2.m_shapeVertexes;
    m_buffers2.m_shapeVertexes = NULL;

    delete m_buffers.m_shapeIndexes;
    m_buffers.m_shapeIndexes = m_buffers2.m_shapeIndexes;
    m_buffers2.m_shapeIndexes = NULL;
  }

  if (m_buffers2.m_transVertexes != NULL)
  {
    m_buffers.m_transCount = m_buffers2.m_transCount;

    delete m_buffers.m_transVertexes;
    m_buffers.m_transVertexes = m_buffers2.m_transVertexes;
    m_buffers2.m_transVertexes = NULL;

    delete m_buffers.m_transIndexes;
    m_buffers.m_transIndexes = m_buffers2.m_transIndexes;
    m_buffers2.m_transIndexes = NULL;
  }

//  mgDebug("useBuffers(%d), sortEye=(%g,%g,%g)", 
//    m_buffers2.m_transCount,
//    m_buffers2.m_sortEyePt.x, m_buffers2.m_sortEyePt.y, m_buffers2.m_sortEyePt.z);

  m_sortEyePt = m_buffers2.m_sortEyePt;
  m_needsSort = false; // =-= debug
}

//--------------------------------------------------------------
// update animation 
BOOL BrickBlob::animate(
  double now,                         // current time (ms)
  double since)                       // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// create buffers, ready to send to display
void BrickBlob::createBuffers(
  const mgPoint3& eyePt)
{
  countTriangles(m_buffers);

  m_buffers.m_sortEyePt = eyePt;
  createTransBrickBuffers(m_buffers);
  createTransTriangles(m_buffers);

  createSolidBrickBuffers(m_buffers);
  createSolidTriangles(m_buffers);

  m_bricksChanged = false;

  // create selection buffer
}

//--------------------------------------------------------------
// delete any display buffers
void BrickBlob::deleteBuffers()
{
  m_buffers.deleteBuffers();
  m_buffers2.deleteBuffers();

  delete m_selectedVertexes;
  m_selectedVertexes = NULL;
}

//--------------------------------------------------------------
// return true if can render (buffers created)
BOOL BrickBlob::canRender()
{
  return m_buffers.m_cubeVertexes != NULL;
}

//--------------------------------------------------------------
// return true if object needs a sort call
BOOL BrickBlob::sortRequired(
  const mgPoint3& eyePt)
{
  // if no transparent, no sort required
  if (m_buffers.m_transCount == 0)
    return false;

  int oldCode, newCode;

  int maxCell = BRICKBLOB_CELLS;

  // test for eye movement on x, y, and z
  oldCode = (int) floor(m_sortEyePt.x);
  oldCode = max(0, min(maxCell, oldCode));

  newCode = (int) floor(eyePt.x);
  newCode = max(0, min(maxCell, newCode));
  if (newCode != oldCode)
    return true;

  oldCode = (int) floor(m_sortEyePt.y);
  oldCode = max(0, min(maxCell, oldCode));

  newCode = (int) floor(eyePt.y);
  newCode = max(0, min(maxCell, newCode));
  if (newCode != oldCode)
    return true;

  oldCode = (int) floor(m_sortEyePt.z);
  oldCode = max(0, min(maxCell, oldCode));

  newCode = (int) floor(eyePt.z);
  newCode = max(0, min(maxCell, newCode));
  if (newCode != oldCode)
    return true;

  return false;
}

//--------------------------------------------------------------
// update xmin edges from neighbor brick
void BrickBlob::updateXMinEdge(
  BrickBlob* other)
{
  for (int y = -1; y <= BRICKBLOB_CELLS; y++)
  {
    for (int z = -1; z <= BRICKBLOB_CELLS; z++)
    {
      m_bricks[BRICK_CELL(BRICKBLOB_SIZE, -1, y, z)] = 
        other->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, BRICKBLOB_CELLS-1, y, z)];
      m_lights[BRICK_CELL(BRICKBLOB_SIZE, -1, y, z)] = 
        other->m_lights[BRICK_CELL(BRICKBLOB_SIZE, BRICKBLOB_CELLS-1, y, z)];
    }
  }
  m_bricksChanged = true;
}

//--------------------------------------------------------------
// update xmin edges from neighbor brick
void BrickBlob::updateXMaxEdge(
  BrickBlob* other)
{
  for (int y = -1; y <= BRICKBLOB_CELLS; y++)
  {
    for (int z = -1; z <= BRICKBLOB_CELLS; z++)
    {
      m_bricks[BRICK_CELL(BRICKBLOB_SIZE, BRICKBLOB_CELLS, y, z)] = 
        other->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, 0, y, z)];
      m_lights[BRICK_CELL(BRICKBLOB_SIZE, BRICKBLOB_CELLS, y, z)] = 
        other->m_lights[BRICK_CELL(BRICKBLOB_SIZE, 0, y, z)];
    }
  }
  m_bricksChanged = true;
}

//--------------------------------------------------------------
// update zmin edges from neighbor brick
void BrickBlob::updateZMinEdge(
  BrickBlob* other)
{
  for (int y = -1; y <= BRICKBLOB_CELLS; y++)
  {
    for (int x = -1; x <= BRICKBLOB_CELLS; x++)
    {
      m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, -1)] = 
        other->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, BRICKBLOB_CELLS-1)];
      m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y, -1)] = 
        other->m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y, BRICKBLOB_CELLS-1)];
    }
  }
  m_bricksChanged = true;
}

//--------------------------------------------------------------
// update zmin edges from neighbor brick
void BrickBlob::updateZMaxEdge(
  BrickBlob* other)
{
  for (int y = -1; y <= BRICKBLOB_CELLS; y++)
  {
    for (int x = -1; x <= BRICKBLOB_CELLS; x++)
    {
      m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, BRICKBLOB_CELLS)] =
        other->m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, 0)];
      m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y, BRICKBLOB_CELLS)] =
        other->m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y, 0)];
    }
  }
  m_bricksChanged = true;
}

//--------------------------------------------------------------
// step to next cell on ray
void BrickBlob::cellStep(
  mgPoint3& pt, 
  const mgPoint3& ray,
  double& dist, 
  int& cellX,
  int& cellY, 
  int& cellZ,
  int& sideHit)
{
  // move the point along the ray, through either the x, y, or z planes,
  // whichever is closer
  double minT = INT_MAX;  // infinity
  double t;

  if (ray.x > 0.0)
  {
    t = (cellX + 1.0 - pt.x) / ray.x;
    if (t < minT)
    {
      minT = t;
      sideHit = BRICK_FACE_XMIN;
    }
  }
  else if (ray.x < 0.0)
  {
    t = (cellX - pt.x) / ray.x;
    if (t < minT)
    {
      minT = t;
      sideHit = BRICK_FACE_XMAX;
    }
  }

  if (ray.y > 0.0)
  {
    t = (cellY + 1.0 - pt.y) / ray.y;
    if (t < minT)
    {
      minT = t;
      sideHit = BRICK_FACE_YMIN;
    }
  }
  else if (ray.y < 0.0)
  {
    t = (cellY - pt.y) / ray.y;
    if (t < minT)
    {
      minT = t;
      sideHit = BRICK_FACE_YMAX;
    }
  }

  if (ray.z > 0.0)
  {
    t = (cellZ + 1.0 - pt.z) / ray.z;
    if (t < minT)
    {
      minT = t;
      sideHit = BRICK_FACE_ZMIN;
    }
  }
  else if (ray.z < 0.0)
  {
    t = (cellZ - pt.z) / ray.z;
    if (t < minT)
    {
      minT = t;
      sideHit = BRICK_FACE_ZMAX;
    }
  }

  // advance to next cell based on side hit
  switch (sideHit)
  {
    case BRICK_FACE_XMIN:
      cellX++;  // went through to min side of next cell
      break;
    case BRICK_FACE_XMAX:
      cellX--;  // went through to max side of next cell
      break;
    case BRICK_FACE_YMIN:
      cellY++;  // went through to min side of next cell
      break;
    case BRICK_FACE_YMAX:
      cellY--;  // went through to max side of next cell
      break;
    case BRICK_FACE_ZMIN:
      cellZ++;  // went through to min side of next cell
      break;
    case BRICK_FACE_ZMAX:
      cellZ--;  // went through to max side of next cell
      break;
  }

  // advance point based on distance travelled
  pt.x += minT * ray.x;
  pt.y += minT * ray.y;
  pt.z += minT * ray.z;

  dist += minT;
}

//--------------------------------------------------------------
// return first brick at ray.  return false if none
BOOL BrickBlob::rayIntersect(
  const mgPoint3& origin,
  const mgPoint3& ray,
  double limit,
  double& dist,
  mgPoint3& hitPt,
  int& face,
  int& cellX,
  int& cellY,
  int& cellZ)
{
  mgPoint3 pt(origin);

  // get the initial cell
  cellX = (int) floor(pt.x);
  cellY = (int) floor(pt.y);
  cellZ = (int) floor(pt.z);
  dist = 0.0;

  // point assumed to start within the blob, in an air brick
  while (true)
  {
    // step to next cell edge
    cellStep(pt, ray, dist, cellX, cellY, cellZ, face);

    // if we're out of the blob, we're done
    if (cellX < 0 || cellX >= BRICKBLOB_CELLS ||
        cellY < 0 || cellY >= BRICKBLOB_CELLS ||
        cellZ < 0 || cellZ >= BRICKBLOB_CELLS)
      return false;

    // if distance over limit, we're done
    if (dist > limit)
      return false;

    short brickType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, cellX, cellY, cellZ)];

    // if not air, we've hit a block
    if (brickType != 0)
    {
      hitPt = pt;
      return true;
    }
  }
}

//--------------------------------------------------------------
// return lighting for vertex
void BrickBlob::lightXMin(
  int x,
  int y,
  int z,
  float& ambientLight,
  float& skyLight,
  float& blockLight)
{
  ambientLight = 1.0f;
  skyLight = 0.0f;
  blockLight = 0.0f;

  int exposed = 0;
  for (int dy = -1; dy < 1; dy++)
  {
    for (int dz = -1; dz < 1; dz++)
    {
      // if the inner blocks around origin are present, ray is blocked
      WORD brick = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x-1, y+dy, z+dz)];
      if (brick != 0)
      {
        BrickDefn* defn = m_brickSet->m_defns[MAJOR_CODE(brick)];
        if (defn->m_hasTransparent)
        {
          ambientLight -= TRANSPARENT_AMBIENT;
          brick = 0;  // average in brightness below
        }
        else ambientLight -= 0.25f;
      }

      if (brick == 0)
      {
        BYTE inten = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x-1, y+dy, z+dz)];
        skyLight += m_intenLevels[inten>>4];
        blockLight += m_intenLevels[inten & 0xF];
        exposed++;
      }
    }
  }

  // average the exposed light values
  if (exposed != 0)
  {
    skyLight = skyLight/exposed;
    blockLight = blockLight/exposed;
  }
}

//--------------------------------------------------------------
// return lighting for vertex
void BrickBlob::lightXMax(
  int x,
  int y,
  int z,
  float& ambientLight,
  float& skyLight,
  float& blockLight)
{
  ambientLight = 1.0;
  skyLight = 0.0f;
  blockLight = 0.0f;

  int exposed = 0;
  for (int dy = -1; dy < 1; dy++)
  {
    for (int dz = -1; dz < 1; dz++)
    {
      // if the inner blocks around origin are present, ray is blocked
      WORD brick = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y+dy, z+dz)];
      if (brick != 0)
      {
        BrickDefn* defn = m_brickSet->m_defns[MAJOR_CODE(brick)];
        if (defn->m_hasTransparent)
        {
          ambientLight -= TRANSPARENT_AMBIENT;
          brick = 0;  // average in brightness below
        }
        else ambientLight -= 0.25f;
      }

      if (brick == 0)
      {
        BYTE inten = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y+dy, z+dz)];
        skyLight += m_intenLevels[inten>>4];
        blockLight += m_intenLevels[inten & 0xF];
        exposed++;
      }
    }
  }

  // average the exposed light values
  if (exposed != 0)
  {
    skyLight = skyLight/exposed;
    blockLight = blockLight/exposed;
  }
}

//--------------------------------------------------------------
// return lighting for vertex
void BrickBlob::lightYMin(
  int x,
  int y,
  int z,
  float& ambientLight,
  float& skyLight,
  float& blockLight)
{
  ambientLight = 1.0;
  skyLight = 0.0f;
  blockLight = 0.0f;

  int exposed = 0;
  for (int dx = -1; dx < 1; dx++)
  {
    for (int dz = -1; dz < 1; dz++)
    {
      // if the inner blocks around origin are present, ray is blocked
      WORD brick = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+dx, y-1, z+dz)];
      if (brick != 0)
      {
        BrickDefn* defn = m_brickSet->m_defns[MAJOR_CODE(brick)];
        if (defn->m_hasTransparent)
        {
          ambientLight -= TRANSPARENT_AMBIENT;
          brick = 0;  // average in brightness below
        }
        else ambientLight -= 0.25f;
      }

      if (brick == 0)
      {
        BYTE inten = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x+dx, y-1, z+dz)];
        skyLight += m_intenLevels[inten>>4];
        blockLight += m_intenLevels[inten & 0xF];
        exposed++;
      }
    }
  }

  // average the exposed light values
  if (exposed != 0)
  {
    skyLight = skyLight/exposed;
    blockLight = blockLight/exposed;
  }
}

//--------------------------------------------------------------
// return lighting for vertex
void BrickBlob::lightYMax(
  int x,
  int y,
  int z,
  float& ambientLight,
  float& skyLight,
  float& blockLight)
{
  ambientLight = 1.0;
  skyLight = 0.0f;
  blockLight = 0.0f;

  int exposed = 0;
  for (int dx = -1; dx < 1; dx++)
  {
    for (int dz = -1; dz < 1; dz++)
    {
      // if the inner blocks around origin are present, ray is blocked
      WORD brick = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+dx, y, z+dz)];
      if (brick != 0)
      {
        BrickDefn* defn = m_brickSet->m_defns[MAJOR_CODE(brick)];
        if (defn->m_hasTransparent)
        {
          ambientLight -= TRANSPARENT_AMBIENT;
          brick = 0;  // average in brightness below
        }
        else ambientLight -= 0.25f;
      }

      if (brick == 0)
      {
        BYTE inten = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x+dx, y, z+dz)];
        skyLight += m_intenLevels[inten>>4];
        blockLight += m_intenLevels[inten & 0xF];
        exposed++;
      }
    }
  }

  // average the exposed light values
  if (exposed != 0)
  {
    skyLight = skyLight/exposed;
    blockLight = blockLight/exposed;
  }
}

//--------------------------------------------------------------
// return lighting for vertex
void BrickBlob::lightZMin(
  int x,
  int y,
  int z,
  float& ambientLight,
  float& skyLight,
  float& blockLight)
{
  ambientLight = 1.0;
  skyLight = 0.0f;
  blockLight = 0.0f;

  int exposed = 0;
  for (int dy = -1; dy < 1; dy++)
  {
    for (int dx = -1; dx < 1; dx++)
    {
      // if the inner blocks around origin are present, ray is blocked
      WORD brick = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+dx, y+dy, z-1)];
      if (brick != 0)
      {
        BrickDefn* defn = m_brickSet->m_defns[MAJOR_CODE(brick)];
        if (defn->m_hasTransparent)
        {
          ambientLight -= TRANSPARENT_AMBIENT;
          brick = 0;  // average in brightness below
        }
        else ambientLight -= 0.25f;
      }

      if (brick == 0)
      {
        BYTE inten = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x+dx, y+dy, z-1)];
        skyLight += m_intenLevels[inten>>4];
        blockLight += m_intenLevels[inten & 0xF];
        exposed++;
      }
    }
  }

  // average the exposed light values
  if (exposed != 0)
  {
    skyLight = skyLight/exposed;
    blockLight = blockLight/exposed;
  }
}

//--------------------------------------------------------------
// return lighting for vertex
void BrickBlob::lightZMax(
  int x,
  int y,
  int z,
  float& ambientLight,
  float& skyLight,
  float& blockLight)
{
  ambientLight = 1.0;
  skyLight = 0.0f;
  blockLight = 0.0f;

  int exposed = 0;
  for (int dy = -1; dy < 1; dy++)
  {
    for (int dx = -1; dx < 1; dx++)
    {
      // if the inner blocks around origin are present, ray is blocked
      WORD brick = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+dx, y+dy, z)];
      if (brick != 0)
      {
        BrickDefn* defn = m_brickSet->m_defns[MAJOR_CODE(brick)];
        if (defn->m_hasTransparent)
        {
          ambientLight -= TRANSPARENT_AMBIENT;
          brick = 0;  // average in brightness below
        }
        else ambientLight -= 0.25f;
      }

      if (brick == 0)
      {
        BYTE inten = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x+dx, y+dy, z)];
        skyLight += m_intenLevels[inten >> 4];
        blockLight += m_intenLevels[inten & 0xF];
        exposed++;
      }
    }
  }

  // average the exposed light values, return 0 to 1 value
  if (exposed != 0)
  {
    skyLight = skyLight/exposed;
    blockLight = blockLight/exposed;
  }
}

