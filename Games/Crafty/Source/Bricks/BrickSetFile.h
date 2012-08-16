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
#ifndef BRICKSETFILE_H
#define BRICKSETFILE_H

class BrickTag : public mgXMLTag
{
public:
  int m_majorCode;
  int m_minorCode;
  mgString m_xminTexture;
  mgString m_xmaxTexture;
  mgString m_yminTexture;
  mgString m_ymaxTexture;
  mgString m_zminTexture;
  mgString m_zmaxTexture;
  mgString m_icon;
  int m_dir;
  BOOL m_light;

  mgString m_shape;

  // constructor
  BrickTag(
    const char* name);

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);

  // tag ends
  virtual void tagClose(
    mgXMLParser* parser);
};

class BrickSetTag : public mgXMLTag
{
public:
  // constructor
  BrickSetTag(
    const char* name);

  // child tag
  virtual void tagChild(
    mgXMLParser* parser,
    mgXMLTag* tag);
};

class BrickSetFile : public mgXMLParser
{
public:
  mgPtrArray m_brickTags;

  // constructor
  BrickSetFile(
    const char* fileName);

  // destructor
  virtual ~BrickSetFile();

  // create a tag instance
  virtual mgXMLTag* createTag(
    const char* tagName);

  // handle the top tag (no parent to add it to)
  virtual void processTopTag(
    mgXMLTag* tag);
};

#endif
