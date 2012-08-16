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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Util/mgMapStringToDouble.h"

struct mgMapStringToDoubleEntry
{
  char* key;
  double value;
};

const int GROW_LIMIT = 66;  // percent

//--------------------------------------------------------------------
// constructor
mgMapStringToDouble::mgMapStringToDouble()
{
  m_tableSize = 97;
  m_tableCount = 0;
  m_entries = new mgMapStringToDoubleEntry[m_tableSize];
  
  for (int i = 0; i < m_tableSize; i++)
  {
    m_entries[i].key = NULL;
    m_entries[i].value = NULL;
  }
}

//--------------------------------------------------------------------
// destructor
mgMapStringToDouble::~mgMapStringToDouble()
{
  removeAll();
  delete m_entries;
}

//--------------------------------------------------------------------
// lookup a key
BOOL mgMapStringToDouble::lookup(
  const char* key, 
  double& value) const
{
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapStringToDoubleEntry *entry = (mgMapStringToDoubleEntry *) &m_entries[index];
    if (entry->key == NULL)
      return false;
      
    if (strcmp(entry->key, key) == 0)
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
void mgMapStringToDouble::setAt(
  const char* key, 
  double value)
{
  if (m_tableCount * 100 > m_tableSize * GROW_LIMIT)
    grow();
    
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapStringToDoubleEntry *entry = (mgMapStringToDoubleEntry *) &m_entries[index];
    if (entry->key == NULL)
    {
      // set entry
      int len = (int) strlen(key);
      entry->key = new char[len+1];
      strcpy(entry->key, key);
      entry->value = value;
      m_tableCount++;
      return;
    }
    
    if (strcmp(entry->key, key) == 0)
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
      // table is full, so grow and try again
      grow();
      setAt(key, value);
      return;
    }
  }
}

//--------------------------------------------------------------------
// remove a key
void mgMapStringToDouble::removeKey(
  const char* key)
{
  // hash the key for first probe into table
  int index = hash(key) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapStringToDoubleEntry *entry = (mgMapStringToDoubleEntry *) &m_entries[index];
    if (strcmp(entry->key, key) == 0)
    {
      // remove the key
      delete m_entries[index].key;
      m_entries[index].key = NULL;
      m_entries[index].value = 0;
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
int mgMapStringToDouble::getStartPosition() const
{
  // find first non-empty entry
  for (int index = 0; index < m_tableSize; index++)
  {
    mgMapStringToDoubleEntry *entry = (mgMapStringToDoubleEntry *) &m_entries[index];
    if (entry->key != NULL)
      return index;
  }
  return -1;
}

//--------------------------------------------------------------------
// get next association
void mgMapStringToDouble::getNextAssoc(
  int& posn, 
  mgString& key, 
  double& value) const
{
  int index = posn;
  index = max(0, min(m_tableSize-1, index));
  mgMapStringToDoubleEntry *entry = (mgMapStringToDoubleEntry *) &m_entries[index];
  if (entry->key == NULL)
    return;  // bad posn
  key = entry->key;
  value = entry->value;
  
  // find the next key, if any
  index++;
  while (index < m_tableSize)
  {
    mgMapStringToDoubleEntry *entry = (mgMapStringToDoubleEntry *) &m_entries[index];
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
void mgMapStringToDouble::removeAll() 
{
  // delete all the entries
  for (int i = 0; i < m_tableSize; i++)
  {
    mgMapStringToDoubleEntry *entry = (mgMapStringToDoubleEntry *) &m_entries[i];
    if (entry->key != NULL)
    {
      delete entry->key;
      entry->key = NULL;
      entry->value = 0;
      m_tableCount--;
    }
  }
}

//--------------------------------------------------------------------
// compute hash of a key
int mgMapStringToDouble::hash(
  const char* key) const
{
  int hashValue = 0;
  for (int i = 0; ; i++)
  {
    char c = key[i];
    if (c == L'\0')
      break;
    hashValue ^= c;
    hashValue = (hashValue << 8) | (hashValue >> 24);
  }
  return abs(hashValue);
}

//--------------------------------------------------------------------
// grow the table
void mgMapStringToDouble::grow()
{
  int newSize = 1+2*m_tableSize;
  mgMapStringToDoubleEntry* newEntries = new mgMapStringToDoubleEntry[newSize];
  for (int i = 0; i < newSize; i++)
  {
    newEntries[i].key = NULL;
    newEntries[i].value = 0;
  }
    
  mgMapStringToDoubleEntry* oldEntries = m_entries;
  int oldSize = m_tableSize;
  
  m_entries = newEntries;
  m_tableSize = newSize;
  m_tableCount = 0;
  for (int i = 0; i < oldSize; i++)
  {
    mgMapStringToDoubleEntry *entry = &oldEntries[i];
    if (entry->key != NULL)
    {
      setAt(entry->key, entry->value);
      // done with old key
      delete entry->key;
    }
  }
  // done with old table
  delete oldEntries;
}
