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

#include "ShapeFile.h"

//--------------------------------------------------------------
// constructor
ShapeFile::ShapeFile(
  const char* fileName)
{
  m_xmin = m_ymin = m_zmin = 0.0;
  m_xmax = m_ymax = m_zmax = 1.0;

  parseFile(fileName);
}

//--------------------------------------------------------------
// destructor
ShapeFile::~ShapeFile()
{
  // delete the triangles
  for (int i = 0; i < m_triangles.length(); i++)
  {
    TrianglesTag* triangles = (TrianglesTag*) m_triangles[i];
    delete triangles;
  }
  m_triangles.removeAll();

  // delete the arrays
  int posn = m_arrays.getStartPosition();
  mgString key;
  const void* value;
  while (posn != -1)
  {
    m_arrays.getNextAssoc(posn, key, value);
    FloatArrayTag* arr = (FloatArrayTag*) value;
    delete arr;
  }
  m_arrays.removeAll();
}

//--------------------------------------------------------------
// create a tag instance
mgXMLTag* ShapeFile::createTag(
  const char* tagName)
{
  if (_stricmp(tagName, "float_array") == 0)
    return new FloatArrayTag(tagName);
  else if (_stricmp(tagName, "triangles") == 0)
    return new TrianglesTag(tagName);
  else if (_stricmp(tagName, "shape") == 0)
    return new ShapeTag(tagName);
  else return NULL;
}

//--------------------------------------------------------------
// handle the top tag (no parent to add it to)
void ShapeFile::processTopTag(
  mgXMLTag* tag)
{
  // tag no longer needed
  delete tag;
}

//--------------------------------------------------------------
// constructor
ShapeTag::ShapeTag(
  const char* name)
: mgXMLTag(name)
{
}

//--------------------------------------------------------------
// attribute set
void ShapeTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  ShapeFile* shape = (ShapeFile*) parser;
  if (_stricmp(attrName, "xmin") == 0)
    shape->m_xmin = getDouble(parser, attrName, attrValue);
  else if (_stricmp(attrName, "xmax") == 0)
    shape->m_xmax = getDouble(parser, attrName, attrValue);

  else if (_stricmp(attrName, "ymin") == 0)
    shape->m_ymin = getDouble(parser, attrName, attrValue);
  else if (_stricmp(attrName, "ymax") == 0)
    shape->m_ymax = getDouble(parser, attrName, attrValue);

  else if (_stricmp(attrName, "zmin") == 0)
    shape->m_zmin = getDouble(parser, attrName, attrValue);
  else if (_stricmp(attrName, "zmax") == 0)
    shape->m_zmax = getDouble(parser, attrName, attrValue);
}

//--------------------------------------------------------------
// child tag
void ShapeTag::tagChild(
  mgXMLParser* parser,
  mgXMLTag* tag)
{
  ShapeFile* shape = (ShapeFile*) parser;
  if (tag->m_tagName.equalsIgnoreCase("float_array"))
  {
    FloatArrayTag* arr = (FloatArrayTag*) tag;
    shape->m_arrays.setAt(arr->m_ID, arr);
  }
  else if (tag->m_tagName.equalsIgnoreCase("triangles"))
  {
    shape->m_triangles.add(tag);
  }
  else delete tag;
}

//--------------------------------------------------------------
// constructor
TrianglesTag::TrianglesTag(
  const char* name)
: mgXMLTag(name)
{
  m_len = 0;
  m_posn = 0;
  m_values = NULL;
  m_material = -1;
}

//--------------------------------------------------------------
// destructor
TrianglesTag::~TrianglesTag()
{
  delete m_values;
  m_values = NULL;
}

//--------------------------------------------------------------
// attribute set
void TrianglesTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  if (_stricmp(attrName, "positions") == 0)
    m_positionsID = attrValue;
  else if (_stricmp(attrName, "normals") == 0)
    m_normalsID = attrValue;
  else if (_stricmp(attrName, "texcoords") == 0)
    m_texcoordsID = attrValue;
  else if (_stricmp(attrName, "ambient") == 0)
    m_ambientID = attrValue;

  else if (_stricmp(attrName, "material") == 0)
    m_material = getInteger(parser, attrName, attrValue);

  else if (_stricmp(attrName, "count") == 0)
  {
    if (1 != sscanf(attrValue, "%d", &m_len))
      parser->exception("invalid count");
    m_values = new int[m_len];
  }
}

//--------------------------------------------------------------
// text content
void TrianglesTag::tagContent(
  mgXMLParser* parser,
  const char* text,
  int len)
{
  for (int i = 0; i < len; i++)
  {
    char c = text[i];
    if (isspace(c))
    {
      if (!m_token.isEmpty())
      {
        int value;
        sscanf(m_token, "%d", &value);
        if (m_posn == m_len)
          parser->exception("too many index values in Triangle tag"); 
        m_values[m_posn++] = value;
        m_token.empty();
      }
    }
    else m_token += c;
  }
}

//--------------------------------------------------------------
// tag ends
void TrianglesTag::tagClose(
  mgXMLParser* parser)
{
  if (!m_token.isEmpty())
  {
    int value;
    sscanf(m_token, "%d", &value);
    if (m_posn == m_len)
      parser->exception("too many values in float array"); 
    m_values[m_posn++] = value;
    m_token.empty();
  }
}

//--------------------------------------------------------------
// constructor
FloatArrayTag::FloatArrayTag(
  const char* name)
: mgXMLTag(name)
{
  m_len = 0;
  m_posn = 0;
  m_values = NULL;
}

//--------------------------------------------------------------
// destructor
FloatArrayTag::~FloatArrayTag()
{
  delete m_values;
  m_values = NULL;
}

//--------------------------------------------------------------
// attribute set
void FloatArrayTag::tagAttr(
  mgXMLParser* parser,
  const char* attrName,
  const char* attrValue)
{
  if (_stricmp(attrName, "id") == 0)
    m_ID = attrValue;
  else if (_stricmp(attrName, "count") == 0)
  {
    if (1 != sscanf(attrValue, "%d", &m_len))
      parser->exception("invalid count");
    m_values = new double[m_len];
  }
}

//--------------------------------------------------------------
// text content
void FloatArrayTag::tagContent(
  mgXMLParser* parser,
  const char* text,
  int len)
{
  for (int i = 0; i < len; i++)
  {
    char c = text[i];
    if (isspace(c))
    {
      if (!m_token.isEmpty())
      {
        double value;
        sscanf(m_token, "%lf", &value);
        if (m_posn == m_len)
          parser->exception("too many values in %s", (const char*) m_tagName); 
        m_values[m_posn++] = value;
        m_token.empty();
      }
    }
    else m_token += c;
  }
}

//--------------------------------------------------------------
// tag ends
void FloatArrayTag::tagClose(
  mgXMLParser* parser)
{
  if (!m_token.isEmpty())
  {
    double value;
    sscanf(m_token, "%lf", &value);
    if (m_posn == m_len)
      parser->exception("too many values in float array"); 
    m_values[m_posn++] = value;
    m_token.empty();
  }
}
