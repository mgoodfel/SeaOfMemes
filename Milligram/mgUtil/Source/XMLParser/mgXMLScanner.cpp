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

#include "XMLParser/mgXMLScanner.h"

//--------------------------------------------------------------
// constructor
mgXMLScanner::mgXMLScanner()
{
  reset();
}

//--------------------------------------------------------------
// destructor
mgXMLScanner::~mgXMLScanner()
{
}

#define PARSE_BUFFER_SIZE             4096
#define CONTENT_CHUNK_MAX             1024

#define START_STATE                   0
#define OPEN_STATE                    1
#define PROCESS_STATE                 2
#define PROCESS_END_STATE             3
#define TAGEXP_STATE                  4
#define TAGEXP_DASH_STATE             5
#define COMMENT_STATE                 6
#define COMMENT_END_STATE             7
#define COMMENT_END2_STATE            8
#define CDATA_OPEN_STATE              9
#define CDATA_OPEN2_STATE             10
#define CDATA_OPEN3_STATE             11
#define CDATA_OPEN4_STATE             12
#define CDATA_OPEN5_STATE             13
#define CDATA_OPEN6_STATE             14
#define CDATA_STATE                   15
#define CDATA_END_STATE               16
#define CDATA_END2_STATE              17
#define CDATA_END3_STATE              18

#define DOCTYPE_OPEN_STATE            19
#define DOCTYPE_OPEN2_STATE           20
#define DOCTYPE_OPEN3_STATE           21
#define DOCTYPE_OPEN4_STATE           22
#define DOCTYPE_OPEN5_STATE           23
#define DOCTYPE_OPEN6_STATE           24
#define DOCTYPE_STATE                 25

#define TAGOPEN_STATE                 26
#define TAGCLOSE_STATE                27
#define TAGCLOSE_BLANK_STATE          28
#define ATTR_START_STATE              29
#define ATTR_NAME_STATE               30
#define EQUAL_START_STATE             31 
#define VALUE_START_STATE             33
#define VALUE_STATE                   34

#define VALUE_AMPER_STATE             35
#define AMPER_STATE                   36

//--------------------------------------------------------------
// reset parser state
void mgXMLScanner::reset()
{
  m_lineNum = 1;
  m_colNum = 0;
  m_state = START_STATE;
  m_lastChar = '\0';
}

//--------------------------------------------------------------
// end of input
void mgXMLScanner::parseEnd()
{
  if (m_state != START_STATE)
    errorMsg("xmlEnds", "", "");

  if (m_token.length() > 0)
  {
    content(m_token, m_token.length());
    m_token.empty();
  }
}

//--------------------------------------------------------------
// parse a char buffer.  Characters are assumed UTF-8, but this
// is passed through to content and attributes.  All we care
// about is xml characters (<, >, =", etc.)
void mgXMLScanner::parse(
  int len,
  const char* buffer)
{
  int posn = 0;
  while (true)
  {
    // read a char, possibly pushed back
    char c;
    if (m_lastChar != '\0')
    {
      c = m_lastChar;
      m_lastChar = '\0';
    }
    else
    {
      // if buffer empty
      if (posn >= len)
        break;
      c = buffer[posn++];

      // track line and col nums for error messages
      if (c == '\n')
      {
        m_lineNum++;
        m_colNum = 0;
      }
      else if (c != '\r')
        m_colNum++;
    }

    // process against state machine
    switch (m_state)
    {
      case START_STATE:
        if (c == '<')
        {
          if (m_token.length() > 0)
          {
            content(m_token, m_token.length());
            m_token.empty();
          }
          m_state = OPEN_STATE;
        }
        else if (c == '&')
        {
          if (m_token.length() > 0)
          {
            content(m_token, m_token.length());
            m_token.empty();
          }
          m_state = AMPER_STATE;
        }
        else 
        {
          if (m_token.length() > CONTENT_CHUNK_MAX)
          {
            content(m_token, m_token.length());
            m_token.empty();
          }
          m_token += c;
        }
        break;

      case OPEN_STATE:     // '<' seen
        if (c == '?')
          m_state = PROCESS_STATE;
        else if (c == '!')
          m_state = TAGEXP_STATE;
        else if (c == '/')  
          m_state = TAGCLOSE_STATE;
        else 
        {
          m_state = TAGOPEN_STATE;
          m_lastChar = c;
        }
        break;

      case PROCESS_STATE:   // <? seen
        if (c == '?')
          m_state = PROCESS_END_STATE;
        else m_token += c;
        break;
      
      case PROCESS_END_STATE:  // <? ... ? seen
        if (c == '>')
        {
          processingInstruction(m_token);
          m_token.empty();
          m_state = START_STATE;
        }
        else 
        {
          m_token += '?';
          m_lastChar = c;
          m_state = PROCESS_STATE;
        }
        break;

      case TAGEXP_STATE:    // <! seen
        if (c == '-')
          m_state = TAGEXP_DASH_STATE;
        else if (c == '[')
          m_state = CDATA_OPEN_STATE;
        else if (c == 'D')
          m_state = DOCTYPE_OPEN_STATE;
        else errorMsg("xmlBadString", "string", "<!%c", c);
        break;

      case TAGEXP_DASH_STATE:  // <!- seen 
        if (c == '-')
          m_state = COMMENT_STATE;
        else errorMsg("xmlBadString", "string", "<!-%c", c);
        break;

      case COMMENT_STATE:   // <!-- seen
        if (c == '-')
          m_state = COMMENT_END_STATE;
        else m_token += c;
        break;
      
      case COMMENT_END_STATE: // <!-- ... - seen
        if (c == '-')
          m_state = COMMENT_END2_STATE;
        else 
        {
          m_token += '-';
          m_lastChar = c;
          m_state = COMMENT_STATE;
        }
        break;

      case COMMENT_END2_STATE:  // <!-- ... -- seen
        if (c == '>')
        {
          comment(m_token);
          m_token.empty();
          m_state = START_STATE;
        }
        else 
        {
          m_token += "--";
          m_lastChar = c;
          m_state = COMMENT_STATE;
        }
        break;

      case CDATA_OPEN_STATE:   // <![ seen
        if (c == 'C')
          m_state = CDATA_OPEN2_STATE;
        else errorMsg("xmlBadString", "string", "<![%c", c);
        break;

      case CDATA_OPEN2_STATE: // <![C seen
        if (c == 'D')
          m_state = CDATA_OPEN3_STATE;
        else errorMsg("xmlBadString", "string", "<![C%c", c);
        break;

      case CDATA_OPEN3_STATE: // <![CD seen
        if (c == 'A')
          m_state = CDATA_OPEN4_STATE;
        else errorMsg("xmlBadString", "string", "<![CD%c", c);
        break;

      case CDATA_OPEN4_STATE: // <![CDA seen
        if (c == 'T')
          m_state = CDATA_OPEN5_STATE;
        else errorMsg("xmlBadString", "string", "<![CDA%c", c);
        break;

      case CDATA_OPEN5_STATE: // <![CDAT seen
        if (c == 'A')
          m_state = CDATA_OPEN6_STATE;
        else errorMsg("xmlBadString", "string", "<![CDAT%c", c);
        break;

      case CDATA_OPEN6_STATE: // <![CDATA seen
        if (c == '[')
          m_state = CDATA_STATE;
        else errorMsg("xmlBadString", "string", "<![CDATA%c", c);
        break;

      case CDATA_STATE:  // <![CDATA[ ... seen
        if (c == ']')
          m_state = CDATA_END_STATE;
        else 
        {
          // break this into chunks
          if (m_token.length() > CONTENT_CHUNK_MAX)
          {
            CDATAContent(m_token, m_token.length());
            m_token.empty();
          }
          m_token += c;
        }
        break;

      case CDATA_END_STATE:  // <![CDATA[ ... ] seen
        if (c == ']')
          m_state = CDATA_END2_STATE;
        else
        {
          m_state = CDATA_STATE;
          m_token += ']';
          m_lastChar = c;
        }
        break;

      case CDATA_END2_STATE:  // <![CDATA[ ... ]] seen
        if (c == '>')
        {
          CDATAContent(m_token, m_token.length());
          m_token.empty();
          m_state = START_STATE;
        }
        else
        {
          m_state = CDATA_STATE;
          m_token += "]]";
          m_lastChar = c;
        }
        break;

      case DOCTYPE_OPEN_STATE: // <!D seen
        if (c == 'O')
          m_state = DOCTYPE_OPEN2_STATE;
        else errorMsg("xmlBadString", "string", "<![%c", c);
        break;

      case DOCTYPE_OPEN2_STATE: // <!DO seen
        if (c == 'C')
          m_state = DOCTYPE_OPEN3_STATE;
        else errorMsg("xmlBadString", "string", "<![C%c", c);
        break;

      case DOCTYPE_OPEN3_STATE: // <!DOC seen
        if (c == 'T')
          m_state = DOCTYPE_OPEN4_STATE;
        else errorMsg("xmlBadString", "string", "<![C%c", c);
        break;

      case DOCTYPE_OPEN4_STATE: // <!DOCT seen
        if (c == 'Y')
          m_state = DOCTYPE_OPEN5_STATE;
        else errorMsg("xmlBadString", "string", "<![C%c", c);
        break;

      case DOCTYPE_OPEN5_STATE: // <!DOCTY seen
        if (c == 'P')
          m_state = DOCTYPE_OPEN6_STATE;
        else errorMsg("xmlBadString", "string", "<![C%c", c);
        break;

      case DOCTYPE_OPEN6_STATE: // <!DOCTYP seen
        if (c == 'E')
          m_state = DOCTYPE_STATE;
        else errorMsg("xmlBadString", "string", "<![C%c", c);
        break;

      case DOCTYPE_STATE:  // <!DOCTYPE ... seen
        if (c == '>')
          m_state = START_STATE;
        break;


      case TAGOPEN_STATE:   // <x... seen
        if (c == '>')
        {
          tagOpen(m_token);
          endAttrs();
          m_token.empty();
          m_state = START_STATE;
        }
        else if (c == '/')
        {
          tagOpen(m_token);
          m_token.empty();
          endAttrs();
          tagNoContent();
          m_state = TAGCLOSE_BLANK_STATE;
        }
        else if (isspace(c))
        {
          tagOpen(m_token);
          m_token.empty();
          m_state = ATTR_START_STATE;
        }
        else m_token += c;
        break;

      case TAGCLOSE_STATE:  // </ seen
        if (c == '>')
        {
          tagClose(m_token);
          m_token.empty();
          m_state = START_STATE;
        }
        else if (isspace(c))
        {
          tagClose(m_token);
          m_token.empty();
          m_state = TAGCLOSE_BLANK_STATE;
        }
        else m_token += c;
        break;

      case TAGCLOSE_BLANK_STATE:  // </tag ' ' seen
        if (c == '>')
          m_state = START_STATE;

        else if (!isspace(c))
          errorMsg("xmlBadClose", "", "");
        break;

      case ATTR_START_STATE: // <tag ' ' seen, or attr=value seen
        if (c == '>')
        {
          if (m_token.length() > 0)
          {
            tagOpen(m_token);
            m_token.empty();
          }
          endAttrs();
          m_state = START_STATE;
        }
        else if (c == '/')
        {
          if (m_token.length() > 0)
          {
            tagOpen(m_token);
            m_token.empty();
          }
          endAttrs();
          tagNoContent();
          m_state = TAGCLOSE_BLANK_STATE;
        }
        else if (!isspace(c))
        {
          m_state = ATTR_NAME_STATE;
          m_lastChar = c;
        }
        break;

      case ATTR_NAME_STATE: // <tag ... 'letter' a seen
        if (iswalnum(c) || c == '_' || c == '.' || c == '-')
          m_token += c;
        else if (c == '=')
        {
          attrName(m_token);
          m_token.empty();
          m_state = VALUE_START_STATE;
        }
        else if (isspace(c))
        {
          attrName(m_token);
          m_token.empty();
          m_state = EQUAL_START_STATE;
        }

        else errorMsg("xmlMissingEquals", "", "");
        break;

      case EQUAL_START_STATE:
        if (c == '=')
          m_state = VALUE_START_STATE;
        else if (!isspace(c))
          errorMsg("xmlMissingEquals", "", "");
        break;

      case VALUE_START_STATE:
        if (c == '"')
        {
          m_state = VALUE_STATE;
          m_valueDelim = '"';
        }
        else if (c == '\'')
        {
          m_state = VALUE_STATE;
          m_valueDelim = '\'';
        }
        else if (!isspace(c))
          errorMsg("xmlMissingQuote", "", "");
        break;

      case VALUE_STATE:
        if (c == m_valueDelim)
        {
          attrValue(m_token);
          m_token.empty();
          m_state = ATTR_START_STATE;
        }
        else if (c == '&')
          m_state = VALUE_AMPER_STATE;
        else m_token += c;
        break;

      case VALUE_AMPER_STATE:  // & seen in attribute value
        if (c == ';')
        {
          mgString value;
          entityRef(m_entityRef, value);
          m_token += value;
          m_entityRef.empty();
          m_state = VALUE_STATE;
        }
        else if (isspace(c))
          errorMsg("xmlMissingSemi", "", "");

        else m_entityRef += c;
        break;

      case AMPER_STATE: // & seen in content
        if (c == ';')
        {
          mgString value;
          entityRef(m_entityRef, value);
          m_token += value;
          m_entityRef.empty();
          m_state = START_STATE;
        }
        else if (isspace(c))
          errorMsg("xmlMissingSemi", "", "");

        else m_entityRef += c;
        break;
    }
  }
}

//--------------------------------------------------------------
// throw an error exception with source and line numbers
void mgXMLScanner::exception(
  const char* format,
  ...) const
{
  va_list args;
  va_start(args, format);

  mgString msg;
  msg.formatV(format, args);

  throw new mgException("%s (%d, %d): %s", 
    (const char*) m_sourceFileName, m_lineNum, m_colNum, (const char*) msg);
}

//--------------------------------------------------------------
// throw an mgErrorMsg, adding source and line numbers
void mgXMLScanner::errorMsg(
  const char* msgId,
  const char* varNames,
  const char* format,
  ...) const
{
  va_list args;
  va_start(args, format);
  mgErrorMsg* msg = new mgErrorMsg(msgId, varNames, format, args);

  msg->addVars("filename,line,col,", "%s,%d,%d", (const char*) m_sourceFileName, m_lineNum, m_colNum);

  throw msg;
}

//--------------------------------------------------------------
// parse a file
void mgXMLScanner::parseFile(
  const char* fileName)
{
  m_sourceFileName = fileName;
  mgOSFixFileName(m_sourceFileName);

  FILE* inFile = mgOSFileOpen(m_sourceFileName, "rb");
  if (inFile == NULL)
    throw new mgErrorMsg("xmlFileNotFound", "filename", "%s", (const char*) m_sourceFileName);

  char* buffer = new char[PARSE_BUFFER_SIZE];
  while (true)
  {
    size_t len = fread(buffer, 1, PARSE_BUFFER_SIZE, inFile);
    if (len <= 0)
      break;

    // parse buffer of file data
    parse((int) len, buffer);
  }
  delete buffer;
  fclose(inFile);

  // end of input
  parseEnd();
}

//--------------------------------------------------------------
// handle entity reference (ampersand)
void mgXMLScanner::entityRef(
  const char* symbol,
  mgString& value)
{
  if (_stricmp(symbol, "amp") == 0)
    value = "&";
  else if (_stricmp(symbol, "lt") == 0)
    value = "<";
  else if (_stricmp(symbol, "gt") == 0)
    value = ">";
  // =-= handle decimal and hex char escapes  
}

//--------------------------------------------------------------
// handle a processing instruction
void mgXMLScanner::processingInstruction(
  const char* text)
{
  // by default, do nothing
//  mgDebug("process: '%s'", (const char*) text);
}

//--------------------------------------------------------------
// handle a comment
void mgXMLScanner::comment(
  const char* text)
{
  // by default, do nothing
//  mgDebug("comment: '%s'", (const char*) text);
}

//--------------------------------------------------------------
// handle CDATA content
void mgXMLScanner::CDATAContent(
  const char* text,
  int len)
{
  // by default, do nothing =-= handle UTF8-chars
  mgString output;
  for (int i = 0; i < len; i++)
  {
    char c = text[i];
    if (c == '\n')
      output += "\\n";
    else output += c;
  }
  mgDebug("CDATA: '%s'", (const char*) output);
}

//--------------------------------------------------------------
// handle ordinary content
void mgXMLScanner::content(
  const char* text,
  int len)
{
  // by default, do nothing =-= how should we handle UTF8 text?
  mgString output;
  for (int i = 0; i < len; i++)
  {
    char c = text[i];
    if (c == '\n')
      output += "\\n";
    else output += c;
  }
  mgDebug("'%s' ", (const char*) output);
}

//--------------------------------------------------------------
// handle start of tag
void mgXMLScanner::tagOpen(
  const char* text)
{
  // by default, do nothing
  mgDebug("<'%s' ", (const char*) text);
}

//--------------------------------------------------------------
// handle tag with no content
void mgXMLScanner::tagNoContent()
{
  // by default, do nothing
  mgDebug("/>");
}

//--------------------------------------------------------------
// handle close tag
void mgXMLScanner::tagClose(
  const char* text)
{
  // by default, do nothing
  mgDebug("</'%s'>", (const char*) text);
}

//--------------------------------------------------------------
// handle attribute name
void mgXMLScanner::attrName(
  const char* text)
{
  mgDebug("attr %s=", (const char*) text);
}

//--------------------------------------------------------------
// handle attribute value
void mgXMLScanner::attrValue(
  const char* text)
{
  mgDebug("value \"%s\"", (const char*) text);
}

//--------------------------------------------------------------
// end of attributes
void mgXMLScanner::endAttrs()
{
}

