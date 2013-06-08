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
#ifdef SUPPORT_WEBGL

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgPlatform/Include/mgPlatformServices.h"
#include "mgWebGLTypes.h"
#include "mgWebGLMisc.h"
#include "mgWebGLDisplay.h"

#include "mgWebGLVertexBuffer.h"

//--------------------------------------------------------------
// constructor
mgWebGLVertexBuffer::mgWebGLVertexBuffer(
  int vertexSize,                     // size of vertex in bytes
  const mgVertexAttrib* attribs,      // array of attributes.  end with offset=-1
  int maxVertices,                    // max number of vertices
  BOOL dynamic)                       // support reset and reuse
: mgVertexBuffer(vertexSize, attribs, maxVertices, dynamic)
{
  m_buffer = mgNullHandle;
  m_updateBuffer = true;
}

//--------------------------------------------------------------
// destructor
mgWebGLVertexBuffer::~mgWebGLVertexBuffer()
{
  unloadDisplay();
}

//--------------------------------------------------------------
// reset the buffer
void mgWebGLVertexBuffer::reset()
{
  mgVertexBuffer::reset();
  m_updateBuffer = true;
}

//--------------------------------------------------------------
// move buffer to display.  delete memory.
void mgWebGLVertexBuffer::loadDisplay()
{
  if (m_buffer != mgNullHandle && !m_updateBuffer)
    return;  // nothing to do

  if (m_data == NULL)
    throw new mgErrorMsg("glNullVertex", "", "");

  // create OpenGL arrays
  if (m_buffer == mgNullHandle)
    glGenBuffers(1, &m_buffer);

  glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
  glBufferData(GL_ARRAY_BUFFER, m_vertexSize * m_count, m_data, 
    m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

  if (!m_dynamic)
  {
    // gcc complains about delete of void*
    delete (char*) m_data;
    m_data = NULL;
    m_size = 0;
  }
  m_updateBuffer = false;

  glBindBuffer(GL_ARRAY_BUFFER, mgNullHandle);
}

//--------------------------------------------------------------
// destructor
void mgWebGLVertexBuffer::unloadDisplay()
{
  if (m_buffer != mgNullHandle)
  {
    glDeleteBuffers(1, &m_buffer);
    m_buffer = mgNullHandle;
  }
}

#endif
