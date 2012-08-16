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

#include "mgCursorDefn.h"

//--------------------------------------------------------------
// constructor
mgCursorDefn::mgCursorDefn(
  const char* fileName)
{
  m_hotX = 0;
  m_hotY = 0;

  parseFile(fileName);
}

//--------------------------------------------------------------
// destructor
mgCursorDefn::~mgCursorDefn()
{
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* mgCursorDefn::createTag(
  const char* tagName)
{
  if (_stricmp(tagName, "cursorDefn") == 0)
    return new mgCursorDefnTag(tagName);
  else return NULL;
}

//--------------------------------------------------------------
// handle the top tag (no parent to add it to)
void mgCursorDefn::processTopTag(
  mgXMLTag* tag)
{
  // we don't need the top tag anymore
  delete tag;
}

//--------------------------------------------------------------
// constructor
mgCursorDefnTag::mgCursorDefnTag(
  const char* name)
: mgXMLTag(name)
{
}

//--------------------------------------------------------------
// attribute set
void mgCursorDefnTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  mgCursorDefn* options = (mgCursorDefn*) parser;

  if (_stricmp(attrName, "texture") == 0)
    getFileName(parser, attrName, attrValue, options->m_texture);

  else if (_stricmp(attrName, "hotX") == 0)
    options->m_hotX = getInteger(parser, attrName, attrValue);

  else if (_stricmp(attrName, "hotY") == 0)
    options->m_hotY = getInteger(parser, attrName, attrValue);
}
