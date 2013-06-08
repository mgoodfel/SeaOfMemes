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
#ifndef MGOSTHREAD_H
#define MGOSTHREAD_H

class mgOSThread;

typedef void (*mgOSThreadCallback) (
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2);

/*
  A <i>group</i> of threads (constructor threadCount) is created.  suspend, resume, setPriority and
  waitForEnd apply to ALL threads of the group.  All threads call the same callback with the
  same threadArgs pointer.
*/
class mgOSThread
{
public:
  static const int PRIORITY_LOW     = 1;
  static const int PRIORITY_NORMAL  = 2;
  static const int PRIORITY_HIGH    = 3;

  // constructor
  static mgOSThread* create(
    int threadCount,
    mgOSThreadCallback callback,
    int priority,
    void* threadArg1,
    void** threadArgs);

  // destructor
  virtual ~mgOSThread()
  {}

  // set priority
  virtual void setPriority(
    int priority) = 0;

  // wait for all threads to end
  virtual void waitForEnd(
    double ms) = 0;
};

// create a new thread group
mgOSThread* mgOSNewThread();

#endif
