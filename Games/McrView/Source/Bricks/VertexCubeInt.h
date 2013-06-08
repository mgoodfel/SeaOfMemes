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
#ifndef VERTEXCUBEINT_H
#define VERTEXCUBEINT_H

const int VERTEX_CUBEINT_PT_FRACT = 3;  // 3 bits of resolution
const int VERTEX_CUBEINT_SIZE_FRACT = 4;  // 4 bits of resolution
const int VERTEX_CUBEINT_TUV_FRACT = 4;  // 4 bits of resolution

const int VERTEX_CUBEINT_UNIT = 1 << VERTEX_CUBEINT_PT_FRACT;

// m_pt encoding
const int VERTEX_CUBEINT_PX_SHIFT = 0;
const int VERTEX_CUBEINT_PY_SHIFT = 9;
const int VERTEX_CUBEINT_PZ_SHIFT = 18;
const int VERTEX_CUBEINT_PT_MASK = 0x7FFFFFF;

const int VERTEX_CUBEINT_LAMP_FLAG = 1 << 27;
// 4 bits left

// m_code encoding
const int VERTEX_CUBEINT_TZ_SHIFT = 0;
const int VERTEX_CUBEINT_TZ_MASK = 0x3FF << VERTEX_CUBEINT_TZ_SHIFT;

const int VERTEX_CUBEINT_TU_SHIFT = 10;
const int VERTEX_CUBEINT_TV_SHIFT = 15;
const int VERTEX_CUBEINT_TUV_MASK = 0x3FF << VERTEX_CUBEINT_TU_SHIFT;

const int VERTEX_CUBEINT_AMBIENT_SHIFT = 20;
const int VERTEX_CUBEINT_AMBIENT_MASK = 0xF << VERTEX_CUBEINT_AMBIENT_SHIFT;
const int VERTEX_CUBEINT_SKY_SHIFT = 24;
const int VERTEX_CUBEINT_SKY_MASK = 0xF << VERTEX_CUBEINT_SKY_SHIFT;
const int VERTEX_CUBEINT_BLOCK_SHIFT = 28;
const int VERTEX_CUBEINT_BLOCK_MASK = 0xF << VERTEX_CUBEINT_BLOCK_SHIFT;

const int VERTEX_CUBEINT_LIGHT_MASK = VERTEX_CUBEINT_AMBIENT_MASK |
                                      VERTEX_CUBEINT_SKY_MASK |
                                      VERTEX_CUBEINT_BLOCK_MASK;

const float VERTEX_CUBEINT_INTEN_BASE = log(0.8f);

// 0 bits left

class VertexCubeInt
{
public:
  unsigned int m_pt;
  unsigned int m_code;

  VertexCubeInt()
  {
    m_pt = 0;
    m_code = 0;
  }

  // set lamp intensity
  void setLamp(
    float blockLight)
  {
    // invert block intensity, takes one subtract from shader
    int blockInten = (int) floor(0.5+ log(blockLight)/VERTEX_CUBEINT_INTEN_BASE);

    m_pt |= VERTEX_CUBEINT_LAMP_FLAG;
    m_code = (m_code & ~VERTEX_CUBEINT_BLOCK_MASK) |
             blockInten << VERTEX_CUBEINT_BLOCK_SHIFT;
  }

  // set light intensity
  void setLight(
    float ambientLight,
    float skyLight,
    float blockLight)
  {
    int ambientInten = (int) floor(0.5+ 15*ambientLight);

    int skyInten = (int) floor(0.5+ log(skyLight)/VERTEX_CUBEINT_INTEN_BASE);
    int blockInten = (int) floor(0.5+ log(blockLight)/VERTEX_CUBEINT_INTEN_BASE);

    m_pt &= ~VERTEX_CUBEINT_LAMP_FLAG;
    m_code = (m_code & ~VERTEX_CUBEINT_LIGHT_MASK) | 
           (ambientInten << VERTEX_CUBEINT_AMBIENT_SHIFT) |
           (skyInten << VERTEX_CUBEINT_SKY_SHIFT) |
           (blockInten << VERTEX_CUBEINT_BLOCK_SHIFT);
  }

  // set the point
  void setPoint(
    int x, 
    int xFract,
    int y, 
    int yFract,
    int z,
    int zFract)
  {
    x = (x << VERTEX_CUBEINT_PT_FRACT) | xFract;
    y = (y << VERTEX_CUBEINT_PT_FRACT) | yFract;
    z = (z << VERTEX_CUBEINT_PT_FRACT) | zFract;

    m_pt = (m_pt & ~VERTEX_CUBEINT_PT_MASK) |
           (x << VERTEX_CUBEINT_PX_SHIFT) | 
           (y << VERTEX_CUBEINT_PY_SHIFT) | 
           (z << VERTEX_CUBEINT_PZ_SHIFT);
  }

  // set the point
  void setPoint(
    const mgMatrix4* dir,
    int x, 
    int y, 
    int z,
    double cx,
    double cy,
    double cz)
  {
    // transform the cube vertex point
    double px, py, pz;
    dir->mapPt(cx, cy, cz, px, py, pz);
    px += x;
    py += y;
    pz += z;

    // convert to integer/fraction
    x = (int) floor(0.5+px* VERTEX_CUBEINT_UNIT);
    y = (int) floor(0.5+py* VERTEX_CUBEINT_UNIT);
    z = (int) floor(0.5+pz* VERTEX_CUBEINT_UNIT);

    // pack into vertex code
    m_pt = (m_pt & ~VERTEX_CUBEINT_PT_MASK) |
           (x << VERTEX_CUBEINT_PX_SHIFT) | 
           (y << VERTEX_CUBEINT_PY_SHIFT) | 
           (z << VERTEX_CUBEINT_PZ_SHIFT);
  }

  // set the normal
  void setNormal(
    int nx,
    int ny, 
    int nz)
  {
/*
    int normal = ((nx+1) << VERTEX_CUBEINT_NX_SHIFT) |
                 ((ny+1) << VERTEX_CUBEINT_NY_SHIFT) |
                 ((nz+1) << VERTEX_CUBEINT_NZ_SHIFT);

    m_code = (m_code & ~VERTEX_CUBEINT_NORMAL_MASK) | normal;
*/
  }

  // set the normal
  void setNormal(
    const mgMatrix4* dir,
    double nx,
    double ny, 
    double nz)
  {
  }

  // set the texture uv 
  void setTextureUV(
    int tu,
    int tuFract,
    int tv,
    int tvFract)
  {
    tu = (tu << VERTEX_CUBEINT_TUV_FRACT) | tuFract;
    tv = (tv << VERTEX_CUBEINT_TUV_FRACT) | tvFract;

    int tuv = (tu << VERTEX_CUBEINT_TU_SHIFT) |
              (tv << VERTEX_CUBEINT_TV_SHIFT);

    m_code = (m_code & ~VERTEX_CUBEINT_TUV_MASK) | tuv;
  }

  // set the texture to use
  void setTextureIndex(
    int index)
  {
    int tz = index << VERTEX_CUBEINT_TZ_SHIFT;
    m_code = (m_code & ~VERTEX_CUBEINT_TZ_MASK) | tz;
  }

  // add this vertex to a buffer
  void addTo(
    mgVertexBuffer* buffer)
  {
    buffer->addVertex(this);
  }

  // load a shader using this vertex type
  static mgShader* loadShader(
    const char* shaderName);

  // create a vertex buffer of this type
  static mgVertexBuffer* newBuffer(
    int size);                        // number of vertexes
};

#endif
