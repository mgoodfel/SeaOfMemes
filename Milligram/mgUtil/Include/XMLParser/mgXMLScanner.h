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
#ifndef MGXMLSCANNER_H
#define MGXMLSCANNER_H

/*
  Tokenize an XML input stream.
*/
class mgXMLScanner
{
public:
  mgString m_sourceFileName;            // use to resolve relative paths
  int m_lineNum;                      // current line
  int m_colNum;                       // current column

  // constructor
  mgXMLScanner();

  // destructor
  virtual ~mgXMLScanner();

  // reset parser state
  virtual void reset();

  // parse a char buffer
  virtual void parse(
    int len,
    const char* buffer);
  
  // end parsing buffer
  virtual void parseEnd();

  // parse a file
  virtual void parseFile(
    const char* fileName);

  // throw an error exception with source and line numbers
  virtual void errorMsg(
    const char* msgId,
    const char* varNames,
    const char* format,
    ...) const;

  // throw an error exception with source and line numbers
  virtual void exception(
    const char* format,
    ...) const;

protected:
  char m_lastChar;              // pushed-back character
  int m_state;                  // parser state

  mgString m_token;             // token in progress
  mgString m_entityRef;         // entity reference in progress
  char m_valueDelim;            // which quote type on value

  // handle a processing instruction
  virtual void processingInstruction(
    const char* text);

  // handle a comment
  virtual void comment(
    const char* text);

  // handle CDATA content
  virtual void CDATAContent(
    const char* text,
    int len);

  // handle ordinary content
  virtual void content(
    const char* text,
    int len);

  // handle tag open
  virtual void tagOpen(
    const char* text);

  // handle no-content tag
  virtual void tagNoContent();

  // handle tag close
  virtual void tagClose(
    const char* text);

  // handle attribute name
  virtual void attrName(
    const char* text);

  // handle attribute value
  virtual void attrValue(
    const char* text);

  // end of attributes
  virtual void endAttrs();

  // handle entity references (ampersand)
  virtual void entityRef(
    const char* symbol,
    mgString& value);
};

#endif
