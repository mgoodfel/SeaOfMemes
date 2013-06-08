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

#include "OS/mgOSFileUtil.h"
#include <direct.h>

//--------------------------------------------------------------
// open file
FILE* mgOSFileOpen(
  const char* fileName,
  const char* mode)
{
  WCHAR* wideName; int wideLen;
  mgString::toWCHAR(fileName, wideName, wideLen);

  WCHAR* wideMode;
  mgString::toWCHAR(mode, wideMode, wideLen);
  FILE* result = _wfopen(wideName, wideMode);

  delete wideName;
  delete wideMode;
  return result;
}

//--------------------------------------------------------------
// set slashes appropriately for OS
void mgOSFixFileName(
  mgString& name)
{
  name.trim();

  // convert any forward slashs to backslashes
  mgString bs("\\");
  char letter[MG_MAX_LETTER];
  int posn = 0;
  while (posn < name.length())
  {
    int next = name.nextLetter(posn, letter);
    if (strcmp(letter, "/") == 0)
      next = name.setLetter(posn, bs);
    posn = next;
  }
}

//--------------------------------------------------------------
// resolve possibly relative file name.  
void mgOSResolveRelativeName(
  const char* sourceName,
  const char* relName,
  mgString& absName)
{
  mgString name(relName);
  mgOSFixFileName(name);

  // if relative name starts with backslash, we're done
  if (name.startsWith("\\"))
  {
    absName = name;
    return;
  }

  // if relative name starts with X: (drive letter), we're done
  char letter[MG_MAX_LETTER];
  int posn = name.nextLetter(0, letter);
  if (posn < name.length())
  {
    posn = name.nextLetter(posn, letter);
    if (strcmp(letter, ":") == 0)
    {
      absName = name;
      return;
    }
  }

  // assume it's really relative.  strip last directory in source file
  mgString sourceFile(sourceName);
  int lastSlash = sourceFile.reverseFind(sourceFile.length(), '\\');
  if (lastSlash != -1)
    sourceFile.substring(absName, 0, lastSlash+1);
  else absName.empty();  // source file has no dir

  absName += name;
}

//--------------------------------------------------------------
// move current working directory up tree until file is found
void mgOSFindWD(
  const char* fileName)
{
  WCHAR* oldCWD = _wgetcwd(NULL, 0);
  if (oldCWD == NULL)
    throw new mgException("unable to get _getcwd");
  mgString cwd(oldCWD);
  //mgDebug("CWD is %s", (const char*) cwd);

  BOOL changed = false;
  while (true)
  {
    mgString name;
    if (cwd.endsWith("\\"))
      name.format("%s%s", (const char*) cwd, fileName);
    else name.format("%s\\%s", (const char*) cwd, fileName);

    WCHAR* wideName;
    int wideLen;
    name.toWCHAR(wideName, wideLen);
    struct _stat filestat;
    BOOL found = 0 == _wstat(wideName, &filestat);
    delete wideName; wideName = NULL;

    if (found)
    {
//      mgDebug("found %s", (const char*) name);
      break;
    }
    else
    {
      // remove last directory from cwd
      int slash = cwd.reverseFind(cwd.length(), '\\');
      if (slash != -1)
      {
        cwd.deleteAt(slash, cwd.length()-slash);
        changed = true;
      }
      else 
      {
        // no more directories. file not found.
        mgString start(oldCWD);
        throw new mgException("mgOSFindWD cannot find %s from %s", 
          (const char*) fileName, (const char*) start);
      }
    }
  }

  // set the new working directory
  if (changed)
  {
    // move errors.txt to new location
    WCHAR* wideOld;
    int lenOld;
    mgString oldErrors(oldCWD);
    oldErrors += "\\errors.txt";
    oldErrors.toWCHAR(wideOld, lenOld);

    mgString newErrors(cwd);
    newErrors += "\\errors.txt";
    WCHAR* wideNew;
    int lenNew;
    newErrors.toWCHAR(wideNew, lenNew);
    MoveFileEx(wideOld, wideNew, MOVEFILE_REPLACE_EXISTING);

    delete wideOld; wideOld = NULL;
    delete wideNew; wideNew = NULL;

    if (0 != _chdir(cwd))
      throw new mgException("unable to _chdir(%s)", (const char*) cwd);
  }
  free(oldCWD); oldCWD = NULL;
}

//--------------------------------------------------------------
// find a file in parent directories
BOOL mgOSFindFile(
  mgString& location,
  const char* fileName)
{
  WCHAR* oldCWD = _wgetcwd(NULL, 0);
  if (oldCWD == NULL)
    return false;
  mgString cwd(oldCWD);

  while (true)
  {
    if (cwd.endsWith("\\"))
      location.format("%s%s", (const char*) cwd, fileName);
    else location.format("%s\\%s", (const char*) cwd, fileName);

    WCHAR* wideName;
    int wideLen;
    location.toWCHAR(wideName, wideLen);
    struct _stat filestat;
    BOOL found = 0 == _wstat(wideName, &filestat);
    delete wideName; wideName = NULL;

    if (found)
      return true;
    else
    {
      // remove last directory from cwd
      int slash = cwd.reverseFind(cwd.length(), '\\');
      if (slash != -1)
        cwd.deleteAt(slash, cwd.length()-slash);
      else return false;   // no more directories. file not found.
    }
  }
}
#endif
