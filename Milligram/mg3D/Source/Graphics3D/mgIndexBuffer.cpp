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

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Graphics3D/mgIndexBuffer.h"

//--------------------------------------------------------------
// constructor
mgIndexBuffer::mgIndexBuffer(
  int maxIndexes,               // max number of indexes
  BOOL dynamic,                 // support reset and reuse
  BOOL longIndexes)             // use 32-bit indexes
{
  m_size = maxIndexes;
  m_dynamic = dynamic;
  m_longIndexes = longIndexes;

  if (m_longIndexes)
  {
    m_data = m_longData = new UINT32[m_size];
    m_shortData = NULL;
  }
  else
  {
    m_data = m_shortData = new UINT16[m_size];
    m_longData = NULL;
  }

  m_count = 0;
}

//--------------------------------------------------------------
// destructor
mgIndexBuffer::~mgIndexBuffer()
{
  // gcc complains about delete of void*
  delete (char*) m_data;
  m_data = NULL;
}

//--------------------------------------------------------------
// reset buffer for reuse
void mgIndexBuffer::reset()
{
  if (!m_dynamic)
    throw new mgErrorMsg("glIndexReset", "", "");

  m_count = 0;
}

//--------------------------------------------------------------
// add indexes for a grid of points
void mgIndexBuffer::addGrid(
  int vertexBase,         // starting index
  int rowSize,            // length of row
  int rows,               // total row count
  int cols,               // total col count
  BOOL outward)           // true to face outward
{
  // if no room in buffer, throw exception
  if (!canAdd(rows*cols*6))
    throw new mgErrorMsg("glIndexFull", "", ""); 

  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      int index = vertexBase + i*rowSize+j;

      addIndex(index);  // tl
      if (outward)
      {
        addIndex(index+1);  // tr
        addIndex(index+rowSize);  // bl
      }
      else
      {
        addIndex(index+rowSize);  // bl
        addIndex(index+1);  // tr
      }

      addIndex(index+rowSize);  // bl
      if (outward)
      {
        addIndex(index+1);  // tr
        addIndex(index+rowSize+1);  // br
      }
      else
      {
        addIndex(index+rowSize+1);  // br
        addIndex(index+1);  // tr
      }
    }
  }
}
