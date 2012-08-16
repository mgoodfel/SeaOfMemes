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

#ifdef WIN32
#include "Windows/mgWinEvent.h"
#include "Windows/mgWinLock.h"
#include "Windows/mgWinThread.h"

//--------------------------------------------------------------
// create an event
mgOSEvent* mgOSEvent::create()
{
  return new mgWinEvent();
}

//--------------------------------------------------------------
// create a lock object
mgOSLock* mgOSLock::create()
{
  return new mgWinLock();
}

//--------------------------------------------------------------
// create a thread group
mgOSThread* mgOSThread::create(
  int threadCount,
  mgOSThreadCallback callback,
  int priority,
  void* threadArg1,
  void** threadArgs)
{
  return new mgWinThread(threadCount, callback, priority, threadArg1, threadArgs);
}

#endif

#ifdef __unix__
#include "Linux/mgLinuxEvent.h"
#include "Linux/mgLinuxLock.h"
#include "Linux/mgLinuxThread.h"

//--------------------------------------------------------------
// create an event
mgOSEvent* mgOSEvent::create()
{
  return new mgLinuxEvent();
}

//--------------------------------------------------------------
// create a lock object
mgOSLock* mgOSLock::create()
{
  return new mgLinuxLock();
}

//--------------------------------------------------------------
// create a thread group
mgOSThread* mgOSThread::create(
  int threadCount,
  mgOSThreadCallback callback,
  int priority,
  void* threadArg1,
  void** threadArgs)
{
  return new mgLinuxThread(threadCount, callback, priority, threadArg1, threadArgs);
}

#endif

#ifdef __APPLE__

#include "OSX/mgOSXEvent.h"
#include "OSX/mgOSXLock.h"
#include "OSX/mgOSXThread.h"

//--------------------------------------------------------------
// create an event
mgOSEvent* mgOSEvent::create()
{
  return new mgOSXEvent();
}

//--------------------------------------------------------------
// create a lock object
mgOSLock* mgOSLock::create()
{
  return new mgOSXLock();
}

//--------------------------------------------------------------
// create a thread group
mgOSThread* mgOSThread::create(
  int threadCount,
  mgOSThreadCallback callback,
  int priority,
  void* threadArg1,
  void** threadArgs)
{
  return new mgOSXThread(threadCount, callback, priority, threadArg1, threadArgs);
}
#endif
