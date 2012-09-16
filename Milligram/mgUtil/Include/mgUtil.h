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
#ifndef MGUTIL_H
#define MGUTIL_H

#ifdef WIN32

#pragma comment(lib, "mgUtil.lib")

#include "OS/mgOSWindows.h"

#ifdef DEBUG_MEMORY
#include "Util/mgDebugMemory.h"
#endif

#endif

#if defined(__unix__)
#include "OS/mgOSLinux.h"
#include "OS/mgLinuxMisc.h"
#endif

#if defined(__APPLE__)
#include "OS/mgOSOSX.h"
#endif

// Include the entire framework.  Most apps won't need all of this

// utilities
#include "Util/mgString.h"
#include "Util/mgDebug.h"
#include "Util/mgException.h"
#include "Util/mgErrorMsg.h"
#include "Util/mgMatrix.h"
#include "Util/mgQuat.h"
#include "Util/mgDWordArray.h"
#include "Util/mgPtrArray.h"
#include "Util/mgStringArray.h"
#include "Util/mgMapStringToPtr.h"
#include "Util/mgMapStringToDWord.h"
#include "Util/mgMapStringToDouble.h"
#include "Util/mgMapStringToString.h"
#include "Util/mgMapPtrToPtr.h"
#include "Util/mgMapDWordToPtr.h"
#include "Util/mgMapXYZToPtr.h"
#include "Util/mgBlockPool.h"
#include "Util/mgUtilErrorTable.h"

// file formats
#include "Util/mgOptionsFile.h"
//#include "Util/mgObjectStore.h"

// operating system services
#include "OS/mgMiscUtil.h"
#include "OS/mgOSTimer.h"
#include "OS/mgOSFileUtil.h"
#include "OS/mgOSLock.h"
#include "OS/mgOSThread.h"
#include "OS/mgOSEvent.h"

// graphics utilities
#include "GraphicsUtil/mgBezier.h"
#include "GraphicsUtil/mgSimplexNoise.h"

#endif
