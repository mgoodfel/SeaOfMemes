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

#ifdef __unix__

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "OS/mgOSTimer.h"

int m_baseTime;

//--------------------------------------------------------------
// initialize performance counter
void mgOSInitTimer()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  m_baseTime = ts.tv_sec;

  double avgRes = 0.0;
  for (int i = 0; i < 10; i++)
  {
    double now = mgOSGetTime();
    double when;
    while (true)
    {
      when = mgOSGetTime();
      if (when != now)
        break;
    }
    avgRes += when-now;
  }
  mgDebug(":Machine TimerRes: %g ms", avgRes/10);
}

//--------------------------------------------------------------
// return performance counter time
double mgOSGetTime()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  // return time in milliseconds
  return (ts.tv_sec-m_baseTime) * 1000.0 + ts.tv_nsec/1000000.0;
}

#endif
