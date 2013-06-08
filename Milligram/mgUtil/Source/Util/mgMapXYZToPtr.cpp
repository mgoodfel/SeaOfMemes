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

#include "Util/mgMapXYZToPtr.h"

const int GROW_LIMIT = 66;  // percent
const int INIT_TABLE_SIZE = 6037;

struct mgMapXYZToPtrEntry
{
  int x;
  int y;
  int z;
  const void* value;
};

//--------------------------------------------------------------
// constructor
mgMapXYZToPtr::mgMapXYZToPtr()
{
  m_tableSize = INIT_TABLE_SIZE;
  m_tableCount = 0;
  m_entries = new mgMapXYZToPtrEntry[m_tableSize];
  
  for (int i = 0; i < m_tableSize; i++)
  {
    m_entries[i].x = 0;
    m_entries[i].y = 0;
    m_entries[i].z = 0;
    m_entries[i].value = NULL;
  }
}

//--------------------------------------------------------------
// destructor
mgMapXYZToPtr::~mgMapXYZToPtr()
{
  removeAll();
  delete m_entries;
}

//--------------------------------------------------------------------
// lookup a key
BOOL mgMapXYZToPtr::lookup(
  int x, 
  int y, 
  int z,
  const void* &value) const
{
  // hash the key for first probe into table
  int index = hash(x, y, z) % m_tableSize;
  int startIndex = index;

  while (true)
  {
    mgMapXYZToPtrEntry *entry = (mgMapXYZToPtrEntry *) &m_entries[index];
    if (entry->value == NULL)
      return false;
      
    if (entry->x == x && entry->y == y && entry->z == z)
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
void mgMapXYZToPtr::setAt(
  int x,
  int y,
  int z,
  const void* value)
{
  if (m_tableCount * 100 > m_tableSize * GROW_LIMIT)
    grow();
    
  // hash the key for first probe into table
  int index = hash(x, y, z) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapXYZToPtrEntry *entry = (mgMapXYZToPtrEntry *) &m_entries[index];
    if (entry->value == NULL)
    {
      // set entry
      entry->x = x;
      entry->y = y;
      entry->z = z;
      entry->value = value;
      m_tableCount++;
      return;
    }
    
    if (entry->x == x && entry->y == y && entry->z == z)
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
      setAt(x, y, z, value);
      return;
    }
  }
}

//--------------------------------------------------------------------
// remove a chunk
void mgMapXYZToPtr::removeKey(
  int x,
  int y,
  int z)
{
  // hash the key for first probe into table
  int index = hash(x, y, z) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgMapXYZToPtrEntry *entry = (mgMapXYZToPtrEntry *) &m_entries[index];
    if (entry->x == x && entry->y == y && entry->z == z)
    {
      // remove the key
      m_entries[index].x = 0;
      m_entries[index].y = 0;
      m_entries[index].z = 0;
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
int mgMapXYZToPtr::getStartPosition() const
{
  // find first non-empty entry
  for (int index = 0; index < m_tableSize; index++)
  {
    mgMapXYZToPtrEntry *entry = (mgMapXYZToPtrEntry *) &m_entries[index];
    if (entry->value != NULL)
      return index;
  }
  return -1;
}

//--------------------------------------------------------------------
// get next association
void mgMapXYZToPtr::getNextAssoc(
  int& index, 
  int& x,
  int& y,
  int& z,
  const void*& value) const
{
  index = max(0, min(m_tableSize-1, index));
  mgMapXYZToPtrEntry *entry = (mgMapXYZToPtrEntry *) &m_entries[index];
  if (entry->value == NULL)
    return;  // bad posn
  x = entry->x;
  y = entry->y;
  z = entry->z;
  value = entry->value;
  
  // find the next key, if any
  index++;
  while (index < m_tableSize)
  {
    mgMapXYZToPtrEntry *entry = (mgMapXYZToPtrEntry *) &m_entries[index];
    if (entry->value != NULL)
      return;
    else index++;
  }
  // no next key found
  index = -1;
}

//--------------------------------------------------------------------
// remove all keys
void mgMapXYZToPtr::removeAll() 
{
  // delete all the entries
  for (int i = 0; i < m_tableSize; i++)
  {
    mgMapXYZToPtrEntry *entry = (mgMapXYZToPtrEntry *) &m_entries[i];
    if (entry->value != NULL)
    {
      entry->value = NULL;
      m_tableCount--;
    }
  }
}

//--------------------------------------------------------------------
// hash a key
int mgMapXYZToPtr::hash(
  int x,
  int y, 
  int z) const
{
  // hash of coordinates
  return abs(x ^ (y << 10) ^ (z << 20));
}

//--------------------------------------------------------------------
// grow the table
void mgMapXYZToPtr::grow()
{
  int newSize = 1+2*m_tableSize;
  mgMapXYZToPtrEntry* newEntries = new mgMapXYZToPtrEntry[newSize];
  for (int i = 0; i < newSize; i++)
  {
    newEntries[i].x = 0;
    newEntries[i].y = 0;
    newEntries[i].z = 0;
    newEntries[i].value = NULL;
  }
    
  mgMapXYZToPtrEntry* oldEntries = m_entries;
  int oldSize = m_tableSize;
  
  m_entries = newEntries;
  m_tableSize = newSize;
  m_tableCount = 0;
  for (int i = 0; i < oldSize; i++)
  {
    mgMapXYZToPtrEntry *entry = &oldEntries[i];
    if (entry->value != NULL)
      setAt(entry->x, entry->y, entry->z, entry->value);
  }
  // done with old table
  delete oldEntries;
}
