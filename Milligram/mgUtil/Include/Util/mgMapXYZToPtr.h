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
#ifndef MGMAPXYZTOPTR_H
#define MGMAPXYZTOPTR_H

class mgMapXYZToPtr
{
public:
  // constructor
  mgMapXYZToPtr();

  // destructor
  virtual ~mgMapXYZToPtr();
  
  // return count of entries
  virtual int length() const
  {
    return m_tableCount;
  }

  // find chunk at coordinates
  virtual BOOL lookup(
    int x,
    int y,
    int z,
    const void*& value) const;
  
  // set new value of key
  virtual void setAt(
    int x,
    int y,
    int z,
    const void* value);

  // remove a key
  virtual void removeKey(
    int x, 
    int y, 
    int z);
    
  // get position of first entry
  virtual int getStartPosition() const;
  
  // get next association in table
  virtual void getNextAssoc(
    int &posn, 
    const void*& value) const;

  // remove all entries
  virtual void removeAll(); 

protected:
  int m_tableSize;
  int m_tableCount;
  struct mgMapXYZToPtrEntry* m_entries;
  
  // hash the key 
  int hash(
    int x,
    int y,
    int z) const;
  
  // grow the table
  void grow();
};

#endif