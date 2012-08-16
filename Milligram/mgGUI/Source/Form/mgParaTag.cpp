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

#include "TextFormatter/mgTextBuffer.h"

#include "mgParaTag.h"
#include "Form/mgFormParser.h"

//--------------------------------------------------------------
// tag starts
void mgParaTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  m_justify = text->getJustify();

  text->writeBreak(100);
}

//--------------------------------------------------------------
// attribute set
void mgParaTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  if (_stricmp(attrName, "align") == 0)
  {
    mgString align(attrValue);
    align.trim();
    if (_stricmp(align, "left") == 0)
      text->writeJustify(mgTextAlignLeft);

    else if (_stricmp(align, "center") == 0)
      text->writeJustify(mgTextAlignHCenter);

    else if (_stricmp(align, "right") == 0)
      text->writeJustify(mgTextAlignRight);
  }
}

//--------------------------------------------------------------
// text content
void mgParaTag::tagContent(
  mgXMLParser* parser,
  const char* string,
  int len)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text != NULL)
    text->writeText(string, len);
}

//--------------------------------------------------------------
// add child tag.  parent tag now owns child pointer.
void mgParaTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  // no state in tags. just delete it
  delete tag;
}

//--------------------------------------------------------------
// tag ends
void mgParaTag::tagClose(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  text->writeBreak(100);
  text->writeJustify(m_justify);
}

