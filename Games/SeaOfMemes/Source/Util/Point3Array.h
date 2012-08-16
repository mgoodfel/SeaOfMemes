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
#ifndef POINT3ARRAY_H
#define POINT3ARRAY_H

class Point3Array
{
public:
  // constructor
  Point3Array();

  // destructor
  virtual ~Point3Array();
  
  // reset to empty and free storage
  virtual void reset();

  // return length of array
  int length() const
  {
    return m_elementCount;
  }
  
  // find index of element
  int find(
    const mgPoint3& elm) const;

  // add an element to end
  void add(
    const mgPoint3& elm);
  
  // add a list to end
  void addAll(
    const Point3Array& list);

  // insert element at index
  void insertAt(
    int index,
    const mgPoint3& elm);
  
  // set element of array
  void setAt(
    int index, 
    const mgPoint3& elm);
    
  // return element of array
  BOOL getAt(
    int index,
    mgPoint3& pt) const;
      
  // remove item
  void remove(
    const mgPoint3& elm);
    
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
    const mgPoint3& elm);
    
  // return end of array
  virtual BOOL top(
    mgPoint3& pt) const;
  
  // remove from end of array
  virtual BOOL pop(
    mgPoint3& pt);
  
protected:
  mgPoint3* m_elements;
  int m_elementCount;
  int m_elementMax;
  mgPoint3 m_shortData[50];

  // grow the array
  void grow(
    int newSize);
};

#endif
