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
#include <process.h>

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgWinThread.h"

class WinThreadArgs
{
public:
  mgOSThread* m_thread;
  mgOSThreadCallback m_callback;
  void* m_threadArg1;
  void* m_threadArg2;
};

//--------------------------------------------------------------
// call the thread function
unsigned __stdcall WinThreadProc(
  void *argPtr)
{
  WinThreadArgs* args = (WinThreadArgs*) argPtr;
  (*args->m_callback) (args->m_thread, args->m_threadArg1, args->m_threadArg2);

  delete args;
  return 0;
}

//--------------------------------------------------------------
// constructor
mgWinThread::mgWinThread(
  int threadCount,
  mgOSThreadCallback callback,
  int priority,
  void* threadArg1,
  void** threadArgs)
{
  m_threadCount = threadCount;
  m_handles = new HANDLE[threadCount];  

  for (int i = 0; i < m_threadCount; i++)
  {
    WinThreadArgs* args = new WinThreadArgs;
    args->m_thread = this;
    args->m_callback = callback;
    args->m_threadArg1 = threadArg1;
    if (threadArgs != NULL)
      args->m_threadArg2 = threadArgs[i];
    else args->m_threadArg2 = NULL;

    m_handles[i] = (HANDLE) _beginthreadex(NULL, 0, WinThreadProc, args, CREATE_SUSPENDED, NULL);
//    DWORD lThreadId;
//    m_handles[i] = CreateThread(NULL, 0, WinThreadProc, args, CREATE_SUSPENDED, &lThreadId);
    setPriority(priority);

    ResumeThread(m_handles[i]);
  }
}

//--------------------------------------------------------------
// destructor
mgWinThread::~mgWinThread()
{
  for (int i = 0; i < m_threadCount; i++)
  {
    CloseHandle(m_handles[i]);
  }
  delete m_handles;
  m_handles = NULL;
}

//--------------------------------------------------------------
// set priority
void mgWinThread::setPriority(
  int priority)
{
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
}

//--------------------------------------------------------------
// wait for thread to end
void mgWinThread::waitForEnd(
  double ms)
{
  DWORD result = WaitForMultipleObjects(m_threadCount, m_handles, true, (DWORD) ms);
  if (result != WAIT_OBJECT_0)
    mgDebug("thread shutdown wait failed.");
}

