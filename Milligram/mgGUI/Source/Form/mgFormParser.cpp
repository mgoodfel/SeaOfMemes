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
#include "Form/mgFormPane.h"
#include "Form/mgFormParser.h"

#include "mgFormTag.h"
#include "mgTextTag.h"
#include "mgParaTag.h"
#include "mgBreakTag.h"
#include "mgPreTag.h"
#include "mgBlockTag.h"
#include "mgRuleTag.h"
#include "mgHeadingTag.h"
#include "mgListTag.h"
#include "mgListItemTag.h"
#include "mgTableTag.h"
#include "mgTableRowTag.h"
#include "mgTableColTag.h"

#include "mgButtonTag.h"
#include "mgCheckboxTag.h"
#include "mgFieldTag.h"
#include "mgLabelTag.h"
#include "mgConsoleTag.h"

//--------------------------------------------------------------
// constructor
mgFormParser::mgFormParser(
  mgFormPane* form)
{
  m_cntlNames = NULL;
  m_form = form;

  pushCntl(m_form);
  pushText(m_form->m_text);
}

//--------------------------------------------------------------
// constructor
mgFormParser::mgFormParser(
  mgFormPane* form,
  const char* fileName)
{
  m_cntlNames = NULL;
  m_form = form;

  pushCntl(m_form);
  pushText(m_form->m_text);
  parseFile(fileName);
}

//--------------------------------------------------------------
// destructor
mgFormParser::~mgFormParser()
{
  delete m_cntlNames;
  m_cntlNames = NULL;
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* mgFormParser::createTag(
  const char* tagName)
{
  if (_stricmp(tagName, "form") == 0)
    return new mgFormTag(tagName);

  if (_stricmp(tagName, "b") == 0)
    return new mgTextTag(tagName);
  if (_stricmp(tagName, "i") == 0)
    return new mgTextTag(tagName);
  if (_stricmp(tagName, "tt") == 0)
    return new mgTextTag(tagName);
  if (_stricmp(tagName, "q") == 0)
    return new mgTextTag(tagName);
  if (_stricmp(tagName, "font") == 0)
    return new mgTextTag(tagName);

  if (_stricmp(tagName, "p") == 0)
    return new mgParaTag(tagName);
  if (_stricmp(tagName, "br") == 0)
    return new mgBreakTag(tagName);
  if (_stricmp(tagName, "pre") == 0)
    return new mgPreTag(tagName);
  if (_stricmp(tagName, "blockquote") == 0)
    return new mgBlockTag(tagName);
  if (_stricmp(tagName, "hr") == 0)
    return new mgRuleTag(tagName);

  if (_stricmp(tagName, "h1") == 0)
    return new mgHeadingTag(tagName);
  if (_stricmp(tagName, "h2") == 0)
    return new mgHeadingTag(tagName);
  if (_stricmp(tagName, "h3") == 0)
    return new mgHeadingTag(tagName);
  if (_stricmp(tagName, "h4") == 0)
    return new mgHeadingTag(tagName);
  if (_stricmp(tagName, "h5") == 0)
    return new mgHeadingTag(tagName);
  if (_stricmp(tagName, "h6") == 0)
    return new mgHeadingTag(tagName);

  if (_stricmp(tagName, "ul") == 0)
    return new mgListTag(tagName);
  if (_stricmp(tagName, "ol") == 0)
    return new mgListTag(tagName);
  if (_stricmp(tagName, "li") == 0)
    return new mgListItemTag(tagName);

  if (_stricmp(tagName, "table") == 0)
    return new mgTableTag(tagName);
  if (_stricmp(tagName, "tr") == 0)
    return new mgTableRowTag(tagName);
  if (_stricmp(tagName, "td") == 0)
    return new mgTableColTag(tagName);
  if (_stricmp(tagName, "th") == 0)
    return new mgTableColTag(tagName);

  if (_stricmp(tagName, "button") == 0)
    return new mgButtonTag(tagName);
  if (_stricmp(tagName, "checkbox") == 0)
    return new mgCheckboxTag(tagName);
  if (_stricmp(tagName, "field") == 0)
    return new mgFieldTag(tagName);
  if (_stricmp(tagName, "label") == 0)
    return new mgLabelTag(tagName);
  if (_stricmp(tagName, "console") == 0)
    return new mgConsoleTag(tagName);

  return NULL;
}

//--------------------------------------------------------------
// handle the top tag (no parent to add it to)
void mgFormParser::processTopTag(
  mgXMLTag* tag)
{
  // we don't need the top tag anymore
  delete tag;
}

//--------------------------------------------------------------
// push new text buffer on stack
void mgFormParser::pushText(
  mgTextBuffer* text)
{
  m_textStack.push(text);
}

//--------------------------------------------------------------
// pop a text buffer off the stack (does not free)
void mgFormParser::popText()
{
  m_textStack.pop();
}

//--------------------------------------------------------------
// return the top text buffer
mgTextBuffer* mgFormParser::topText()
{
  if (m_textStack.length() > 0)
    return (mgTextBuffer*) m_textStack.last();
  else return NULL;
}

//--------------------------------------------------------------
// push a new list on the stack
void mgFormParser::pushList(
  mgListTag* list)
{
  m_listStack.push(list);
}

//--------------------------------------------------------------
// pop a list tag off the stack (does not free)
void mgFormParser::popList()
{
  m_listStack.pop();
}

//--------------------------------------------------------------
// return top list tag
mgListTag* mgFormParser::topList()
{
  if (m_listStack.length() > 0)
    return (mgListTag*) m_listStack.last();
  else return NULL;
}

//--------------------------------------------------------------
// push a new control on the stack
void mgFormParser::pushCntl(
  mgControl* list)
{
  m_listStack.push(list);
}

//--------------------------------------------------------------
// pop a control off the stack (does not free)
void mgFormParser::popCntl()
{
  m_listStack.pop();
}

//--------------------------------------------------------------
// return top control
mgControl* mgFormParser::topCntl()
{
  if (m_listStack.length() > 0)
    return (mgControl*) m_listStack.last();
  else return NULL;
}

//--------------------------------------------------------------
// set a control name
void mgFormParser::setCntlName(
  const char* name,
  mgControl* control)
{
  if (m_cntlNames == NULL)
    m_cntlNames = new mgMapStringToPtr();

  m_cntlNames->setAt(name, control);
}

//--------------------------------------------------------------
// get control by name
mgControl* mgFormParser::getCntl(
  const char* cntlName)
{
  if (m_cntlNames == NULL)
    return NULL;

  const void* value;
  if (!m_cntlNames->lookup(cntlName, value))
    return NULL;

  return (mgControl*) value;
}
