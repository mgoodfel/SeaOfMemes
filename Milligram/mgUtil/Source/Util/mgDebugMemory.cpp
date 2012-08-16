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
#include "Util/mgDebugMemory.h"

#ifdef DEBUG_MEMORY

typedef struct _MemoryEntry
{
  void* key;
  int seq;
  size_t size;
  const char* fileName;
  int lineNum;
} MemoryEntry;

MemoryEntry* mgDebugMemoryEntries = NULL;
int mgDebugMemorySeq = 0;
int mgDebugMemorySize;
int mgDebugMemoryCount;
BOOL m_inDebug = false;

CRITICAL_SECTION mgDebugMemoryLock;
size_t mgDebugMemoryUsed = 0;
size_t mgDebugMemoryMaxUsed = 0;
int mgDebugMemoryMaxCount = 0;
int mgDebugMemoryMarked = -1;

void mgDebugMemoryGrow();

const int GROW_LIMIT = 66;  // percent

//--------------------------------------------------------------------
// constructor
void mgDebugMemoryInit()
{
  mgDebugMemorySize = 4093;
  mgDebugMemoryCount = 0;
  mgDebugMemoryEntries = (MemoryEntry*) malloc(sizeof(MemoryEntry) * mgDebugMemorySize);
  
  for (int i = 0; i < mgDebugMemorySize; i++)
  {
    mgDebugMemoryEntries[i].key = NULL;
    mgDebugMemoryEntries[i].size = 0;
    mgDebugMemoryEntries[i].seq = 0;
    mgDebugMemoryEntries[i].fileName = NULL;
    mgDebugMemoryEntries[i].lineNum = 0;
  }
  InitializeCriticalSection(&mgDebugMemoryLock);
}

//--------------------------------------------------------------------
// lookup a key
BOOL mgDebugMemoryLookup(
  void* key)
{
  // hash the key for first probe into table
  int index = ((int) key) % mgDebugMemorySize;
  int startIndex = index;
  while (true)
  {
    MemoryEntry *entry = (MemoryEntry *) &mgDebugMemoryEntries[index];
    if (entry->key == NULL)
      return false;
      
    if (entry->key == key)
      return true;

    // try next entry
    index++;
    if (index >= mgDebugMemorySize)
      index = 0;
      
    // if we've wrapped
    if (index == startIndex)
      return false;  // no match on full table
  }
}

//--------------------------------------------------------------------
// set/change a key
void mgDebugMemorySet(
  void* key, 
  int seq,
  size_t size,
  const char* fileName,
  int lineNum)
{
  if (mgDebugMemoryCount * 100 > mgDebugMemorySize * GROW_LIMIT)
    mgDebugMemoryGrow();
    
  // hash the key for first probe into table
  int index = ((int) key) % mgDebugMemorySize;
  int startIndex = index;
  while (true)
  {
    MemoryEntry *entry = (MemoryEntry *) &mgDebugMemoryEntries[index];
    if (entry->key == NULL)
    {
      // set entry
      entry->key = key;
      entry->seq = seq;
      entry->size = size;
      entry->fileName = fileName;
      entry->lineNum = lineNum;
      mgDebugMemoryCount++;
      return;
    }
    
    if (entry->key == key)
    {
      mgDebug("%d: existing memory allocation %08x replaced in %s, line %d", seq, key, (const char*) fileName, lineNum);
      // existing key changed
      entry->seq = seq;
      entry->size = size;
      entry->fileName = fileName;
      entry->lineNum = lineNum;
      return;
    }
    
    // look in next entry
    index++;
    if (index >= mgDebugMemorySize)
      index = 0;
      
    // if we've wrapped
    if (index == startIndex)
    {
      // table is full.  shouldn't happen with grow test at top
      mgDebugMemoryGrow();
      mgDebugMemorySet(key, seq, size, fileName, lineNum);
      return;
    }
  }
}

//--------------------------------------------------------------------
// remove a key
void mgDebugMemoryRemove(
  void* key,
  size_t& size)
{
  // hash the key for first probe into table
  int index = ((int) key) % mgDebugMemorySize;
  int startIndex = index;
  while (true)
  {
    MemoryEntry *entry = (MemoryEntry *) &mgDebugMemoryEntries[index];
    if (entry->key == key)
    {
      size = mgDebugMemoryEntries[index].size;

      // remove the key
      mgDebugMemoryEntries[index].key = NULL;
      mgDebugMemoryEntries[index].seq = 0;
      mgDebugMemoryEntries[index].size = 0;
      mgDebugMemoryEntries[index].fileName = NULL;
      mgDebugMemoryEntries[index].lineNum = 0;
      mgDebugMemoryCount--;
      return;
    }
    
    // look in next entry
    index++;
    if (index >= mgDebugMemorySize)
      index = 0;
      
    // if we've wrapped, key not found
    if (index == startIndex)
    {
      mgDebug("delete of unknown pointer %08x", key);
      size = 0;
      return;
    }
  }
}
    
//--------------------------------------------------------------------
// start iteration through map
int mgDebugMemoryStart()
{
  // find first non-empty entry
  for (int index = 0; index < mgDebugMemorySize; index++)
  {
    MemoryEntry *entry = (MemoryEntry *) &mgDebugMemoryEntries[index];
    if (entry->key != NULL)
      return index;
  }
  return -1;
}

//--------------------------------------------------------------------
// get next association
void mgDebugMemoryNext(
  int& posn, 
  void*& key, 
  int& seq,
  size_t& size,
  const char*& fileName,
  int& lineNum)
{
  int index = posn;
  index = max(0, min(mgDebugMemorySize-1, index));
  MemoryEntry *entry = (MemoryEntry *) &mgDebugMemoryEntries[index];
  if (entry->key == NULL)
    return;  // bad posn
  key = entry->key;
  seq = entry->seq;
  size = entry->size;
  fileName = entry->fileName;
  lineNum = entry->lineNum;
  
  // find the next key, if any
  index++;
  while (index < mgDebugMemorySize)
  {
    MemoryEntry *entry = (MemoryEntry *) &mgDebugMemoryEntries[index];
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
// grow the table
void mgDebugMemoryGrow()
{
  int newSize = 1+2*mgDebugMemorySize;
  MemoryEntry* newEntries = (MemoryEntry*) malloc(newSize * sizeof(MemoryEntry));
  for (int i = 0; i < newSize; i++)
  {
    newEntries[i].key = NULL;
    newEntries[i].seq = 0;
    newEntries[i].size = 0;
    newEntries[i].fileName = NULL;
    newEntries[i].lineNum = 0;
  }
    
  MemoryEntry* oldEntries = mgDebugMemoryEntries;
  int oldSize = mgDebugMemorySize;
  
  mgDebugMemoryEntries = newEntries;
  mgDebugMemorySize = newSize;
  mgDebugMemoryCount = 0;
  for (int i = 0; i < oldSize; i++)
  {
    MemoryEntry *entry = &oldEntries[i];
    if (entry->key != NULL)
      mgDebugMemorySet(entry->key, entry->seq, entry->size, entry->fileName, entry->lineNum);
  }
  free(oldEntries);
}

//--------------------------------------------------------------------
// mgDebug new operator
void* operator new(
  size_t count, 
  const char* file, 
  int line)
{
  EnterCriticalSection(&mgDebugMemoryLock);
  void* result = malloc(count);
  if (!m_inDebug)
  {
    m_inDebug = true;
//    mgDebug("%08x (%d) allocate %s:%d", result, count, (const char*) file, line);
    if (result == NULL)
    {
      mgDebug("malloc(%d) returned null", count);
    }

    if (mgDebugMemorySeq == mgDebugMemoryMarked)
    {
      mgDebug("allocated tagged item"); // for breakpoints
    }

    if (mgDebugMemoryEntries == NULL)
      mgDebugMemoryInit();

    mgDebugMemorySet(result, mgDebugMemorySeq++, count, file, line);
    
    mgDebugMemoryUsed += count;
    mgDebugMemoryMaxUsed = max(mgDebugMemoryMaxUsed, mgDebugMemoryUsed);
    mgDebugMemoryMaxCount = max(mgDebugMemoryMaxCount, mgDebugMemoryCount);

    m_inDebug = false;
  }
  LeaveCriticalSection(&mgDebugMemoryLock);
  return result;
}

//--------------------------------------------------------------------
// delete with extra parameters
void operator delete(
  void* ptr, 
  const char* file, 
  int line)
{
  if (ptr == NULL)
    return;
  EnterCriticalSection(&mgDebugMemoryLock);
  if (!m_inDebug)
  {
//    mgDebug("%08x delete %s:%d", ptr, (const char*) file, line);
    m_inDebug = true;
    
    if (mgDebugMemoryEntries == NULL)
      mgDebugMemoryInit();
      
    size_t size;
    mgDebugMemoryRemove(ptr, size);
    mgDebugMemoryUsed -= size;
    
    m_inDebug = false;
  }
  
  free(ptr);
  LeaveCriticalSection(&mgDebugMemoryLock);
}

//--------------------------------------------------------------------
// delete with extra parameters
void operator delete(
  void* ptr, 
  size_t count, 
  const char* file, 
  int line)
{
  if (ptr == NULL)
    return;
  EnterCriticalSection(&mgDebugMemoryLock);
  if (!m_inDebug)
  {
    m_inDebug = true;
//    mgDebug("%08x (%d) delete %s:%d", ptr, count, (const char*) file, line);
    
    if (mgDebugMemoryEntries == NULL)
      mgDebugMemoryInit();
      
    size_t size;
    mgDebugMemoryRemove(ptr, size);
    mgDebugMemoryUsed -= size;
    
    m_inDebug = false;
  }
  
  free(ptr);
  LeaveCriticalSection(&mgDebugMemoryLock);
}

//--------------------------------------------------------------------
// normal delete operator
void operator delete(
  void* ptr)
{
  if (ptr == NULL)
    return;
  EnterCriticalSection(&mgDebugMemoryLock);
  if (!m_inDebug)
  {
    m_inDebug = true;
//    mgDebug("%08x delete", ptr);
    
    if (mgDebugMemoryEntries == NULL)
      mgDebugMemoryInit();
      
    size_t size;
    mgDebugMemoryRemove(ptr, size);
    mgDebugMemoryUsed -= size;
    
    m_inDebug = false;
  }
  
  free(ptr);
  LeaveCriticalSection(&mgDebugMemoryLock);
}
#endif

//--------------------------------------------------------------------
// report remaining allocations
void mgDebugMemory()
{
#ifdef DEBUG_MEMORY
  m_inDebug = true;
  
  mgDebug("max memory used = %d K", mgDebugMemoryMaxUsed/1024);
  mgDebug("max allocations = %d", mgDebugMemoryMaxCount);
  mgDebug("ending memory use = %d", mgDebugMemoryUsed);

  // =-= sort in sequence order?
  
  mgDebug("------ memory leaks");
  EnterCriticalSection(&mgDebugMemoryLock);

  int posn = mgDebugMemoryStart();
  while (posn != -1)
  {
    void* key;
    int seq;
    size_t size;
    const char* fileName;
    int lineNum;
    mgDebugMemoryNext(posn, key, seq, size, fileName, lineNum);
    // =-= on 64-bit, should display 8-byte size
    mgDebug("%d: allocated %08x (%d) in %s, line %d", seq, key, (int) size, (const char*) fileName, lineNum);
  }

  LeaveCriticalSection(&mgDebugMemoryLock);
  mgDebug("------ end of memory leaks");
  
  m_inDebug = false;
#endif  
}
