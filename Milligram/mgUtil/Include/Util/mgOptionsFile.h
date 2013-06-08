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
#ifndef MGOPTIONSFILE_H
#define MGOPTIONSFILE_H

#include "../XMLParser/mgXMLParser.h"

class mgOptionsFile;

/*
  A single option tag in the option file.
*/
class mgOptionsTag : public mgXMLTag
{
public:
  // constructor
  mgOptionsTag(
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

/*
  Parses an XML file of name=value options.  Options can be read as strings,
  integers, booleans, etc.
*/

class mgOptionsFile : public mgXMLParser
{
public:
  mgMapStringToString m_options;

  // constructor
  mgOptionsFile();

  // destructor
  virtual ~mgOptionsFile();

  // get boolean valued option
  virtual BOOL getBoolean(
    const char* name,
    BOOL defaultValue) const;

  // get integer valued option
  virtual int getInteger(
    const char* name,
    int defaultValue) const;

  // get double valued option
  virtual double getDouble(
    const char* name,
    double defaultValue) const;

  // get point valued option
  virtual void getPoint(
    const char* name,
    const mgPoint3& defaultValue,
    mgPoint3& value) const;

  // get string valued option
  virtual void getString(
    const char* name,
    const char* defaultValue,
    mgString& value) const;

  // get filename valued option
  virtual void getFileName(
    const char* name,
    const char* relativeTo,
    const char* defaultValue,
    mgString& fileName) const;

protected:
  // create a tag instance
  virtual mgXMLTag* createTag(
    const char* tagName);

  // handle the top tag (no parent to add it to)
  virtual void processTopTag(
    mgXMLTag* tag);
};

#endif
