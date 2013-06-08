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

#include "Util/mgOptionsFile.h"

//--------------------------------------------------------------
// constructor
mgOptionsFile::mgOptionsFile()
{
}

//--------------------------------------------------------------
// destructor
mgOptionsFile::~mgOptionsFile()
{
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* mgOptionsFile::createTag(
  const char* tagName)
{
  if (_stricmp(tagName, "options") == 0)
    return new mgOptionsTag(tagName);
  else return NULL;
}

//--------------------------------------------------------------
// handle the top tag (no parent to add it to)
void mgOptionsFile::processTopTag(
  mgXMLTag* tag)
{
  // options tag no longer needed
  delete tag;
}

//--------------------------------------------------------------
// constructor
mgOptionsTag::mgOptionsTag(
  const char* name)
: mgXMLTag(name)
{
  mgDebug("Options:");
}

//--------------------------------------------------------------
// attribute set
void mgOptionsTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  mgOptionsFile* options = (mgOptionsFile*) parser;

  mgDebug("  %s: %s", (const char*) attrName, (const char*) attrValue);

  mgString key(attrName);
  key.makeLower();
  options->m_options.setAt(key, attrValue);
}

//--------------------------------------------------------------
// tag ends
void mgOptionsTag::tagClose(
  mgXMLParser* parser)
{
  mgDebug("end of options.");
  mgDebug("");
}

//--------------------------------------------------------------
// get boolean valued option
BOOL mgOptionsFile::getBoolean(
  const char* name,
  BOOL defaultValue) const
{
  mgString key(name);
  key.makeLower();

  mgString value;
  if (!m_options.lookup(key, value))
    return defaultValue;  

  if (value.equalsIgnoreCase("true"))
    return true;
  else if (value.equalsIgnoreCase("false"))
    return false;
  else throw new mgErrorMsg("optionBadBoolean", "filename,attr,value", "%s,%s,%s", 
           (const char*) m_sourceFileName, (const char*) name, (const char*) value);
}

//--------------------------------------------------------------
// get integer valued option
int mgOptionsFile::getInteger(
  const char* name,
  int defaultValue) const
{
  mgString key(name);
  key.makeLower();

  mgString value;
  if (!m_options.lookup(key, value))
    return defaultValue;  

  int intValue;
  if (1 == sscanf((const char*) value, "%d", &intValue))
    return intValue;

  else throw new mgErrorMsg("optionBadInteger", "filename,attr,value", "%s,%s,%s", 
           (const char*) m_sourceFileName, (const char*) name, (const char*) value);
}

//--------------------------------------------------------------
// get double valued option
double mgOptionsFile::getDouble(
  const char* name,
  double defaultValue) const
{
  mgString key(name);
  key.makeLower();

  mgString value;
  if (!m_options.lookup(key, value))
    return defaultValue;  

  double doubleValue;
  if (1 == sscanf((const char*) value, "%lg", &doubleValue))
    return doubleValue;

  else throw new mgErrorMsg("optionBadDouble", "filename,attr,value", "%s,%s,%s", 
           (const char*) m_sourceFileName, (const char*) name, (const char*) value);
}

//--------------------------------------------------------------
// get point valued option
void mgOptionsFile::getPoint(
  const char* name,
  const mgPoint3& defaultValue,
  mgPoint3& pt) const
{
  mgString key(name);
  key.makeLower();

  pt = defaultValue;

  mgString value;
  if (!m_options.lookup(key, value))
    return;

  if (3 != sscanf((const char*) value, "%lg %lg %lg", &pt.x, &pt.y, &pt.z))
  {
    throw new mgErrorMsg("optionBadPoint", "filename,attr,value", "%s,%s,%s", 
           (const char*) m_sourceFileName, (const char*) name, (const char*) value);
  }
}

//--------------------------------------------------------------
// get string valued option
void mgOptionsFile::getString(
  const char* name,
  const char* defaultValue,
  mgString& value) const
{
  mgString key(name);
  key.makeLower();

  if (!m_options.lookup(key, value))
    value = defaultValue;  
}

//--------------------------------------------------------------
// get filename valued option
void mgOptionsFile::getFileName(
  const char* name,
  const char* relativeTo,
  const char* defaultValue,
  mgString& fileName) const
{
  mgString key(name);
  key.makeLower();

  mgString value(defaultValue);
  m_options.lookup(key, value);

  mgString relativeFileName(relativeTo);
  mgOSFixFileName(relativeFileName);

  // handle ;-delimeted pairs of filenames we use for RGB+alpha images
  int semi = value.find(0, ";");
  if (semi != -1)
  {
    mgString rgbName;
    value.substring(rgbName, 0, semi);
    value.deleteAt(0, semi+1);
    mgOSResolveRelativeName(relativeFileName, rgbName, fileName);
    fileName += ';';
    mgString alphaName;
    mgOSResolveRelativeName(relativeFileName, value, alphaName);
    fileName += alphaName;
  }
  else
  {
    mgOSResolveRelativeName(relativeFileName, value, fileName);
  }
}
