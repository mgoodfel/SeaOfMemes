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

#include "mgLinuxEvent.h"

//--------------------------------------------------------------
// constructor
mgLinuxEvent::mgLinuxEvent()
{
  pthread_mutex_init(&m_mutex, NULL);
  pthread_cond_init(&m_condition, NULL);
}

//--------------------------------------------------------------
// destructor
mgLinuxEvent::~mgLinuxEvent()
{
  pthread_mutex_destroy(&m_mutex);
  pthread_cond_destroy(&m_condition);
}

//--------------------------------------------------------------
// signal the event
void mgLinuxEvent::signal()
{
  pthread_mutex_lock(&m_mutex);
  pthread_cond_signal(&m_condition);
  pthread_mutex_unlock(&m_mutex);
}

//--------------------------------------------------------------
// wait for the event to be signalled
void mgLinuxEvent::wait(
  double limit)
{
  pthread_mutex_lock(&m_mutex);
  pthread_cond_wait(&m_condition, &m_mutex);
  pthread_mutex_unlock(&m_mutex);
}

#endif
