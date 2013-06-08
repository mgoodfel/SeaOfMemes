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
#ifdef __unix__

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgLinuxLock.h"

mgLinuxLock g_globalLock;

//--------------------------------------------------------------
// constructor
mgLinuxLock::mgLinuxLock()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  
  pthread_mutex_init(&m_mutex, &attr);
  
  pthread_mutexattr_destroy(&attr);
}

//--------------------------------------------------------------
// destructor
mgLinuxLock::~mgLinuxLock()
{
  pthread_mutex_destroy(&m_mutex);
}

//--------------------------------------------------------------
// lock it
void mgLinuxLock::lock()
{
  pthread_mutex_lock(&m_mutex);
}

//--------------------------------------------------------------
// unlock it
void mgLinuxLock::unlock()
{
  pthread_mutex_unlock(&m_mutex);
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

#endif
