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

#include "MinecraftFile.h"

const int TARGET_NONE = 0;
const int TARGET_BLOCKS = 1;
const int TARGET_DATA = 2;
const int TARGET_SKY_LIGHT = 3;
const int TARGET_BLOCK_LIGHT = 4;

//--------------------------------------------------------------
// constructor
MinecraftFile::MinecraftFile()
{
  m_blocks = NULL;
  m_data = NULL;
  m_skyLight = NULL;
  m_blockLight = NULL;
  reset();
}

//--------------------------------------------------------------
// destructor
MinecraftFile::~MinecraftFile()
{
  reset();
}

//--------------------------------------------------------------
// reset for new parse
void MinecraftFile::reset()
{
  delete m_blocks;
  m_blocks = NULL;

  delete m_data;
  m_data = NULL;

  delete m_skyLight;
  m_skyLight = NULL;

  delete m_blockLight;
  m_blockLight = NULL;

  m_xlen = m_ylen = m_zlen = 0;
  m_xpos = m_zpos = 0;
  m_arrayTarget = TARGET_NONE;
}

//--------------------------------------------------------------
void MinecraftFile::tagByte(
  const char* name,
  BYTE value)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagShort(
  const char* name,
  short value)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagInt(
  const char* name,
  int value)
{
  if (_stricmp(name, "xPos") == 0)
    m_xpos = value;

  else if (_stricmp(name, "zPos") == 0)
    m_zpos = value;
}

//--------------------------------------------------------------
void MinecraftFile::tagLong(
  const char* name,
  INT64 value)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagFloat(
  const char* name,
  float value)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagDouble(
  const char* name,
  double value)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagByteArrayBegin(
  const char* name,
  int len)
{
  if (_stricmp(name, "Blocks") == 0)
  {
    m_xlen = 16;
    m_zlen = 16;
    m_ylen = 128;
    m_blocks = new BYTE[len];
    m_arrayPosn = 0;
    m_arrayTarget = TARGET_BLOCKS;
  }
  else if (_stricmp(name, "Data") == 0)
  {
    m_data = new BYTE[len];
    m_arrayPosn = 0;
    m_arrayTarget = TARGET_DATA;
  }
  else if (_stricmp(name, "SkyLight") == 0)
  {
    m_skyLight = new BYTE[len];
    m_arrayPosn = 0;
    m_arrayTarget = TARGET_SKY_LIGHT;
  }
  else if (_stricmp(name, "BlockLight") == 0)
  {
    m_blockLight = new BYTE[len];
    m_arrayPosn = 0;
    m_arrayTarget = TARGET_BLOCK_LIGHT;
  }
}

//--------------------------------------------------------------
void MinecraftFile::byteArrayBuffer(
  BYTE* buffer,
  int len)
{
  switch (m_arrayTarget)
  {
    case TARGET_BLOCKS:
      memcpy(m_blocks+m_arrayPosn, buffer, len);
      m_arrayPosn += len;
      break;

    case TARGET_DATA:
      memcpy(m_data+m_arrayPosn, buffer, len);
      m_arrayPosn += len;
      break;

    case TARGET_SKY_LIGHT:
      memcpy(m_skyLight+m_arrayPosn, buffer, len);
      m_arrayPosn += len;
      break;

    case TARGET_BLOCK_LIGHT:
      memcpy(m_blockLight+m_arrayPosn, buffer, len);
      m_arrayPosn += len;
      break;
  }
}

//--------------------------------------------------------------
void MinecraftFile::tagByteArrayEnd(
  const char* name)
{
  m_arrayTarget = TARGET_NONE;
}

//--------------------------------------------------------------
void MinecraftFile::tagString(
  const char* name,
  const mgString& value)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagListBegin(
  const char* name)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagListEnd(
  const char* name)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagCompoundBegin(
  const char* name)
{
}

//--------------------------------------------------------------
void MinecraftFile::tagCompoundEnd(
  const char* name)
{
}

