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

#include "Util/mgStringArray.h"

const int INIT_SIZE = 50;
const int GROW_SIZE = 50;

//--------------------------------------------------------------
// constructor and destructor
mgStringArray::mgStringArray()
{
  m_elementMax = 0;
  m_elements = NULL;
  m_elementCount = 0;
}

//--------------------------------------------------------------
mgStringArray::~mgStringArray()
{
  removeAll();

  delete m_elements;
  m_elements = NULL;
}
 
//--------------------------------------------------------------
// grow the array
void mgStringArray::grow(
  int newSize)
{
  if (newSize < m_elementMax)
    return;

  // reallocate the array and copy old elements
  m_elementMax = newSize + GROW_SIZE;
  const char** newElements = new const char*[m_elementMax];
  memset(newElements, 0, m_elementMax*sizeof(const char*));
  memcpy(newElements, m_elements, m_elementCount*sizeof(const char*));
  
  // dispose of old array
  delete m_elements;
  m_elements = newElements;
}
  
//--------------------------------------------------------------
// add an element
void mgStringArray::add(
  const char* string)
{
  grow(m_elementCount+1);

  int len = (int) strlen(string);
  char* elm = new char[len+1];
  strcpy(elm, string);

  m_elements[m_elementCount++] = elm;
}

//--------------------------------------------------------------
// find index of element
int mgStringArray::find(
  const char* string) const
{
  for (int i = 0; i < m_elementCount; i++)
  {
    if (strcmp(m_elements[i], string) == 0) 
      return i;
  }
  return -1;
}

//--------------------------------------------------------------
// insert element at index
void mgStringArray::insertAt(
  int index,
  const char* string)
{
  int len = (int) strlen(string);
  char* elm = new char[len+1];
  strcpy(elm, string);

  grow(m_elementCount+1);
  memmove(m_elements+index+1, m_elements+index, (m_elementCount-index)*sizeof(const char*));
  m_elements[index] = elm;
  m_elementCount++;
}

//--------------------------------------------------------------
// set element of array
void mgStringArray::setAt(
  int index, 
  const char* string)
{
  int len = (int) strlen(string);
  char* elm = new char[len+1];
  strcpy(elm, string);

  grow(index);
  // pad to position of new element
  for (int i = m_elementCount; i < index; i++)
    m_elements[i] = NULL;

  if (m_elements[index] != NULL)
  {
    delete m_elements[index];
    m_elements[index] = NULL;
  }
        
  // set new element
  m_elements[index] = elm;
  m_elementCount = max(m_elementCount, index+1);
}
  
//--------------------------------------------------------------
// return element of array
const char* mgStringArray::getAt(
  int index) const
{
  if (index >= m_elementCount)
    return 0;
  return m_elements[index];
}

//--------------------------------------------------------------
// remove an element by index
void mgStringArray::removeAt(
  int index)
{
  if (index < 0 || index >= m_elementCount)
    return;

  if (m_elements[index] != NULL)
  {
    delete m_elements[index];
    m_elements[index] = NULL;
  }

  memmove(m_elements+index, m_elements+index+1, (m_elementCount-index)*sizeof(const char*));
  m_elementCount--;
}

//--------------------------------------------------------------
// remove an element by value
void mgStringArray::remove(
  const char* string)
{
  int index = find(string);
  if (index == -1)
    return;  // not found
    
  memmove(m_elements+index, m_elements+index+1, (m_elementCount-index)*sizeof(const char*));
  m_elementCount--;
}

//--------------------------------------------------------------
// remove all elements
void mgStringArray::removeAll()
{
  // free all the strings
  for (int index = 0; index < m_elementCount; index++)
  {
    if (m_elements[index] != NULL)
    {
      delete m_elements[index];
      m_elements[index] = NULL;
    }
  }
  m_elementCount = 0;
}

//--------------------------------------------------------------
// add to end of array
int mgStringArray::push(
  const char* string)
{
  int len = (int) strlen(string);
  char* elm = new char[len+1];
  strcpy(elm, string);

  grow(m_elementCount+1);
  m_elements[m_elementCount++] = elm;
  
  return m_elementCount;
}

//--------------------------------------------------------------
// return end of array
const char* mgStringArray::top() const
{
  if (m_elementCount == 0)
    return 0;
    
  return m_elements[m_elementCount-1];
}

//--------------------------------------------------------------
// pop last entry off array
void mgStringArray::pop(
  mgString& elm)
{
  if (m_elementCount == 0)
    elm.empty();
    
  elm = m_elements[m_elementCount-1];
  removeAt(m_elementCount-1);
}
  
//--------------------------------------------------------------
// sort the array
void mgStringArray::sort(
  int (*compareFn)(const void*, const void*))
{
  qsort(m_elements, m_elementCount, sizeof(void*), compareFn);
}
