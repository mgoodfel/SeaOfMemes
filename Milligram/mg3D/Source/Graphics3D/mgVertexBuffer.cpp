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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Graphics3D/mgVertexBuffer.h"

//--------------------------------------------------------------
// constructor
mgVertexBuffer::mgVertexBuffer(
  int vertexSize,                     // size of vertex in bytes
  const mgVertexAttrib* attribs,      // array of attributes.  end with offset=-1
  int maxVertexes,                    // max number of vertices
  BOOL dynamic) 
{
  m_vertexSize = vertexSize;
  m_attribs = attribs;
  m_size = maxVertexes;
  m_data = new BYTE[m_size * m_vertexSize];
  m_count = 0;
  m_dynamic = dynamic;
}

//--------------------------------------------------------------
// destructor
mgVertexBuffer::~mgVertexBuffer()
{
  // gcc complains about delete of void*
  delete (char*) m_data;
  m_data = NULL;
}

//--------------------------------------------------------------
// reset buffer for reuse
void mgVertexBuffer::reset()
{
  if (!m_dynamic)
    throw new mgErrorMsg("glVertexReset", "", "");

  m_count = 0;
}

