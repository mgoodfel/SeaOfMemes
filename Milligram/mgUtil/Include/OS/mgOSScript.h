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
#if !defined(MGOSSCRIPT_H) && defined(EMSCRIPTEN)
#define MGOSSCRIPT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#ifndef _stat
#define _stat stat
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef offsetof
#define offsetof(type, element) (size_t) (&((type *) 0)->element)
#endif

#ifndef MAX_PATH
#define MAX_PATH 512
#endif

typedef wchar_t WCHAR;
typedef long long int INT64;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int BOOL;
typedef int LONG;
#define TRUE 1 
#define FALSE 0

#endif
