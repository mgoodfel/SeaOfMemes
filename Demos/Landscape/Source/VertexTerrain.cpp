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

#include "VertexTerrain.h"

const mgVertexAttrib VERTEX_TERRAIN_ATTRIBS[] = {
  "vertPoint", MG_VERTEX_FLOAT3, (int) offsetof(VertexTerrain, m_px), 
  "vertNormal", MG_VERTEX_FLOAT3, (int) offsetof(VertexTerrain, m_nx), 
  "vertTexCoord0", MG_VERTEX_FLOAT2, (int) offsetof(VertexTerrain, m_tx),
  "vertColor", MG_VERTEX_FLOAT3, (int) offsetof(VertexTerrain, m_red),
  NULL, 0, -1  
};

//--------------------------------------------------------------
// load a shader using this vertex
mgShader* VertexTerrain::loadShader(
  const char* shaderName)
{
  return mgDisplay->loadShader(shaderName, VERTEX_TERRAIN_ATTRIBS);
}

//--------------------------------------------------------------
// create a buffer of this vertex type
mgVertexBuffer* VertexTerrain::newBuffer(
  int size)
{
  return mgDisplay->newVertexBuffer(sizeof(VertexTerrain), VERTEX_TERRAIN_ATTRIBS, size);
}
