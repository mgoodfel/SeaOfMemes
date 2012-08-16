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
#ifndef MGXMLPARSER_H
#define MGXMLPARSER_H

#include "mgXMLScanner.h"
#include "mgXMLTag.h"

class mgXMLParser : public mgXMLScanner
{
public:
  // constructor
  mgXMLParser();

  // destructor
  virtual ~mgXMLParser();

  // create a tag instance
  virtual mgXMLTag* createTag(
    const char* tagName);

  // handle the top tag (no parent to add it to)
  virtual void processTopTag(
    mgXMLTag* tag);

  // find tag instance by name
  virtual mgXMLTag* findTag(
    const char* name);

  // return tag stack depth
  virtual int getTagStackDepth();

  // return nth tag on stack (0=top=end of array)
  virtual mgXMLTag* getTagStackAt(
    int index);

protected:
  mgPtrArray m_tagStack;
  mgXMLTag* m_topTag;
  mgString m_attrName;

  // pop a tag off the stack
  virtual void popTagStack();

  // from mgXMLScanner:

  // handle CDATA content
  virtual void CDATAContent(
    const char* text,
    int len);

  // handle ordinary content
  virtual void content(
    const char* text,
    int len);

  // handle tag open
  virtual void tagOpen(
    const char* text);

  // handle attribute name
  virtual void attrName(
    const char* text);

  // handle attribute value
  virtual void attrValue(
    const char* text);

  // end of attributes
  virtual void endAttrs();

  // handle no-content tag
  virtual void tagNoContent();

  // handle tag close
  virtual void tagClose(
    const char* text);

};

#endif

