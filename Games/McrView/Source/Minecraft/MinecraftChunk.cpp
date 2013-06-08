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

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "ZLib/Include/zlib.h"

#include "MinecraftRegion.h"
#include "MinecraftChunk.h"

const int ZLIB_READ_SIZE = 16384;
const int ZLIB_WRITE_SIZE = 32768;

//--------------------------------------------------------------
// constructor
MinecraftChunk::MinecraftChunk()
{
  m_zlibData = new BYTE[ZLIB_READ_SIZE];
  m_source = new BYTE[ZLIB_WRITE_SIZE];
  m_zlibStream = NULL;
}

//--------------------------------------------------------------
// parse a chunk from the region file
void MinecraftChunk::parseRegion(
  MinecraftRegion* region,
  int chunkPosn,
  int chunkLen)
{
  // reset from last use
  reset();

  m_region = region;
  m_chunkPosn = chunkPosn;
  m_chunkLen = chunkLen;

  z_stream* strm = new z_stream;
  strm->zalloc = Z_NULL;
  strm->zfree = Z_NULL;
  strm->opaque = Z_NULL;
  strm->avail_in = 0;
  strm->next_in = Z_NULL;
  int ret = inflateInit(strm);
  if (ret != Z_OK)
    throw new mgException("unable to initialize ZLib stream");

  m_zlibStream = strm;

  // position on empty buffer.  first decompress will force read of more source
  strm->avail_in = 0;
  strm->next_in = m_zlibData;

  parse();

  // free zlib stream
  inflateEnd(strm);
  delete strm;
  m_zlibStream = NULL;
}

//--------------------------------------------------------------
// destructor
MinecraftChunk::~MinecraftChunk()
{
  delete m_source;
  m_source = NULL;

  delete m_zlibData;
  m_zlibData = NULL;
}

//--------------------------------------------------------------
// read a buffer from the source file
void MinecraftChunk::readSource()
{
  z_stream* strm = (z_stream*) m_zlibStream;

  while (true)
  {
    // try to decompress from current input
    strm->avail_out = ZLIB_WRITE_SIZE;
    strm->next_out = m_source;
    int ret = inflate(strm, Z_NO_FLUSH);

    // check for errors
    const char* msg = NULL;
    switch (ret) 
    {
      case Z_NEED_DICT:
        msg = "Need Dict";
        break;
      case Z_DATA_ERROR:
        msg = "Data Error";
        break;
      case Z_MEM_ERROR:
        msg = "Memory Error";
        break;
    }
    if (msg != NULL)
      throw new mgException("ZLib inflate error %s", (const char*) msg);

    m_sourceLen = ZLIB_WRITE_SIZE - strm->avail_out;
    m_sourcePosn = 0;

    // if no data produced, read more input
    if (m_sourceLen == 0)
    {
      // move old input down
      memcpy(m_zlibData, strm->next_in, strm->avail_in);

      m_region->lock();
      FILE* regionFile = m_region->getFile();
      fseek(regionFile, m_chunkPosn, SEEK_SET);
      int readLen = fread(m_zlibData + strm->avail_in, 1, 
                          ZLIB_READ_SIZE - strm->avail_in, regionFile);
      m_region->unlock();

      if (readLen <= 0)
        throw new mgException("unexpected end of chunk data");

      m_chunkPosn += readLen;
      m_chunkLen -= readLen;

      strm->avail_in += readLen;
      strm->next_in = m_zlibData;
    }
    else break;
  }
}
