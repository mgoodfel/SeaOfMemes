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

#include "Util3D/mg3DErrorTable.h"

//--------------------------------------------------------------
// constructor
mg3DErrorTable::mg3DErrorTable()
{
  mgString xml;
  xml += "<errorTable>";

  xml += "<errorMsg id=\"glNullIndex\">mgGLIndexBuffer: loadDisplay with NULL memory.</errorMsg>";

  xml += "<errorMsg id=\"glNullVertex\">mgGLVertexBuffer: loadDisplay with NULL memory.</errorMsg>";

  xml += "<errorMsg id=\"glBadShader\">mgGLMisc: Cannot compile any version of shader ";
  xml += "<var name=\"vsName\"/>, <var name=\"fsName\"/>.</errorMsg>";

  xml += "<errorMsg id=\"glBadShaderLink\">mgGLMisc: Linker error on shaders ";
  xml += "<var name=\"vsName\"/>, <var name=\"fsName\"/>.</errorMsg>";

  xml += "<errorMsg id=\"glShader\">mgGL: Shader \"<var name=\"shaderName\"/>\" not found.</errorMsg>";

  xml += "<errorMsg id=\"glWrongThread\">mgGL: OpenGL call from non-main thread.</errorMsg>";

  xml += "<errorMsg id=\"glIndexReset\">mgGLIndexBuffer: Reset of static buffer.</errorMsg>";

  xml += "<errorMsg id=\"glVertexReset\">mgGLVertexBuffer: Reset of static buffer.</errorMsg>";

  xml += "<errorMsg id=\"glIndexFull\">mgGLIndexBuffer: Out of space.</errorMsg>";

  xml += "<errorMsg id=\"glVertexFull\">mgGLVertexBuffer: Out of space.</errorMsg>";

  xml += "<errorMsg id=\"2dBadFontSpec\">Invalid font specification <var name=\"spec\"/>.</errorMsg>";

  xml += "</errorTable>";

  parse(xml.length(), xml);
}

