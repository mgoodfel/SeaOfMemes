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

#include "mgFontConfParser.h"

//--------------------------------------------------------------
// constructor
mgFontConfParser::mgFontConfParser()
{
}

//--------------------------------------------------------------
// destructor
mgFontConfParser::~mgFontConfParser()
{
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* mgFontConfParser::createTag(
  const char* tagName)
{
  if (_stricmp(tagName, "dir") == 0)
    return new mgFontDirTag(tagName);
  else return new mgFontConfTag(tagName);
}

//--------------------------------------------------------------
// handle the top tag (no parent to add it to)
void mgFontConfParser::processTopTag(
  mgXMLTag* tag)
{
  // options tag no longer needed
  delete tag;
}

//--------------------------------------------------------------
// constructor
mgFontConfTag::mgFontConfTag(
  const char* name)
: mgXMLTag(name)
{
}

//--------------------------------------------------------------
// attribute set
void mgFontConfTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  // ignore attributes
}

//--------------------------------------------------------------
// add child tag.  parent tag now owns child pointer.
void mgFontConfTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  // we don't need tag instances
  delete tag;
}

//--------------------------------------------------------------
// constructor
mgFontDirTag::mgFontDirTag(
  const char* name)
: mgXMLTag(name)
{
}

//--------------------------------------------------------------
// attribute set
void mgFontDirTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  // ignore attributes
}

//--------------------------------------------------------------
// text content
void mgFontDirTag::tagContent(
  mgXMLParser* parser,
  const char* string,
  int len)
{
  // add the content to the dirname string
  m_dirName.write(string, len);
}

//--------------------------------------------------------------
// tag ends
void mgFontDirTag::tagClose(
  mgXMLParser* parser)
{
  mgFontConfParser* conf = (mgFontConfParser*) parser;

  // add the directory to the table (prevents duplicates)
  m_dirName.trim();
  conf->m_dirList.setAt(m_dirName, 0);
}
