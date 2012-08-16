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

#include "Util/mgPtrArray.h"

const int GROW_SIZE = 100;

//--------------------------------------------------------------
// constructor
mgPtrArray::mgPtrArray()
{
  m_elementMax = sizeof(m_shortData)/sizeof(void*);
  m_elements = m_shortData;
  m_elementCount = 0;
}

//--------------------------------------------------------------
// destructor
mgPtrArray::~mgPtrArray()
{
  reset();
}
 
//--------------------------------------------------------------
// reset to empty and free storage
void mgPtrArray::reset()
{
  if (m_elements != m_shortData)
    delete m_elements;
  m_elementMax = sizeof(m_shortData)/sizeof(void*);
  m_elements = m_shortData;
  m_elementCount = 0;
}

//--------------------------------------------------------------
// grow the array to new size
void mgPtrArray::grow(
  int newSize)
{
  if (newSize <= m_elementMax)
    return;

  // reallocate the array and copy old elements
  m_elementMax = newSize + GROW_SIZE;
  
  const void** newElements = new const void*[m_elementMax];
  memmove(newElements, m_elements, m_elementCount*sizeof(void*));
  
  // dispose of old array
  if (m_elements != m_shortData)
    delete m_elements;
  m_elements = newElements;
}
  
//--------------------------------------------------------------
// add an element
void mgPtrArray::add(
  const void* elm)
{
  grow(m_elementCount+1);
  m_elements[m_elementCount++] = elm;
}

//--------------------------------------------------------------
// add a list to end
void mgPtrArray::addAll(
  const mgPtrArray& list)
{
  int len = list.length();
  grow(m_elementCount + len);
  for (int i = 0; i < len; i++)
  {
    m_elements[m_elementCount++] = list[i];
  }
}

//--------------------------------------------------------------
// find index of element
int mgPtrArray::find(
  const void* elm) const
{
  for (int i = 0; i < m_elementCount; i++)
  {
    if (m_elements[i] == elm) 
      return i;
  }
  return -1;
}

//--------------------------------------------------------------
// insert element at index
void mgPtrArray::insertAt(
  int index,
  const void* elm)
{
  grow(m_elementCount+1);
  memmove(m_elements+index+1, m_elements+index, (m_elementCount-index)*sizeof(void*));
  m_elements[index] = elm;
  m_elementCount++;
}

//--------------------------------------------------------------
// set element of array
void mgPtrArray::setAt(
  int index, 
  const void* elm)
{
  grow(index+1);
  // pad to position of new element
  for (int i = m_elementCount; i < index; i++)
    m_elements[i] = NULL;
    
  // set new element
  m_elements[index] = elm;
  m_elementCount = max(m_elementCount, index+1);
}
  
//--------------------------------------------------------------
// return element of array
const void* mgPtrArray::getAt(
  int index) const
{
  if (index >= m_elementCount)
    return NULL;
  return m_elements[index];
}

//--------------------------------------------------------------
// remove an element by index
void mgPtrArray::removeAt(
  int index)
{
  if (index >= m_elementCount)
    return;
  memmove(m_elements+index, m_elements+index+1, (m_elementCount-index)*sizeof(void*));
  m_elementCount--;
}

//--------------------------------------------------------------
// remove an element by value
void mgPtrArray::remove(
  const void* elm)
{
  int index = find(elm);
  if (index < 0)
    return;  // not found
    
  memmove(m_elements+index, m_elements+index+1, (m_elementCount-index)*sizeof(void*));
  m_elementCount--;
}

//--------------------------------------------------------------
// add to end of array
int mgPtrArray::push(
  const void* elm)
{
  grow(m_elementCount+1);
  m_elements[m_elementCount++] = elm;
  
  return m_elementCount;
}

//--------------------------------------------------------------
// return end of array
const void* mgPtrArray::top() const
{
  if (m_elementCount == 0)
    return NULL;
    
  return m_elements[m_elementCount-1];
}

//--------------------------------------------------------------
// remove from end of array
const void* mgPtrArray::pop()
{
  if (m_elementCount == 0)
    return NULL;
    
  const void* result = m_elements[m_elementCount-1];
  m_elementCount--;
  return result;
}

//--------------------------------------------------------------
// sort the array
void mgPtrArray::sort(
  int (*compareFn)(const void*, const void*))
{
  qsort(m_elements, m_elementCount, sizeof(void*), compareFn);
}
