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

#include "XMLParser/mgXMLTag.h"
#include "XMLParser/mgXMLParser.h"

//--------------------------------------------------------------
// constructor
mgXMLTag::mgXMLTag(
  const char* tagName)
{
  m_tagName = tagName;
}

//--------------------------------------------------------------
// attribute set
void mgXMLTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue) 
{
  parser->errorMsg("xmlBadAttr", "tagName,attr", "%s,%s", (const char*) m_tagName, (const char*) attrName);
}

//--------------------------------------------------------------
// child tag
void mgXMLTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag) 
{
  parser->errorMsg("xmlBadChild", "tagName,childName", "%s,%s", (const char*) m_tagName, (const char*) tag->m_tagName);

  // parent tag now owns child pointer
  delete tag;
}

//--------------------------------------------------------------
// return boolean value from attribute string
BOOL mgXMLTag::getBoolean(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  if (_stricmp(attrValue, "true") == 0)
    return true;
  else if (_stricmp(attrValue, "false") == 0)
    return false;
  else 
  {
    parser->errorMsg("xmlBadBoolean", "attr,value", "%s,%s", (const char*) attrName, (const char*) attrValue);
    return false;
  }
}

//--------------------------------------------------------------
// return integer value from attribute string
int mgXMLTag::getInteger(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  int value;
  if (1 == sscanf((const char*) attrValue, "%d", &value))
    return value;

  else 
  {
    parser->errorMsg("xmlBadInteger", "attr,value", "%s,%s", (const char*) attrName, (const char*) attrValue);
    return 0;
  }
}

//--------------------------------------------------------------
// return integer value from attribute string
double mgXMLTag::getDouble(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  double value;
  if (1 == sscanf((const char*) attrValue, "%lg", &value))
    return value;

  else 
  {
    parser->errorMsg("xmlBadDouble", "attr,value", "%s,%s", (const char*) attrName, (const char*) attrValue);
    return 0.0;
  }
}

//--------------------------------------------------------------
// get file name from attribute string
void mgXMLTag::getFileName(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue,
  mgString& fileName)
{
  mgString name(attrValue);
  // handle ;-delimeted pairs of filenames we use for RGB+alpha images
  int semi = name.find(0, ";");
  if (semi != -1)
  {
    mgString rgbName;
    name.substring(rgbName, 0, semi);
    name.deleteAt(0, semi+1);
    mgOSResolveRelativeName(parser->m_sourceFileName, rgbName, fileName);
    fileName += ';';
    mgString alphaName;
    mgOSResolveRelativeName(parser->m_sourceFileName, name, alphaName);
    fileName += alphaName;
  }
  else
  {
    mgOSResolveRelativeName(parser->m_sourceFileName, name, fileName);
  }
}
