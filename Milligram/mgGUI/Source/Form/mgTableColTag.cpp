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
#include "GenericStyle/mgGenericButton.h"

#include "Form/mgFormPane.h"
#include "mgTableTag.h"
#include "mgTableColTag.h"

#include "Form/mgFormParser.h"
#include "mgTableRowTag.h"

//--------------------------------------------------------------
// tag starts
void mgTableColTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTableRowTag* rowTag = (mgTableRowTag*) formParser->findTag("tr");
  if (rowTag == NULL)
    formParser->exception("<td> without enclosing <tr>");

  m_units = rowTag->m_units;

  // set attribute defaults
  m_colWeight = -1;
  m_rowSpan = 1;
  m_colSpan = 1;
  m_halign = rowTag->m_halign;
  m_valign = rowTag->m_valign;
  m_leftInset = 0;
  m_rightInset = 0;
}

//--------------------------------------------------------------
// attribute set
void mgTableColTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  mgString value(attrValue);
  value.trim();

  if (_stricmp(attrName, "align") == 0)
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

  else if (_stricmp(attrName, "rowspan") == 0)
    m_rowSpan = getInteger(parser, attrName, attrValue);

  else if (_stricmp(attrName, "colspan") == 0)
    m_colSpan = getInteger(parser, attrName, attrValue);

  else if (_stricmp(attrName, "weight") == 0)
    m_colWeight = getInteger(parser, attrName, attrValue);

  else if (_stricmp(attrName, "leftinset") == 0)
    m_leftInset = (m_units * getInteger(parser, attrName, attrValue))/100;

  else if (_stricmp(attrName, "rightinset") == 0)
    m_rightInset = (m_units * getInteger(parser, attrName, attrValue))/100;
}

//--------------------------------------------------------------
// end of attributes
void mgTableColTag::tagEndAttrs(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTableTag* tableTag = (mgTableTag*) formParser->findTag("table");
  if (tableTag == NULL)
    return;

  // get the default formatting
  mgString fontFace;
  tableTag->m_parentForm->getDefaultFontFace(fontFace);
  int fontSize = tableTag->m_parentForm->getDefaultFontSize();
  mgColor textColor;
  tableTag->m_parentForm->getDefaultTextColor(textColor);

  // create the child text pane and initialize formatting
  mgFormPane* pane = new mgFormPane(formParser->topCntl());
  pane->setDefaultFontFace(fontFace);
  pane->setDefaultFontSize(fontSize);
  pane->setDefaultTextColor(textColor);

  // get current colors, which may be just "default"
  mgTextBuffer* parentText = formParser->topText();
  fontFace = parentText->getFontFace();
  fontSize = parentText->getFontSize();
  DWORD color = parentText->getTextColor();

  pane->m_text->writeFontFace(fontFace);
  pane->m_text->writeFontSize(fontSize);
  pane->m_text->writeTextColor(color);

  formParser->pushText(pane->m_text);
  formParser->pushCntl(pane);

  mgTableLayout* layout = tableTag->m_layout;
  layout->addCell(pane, m_halign, m_valign, m_rowSpan, m_colSpan);
  if (m_colWeight >= 0)
    layout->setColWeight(m_colWeight);
  if (m_leftInset > 0)
    layout->setColLeftInset(m_leftInset);
  if (m_rightInset > 0)
    layout->setColRightInset(m_rightInset);
}

//--------------------------------------------------------------
// text content
void mgTableColTag::tagContent(
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
void mgTableColTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  // no state in tags. just delete it
  delete tag;
}

//--------------------------------------------------------------
// tag ends
void mgTableColTag::tagClose(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text != NULL)
    text->writeDone();

  formParser->popText();
  formParser->popCntl();
}

