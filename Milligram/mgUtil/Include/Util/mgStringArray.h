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
#ifndef MGSTRINGARRAY_H
#define MGSTRINGARRAY_H

class mgStringArray
{
public:
  // constructor and destructor
  mgStringArray();
  virtual ~mgStringArray();
  
  // return length of array
  int length() const
  {
    return m_elementCount;
  }
  
  // find index of element
  int find(
    const char* string) const;

  // add an element to end
  virtual void add(
    const char* string);
  
  // insert element at index
  virtual void insertAt(
    int index,
    const char* string);
  
  // set element of array
  virtual void setAt(
    int index, 
    const char* string);
    
  // return element of array
  virtual const char* getAt(
    int index) const;
      
  virtual const char* operator[](
    int index) const
  {
    return getAt(index);
  }

  // find and remove item
  virtual void remove(
    const char* string);
    
  // remove an element by index
  virtual void removeAt(
    int index);
    
  // remove all elements
  virtual void removeAll();
  
  // add to end of array, return count
  virtual int push(
    const char* elm);
    
  // return end of array
  virtual const char* top() const;
  
  // pop last entry off array
  virtual void pop(
    mgString& elm);
  
  // sort the array
  void sort(
    int (*compareFn)(const void*, const void*));

protected:
  const char** m_elements;
  int m_elementCount;
  int m_elementMax;
  
  // grow the array
  void grow(
    int newSize);
};

#endif
