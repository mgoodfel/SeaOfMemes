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

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "MinecraftRegion.h"

//--------------------------------------------------------------
// constructor
MinecraftRegion::MinecraftRegion(
  const char* regionDir,
  int regionX,
  int regionZ)
{
  memset(m_header, 0, sizeof(m_header));

  m_lock = mgOSLock::create();

  // create the region file name
  m_fileName.format("%s/r.%d.%d.mcr", (const char*) regionDir, regionX, regionZ);
  mgOSFixFileName(m_fileName);

  // open the file
  m_regionFile = mgOSFileOpen(m_fileName, "rb");
  if (m_regionFile == NULL)
  {
    mgDebug("could not open region file %s", (const char*) m_fileName);
    m_exists = false;
    return;
  }
  else m_exists = true;

  // read the header
  int count = fread(m_header, 1, sizeof(m_header), m_regionFile);
  if (count != sizeof(m_header))
  {
    mgDebug("Could not read region (%d, %d) header -- %d bytes read, %d expected", 
      regionX, regionZ, count, sizeof(m_header));
    m_exists = false;
  }
}

//--------------------------------------------------------------
// destructor
MinecraftRegion::~MinecraftRegion()
{
  m_lock->lock();
  if (m_regionFile != NULL)
    fclose(m_regionFile);
  m_lock->unlock();

  delete m_lock;
  m_lock = NULL;
}

//--------------------------------------------------------------
// return file handle (open if necessary)
FILE* MinecraftRegion::getFile()
{
  if (!m_exists)
    return NULL;

  if (m_regionFile != NULL)
    return m_regionFile;

  m_regionFile = mgOSFileOpen(m_fileName, "rb");
  if (m_regionFile == NULL)
    m_exists = false;

  return m_regionFile;
}

//--------------------------------------------------------------
// close file handle
void MinecraftRegion::closeFile()
{
  m_lock->lock();
  if (m_regionFile != NULL)
  {
    fclose(m_regionFile);
    m_regionFile = NULL;
  }
  m_lock->unlock();
}

//--------------------------------------------------------------
// get location of chunk in region file.  return false if not present
BOOL MinecraftRegion::getChunkLoc(
  int chunkX,
  int chunkZ,
  int& chunkPosn, 
  int& chunkLen, 
  int& compressCode)
{
  // if this region does not exist, none of the chunks do
  if (!m_exists)
    return false;

  int headerPosn = 4 * ((chunkX%32) + (chunkZ%32) * 32);
  int loc = m_header[headerPosn++];
  loc = (loc << 8) | m_header[headerPosn++];
  loc = (loc << 8) | m_header[headerPosn++];
  int len = m_header[headerPosn];

  // if no chunk at this position
  if (loc == 0 || len == 0)
    return false;

  // locations are in 4K sectors
  chunkPosn = loc*4096;

  BYTE chunkHeader[5];
  m_lock->lock();
  FILE* regionFile = getFile();
  if (0 == fseek(m_regionFile, chunkPosn, SEEK_SET))
  {
    if (5 != fread(chunkHeader, 1, 5, regionFile))
    {
      mgDebug("could not read chunk data");
      memset(chunkHeader, 0, sizeof(chunkHeader));
    }
  }
  else mgDebug("could not seek to %d", chunkPosn);
  m_lock->unlock();

  // get length and format of compressed chunk data
  chunkLen = (chunkHeader[0] << 24) | (chunkHeader[1] << 16) |
             (chunkHeader[2] <<  8) | chunkHeader[3];
  compressCode = chunkHeader[4];

  chunkPosn += 5;  // start of data
  chunkLen -= 1;  // length after compress code

  return true;
}
