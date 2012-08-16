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

#include "mgTextTag.h"
#include "Form/mgFormParser.h"

//--------------------------------------------------------------
// tag starts
void mgTextTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  // save current state of font
  m_face = text->getFontFace();
  m_size = text->getFontSize();
  m_bold = text->getFontBold();
  m_italic = text->getFontItalic();
  m_color = text->getTextColor();

  // make font changes based on tag name
  if (_stricmp(m_tagName, "b") == 0)
    text->writeFontBold(mgTrue);

  else if (_stricmp(m_tagName, "i") == 0)
    text->writeFontItalic(mgTrue);

  else if (_stricmp(m_tagName, "tt") == 0)
  {
    text->writeFontItalic(mgFalse);
    text->writeFontBold(mgFalse);
    text->writeFontFace("courier");
  }
  else if (_stricmp(m_tagName, "q") == 0)
  {
    text->writeText("\xE2\x80\x9C", 3);  // left quote U+201C
  }
  // tagName could also be 'font', but attributes do the work there
}

//--------------------------------------------------------------
// attribute set
void mgTextTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  if (_stricmp(attrName, "color") == 0)
  {
    mgColor color(attrValue);
    text->writeTextColor(color.asRGB());
  }
  else if (_stricmp(attrName, "face") == 0)
  {
    mgString face(attrValue);
    face.trim();
    text->writeFontFace(face);
  }
  else if (_stricmp(attrName, "size") == 0)
  {
    int size = getInteger(parser, attrName, attrValue);
    text->writeFontSize(size);
  }
}

//--------------------------------------------------------------
// text content
void mgTextTag::tagContent(
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
void mgTextTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  // no state in tags. just delete it
  delete tag;
}

//--------------------------------------------------------------
// tag ends
void mgTextTag::tagClose(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  if (_stricmp(m_tagName, "q") == 0)
  {
    text->writeText("\xE2\x80\x9D", 3);   // U+201D
  }

  // restore previous font state
  if (_stricmp(m_face, text->getFontFace()) != 0)
    text->writeFontFace(m_face);
        
  if (text->getFontSize() != m_size)
    text->writeFontSize(m_size);

  if (text->getFontItalic() != m_italic)
    text->writeFontItalic(m_italic);

  if (text->getFontBold() != m_bold)
    text->writeFontBold(m_bold);

  if (text->getTextColor() != m_color)
    text->writeTextColor(m_color);
}

