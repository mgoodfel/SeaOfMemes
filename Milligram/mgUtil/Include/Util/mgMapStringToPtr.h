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
#ifndef MAPSTRINGTOPTR_H
#define MAPSTRINGTOPTR_H

#include "mgString.h"

/*
  Hashes string keys to pointers.  The key string is copied into the table.  The caller
  owns the value pointer.
*/
class mgMapStringToPtr
{
public:
  // constructor
  mgMapStringToPtr();

  // destructor
  virtual ~mgMapStringToPtr();
  
  // return count of entries
  virtual int length() const
  {
    return m_tableCount;
  }

  // find a key
  virtual BOOL lookup(
    const char* key, 
    const void*& value) const;
  
  // set a key
  virtual void setAt(
    const char* key, 
    const void* value);

  // remove a key
  virtual void removeKey(
    const char* key);
    
  // start iteration
  virtual int getStartPosition() const;
  
  // get next key in iteration
  virtual void getNextAssoc(
    int& posn, 
    mgString& key, 
    const void*& value) const;

  // remove all entries
  virtual void removeAll(); 

protected:
  int m_tableSize;
  int m_tableCount;
  struct mgMapStringToPtrEntry* m_entries;
  
  // hash the key 
  int hash(
    const char* key) const;
  
  // grow the table
  void grow();
};

#endif
