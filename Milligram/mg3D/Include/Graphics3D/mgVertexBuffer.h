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
#ifndef MGVERTEXBUFFER_H
#define MGVERTEXBUFFER_H

#include "mgVertexAttrib.h"

class mgVertexBuffer
{
public:
  // destructor
  virtual ~mgVertexBuffer();

  // add a vertex to buffer
  void addVertex(
    const void* v)
  {
    // if no room in buffer, throw exception
    if (!canAdd(1))
      throw new mgErrorMsg("mgVertexFull", "", "");

    memcpy(m_data+m_vertexSize*m_count, v, m_vertexSize);
    m_count++;
  }

  // return current vertex count
  int getLength() const
  {
    return m_count;
  }

  // return true if room for triangles
  BOOL canAdd(
    int count) const
  {
    return (m_count + count) <= m_size;
  }

  // reset the buffer
  virtual void reset();

  // copy buffer to display
  virtual void loadDisplay() = 0;

  // delete buffer from display
  virtual void unloadDisplay() = 0;

protected:
  int m_vertexSize;
  const mgVertexAttrib* m_attribs;

  BYTE* m_data;
  int m_size;
  int m_count;
  BOOL m_dynamic;

  // constructor
  mgVertexBuffer(
    int vertexSize,                   // size of vertex in bytes
    const mgVertexAttrib* attribs,    // array of attributes.  end with offset=-1
    int maxVertexes,                  // max number of vertices
    BOOL dynamic = false);                  
};

#endif
