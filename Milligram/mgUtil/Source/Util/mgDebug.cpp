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
CRITICAL_SECTION m_debugLock;
#endif

BOOL m_mgDebugInitialized = false;
BOOL m_debugConsole = false;

void mgDebugInit(
  BOOL console)
{
#ifdef WIN32
  InitializeCriticalSection(&m_debugLock);
#endif
  m_mgDebugInitialized = true;
  m_debugConsole = console;
}

void mgDebugTerm()
{
  if (!m_mgDebugInitialized)
    return;
#ifdef WIN32
  DeleteCriticalSection(&m_debugLock);
#endif
}

// reset trace file
void mgDebugReset(
  BOOL console)
{
  if (!m_mgDebugInitialized)
    mgDebugInit(console);
    
  FILE *errors = fopen("errors.txt", "wt");
  fclose(errors);
}

// write message to mgDebug output
void mgDebug(
  const char* format,
  ...)
{
  if (!m_mgDebugInitialized)
    mgDebugInit(m_debugConsole);

#ifdef WIN32    
  EnterCriticalSection(&m_debugLock);
#endif

  va_list args;
  va_start(args, format);

  mgString msg;
  msg.formatV(format, args);

  FILE *errors = fopen("errors.txt", "a+t");
  fprintf(errors, "%s\n", (const char*) msg);
  fclose(errors);

  if (m_debugConsole)
    printf("%s\n", (const char*) msg);

#ifdef WIN32    
  LeaveCriticalSection(&m_debugLock);
#endif
}
