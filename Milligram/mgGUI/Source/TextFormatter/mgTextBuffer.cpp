/*
  Copyright (C) 1995-2011 by Michael J. Goodfellow

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

#include "TextFormatter/mgTextBuffer.h"

// Size to grow the buffer when we need more space.
const int BUFFER_PAD_SIZE = 4096;

//-----------------------------------------------------------------
// constructor
mgTextBuffer::mgTextBuffer()
{
  m_buffer = NULL;
  m_bufferSize = 0;
  m_bufferLen = 0;

  m_justify = mgTextAlignHDefault;
  m_leftMargin = MGDEFAULTSHORT;
  m_rightMargin = MGDEFAULTSHORT;
  m_indent = MGDEFAULTSHORT;

  m_color = MGDEFAULTCOLOR;
  m_anchor = NULL;
  m_wrap = mgDefaultBoolean;
  m_wasBlank = false;

  m_fontFace = "default";
  m_fontSize = MGDEFAULTSHORT;
  m_fontItalic = mgDefaultBoolean;
  m_fontBold = mgDefaultBoolean;
}

//-----------------------------------------------------------------
// destructor
mgTextBuffer::~mgTextBuffer()
{
  // delete all the child boxes
  unsigned int posn = 0;
  while (posn < m_bufferLen) 
  {
    mgFormatCmd cmd = (mgFormatCmd) m_buffer[posn++];
    switch (cmd)
    {
      case mgJustifyCmd: 
      case mgLeftMarginCmd: 
      case mgRightMarginCmd: 
      case mgIndentCmd:
      case mgSpaceCmd: 
      case mgTabCmd: 
      case mgWrapCmd: 
      case mgBreakCmd: 
      case mgClearCmd: 
      case mgFontSizeCmd:
      case mgFontBoldCmd:
      case mgFontItalicCmd:
      case mgDoneCmd:
        readShort(posn);
        break;

      case mgColorCmd: 
      case mgAnchorCmd: 
      case mgTargetCmd: 
        readDWORD(posn);
        break;
    
      case mgFontFaceCmd: 
      case mgTextCmd:
      {
        int textLen;
        const char* text;
        readText(posn, textLen, text);
        break;
      }

      case mgChildCmd:
      {
        // read child box from buffer
        const void* child;
        mgTextAlign halign;
        mgTextAlign valign;
        readChild(posn, child, halign, valign);
        break;
      } 
    } 
  } 

  delete m_buffer;
}

//-----------------------------------------------------------------
// create box for child and add it
void mgTextBuffer::writeChild(
  const void* child,                   // child object
  mgTextAlign horzAlign,               // horizontal alignment
  mgTextAlign vertAlign)               // vertical alignment
{
  // add ChildCmd (byte), child (ptr) to buffer
  checkBufferSize(1+sizeof(void*) + 2*sizeof(short));
  m_buffer[m_bufferLen++] = (BYTE) mgChildCmd;
  
  *(const void**) (m_buffer+m_bufferLen) = child;
  m_bufferLen += sizeof(void*);
  *(short* ) (m_buffer+m_bufferLen) = (short) horzAlign;
  m_bufferLen += sizeof(short);
  *(short* ) (m_buffer+m_bufferLen) = (short) vertAlign;
  m_bufferLen += sizeof(short);
}

//-----------------------------------------------------------------
// write text in the current font
void mgTextBuffer::writeText(
  const char* text,
  int len)
{
  if (len == -1)
    len = (int) strlen(text);
  if (len == 0)
    return;  // nothing to do

  // add TextCmd (byte), text length (short) and text to buffer
  checkBufferSize(1+sizeof(WORD)+len);

  m_buffer[m_bufferLen++] = (BYTE) mgTextCmd;

  // fill in length after we remove duplicate blanks
  unsigned lenPosn = m_bufferLen;
  m_bufferLen += sizeof(WORD);
  int outLen = 0;

  // if we're not wrapping, copy text as-is
  if (m_wrap == mgFalse)
  {
    memcpy(m_buffer+m_bufferLen, text, len);
    m_bufferLen += len;
    outLen = len;
  }
  else
  {
    while (len > 0)
    {
      int c = (int) (0xFF & *text++);
      len--;

      if (isspace(c))
      {
        // remove duplicate blanks if wrap
        if (!m_wasBlank)
        {
          m_buffer[m_bufferLen++] = ' ';
          outLen++;
        }
        m_wasBlank = true;
      }
      else 
      {
        m_wasBlank = false;
        m_buffer[m_bufferLen++] = c;
        outLen++;
      }
    }
  }

  // if nothing written, remove text command
  if (outLen == 0)
  {
    m_bufferLen -= 1+sizeof(WORD);
    return;
  }

  // write length
  *(WORD*) (m_buffer+lenPosn) = (WORD) outLen;
}

//-----------------------------------------------------------------
// write set justification to buffer
void mgTextBuffer::writeJustify(
  mgTextAlign justify)
{
  if (m_justify == justify)
    return;
  m_justify = justify;
  writeShort(mgJustifyCmd, justify);
}

//-----------------------------------------------------------------
// write set left margin to buffer
void mgTextBuffer::writeLeftMargin(
  short margin)                   // margin, -1 for current position
{
  if (m_leftMargin == margin)
    return;
  m_leftMargin = margin;
  writeShort(mgLeftMarginCmd, margin);
}

//-----------------------------------------------------------------
// write set right margin to buffer
void mgTextBuffer::writeRightMargin(
  short margin)
{                     
  if (m_rightMargin == margin)
    return;
  m_rightMargin = margin;
  writeShort(mgRightMarginCmd, margin);
}

//-----------------------------------------------------------------
// write set indent to buffer
void mgTextBuffer::writeIndent(
  short margin)
{
  if (m_indent == margin)
    return;
  m_indent = margin;
  writeShort(mgIndentCmd, margin);
}

//-----------------------------------------------------------------
// set font face
void mgTextBuffer::writeFontFace(
  const char* face)
{ 
  if (m_fontFace.equalsIgnoreCase(face))
    return;
  m_fontFace = face;

  int len = m_fontFace.length();
  checkBufferSize(1+sizeof(WORD)+len);

  m_buffer[m_bufferLen++] = (BYTE) mgFontFaceCmd;
  *(WORD*) (m_buffer+m_bufferLen) = len;
  m_bufferLen += sizeof(WORD);
  memcpy(m_buffer+m_bufferLen, (const char*) m_fontFace, len);
  m_bufferLen += len;
}

//-----------------------------------------------------------------
// set font size
void mgTextBuffer::writeFontSize(
  short size)                    // pointsize
{
  if (m_fontSize == size)
    return;
  m_fontSize = size;
  writeShort(mgFontSizeCmd, m_fontSize);
}

//-----------------------------------------------------------------
// set font italic
void mgTextBuffer::writeFontItalic(
  mgBooleanAttr italic)
{ 
  if (m_fontItalic == italic)
    return;
  m_fontItalic = italic;
  writeShort(mgFontItalicCmd, (short) m_fontItalic);
}

//-----------------------------------------------------------------
// set font bold
void mgTextBuffer::writeFontBold(
  mgBooleanAttr bold)
{
  if (m_fontBold == bold)
    return;
  m_fontBold = bold;
  writeShort(mgFontBoldCmd, (short) m_fontBold);
}

//-----------------------------------------------------------------
// get font string
void mgTextBuffer::getFont(
  mgString& fontSpec)
{
  fontSpec.format("%s-%d%s%s", (const char*) m_fontFace, m_fontSize, 
    m_fontBold == mgTrue? "-b" : "", m_fontItalic == mgTrue ? "-i" : "");
}

//-----------------------------------------------------------------
// set Anchor value
void mgTextBuffer::writeAnchor(
  const void* anchor)
{
  m_anchor = anchor;
  writePtr(mgAnchorCmd, (void*) anchor);
}

//-----------------------------------------------------------------
// set text color
void mgTextBuffer::writeTextColor(
  DWORD color)
{
  if (m_color == color)
    return;
  m_color = color;
  writeDWORD(mgColorCmd, color);
}

//-----------------------------------------------------------------
// set word wrap off or on
void mgTextBuffer::writeWrap(
  mgBooleanAttr wrap)
{
  m_wrap = wrap;
  writeShort(mgWrapCmd, (short) wrap);
}

//-----------------------------------------------------------------
// move to position on line
void mgTextBuffer::writeTab(
  short value)
{ 
  writeShort(mgTabCmd, value); 
}

//-----------------------------------------------------------------
// write blank of indicated width
void mgTextBuffer::writeSpace(
  short value)
{ 
  writeShort(mgSpaceCmd, value); 
}

//-----------------------------------------------------------------
// line break
void mgTextBuffer::writeBreak(
  short height)                  // height of break in lines*100
{ 
  writeShort(mgBreakCmd, height); 
}

//-----------------------------------------------------------------
// write clear of floating images
void mgTextBuffer::writeClear(
  mgTextAlign clear)                // Left, Right or Center (both)
{ 
  writeShort(mgClearCmd, clear); 
}

//-----------------------------------------------------------------
// write command to record target position
void mgTextBuffer::writeTarget(
  int *pnY)
{ 
  writePtr(mgTargetCmd, pnY); 
}

//-----------------------------------------------------------------
// set end of input
void mgTextBuffer::writeDone()
{ 
  writeShort(mgDoneCmd, 0); 
}

//-----------------------------------------------------------------
// check to see if there's room in buffer
void mgTextBuffer::checkBufferSize(
  int addLen)                  // additional length required
{
  if (m_bufferLen + addLen > m_bufferSize)
    makeRoom(addLen);
}

//-----------------------------------------------------------------
// make room in buffer
void mgTextBuffer::makeRoom(
  int addLen)                  // additional length required
{
  // allocate more space (add extra to cut down on allocates)
  m_bufferSize = m_bufferLen + addLen + BUFFER_PAD_SIZE;

  BYTE* oldBuffer = m_buffer;
  m_buffer = new BYTE[m_bufferSize];
  memcpy(m_buffer, oldBuffer, m_bufferLen);

  delete oldBuffer;
}

//-----------------------------------------------------------------
// write a command and short value to buffer
void mgTextBuffer::writeShort(
  mgFormatCmd cmd, 
  short value)
{
  // add command (byte) and value (short) to buffer
  checkBufferSize(1+sizeof(short));

  m_buffer[m_bufferLen++] = (BYTE) cmd;
  *(short* ) (m_buffer+m_bufferLen) = value;

  m_bufferLen += sizeof(short);
}

//-----------------------------------------------------------------
// write a command and long value to buffer
void mgTextBuffer::writeDWORD(
  mgFormatCmd cmd, 
  DWORD value)
{
  // add command (byte) and value (long) to buffer
  checkBufferSize(1+sizeof(DWORD));

  m_buffer[m_bufferLen++] = (BYTE) cmd;
  *(DWORD*) (m_buffer+m_bufferLen) = value;

  m_bufferLen += sizeof(DWORD);
}

//-----------------------------------------------------------------
// write a command and ptr to buffer
void mgTextBuffer::writePtr(
  mgFormatCmd cmd, 
  void* value)
{
  // add command (byte) and value (ptr) to buffer
  checkBufferSize(1+sizeof(void*));

  m_buffer[m_bufferLen++] = (BYTE) cmd;
  *(void**) (m_buffer+m_bufferLen) = value;

  m_bufferLen += sizeof(void*);
}

//-----------------------------------------------------------------
// write a string to buffer
void mgTextBuffer::writeString(
  const char* value,
  int len)
{
  if (len == -1)
    len = strlen(value);

  // add length plus string bytes
  checkBufferSize(1+sizeof(short) + len);

  *(short*) (m_buffer+m_bufferLen) = (short) len;
  memcpy(m_buffer+m_bufferLen, value, len);

  m_bufferLen += sizeof(short)+ len;
}

//-----------------------------------------------------------------
// read short value from buffer
short mgTextBuffer::readShort(
  unsigned int &posn)          // buffer position
{                       
  short value = *(short*) (m_buffer + posn);
  posn += sizeof(short);

  return value;
}

//-----------------------------------------------------------------
// read long value from buffer
DWORD mgTextBuffer::readDWORD(
  unsigned int &posn)          // buffer position
{
  DWORD lValue = *(DWORD*) (m_buffer + posn);
  posn += sizeof(DWORD);

  return lValue;
}

//-----------------------------------------------------------------
// read pointer from buffer
void* mgTextBuffer::readPtr(
  unsigned int &posn)          // buffer position
{
  void* value = *(void**) (m_buffer + posn);
  posn += sizeof(void*);

  return value;
}

//-----------------------------------------------------------------
// read text from buffer
void mgTextBuffer::readText(
  unsigned int &posn,                   // buffer position
  int &len,                    // length of text
  const char* &text)          // pointer to text data
{
  len = *(WORD*) (m_buffer + posn);
  posn += sizeof(WORD);
  text = (const char*) (m_buffer + posn);
  posn += len;
}

//-----------------------------------------------------------------
// read child box
void mgTextBuffer::readChild(
  unsigned int& posn,                // position in buffer
  const void*& child,                // child box
  mgTextAlign& horzAlign,            // horizontal alignment
  mgTextAlign& vertAlign)            // vertical alignment
{
  child = *(const void**) (m_buffer + posn);
  posn += sizeof(void*);
  horzAlign = (mgTextAlign) *(short*) (m_buffer + posn);
  posn += sizeof(short);
  vertAlign = (mgTextAlign) *(short*) (m_buffer + posn);
  posn += sizeof(short);
}

