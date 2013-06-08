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

#include "Util/mgPoint3Array.h"

const int INIT_SIZE = 50;
const int GROW_SIZE = 50;

//--------------------------------------------------------------
// constructor and destructor
mgPoint3Array::mgPoint3Array()
{
  m_elementMax = 0;
  m_elements = NULL;
  m_elementCount = 0;
}

//--------------------------------------------------------------
mgPoint3Array::~mgPoint3Array()
{
  delete m_elements;
  m_elements = NULL;
}
 
//--------------------------------------------------------------
// grow the array
void mgPoint3Array::grow(
  int newSize)
{
  if (newSize < m_elementMax)
    return;

  // reallocate the array and copy old elements
  m_elementMax = newSize + GROW_SIZE;
  mgPoint3* newElements = new mgPoint3[m_elementMax];
  memcpy(newElements, m_elements, m_elementCount*sizeof(mgPoint3));
  
  // dispose of old array
  delete m_elements;
  m_elements = newElements;
}
  
//--------------------------------------------------------------
// add an element
void mgPoint3Array::add(
  const mgPoint3& elm)
{
  grow(m_elementCount+1);
  m_elements[m_elementCount++] = elm;
}

//--------------------------------------------------------------
// add all elements of array
void mgPoint3Array::addAll(
  const mgPoint3Array& other)
{
  grow(m_elementCount + other.m_elementCount);
  memcpy(m_elements+m_elementCount, other.m_elements, sizeof(mgPoint3)*other.m_elementCount);
  m_elementCount += other.m_elementCount;
}
      
//--------------------------------------------------------------
// find index of element
int mgPoint3Array::find(
  const mgPoint3& elm) const
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
void mgPoint3Array::insertAt(
  int index,
  const mgPoint3& elm)
{
  grow(m_elementCount+1);
  memmove(m_elements+index+1, m_elements+index, (m_elementCount-index)*sizeof(mgPoint3));
  m_elements[index] = elm;
  m_elementCount++;
}

//--------------------------------------------------------------
// set element of array
void mgPoint3Array::setAt(
  int index, 
  const mgPoint3& elm)
{
  grow(index);
  // pad to position of new element
  for (int i = m_elementCount; i < index; i++)
    m_elements[i] = mgPoint3(0, 0, 0);
    
  // set new element
  m_elements[index] = elm;
  m_elementCount = max(m_elementCount, index+1);
}
  
//--------------------------------------------------------------
// return element of array
void mgPoint3Array::getAt(
  int index,
  mgPoint3& elm) const
{
  if (index < 0 || index >= m_elementCount)
  {
    elm = mgPoint3(0, 0, 0);
    return;
  }
  elm = m_elements[index];
}

//--------------------------------------------------------------
// remove an element by index
void mgPoint3Array::removeAt(
  int index)
{
  if (index < 0 || index >= m_elementCount)
    return;
  memmove(m_elements+index, m_elements+index+1, (m_elementCount-index)*sizeof(mgPoint3));
  m_elementCount--;
}

//--------------------------------------------------------------
// remove an element by value
void mgPoint3Array::remove(
  mgPoint3& elm)
{
  int index = find(elm);
  if (index == -1)
    return;  // not found
    
  memmove(m_elements+index, m_elements+index+1, (m_elementCount-index)*sizeof(mgPoint3));
  m_elementCount--;
}

//--------------------------------------------------------------
// add to end of array
int mgPoint3Array::push(
  mgPoint3& elm)
{
  grow(m_elementCount+1);
  m_elements[m_elementCount++] = elm;
  
  return m_elementCount;
}

//--------------------------------------------------------------
// return end of array
void mgPoint3Array::top(
  mgPoint3& elm) const
{
  if (m_elementCount == 0)
  {
    elm = mgPoint3(0, 0, 0);
    return;
  }
    
  elm = m_elements[m_elementCount-1];
}

//--------------------------------------------------------------
// remove from end of array
void mgPoint3Array::pop(
  mgPoint3& elm)
{
  if (m_elementCount == 0)
  {
    elm = mgPoint3(0, 0, 0);
    return;
  }
    
  elm = m_elements[m_elementCount-1];
  m_elementCount--;
}

//--------------------------------------------------------------
// sort the array
void mgPoint3Array::sort(
  mgCompareFn compare)
{
  qsort(m_elements, m_elementCount, sizeof(mgPoint3), compare);
}
