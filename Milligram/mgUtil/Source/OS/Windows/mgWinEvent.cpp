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
#ifdef WIN32

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgWinEvent.h"

//--------------------------------------------------------------
// constructor
mgWinEvent::mgWinEvent()
{
  m_event = CreateEvent(NULL, false, false, NULL);
}

//--------------------------------------------------------------
// destructor
mgWinEvent::~mgWinEvent()
{
  CloseHandle(m_event);
}

//--------------------------------------------------------------
// signal the event
void mgWinEvent::signal()
{
  SetEvent(m_event);
}

//--------------------------------------------------------------
// wait for the event to be signalled
void mgWinEvent::wait(
  double limit)
{
  WaitForSingleObject(m_event, (DWORD) limit);
}

#endif
