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
#include "mgTableRowTag.h"
#include "Form/mgFormPane.h"
#include "Form/mgFormParser.h"

//--------------------------------------------------------------
// tag starts
void mgTableRowTag::tagOpen(
  mgXMLParser* parser)
{
  mgTableTag* tableTag = (mgTableTag*) parser->findTag("table");
  if (tableTag == NULL)
    parser->exception("<tr> without enclosing <table>");

  m_units = tableTag->m_parentForm->getUnits();
  
  m_rowWeight = -1;
  m_topInset = 0;
  m_botInset = 0;
  m_halign = mgTableAlignLeft;
}

//--------------------------------------------------------------
// attribute set
void mgTableRowTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // default alignment attrs
  mgString value(attrValue);
  value.trim();

  if (_stricmp(attrName, "weight") == 0)
    m_rowWeight = getInteger(parser, attrName, attrValue);

  else if (_stricmp(attrName, "topInset") == 0)
    m_topInset = (m_units * getInteger(parser, attrName, attrValue))/100;

  else if (_stricmp(attrName, "botInset") == 0)
    m_botInset = (m_units * getInteger(parser, attrName, attrValue))/100;

  else if (_stricmp(attrName, "align") == 0)
  {
    if (_stricmp(value, "left") == 0)
      m_halign = mgTableAlignLeft;

    else if (_stricmp(value, "center") == 0)
      m_halign = mgTableAlignHCenter;

    else if (_stricmp(value, "right") == 0)
      m_halign = mgTableAlignRight;

    else if (_stricmp(value, "fill") == 0)
      m_halign = mgTableAlignHFill;
  }

  else if (_stricmp(attrName, "valign") == 0)
  {
    if (_stricmp(value, "top") == 0)
      m_valign = mgTableAlignTop;

    else if (_stricmp(value, "center") == 0)
      m_valign = mgTableAlignVCenter;

    else if (_stricmp(value, "bottom") == 0)
      m_valign = mgTableAlignBottom;

    else if (_stricmp(value, "fill") == 0)
      m_valign = mgTableAlignVFill;
  }
  // =-= frames
}

//--------------------------------------------------------------
// end of attributes
void mgTableRowTag::tagEndAttrs(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTableTag* tableTag = (mgTableTag*) formParser->findTag("table");
  if (tableTag == NULL)
    return;

  mgTableLayout* layout = tableTag->m_layout;
  
  layout->newRow();
  if (m_rowWeight >= 0)
    layout->setRowWeight(m_rowWeight);
  if (m_topInset > 0)
    layout->setRowTopInset(m_topInset);
  if (m_botInset > 0)
    layout->setRowBottomInset(m_botInset);
}

//--------------------------------------------------------------
// text content
void mgTableRowTag::tagContent(
  mgXMLParser* parser,
  const char* string,
  int len)
{
  // ignore content
}

//--------------------------------------------------------------
// add child tag.  parent tag now owns child pointer.
void mgTableRowTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  // no state in tags. just delete it
  delete tag;
}

//--------------------------------------------------------------
// tag ends
void mgTableRowTag::tagClose(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTableTag* tableTag = (mgTableTag*) formParser->findTag("table");
  if (tableTag == NULL)
    return;

  tableTag->m_layout->endRow();
}

