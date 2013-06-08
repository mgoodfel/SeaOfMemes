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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Util/mgMapDWordToDWord.h"

struct mgMapDWordToDWordEntry
{
  DWORD key;
  DWORD value;
  BOOL isNull;
};

const int GROW_LIMIT = 66;  // percent

//--------------------------------------------------------------------
// constructor
mgMapDWordToDWord::mgMapDWordToDWord()
{
  m_tableSize = 97;
  m_tableCount = 0;
  m_entries = new mgMapDWordToDWordEntry[m_tableSize];
  
  for (int i = 0; i < m_tableSize; i++)
  {
    m_entries[i].key = 0;
    m_entries[i].value = 0;
    m_entries[i].isNull = true;
  }
}

//--------------------------------------------------------------------
// destructor
mgMapDWordToDWord::~mgMapDWordToDWord()
{
  removeAll();
  delete m_entries;
}

//--------------------------------------------------------------------
// lookup a key
BOOL mgMapDWordToDWord::lookup(
  DWORD key, 
  DWORD& value) const
{
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapDWordToDWordEntry *entry = (mgMapDWordToDWordEntry *) &m_entries[index];
    if (entry->isNull)
      return false;
      
    if (entry->key == key)
    {
      value = entry->value;
      return true;
    }

    // try next entry
    index++;
    if (index >= m_tableSize)
      index = 0;
      
    // if we've wrapped
    if (index == startIndex)
      return false;  // no match on full table
  }
}

//--------------------------------------------------------------------
// set/change a key
void mgMapDWordToDWord::setAt(
  DWORD key, 
  DWORD value)
{
  if (m_tableCount * 100 > m_tableSize * GROW_LIMIT)
    grow();
    
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapDWordToDWordEntry *entry = (mgMapDWordToDWordEntry *) &m_entries[index];
    if (entry->isNull)
    {
      // set entry
      entry->key = key;
      entry->value = value;
      entry->isNull = false;
      m_tableCount++;
      return;
    }
    
    if (entry->key == key)
    {
      // existing key changed
      entry->value = value;
      return;
    }
    
    // look in next entry
    index++;
    if (index >= m_tableSize)
      index = 0;
      
    // if we've wrapped
    if (index == startIndex)
    {
      // table is full.  shouldn't happen with grow test at top
      grow();
      setAt(key, value);
      return;
    }
  }
}

//--------------------------------------------------------------------
// remove a key
void mgMapDWordToDWord::removeKey(
  DWORD key)
{
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapDWordToDWordEntry *entry = (mgMapDWordToDWordEntry *) &m_entries[index];
    if (entry->key == key)
    {
      // remove the key
      m_entries[index].key = 0;
      m_entries[index].value = 0;
      m_entries[index].isNull = true;
      m_tableCount--;
      return;
    }
    
    // look in next entry
    index++;
    if (index >= m_tableSize)
      index = 0;
      
    // if we've wrapped, key not found
    if (index == startIndex)
      return;
  }
}
    
//--------------------------------------------------------------------
// start iteration through map
int mgMapDWordToDWord::getStartPosition() const
{
  // find first non-empty entry
  for (int index = 0; index < m_tableSize; index++)
  {
    mgMapDWordToDWordEntry *entry = (mgMapDWordToDWordEntry *) &m_entries[index];
    if (!entry->isNull)
      return index;
  }
  return -1;
}

//--------------------------------------------------------------------
// get next association
void mgMapDWordToDWord::getNextAssoc(
  int& posn, 
  DWORD& key, 
  DWORD& value) const
{
  int index = posn;
  index = max(0, min(m_tableSize-1, index));
  mgMapDWordToDWordEntry *entry = (mgMapDWordToDWordEntry *) &m_entries[index];
  if (entry->isNull)
    return;  // bad posn
  key = entry->key;
  value = entry->value;
  
  // find the next key, if any
  index++;
  while (index < m_tableSize)
  {
    mgMapDWordToDWordEntry *entry = (mgMapDWordToDWordEntry *) &m_entries[index];
    if (!entry->isNull)
    {
      posn = index;
      return;
    }
    else index++;
  }
  // no next key found
  posn = -1;
}

//--------------------------------------------------------------------
// remove all keys
void mgMapDWordToDWord::removeAll() 
{
  // delete all the entries
  for (int i = 0; i < m_tableSize; i++)
  {
    mgMapDWordToDWordEntry *entry = (mgMapDWordToDWordEntry *) &m_entries[i];
    if (!entry->isNull)
    {
      entry->key = 0;
      entry->value = 0;
      entry->isNull = true;
      m_tableCount--;
    }
  }
}

//--------------------------------------------------------------------
// hash a key
int mgMapDWordToDWord::hash(
  DWORD key) const
{
  // just use the pointer as the hash
  return abs((int) key);
}

//--------------------------------------------------------------------
// grow the table
void mgMapDWordToDWord::grow()
{
  int newSize = 1+2*m_tableSize;
  mgMapDWordToDWordEntry* newEntries = new mgMapDWordToDWordEntry[newSize];
  for (int i = 0; i < newSize; i++)
  {
    newEntries[i].key = 0;
    newEntries[i].value = 0;
    newEntries[i].isNull = true;
  }
    
  mgMapDWordToDWordEntry* oldEntries = m_entries;
  int oldSize = m_tableSize;
  
  m_entries = newEntries;
  m_tableSize = newSize;
  for (int i = 0; i < oldSize; i++)
  {
    mgMapDWordToDWordEntry *entry = &oldEntries[i];
    if (!entry->isNull)
      setAt(entry->key, entry->value);
  }
  // done with old table
  delete oldEntries;
}
