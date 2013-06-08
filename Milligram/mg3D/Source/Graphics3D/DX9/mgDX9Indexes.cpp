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

#ifdef SUPPORT_DX9
#include "mgDX9State.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgDX9Display.h"
#include "mgDX9Indexes.h"

//--------------------------------------------------------------
// constructor
mgDX9Indexes::mgDX9Indexes(
  int maxIndexes,              // max number of indexes
  BOOL dynamic)                
: mgIndexBuffer(maxIndexes, dynamic)
{
  m_buffer = NULL;
  m_updateBuffer = true;
}

//--------------------------------------------------------------
// destructor
mgDX9Indexes::~mgDX9Indexes()
{
  unloadDisplay();
}

//--------------------------------------------------------------
// reset buffer for reuse
void mgDX9Indexes::reset()
{
  mgIndexBuffer::reset();
  m_updateBuffer = true;
}

//--------------------------------------------------------------
// move buffer to display.  memory deleted.
void mgDX9Indexes::loadDisplay()
{
  if (m_buffer != NULL && !m_updateBuffer)
    return;

  if (m_data == NULL)
    throw new mgException("loadDisplay with NULL memory in mgDX9Indexes");

  if (m_size == 0)
    return;  // nothing to do

  HRESULT hr;
  int size = m_size*sizeof(int);
  if (m_buffer == NULL)
  {
    hr = mg_d3dDevice->CreateIndexBuffer(size, m_dynamic ? D3DUSAGE_DYNAMIC : 0 /* Usage */, 
           D3DFMT_INDEX32, m_dynamic ? D3DPOOL_SYSTEMMEM : D3DPOOL_MANAGED, &m_buffer, NULL );
    if (FAILED(hr))
      throw new mgException("CreateIndexBuffer failed in mgDX9Indexes");
  }

  void* data;
  hr = m_buffer->Lock(0, size, &data, 0);
  if (FAILED(hr))
    throw new mgException("buffer Lock failed in mgDX9Indexes");
  memcpy(data, m_data, size);
  m_buffer->Unlock();

  if (!m_dynamic)
  {
    delete m_data;
    m_data = NULL;
    m_size = 0;
    // leave count since callers may use it
  }
  m_updateBuffer = false;
}

//--------------------------------------------------------------
// remove buffer from display
void mgDX9Indexes::unloadDisplay()
{
  // release the vertex buffer
  if (m_buffer != NULL)
  {
    ULONG refCount = m_buffer->Release();
    m_buffer = NULL;
  }
}

#endif
