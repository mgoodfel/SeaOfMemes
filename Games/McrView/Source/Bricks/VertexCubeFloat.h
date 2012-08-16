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
#ifndef VERTEXCUBEFLOAT_H
#define VERTEXCUBEFLOAT_H

class VertexCubeFloat
{
public:
  float m_px;
  float m_py;
  float m_pz;
  float m_tx;
  float m_ty;
  float m_tz;
  float m_ambientLight;
  float m_skyLight;
  float m_blockLight;

  VertexCubeFloat()
  {
    m_px = m_py = m_pz = 0.0f;
    m_tx = m_ty = m_tz = 0.0f;
    m_ambientLight = m_skyLight = m_blockLight = 0.0f;
 }

  // set lamp intensity
  void setLamp(
    float blockLight)
  {
    // signal this is a lamp
    m_blockLight = -blockLight;
    m_ambientLight = 1.0f;
    m_skyLight = 1.0f;
  }

  // set light intensity
  void setLight(
    float ambientLight,
    float skyLight,
    float blockLight)
  {
    m_ambientLight = ambientLight;
    m_skyLight = skyLight;
    m_blockLight = blockLight;
  }

  // set the point
  void setPoint(
    float x, 
    float y, 
    float z)
  {
    m_px = x;
    m_py = y;
    m_pz = z;
  }

  // set the point
  void setPoint(
    int x, 
    int y, 
    int z)
  {
    m_px = (float) x;
    m_py = (float) y;
    m_pz = (float) z;
  }

  // transform and set the point
  void setPoint(
    const mgMatrix4* dir,
    int x, 
    int y, 
    int z,
    float cx, 
    float cy, 
    float cz)
  {
    double px, py, pz;
    dir->mapPt(cx, cy, cz, px, py, pz);
    m_px = (float) (px + x);
    m_py = (float) (py + y);
    m_pz = (float) (pz + z);
  }

  // set the normal
  void setNormal(
    float nx,
    float ny, 
    float nz)
  {
  }

  // set the normal
  void setNormal(
    const mgMatrix4* dir,
    float nx,
    float ny, 
    float nz)
  {
  }

  // set the texture uv 
  void setTextureUV(
    float tu,
    float tv)
  {
    m_tx = tu;
    m_ty = tv;
  }

  // set the texture to use
  void setTextureIndex(
    int tz)
  {
    m_tz = (float) tz;
  }

  // add this vertex to a buffer
  void addTo(
    mgVertexBuffer* buffer)
  {
    buffer->addVertex(this);
  }

  // load a shader using this vertex type
  static void loadShader(
    const char* shaderName);

  // create a vertex buffer of this type
  static mgVertexBuffer* newBuffer(
    int size);                        // number of vertexes
};

#endif
