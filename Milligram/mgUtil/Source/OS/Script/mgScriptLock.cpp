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
#ifdef EMSCRIPTEN

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

/*
  This class does nothing, since Threads are not supported under Javascript.
  It is supplied to keep compile and link happy for demos.
*/

#include "mgScriptLock.h"

mgScriptLock g_globalLock;

//--------------------------------------------------------------
// constructor
mgScriptLock::mgScriptLock()
{
}

//--------------------------------------------------------------
// destructor
mgScriptLock::~mgScriptLock()
{
}

//--------------------------------------------------------------
// lock it
void mgScriptLock::lock()
{
}

//--------------------------------------------------------------
// unlock it
void mgScriptLock::unlock()
{
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
