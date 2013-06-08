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
#ifndef MGPOINT3ARRAY_H
#define MGPOINT3ARRAY_H

#include "mgMatrix.h"

typedef int (*mgCompareFn)(const void*, const void*);

/*
  A variable-length array of mgPoint3 values.
*/

class mgPoint3Array
{
public:
  // constructor and destructor
  mgPoint3Array();
  virtual ~mgPoint3Array();
  
  // return length of array
  int length() const
  {
    return m_elementCount;
  }
  
  // find index of element
  int find(
    const mgPoint3& elm) const;

  // add an element to end
  virtual void add(
    const mgPoint3& elm);
  
  // add all elements of array
  virtual void addAll(
    const mgPoint3Array& other);

  // insert element at index
  virtual void insertAt(
    int index,
    const mgPoint3& elm);
  
  // set element of array
  virtual void setAt(
    int index, 
    const mgPoint3& elm);
    
  // return element of array
  virtual void getAt(
    int index,
    mgPoint3& elm) const;
      
  virtual void first(
    mgPoint3& elm) const
  {
    getAt(0, elm);
  }

  virtual void last(
    mgPoint3& elm) const
  {
    return getAt(m_elementCount-1, elm);
  }

  // remove item
  virtual void remove(
    mgPoint3& elm);
    
  // remove an element by index
  virtual void removeAt(
    int index);
    
  // remove all elements
  virtual void removeAll()
  {
    m_elementCount = 0;
  }
  
  // add to end of array, return count
  virtual int push(
    mgPoint3& elm);
    
  // return end of array
  virtual void top(
     mgPoint3& elm) const;
  
  // remove from end of array
  virtual void pop(
    mgPoint3& elm);
  
  // sort the array
  void sort(
    mgCompareFn compare);

protected:
  mgPoint3* m_elements;
  int m_elementCount;
  int m_elementMax;
  
  // grow the array
  void grow(
    int newSize);
};

#endif
