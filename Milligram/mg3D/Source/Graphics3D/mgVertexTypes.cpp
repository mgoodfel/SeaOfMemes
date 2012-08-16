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

#include "mgPlatform/Include/mgPlatformServices.h"
#include "mgDisplayServices.h"
#include "Graphics3D/mgVertexTypes.h"

const mgVertexAttrib MG_VERTEX_ATTRIBS[] = {
  "vertPoint", MG_VERTEX_FLOAT3, (int) offsetof(mgVertex, m_px), 
  "vertNormal", MG_VERTEX_FLOAT3, (int) offsetof(mgVertex, m_nx), 
  "vertTexCoord0", MG_VERTEX_FLOAT2, (int) offsetof(mgVertex, m_tx),
  NULL, 0, -1  
};

//--------------------------------------------------------------
// load a shader using this vertex
void mgVertex::loadShader(
  const char* shaderName)
{
  mgDisplay->loadShader(shaderName, MG_VERTEX_ATTRIBS);
}

//--------------------------------------------------------------
// create a buffer of this vertex type
mgVertexBuffer* mgVertex::newBuffer(
  int size,
  BOOL dynamic)
{
  return mgDisplay->newVertexBuffer(sizeof(mgVertex), MG_VERTEX_ATTRIBS, size, dynamic);
}

//--------------------------------------------------------------
// texture-array vertex
const mgVertexAttrib MG_VERTEX_TA_ATTRIBS[] = {
  "vertPoint", MG_VERTEX_FLOAT3, (int) offsetof(mgVertexTA, m_px), 
  "vertNormal", MG_VERTEX_FLOAT3, (int) offsetof(mgVertexTA, m_nx), 
  "vertTexCoord0", MG_VERTEX_FLOAT3, (int) offsetof(mgVertexTA, m_tx),
  NULL, 0, -1
};

//--------------------------------------------------------------
// load a shader using this vertex
void mgVertexTA::loadShader(
  const char* shaderName)
{
  mgDisplay->loadShader(shaderName, MG_VERTEX_TA_ATTRIBS);
}

//--------------------------------------------------------------
// create a buffer of this vertex type
mgVertexBuffer* mgVertexTA::newBuffer(
  int size,
  BOOL dynamic)
{
  return mgDisplay->newVertexBuffer(sizeof(mgVertexTA), MG_VERTEX_TA_ATTRIBS, size, dynamic);
}

