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
#ifdef __APPLE__

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgOSXThread.h"

class MacThreadArgs
{
public:
  mgOSThread* m_thread;
  mgOSThreadCallback m_callback;
  void* m_threadArg1;
  void* m_threadArg2;
};

// call the thread function
void* MacThreadProc(
  void *argPtr);

//--------------------------------------------------------------
// call the thread function
void* MacThreadProc(
  void *argPtr)
{
  MacThreadArgs* args = (MacThreadArgs*) argPtr;
  (*args->m_callback) (args->m_thread, args->m_threadArg1, args->m_threadArg2);

  delete args;
  return NULL;
}

//--------------------------------------------------------------
// constructor
mgOSXThread::mgOSXThread(
  int threadCount,
  mgOSThreadCallback callback,
  int priority,
  void* threadArg1,
  void** threadArgs)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

// =-= set priority

  m_threadCount = threadCount;
  m_threads = new pthread_t[threadCount];  

  for (int i = 0; i < m_threadCount; i++)
  {
    MacThreadArgs* args = new MacThreadArgs;
    args->m_thread = this;
    args->m_callback = callback;
    args->m_threadArg1 = threadArg1;
    if (threadArgs != NULL)
      args->m_threadArg2 = threadArgs[i];
    else args->m_threadArg2 = NULL;

    pthread_create(&m_threads[i], &attr, MacThreadProc, args);
  }
  pthread_attr_destroy(&attr);
}

//--------------------------------------------------------------
// destructor
mgOSXThread::~mgOSXThread()
{
  delete m_threads;
  m_threads = NULL;
}

//--------------------------------------------------------------
// set priority
void mgOSXThread::setPriority(
  int priority)
{
#ifdef WORKED
  int winPriority = THREAD_PRIORITY_NORMAL;
  switch (priority)
  {
    case PRIORITY_LOW: winPriority = THREAD_PRIORITY_LOWEST;
      break;
    case PRIORITY_NORMAL: winPriority = THREAD_PRIORITY_NORMAL;
      break;
    case PRIORITY_HIGH: winPriority = THREAD_PRIORITY_HIGHEST;
      break;
  }

  for (int i = 0; i < m_threadCount; i++)
  {
    SetThreadPriority(m_handles[i], winPriority);
  }
#endif
}

//--------------------------------------------------------------
// wait for thread to end
void mgOSXThread::waitForEnd(
  double ms)
{
  for (int i = 0; i < m_threadCount; i++)
  {
    void* status;
    pthread_join(m_threads[i], &status);
  }
}

#endif
