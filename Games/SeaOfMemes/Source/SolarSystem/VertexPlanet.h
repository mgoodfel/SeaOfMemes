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
#ifndef VERTEXPLANET_H
#define VERTEXPLANET_H

// vertex 
class VertexPlanet
{
public:
  // world coordinates of vertex
  float m_px;
  float m_py;
  float m_pz;

  // model coordinates of vertex
  float m_mx;
  float m_my;
  float m_mz;

  // set the world coordinates
  void setPoint(
    double x,
    double y,
    double z)
  {
    m_px = (float) x;
    m_py = (float) y;
    m_pz = (float) z;
  }

  // set the model coordinates
  void setModelPoint(
    double x,
    double y,
    double z)
  {
    m_mx = (float) x;
    m_my = (float) y;
    m_mz = (float) z;
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
    int size,                         // number of vertexes
    BOOL dynamic = false);            // dynamic buffer
};

#endif
