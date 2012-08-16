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
#ifndef MAPPTRTOPTR_H
#define MAPPTRTOPTR_H

class mgMapPtrToPtr 
{
public:
  // constructor
  mgMapPtrToPtr();
  
  // destructor
  virtual ~mgMapPtrToPtr();

  // return number of entries
  virtual int length() const
  {
    return m_tableCount;
  }
    
  // lookup value of key
  virtual BOOL lookup(
    const void* key, 
    const void*& value) const;
  
  // set new value of key
  virtual void setAt(
    const void* key, 
    const void* value);

  // remove a key
  virtual void removeKey(
    const void* key);
    
  // get position of first association
  virtual int getStartPosition() const;
  
  // get next association in table
  virtual void getNextAssoc(
    int &posn, 
    const void*& key, 
    const void*& value) const;

  // remove all entries
  virtual void removeAll(); 

protected:
  int m_tableSize;
  int m_tableCount;
  struct mgMapPtrToPtrEntry* m_entries;
  
  // hash the key 
  int hash(
    const void* key) const;
  
  // grow the table
  void grow();
};

#endif
