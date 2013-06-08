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
#ifndef VERTEXSHAPEINT_H
#define VERTEXSHAPEINT_H

/*
     inten 3*4 =      12 bits
     lamp flag         1 bits
     tz               10 bits
     tx, ty, 16*128 = 22 bits
     x, y, z 64*512 = 45 bits
                      90 bits
*/

// word1 fields
const int VERTEX_SHAPEINT_PX_SHIFT = 0;
const int VERTEX_SHAPEINT_PX_MASK = 0x7FFF << VERTEX_SHAPEINT_PX_SHIFT;
const int VERTEX_SHAPEINT_PY_SHIFT = 15;
const int VERTEX_SHAPEINT_PY_MASK = 0x7FFF << VERTEX_SHAPEINT_PY_SHIFT;
// 2 bits left

// word2 fields
const int VERTEX_SHAPEINT_PZ_SHIFT = 0;
const int VERTEX_SHAPEINT_PZ_MASK = 0x7FFF << VERTEX_SHAPEINT_PZ_SHIFT;

const int VERTEX_SHAPEINT_AMBIENT_SHIFT = 15;
const int VERTEX_SHAPEINT_AMBIENT_MASK = 0xF << VERTEX_SHAPEINT_AMBIENT_SHIFT;
const int VERTEX_SHAPEINT_SKY_SHIFT = 19;
const int VERTEX_SHAPEINT_SKY_MASK = 0xF << VERTEX_SHAPEINT_SKY_SHIFT;
const int VERTEX_SHAPEINT_BLOCK_SHIFT = 23;
const int VERTEX_SHAPEINT_BLOCK_MASK = 0xF << VERTEX_SHAPEINT_BLOCK_SHIFT;

const int VERTEX_SHAPEINT_LIGHT_MASK = VERTEX_SHAPEINT_AMBIENT_MASK |
                                       VERTEX_SHAPEINT_SKY_MASK |
                                       VERTEX_SHAPEINT_BLOCK_MASK;

const int VERTEX_SHAPEINT_LAMP_FLAG = 1 << 27;
// 4 bits left

// word3 fields
const int VERTEX_SHAPEINT_TU_SHIFT = 0;
const int VERTEX_SHAPEINT_TU_MASK = 0x7FF << VERTEX_SHAPEINT_TU_SHIFT;

const int VERTEX_SHAPEINT_TV_SHIFT = 11;
const int VERTEX_SHAPEINT_TV_MASK = 0x7FF << VERTEX_SHAPEINT_TV_SHIFT;

const int VERTEX_SHAPEINT_TZ_SHIFT = 22;
const int VERTEX_SHAPEINT_TZ_MASK = 0x3FF << VERTEX_SHAPEINT_TZ_SHIFT;
// 0 bits left

const float VERTEX_SHAPEINT_INTEN_BASE = log(0.8f);
const int VERTEX_SHAPEINT_TUV_FRACT = 7;
const int VERTEX_SHAPEINT_TUV_ZERO = 8;
const int VERTEX_SHAPEINT_TUV_MAX = 1 << 11;
const int VERTEX_SHAPEINT_PT_FRACT = 9;

class VertexShapeInt
{
public:
  DWORD m_word1;
  DWORD m_word2;
  DWORD m_word3;

  VertexShapeInt()
  {
    m_word1 = m_word2 = m_word3 = 0;
 }

  // set lamp intensity
  void setLamp(
    float blockLight)
  {
    int blockInten = (int) floor(0.5+ log(blockLight)/VERTEX_SHAPEINT_INTEN_BASE);

    m_word2 |= VERTEX_SHAPEINT_LAMP_FLAG;
    m_word2 &= ~VERTEX_SHAPEINT_BLOCK_MASK;
    m_word2 |= blockInten << VERTEX_SHAPEINT_BLOCK_SHIFT;
  }

  // set light intensity
  void setLight(
    float ambientLight,
    float skyLight,
    float blockLight)
  {
    int ambientInten = (int) floor(0.5+ 15*ambientLight);

    int skyInten = (int) floor(0.5+ log(skyLight)/VERTEX_SHAPEINT_INTEN_BASE);
    int blockInten = (int) floor(0.5+ log(blockLight)/VERTEX_SHAPEINT_INTEN_BASE);

    m_word2 &= ~VERTEX_SHAPEINT_LAMP_FLAG;
    m_word2 &= ~VERTEX_SHAPEINT_LIGHT_MASK; 
    m_word2 |= (ambientInten << VERTEX_SHAPEINT_AMBIENT_SHIFT) |
              (skyInten << VERTEX_SHAPEINT_SKY_SHIFT) |
              (blockInten << VERTEX_SHAPEINT_BLOCK_SHIFT);
  }

  // set the point
  void setPoint(
    float x, 
    float y, 
    float z)
  {
    int px = (int) floor(max(0.0f, x)* (1 << VERTEX_SHAPEINT_PT_FRACT));
    int py = (int) floor(max(0.0f, y)* (1 << VERTEX_SHAPEINT_PT_FRACT));
    int pz = (int) floor(max(0.0f, z)* (1 << VERTEX_SHAPEINT_PT_FRACT));

    m_word1 &= ~(VERTEX_SHAPEINT_PX_MASK | VERTEX_SHAPEINT_PY_MASK);
    m_word1 |= (px << VERTEX_SHAPEINT_PX_SHIFT) |
               (py << VERTEX_SHAPEINT_PY_SHIFT);

    m_word2 &= ~VERTEX_SHAPEINT_PZ_MASK;
    m_word2 |= pz << VERTEX_SHAPEINT_PZ_SHIFT;
  }

  // set the texture uv 
  void setTextureUV(
    float tu,
    float tv)
  {
    tu = (tu+VERTEX_SHAPEINT_TUV_ZERO) * (1 << VERTEX_SHAPEINT_TUV_FRACT);
    tv = (tv+VERTEX_SHAPEINT_TUV_ZERO) * (1 << VERTEX_SHAPEINT_TUV_FRACT);
    int itu = (int) floor(tu);
    itu = max(0, min(VERTEX_SHAPEINT_TUV_MAX, itu));
    int itv = (int) floor(tv);
    itv = max(0, min(VERTEX_SHAPEINT_TUV_MAX, itv));

    m_word3 &= ~(VERTEX_SHAPEINT_TU_MASK | VERTEX_SHAPEINT_TV_MASK);
    m_word3 |= (itu << VERTEX_SHAPEINT_TU_SHIFT) |
               (itv << VERTEX_SHAPEINT_TV_SHIFT);
  }

  // set the texture to use
  void setTextureIndex(
    int tz)
  {
    m_word3 &= ~VERTEX_SHAPEINT_TZ_MASK;
    m_word3 |= tz << VERTEX_SHAPEINT_TZ_SHIFT;
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
