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

#include "TextFormatter/mgTextBuffer.h"

#include "GUI/mgToolkit.h"
#include "mgTableTag.h"
#include "Form/mgFormParser.h"

//--------------------------------------------------------------
// tag starts
void mgTableTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // table must be within a mgFormPane
  m_parentForm = (mgFormPane*) formParser->topCntl();
  m_parentText = text;
  m_align = mgTextAlignInline;
}

//--------------------------------------------------------------
// attribute set
void mgTableTag::tagAttr(
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
      m_align = mgTextAlignLeft;

    else if (_stricmp(align, "center") == 0)
      m_align = mgTextAlignHCenter;

    else if (_stricmp(align, "right") == 0)
      m_align = mgTextAlignRight;
  }
}

//--------------------------------------------------------------
// end of attributes
void mgTableTag::tagEndAttrs(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // create the table
  mgControl* parent = formParser->topCntl();
  mgControl* pane = new mgControl(parent, "");
  m_layout = new mgTableLayout(pane);

  formParser->pushCntl(pane);

  // add it to the text
  text->writeChild(pane, m_align, mgTextAlignVCenter);
}

//--------------------------------------------------------------
// text content
void mgTableTag::tagContent(
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
void mgTableTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  // no state in tags. just delete it
  delete tag;
}

//--------------------------------------------------------------
// tag ends
void mgTableTag::tagClose(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // =-= for inline table, add break?
  formParser->popCntl();
}

