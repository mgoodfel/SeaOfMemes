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
#include "mgCheckboxTag.h"

#include "Form/mgFormParser.h"
#include "Form/mgFormPane.h"

//--------------------------------------------------------------
// tag starts
void mgCheckboxTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  m_label = "Checkbox";
}

//--------------------------------------------------------------
// attribute set
void mgCheckboxTag::tagAttr(
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
  else if (_stricmp(attrName, "label") == 0)
    m_label = attrValue;
}

//--------------------------------------------------------------
// end of attributes
void mgCheckboxTag::tagEndAttrs(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // create the table
  mgControl* parent = formParser->topCntl();
  mgCheckboxControl* checkbox = parent->getStyle()->createCheckbox(parent, m_name, m_label);

  if (!m_name.isEmpty())
    formParser->setCntlName(m_name, checkbox);

  // add it to the text
  text->writeChild(checkbox, mgTextAlignInline, mgTextAlignVCenter);
}

//--------------------------------------------------------------
// tag ends
void mgCheckboxTag::tagClose(
  mgXMLParser* parser)
{
}

