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
#ifdef __APPLE__

#include <unistd.h>
#include <sys/errno.h>
#include <sys/wait.h>

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

//--------------------------------------------------------------
// launch the mgSendLog application
void mgOSLaunchSendLog()
{
  // fork and let the child run the app
  pid_t app = fork();
  if (app == 0 || app == -1)
    return;

  // wait for app to end
  int status;
  pid_t child = waitpid(app, &status, 0);

  // send return code to sendLog
  char statusStr[10];
  if (WIFEXITED(status))
    sprintf(statusStr, "%d", WEXITSTATUS(status));
  else sprintf(statusStr, "-666");

  try
  {  
    mgOSFindWD("errors.txt");
  }
  catch (...)
  {
    mgDebug("could not find errors.txt");
    // something is seriously wrong!
    _exit(0);
  }

  mgString location;
  if (!mgOSFindFile(location, "mgSendLog"))
  {
    mgDebug("could not find mgSendLog");
    _exit(0);
  }

  if (execl(location, location, statusStr, NULL) == -1)
  {
    mgDebug("exec(mgSendLog) failed, rc = %d", errno);
    _exit(0);
  }
}

#endif
