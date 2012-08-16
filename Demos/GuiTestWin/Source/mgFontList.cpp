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

#include "mgFontList.h"

//--------------------------------------------------------------
// constructor
mgFontList::mgFontList(
  const char* fileName)
{
  parseFile(fileName);
}

//--------------------------------------------------------------
// destructor
mgFontList::~mgFontList()
{
  // delete the fonts
  int posn = m_fonts.getStartPosition();
  while (posn != -1)
  {
    mgString key;
    const void* value;
    m_fonts.getNextAssoc(posn, key, value);
    delete (mgFontTag*) value;
  }
  m_fonts.removeAll();
}

//--------------------------------------------------------------
// find a font file
BOOL mgFontList::findFont(
  const mgStringArray& fontDirs,
  const char* face,
  BOOL bold,
  BOOL italic,
  mgString& fontFile)
{
  mgDebug("search for font %s%s%s", face, bold?"-b":"", italic?"-i":"");

  // build the key for the font 
  mgString key(face);
  key.makeLower();
  if (bold)
    key += "-b";
  if (italic)
    key += "-i";

  // find the face in the font list
  const void *value;
  if (!m_fonts.lookup(key, value))
  {
    mgDebug("font key not found, use default.");
    // not found, try default
    key = "default";
    key.makeLower();
    if (bold)
      key += "-b";
    if (italic)
      key += "-i";
    if (!m_fonts.lookup(key, value))
      return false;
  }
  mgFontTag* fontTag = (mgFontTag*) value;

  // look for each file name along the path
  for (int i = fontDirs.length()-1; i >= 0; i--)
  {
    for (int j = 0; j < fontTag->m_fileNames.length(); j++)
    {
      // construct the file name to test
      fontFile.format("%s%s", (const char*) fontDirs[i], (const char*) fontTag->m_fileNames[j]);
      mgOSFixFileName(fontFile);
      mgDebug("check for font at %s", (const char*) fontFile);

      // if file found, we're done
      struct _stat info;
      if (0 == _stat(fontFile, &info))
        return true;
    }
  }
  return false;
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* mgFontList::createTag(
  const char* tagName)
{
  if (_stricmp(tagName, "fontList") == 0)
    return new mgFontListTag(tagName);
  else if (_stricmp(tagName, "font") == 0)
    return new mgFontTag(tagName);
  else return NULL;
}

//--------------------------------------------------------------
// handle the top tag (no parent to add it to)
void mgFontList::processTopTag(
  mgXMLTag* tag)
{
  // we don't need the top tag anymore
  delete tag;
}

//--------------------------------------------------------------
// constructor
mgFontListTag::mgFontListTag(
  const char* name)
: mgXMLTag(name)
{
}

//--------------------------------------------------------------
// destructor
mgFontListTag::~mgFontListTag()
{
}

//--------------------------------------------------------------
// add child tag.  parent tag now owns child pointer.
void mgFontListTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  mgFontList* list = (mgFontList*) parser;
  mgFontTag* fontTag = (mgFontTag*) tag;

  // create key for font
  mgString key(fontTag->m_face);
  key.makeLower();
  if (fontTag->m_bold)
    key += "-b";
  if (fontTag->m_italic)
    key += "-i";

  // add it to the font table
  list->m_fonts.setAt(key, fontTag);
}

//--------------------------------------------------------------
// constructor
mgFontTag::mgFontTag(
  const char* name)
: mgXMLTag(name)
{
}

//--------------------------------------------------------------
// destructor
mgFontTag::~mgFontTag()
{
}

//--------------------------------------------------------------
// attribute set
void mgFontTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  if (_stricmp(attrName, "face") == 0)
  {
    m_face = attrValue;
    m_face.trim();
  }

  else if (_stricmp(attrName, "bold") == 0)
    m_bold = getBoolean(parser, attrName, attrValue);

  else if (_stricmp(attrName, "italic") == 0)
    m_italic = getBoolean(parser, attrName, attrValue);

  else if (_stricmp(attrName, "names") == 0)
  {
    mgString value(attrValue);
    mgString token;
    int posn = 0;
    while (true)
    {
      posn = value.getToken(posn, ";", token);
      if (token.length() > 0)
        m_fileNames.add(token);
      else break;
    }
  }
}
