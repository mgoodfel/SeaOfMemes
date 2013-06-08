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

#include "mgPreTag.h"
#include "Form/mgFormParser.h"

//--------------------------------------------------------------
// tag starts
void mgPreTag::tagOpen(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  m_justify = text->getJustify();
  m_wrap = text->getWrap();
  m_face = text->getFontFace();

  text->writeBreak(100);
  text->writeFontFace("Courier");
  text->writeWrap(mgFalse);

  m_seenBreak = false;
}

//--------------------------------------------------------------
// text content
void mgPreTag::tagContent(
  mgXMLParser* parser,
  const char* string,
  int len)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  int start = 0;
  int end = -1;
  for (int posn = 0; posn < len; posn++)
  {
    if (string[posn] == '\n')
    {
      if (end >= start)
        text->writeText(string+start, 1+end-start);
      text->writeBreak(m_seenBreak ? 100 : 0);
      m_seenBreak = true;
      start = posn+1;
    }
    else if (string[posn] != '\r')
    {
      m_seenBreak = false;
      end = posn;
    }
  }
  if (end >= start)
    text->writeText(string+start, 1+end-start);
}

//--------------------------------------------------------------
// add child tag.  parent tag now owns child pointer.
void mgPreTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  // no state in tags. just delete it
  delete tag;
}

//--------------------------------------------------------------
// tag ends
void mgPreTag::tagClose(
  mgXMLParser* parser)
{
  mgFormParser* formParser = (mgFormParser*) parser;
  mgTextBuffer* text = formParser->topText();
  if (text == NULL)
    return;

  text->writeBreak(100);
  text->writeJustify(m_justify);
  text->writeWrap(m_wrap);
  text->writeFontFace(m_face);
}

