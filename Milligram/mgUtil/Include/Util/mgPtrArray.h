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
#ifndef MGPTRARRAY_H
#define MGPTRARRAY_H

class mgPtrArray
{
public:
  // constructor
  mgPtrArray();

  // destructor
  virtual ~mgPtrArray();
  
  // reset to empty and free storage
  virtual void reset();

  // return length of array
  int length() const
  {
    return m_elementCount;
  }
  
  // find index of element
  int find(
    const void* elm) const;

  // add an element to end
  void add(
    const void* elm);
  
  // add a list to end
  void addAll(
    const mgPtrArray& list);

  // insert element at index
  void insertAt(
    int index,
    const void* elm);
  
  // set element of array
  void setAt(
    int index, 
    const void* elm);
    
  // return element of array
  const void* getAt(
    int index) const;
      
  const void* operator[](
    int index) const
  {
    return getAt(index);
  }
  
  virtual const void* first()
  {
    return getAt(0);
  }

  virtual const void* last()
  {
    return getAt(m_elementCount-1);
  }

  // remove item
  void remove(
    const void* elm);
    
  // remove an element by index
  void removeAt(
    int index);
    
  // remove all elements
  void removeAll()
  {
    m_elementCount = 0;
  }
  
  // add to end of array, return count
  virtual int push(
    const void* elm);
    
  // return end of array
  virtual const void* top() const;
  
  // remove from end of array
  virtual const void* pop();
  
  // sort the array
  void sort(
    int (*compareFn)(const void*, const void*));

protected:
  const void** m_elements;
  int m_elementCount;
  int m_elementMax;
  const void* m_shortData[20];

  // grow the array
  void grow(
    int newSize);
};

#endif
