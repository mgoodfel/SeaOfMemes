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

#include "NBTFile.h"

// block size when using ParseFile
const int FILE_BLOCK_SIZE = 32768;

const int NBT_TAG_END       = 0;
const int NBT_TAG_BYTE      = 1;
const int NBT_TAG_SHORT     = 2;
const int NBT_TAG_INT       = 3;
const int NBT_TAG_LONG      = 4;
const int NBT_TAG_FLOAT     = 5;
const int NBT_TAG_DOUBLE    = 6;
const int NBT_TAG_BYTEARRAY = 7;
const int NBT_TAG_STRING    = 8;
const int NBT_TAG_LIST      = 9;
const int NBT_TAG_COMPOUND  = 10;

//--------------------------------------------------------------
// constructor
NBTFile::NBTFile()
{
  m_sourceFile = NULL;
  m_source = NULL;
  m_sourceLen = 0;
  m_sourcePosn = 0;
}

//--------------------------------------------------------------
// destructor
NBTFile::~NBTFile()
{
}

//--------------------------------------------------------------
// parse file
void NBTFile::parseFile(
  const char* fileName)
{
  m_sourceFile = mgOSFileOpen(fileName, "rb");
  if (m_sourceFile == NULL)
    throw new mgException("file %s not found", (const char*) fileName);
  m_source = new BYTE[FILE_BLOCK_SIZE];
  m_sourceLen = 0;
  m_sourcePosn = 0;

  parse();

  fclose(m_sourceFile);
  m_sourceFile = NULL;

  delete m_source;
  m_source = NULL;
}

//--------------------------------------------------------------
// parse input source
void NBTFile::parse()
{
  try
  {
    BYTE tagType = readByte();
    if (tagType != NBT_TAG_COMPOUND)
      throw new mgException("top tag not a compound tag.  type = %d", tagType);

    mgString name;
    readString(name);

    tagCompoundBegin(name);
    readCompound();
    tagCompoundEnd(name);
  }
  catch (mgException* e)
  {
    mgDebug("read failed.  %s", (const char*) e->m_message);
    delete e;
  }
}

//--------------------------------------------------------------
// read a byte
void NBTFile::readSource()
{
  m_sourceLen = fread(m_source, 1, FILE_BLOCK_SIZE, m_sourceFile);
  if (m_sourceLen <= 0)
    throw new mgException("premature end of data in NBTFile");
  m_sourcePosn = 0;
}

//--------------------------------------------------------------
// read short data
short NBTFile::readShort()
{
  short value = readByte();
  value = (value << 8) | readByte();
  return value;
}

//--------------------------------------------------------------
// read int data
int NBTFile::readInt()
{
  int value = readByte();
  value = (value << 8) | readByte();
  value = (value << 8) | readByte();
  value = (value << 8) | readByte();
  return value;
}

//--------------------------------------------------------------
// read long data
INT64 NBTFile::readLong()
{
  INT64 value = readByte();
  value = (value << 8) | readByte();
  value = (value << 8) | readByte();
  value = (value << 8) | readByte();
  value = (value << 8) | readByte();
  value = (value << 8) | readByte();
  value = (value << 8) | readByte();
  value = (value << 8) | readByte();
  return value;
}

//--------------------------------------------------------------
// read float data
float NBTFile::readFloat()
{
  float value = 1.0f;
  BYTE* valueBytes = (BYTE*) &value;
  valueBytes[3] = readByte();
  valueBytes[2] = readByte();
  valueBytes[1] = readByte();
  valueBytes[0] = readByte();
  return value;
}

//--------------------------------------------------------------
// read double data
double NBTFile::readDouble()
{
  double value = 1.0;
  BYTE* valueBytes = (BYTE*) &value;
  valueBytes[7] = readByte();
  valueBytes[6] = readByte();
  valueBytes[5] = readByte();
  valueBytes[4] = readByte();
  valueBytes[3] = readByte();
  valueBytes[2] = readByte();
  valueBytes[1] = readByte();
  valueBytes[0] = readByte();
  return value;
}

//--------------------------------------------------------------
// read a string tag
void NBTFile::readString(
  mgString& name)
{
  int len = readShort();

  name.empty();
  while (len-- > 0)
  {
    name += (char) readByte();
  }
}

//--------------------------------------------------------------
// read a byte array
void NBTFile::readByteArray(
  int len)
{
  while (len > 0)
  {
    if (m_sourcePosn >= m_sourceLen)
      readSource();

    int readLen = min(len, m_sourceLen - m_sourcePosn);
    if (readLen <= 0)
      break;
    byteArrayBuffer(m_source+m_sourcePosn, readLen);
    len -= readLen;
    m_sourcePosn += readLen;
  }
}

//--------------------------------------------------------------
// read a list
void NBTFile::readList(
  int elementType,
  int len)
{
  for (int i = 0; i < len; i++)
  {
    // read the value by type
    switch (elementType)
    {
      case NBT_TAG_BYTE:
      {
        BYTE value = readByte();
        tagByte("", value);
        break;
      }

      case NBT_TAG_SHORT:
      {
        short value = readShort();
        tagShort("", value);
        break;
      }

      case NBT_TAG_INT:
      {
        int value = readInt();
        tagInt("", value);
        break;
      }

      case NBT_TAG_LONG:
      {
        INT64 value = readLong();
        tagLong("", value);
        break;
      }

      case NBT_TAG_FLOAT:
      {
        float value = readFloat();
        tagFloat("", value);
        break;
      }

      case NBT_TAG_DOUBLE:
      {
        double value = readDouble();
        tagDouble("", value);
        break;
      }

      case NBT_TAG_BYTEARRAY:
      {
        int len = readInt();
        tagByteArrayBegin("", len);
        readByteArray(len);
        tagByteArrayEnd("");
        break;
      }

      case NBT_TAG_STRING:
      {
        mgString string;
        readString(string);
        tagString("", string);
        break;
      }

      case NBT_TAG_LIST:
      {
        int elementType = readByte();
        int len = readInt();
        tagListBegin("");
        readList(elementType, len);
        tagListEnd("");
        break;
      }

      case NBT_TAG_COMPOUND:
      {
        tagCompoundBegin("");
        readCompound();
        tagCompoundEnd("");
        break;
      }
    }
  }
}

//--------------------------------------------------------------
// read a compound tag
void NBTFile::readCompound()
{
  mgString name;
  while (true)
  {
    int tagType = readByte();
    if (tagType == NBT_TAG_END)
      break;

    // read the name
    readString(name);

    // read the value by type
    switch (tagType)
    {
      case NBT_TAG_BYTE:
      {
        BYTE value = readByte();
        tagByte(name, value);
        break;
      }

      case NBT_TAG_SHORT:
      {
        short value = readShort();
        tagShort(name, value);
        break;
      }

      case NBT_TAG_INT:
      {
        int value = readInt();
        tagInt(name, value);
        break;
      }

      case NBT_TAG_LONG:
      {
        INT64 value = readLong();
        tagLong(name, value);
        break;
      }

      case NBT_TAG_FLOAT:
      {
        float value = readFloat();
        tagFloat(name, value);
        break;
      }

      case NBT_TAG_DOUBLE:
      {
        double value = readDouble();
        tagDouble(name, value);
        break;
      }

      case NBT_TAG_BYTEARRAY:
      {
        int len = readInt();
        tagByteArrayBegin(name, len);
        readByteArray(len);
        tagByteArrayEnd(name);
        break;
      }

      case NBT_TAG_STRING:
      {
        mgString string;
        readString(string);
        tagString(name, string);
        break;
      }

      case NBT_TAG_LIST:
      {
        int elementType = readByte();
        int len = readInt();
        tagListBegin(name);
        readList(elementType, len);
        tagListEnd(name);
        break;
      }

      case NBT_TAG_COMPOUND:
      {
        tagCompoundBegin(name);
        readCompound();
        tagCompoundEnd(name);
        break;
      }
    }
  }
}

//--------------------------------------------------------------
void NBTFile::tagByte(
  const char* name,
  BYTE value)
{
  mgDebug("%stag %s byte %02x", (const char*) m_indent, (const char*) name, value);
}

//--------------------------------------------------------------
void NBTFile::tagShort(
  const char* name,
  short value)
{
  mgDebug("%stag %s short %d", (const char*) m_indent, (const char*) name, value);
}

//--------------------------------------------------------------
void NBTFile::tagInt(
  const char* name,
  int value)
{
  mgDebug("%stag %s int %d", (const char*) m_indent, (const char*) name, value);
}

//--------------------------------------------------------------
void NBTFile::tagLong(
  const char* name,
  INT64 value)
{
  mgDebug("%stag %s long %I64d", (const char*) m_indent, (const char*) name, value);
}

//--------------------------------------------------------------
void NBTFile::tagFloat(
  const char* name,
  float value)
{
  mgDebug("%stag %s float %f", (const char*) m_indent, (const char*) name, value);
}

//--------------------------------------------------------------
void NBTFile::tagDouble(
  const char* name,
  double value)
{
  mgDebug("%stag %s double %g", (const char*) m_indent, (const char*) name, value);
}

//--------------------------------------------------------------
void NBTFile::tagByteArrayBegin(
  const char* name,
  int len)
{
  mgDebug("%stag %s ByteArray len=%d", (const char*) m_indent, (const char*) name, len);
  m_indent += "  ";
}

//--------------------------------------------------------------
void NBTFile::byteArrayBuffer(
  BYTE* buffer,
  int len)
{
  mgDebug("%s%d bytes: %02x %02x %02x %02x %02x %02x %02x %02x ", 
        (const char*) m_indent, len, 
        buffer[0], buffer[1], buffer[2], buffer[3], 
        buffer[4], buffer[5], buffer[6], buffer[7]);
}

//--------------------------------------------------------------
void NBTFile::tagByteArrayEnd(
  const char* name)
{
  m_indent.deleteAt(0, 2);
  mgDebug("%sbyte array %s ends", (const char*) m_indent, name);
}

//--------------------------------------------------------------
void NBTFile::tagString(
  const char* name,
  const mgString& value)
{
  mgDebug("%stag%s string \"%s\"", (const char*) m_indent, (const char*) name, (const char*) value);
}

//--------------------------------------------------------------
void NBTFile::tagListBegin(
  const char* name)
{
  mgDebug("%stag %s List begins", (const char*) m_indent, (const char*) name);
  m_indent += "  ";
}

//--------------------------------------------------------------
void NBTFile::tagListEnd(
  const char* name)
{
  m_indent.deleteAt(0, 2);
  mgDebug("%stag %s List ends", (const char*) m_indent, (const char*) name);
}

//--------------------------------------------------------------
void NBTFile::tagCompoundBegin(
  const char* name)
{
  mgDebug("%stag %s Compound begins", (const char*) m_indent, (const char*) name);
  m_indent += "  ";
}

//--------------------------------------------------------------
void NBTFile::tagCompoundEnd(
  const char* name)
{
  m_indent.deleteAt(0, 2);
  mgDebug("%stag %s Compound ends", (const char*) m_indent, (const char*) name);
}

