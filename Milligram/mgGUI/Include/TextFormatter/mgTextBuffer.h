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
#ifndef MGTEXTBUFFER_H
#define MGTEXTBUFFER_H

enum mgTextAlign { 
  mgTextAlignLeft        = 0, 
  mgTextAlignHCenter     = 1, 
  mgTextAlignRight       = 2,
  mgTextAlignHFill       = 3,
  mgTextAlignInline      = 4,
  mgTextAlignHDefault    = 5,
  mgTextAlignTop         = 6, 
  mgTextAlignVCenter     = 7, 
  mgTextAlignBottom      = 8,
  mgTextAlignVFill       = 9,
  mgTextAlignVDefault    = 10
};
enum mgBooleanAttr { mgTrue, mgFalse, mgDefaultBoolean };

#define MGDEFAULTSHORT SHRT_MAX
#define MGDEFAULTCOLOR 0xFF000000

// the buffer contains a string of formatting commands, each introduced by a code
enum mgFormatCmd 
{ 
  mgJustifyCmd, 
  mgLeftMarginCmd, 
  mgRightMarginCmd, 
  mgIndentCmd,
  mgColorCmd, 
  mgTargetCmd, 
  mgAnchorCmd, 
  mgFontFaceCmd,
  mgFontSizeCmd,
  mgFontItalicCmd,
  mgFontBoldCmd,
  mgWrapCmd, 
  mgSpaceCmd, 
  mgTabCmd, 
  mgTextCmd,
  mgChildCmd, 
  mgBreakCmd,
  mgClearCmd,
  mgDoneCmd 
};

// a buffer of formatting commands, held in a mgTextBox
class mgTextBuffer
{
public:
  // constructor
  mgTextBuffer();

  // destructor
  virtual ~mgTextBuffer();

  /*------ write buffer methods -------*/

  // write set justification to buffer
  void writeJustify(
    mgTextAlign justify);

  // return current justification
  mgTextAlign getJustify()
  {
    return m_justify; 
  }

  // write set left margin to buffer
  void writeLeftMargin(
    short margin);                   // margin, -1 for current position

  // return current left margin
  short getLeftMargin()
  { 
    return m_leftMargin; 
  }

  // write set right margin to buffer
  void writeRightMargin(
    short margin);

  // return current right margin
  short getRightMargin()
  { 
    return m_rightMargin; 
  }

  // write set indent to buffer
  void writeIndent(
    short margin);

  // return current indent
  short getIndent()
  { 
    return m_indent; 
  }

  // write set font face to buffer
  void writeFontFace(
    const char* face);

  // return current font face
  const char* getFontFace()
  { 
    return (const char*) m_fontFace; 
  }

  // write set font size to buffer
  void writeFontSize(
    short size);                    // pointsize * 10

  // return current font size
  short getFontSize()
  { 
    return m_fontSize; 
  }

  // write set font italic to buffer
  void writeFontItalic(
    mgBooleanAttr italic);

  // return current font italic
  mgBooleanAttr getFontItalic()
  { 
    return m_fontItalic; 
  }

  // write set font bold to buffer
  void writeFontBold(
    mgBooleanAttr bold);

  // return current font bold
  mgBooleanAttr getFontBold()
  { 
    return m_fontBold; 
  }

  // get current font string
  void getFont(
    mgString& fontSpec);

  // write set text color to buffer
  void writeTextColor(
    DWORD color);

  // return current text color
  DWORD getTextColor()
  { 
    return m_color; 
  }

  // write set Anchor value to buffer
  void writeAnchor(
    const void* anchor);

  // return anchor value
  const void* getAnchor()
  { 
    return m_anchor; 
  }

  // write set word wrap to buffer
  void writeWrap(
    mgBooleanAttr wrap);

  // return current wrap
  mgBooleanAttr getWrap()
  { 
    return m_wrap; 
  }

  // write move to position to buffer
  void writeTab(
    short value);

  // write blank to buffer
  void writeSpace(
    short value);

  // write line break to buffer
  void writeBreak(
    short height);                  // height of break in lines*100

  // write clear of floating images to buffer
  void writeClear(
    mgTextAlign clear);             // Left, Right or Center (both)

  // write command to record target position to buffer
  void writeTarget(
    int *yval);

  // write text in the current font
  void writeText(
    const char *string,             // text to write
    int len = -1);                  // do a strlen if len==-1

  // create a child box and write it
  void writeChild(
    const void* child,              // child object
    mgTextAlign horzAlign,          // horizontal alignment
    mgTextAlign vertAlign);         // vertical alignment

  // set end of input
  void writeDone();

  /*------ read buffer methods -------*/

  // read next command
  mgFormatCmd readCommand(            
    unsigned int& posn)
  {
    if (posn < m_bufferLen)
      return (mgFormatCmd) m_buffer[posn++];
    else return mgDoneCmd;
  }

  // read short-valued commands
  short readFontSize(
    unsigned int& posn)
  {
    return readShort(posn);
  }    

  mgBooleanAttr readFontBold(
    unsigned int& posn)
  {
    return (mgBooleanAttr) readShort(posn);
  }    

  mgBooleanAttr readFontItalic(
    unsigned int& posn)
  {
    return (mgBooleanAttr) readShort(posn);
  }    

  mgTextAlign readJustify(
    unsigned int& posn)
  {
    return (mgTextAlign) readShort(posn);
  }    

  short readLeftMargin(
    unsigned int& posn)
  {
    return readShort(posn);
  }    

  short readRightMargin(
    unsigned int& posn)
  {
    return readShort(posn);
  }    

  short readIndent(
    unsigned int& posn)
  {
    return readShort(posn);
  }    

  short readSpace(
    unsigned int& posn)
  {
    return readShort(posn);
  }    

  short readTab(
    unsigned int& posn)
  {
    return readShort(posn);
  }    

  mgBooleanAttr readWrap(
    unsigned int& posn)
  {
    return (mgBooleanAttr) readShort(posn);
  }    

  short readBreak(
    unsigned int& posn)
  {
    return readShort(posn);
  }    

  mgTextAlign readClear(
    unsigned int& posn)
  {
    return (mgTextAlign) readShort(posn);
  }    

  DWORD readColor(
    unsigned int& posn)
  {
    return readDWORD(posn);
  }    

  void* readAnchor(
    unsigned int& posn)
  {
    return readPtr(posn);
  }    

  void* readTarget(
    unsigned int& posn)
  {
    return readPtr(posn);
  }    

  void readFontFace(
    unsigned int& posn,
    mgString& face)
  {
    int len;
    const char* string;
    readText(posn, len, string);
    face.empty();
    face.write(string, len);
  }

  // read text from buffer
  void readText(
    unsigned int &posn,               // buffer position
    int &len,                         // length of text
    const char* &text);               // pointer to text data

  // read child object
  void readChild(
    unsigned int &posn,               // buffer position
    const void*& child,               // child object
    mgTextAlign& horzAlign,           // horizontal alignment
    mgTextAlign& vertAlign);          // vertical alignment

protected:
  BYTE* m_buffer;                     // content buffer 
  unsigned int m_bufferSize;          // size of buffer allocation
  unsigned int m_bufferLen;           // length used

  // formatting state so we can query it
  mgTextAlign m_justify;              // justification
  short m_leftMargin;                 // left margin
  short m_rightMargin;                // right margin
  short m_indent;                     // indent level

  mgString m_fontFace;                // face of font
  short m_fontSize;                   // size of font (points)
  mgBooleanAttr m_fontItalic;         // true if italic
  mgBooleanAttr m_fontBold;           // true if bold
  mgBooleanAttr m_wrap;               // false if wrap suppressed

  DWORD m_color;                      // text color
  const void* m_anchor;               // anchor value

  mgPtrArray m_anchors;               // array of mgAnchorRect* 
  BOOL m_wasBlank;                    // true if last char blank

  // make room in buffer
  void makeRoom(
    int addLen);                      // additional length required

  // check to see if there's room in buffer
  void checkBufferSize(
    int addLen);                 // additional length required

  // write a command and short value to buffer
  void writeShort(
    mgFormatCmd nCmd, 
    short value);

  // write a command and long value to buffer
  void writeDWORD(
    mgFormatCmd nCmd, 
    DWORD value);

  // write a command and ptr to buffer
  void writePtr(
    mgFormatCmd nCmd, 
    void* value);

  // write a string
  void writeString(
    const char* string,
    int len = -1);

  // read short value from buffer
  short readShort(
    unsigned int &posn);         // buffer position

  // read long value from buffer
  DWORD readDWORD(
    unsigned int &posn);         // buffer position

  // read pointer from buffer
  void* readPtr(
    unsigned int &posn);          // buffer position
};

#endif
