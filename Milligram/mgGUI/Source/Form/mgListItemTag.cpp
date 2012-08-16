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

#include "mgListTag.h"
#include "mgListItemTag.h"
#include "Form/mgFormParser.h"

//--------------------------------------------------------------
// tag starts
void mgListItemTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  mgListTag* list = formParser->topList();
  if (list == NULL)
    formParser->exception("list item tag without encloding list");

  text->writeBreak(0);

  m_leftMargin = text->getLeftMargin();

  if (list->m_tagName.equalsIgnoreCase("ol"))
  {
    mgString num;
    num.format("%d.", list->m_itemNum++);
    text->writeText(num, num.length());
  }
  else text->writeText("\xE2\x80\xA2", 3);   // write bullet U+2022

  text->writeTab(150);
  text->writeLeftMargin(150);
}

//--------------------------------------------------------------
// text content
void mgListItemTag::tagContent(
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
void mgListItemTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  // no state in tags. just delete it
  delete tag;
}

//--------------------------------------------------------------
// tag ends
void mgListItemTag::tagClose(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  text->writeBreak(0);
  text->writeLeftMargin(m_leftMargin);
}

