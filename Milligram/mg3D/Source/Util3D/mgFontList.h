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
#ifndef MGFONTLIST_H
#define MGFONTLIST_H

#include "mgUtil/Include/XMLParser/mgXMLParser.h"

class mgCursorDefn;

class mgFontListTag : public mgXMLTag
{
public:
  // constructor
  mgFontListTag(
    const char* name);

  // destructor
  virtual ~mgFontListTag();

  // add child tag.  parent tag now owns child pointer.
  virtual void tagChild(
    mgXMLParser* parser,
    mgXMLTag* tag);
};

class mgFontTag : public mgXMLTag
{
public:
  mgString m_face;  
  BOOL m_bold;
  BOOL m_italic;
  mgStringArray m_fileNames;

  // constructor
  mgFontTag(
    const char* name);

  // destructor
  virtual ~mgFontTag();

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);
};

class mgFontList: public mgXMLParser
{
public:
  mgMapStringToPtr m_fonts;

  // constructor
  mgFontList(
    const char* fileName);

  // destructor
  virtual ~mgFontList();

  // find a font file
  virtual BOOL findFont(
    const mgStringArray& fontDirs,
    const char* face,
    BOOL bold,
    BOOL italic,
    mgString& fontFile);

  // create a tag instance
  virtual mgXMLTag* createTag(
    const char* tagName);

  // handle the top tag (no parent to add it to)
  virtual void processTopTag(
    mgXMLTag* tag);
};

#endif
