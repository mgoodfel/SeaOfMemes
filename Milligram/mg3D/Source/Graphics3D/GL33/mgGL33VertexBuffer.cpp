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
#ifdef SUPPORT_GL33

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgPlatform/Include/mgPlatformServices.h"
#include "mgGL33Types.h"
#include "mgGL33Misc.h"
#include "mgGL33Services.h"

#include "mgGL33VertexBuffer.h"

//--------------------------------------------------------------
// constructor
mgGL33VertexBuffer::mgGL33VertexBuffer(
  int vertexSize,                     // size of vertex in bytes
  const mgVertexAttrib* attribs,      // array of attributes.  end with offset=-1
  int maxVertices,                    // max number of vertices
  BOOL dynamic)
: mgVertexBuffer(vertexSize, attribs, maxVertices, dynamic)
{
  m_array = mgNullHandle;
  m_buffer = mgNullHandle;
  m_updateBuffer = true;
}

//--------------------------------------------------------------
// destructor
mgGL33VertexBuffer::~mgGL33VertexBuffer()
{
  unloadDisplay();
}

//--------------------------------------------------------------
// reset the buffer
void mgGL33VertexBuffer::reset()
{
  mgVertexBuffer::reset();
  m_updateBuffer = true;
}

//--------------------------------------------------------------
// move buffer to display.  delete memory.
void mgGL33VertexBuffer::loadDisplay()
{
  CHECK_THREAD();
  if (m_array != mgNullHandle && !m_updateBuffer)
    return;  // nothing to do

  if (m_data == NULL)
    throw new mgErrorMsg("glNullVertex", "", "");

  // create OpenGL arrays
  if (m_array == mgNullHandle)
    glGenVertexArrays(1, &m_array);
  glBindVertexArray(m_array);

  if (m_buffer == mgNullHandle)
    glGenBuffers(1, &m_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

  glBufferData(GL_ARRAY_BUFFER, m_vertexSize * m_count, m_data, 
    m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

  // bind arrays to attribute slots
  for (int i = 0; ; i++)
  {
    if (m_attribs[i].m_name == NULL)
      break;

    glEnableVertexAttribArray(i);
    switch (m_attribs[i].m_type)
    {
      case MG_VERTEX_UBYTE1:
        glVertexAttribIPointer(i, 1, GL_UNSIGNED_BYTE, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
      break;
      case MG_VERTEX_UBYTE2:
        glVertexAttribIPointer(i, 2, GL_UNSIGNED_BYTE, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
      break;
      case MG_VERTEX_UBYTE3:
        glVertexAttribIPointer(i, 3, GL_UNSIGNED_BYTE, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
      break;
      case MG_VERTEX_UBYTE4:
        glVertexAttribIPointer(i, 4, GL_UNSIGNED_BYTE, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
      break;

      case MG_VERTEX_UINT1:
        glVertexAttribIPointer(i, 1, GL_UNSIGNED_INT, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
      break;
      case MG_VERTEX_UINT2:
        glVertexAttribIPointer(i, 2, GL_UNSIGNED_INT, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
      break;
      case MG_VERTEX_UINT3:
        glVertexAttribIPointer(i, 3, GL_UNSIGNED_INT, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
      break;

      case MG_VERTEX_FLOAT1:
        glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
        break;
      case MG_VERTEX_FLOAT2:
        glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
        break;
      case MG_VERTEX_FLOAT3:
        glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
        break;
      case MG_VERTEX_FLOAT4:
        glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, m_vertexSize, (const GLvoid*) m_attribs[i].m_offset);
        break;
    }
  }

  if (!m_dynamic)
  {
    // gcc complains about delete of void*
    delete (char*) m_data;
    m_data = NULL;
    m_size = 0;
  }
  m_updateBuffer = false;

  glBindVertexArray(mgNullHandle);
  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
}

//--------------------------------------------------------------
// destructor
void mgGL33VertexBuffer::unloadDisplay()
{
  CHECK_THREAD();
  // release the vertex buffer
  if (m_array != mgNullHandle)
  {
    glDeleteVertexArrays(1, &m_array);
    m_array = mgNullHandle;
  }

  if (m_buffer != mgNullHandle)
  {
    glDeleteBuffers(1, &m_buffer);
    m_buffer = mgNullHandle;
  }
}

#endif
