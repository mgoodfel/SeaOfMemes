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

#include "GUI/mgToolkit.h"
#include "mgLabelTag.h"

#include "Form/mgFormParser.h"
#include "Form/mgFormPane.h"

//--------------------------------------------------------------
// tag starts
void mgLabelTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;
}

//--------------------------------------------------------------
// attribute set
void mgLabelTag::tagAttr(
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
  else if (_stricmp(attrName, "maxlabel") == 0)
    m_maxLabel = attrValue;
}

//--------------------------------------------------------------
// end of attributes
void mgLabelTag::tagEndAttrs(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // create the table
  mgControl* parent = formParser->topCntl();
  mgLabelControl* label = parent->getStyle()->createLabel(parent, m_name, m_label);
  label->setLabel(m_label);
  if (!m_maxLabel.isEmpty())
    label->setMaxLabel(m_maxLabel);

  if (!m_name.isEmpty())
    formParser->setCntlName(m_name, label);

  // add it to the text
  text->writeChild(label, mgTextAlignHCenter, mgTextAlignVCenter);
}

//--------------------------------------------------------------
// tag ends
void mgLabelTag::tagClose(
  mgXMLParser* parser)
{
}

