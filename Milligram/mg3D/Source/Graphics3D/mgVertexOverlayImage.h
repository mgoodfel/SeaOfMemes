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
#ifndef MGVERTEXOVERLAYIMAGE_H
#define MGVERTEXOVERLAYIMAGE_H

// vertex for rgba overlay graphics
class mgVertexOverlayImage
{
public:
  float m_px;
  float m_py;
  float m_tx;
  float m_ty;

  // add this vertex to a buffer
  void addTo(
    mgVertexBuffer* buffer)
  {
    buffer->addVertex(this);
  }
};

const mgVertexAttrib MGVERTEX_OVERLAYIMAGE_ATTRIBS[] = {
  "vertPoint", MG_VERTEX_FLOAT2, (int) offsetof(mgVertexOverlayImage, m_px), 
  "vertTexCoord0", MG_VERTEX_FLOAT2, (int) offsetof(mgVertexOverlayImage, m_tx),
  NULL, 0, -1  
};

#endif
