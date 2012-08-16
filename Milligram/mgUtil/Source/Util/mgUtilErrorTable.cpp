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

#include "Util/mgUtilErrorTable.h"

//--------------------------------------------------------------
// constructor
mgUtilErrorTable::mgUtilErrorTable()
{
  mgString xml;
  xml += "<errorTable>";

  xml += "<errorMsg id=\"noTimer\">No high resolution timer available.</errorMsg>";

  xml += "<errorMsg id=\"optionBadBoolean\"><var name=\"filename\"/>: Attribute \"<var name=\"attr\"/>\" ";
  xml += "must be true or false, not \"<var name=\"value\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"optionBadInteger\"><var name=\"filename\"/>: Attribute \"<var name=\"attr\"/>\" ";
  xml += "must have integer value, not \"<var name=\"value\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"optionBadDouble\"><var name=\"filename\"/>: Attribute \"<var name=\"attr\"/>\" ";
  xml += "must have floating point value, not \"<var name=\"value\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"optionBadPoint\"><var name=\"filename\"/>: Attribute \"<var name=\"attr\"/>\" ";
  xml += "must have point \"x y z\" value, not \"<var name=\"value\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"utfContinue\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Invalid UTF-8 code <var name=\"char\"/> -- too many continuation bytes.</errorMsg>";

  xml += "<errorMsg id=\"utfRange\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Cannot handle Unicode char over 2 bytes \"<var name=\"char\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"utfInvalid\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Invalid unicode byte <var name=\"char\"/>.</errorMsg>";

  xml += "<errorMsg id=\"xmlEnds\">Unexpected end of input.</errorMsg>";

  xml += "<errorMsg id=\"xmlFileNotFound\">Could not open file \"<var name=\"filename\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlBadString\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Unexpected string \"<var name=\"string\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlMissingEquals\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Expected '='.</errorMsg>";

  xml += "<errorMsg id=\"xmlMissingQuote\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Expected quoted value.</errorMsg>";

  xml += "<errorMsg id=\"xmlMissingSemi\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Expected ; in entity reference.</errorMsg>";

  xml += "<errorMsg id=\"xmlBadClose\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Bad close tag.</errorMsg>";

  xml += "<errorMsg id=\"xmlTagUnknown\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Unknown tag \"<var name=\"tagName\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlNoOpenTag\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Closing tag \"<var name=\"tagName\"/>\", open tag is \"<var name=\"topTag\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlBadAttr\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Unknown attribute \"<var name=\"tagName\"/>.<var name=\"attr\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlBadAttrValue\"><var name=\"filename\"/>, line <var name=\"line\"/>: attribute ";
  xml += "<var name=\"attr\"/> cannot be \"<var name=\"value\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlBadChild\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Tag \"<var name=\"tagName\"/>\" cannot contain \"<var name=\"childName\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlBadBoolean\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Attribute \"<var name=\"attr\"/>\" must be true or false, not \"<var name=\"value\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlBadInteger\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Attribute \"<var name=\"attr\"/>\" must have integer value, not \"<var name=\"value\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"xmlBadDouble\"><var name=\"filename\"/>, line <var name=\"line\"/>, col <var name=\"col\"/>: ";
  xml += "Attribute \"<var name=\"attr\"/>\" must have floating point value, not \"<var name=\"value\"/>\".</errorMsg>";

  xml += "<errorMsg id=\"imgJpgLib\"><var name=\"filename\"/>: Error during Jpeg image encode or decode.</errorMsg>";

  xml += "<errorMsg id=\"imgNotBMP\"><var name=\"filename\"/>: File not in Windows BMP format.</errorMsg>";

  xml += "<errorMsg id=\"imgBMPDepth\"><var name=\"filename\"/>: Only 24 and 32 bit uncompressed Windows bitmaps supported.</errorMsg>";

  xml += "<errorMsg id=\"imgBMPSize\"><var name=\"filename\"/>: Calculated image size (<var name=\"calcSize\"/>) does not match declared size (<var name=\"declSize\"/>).</errorMsg>";

  xml += "<errorMsg id=\"imgBMPRead\"><var name=\"filename\"/>: Could not read <var name=\"size\"/> bytes of image.</errorMsg>";

  xml += "<errorMsg id=\"imgAlphaSize\"><var name=\"filename\"/>: Alpha image <var name=\"alphawd\"/> by <var name=\"alphaht\"/> does not match ";
  xml += "RGB image <var name=\"rgbwd\"/> by <var name=\"rgbht\"/>.</errorMsg>";

  xml += "<errorMsg id=\"imgBadSize\"><var name=\"filename\"/>: Bad image size <var name=\"wd\"/> by <var name=\"ht\"/>.</errorMsg>";

  xml += "<errorMsg id=\"imgNoType\"><var name=\"filename\"/>: No filetype (ex: \".jpg\").</errorMsg>";

  xml += "<errorMsg id=\"imgBadType\"><var name=\"filename\"/>: File Type <var name=\"type\"/> not supported.</errorMsg>";

  xml += "<errorMsg id=\"imgBadOpen\"><var name=\"filename\"/>: Cannot open file.</errorMsg>";

  xml += "<errorMsg id=\"imgRead\"><var name=\"filename\"/>: Decode error.</errorMsg>";

  xml += "<errorMsg id=\"imgWrite\"><var name=\"filename\"/>: Encode error.</errorMsg>";

  xml += "</errorTable>";

  parse(xml.length(), xml);
}

