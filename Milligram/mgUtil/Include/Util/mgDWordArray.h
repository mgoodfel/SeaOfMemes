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
#ifndef MGDWORDARRAY_H
#define MGDWORDARRAY_H

class mgDWordArray
{
public:
  // constructor and destructor
  mgDWordArray();
  virtual ~mgDWordArray();
  
  // return length of array
  int length() const
  {
    return m_elementCount;
  }
  
  // find index of element
  int find(
    DWORD elm) const;

  // add an element to end
  virtual void add(
    DWORD elm);
  
  // insert element at index
  virtual void insertAt(
    int index,
    DWORD elm);
  
  // set element of array
  virtual void setAt(
    int index, 
    DWORD elm);
    
  // return element of array
  virtual DWORD getAt(
    int index) const;
      
  virtual DWORD operator[](
    int index) const
  {
    return getAt(index);
  }

  virtual DWORD first()
  {
    return getAt(0);
  }

  virtual DWORD last()
  {
    return getAt(m_elementCount-1);
  }

  // remove item
  virtual void remove(
    DWORD elm);
    
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
    DWORD elm);
    
  // return end of array
  virtual DWORD top() const;
  
  // remove from end of array
  virtual DWORD pop();
  
  // sort the array
  void sort(
    int (*compareFn)(const void*, const void*));

protected:
  DWORD* m_elements;
  int m_elementCount;
  int m_elementMax;
  
  // grow the array
  void grow(
    int newSize);
};

#endif
