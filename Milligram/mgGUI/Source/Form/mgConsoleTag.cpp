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
#include "mgConsoleTag.h"

#include "Form/mgFormParser.h"
#include "Form/mgFormPane.h"

//--------------------------------------------------------------
// tag starts
void mgConsoleTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  m_displayLines = 0;
  m_history = 0;
}

//--------------------------------------------------------------
// attribute set
void mgConsoleTag::tagAttr(
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
  else if (_stricmp(attrName, "displayLines") == 0)
    m_displayLines = getInteger(parser, attrName, attrValue);

  else if (_stricmp(attrName, "history") == 0)
    m_history = getInteger(parser, attrName, attrValue);
}

//--------------------------------------------------------------
// end of attributes
void mgConsoleTag::tagEndAttrs(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // create the console
  mgControl* parent = formParser->topCntl();
  mgConsoleControl* console = parent->getStyle()->createConsole(parent, m_name);
  if (m_displayLines > 0)
    console->setDisplayLines(m_displayLines);
  if (m_history > 0)
    console->setHistoryLines(m_history);
  console->addLine(mgColor("white"), NULL, "Ready!");

  if (!m_name.isEmpty())
    formParser->setCntlName(m_name, console);

  // add it to the text
  text->writeChild(console, mgTextAlignInline, mgTextAlignVFill);
}

//--------------------------------------------------------------
// tag ends
void mgConsoleTag::tagClose(
  mgXMLParser* parser)
{
}

