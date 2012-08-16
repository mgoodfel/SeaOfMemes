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
#ifndef MGFONTCONFPARSER_H
#define MGFONTCONFPARSER_H

class mgFontConfParser;

// ignore all generic tags
class mgFontConfTag : public mgXMLTag
{
public:
  // constructor
  mgFontConfTag(
    const char* name);

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);

  // add child tag.  parent tag now owns child pointer.
  virtual void tagChild(
    mgXMLParser* parser,
    mgXMLTag* tag);
};

// process <dir> tag
class mgFontDirTag : public mgXMLTag
{
public:
  mgString m_dirName;

  // constructor
  mgFontDirTag(
    const char* name);

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);

  // text content
  virtual void tagContent(
    mgXMLParser* parser,
    const char* string,
    int len); 

  // tag ends
  virtual void tagClose(
    mgXMLParser* parser);
};

class mgFontConfParser : public mgXMLParser
{
public:
  mgMapStringToDWord m_dirList;

  // constructor
  mgFontConfParser();

  // destructor
  virtual ~mgFontConfParser();

protected:
  // create a tag instance
  virtual mgXMLTag* createTag(
    const char* tagName);

  // handle the top tag (no parent to add it to)
  virtual void processTopTag(
    mgXMLTag* tag);
};

#endif
