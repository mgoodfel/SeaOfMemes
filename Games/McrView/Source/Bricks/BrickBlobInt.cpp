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
#include "BrickBlobInt.h"

#include "VertexCubeInt.h"
#include "VertexShapeInt.h"

const int MAX_INTEN = 15;

//--------------------------------------------------------------
// load the shaders we use
void BrickBlobInt::loadShaders()
{
  VertexCubeInt::loadShader("cubesInt");
  VertexShapeInt::loadShader("shapesInt");
}

//--------------------------------------------------------------
// constructor
BrickBlobInt::BrickBlobInt()
{
}

//--------------------------------------------------------------
// destructor
BrickBlobInt::~BrickBlobInt()
{
}

//--------------------------------------------------------------
// create solid brick buffers
void BrickBlobInt::createSolidBrickBuffers(
  BrickBuffers& buffers)
{
  // delete old buffers
  delete buffers.m_cubeVertexes;
  buffers.m_cubeVertexes = NULL;
  delete buffers.m_cubeIndexes;
  buffers.m_cubeIndexes = NULL;
  delete buffers.m_shapeVertexes;
  buffers.m_shapeVertexes = NULL;
  delete buffers.m_shapeIndexes;
  buffers.m_shapeIndexes = NULL;

  // create cube buffers.  count is rectangles
  buffers.m_cubeVertexes = VertexCubeInt::newBuffer(buffers.m_cubeCount*4);
  buffers.m_cubeIndexes = mgDisplay->newIndexBuffer(buffers.m_cubeCount*6, false, (buffers.m_cubeCount*4) > 65536);

  buffers.m_shapeVertexes = VertexShapeInt::newBuffer(buffers.m_shapeCount*3);
}

//--------------------------------------------------------------
// create transparent brick buffers
void BrickBlobInt::createTransBrickBuffers(
  BrickBuffers& buffers)
{
  // delete old buffers
  delete buffers.m_transVertexes;
  buffers.m_transVertexes = NULL;
  delete buffers.m_transIndexes;
  buffers.m_transIndexes = NULL;

  if (buffers.m_transCount == 0)
    return;

  // create transparent cube buffers
  buffers.m_transVertexes = VertexCubeInt::newBuffer(buffers.m_transCount*4);
  buffers.m_transIndexes = mgDisplay->newIndexBuffer(buffers.m_transCount*6, false, (buffers.m_transCount*4) > 65536);
}

//--------------------------------------------------------------
// render the blob
void BrickBlobInt::render()
{
  mgDisplay->setShader("cubesInt");
  mgDisplay->setShaderUniform("cubesInt", "torchColor", m_torchColor);
  mgDisplay->setShaderUniform("cubesInt", "fogColor", m_fogColor);
  mgDisplay->setShaderUniform("cubesInt", "fogBotHeight", (float) m_fogBotHeight);
  mgDisplay->setShaderUniform("cubesInt", "fogBotInten", (float) m_fogBotInten);
  mgDisplay->setShaderUniform("cubesInt", "fogTopHeight", (float) m_fogTopHeight);
  mgDisplay->setShaderUniform("cubesInt", "fogTopInten", (float) m_fogTopInten);
  mgDisplay->setShaderUniform("cubesInt", "fogMaxDist", (float) m_fogMaxDist);
  mgDisplay->setTexture(m_textureArray);
  mgDisplay->draw(MG_TRIANGLES, m_buffers.m_cubeVertexes, m_buffers.m_cubeIndexes);

  mgDisplay->setShader("shapesInt");
  mgDisplay->setShaderUniform("shapesInt", "torchColor", m_torchColor);
  mgDisplay->setShaderUniform("shapesInt", "fogColor", m_fogColor);
  mgDisplay->setShaderUniform("shapesInt", "fogBotHeight", (float) m_fogBotHeight);
  mgDisplay->setShaderUniform("shapesInt", "fogBotInten", (float) m_fogBotInten);
  mgDisplay->setShaderUniform("shapesInt", "fogTopHeight", (float) m_fogTopHeight);
  mgDisplay->setShaderUniform("shapesInt", "fogTopInten", (float) m_fogTopInten);
  mgDisplay->setShaderUniform("shapesInt", "fogMaxDist", (float) m_fogMaxDist);
  mgDisplay->setTexture(m_textureArray);
  mgDisplay->draw(MG_TRIANGLES, m_buffers.m_shapeVertexes);
}

//--------------------------------------------------------------
// render the blob transparent data
void BrickBlobInt::renderTransparent()
{
  if (m_buffers.m_transCount == 0)
    return;

//  if (m_needsSort)
//    mgDisplay->setMatColor(1, 0, 0);

  mgDisplay->setShader("cubesInt");
  mgDisplay->setShaderUniform("cubesInt", "torchColor", m_torchColor);
  mgDisplay->setShaderUniform("cubesInt", "fogColor", m_fogColor);
  mgDisplay->setShaderUniform("cubesInt", "fogBotHeight", (float) m_fogBotHeight);
  mgDisplay->setShaderUniform("cubesInt", "fogBotInten", (float) m_fogBotInten);
  mgDisplay->setShaderUniform("cubesInt", "fogTopHeight", (float) m_fogTopHeight);
  mgDisplay->setShaderUniform("cubesInt", "fogTopInten", (float) m_fogTopInten);
  mgDisplay->setShaderUniform("cubesInt", "fogMaxDist", (float) m_fogMaxDist);
  mgDisplay->setTexture(m_textureArray);
  mgDisplay->draw(MG_TRIANGLES, m_buffers.m_transVertexes, m_buffers.m_transIndexes);

//  mgDisplay->setMatColor(1, 1, 1);
}

//--------------------------------------------------------------
// return size in bytes of object in memory
int BrickBlobInt::getSystemMemUsed()
{
  return sizeof(BrickBlobInt) + 3*BRICKBLOB_SIZE*BRICKBLOB_SIZE*BRICKBLOB_SIZE;
}

//--------------------------------------------------------------
// return size in bytes of object in display
int BrickBlobInt::getDisplayMemUsed()
{
  // count cube faces (rectangles)
  int size = (m_buffers.m_cubeCount+m_buffers.m_transCount)*4*sizeof(VertexCubeInt); 

  // count shape triangles
  size += m_buffers.m_shapeCount*3*sizeof(VertexShapeInt);

  // count indexes
  int cubeIndexSize = (m_buffers.m_cubeCount*6 > 65536) ? sizeof(UINT32) : sizeof(UINT16);
  int transIndexSize = (m_buffers.m_transCount*6 > 65536) ? sizeof(UINT32) : sizeof(UINT16);

  size += m_buffers.m_cubeCount*6*cubeIndexSize;
  size += m_buffers.m_transCount*6*transIndexSize;

  return size;
}

//--------------------------------------------------------------
// add vertexes and indexes for cube
void BrickBlobInt::cubeTriangles(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  VertexCubeInt tl, tr, bl, br;

  // set texture coordinates (integer, fract) for each.
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  float ambientLight, skyLight, blockLight;
  // default lighting to full on.  sets new value if not a light
  tl.setLamp(1.0f);
  tr.setLamp(1.0f);
  bl.setLamp(1.0f);
  br.setLamp(1.0f);

  WORD otherType;
  int baseVertex;

  // build XMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x-1, y, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMIN))
  {
    tl.setNormal(-1, 0, 0);
    tr.setNormal(-1, 0, 0);
    bl.setNormal(-1, 0, 0);
    br.setNormal(-1, 0, 0);

    tl.setTextureIndex(subDefn->m_xmin);
    tr.setTextureIndex(subDefn->m_xmin);
    bl.setTextureIndex(subDefn->m_xmin);
    br.setTextureIndex(subDefn->m_xmin);

    tl.setPoint(x, 0, y+1, 0, z+1, 0); 
    tr.setPoint(x, 0, y+1, 0, z,   0);      
    bl.setPoint(x, 0, y,   0, z+1, 0); 
    br.setPoint(x, 0, y,   0, z,   0);      

    if (!subDefn->m_light)                                         
    {
      lightXMin(x, y+1, z+1, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightXMin(x, y+1, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightXMin(x, y,   z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight);
      lightXMin(x, y,   z, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build XMAX face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+1, y, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMAX))
  {
    tl.setNormal(1, 0, 0);
    tr.setNormal(1, 0, 0);
    bl.setNormal(1, 0, 0);
    br.setNormal(1, 0, 0);

    tl.setTextureIndex(subDefn->m_xmax);
    tr.setTextureIndex(subDefn->m_xmax);
    bl.setTextureIndex(subDefn->m_xmax);
    br.setTextureIndex(subDefn->m_xmax);

    tl.setPoint(x+1, 0, y+1, 0,  z,   0);      
    tr.setPoint(x+1, 0, y+1, 0,  z+1, 0); 
    bl.setPoint(x+1, 0, y,   0,  z,   0);      
    br.setPoint(x+1, 0, y,   0,  z+1, 0); 

    if (!subDefn->m_light)
    {
      lightXMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y,   z, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build YMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
  {
    tl.setNormal(0, -1, 0);
    tr.setNormal(0, -1, 0);
    bl.setNormal(0, -1, 0);
    br.setNormal(0, -1, 0);

    tl.setTextureIndex(subDefn->m_ymin);
    tr.setTextureIndex(subDefn->m_ymin);
    bl.setTextureIndex(subDefn->m_ymin);
    br.setTextureIndex(subDefn->m_ymin);

    tl.setPoint(x,   0, y, 0, z,   0); 
    tr.setPoint(x+1, 0, y, 0, z,   0); 
    bl.setPoint(x,   0, y, 0, z+1, 0);
    br.setPoint(x+1, 0, y, 0, z+1, 0);

    if (!subDefn->m_light)
    {
      lightYMin(x,   y, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x,   y, z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build YMAX face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y+1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMAX))
  {
    tl.setNormal(0, 1, 0);
    tr.setNormal(0, 1, 0);
    bl.setNormal(0, 1, 0);
    br.setNormal(0, 1, 0);

    tl.setTextureIndex(subDefn->m_ymax);
    tr.setTextureIndex(subDefn->m_ymax);
    bl.setTextureIndex(subDefn->m_ymax);
    br.setTextureIndex(subDefn->m_ymax);

    tl.setPoint(x,   0, y+1, 0, z+1, 0);        
    tr.setPoint(x+1, 0, y+1, 0, z+1, 0);        
    bl.setPoint(x,   0, y+1, 0, z,   0);             
    br.setPoint(x+1, 0, y+1, 0, z,   0);

    if (!subDefn->m_light)
    {
      lightYMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMax(x,   y+1, z, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build ZMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z-1)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMIN))
  {
    tl.setNormal(0, 0, -1);
    tr.setNormal(0, 0, -1);
    bl.setNormal(0, 0, -1);
    br.setNormal(0, 0, -1);

    tl.setTextureIndex(subDefn->m_zmin);
    tr.setTextureIndex(subDefn->m_zmin);
    bl.setTextureIndex(subDefn->m_zmin);
    br.setTextureIndex(subDefn->m_zmin);

    tl.setPoint(x,   0,  y+1, 0,  z, 0);      
    tr.setPoint(x+1, 0,  y+1, 0,  z, 0);      
    bl.setPoint(x,   0,  y,   0,  z, 0);      
    br.setPoint(x+1, 0,  y,   0,  z, 0);      

    if (!subDefn->m_light)
    {
      lightZMin(x,   y+1, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x+1, y+1, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x,   y,   z, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x+1, y,   z, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build ZMAX face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z+1)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMAX))
  {
    tl.setNormal(0, 0, 1);
    tr.setNormal(0, 0, 1);
    bl.setNormal(0, 0, 1);
    br.setNormal(0, 0, 1);

    tl.setTextureIndex(subDefn->m_zmax);
    tr.setTextureIndex(subDefn->m_zmax);
    bl.setTextureIndex(subDefn->m_zmax);
    br.setTextureIndex(subDefn->m_zmax);

    tl.setPoint(x+1,  0,  y+1, 0,  z+1, 0); 
    tr.setPoint(x,    0,  y+1, 0,  z+1, 0); 
    bl.setPoint(x+1,  0,  y,   0,  z+1, 0); 
    br.setPoint(x,    0,  y,   0,  z+1, 0); 

    if (!subDefn->m_light)
    {
      lightZMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightZMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightZMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightZMax(x,   y,   z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }
}

//--------------------------------------------------------------
// add vertexes and indexes for slab
void BrickBlobInt::slabTriangles(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  VertexCubeInt tl, tr, bl, br;

  int hsize = (1 << VERTEX_CUBEINT_PT_FRACT) / 2;
  int htv = (1 << VERTEX_CUBEINT_TUV_FRACT) / 2;

  // set texture coordinates (integer, fract) for each.
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  float ambientLight, skyLight, blockLight;
  // default lighting to full on.  sets new value if not a light
  tl.setLamp(1.0f);
  tr.setLamp(1.0f);
  bl.setLamp(1.0f);
  br.setLamp(1.0f);

  WORD otherType;
  int baseVertex;

  // build top and bottom of the slab full size

  // build YMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
  {
    tl.setNormal(0, -1, 0);
    tr.setNormal(0, -1, 0);
    bl.setNormal(0, -1, 0);
    br.setNormal(0, -1, 0);

    tl.setTextureIndex(subDefn->m_ymin);
    tr.setTextureIndex(subDefn->m_ymin);
    bl.setTextureIndex(subDefn->m_ymin);
    br.setTextureIndex(subDefn->m_ymin);

    tl.setPoint(x,   0, y, 0, z,   0); 
    tr.setPoint(x+1, 0, y, 0, z,   0); 
    bl.setPoint(x,   0, y, 0, z+1, 0);
    br.setPoint(x+1, 0, y, 0, z+1, 0);

    if (!subDefn->m_light)
    {
      lightYMin(x, y, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x, y, z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build YMAX face.  always drawn
  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setTextureIndex(subDefn->m_ymax);
  tr.setTextureIndex(subDefn->m_ymax);
  bl.setTextureIndex(subDefn->m_ymax);
  br.setTextureIndex(subDefn->m_ymax);

  tl.setPoint(x,   0, y, hsize, z+1, 0);        
  tr.setPoint(x+1, 0, y, hsize, z+1, 0);        
  bl.setPoint(x,   0, y, hsize, z, 0);             
  br.setPoint(x+1, 0, y, hsize, z, 0);

  if (!subDefn->m_light)
  {
    lightYMax(x, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x, y+1, z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // set texture coordinates for sides
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);

  // build short sides of slab

  // build XMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x-1, y, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMIN))
  {
    tl.setNormal(-1, 0, 0);
    tr.setNormal(-1, 0, 0);
    bl.setNormal(-1, 0, 0);
    br.setNormal(-1, 0, 0);

    tl.setTextureIndex(subDefn->m_xmin);
    tr.setTextureIndex(subDefn->m_xmin);
    bl.setTextureIndex(subDefn->m_xmin);
    br.setTextureIndex(subDefn->m_xmin);

    tl.setPoint(x, 0, y, hsize,  z+1, 0); 
    tr.setPoint(x, 0, y, hsize,  z,   0);      
    bl.setPoint(x, 0, y, 0,      z+1, 0); 
    br.setPoint(x, 0, y, 0,      z,   0);      

    if (!subDefn->m_light)
    {
      lightXMin(x, y+1, z+1, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightXMin(x, y+1, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightXMin(x, y, z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightXMin(x, y, z, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build XMAX face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+1, y, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMAX))
  {
    tl.setNormal(1, 0, 0);
    tr.setNormal(1, 0, 0);
    bl.setNormal(1, 0, 0);
    br.setNormal(1, 0, 0);

    tl.setTextureIndex(subDefn->m_xmax);
    tr.setTextureIndex(subDefn->m_xmax);
    bl.setTextureIndex(subDefn->m_xmax);
    br.setTextureIndex(subDefn->m_xmax);

    tl.setPoint(x+1, 0, y, hsize, z,   0);      
    tr.setPoint(x+1, 0, y, hsize, z+1, 0); 
    bl.setPoint(x+1, 0, y, 0,     z,   0);      
    br.setPoint(x+1, 0, y, 0,     z+1, 0); 

    if (!subDefn->m_light)
    {
      lightXMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y, z, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build ZMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z-1)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMIN))
  {
    tl.setNormal(0, 0, -1);
    tr.setNormal(0, 0, -1);
    bl.setNormal(0, 0, -1);
    br.setNormal(0, 0, -1);

    tl.setTextureIndex(subDefn->m_zmin);
    tr.setTextureIndex(subDefn->m_zmin);
    bl.setTextureIndex(subDefn->m_zmin);
    br.setTextureIndex(subDefn->m_zmin);

    tl.setPoint(x,   0, y, hsize, z, 0);      
    tr.setPoint(x+1, 0, y, hsize, z, 0);      
    bl.setPoint(x,   0, y, 0,     z, 0);      
    br.setPoint(x+1, 0, y, 0,     z, 0);      

    if (!subDefn->m_light)
    {
      lightZMin(x, y+1, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x+1, y+1, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x, y, z, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x+1, y, z, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build ZMAX face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z+1)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMAX))
  {
    tl.setNormal(0, 0, 1);
    tr.setNormal(0, 0, 1);
    bl.setNormal(0, 0, 1);
    br.setNormal(0, 0, 1);

    tl.setTextureIndex(subDefn->m_zmax);
    tr.setTextureIndex(subDefn->m_zmax);
    bl.setTextureIndex(subDefn->m_zmax);
    br.setTextureIndex(subDefn->m_zmax);

    tl.setPoint(x+1, 0, y, hsize,   z+1, 0); 
    tr.setPoint(x,   0, y, hsize,   z+1, 0); 
    bl.setPoint(x+1, 0, y, 0,       z+1, 0); 
    br.setPoint(x,   0, y, 0,       z+1, 0); 

    if (!subDefn->m_light)
    {
      lightZMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight);
      lightZMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightZMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightZMax(x,   y,   z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }
}

//--------------------------------------------------------------
// add vertexes and indexes for cap
void BrickBlobInt::capTriangles(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  VertexCubeInt tl, tr, bl, br;

  int hsize = (1 << VERTEX_CUBEINT_PT_FRACT) / 8;
  int htv = (1 << VERTEX_CUBEINT_TUV_FRACT) / 8;

  // set texture coordinates (integer, fract) for each.
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  float ambientLight, skyLight, blockLight;
  // default lighting to full on.  sets new value if not a light
  tl.setLamp(1.0f);
  tr.setLamp(1.0f);
  bl.setLamp(1.0f);
  br.setLamp(1.0f);

  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  WORD otherType;
  int baseVertex;

  // build top and bottom of the cap full size

  // build YMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
  {
    tl.setNormal(0, -1, 0);
    tr.setNormal(0, -1, 0);
    bl.setNormal(0, -1, 0);
    br.setNormal(0, -1, 0);

    tl.setTextureIndex(subDefn->m_ymin);
    tr.setTextureIndex(subDefn->m_ymin);
    bl.setTextureIndex(subDefn->m_ymin);
    br.setTextureIndex(subDefn->m_ymin);

    tl.setPoint(x,   0, y, 0, z,   0); 
    tr.setPoint(x+1, 0, y, 0, z,   0); 
    bl.setPoint(x,   0, y, 0, z+1, 0);
    br.setPoint(x+1, 0, y, 0, z+1, 0);

    if (!subDefn->m_light)
    {
      lightYMin(x,   y, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x,   y, z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build YMAX face.  always drawn
  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setTextureIndex(subDefn->m_ymax);
  tr.setTextureIndex(subDefn->m_ymax);
  bl.setTextureIndex(subDefn->m_ymax);
  br.setTextureIndex(subDefn->m_ymax);

  tl.setPoint(x,   0, y, hsize, z+1, 0);        
  tr.setPoint(x+1, 0, y, hsize, z+1, 0);        
  bl.setPoint(x,   0, y, hsize, z, 0);             
  br.setPoint(x+1, 0, y, hsize, z, 0);

  if (!subDefn->m_light)
  {
    lightYMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x,   y+1, z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // set texture coordinates for sides
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);

  // build short sides of cap

  // build XMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x-1, y, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMIN))
  {
    tl.setNormal(-1, 0, 0);
    tr.setNormal(-1, 0, 0);
    bl.setNormal(-1, 0, 0);
    br.setNormal(-1, 0, 0);

    tl.setTextureIndex(subDefn->m_xmin);
    tr.setTextureIndex(subDefn->m_xmin);
    bl.setTextureIndex(subDefn->m_xmin);
    br.setTextureIndex(subDefn->m_xmin);

    tl.setPoint(x, 0, y, hsize,  z+1, 0); 
    tr.setPoint(x, 0, y, hsize,  z,   0);      
    bl.setPoint(x, 0, y, 0,      z+1, 0); 
    br.setPoint(x, 0, y, 0,      z,   0);      

    if (!subDefn->m_light)
    {
      lightXMin(x, y+1, z+1, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightXMin(x, y+1, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightXMin(x, y,   z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightXMin(x, y,   z, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build XMAX face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x+1, y, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_XMAX))
  {
    tl.setNormal(1, 0, 0);
    tr.setNormal(1, 0, 0);
    bl.setNormal(1, 0, 0);
    br.setNormal(1, 0, 0);

    tl.setTextureIndex(subDefn->m_xmax);
    tr.setTextureIndex(subDefn->m_xmax);
    bl.setTextureIndex(subDefn->m_xmax);
    br.setTextureIndex(subDefn->m_xmax);

    tl.setPoint(x+1, 0, y, hsize, z,   0);      
    tr.setPoint(x+1, 0, y, hsize, z+1, 0); 
    bl.setPoint(x+1, 0, y, 0,     z,   0);      
    br.setPoint(x+1, 0, y, 0,     z+1, 0); 

    if (!subDefn->m_light)
    {
      lightXMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y,   z, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightXMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build ZMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z-1)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMIN))
  {
    tl.setNormal(0, 0, -1);
    tr.setNormal(0, 0, -1);
    bl.setNormal(0, 0, -1);
    br.setNormal(0, 0, -1);

    tl.setTextureIndex(subDefn->m_zmin);
    tr.setTextureIndex(subDefn->m_zmin);
    bl.setTextureIndex(subDefn->m_zmin);
    br.setTextureIndex(subDefn->m_zmin);

    tl.setPoint(x,   0, y, hsize, z, 0);      
    tr.setPoint(x+1, 0, y, hsize, z, 0);      
    bl.setPoint(x,   0, y, 0,     z, 0);      
    br.setPoint(x+1, 0, y, 0,     z, 0);      

    if (!subDefn->m_light)
    {
      lightZMin(x,   y+1, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x+1, y+1, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x,   y,   z, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightZMin(x+1, y,   z, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);

    indexes->addRectIndex(baseVertex);
  }

  // build ZMAX face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y, z+1)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_ZMAX))
  {
    tl.setNormal(0, 0, 1);
    tr.setNormal(0, 0, 1);
    bl.setNormal(0, 0, 1);
    br.setNormal(0, 0, 1);

    tl.setTextureIndex(subDefn->m_zmax);
    tr.setTextureIndex(subDefn->m_zmax);
    bl.setTextureIndex(subDefn->m_zmax);
    br.setTextureIndex(subDefn->m_zmax);

    tl.setPoint(x+1, 0, y, hsize,   z+1, 0); 
    tr.setPoint(x,   0, y, hsize,   z+1, 0); 
    bl.setPoint(x+1, 0, y, 0,       z+1, 0); 
    br.setPoint(x,   0, y, 0,       z+1, 0); 

    if (!subDefn->m_light)
    {
      lightZMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightZMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightZMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightZMax(x,   y,   z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }
}

//--------------------------------------------------------------
// add vertexes and indexes for column
void BrickBlobInt::columnTriangles(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  VertexCubeInt tl, tr, bl, br;

  float ambientLight, skyLight, blockLight;
  // default lighting to full on.  sets new value if not a light
  tl.setLamp(1.0f);
  tr.setLamp(1.0f);
  bl.setLamp(1.0f);
  br.setLamp(1.0f);

  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  int ltu = (int) floor((1 << VERTEX_CUBEINT_TUV_FRACT) * 0.4);
  int htu = (int) ceil((1 << VERTEX_CUBEINT_TUV_FRACT) * 0.6);

  int lsize = (int) floor((1 << VERTEX_CUBEINT_PT_FRACT) * 0.4);
  int hsize = (int) ceil((1 << VERTEX_CUBEINT_PT_FRACT) * 0.6);

  // set texture coordinates for top and bottom
  tl.setTextureUV(0, ltu,   0, ltu);
  tr.setTextureUV(0, htu,   0, ltu);
  bl.setTextureUV(0, ltu,   0, htu);
  br.setTextureUV(0, htu,   0, htu);

  WORD otherType;
  int baseVertex;

  // build top and bottom of the column

  // build YMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
  {
    tl.setNormal(0, -1, 0);
    tr.setNormal(0, -1, 0);
    bl.setNormal(0, -1, 0);
    br.setNormal(0, -1, 0);

    tl.setTextureIndex(subDefn->m_ymin);
    tr.setTextureIndex(subDefn->m_ymin);
    bl.setTextureIndex(subDefn->m_ymin);
    br.setTextureIndex(subDefn->m_ymin);

    tl.setPoint(x, lsize, y, 0, z, lsize); 
    tr.setPoint(x, hsize, y, 0, z, lsize); 
    bl.setPoint(x, lsize, y, 0, z, hsize);
    br.setPoint(x, hsize, y, 0, z, hsize);

    if (!subDefn->m_light)
    {
      lightYMin(x,   y, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x,   y, z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build YMAX face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y+1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMAX))
  {
    tl.setNormal(0, 1, 0);
    tr.setNormal(0, 1, 0);
    bl.setNormal(0, 1, 0);
    br.setNormal(0, 1, 0);

    tl.setTextureIndex(subDefn->m_ymax);
    tr.setTextureIndex(subDefn->m_ymax);
    bl.setTextureIndex(subDefn->m_ymax);
    br.setTextureIndex(subDefn->m_ymax);

    tl.setPoint(x, lsize, y+1, 0, z, hsize);        
    tr.setPoint(x, hsize, y+1, 0, z, hsize);        
    bl.setPoint(x, lsize, y+1, 0, z, lsize);             
    br.setPoint(x, hsize, y+1, 0, z, lsize);

    if (!subDefn->m_light)
    {
      lightYMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMax(x,   y+1, z, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // set texture coordinates for sides
  tl.setTextureUV(0, ltu,   0, 0);
  tr.setTextureUV(0, htu,   0, 0);
  bl.setTextureUV(0, ltu,   1, 0);
  br.setTextureUV(0, htu,   1, 0);

  // build sides of column.  never obscured, so always add them

  // build XMIN face
  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setTextureIndex(subDefn->m_xmin);
  tr.setTextureIndex(subDefn->m_xmin);
  bl.setTextureIndex(subDefn->m_xmin);
  br.setTextureIndex(subDefn->m_xmin);

  tl.setPoint(x, lsize, y+1, 0,    z, hsize); 
  tr.setPoint(x, lsize, y+1, 0,    z, lsize);      
  bl.setPoint(x, lsize, y,   0,    z, hsize); 
  br.setPoint(x, lsize, y,   0,    z, lsize);      

  if (!subDefn->m_light)
  {
    lightXMin(x, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y,   z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y,   z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build XMAX face
  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setTextureIndex(subDefn->m_xmax);
  tr.setTextureIndex(subDefn->m_xmax);
  bl.setTextureIndex(subDefn->m_xmax);
  br.setTextureIndex(subDefn->m_xmax);

  tl.setPoint(x, hsize,  y+1, 0, z, lsize);      
  tr.setPoint(x, hsize,  y+1, 0, z, hsize); 
  bl.setPoint(x, hsize,  y,   0, z, lsize);      
  br.setPoint(x, hsize,  y,   0, z, hsize); 

  if (!subDefn->m_light)
  {
    lightXMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y,   z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build ZMIN face
  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setTextureIndex(subDefn->m_zmin);
  tr.setTextureIndex(subDefn->m_zmin);
  bl.setTextureIndex(subDefn->m_zmin);
  br.setTextureIndex(subDefn->m_zmin);

  tl.setPoint(x, lsize,  y+1, 0,   z, lsize);      
  tr.setPoint(x, hsize,  y+1, 0,   z, lsize);      
  bl.setPoint(x, lsize,  y,   0,   z, lsize);      
  br.setPoint(x, hsize,  y,   0,   z, lsize);      

  if (!subDefn->m_light)
  {
    lightZMin(x,   y+1, z, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x,   y,   z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y,   z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build ZMAX face
  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setTextureIndex(subDefn->m_zmax);
  tr.setTextureIndex(subDefn->m_zmax);
  bl.setTextureIndex(subDefn->m_zmax);
  br.setTextureIndex(subDefn->m_zmax);

  tl.setPoint(x, hsize,   y+1, 0,  z, hsize); 
  tr.setPoint(x, lsize,   y+1, 0,  z, hsize); 
  bl.setPoint(x, hsize,   y,   0,  z, hsize); 
  br.setPoint(x, lsize,   y,   0,  z, hsize); 

  if (!subDefn->m_light)
  {
    lightZMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x,   y,   z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);
}

//--------------------------------------------------------------
// add vertexes and indexes for stair
void BrickBlobInt::stairTriangles(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  switch (subDefn->m_dir)
  {
    case 0: stairTriangles_0(vertexes, indexes, brickType, brickDefn, x, y, z);
      break;
    case 1: stairTriangles_1(vertexes, indexes, brickType, brickDefn, x, y, z);
      break;
    case 2: stairTriangles_2(vertexes, indexes, brickType, brickDefn, x, y, z);
      break;
    case 3: stairTriangles_3(vertexes, indexes, brickType, brickDefn, x, y, z);
      break;
  }
}

//--------------------------------------------------------------
// add vertexes and indexes for stair
void BrickBlobInt::stairTriangles_0(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  VertexCubeInt tl, tr, bl, br;

  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  float ambientLight, skyLight, blockLight;
  // default lighting to full on.  sets new value if not a light
  tl.setLamp(1.0f);
  tr.setLamp(1.0f);
  bl.setLamp(1.0f);
  br.setLamp(1.0f);

  int hsize = (1 << VERTEX_CUBEINT_PT_FRACT) / 2;
  int htv = (1 << VERTEX_CUBEINT_TUV_FRACT) / 2;

  WORD otherType;
  int baseVertex;

  // build bottom full size
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  // build YMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
  {
    tl.setNormal(0, -1, 0);
    tr.setNormal(0, -1, 0);
    bl.setNormal(0, -1, 0);
    br.setNormal(0, -1, 0);

    tl.setTextureIndex(subDefn->m_ymin);
    tr.setTextureIndex(subDefn->m_ymin);
    bl.setTextureIndex(subDefn->m_ymin);
    br.setTextureIndex(subDefn->m_ymin);

    tl.setPoint(x,   0, y, 0, z,   0);
    tr.setPoint(x+1, 0, y, 0, z,   0);
    bl.setPoint(x,   0, y, 0, z+1, 0);
    br.setPoint(x+1, 0, y, 0, z+1, 0);

    if (!subDefn->m_light)
    {
      lightYMin(x, y, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x, y, z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build back (XMAX) side, full height
  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setTextureIndex(subDefn->m_xmax);
  tr.setTextureIndex(subDefn->m_xmax);
  bl.setTextureIndex(subDefn->m_xmax);
  br.setTextureIndex(subDefn->m_xmax);

  tl.setPoint(x+1, 0, y+1, 0, z,   0);      
  tr.setPoint(x+1, 0, y+1, 0, z+1, 0); 
  bl.setPoint(x+1, 0, y,   0, z,   0);      
  br.setPoint(x+1, 0, y,   0, z+1, 0); 

  if (!subDefn->m_light)
  {
    lightXMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y, z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y, z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build front (XMIN) side, half-high
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);

  tl.setTextureIndex(subDefn->m_xmin);
  tr.setTextureIndex(subDefn->m_xmin);
  bl.setTextureIndex(subDefn->m_xmin);
  br.setTextureIndex(subDefn->m_xmin);

  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(x, 0, y, hsize, z+1, 0); 
  tr.setPoint(x, 0, y, hsize, z,   0);      
  bl.setPoint(x, 0, y, 0,     z+1, 0); 
  br.setPoint(x, 0, y, 0,     z,   0);      
                            
  if (!subDefn->m_light)
  {
    lightXMin(x, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y, z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // face of top step
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);

  tl.setPoint(x, hsize, y+1, 0,     z+1, 0); 
  tr.setPoint(x, hsize, y+1, 0,     z,   0);      
  bl.setPoint(x, hsize, y,   hsize, z+1, 0); 
  br.setPoint(x, hsize, y,   hsize, z,   0);      

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build top (YMAX) face.  always drawn
  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setTextureIndex(subDefn->m_ymax);
  tr.setTextureIndex(subDefn->m_ymax);
  bl.setTextureIndex(subDefn->m_ymax);
  br.setTextureIndex(subDefn->m_ymax);

  if (!subDefn->m_light)
  {
    lightYMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x, y+1, z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x, y+1, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // bottom riser
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x, hsize, y, hsize, z+1, 0);        
  tr.setPoint(x, hsize, y, hsize, z,   0);
  bl.setPoint(x, 0,     y, hsize, z+1, 0);        
  br.setPoint(x, 0,     y, hsize, z,   0);             

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top riser
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);

  tl.setPoint(x+1, 0,     y+1, 0, z+1, 0);
  tr.setPoint(x+1, 0,     y+1, 0, z,   0);
  bl.setPoint(x,   hsize, y+1, 0, z+1, 0);        
  br.setPoint(x,   hsize, y+1, 0, z,   0);             

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build left (ZMIN) face.  always drawn
  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setTextureIndex(subDefn->m_zmin);
  tr.setTextureIndex(subDefn->m_zmin);
  bl.setTextureIndex(subDefn->m_zmin);
  br.setTextureIndex(subDefn->m_zmin);

  if (!subDefn->m_light)
  {
    lightZMin(x, y+1, z, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x, y, z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // base of side
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x,   0, y, hsize, z, 0);      
  tr.setPoint(x+1, 0, y, hsize, z, 0);      
  bl.setPoint(x,   0, y, 0,     z, 0);      
  br.setPoint(x+1, 0, y, 0,     z, 0);      

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top of side
  tl.setTextureUV(0, htv,   0, 0);
  tr.setTextureUV(1,   0,   0, 0);
  bl.setTextureUV(0, htv,   0, htv);
  br.setTextureUV(1,   0,   0, htv);

  tl.setPoint(x,   hsize, y+1, 0,     z, 0);      
  tr.setPoint(x+1, 0,     y+1, 0,     z, 0);      
  bl.setPoint(x,   hsize, y,   hsize, z, 0);      
  br.setPoint(x+1, 0,     y,   hsize, z, 0);      

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build right (ZMAX) face.  always drawn
  tl.setTextureIndex(subDefn->m_zmax);
  tr.setTextureIndex(subDefn->m_zmax);
  bl.setTextureIndex(subDefn->m_zmax);
  br.setTextureIndex(subDefn->m_zmax);

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  if (!subDefn->m_light)
  {
    lightZMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x+1, y, z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x, y, z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // base of side
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x+1, 0, y, hsize, z+1, 0); 
  tr.setPoint(x,   0, y, hsize, z+1, 0); 
  bl.setPoint(x+1, 0, y, 0,     z+1, 0); 
  br.setPoint(x,   0, y, 0,     z+1, 0); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top of side
  tl.setTextureUV(0,   0,   0, 0);
  tr.setTextureUV(0, htv,   0, 0);
  bl.setTextureUV(0,   0,   0, htv);
  br.setTextureUV(0, htv,   0, htv);

  tl.setPoint(x+1, 0,     y+1, 0,     z+1, 0); 
  tr.setPoint(x,   hsize, y+1, 0,     z+1, 0); 
  bl.setPoint(x+1, 0,     y,   hsize, z+1, 0); 
  br.setPoint(x,   hsize, y,   hsize, z+1, 0); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);
}

//--------------------------------------------------------------
// add vertexes and indexes for stair
void BrickBlobInt::stairTriangles_1(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  VertexCubeInt tl, tr, bl, br;

  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  float ambientLight, skyLight, blockLight;
  // default lighting to full on.  sets new value if not a light
  tl.setLamp(1.0f);
  tr.setLamp(1.0f);
  bl.setLamp(1.0f);
  br.setLamp(1.0f);

  int hsize = (1 << VERTEX_CUBEINT_PT_FRACT) / 2;
  int htv = (1 << VERTEX_CUBEINT_TUV_FRACT) / 2;

  WORD otherType;
  int baseVertex;

  // build bottom full size
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  // build YMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
  {
    tl.setNormal(0, -1, 0);
    tr.setNormal(0, -1, 0);
    bl.setNormal(0, -1, 0);
    br.setNormal(0, -1, 0);

    tl.setTextureIndex(subDefn->m_ymin);
    tr.setTextureIndex(subDefn->m_ymin);
    bl.setTextureIndex(subDefn->m_ymin);
    br.setTextureIndex(subDefn->m_ymin);

    tl.setPoint(x+1, 0, y, 0, z,   0);
    tr.setPoint(x+1, 0, y, 0, z+1, 0);
    bl.setPoint(x,   0, y, 0, z,   0);
    br.setPoint(x,   0, y, 0, z+1, 0);

    if (!subDefn->m_light)
    {
      lightYMin(x+1, y, z,   ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x,   y, z,   ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x,   y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build back (ZMIN) side, full height
  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setTextureIndex(subDefn->m_zmin);
  tr.setTextureIndex(subDefn->m_zmin);
  bl.setTextureIndex(subDefn->m_zmin);
  br.setTextureIndex(subDefn->m_zmin);

  tl.setPoint(x,   0, y+1, 0, z, 0);      
  tr.setPoint(x+1, 0, y+1, 0, z, 0); 
  bl.setPoint(x,   0, y,   0, z, 0);      
  br.setPoint(x+1, 0, y,   0, z, 0); 

  if (!subDefn->m_light)
  {
    lightZMin(x,   y+1, z, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x,   y,   z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y,   z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build front (ZMAX) side, half-high
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);

  tl.setTextureIndex(subDefn->m_zmax);
  tr.setTextureIndex(subDefn->m_zmax);
  bl.setTextureIndex(subDefn->m_zmax);
  br.setTextureIndex(subDefn->m_zmax);

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setPoint(x+1, 0, y, hsize, z+1, 0); 
  tr.setPoint(x,   0, y, hsize, z+1, 0);      
  bl.setPoint(x+1, 0, y, 0,     z+1, 0); 
  br.setPoint(x,   0, y, 0,     z+1, 0);      
                            
  if (!subDefn->m_light)
  {
    lightZMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x,   y,   z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // face of top step
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);

  tl.setPoint(x+1, 0, y+1, 0,     z, hsize); 
  tr.setPoint(x,   0, y+1, 0,     z, hsize);      
  bl.setPoint(x+1, 0, y,   hsize, z, hsize); 
  br.setPoint(x,   0, y,   hsize, z, hsize);      

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build top (YMAX) face.  always drawn
  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setTextureIndex(subDefn->m_ymax);
  tr.setTextureIndex(subDefn->m_ymax);
  bl.setTextureIndex(subDefn->m_ymax);
  br.setTextureIndex(subDefn->m_ymax);

  if (!subDefn->m_light)
  {
    lightYMax(x, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x, y+1, z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // bottom riser
  tl.setTextureUV(0, 0,   1, 0);
  tr.setTextureUV(1, 0,   1, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);

  tl.setPoint(x,   0, y, hsize, z+1, 0);
  tr.setPoint(x+1, 0, y, hsize, z+1, 0);        
  bl.setPoint(x,   0, y, hsize, z, hsize);             
  br.setPoint(x+1, 0, y, hsize, z, hsize);        

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top riser
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   0, 0);
  br.setTextureUV(1, 0,   0, 0);

  tl.setPoint(x,   0, y+1, 0, z, hsize);
  tr.setPoint(x+1, 0, y+1, 0, z, hsize);
  bl.setPoint(x,   0, y+1, 0, z, 0);
  br.setPoint(x+1, 0, y+1, 0, z, 0);

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build left (XMIN) face.  always drawn
  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setTextureIndex(subDefn->m_xmin);
  tr.setTextureIndex(subDefn->m_xmin);
  bl.setTextureIndex(subDefn->m_xmin);
  br.setTextureIndex(subDefn->m_xmin);

  if (!subDefn->m_light)
  {
    lightXMin(x, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y,   z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y,   z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // base of side
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x, 0, y, hsize, z+1, 0);      
  tr.setPoint(x, 0, y, hsize, z, 0);      
  bl.setPoint(x, 0, y, 0,     z+1, 0);      
  br.setPoint(x, 0, y, 0,     z, 0);      

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top of side
  tl.setTextureUV(0, htv,   0, 0);
  tr.setTextureUV(0, 0,     0, 0);
  bl.setTextureUV(0, htv,   0, htv);
  br.setTextureUV(0, 0,     0, htv);

  tl.setPoint(x, 0, y+1, 0,     z, hsize);
  tr.setPoint(x, 0, y+1, 0,     z, 0);
  bl.setPoint(x, 0, y,   hsize, z, hsize);
  br.setPoint(x, 0, y,   hsize, z, 0);

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build right (XMAX) face.  always drawn
  tl.setTextureIndex(subDefn->m_xmax);
  tr.setTextureIndex(subDefn->m_xmax);
  bl.setTextureIndex(subDefn->m_xmax);
  br.setTextureIndex(subDefn->m_xmax);

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  if (!subDefn->m_light)
  {
    lightXMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y+1, z,   ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y,   z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
  }

  // base of side
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x+1, 0, y, hsize, z,   0); 
  tr.setPoint(x+1, 0, y, hsize, z+1, 0); 
  bl.setPoint(x+1, 0, y, 0,     z,   0); 
  br.setPoint(x+1, 0, y, 0,     z+1, 0); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top of side
  tl.setTextureUV(0, 0,     0, 0);
  tr.setTextureUV(0, htv,   0, 0);
  bl.setTextureUV(0, 0,     0, htv);
  br.setTextureUV(0, htv,   0, htv);

  tl.setPoint(x+1, 0,     y+1, 0,     z, 0);
  tr.setPoint(x+1, 0,     y+1, 0,     z, hsize); 
  bl.setPoint(x+1, 0,     y,   hsize, z, 0);
  br.setPoint(x+1, 0,     y,   hsize, z, hsize); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);
}

//--------------------------------------------------------------
// add vertexes and indexes for stair
void BrickBlobInt::stairTriangles_2(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  VertexCubeInt tl, tr, bl, br;

  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  float ambientLight, skyLight, blockLight;
  // default lighting to full on.  sets new value if not a light
  tl.setLamp(1.0f);
  tr.setLamp(1.0f);
  bl.setLamp(1.0f);
  br.setLamp(1.0f);

  int hsize = (1 << VERTEX_CUBEINT_PT_FRACT) / 2;
  int htv = (1 << VERTEX_CUBEINT_TUV_FRACT) / 2;

  WORD otherType;
  int baseVertex;

  // build bottom full size
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  // build YMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
  {
    tl.setNormal(0, -1, 0);
    tr.setNormal(0, -1, 0);
    bl.setNormal(0, -1, 0);
    br.setNormal(0, -1, 0);

    tl.setTextureIndex(subDefn->m_ymin);
    tr.setTextureIndex(subDefn->m_ymin);
    bl.setTextureIndex(subDefn->m_ymin);
    br.setTextureIndex(subDefn->m_ymin);

    tl.setPoint(x,   0, y, 0, z,   0);
    tr.setPoint(x+1, 0, y, 0, z,   0);
    bl.setPoint(x,   0, y, 0, z+1, 0);
    br.setPoint(x+1, 0, y, 0, z+1, 0);

    if (!subDefn->m_light)
    {
      lightYMin(x,   y, z, ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x,   y, z+1, ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build back (XMIN) side, full height
  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setTextureIndex(subDefn->m_xmin);
  tr.setTextureIndex(subDefn->m_xmin);
  bl.setTextureIndex(subDefn->m_xmin);
  br.setTextureIndex(subDefn->m_xmin);

  tl.setPoint(x, 0, y+1, 0, z+1, 0); 
  tr.setPoint(x, 0, y+1, 0, z,   0);      
  bl.setPoint(x, 0, y,   0, z+1, 0); 
  br.setPoint(x, 0, y,   0, z,   0);      

  if (!subDefn->m_light)
  {
    lightXMin(x, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y, z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build front (XMAX) side, half-high
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);

  tl.setTextureIndex(subDefn->m_xmax);
  tr.setTextureIndex(subDefn->m_xmax);
  bl.setTextureIndex(subDefn->m_xmax);
  br.setTextureIndex(subDefn->m_xmax);

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setPoint(x+1, 0, y, hsize, z,   0);      
  tr.setPoint(x+1, 0, y, hsize, z+1, 0); 
  bl.setPoint(x+1, 0, y, 0,     z,   0);      
  br.setPoint(x+1, 0, y, 0,     z+1, 0); 
                            
  if (!subDefn->m_light)
  {
    lightXMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y, z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y, z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // face of top step
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);

  tl.setPoint(x, hsize, y+1, 0,     z,   0);      
  tr.setPoint(x, hsize, y+1, 0,     z+1, 0); 
  bl.setPoint(x, hsize, y,   hsize, z,   0);      
  br.setPoint(x, hsize, y,   hsize, z+1, 0); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build top (YMAX) face.  always drawn
  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setTextureIndex(subDefn->m_ymax);
  tr.setTextureIndex(subDefn->m_ymax);
  bl.setTextureIndex(subDefn->m_ymax);
  br.setTextureIndex(subDefn->m_ymax);

  if (!subDefn->m_light)
  {
    lightYMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x, y+1, z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x, y+1, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // bottom riser
  tl.setTextureUV(0, 0,   1, 0);
  tr.setTextureUV(1, 0,   1, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);
                         
  tl.setPoint(x+1, 0,     y, hsize, z+1, 0);        
  tr.setPoint(x+1, 0,     y, hsize, z,   0);
  bl.setPoint(x,   hsize, y, hsize, z+1, 0);        
  br.setPoint(x,   hsize, y, hsize, z,   0);             

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top riser
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   0, 0);
  br.setTextureUV(1, 0,   0, 0);

  tl.setPoint(x, hsize, y+1, 0, z+1, 0);
  tr.setPoint(x, hsize, y+1, 0, z,   0);
  bl.setPoint(x, 0,     y+1, 0, z+1, 0);        
  br.setPoint(x, 0,     y+1, 0, z,   0);             

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build left (ZMIN) face.  always drawn
  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setTextureIndex(subDefn->m_zmin);
  tr.setTextureIndex(subDefn->m_zmin);
  bl.setTextureIndex(subDefn->m_zmin);
  br.setTextureIndex(subDefn->m_zmin);

  if (!subDefn->m_light)
  {
    lightZMin(x,   y+1, z, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x,   y,   z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y,   z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // base of side
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x,   0, y, hsize, z, 0);      
  tr.setPoint(x+1, 0, y, hsize, z, 0);      
  bl.setPoint(x,   0, y, 0,     z, 0);      
  br.setPoint(x+1, 0, y, 0,     z, 0);      

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top of side
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(0, htv, 0, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(0, htv, 0, htv);

  tl.setPoint(x, 0,     y+1, 0,     z, 0);      
  tr.setPoint(x, hsize, y+1, 0,     z, 0);      
  bl.setPoint(x, 0,     y,   hsize, z, 0);      
  br.setPoint(x, hsize, y,   hsize, z, 0);      

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build right (ZMAX) face.  always drawn
  tl.setTextureIndex(subDefn->m_zmax);
  tr.setTextureIndex(subDefn->m_zmax);
  bl.setTextureIndex(subDefn->m_zmax);
  br.setTextureIndex(subDefn->m_zmax);

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  if (!subDefn->m_light)
  {
    lightZMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x+1, y, z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x, y, z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // base of side
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x+1, 0, y, hsize, z+1, 0); 
  tr.setPoint(x,   0, y, hsize, z+1, 0); 
  bl.setPoint(x+1, 0, y, 0,     z+1, 0); 
  br.setPoint(x,   0, y, 0,     z+1, 0); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top of side
  tl.setTextureUV(0, htv, 0, 0);
  tr.setTextureUV(0, 0,   0, 0);
  bl.setTextureUV(0, htv, 0, htv);
  br.setTextureUV(0, 0,   0, htv);

  tl.setPoint(x, hsize, y+1, 0,     z+1, 0); 
  tr.setPoint(x, 0,     y+1, 0,     z+1, 0); 
  bl.setPoint(x, hsize, y,   hsize, z+1, 0); 
  br.setPoint(x, 0,     y,   hsize, z+1, 0); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);
}

//--------------------------------------------------------------
// add vertexes and indexes for stair
void BrickBlobInt::stairTriangles_3(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  VertexCubeInt tl, tr, bl, br;

  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  float ambientLight, skyLight, blockLight;
  // default lighting to full on.  sets new value if not a light
  tl.setLamp(1.0f);
  tr.setLamp(1.0f);
  bl.setLamp(1.0f);
  br.setLamp(1.0f);

  int hsize = (1 << VERTEX_CUBEINT_PT_FRACT) / 2;
  int htv = (1 << VERTEX_CUBEINT_TUV_FRACT) / 2;

  WORD otherType;
  int baseVertex;

  // build bottom full size
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  // build YMIN face
  otherType = m_bricks[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  if (!isObscuredBy(brickType, brickDefn, otherType, BRICK_FACE_YMIN))
  {
    tl.setNormal(0, -1, 0);
    tr.setNormal(0, -1, 0);
    bl.setNormal(0, -1, 0);
    br.setNormal(0, -1, 0);

    tl.setTextureIndex(subDefn->m_ymin);
    tr.setTextureIndex(subDefn->m_ymin);
    bl.setTextureIndex(subDefn->m_ymin);
    br.setTextureIndex(subDefn->m_ymin);

    tl.setPoint(x+1, 0, y, 0, z,   0);
    tr.setPoint(x+1, 0, y, 0, z+1, 0);
    bl.setPoint(x,   0, y, 0, z,   0);
    br.setPoint(x,   0, y, 0, z+1, 0);

    if (!subDefn->m_light)
    {
      lightYMin(x+1, y, z,   ambientLight, skyLight, blockLight);
      tl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x+1, y, z+1, ambientLight, skyLight, blockLight);
      tr.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x,   y, z,   ambientLight, skyLight, blockLight);
      bl.setLight(ambientLight, skyLight, blockLight); 
      lightYMin(x,   y, z+1, ambientLight, skyLight, blockLight);
      br.setLight(ambientLight, skyLight, blockLight); 
    }

    baseVertex = vertexes->getLength();
    tl.addTo(vertexes);
    tr.addTo(vertexes);
    bl.addTo(vertexes);
    br.addTo(vertexes);
    indexes->addRectIndex(baseVertex);
  }

  // build back (ZMAX) side, full height
  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setTextureIndex(subDefn->m_zmax);
  tr.setTextureIndex(subDefn->m_zmax);
  bl.setTextureIndex(subDefn->m_zmax);
  br.setTextureIndex(subDefn->m_zmax);

  tl.setPoint(x+1, 0, y+1, 0, z+1, 0); 
  tr.setPoint(x,   0, y+1, 0, z+1, 0);      
  bl.setPoint(x+1, 0, y,   0, z+1, 0); 
  br.setPoint(x,   0, y,   0, z+1, 0);      

  if (!subDefn->m_light)
  {
    lightZMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x,   y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMax(x,   y,   z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build front (XMIN) side, half-high
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);

  tl.setTextureIndex(subDefn->m_zmin);
  tr.setTextureIndex(subDefn->m_zmin);
  bl.setTextureIndex(subDefn->m_zmin);
  br.setTextureIndex(subDefn->m_zmin);

  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(x,   0, y, hsize, z, 0);      
  tr.setPoint(x+1, 0, y, hsize, z, 0); 
  bl.setPoint(x,   0, y, 0,     z, 0);      
  br.setPoint(x+1, 0, y, 0,     z, 0); 
                            
  if (!subDefn->m_light)
  {
    lightZMin(x,   y+1, z, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x,   y,   z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightZMin(x+1, y,   z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // face of top step
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);

  tl.setPoint(x,   0, y+1, 0,     z, hsize);      
  tr.setPoint(x+1, 0, y+1, 0,     z, hsize); 
  bl.setPoint(x,   0, y,   hsize, z, hsize);      
  br.setPoint(x+1, 0, y,   hsize, z, hsize); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build top (YMAX) face.  always drawn
  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setTextureIndex(subDefn->m_ymax);
  tr.setTextureIndex(subDefn->m_ymax);
  bl.setTextureIndex(subDefn->m_ymax);
  br.setTextureIndex(subDefn->m_ymax);

  if (!subDefn->m_light)
  {
    lightYMax(x, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x, y+1, z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightYMax(x+1, y+1, z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // bottom riser
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x,   0, y, hsize, z, hsize);
  tr.setPoint(x+1, 0, y, hsize, z, hsize);        
  bl.setPoint(x,   0, y, hsize, z, 0);             
  br.setPoint(x+1, 0, y, hsize, z, 0);        

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top riser
  tl.setTextureUV(0, 0,   0, 0);
  tr.setTextureUV(1, 0,   0, 0);
  bl.setTextureUV(0, 0,   0, htv);
  br.setTextureUV(1, 0,   0, htv);

  tl.setPoint(x,   0, y+1, 0, z+1, 0);
  tr.setPoint(x+1, 0, y+1, 0, z+1, 0);
  bl.setPoint(x,   0, y+1, 0, z,   hsize);
  br.setPoint(x+1, 0, y+1, 0, z,   hsize);

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build left (XMIN) face.  always drawn
  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setTextureIndex(subDefn->m_xmin);
  tr.setTextureIndex(subDefn->m_xmin);
  bl.setTextureIndex(subDefn->m_xmin);
  br.setTextureIndex(subDefn->m_xmin);

  if (!subDefn->m_light)
  {
    lightXMin(x, y+1, z+1, ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y+1, z, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y,   z+1, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
    lightXMin(x, y,   z, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
  }

  // base of side
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x, 0, y, hsize, z+1, 0);      
  tr.setPoint(x, 0, y, hsize, z, 0);      
  bl.setPoint(x, 0, y, 0,     z+1, 0);      
  br.setPoint(x, 0, y, 0,     z, 0);      

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top of side
  tl.setTextureUV(0, 0,     0, 0);
  tr.setTextureUV(0, htv,   0, 0);
  bl.setTextureUV(0, 0,     0, htv);
  br.setTextureUV(0, htv,   0, htv);

  tl.setPoint(x, 0, y+1, 0,     z+1, 0);
  tr.setPoint(x, 0, y+1, 0,     z,   hsize);
  bl.setPoint(x, 0, y,   hsize, z+1, 0);
  br.setPoint(x, 0, y,   hsize, z,   hsize);

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // build right (XMAX) face.  always drawn
  tl.setTextureIndex(subDefn->m_xmax);
  tr.setTextureIndex(subDefn->m_xmax);
  bl.setTextureIndex(subDefn->m_xmax);
  br.setTextureIndex(subDefn->m_xmax);

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  if (!subDefn->m_light)
  {
    lightXMax(x+1, y+1, z+1, ambientLight, skyLight, blockLight);
    tr.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y+1, z,   ambientLight, skyLight, blockLight);
    tl.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y,   z+1, ambientLight, skyLight, blockLight);
    br.setLight(ambientLight, skyLight, blockLight); 
    lightXMax(x+1, y,   z, ambientLight, skyLight, blockLight);
    bl.setLight(ambientLight, skyLight, blockLight); 
  }

  // base of side
  tl.setTextureUV(0, 0,   0, htv);
  tr.setTextureUV(1, 0,   0, htv);
  bl.setTextureUV(0, 0,   1, 0);
  br.setTextureUV(1, 0,   1, 0);

  tl.setPoint(x+1, 0, y, hsize, z,   0); 
  tr.setPoint(x+1, 0, y, hsize, z+1, 0); 
  bl.setPoint(x+1, 0, y, 0,     z,   0); 
  br.setPoint(x+1, 0, y, 0,     z+1, 0); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);

  // top of side
  tl.setTextureUV(0, htv,   0, 0);
  tr.setTextureUV(1, 0,     0, 0);
  bl.setTextureUV(0, htv,   0, htv);
  br.setTextureUV(1, 0,     0, htv);

  tl.setPoint(x+1, 0,     y+1, 0,     z,   hsize);
  tr.setPoint(x+1, 0,     y+1, 0,     z+1, 0); 
  bl.setPoint(x+1, 0,     y,   hsize, z,   hsize);
  br.setPoint(x+1, 0,     y,   hsize, z+1, 0); 

  baseVertex = vertexes->getLength();
  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);
  indexes->addRectIndex(baseVertex);
}

//--------------------------------------------------------------
// add vertexes and indexes for shape
void BrickBlobInt::shapeTriangles(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int brickType,
  const BrickDefn* brickDefn,
  int x,
  int y,
  int z)
{
  BrickSubDefn* subDefn = brickDefn->m_subDefns[MINOR_CODE(brickType)];
  if (subDefn == NULL)
    subDefn = brickDefn->m_subDefns[0];

  const mgMatrix4* dir = &m_brickSet->m_directions[subDefn->m_dir];

  BYTE light;
  light = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x-1, y, z)];
  float xminSky = m_intenLevels[light >> 4];
  float xminBlock = m_intenLevels[light & 0xF];
  light = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x+1, y, z)];
  float xmaxSky = m_intenLevels[light >> 4];
  float xmaxBlock = m_intenLevels[light & 0xF];
  light = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y-1, z)];
  float yminSky = m_intenLevels[light >> 4];
  float yminBlock = m_intenLevels[light & 0xF];
  light = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y+1, z)];
  float ymaxSky = m_intenLevels[light >> 4];
  float ymaxBlock = m_intenLevels[light & 0xF];
  light = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y, z-1)];
  float zminSky = m_intenLevels[light >> 4];
  float zminBlock = m_intenLevels[light & 0xF];
  light = m_lights[BRICK_CELL(BRICKBLOB_SIZE, x, y, z+1)];
  float zmaxSky = m_intenLevels[light >> 4];
  float zmaxBlock = m_intenLevels[light & 0xF];

  // we have brightness of adjacent cubes.  take the brightest
  // one, since light is shining in on 3D shape.  Average
  // doesn't work because some adjacent cells are full and
  // have brightness=0.
  float skyInten = max(xminSky, xmaxSky);
  skyInten = max(skyInten, max(yminSky, ymaxSky));
  skyInten = max(skyInten, max(zminSky, zmaxSky));

  float blockInten = max(xminBlock, xmaxBlock);
  blockInten = max(blockInten, max(yminBlock, ymaxBlock));
  blockInten = max(blockInten, max(zminBlock, zmaxBlock));

  VertexShapeInt v;
  for (int i = 0; i < subDefn->m_shapeDefn->m_len; i++)
  {
    ShapePoint* p = &subDefn->m_shapeDefn->m_vertexes[i];

    mgPoint3 outPt;
    dir->mapPt(p->m_pt, outPt);
    v.setPoint((float) (x+outPt.x), (float) (y+outPt.y), (float) (z+outPt.z));

    if (subDefn->m_light)
      v.setLamp(1.0f);
    else v.setLight((float) p->m_ambient, skyInten, blockInten);

    v.setTextureUV((float) p->m_texcoord.x, (float) p->m_texcoord.y);

    // use the correct material
    switch((int) (0.5+p->m_texcoord.z))
    {
      case 0: v.setTextureIndex(subDefn->m_xmin);
        break;
      case 1: v.setTextureIndex(subDefn->m_xmax);
        break;
      case 2: v.setTextureIndex(subDefn->m_ymin);
        break;
      case 3: v.setTextureIndex(subDefn->m_ymax);
        break;
      case 4: v.setTextureIndex(subDefn->m_zmin);
        break;
      case 5: v.setTextureIndex(subDefn->m_zmax);
        break;
    }

    // add the vertex
    v.addTo(vertexes);
  }
}

