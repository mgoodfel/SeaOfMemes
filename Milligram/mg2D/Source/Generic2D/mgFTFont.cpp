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
#ifdef SUPPORT_FTFONT

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Surfaces/mgFTFont.h"

#include "ft2build.h"
#include FT_FREETYPE_H

FT_Library mgFTLibrary;
BYTE* mgFTCharImage = NULL;
size_t mgFTCharImageSize = 0;

//--------------------------------------------------------------
// initialize font handling
void mgFTInit()
{
  // initialize instance of free type library
  int error = FT_Init_FreeType(&mgFTLibrary);
  if (error != 0)
    throw new mgException("FT_Init_FreeType returns %d", error);
}

//--------------------------------------------------------------
// terminate font handling
void mgFTTerm()
{
  FT_Done_FreeType(mgFTLibrary);
  delete mgFTCharImage;
  mgFTCharImage = NULL;
}

//--------------------------------------------------------------
// load a font and return handle
void* mgFTLoadFont(
  const char* fontFile,
  double size,
  int dpi,
  double &fontHeight,
  double &fontAscent,
  double &charWidth)
{
  FT_Face face;
  int error = FT_New_Face(mgFTLibrary, fontFile, 0, &face); 
  if (error == FT_Err_Unknown_File_Format) 
  { 
    throw new mgException("unknown file format %s", (const char*) fontFile);
  } 
  else if (error != 0)
  { 
    throw new mgException("FT_New_Face(%s) returns %d", (const char*) fontFile, error);
  }

  mgDebug("load font %s - %s : %d faces, %d glyphs.", face->family_name, face->style_name, face->num_faces, face->num_glyphs);

  error = FT_Set_Char_Size(face, 0, (FT_F26Dot6) (size*64), dpi, dpi);
  if (error != 0)
    mgDebug("FT_Set_Char_Size = %d", error);

  fontHeight = face->size->metrics.height/64.0;
  fontAscent = face->size->metrics.ascender/64.0;
  double advanceX, advanceY, posnX, posnY;
  int imageWidth, imageHeight;
  BYTE* imageData;
  mgFTGetChar(face, 'n', advanceX, advanceY, posnX, posnY, imageWidth, imageHeight, imageData);
  charWidth = advanceX; 
  return face;
}

//--------------------------------------------------------------
// delete font
void mgFTDeleteFont(
  void* handle)
{
  FT_Face face = (FT_Face) handle;
  
  if (face != NULL)
    FT_Done_Face(face);
}

//--------------------------------------------------------------
// get a character in the font
void mgFTGetChar(
  void* ftFace,
  int letter,
  double &advanceX,      // from start to end point
  double &advanceY,
  double &posnX,         // from start to top-left of image
  double &posnY,
  int &imageWidth,
  int &imageHeight,
  BYTE*& imageData)
{
  FT_Face face = (FT_Face) ftFace;
  advanceX = advanceY = posnX = posnY = 0.0;
  imageWidth = imageHeight = 0;
  imageData = NULL;

  FT_UInt glyphIndex = FT_Get_Char_Index(face, letter);

  int error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER ); 
  if (error != 0)
  {
    mgDebug("FT_Load_Glyph returns %d", error);
    return;
  }

  FT_GlyphSlot slot = face->glyph;
  FT_Bitmap bitmap = slot->bitmap;

  advanceX = slot->advance.x/64.0;
  advanceY = slot->advance.y/64.0;

  posnX = slot->bitmap_left;
  posnY = slot->bitmap_top;
  imageWidth = bitmap.width;
  imageHeight = bitmap.rows;

  // round width up to next multiple of 4 for OpenGL glTexSubImage alignment
  int outWidth = 4*((bitmap.width+3)/4);

  // enlarge character buffer if required
  size_t size = (size_t) (outWidth * bitmap.rows);
  if (size > mgFTCharImageSize)
  {
    delete mgFTCharImage;
    mgFTCharImageSize = size;
    mgFTCharImage = new BYTE[mgFTCharImageSize];
  }

  // copy data from FreeType bitmap
  imageData = mgFTCharImage;
  memset(imageData, 0, bitmap.rows*outWidth);

  for (int i = 0; i < bitmap.rows; i++)
  {
    BYTE* outLine = imageData + outWidth * i;
    BYTE* bitmapLine = bitmap.buffer + bitmap.pitch * i;
    memcpy(outLine, bitmapLine, bitmap.width);
  }
}

#endif
