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
#ifndef SHAPEFILE_H
#define SHAPEFILE_H

class FloatArrayTag : public mgXMLTag
{
public:
  mgString m_ID;
  double* m_values;
  int m_len;
  int m_posn;
  mgString m_token;

  // constructor
  FloatArrayTag(
    const char* name);

  // destructor
  virtual ~FloatArrayTag();

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);

  // text content
  virtual void tagContent(
    mgXMLParser* parser,
    const char* text,
    int len);

  // tag ends
  virtual void tagClose(
    mgXMLParser* parser);
};

class TrianglesTag : public mgXMLTag
{
public:
  int m_material;
  mgString m_positionsID;
  mgString m_normalsID;
  mgString m_texcoordsID;
  mgString m_ambientID;

  int* m_values;
  int m_len;
  int m_posn;
  mgString m_token;

  // constructor
  TrianglesTag(
    const char* name);

  // destructor
  virtual ~TrianglesTag();

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);

  // text content
  virtual void tagContent(
    mgXMLParser* parser,
    const char* text,
    int len);

  // tag ends
  virtual void tagClose(
    mgXMLParser* parser);
};

class ShapeTag : public mgXMLTag
{
public:
  // constructor
  ShapeTag(
    const char* name);

  // attribute set
  virtual void tagAttr(
    mgXMLParser* parser,
    const char* attrName,
    const char* attrValue);

  // child tag
  virtual void tagChild(
    mgXMLParser* parser,
    mgXMLTag* tag);
};

class ShapeFile : public mgXMLParser
{
public:
  double m_xmin;
  double m_xmax;
  double m_ymin;
  double m_ymax;
  double m_zmin;
  double m_zmax;

  mgPtrArray m_triangles;
  mgMapStringToPtr m_arrays;

  // constructor
  ShapeFile(
    const char* fileName);

  // destructor
  virtual ~ShapeFile();

  // create a tag instance
  virtual mgXMLTag* createTag(
    const char* tagName);

  // handle the top tag (no parent to add it to)
  virtual void processTopTag(
    mgXMLTag* tag);
};

#endif
