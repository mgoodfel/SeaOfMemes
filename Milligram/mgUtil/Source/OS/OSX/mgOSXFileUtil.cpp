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

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "OS/mgOSFileUtil.h"

//--------------------------------------------------------------
// open file
FILE* mgOSFileOpen(
  const char* fileName,
  const char* mode)
{
  // filename is UTF-8, so just open it
  FILE* result = fopen(fileName, mode);
  return result;
}

//--------------------------------------------------------------
// set slashes appropriately for OS
void mgOSFixFileName(
  mgString& name)
{
  name.trim();

  // convert any forward slashs to backslashes
  mgString slash("/");
  char letter[MG_MAX_LETTER];
  int posn = 0;
  while (posn < name.length())
  {
    int next = name.nextLetter(posn, letter);
    if (strcmp(letter, "\\") == 0)
      next = name.setLetter(posn, slash);
    posn = next;
  }
}

//--------------------------------------------------------------
// resolve possibly relative file name.  
void mgOSResolveRelativeName(
  const char* sourceFile,
  const char* relName,
  mgString& absName)
{
  mgString name(relName);
  mgOSFixFileName(name);

  // if relative name starts with /, we're done
  if (name.startsWith("/"))
  {
    absName = name;
    return;
  }

  // assume it's really relative.  strip last directory in source file
  absName = sourceFile;
  int lastSlash = absName.reverseFind(absName.length(), "/");
  if (lastSlash != -1)
    absName.deleteAt(lastSlash+1, absName.length()-(lastSlash+1));
  else absName.empty();  // source file has no dir

  absName += name;
}

//--------------------------------------------------------------
// move current working directory up tree until file is found
void mgOSFindWD(
  const char* fileName)
{
  char* oldCWD = getcwd(NULL, 0);
  if (oldCWD == NULL)
    throw new mgException("unable to get getcwd");

  BOOL changed = false;
  mgString cwd(oldCWD);
  while (true)
  {
    mgString name;
    if (cwd.endsWith("/"))
      name.format("%s%s", (const char*) cwd, fileName);
    else name.format("%s/%s", (const char*) cwd, fileName);

    struct stat filestat;
    if (0 == stat(name, &filestat))
    {
//      mgDebug("found %s", (const char*) name);
      break;
    }
    else
    {
      // remove last directory from cwd
      int slash = cwd.reverseFind(cwd.length(), '/');
      if (slash != -1)
      {
        cwd.deleteAt(slash, cwd.length()-slash);
        changed = true;
      }
      else 
      {
        // no more directories. file not found.
        throw new mgException("mgOSFindWD cannot find %s", fileName);
      }
    }
  }

  // set the new working directory
  if (changed)
  {
    // move errors.txt to new location
    mgString oldErrors, newErrors;
    oldErrors.format("%s/errors.txt", (const char*) oldCWD);
    newErrors.format("%s/errors.txt", (const char*) cwd);
    rename(oldErrors, newErrors);

    if (0 != chdir(cwd))
      throw new mgException("unable to chdir(%s)", (const char*) cwd);
  }
  free(oldCWD);
}

//--------------------------------------------------------------
// find a file in parent directories
BOOL mgOSFindFile(
  mgString& location,
  const char* fileName)
{
  char* oldCWD = getcwd(NULL, 0);
  if (oldCWD == NULL)
    return false;

  mgString cwd(oldCWD);
  while (true)
  {
    if (cwd.endsWith("/"))
      location.format("%s%s", (const char*) cwd, fileName);
    else location.format("%s/%s", (const char*) cwd, fileName);

    struct stat filestat;
    if (0 == stat(location, &filestat))
      return true;
    else
    {
      // remove last directory from cwd
      int slash = cwd.reverseFind(cwd.length(), '/');
      if (slash != -1)
        cwd.deleteAt(slash, cwd.length()-slash);
      else return false;  // no more directories. file not found.
    }
  }
}

#endif
