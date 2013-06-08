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

#include "GUI/mgToolkit.h"
#include "mgFieldTag.h"

#include "Form/mgFormParser.h"
#include "Form/mgFormPane.h"

//--------------------------------------------------------------
// tag starts
void mgFieldTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  m_count = 0;
}

//--------------------------------------------------------------
// attribute set
void mgFieldTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  if (_stricmp(attrName, "name") == 0)
  {
    m_name = attrValue;
    m_name.trim();
  }
  else if (_stricmp(attrName, "length") == 0)
    m_count = getInteger(parser, attrName, attrValue);
}

//--------------------------------------------------------------
// end of attributes
void mgFieldTag::tagEndAttrs(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // create the table
  mgControl* parent = formParser->topCntl();
  mgFieldControl* field = parent->getStyle()->createField(parent, m_name);
  if (m_count > 0)
    field->setDisplayCount(m_count);

  if (!m_name.isEmpty())
    formParser->setCntlName(m_name, field);

  // add it to the text
  text->writeChild(field, mgTextAlignHCenter, mgTextAlignVCenter);
}

//--------------------------------------------------------------
// tag ends
void mgFieldTag::tagClose(
  mgXMLParser* parser)
{
}

