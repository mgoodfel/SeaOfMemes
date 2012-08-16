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
#ifndef MGERRORTABLE_H
#define MGERRORTABLE_H

#include "mgErrorMsg.h"
#include "../XMLParser/mgXMLParser.h"

// holds local language versions of errors and other messages to the user
class mgErrorTable : public mgXMLParser
{
public:
  // constructor
  mgErrorTable();

  // destructor
  virtual ~mgErrorTable();

  // add message or replace existing version
  virtual void addMsg(
    const char* msgId,
    mgPtrArray& frags);           // table now owns fragments

  // substitute vars into message and return
  virtual void msgText(
    mgString& text,               // returned message text
    const char* msgId,           // message to format
    const char* varNames,        // comma-delimited variable names
    const char* format,          // format spec for values
    ...);                         // variable values

  // substitute vars into message and return
  virtual void msgText(
    mgString& text,               // returned message text
    const mgErrorMsg* msg);       // error message to format

protected:
  mgMapStringToDWord m_errors;    // map error id to index in frag table
  mgPtrArray m_fragments;         // error msg fragments

  // parse a name or value off the strings
  int parseVar(
    mgString& value,
    const char* argString,
    int posn);

  // find value of variable
  BOOL findVar(
    mgString& value,
    const char* varNames,
    const char* varValues,
    const char* name);

  // substitute vars into message and return
  virtual void createText(
    mgString& text,               // returned message text
    const char* msgId,
    const char* varNames,
    const char* varValues);

  // format unknown message id
  virtual void unknownMsg(
    mgString& text,               // returned message text
    const char* msgId,
    const char* varNames,
    const char* varValues);

  // create a tag instance
  virtual mgXMLTag* createTag(
    const char* tagName);

  // handle the top tag (no parent to add it to)
  virtual void processTopTag(
    mgXMLTag* tag);
};

class mgErrorTableTag : public mgXMLTag
{
public:
  mgMapStringToDWord m_errors;    // map error id to index in frag table
  mgPtrArray m_fragments;         // error msg fragments

  // constructor
  mgErrorTableTag(
    const char* tagName)
  : mgXMLTag(tagName)
  {}

  // destructor
  virtual ~mgErrorTableTag() 
  {}

  // add child tag.  parent tag now owns child pointer.
  virtual void tagChild(
    mgXMLParser* parser,
    mgXMLTag* tag);
};

// error msg tag
class mgErrorMsgTag : public mgXMLTag
{
public:
  mgString m_msgId;
  mgPtrArray m_fragments;

  // constructor
  mgErrorMsgTag(
    const char* tagName)
  : mgXMLTag(tagName)
  {}

  // destructor
  virtual ~mgErrorMsgTag() 
  {}

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);

  // text content
  virtual void tagContent(
    mgXMLParser* parser,
    const char* text,
    int len);

  // add child tag.  parent tag now owns child pointer.
  virtual void tagChild(
    mgXMLParser* parser,
    mgXMLTag* tag);
};

// msg var tag
class mgErrorVarTag : public mgXMLTag
{
public:
  mgString m_varName;
  int m_varType;

  // constructor
  mgErrorVarTag(
    const char* tagName)
  : mgXMLTag(tagName)
  {}

  // destructor
  virtual ~mgErrorVarTag() 
  {}

  // tag starts
  virtual void tagOpen(
    mgXMLParser* parser);

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);
};

#endif
