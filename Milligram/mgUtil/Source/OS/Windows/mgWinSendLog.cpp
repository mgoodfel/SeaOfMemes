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
// handle growing character buffer.
#include "stdafx.h"
#ifdef WIN32

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

//--------------------------------------------------------------
// launch the mgSendLog application
void mgOSLaunchSendLog()
{
  STARTUPINFO si;
  
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi;
  memset(&pi, 0, sizeof(pi));

  mgString location;
  mgOSFindFile(location, "mgSendLog.exe");

  DWORD ourId = GetCurrentProcessId();  
  mgString cmdLine;
  cmdLine.format("%s %d", (const char*) location, ourId);
  WCHAR* wideCmd;
  int wideLen;
  cmdLine.toWCHAR(wideCmd, wideLen);

  BOOL ok = CreateProcess(NULL, wideCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
  if (ok)
  {
    // we don't wait for end, so clean up
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }
  else
  {
    mgDebug("CreateProcess(mgSendLog) failed, rc = %d", GetLastError());
  }

  delete wideCmd;  wideCmd = NULL;
}

#endif
