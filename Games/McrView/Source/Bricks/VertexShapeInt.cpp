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

#include "VertexShapeInt.h"

const mgVertexAttrib VERTEX_SHAPE_ATTRIBS[] = {
  "vertPoint", MG_VERTEX_UINT3, (int) offsetof(VertexShapeInt, m_word1), 
  NULL, 0, -1  
};

//--------------------------------------------------------------
// load a shader using this vertex
void VertexShapeInt::loadShader(
  const char* shaderName)
{
  mgDisplay->loadShader(shaderName, VERTEX_SHAPE_ATTRIBS);
}

//--------------------------------------------------------------
// create a buffer of this vertex type
mgVertexBuffer* VertexShapeInt::newBuffer(
  int size)
{
  return mgDisplay->newVertexBuffer(sizeof(VertexShapeInt), VERTEX_SHAPE_ATTRIBS, size);
}
