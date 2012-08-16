/*
  Copyright (C) 1995-2011 by Michael J. Goodfellow

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

#include "Point3Array.h"

const int INIT_SIZE = 20;
const int GROW_SIZE = 100;

//--------------------------------------------------------------
// constructor
Point3Array::Point3Array()
{
  m_elementMax = INIT_SIZE;
  m_elements = m_shortData;
  m_elementCount = 0;
}

//--------------------------------------------------------------
// destructor
Point3Array::~Point3Array()
{
  reset();
}
 
//--------------------------------------------------------------
// reset to empty and free storage
void Point3Array::reset()
{
  if (m_elements != m_shortData)
    delete m_elements;
  m_elementMax = INIT_SIZE;
  m_elements = m_shortData;
  m_elementCount = 0;
}

//--------------------------------------------------------------
// grow the array
void Point3Array::grow(
  int newSize)
{
  if (newSize < m_elementMax)
    return;

  // reallocate the array and copy old elements
  m_elementMax = newSize + GROW_SIZE;
  
  mgPoint3* newElements = new mgPoint3[m_elementMax];
  memmove(newElements, m_elements, m_elementCount*sizeof(mgPoint3));
  
  // dispose of old array
  if (m_elements != m_shortData)
    delete m_elements;
  m_elements = newElements;
}
  
//--------------------------------------------------------------
// add an element
void Point3Array::add(
  const mgPoint3& elm)
{
  grow(m_elementCount+1);
  m_elements[m_elementCount++] = elm;
}

//--------------------------------------------------------------
// add a list to end
void Point3Array::addAll(
  const Point3Array& list)
{
  int len = list.length();
  grow(len);
  for (int i = 0; i < len; i++)
  {
    m_elements[m_elementCount++] = list.m_elements[i];
  }
}

//--------------------------------------------------------------
// find index of element
int Point3Array::find(
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
void Point3Array::insertAt(
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
void Point3Array::setAt(
  int index, 
  const mgPoint3& elm)
{
  grow(index);
    
  // set new element
  m_elements[index] = elm;
  m_elementCount = max(m_elementCount, index+1);
}
  
//--------------------------------------------------------------
// return element of array
BOOL Point3Array::getAt(
  int index,
  mgPoint3& elm) const
{
  if (index >= m_elementCount)
    return false;
  elm = m_elements[index];
  return true;
}

//--------------------------------------------------------------
// remove an element by index
void Point3Array::removeAt(
  int index)
{
  if (index >= m_elementCount)
    return;
  memmove(m_elements+index, m_elements+index+1, (m_elementCount-index)*sizeof(mgPoint3));
  m_elementCount--;
}

//--------------------------------------------------------------
// remove an element by value
void Point3Array::remove(
  const mgPoint3& elm)
{
  int index = find(elm);
  if (index < 0)
    return;  // not found
    
  memmove(m_elements+index, m_elements+index+1, (m_elementCount-index)*sizeof(mgPoint3));
  m_elementCount--;
}

//--------------------------------------------------------------
// add to end of array
int Point3Array::push(
  const mgPoint3& elm)
{
  grow(m_elementCount+1);
  m_elements[m_elementCount++] = elm;
  
  return m_elementCount;
}

//--------------------------------------------------------------
// return end of array
BOOL Point3Array::top(
  mgPoint3& elm) const
{
  if (m_elementCount == 0)
    return false;
    
  elm = m_elements[m_elementCount-1];
  return true;
}

//--------------------------------------------------------------
// remove from end of array
BOOL Point3Array::pop(
  mgPoint3& elm)
{
  if (m_elementCount == 0)
    return false;
    
  elm = m_elements[m_elementCount-1];
  m_elementCount--;
  return true;
}

