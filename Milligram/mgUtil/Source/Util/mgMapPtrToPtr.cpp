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

#include "Util/mgMapPtrToPtr.h"

struct mgMapPtrToPtrEntry
{
  const void* key;
  const void* value;
};

const int GROW_LIMIT = 66;  // percent

//--------------------------------------------------------------------
// constructor
mgMapPtrToPtr::mgMapPtrToPtr()
{
  m_tableSize = 97;
  m_tableCount = 0;
  m_entries = new mgMapPtrToPtrEntry[m_tableSize];
  
  for (int i = 0; i < m_tableSize; i++)
  {
    m_entries[i].key = NULL;
    m_entries[i].value = NULL;
  }
}

//--------------------------------------------------------------------
// destructor
mgMapPtrToPtr::~mgMapPtrToPtr()
{
  removeAll();
  delete m_entries;
}

//--------------------------------------------------------------------
// lookup a key
BOOL mgMapPtrToPtr::lookup(
  const void* key, 
  const void*& value) const
{
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapPtrToPtrEntry *entry = (mgMapPtrToPtrEntry *) &m_entries[index];
    if (entry->key == NULL)
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
void mgMapPtrToPtr::setAt(
  const void* key, 
  const void* value)
{
  if (m_tableCount * 100 > m_tableSize * GROW_LIMIT)
    grow();
    
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapPtrToPtrEntry *entry = (mgMapPtrToPtrEntry *) &m_entries[index];
    if (entry->key == NULL)
    {
      // set entry
      entry->key = key;
      entry->value = value;
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
void mgMapPtrToPtr::removeKey(
  const void* key)
{
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapPtrToPtrEntry *entry = (mgMapPtrToPtrEntry *) &m_entries[index];
    if (entry->key == key)
    {
      // remove the key
      m_entries[index].key = NULL;
      m_entries[index].value = NULL;
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
int mgMapPtrToPtr::getStartPosition() const
{
  // find first non-empty entry
  for (int index = 0; index < m_tableSize; index++)
  {
    mgMapPtrToPtrEntry *entry = (mgMapPtrToPtrEntry *) &m_entries[index];
    if (entry->key != NULL)
      return index;
  }
  return -1;
}

//--------------------------------------------------------------------
// get next association
void mgMapPtrToPtr::getNextAssoc(
  int& posn, 
  const void*& key, 
  const void*& value) const
{
  int index = posn;
  index = max(0, min(m_tableSize-1, index));
  mgMapPtrToPtrEntry *entry = (mgMapPtrToPtrEntry *) &m_entries[index];
  if (entry->key == NULL)
    return;  // bad posn
  key = entry->key;
  value = entry->value;
  
  // find the next key, if any
  index++;
  while (index < m_tableSize)
  {
    mgMapPtrToPtrEntry *entry = (mgMapPtrToPtrEntry *) &m_entries[index];
    if (entry->key != NULL)
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
void mgMapPtrToPtr::removeAll() 
{
  // delete all the entries
  for (int i = 0; i < m_tableSize; i++)
  {
    mgMapPtrToPtrEntry *entry = (mgMapPtrToPtrEntry *) &m_entries[i];
    if (entry->key != NULL)
    {
      entry->key = NULL;
      entry->value = NULL;
      m_tableCount--;
    }
  }
}

//--------------------------------------------------------------------
// hash a key
int mgMapPtrToPtr::hash(
  const void* key) const
{
  // just use the pointer as the hash
  return abs((int) (intptr_t) key);
}

//--------------------------------------------------------------------
// grow the table
void mgMapPtrToPtr::grow()
{
  int newSize = 1+2*m_tableSize;
  mgMapPtrToPtrEntry* newEntries = new mgMapPtrToPtrEntry[newSize];
  for (int i = 0; i < newSize; i++)
  {
    newEntries[i].key = NULL;
    newEntries[i].value = NULL;
  }
    
  mgMapPtrToPtrEntry* oldEntries = m_entries;
  int oldSize = m_tableSize;
  
  m_entries = newEntries;
  m_tableSize = newSize;
  for (int i = 0; i < oldSize; i++)
  {
    mgMapPtrToPtrEntry *entry = &oldEntries[i];
    if (entry->key != NULL)
      setAt(entry->key, entry->value);
  }
  // done with old table
  delete oldEntries;
}
