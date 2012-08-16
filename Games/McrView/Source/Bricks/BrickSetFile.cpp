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

#include "BrickSetFile.h"

//--------------------------------------------------------------
// constructor
BrickSetFile::BrickSetFile(
  const char* fileName)
{
  parseFile(fileName);
}

//--------------------------------------------------------------
// destructor
BrickSetFile::~BrickSetFile()
{
  // delete the bricks
  for (int i = 0; i < m_brickTags.length(); i++)
  {
    BrickTag* brick = (BrickTag*) m_brickTags[i];
    delete brick;
  }
  m_brickTags.removeAll();
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* BrickSetFile::createTag(
  const char* tagName)
{
  if (_stricmp(tagName, "brickSet") == 0)
    return new BrickSetTag(tagName);
  else if (_stricmp(tagName, "brick") == 0)
    return new BrickTag(tagName);
  else return NULL;
}

//--------------------------------------------------------------
// handle the top tag (no parent to add it to)
void BrickSetFile::processTopTag(
  mgXMLTag* tag)
{
  // brickSet tag no longer needed
  delete tag;
}

//--------------------------------------------------------------
// constructor
BrickSetTag::BrickSetTag(
  const char* name)
: mgXMLTag(name)
{
}

//--------------------------------------------------------------
// child tag
void BrickSetTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  BrickSetFile* brickSet = (BrickSetFile*) parser;
  brickSet->m_brickTags.add(tag);
}

//--------------------------------------------------------------
// constructor
BrickTag::BrickTag(
  const char* name)
: mgXMLTag(name)
{
  m_majorCode = -1;  // not set
  m_dir = 0;
  m_light = false;
  m_shape = "cube";
}

//--------------------------------------------------------------
// attribute set
void BrickTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  if (_stricmp(attrName, "code") == 0)
  {
    int code = -1;
    int count = sscanf(attrValue, "%xd", &code);
    if (count == 1)
    {
      m_majorCode = code/256;
      m_minorCode = code%256;
    }
    else throw new mgException("invalid code %s", (const char*) attrValue);
  }
  else if (_stricmp(attrName, "left") == 0 ||
           _stricmp(attrName, "mat0") == 0 ||
           _stricmp(attrName, "xmin") == 0)
      getFileName(parser, attrName, attrValue, m_xminTexture);

  else if (_stricmp(attrName, "right") == 0 ||
           _stricmp(attrName, "mat1") == 0 ||
           _stricmp(attrName, "xmax") == 0)
    getFileName(parser, attrName, attrValue, m_xmaxTexture);

  else if (_stricmp(attrName, "bottom") == 0 ||
           _stricmp(attrName, "mat2") == 0 ||
           _stricmp(attrName, "ymin") == 0)
    getFileName(parser, attrName, attrValue, m_yminTexture);

  else if (_stricmp(attrName, "top") == 0 ||
           _stricmp(attrName, "mat3") == 0 ||
           _stricmp(attrName, "ymax") == 0)
    getFileName(parser, attrName, attrValue, m_ymaxTexture);

  else if (_stricmp(attrName, "front") == 0 ||
           _stricmp(attrName, "mat4") == 0 ||
           _stricmp(attrName, "zmin") == 0)
    getFileName(parser, attrName, attrValue, m_zminTexture);

  else if (_stricmp(attrName, "back") == 0 ||
           _stricmp(attrName, "mat5") == 0 ||
           _stricmp(attrName, "zmax") == 0)
    getFileName(parser, attrName, attrValue, m_zmaxTexture);

  else if (_stricmp(attrName, "sides") == 0)
  {
    getFileName(parser, attrName, attrValue, m_xminTexture);
    m_xmaxTexture = m_xminTexture;
    m_zminTexture = m_xminTexture;
    m_zmaxTexture = m_xminTexture;
  }

  else if (_stricmp(attrName, "all") == 0)
  {
    getFileName(parser, attrName, attrValue, m_xminTexture);
    m_xmaxTexture = m_xminTexture;
    m_zminTexture = m_xminTexture;
    m_zmaxTexture = m_xminTexture;
    m_yminTexture = m_xminTexture;
    m_ymaxTexture = m_xminTexture;
  }

  else if (_stricmp(attrName, "icon") == 0)
    getFileName(parser, attrName, attrValue, m_icon);

  else if (_stricmp(attrName, "light") == 0)
    m_light = getBoolean(parser, attrName, attrValue);

  else if (_stricmp(attrName, "dir") == 0)
    m_dir = getInteger(parser, attrName, attrValue);

  else if (_stricmp(attrName, "shape") == 0)
  {
    m_shape = attrValue;
    m_shape.trim();
    if (!(m_shape.equals("cube") ||
          m_shape.equals("slab") ||
          m_shape.equals("cap") ||
          m_shape.equals("stair") ||
          m_shape.equals("column")))
    {
      getFileName(parser, attrName, attrValue, m_shape);
    }
  }
}

//--------------------------------------------------------------
// tag ends
void BrickTag::tagClose(
  mgXMLParser* parser)
{
  if (m_majorCode == -1)
    throw new mgException("no code specified");
}
