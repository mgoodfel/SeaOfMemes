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

#include "XMLParser/mgXMLParser.h"

//--------------------------------------------------------------
// constructor
mgXMLParser::mgXMLParser()
{
  m_topTag = NULL;
}

//--------------------------------------------------------------
// destructor
mgXMLParser::~mgXMLParser()
{
  // delete anything still on the tag stack
  for (int i = 0; i < m_tagStack.length(); i++)
  {
    mgXMLTag* tag = (mgXMLTag*) m_tagStack[i];
    delete tag;
  }
  m_tagStack.removeAll();
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* mgXMLParser::createTag(
  const char* tagName)
{
  // should be overridden by subclass.  default is create generic tag
  mgXMLTag* tag = new mgXMLTag(tagName);
  return tag;
}

//--------------------------------------------------------------
// pop a tag off the stack
void mgXMLParser::popTagStack()
{
  // pop the tag stack
  int stackLen = m_tagStack.length();
  if (stackLen == 0)
    return;

  m_tagStack.removeAt(stackLen-1);
  stackLen--;

  m_topTag = NULL;

  if (stackLen > 0)
    m_topTag = (mgXMLTag*) m_tagStack[stackLen-1];
}

//--------------------------------------------------------------
  // handle the top tag (no parent to add it to)
void mgXMLParser::processTopTag(
  mgXMLTag* tag)
{
}

//--------------------------------------------------------------
// find tag instance by name
mgXMLTag* mgXMLParser::findTag(
  const char* tagName)
{
  for (int i = m_tagStack.length()-1; i >= 0; i--)
  {
    mgXMLTag* tag = (mgXMLTag*) m_tagStack[i];
    if (tag->m_tagName.equals(tagName))
      return tag;
  }
  return NULL;
}

//--------------------------------------------------------------
// return tag stack depth
int mgXMLParser::getTagStackDepth()
{
  return m_tagStack.length();
}

//--------------------------------------------------------------
// return nth tag on stack (0=top=end of array)
mgXMLTag* mgXMLParser::getTagStackAt(
  int index)
{
  return (mgXMLTag*) m_tagStack[(m_tagStack.length()-1)-index];
}

//--------------------------------------------------------------
// handle CDATA content
void mgXMLParser::CDATAContent(
  const char* text,
  int len)
{
  if (m_topTag != NULL)
    m_topTag->tagContent(this, text, len);
}

//--------------------------------------------------------------
// handle ordinary content
void mgXMLParser::content(
  const char* text,
  int len)
{
  if (m_topTag != NULL)
    m_topTag->tagContent(this, text, len);
}

//--------------------------------------------------------------
// handle start of tag
void mgXMLParser::tagOpen(
  const char* text)
{
  // create the tag and start it
  mgXMLTag* tag = createTag(text);
  if (tag == NULL)
    errorMsg("xmlTagUnknown", "tagName", "%s", (const char*) text);

  m_topTag = tag;
  m_tagStack.add(m_topTag);
  m_topTag->tagOpen(this);
}

//--------------------------------------------------------------
// handle tag with no content
void mgXMLParser::tagNoContent()
{
  if (m_topTag != NULL)
  {
    mgXMLTag* tag = m_topTag;

    // close the tag
    m_topTag->tagClose(this);
    popTagStack();

    // add it to parent
    if (m_topTag != NULL)
      m_topTag->tagChild(this, tag);
    else processTopTag(tag);
  }
}

//--------------------------------------------------------------
// handle close tag
void mgXMLParser::tagClose(
  const char* text)
{
  // if not closing the top tag
  if (m_topTag == NULL)
    errorMsg("xmlNoOpenTag", "tagName,topTag", "%s,%s", (const char*) text, "<none>");
  
  if (!m_topTag->m_tagName.equals(text))
    errorMsg("xmlNoOpenTag", "tagName,topTag", "%s,%s", 
      (const char*) text, (const char*) m_topTag->m_tagName);

  mgXMLTag* tag = m_topTag;

  // close the tag
  m_topTag->tagClose(this);
  popTagStack();

  // add it to parent
  if (m_topTag != NULL)
    m_topTag->tagChild(this, tag);
  else processTopTag(tag);
}

//--------------------------------------------------------------
// handle attribute name
void mgXMLParser::attrName(
  const char* text)
{
  m_attrName = text;
}

//--------------------------------------------------------------
// handle attribute value
void mgXMLParser::attrValue(
  const char* text)
{
  if (m_topTag != NULL)
    m_topTag->tagAttr(this, m_attrName, text);

  m_attrName.empty();
}

//--------------------------------------------------------------
// end of attributes
void mgXMLParser::endAttrs()
{
  if (m_topTag != NULL)
    m_topTag->tagEndAttrs(this);
}

