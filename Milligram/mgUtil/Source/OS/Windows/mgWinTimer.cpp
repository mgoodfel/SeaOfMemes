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

#include "OS/mgOSTimer.h"

__int64 m_timerFreq = 1;   // high performance counter frequency
__int64 m_baseTime = 0;

//--------------------------------------------------------------
// initialize performance counter
void mgOSInitTimer()
{
  LARGE_INTEGER timerFreq;
  if (!QueryPerformanceFrequency(&timerFreq) || timerFreq.QuadPart == 0)
    throw new mgException("no high resolution timer available.");
  
  m_timerFreq = (__int64) timerFreq.QuadPart;
  m_timerFreq /= 10000;  // ticks per ms*10
  m_timerFreq = max(1, m_timerFreq);
  mgDebug(":Machine TimerFreq: %I64d ticks per ms.", m_timerFreq*10);
  
  LARGE_INTEGER baseTime;
  QueryPerformanceCounter(&baseTime);
  m_baseTime = (__int64) baseTime.QuadPart;
  
  double now = mgOSGetTime();
  double when;
  while (true)
  {
    when = mgOSGetTime();
    if (when != now)
      break;
  }
  mgDebug(":Machine TimerRes: %g ms", when-now);
}

//--------------------------------------------------------------
// return performance counter time
double mgOSGetTime()
{
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  
  __int64 when = (__int64) now.QuadPart;
  when -= m_baseTime;
  
  __int64 result = when / m_timerFreq;
  double done = (double) result;
  done /= 10.0;
//  mgDebug("time is %I64d over %I64d is %f", when, m_timerFreq, done);
  return done;
}


