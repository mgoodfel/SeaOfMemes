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
#ifdef SUPPORT_GL21

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgPlatform/Include/mgPlatformServices.h"
#include "mgGL21Types.h"

#include "mgGL21Display.h"
#include "mgGL21IndexBuffer.h"

//--------------------------------------------------------------
// constructor
mgGL21IndexBuffer::mgGL21IndexBuffer(
  int maxIndexes,              // max number of indexes
  BOOL dynamic,
  BOOL longIndexes)
: mgIndexBuffer(maxIndexes, dynamic, longIndexes)
{
  m_buffer = mgNullHandle;
  m_updateBuffer = true;
}

//--------------------------------------------------------------
// destructor
mgGL21IndexBuffer::~mgGL21IndexBuffer()
{
  unloadDisplay();
}

//--------------------------------------------------------------
// reset buffer for reuse
void mgGL21IndexBuffer::reset()
{
  mgIndexBuffer::reset();
  m_updateBuffer = true;
}

//--------------------------------------------------------------
// copy buffer to display
void mgGL21IndexBuffer::loadDisplay()
{
  if (m_buffer != mgNullHandle && !m_updateBuffer)
    return;  // nothing to do

  if (m_data == NULL)
    throw new mgErrorMsg("glNullIndex", "", "");

  if (m_buffer == mgNullHandle)
    glGenBuffers(1, &m_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);

  int indexSize = m_longIndexes ? sizeof(UINT32) : sizeof(UINT16);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * m_count, m_data, 
    m_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

  if (!m_dynamic)
  {
    // gcc complains about delete of void*
    delete (char*) m_data;
    m_data = NULL;
    m_size = 0;
    // leave count, since caller may use it
  }
  m_updateBuffer = false;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mgNullHandle);
}

//--------------------------------------------------------------
// delete buffer from display
void mgGL21IndexBuffer::unloadDisplay()
{
  if (m_buffer != mgNullHandle)
  {
    glDeleteBuffers(1, &m_buffer);
    m_buffer = mgNullHandle;
  }
}

#endif
