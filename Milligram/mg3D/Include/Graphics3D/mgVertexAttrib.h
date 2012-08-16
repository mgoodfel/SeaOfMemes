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
#ifndef MGVERTEXATTRIB_H
#define MGVERTEXATTRIB_H

const int MG_VERTEX_UBYTE1   = 0;
const int MG_VERTEX_UBYTE2   = 1;
const int MG_VERTEX_UBYTE3   = 2;
const int MG_VERTEX_UBYTE4   = 3;
const int MG_VERTEX_UINT1    = 4;
const int MG_VERTEX_UINT2    = 5;
const int MG_VERTEX_UINT3    = 6;
const int MG_VERTEX_FLOAT1   = 7;
const int MG_VERTEX_FLOAT2   = 8;
const int MG_VERTEX_FLOAT3   = 9;
const int MG_VERTEX_FLOAT4   = 10;

class mgVertexAttrib
{
public:
  const char* m_name;         // shader variable name
  int m_type;                 // type of attribute
  int m_offset;               // offset of attribute
};

#endif
