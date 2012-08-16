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

#include "mgWinLock.h"

mgWinLock g_globalLock;

//--------------------------------------------------------------
// constructor
mgWinLock::mgWinLock()
{
  InitializeCriticalSection(&m_section);
}

//--------------------------------------------------------------
// destructor
mgWinLock::~mgWinLock()
{
  DeleteCriticalSection(&m_section);
}

//--------------------------------------------------------------
// lock it
void mgWinLock::lock()
{
  EnterCriticalSection(&m_section);
}

//--------------------------------------------------------------
// unlock it
void mgWinLock::unlock()
{
  LeaveCriticalSection(&m_section);
}

//--------------------------------------------------------------
// lock the global lock
void mgOSGlobalLock()
{
  g_globalLock.lock();
}

//--------------------------------------------------------------
// unlock the global lock
void mgOSGlobalUnlock()
{
  g_globalLock.unlock();
}
