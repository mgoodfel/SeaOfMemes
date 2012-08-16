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

#ifndef NBTFILE_H
#define NBTFILE_H

class NBTFile
{
public:
  // constructor
  NBTFile();

  // destructor
  virtual ~NBTFile();

  // read file and parse it
  virtual void parseFile(
    const char* fileName);

  // parse generic input source
  virtual void parse();

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

protected:
  mgString m_indent;              // indent for debug output

  FILE* m_sourceFile;             // input file
  BYTE* m_source;                 // source data read
  int m_sourceLen;                // length available in block
  int m_sourcePosn;               // position in block

  // read a block of source data
  virtual void readSource();

  // read a byte
  BYTE readByte()
  {
    if (m_sourcePosn >= m_sourceLen)
      readSource();
    return m_source[m_sourcePosn++];
  }

  // read short data
  virtual short readShort();

  // read int data
  virtual int readInt();

  // read long data
  virtual INT64 readLong();

  // read float data
  virtual float readFloat();

  // read double data
  virtual double readDouble();

  // read a string tag
  virtual void readString(
    mgString& name);

  // read a byte array
  void readByteArray(
    int len);

  // read a list
  void readList(
    int elementType,
    int len);

  // read a compound tag
  virtual void readCompound();
};

#endif
