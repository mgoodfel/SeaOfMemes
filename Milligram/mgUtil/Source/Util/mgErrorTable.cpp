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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Util/mgErrorTable.h"

const int MG_ERROR_END    = 0;      // end of msg
const int MG_ERROR_TEXT   = 1;      // text fragment
const int MG_ERROR_STRING = 2;      // string variable
const int MG_ERROR_FLOAT  = 3;      // floating point value
const int MG_ERROR_INT    = 4;      // integer value
const int MG_ERROR_BOOL   = 5;      // boolean value (true/false)
const int MG_ERROR_DATE   = 6;      // date variable

// An error message fragment.  An error message is an array of these
class mgErrorFrag
{
public:
  int m_fragType;
  mgString m_text;
};

//--------------------------------------------------------------
// constructor
mgErrorTable::mgErrorTable()
{
}

//--------------------------------------------------------------
// destructor
mgErrorTable::~mgErrorTable()
{
  // delete all the fragments
  for (int i = 0; i < m_fragments.length(); i++)
  {
    mgErrorFrag* frag = (mgErrorFrag*) m_fragments[i];
    delete frag;
  }
  m_fragments.removeAll();
}

//--------------------------------------------------------------
// substitute vars into message and return
void mgErrorTable::msgText(
  mgString& text,               // returned message text
  const char* msgId,           // message to format
  const char* varNames,        // comma-delimited variable names
  const char* format,          // value format spec
  ...)                          // variable values
{
  va_list args;
  va_start(args, format);

  mgString names(varNames);
  if (!names.endsWith(","))
    names += ",";

  mgString values;
  values.formatV(format, args);
  if (!values.endsWith(","))
    values += ",";

  createText(text, msgId, names, values);
}

//--------------------------------------------------------------
// substitute vars into message and return
void mgErrorTable::msgText(
  mgString& text,               // returned message text
  const mgErrorMsg* msg)
{
  createText(text, msg->m_msgId, msg->m_varNames, msg->m_varValues);
}

//--------------------------------------------------------------
// parse a name or value off the strings
int mgErrorTable::parseVar(
  mgString& value,
  const char* argString,
  int posn)
{
  value.empty();
  while (true)
  {
    char c = argString[posn++];
    if (c == '\0')
      return -1;
    if (c == MG_ERROR_DELIM)
      return posn;

    value += c;
  }
}

//--------------------------------------------------------------
// find value of variable
BOOL mgErrorTable::findVar(
  mgString& value,
  const char* varNames,
  const char* varValues,
  const char* name)
{
  int namePosn = 0;
  int valuePosn = 0;

  mgString argName;
  mgString argValue;
  while (true)
  {
    namePosn = parseVar(argName, varNames, namePosn);
    valuePosn = parseVar(argValue, varValues, valuePosn);
    if (namePosn == -1 || valuePosn == -1)
      return false;

    if (argName.equalsIgnoreCase(name))
    {
      value = argValue;
      return true;
    }
  }
}

//--------------------------------------------------------------
// substitute vars into message and return
void mgErrorTable::unknownMsg(
  mgString& text,               // returned message text
  const char* msgId,
  const char* varNames,
  const char* varValues)
{
  // no error message, so format args
  text.format("Unknown error(%s)", (const char*) msgId);
  int namePosn = 0;
  int valuePosn = 0;

  mgString argName;
  mgString argValue;
  while (true)
  {
    namePosn = parseVar(argName, varNames, namePosn);
    valuePosn = parseVar(argValue, varValues, valuePosn);
    if (namePosn == -1 || valuePosn == -1)
      break;

    text += ", ";
    text += argName;
    text += "=\"";
    text += argValue;
    text += "\"";
  }
}

//--------------------------------------------------------------
// add message or replace existing version
void mgErrorTable::addMsg(
  const char* msgId,
  mgPtrArray& frags)    // table now owns fragments
{
  // add all the fragments to the fragment table
  int index = m_fragments.length();
  for (int i = 0; i < frags.length(); i++)
  {
    m_fragments.add(frags[i]);
  }

  // add the end fragment if not found
  mgErrorFrag* frag = NULL;
  int last = frags.length()-1;
  if (last >= 0)
    frag = (mgErrorFrag*) frags[frags.length()-1];

  if (frag == NULL || frag->m_fragType != MG_ERROR_END)
  {
    frag = new mgErrorFrag;
    frag->m_fragType = MG_ERROR_END;
    m_fragments.add(frag);
  }
  frags.removeAll();

  // map msg id to start position
  mgString key(msgId);
  key.makeLower();
  m_errors.setAt(key, index);
}

//--------------------------------------------------------------
// substitute vars into message and return
void mgErrorTable::createText(
  mgString& text,               // returned message text
  const char* msgId,
  const char* varNames,
  const char* varValues)
{
  // find the error message by id
  mgString key(msgId);
  key.makeLower();
  DWORD index;
  if (!m_errors.lookup(key, index))
  {
    unknownMsg(text, msgId, varNames, varValues);
    return;
  }

  BOOL done = false;
  mgString argValue;
  while (!done)
  {
    mgErrorFrag* frag = (mgErrorFrag*) m_fragments[index++];
    switch (frag->m_fragType)
    {
      case MG_ERROR_END:
        done = true;
        break;

      case MG_ERROR_TEXT:
        text += frag->m_text;
        break;

      case MG_ERROR_STRING:
      {
        if (findVar(argValue, varNames, varValues, frag->m_text))
          text += argValue;
        else 
        {
          // unknown variable
          text += "*";
          text += frag->m_text;
          text += "*";
        }
      }
    }
  }
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* mgErrorTable::createTag(
  const char* tagName)
{
  if (_stricmp(tagName, "errorTable") == 0)
    return new mgErrorTableTag(tagName);
  else if (_stricmp(tagName, "errorMsg") == 0)
    return new mgErrorMsgTag(tagName);
  else if (_stricmp(tagName, "var") == 0)
    return new mgErrorVarTag(tagName);
  else return NULL;
}

//--------------------------------------------------------------
// handle the top tag (no parent to add it to)
void mgErrorTable::processTopTag(
  mgXMLTag* tag)
{
  if (!tag->m_tagName.equalsIgnoreCase("errorTable"))
  {
    // =-= exception -- tag not allowed
    delete tag;
    return;
  }

  mgErrorTableTag* tableTag = (mgErrorTableTag*) tag;

  // copy table entries
  int indexOffset = m_fragments.length();
  int posn = tableTag->m_errors.getStartPosition();
  while (posn != -1)
  {
    mgString msgId;
    DWORD index;
    tableTag->m_errors.getNextAssoc(posn, msgId, index);
    index += indexOffset;

    msgId.makeLower();
    m_errors.setAt(msgId, index);
  }

  // copy fragments
  m_fragments.addAll(tableTag->m_fragments);
  tableTag->m_fragments.removeAll();  // we own frag storage

  // table tag no longer needed
  delete tag;
}

//--------------------------------------------------------------
// add child tag.  parent tag now owns child pointer.
void mgErrorTableTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  if (tag->m_tagName.equalsIgnoreCase("errorMsg"))
  {
    mgErrorMsgTag* msgTag = (mgErrorMsgTag*) tag;

    // add the message
    m_errors.setAt(msgTag->m_msgId, m_fragments.length());
    
    // add the fragments
    m_fragments.addAll(msgTag->m_fragments);
    msgTag->m_fragments.removeAll();  // we own frag storage

    // add end fragment
    mgErrorFrag* frag = new mgErrorFrag;
    frag->m_fragType = MG_ERROR_END;
    m_fragments.add(frag);

    delete tag;
  }
  else mgXMLTag::tagChild(parser, tag);
}

//--------------------------------------------------------------
// attribute set
void mgErrorMsgTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  if (_stricmp(attrName, "id") == 0)
  {
    m_msgId = attrValue;
    m_msgId.trim();
  }
  else mgXMLTag::tagAttr(parser, attrName, attrValue);
}

//--------------------------------------------------------------
// text content
void mgErrorMsgTag::tagContent(
  mgXMLParser* parser,
  const char* text,
  int len)
{
  // add a fragment for text
  mgErrorFrag* frag = new mgErrorFrag;
  frag->m_fragType = MG_ERROR_TEXT;
  frag->m_text.write(text, len);
  m_fragments.add(frag);
}

//--------------------------------------------------------------
// add child tag.  parent tag now owns child pointer.
void mgErrorMsgTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  if (tag->m_tagName.equalsIgnoreCase("var"))
  {
    mgErrorVarTag* varTag = (mgErrorVarTag*) tag;

    // add a fragment for variable
    mgErrorFrag* frag = new mgErrorFrag;
    frag->m_fragType = varTag->m_varType;
    frag->m_text = varTag->m_varName;
    m_fragments.add(frag);

    delete tag;
  }
  else mgXMLTag::tagChild(parser, tag);
}

//--------------------------------------------------------------
// tag starts
void mgErrorVarTag::tagOpen(
  mgXMLParser* parser)
{
  m_varType = MG_ERROR_STRING;
}

//--------------------------------------------------------------
// attribute set
void mgErrorVarTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  if (_stricmp(attrName, "name") == 0)
  {
    m_varName = attrValue;
  }
  else if (_stricmp(attrName, "type") == 0)
  {
    if (_stricmp(attrValue, "string") == 0)
      m_varType = MG_ERROR_STRING;
    else if (_stricmp(attrValue, "boolean") == 0)
      m_varType = MG_ERROR_BOOL;
    else if (_stricmp(attrValue, "integer") == 0)
      m_varType = MG_ERROR_INT;
    else if (_stricmp(attrValue, "float") == 0)
      m_varType = MG_ERROR_FLOAT;
    else if (_stricmp(attrValue, "date") == 0)
      m_varType = MG_ERROR_DATE;
    else parser->errorMsg("tagAttrValue", "attr,value", "%s,%s", (const char*) attrName, (const char*) attrValue);
  }
  else mgXMLTag::tagAttr(parser, attrName, attrValue);
}

