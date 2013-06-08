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
#ifndef VERTEXTERRAIN_H
#define VERTEXTERRAIN_H

// vertex 
class VertexTerrain
{
public:
  float m_px;
  float m_py;
  float m_pz;
  float m_nx;
  float m_ny;
  float m_nz;
  float m_tx;
  float m_ty;
  float m_red;
  float m_green;
  float m_blue;

  // set the point
  void setPoint(
    double x, 
    double y, 
    double z)
  {
    m_px = (float) x;
    m_py = (float) y;
    m_pz = (float) z;
  }

  // set the normal
  void setNormal(
    double x, 
    double y, 
    double z)
  {
    m_nx = (float) x;
    m_ny = (float) y;
    m_nz = (float) z;
  }

  // set the texture coordinates
  void setTexture(
    double x, 
    double y)
  {
    m_tx = (float) x;
    m_ty = (float) y;
  }

  // set the material color
  void setColor(
    double red,
    double green,
    double blue)
  {
    m_red = (float) red;
    m_green = (float) green;
    m_blue = (float) blue;
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
