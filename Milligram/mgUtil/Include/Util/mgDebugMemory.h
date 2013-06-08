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
#ifndef MGDEBUGMEMORY_H
#define MGDEBUGMEMORY_H

// override new and delete operators so we can track un-freed storage at
// program end.  'new' must be redefined in each module to get the line
// number macro from the compiler.

#ifdef DEBUG_MEMORY

extern int m_memorySeq;

void* operator new(size_t count, const char* file, int line);

void operator delete(void* ptr, const char* file, int line);

void operator delete(void* ptr, size_t count, const char* file, int line);

void operator delete(void* ptr);
#endif

// initialize
void mgDebugMemoryInit();

// write all unfreed memory
void mgDebugMemory();

#endif
