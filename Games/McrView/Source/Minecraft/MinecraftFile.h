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

#ifndef MINECRAFTFILE_H
#define MINECRAFTFILE_H

#include "NBTFile.h"

class MinecraftFile : public NBTFile
{
public:
  int m_xpos;
  int m_zpos;
  int m_xlen;
  int m_ylen;
  int m_zlen;

  // constructor
  MinecraftFile();

  // destructor
  virtual ~MinecraftFile();

  // get a block at point
  BYTE getBlock(
    int x,
    int y,
    int z)
  {
    return m_blocks[y + m_ylen * (z + (x * m_zlen))];
  }

  // get extra data for block at point
  BYTE getModifier(
    int x,
    int y,
    int z)
  {
    int posn = y + m_ylen * (z + (x * m_zlen));
    BYTE blockData = m_data[posn/2];
    if ((posn&1) == 0)
      blockData = blockData & 0xF;
    else blockData = (blockData >> 4) & 0xF;

    return blockData;
  }

  // get sky light and block light data at point
  void getLights(
    int x,
    int y,
    int z,
    int& skyInten,
    int& blockInten)
  {
    int posn = y + m_ylen * (z + (x * m_zlen));

    skyInten = -1;
    if (m_skyLight != NULL)
    {
      BYTE data = m_skyLight[posn/2];
      if ((posn&1) == 0)
        skyInten = data & 0xF;
      else skyInten = (data >> 4) & 0xF;
    }

    blockInten = -1;
    if (m_blockLight != NULL)
    {
      BYTE data = m_blockLight[posn/2];
      if ((posn&1) == 0)
        blockInten = data & 0xF;
      else blockInten = (data >> 4) & 0xF;
    }
  }

  // reset for new parse
  virtual void reset();

protected:
  BYTE* m_blocks;
  BYTE* m_data;
  BYTE* m_skyLight;
  BYTE* m_blockLight;

  int m_arrayPosn;
  int m_arrayTarget;

  virtual void tagByte(
    const char* name,
    BYTE value);

  virtual void tagShort(
    const char* name,
    short value);

  virtual void tagInt(
    const char* name,
    int value);

  virtual void tagLong(
    const char* name,
    INT64 value);

  virtual void tagFloat(
    const char* name,
    float value);

  virtual void tagDouble(
    const char* name,
    double value);

  virtual void tagByteArrayBegin(
    const char* name,
    int len);

  virtual void byteArrayBuffer(
    BYTE* buffer,
    int len);

  virtual void tagByteArrayEnd(
    const char* name);

  virtual void tagString(
    const char* name,
    const mgString& value);

  virtual void tagListBegin(
    const char* name);

  virtual void tagListEnd(
    const char* name);

  virtual void tagCompoundBegin(
    const char* name);

  virtual void tagCompoundEnd(
    const char* name);

};

#endif
