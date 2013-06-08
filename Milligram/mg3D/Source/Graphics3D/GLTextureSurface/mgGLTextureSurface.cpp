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

#include "mgGLTextureSurface.h"
#include "mg2D/Include/Surfaces/mgGenSurface.h"
#include "mg2D/Include/Surfaces/mgGenFont.h"

#ifdef SUPPORT_FTFONT
#include "mg2D/Include/Surfaces/mgFTFont.h"
#endif

#ifdef SUPPORT_CANVASFONT
#include "mg2D/Include/Surfaces/mgCanvasFont.h"
#endif

#include "../mgVertexOverlaySolid.h"
#include "../mgVertexOverlayImage.h"
#include "../mgVertexOverlayText.h"

const int SURFACE_TILE_SIZE = 256;
const int OUTPUT_GRAPHICS_SIZE = 1024*3;  // triangles
const int PRIM_SOLID    = 0;
const int PRIM_IMAGE    = 1;
const int PRIM_TEXT     = 2;

const int CHAR_KEY_FONT_SHIFT = 20;

/*
  position and size of character in charTexture
*/
class mgCharDesc
{
public:
  // position and size of character image in texture
  int m_imageX;
  int m_imageY;
  int m_imageWidth;
  int m_imageHeight;

  // position and advance of glyph when drawn
  double m_posnX;
  double m_posnY;
  double m_advanceX;
  double m_advanceY;
};

/*
  Subclass of GenSurface implemented with OpenGL and mg3D
*/
class mgGLGenSurface : public mgGenSurface
{
  // constructor
  mgGLGenSurface(
    BOOL singleTile,
    BOOL inworld);

  // destructor
  virtual ~mgGLGenSurface();

  // repair damage
  virtual void repair(
    mgRectangle& bounds);

  // set dimensions of surface
  virtual void setSurfaceSize(
    int width,
    int height);

  // create buffers, ready to send to display
  virtual void createBuffers();
  
  // delete any display buffers
  virtual void deleteBuffers();
  
  // draw the surface as an overlay
  virtual void drawOverlay(
    int x,
    int y) const;

protected:
  BOOL m_singleTile;              // true to keep as single tile
  BOOL m_inworld;                 // true if world object, false=headspace

  int m_horzTiles;                // count of horizontal tiles
  int m_vertTiles;                // count of vertical tiles
  mgTextureImage** m_tiles;       // texture array

  mgVertexBuffer* m_solidVertexes;
  mgShader* m_solidShader;

  mgVertexBuffer* m_imageVertexes;
  mgShader* m_imageShader;

  mgVertexBuffer* m_textVertexes;
  mgShader* m_textShader;
  mgShader* m_textM1Shader;
  mgShader* m_textM2Shader;
  int m_lastAlphaMode;
  int m_lastPrimitive;
  mgRectangle m_lastBounds;
  mgTextureImage* m_lastImage;
  mgColor m_lastTextColor;
  BOOL m_pendingGraphics;
  mgRectangle m_lastClip;

  int m_nextFontId;
  mgMapDWordToPtr m_charMap;
  mgTextureImage* m_charTexture;
  int m_charRowHeight;
  int m_charRowWidth;
  int m_charHeightUsed;

  // Subclasses must define this method to return vertical pixels per inch
  virtual int displayDPI() const;

  // load a new font
  virtual void loadFont(
    mgGenFont* font);

  // delete a font
  virtual void deleteFont(
    mgGenFont* font);

  // load image data, return size and handle
  virtual void* loadImage(
    mgGenImage* image);

  // delete image data
  virtual void deleteImage(
    mgGenImage* image);

  // load icon data, return size and handle
  virtual void* loadIcon(
    mgGenIcon* icon);

  // delete icon data
  virtual void deleteIcon(
    mgGenIcon* icon);

  // draw buffered graphics
  virtual void drawGraphics();

  // fill a triangle with color
  virtual void fillSolidTriangle(
    mgGenContextState* state,
    const mgColor& color,
    double ax,
    double ay,
    double bx,
    double by,
    double cx,
    double cy);

  // fill a triangle with image data
  virtual void fillImageTriangle(
    mgGenContextState* state,
    void* imageHandle,
    double ax,
    double ay,
    double bx,
    double by,
    double cx,
    double cy);

  // fill a rectangle with color
  virtual void fillSolidRectangle(
    mgGenContextState* state,
    const mgColor& color,
    double lx,
    double ly,
    double hx,
    double hy);

  // fill a rectangle with image data
  virtual void fillImageRectangle(
    mgGenContextState* state,
    void* imageHandle,
    double dx,
    double dy,
    double dwd,
    double dht,
    double sx,
    double sy,
    double swd,
    double sht);

  // get info on a glyph
  mgCharDesc* getChar(
    const mgGenFont* font,
    int letter,
    BOOL& resize);

  // reset the charmap
  void resetCharMap();

  // empty char texture and resize
  void resizeCharTexture();

  // draw text in font with color
  virtual void drawString(
    mgGenContextState* state,
    const char* text,
    int textLen,
    double x,
    double y);

  // output graphics
  virtual void flush();

  // measure a string
  virtual void stringExtent(
    const mgGenFont* font,
    const char* text,
    int textLen,
    mgPoint& endPt,
    mgRectangle& bounds);
    
  // measure a string (total advance)
  virtual int stringWidth(
    const mgGenFont* font,
    const char* text,
    int textLen);
    
  // return number of chars which fit in width
  virtual int stringFit(
    const mgGenFont* font,
    const char* text,
    int textLen,
    int width);

  // set dimensions of singletile image
  virtual void resizeSingleImage(
    int width,
    int height);

  // set dimensions of singletile image
  virtual void resizeTiledImage(
    int width,
    int height);

  // delete all the tiles
  virtual void deleteTiles();

  friend class mgGLTextureSurface;
};

//--------------------------------------------------------------
// constructor
mgGLTextureSurface::mgGLTextureSurface(
  BOOL singleTile,
  BOOL inworld)
{
//  singleTile = true; // =-= debug
  m_surface = new mgGLGenSurface(singleTile, inworld);
}

//--------------------------------------------------------------
// destructor
mgGLTextureSurface::~mgGLTextureSurface()
{
  delete m_surface;
  m_surface = NULL;
}

//--------------------------------------------------------------
// return the rendering surface
mgSurface* mgGLTextureSurface::getSurface()
{
  return m_surface;
}

//--------------------------------------------------------------
// get texture for normal surfaces (not multi-tiled)
mgTextureImage* mgGLTextureSurface::getTexture() 
{
  if (m_surface->m_singleTile)
    return m_surface->m_tiles[0];
  else throw new mgException("Cannot return texture image for overlay surface.");
}

//--------------------------------------------------------------------
// create buffers, ready to send to display
void mgGLTextureSurface::createBuffers()
{
  m_surface->createBuffers();
}

//--------------------------------------------------------------------
// draw the surface as an overlay
void mgGLTextureSurface::drawOverlay(
  int x,
  int y) const
{
  m_surface->drawOverlay(x, y);
}

//--------------------------------------------------------------------
// delete any display buffers
void mgGLTextureSurface::deleteBuffers()
{
  m_surface->deleteBuffers();
}

/*
  Subclass of mgGenSurface implemented using OpenGL graphics
*/

//--------------------------------------------------------------
// constructor
mgGLGenSurface::mgGLGenSurface(
  BOOL singleTile,
  BOOL inworld)
{
  m_singleTile = singleTile;
  m_inworld = inworld;

  m_tiles = NULL;
  m_vertTiles = 0;
  m_horzTiles = 0;

  mgDisplay->getOverlayShaders(m_solidShader, m_imageShader, m_textShader, m_textM1Shader, m_textM2Shader);

  // create the vertex buffers
  m_solidVertexes = mgDisplay->newVertexBuffer(sizeof(mgVertexOverlaySolid), MGVERTEX_OVERLAYSOLID_ATTRIBS, OUTPUT_GRAPHICS_SIZE, true);
  m_imageVertexes = mgDisplay->newVertexBuffer(sizeof(mgVertexOverlayImage), MGVERTEX_OVERLAYIMAGE_ATTRIBS, OUTPUT_GRAPHICS_SIZE, true);
  m_textVertexes = mgDisplay->newVertexBuffer(sizeof(mgVertexOverlayText), MGVERTEX_OVERLAYTEXT_ATTRIBS, OUTPUT_GRAPHICS_SIZE, true);

  // create the texture for font characters
  m_charTexture = mgDisplay->createTextureMemory(256, 256, MG_MEMORY_FORMAT_GRAY, false);
  m_charRowHeight = 0;
  m_charRowWidth = 0;
  m_charHeightUsed = 0;
  m_nextFontId = 0;

  m_lastAlphaMode = -1;
  m_lastPrimitive = -1;
  m_lastBounds.empty();
  m_lastTextColor = mgColor(0, 0, 0, 1);
  m_pendingGraphics = false;
}

//--------------------------------------------------------------
// destructor
mgGLGenSurface::~mgGLGenSurface()
{
  deleteTiles();

  delete m_solidVertexes;
  m_solidVertexes = NULL;

  delete m_imageVertexes;
  m_imageVertexes = NULL;

  delete m_textVertexes;
  m_textVertexes = NULL;

  delete m_charTexture;
  m_charTexture = NULL;

  resetCharMap();

  // we must reset resources here while we still exist.  otherwise,
  // superclass will try to delete them and fail.
  removeAllResources();
}

//--------------------------------------------------------------------
// Subclasses must define this method to return vertical pixels per inch
int mgGLGenSurface::displayDPI() const
{
  // get dpi from mgDisplay
  return mgDisplay->getDPI();
}

//--------------------------------------------------------------------
// load a new font
void mgGLGenSurface::loadFont(
  mgGenFont* font)
{
#ifdef SUPPORT_FTFONT
  // find the font file
  mgString fontFile;
  if (!mgDisplay->findFont(font->m_faceName, font->m_bold, font->m_italic, fontFile))
  {
    if (!mgDisplay->findFont("default", font->m_bold, font->m_italic, fontFile))
     throw new mgException("cannot find font %s%s%s or default.", 
        (const char*) font->m_faceName, font->m_bold?"-b":"", font->m_italic?"-i":"");
  }

  double height, ascent, charWidth;
  font->m_handle = mgFTLoadFont(fontFile, font->m_size, mgDisplay->getDPI(), 
                                height, ascent, charWidth);

  // GUI code still expects integer char sizes
  font->m_height = (int) ceil(height);
  font->m_ascent = (int) ceil(ascent);
  font->m_charWidth = (int) ceil(charWidth);
#endif

#ifdef SUPPORT_CANVASFONT
  double height, ascent, charWidth;
  font->m_handle = mgCanvasLoadFont(font->m_faceName, font->m_size, mgDisplay->getDPI(), 
                     font->m_bold, font->m_italic, height, ascent, charWidth);
  font->m_height = (int) ceil(height);
  font->m_ascent = (int) ceil(ascent);
  font->m_charWidth = (int) ceil(charWidth);
#endif
 
  font->m_id = m_nextFontId++;
}

//--------------------------------------------------------------------
// delete a font
void mgGLGenSurface::deleteFont(
  mgGenFont* font)
{
#ifdef SUPPORT_FTFONT
  // delete the FTFont
  mgFTDeleteFont(font->m_handle);
  font->m_handle = NULL;
#endif

#ifdef SUPPORT_CANVASFONT
  // delete the canvas font
  mgCanvasDeleteFont(font->m_handle);
#endif

}

//--------------------------------------------------------------------
// load image data, return size and handle
void* mgGLGenSurface::loadImage(
  mgGenImage* image)
{
  // =-= load the image as a texture
  return NULL;
}

//--------------------------------------------------------------------
// delete image data
void mgGLGenSurface::deleteImage(
  mgGenImage* image)
{
  // =-= delete image texture
}

//--------------------------------------------------------------------
// load icon data, return size and handle
void* mgGLGenSurface::loadIcon(
  mgGenIcon* icon)
{
  // =-= load the icon as a texture
  return NULL;
}

//--------------------------------------------------------------------
// delete icon data
void mgGLGenSurface::deleteIcon(
  mgGenIcon* icon)
{
  // =-= delete icon texture
}

//--------------------------------------------------------------------
// draw buffered graphics
void mgGLGenSurface::drawGraphics()
{
  mgPoint4 textColor(m_lastTextColor.m_r, m_lastTextColor.m_g, m_lastTextColor.m_b, m_lastTextColor.m_a);

  // for all tiles
  for (int i = 0; i < m_vertTiles; i++)
  {
    for (int j = 0; j < m_horzTiles; j++)
    {
      mgTextureImage* tile = m_tiles[i*m_horzTiles+j];
      mgRectangle bounds(j*SURFACE_TILE_SIZE, i*SURFACE_TILE_SIZE, SURFACE_TILE_SIZE, SURFACE_TILE_SIZE);

      // if clip rect does not intersect tile, continue
      mgRectangle clip(m_lastClip);
      clip.intersectRect(bounds);
      if (clip.isEmpty())
        continue;

      // =-= if graphics bounds does not intersect tile, continue
      // if tile is null, allocate it
      if (tile == NULL)
      {
        tile = mgDisplay->createTextureMemory(SURFACE_TILE_SIZE, SURFACE_TILE_SIZE,
                      MG_MEMORY_FORMAT_RGBA, m_inworld);
        m_tiles[i*m_horzTiles+j] = tile;
      }

      mgDisplay->renderToTexture(tile, false);
      glScissor(clip.m_x - bounds.m_x, clip.m_y - bounds.m_y, clip.m_width, clip.m_height);

      // set alpha mode 
      switch (m_lastAlphaMode)
      {
        case MG_ALPHA_SET: 
          glBlendFunc(GL_ONE, GL_ZERO);
          mgDisplay->setTransparent(false);
          break;

        case MG_ALPHA_MERGE: 
          glBlendFunc(GL_ONE, GL_ZERO);
          mgDisplay->setTransparent(false);
          break;

        case MG_ALPHA_RGB:
          glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                              GL_ZERO, GL_ONE);
          mgDisplay->setTransparent(true);
          break;
      }
      glDisable(GL_CULL_FACE);

      // draw solid triangles
      if (m_solidVertexes->getLength() > 0)
      {
        mgDisplay->setShader(m_solidShader);
        mgDisplay->setShaderUniform(m_solidShader, "mgSize", (float) tile->m_width, (float) tile->m_height);
        mgDisplay->setShaderUniform(m_solidShader, "mgOrigin", (float) -bounds.m_x, (float) -bounds.m_y);
        mgDisplay->draw(MG_TRIANGLES, m_solidVertexes, 0, m_solidVertexes->getLength());
      }

      // draw image triangles
      else if (m_imageVertexes->getLength() > 0)
      {
        mgDisplay->setShader(m_imageShader);
        mgDisplay->setTexture(m_lastImage);
        mgDisplay->setShaderUniform(m_imageShader, "mgSize", (float) tile->m_width, (float) tile->m_height);
        mgDisplay->setShaderUniform(m_imageShader, "mgOrigin", (float) -bounds.m_x, (float) -bounds.m_y);
        mgDisplay->draw(MG_TRIANGLES, m_imageVertexes, 0, m_imageVertexes->getLength());
      }

      // draw text triangles
      else if (m_textVertexes->getLength() > 0)
      {
        mgDisplay->setTexture(m_charTexture);
        mgDisplay->setTransparent(true);

        if (m_lastAlphaMode == MG_ALPHA_MERGE)
        {
          // first pass.  set RGB and half of alpha
          glBlendFuncSeparate(
            GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
            GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

          mgDisplay->setShader(m_textM1Shader);
          mgDisplay->setShaderUniform(m_textM1Shader, "mgSize", (float) tile->m_width, (float) tile->m_height);
          mgDisplay->setShaderUniform(m_textM1Shader, "mgOrigin", (float) -bounds.m_x, (float) -bounds.m_y);
          mgDisplay->setShaderUniform(m_textM1Shader, "textColor", textColor);
          mgDisplay->draw(MG_TRIANGLES, m_textVertexes, 0, m_textVertexes->getLength());

          // second pass.  set rest of alpha
          glBlendFuncSeparate(

            GL_ZERO, GL_ONE,    // leave the RGB we've built in pass 1
            GL_ONE, GL_ONE);    // add in the alpha from pass 2

          mgDisplay->setShader(m_textM2Shader);
          mgDisplay->setShaderUniform(m_textM2Shader, "mgSize", (float) tile->m_width, (float) tile->m_height);
          mgDisplay->setShaderUniform(m_textM2Shader, "mgOrigin", (float) -bounds.m_x, (float) -bounds.m_y);
          mgDisplay->setShaderUniform(m_textM2Shader, "textColor", textColor);
          mgDisplay->draw(MG_TRIANGLES, m_textVertexes, 0, m_textVertexes->getLength());
        }

        else
        {
          // MG_ALPHA_SET or MG_ALPHA_RGB
          mgDisplay->setShader(m_textShader);
          mgDisplay->setShaderUniform(m_textShader, "mgSize", (float) tile->m_width, (float) tile->m_height);
          mgDisplay->setShaderUniform(m_textShader, "mgOrigin", (float) -bounds.m_x, (float) -bounds.m_y);
          mgDisplay->setShaderUniform(m_textShader, "textColor", textColor);
          mgDisplay->draw(MG_TRIANGLES, m_textVertexes, 0, m_textVertexes->getLength());
        }
      }
    }
  }

  m_solidVertexes->reset();
  m_imageVertexes->reset();
  m_textVertexes->reset();

  mgDisplay->renderToDisplay();

  m_pendingGraphics = false;
}

//--------------------------------------------------------------------
// fill a triangle with color
void mgGLGenSurface::fillSolidTriangle(
  mgGenContextState* state,
  const mgColor& color,
  double ax,
  double ay,
  double bx,
  double by,
  double cx,
  double cy)
{
  if (m_pendingGraphics)
  {
    if (m_lastAlphaMode != state->m_alphaMode ||
        m_lastPrimitive != PRIM_SOLID ||
        !m_lastClip.equals(state->m_reqClip) ||
        m_solidVertexes->getLength() + 3 > OUTPUT_GRAPHICS_SIZE)
      drawGraphics();
  }

  m_lastPrimitive = PRIM_SOLID;
  m_lastAlphaMode = state->m_alphaMode;
  m_lastClip = state->m_reqClip;
  m_pendingGraphics = true;

  ax += state->m_originX;
  ay += state->m_originY;
  bx += state->m_originX;
  by += state->m_originY;
  cx += state->m_originX;
  cy += state->m_originY;

  // add solid rectangle to vertex buffer
  mgVertexOverlaySolid va, vb, vc;

  // set color on all vertexes
  va.m_r = (float) color.m_r; va.m_g = (float) color.m_g; va.m_b = (float) color.m_b; va.m_a = (float) color.m_a;
  vb.m_r = va.m_r; vb.m_g = va.m_g; vb.m_b = va.m_b; vb.m_a = va.m_a;
  vc.m_r = va.m_r; vc.m_g = va.m_g; vc.m_b = va.m_b; vc.m_a = va.m_a;

  va.m_px = (float) ax; va.m_py = (float) ay;
  vb.m_px = (float) bx; vb.m_py = (float) by;
  vc.m_px = (float) cx; vc.m_py = (float) cy;

  va.addTo(m_solidVertexes);
  vb.addTo(m_solidVertexes);
  vc.addTo(m_solidVertexes);
}

//--------------------------------------------------------------------
// fill a triangle with image data
void mgGLGenSurface::fillImageTriangle(
  mgGenContextState* state,
  void* imageHandle,
  double ax,
  double ay,
  double bx,
  double by,
  double cx,
  double cy)
{
}

//--------------------------------------------------------------------
// fill a rectangle with color
void mgGLGenSurface::fillSolidRectangle(
  mgGenContextState* state,
  const mgColor& color,
  double lx,
  double ly,
  double hx,
  double hy)
{
  if (m_pendingGraphics)
  {
    if (m_lastAlphaMode != state->m_alphaMode ||
        m_lastPrimitive != PRIM_SOLID ||
        !m_lastClip.equals(state->m_reqClip) ||
        m_solidVertexes->getLength() + 6 > OUTPUT_GRAPHICS_SIZE)
      drawGraphics();
  }

  m_lastPrimitive = PRIM_SOLID;
  m_lastAlphaMode = state->m_alphaMode;
  m_lastClip = state->m_reqClip;
  m_pendingGraphics = true;

  lx += state->m_originX;
  ly += state->m_originY;
  hx += state->m_originX;
  hy += state->m_originY;

  // add solid rectangle to vertex buffer
  mgVertexOverlaySolid tl, tr, bl, br;

  // set color on all vertexes
  tl.m_r = (float) color.m_r; tl.m_g = (float) color.m_g; tl.m_b = (float) color.m_b; tl.m_a = (float) color.m_a;
  tr.m_r = tl.m_r; tr.m_g = tl.m_g; tr.m_b = tl.m_b; tr.m_a = tl.m_a;
  bl.m_r = tl.m_r; bl.m_g = tl.m_g; bl.m_b = tl.m_b; bl.m_a = tl.m_a;
  br.m_r = tl.m_r; br.m_g = tl.m_g; br.m_b = tl.m_b; br.m_a = tl.m_a;

  tl.m_px = (float) lx; tl.m_py = (float) ly;
  tr.m_px = (float) hx; tr.m_py = (float) ly;
  bl.m_px = (float) lx; bl.m_py = (float) hy;
  br.m_px = (float) hx; br.m_py = (float) hy;

  tl.addTo(m_solidVertexes);
  tr.addTo(m_solidVertexes);
  bl.addTo(m_solidVertexes);

  bl.addTo(m_solidVertexes);
  tr.addTo(m_solidVertexes);
  br.addTo(m_solidVertexes);
}

//--------------------------------------------------------------------
// fill a rectangle with image data
void mgGLGenSurface::fillImageRectangle(
  mgGenContextState* state,
  void* imageHandle,
  double dx,
  double dy,
  double dwd,
  double dht,
  double sx,
  double sy,
  double swd,
  double sht)
{
  // if alphamode or primitive changed, flush output
  // if buffer full, flush output

  // add image rectangle to vertex buffer
}

//--------------------------------------------------------------------
// get a glyph and add it to the char texture
mgCharDesc* mgGLGenSurface::getChar(
  const mgGenFont* font,
  int letter,
  BOOL& resize)
{
  resize = false;

  // find the letter in the font texture
  const void* value;
  DWORD charKey = (font->m_id << CHAR_KEY_FONT_SHIFT) | letter;
  if (m_charMap.lookup(charKey, value))
    return (mgCharDesc*) value;

  double advanceX, advanceY, posnX, posnY;
  int imageWidth, imageHeight;
  BYTE* imageData = NULL;
#ifdef SUPPORT_FTFONT
  mgFTGetChar(font->m_handle, letter, 
    advanceX, advanceY, posnX, posnY, imageWidth, imageHeight, imageData);
#endif

#ifdef SUPPORT_CANVASFONT
  mgCanvasGetChar(font->m_handle, letter, 
    advanceX, advanceY, posnX, posnY, imageWidth, imageHeight, imageData);
#endif

  // round output width to next multiple of 4 bytes
  int outWidth = 4*((imageWidth+3)/4);

  // find position for letter in char texture
  int imageY = m_charHeightUsed;
  int imageX = m_charRowWidth;
  if (imageX + outWidth+1 > m_charTexture->m_width)
  {
    // start on next row of char texture
    m_charHeightUsed += m_charRowHeight;
    m_charRowHeight = 0;
    m_charRowWidth = 0;
    // if the letter won't fit, set flag and return NULL
    if (outWidth+1 > m_charTexture->m_width ||
        m_charHeightUsed + imageHeight+1 > m_charTexture->m_height)
    {
      resize = true;
      return NULL;
    }
    imageY = m_charHeightUsed;
    imageX = m_charRowWidth;
  }

  // add the character to the row.  leave a 1-pixel buffer between chars
  imageX++;
  imageY++;
  m_charRowHeight = max(m_charRowHeight, imageHeight+2);
  m_charRowWidth += imageWidth+2;

  // copy character into texture
  if (imageData != NULL)
    m_charTexture->updateMemory(imageX, imageY, outWidth, imageHeight, imageData);

  mgCharDesc* desc = new mgCharDesc;
  desc->m_advanceX = advanceX;
  desc->m_advanceY = advanceY;
  desc->m_posnX = posnX;
  desc->m_posnY = posnY;
  desc->m_imageX = imageX;
  desc->m_imageY = imageY;
  desc->m_imageWidth = imageWidth;
  desc->m_imageHeight = imageHeight;

  // add to the char table
  m_charMap.setAt(charKey, desc);

  return desc;
}

//--------------------------------------------------------------------
// reset the character map
void mgGLGenSurface::resetCharMap()
{
  // clear the map of cached char patterns
  int posn = m_charMap.getStartPosition();
  while (posn != -1)
  {
    DWORD key;
    const void* value;
    m_charMap.getNextAssoc(posn, key, value);
    delete (mgCharDesc*) value;
  }
  m_charMap.removeAll();
}

//--------------------------------------------------------------------
// empty and resize the char texture
void mgGLGenSurface::resizeCharTexture()
{
  resetCharMap();

  // figure new size of bitmap
  int newWidth = m_charTexture->m_width;
  int newHeight = m_charTexture->m_height;

  if (newWidth < 1024)
    newWidth *= 2;
  else if (newHeight < 1024)
    newHeight *= 2;
  else return; // can't grow, just clear cache

  delete m_charTexture;
  m_charHeightUsed = 0;
  m_charRowWidth = 0;
  m_charRowHeight = 0;

  // reallocate char cache texture
  m_charTexture = mgDisplay->createTextureMemory(newWidth, newHeight, MG_MEMORY_FORMAT_GRAY, false);
mgDebug("char texture resized to %d by %d", newWidth, newHeight);
}

//--------------------------------------------------------------------
// draw text in font with color
void mgGLGenSurface::drawString(
  mgGenContextState* state,
  const char* text,
  int textLen,
  double x,
  double y)
{
  if (m_pendingGraphics)
  {
    if (m_lastAlphaMode != state->m_alphaMode ||
        m_lastPrimitive != PRIM_TEXT ||
        !state->m_textColor.equals(m_lastTextColor) ||
        !m_lastClip.equals(state->m_reqClip) ||
        m_textVertexes->getLength() + 6*textLen > OUTPUT_GRAPHICS_SIZE)
      drawGraphics();
  }

  m_lastPrimitive = PRIM_TEXT;
  m_lastAlphaMode = state->m_alphaMode;
  m_lastClip = state->m_reqClip;
  m_lastTextColor = state->m_textColor;
  m_pendingGraphics = true;

  mgVertexOverlayText tl, tr, bl, br;
  float textureWidth = (float) m_charTexture->m_width;
  float textureHeight = (float) m_charTexture->m_height;

  x += state->m_originX;
  y += state->m_originY;

  // for each char in text
  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    // get letter info
    BOOL resize;
    mgCharDesc* desc = getChar(state->m_font, letter, resize);
    if (resize)
    {
      // force out current string
      drawGraphics();   
      m_pendingGraphics = true;

      // clear the cache and enlarge the texture
      resizeCharTexture(); 
      textureWidth = (float) m_charTexture->m_width;
      textureHeight = (float) m_charTexture->m_height;

      desc = getChar(state->m_font, letter, resize);
      if (resize)
        continue;  // skip char
    }
    if (desc == NULL)
      continue;

    // draw letter from image
    tl.m_px = (float) (x+desc->m_posnX); tl.m_py = (float) (y-desc->m_posnY);
    tl.m_tx = desc->m_imageX/textureWidth; tl.m_ty = desc->m_imageY/textureHeight;

    tr.m_px = (float) (x+desc->m_posnX+desc->m_imageWidth); tr.m_py = (float) (y-desc->m_posnY);
    tr.m_tx = (desc->m_imageX+desc->m_imageWidth)/textureWidth; tr.m_ty = tl.m_ty;

    bl.m_px = tl.m_px; bl.m_py = (float) (y-desc->m_posnY+desc->m_imageHeight);
    bl.m_tx = tl.m_tx; bl.m_ty = (desc->m_imageY+desc->m_imageHeight)/textureHeight;

    br.m_px = tr.m_px; br.m_py = bl.m_py;
    br.m_tx = tr.m_tx; br.m_ty = bl.m_ty;

    tl.addTo(m_textVertexes);
    tr.addTo(m_textVertexes);
    bl.addTo(m_textVertexes);

    bl.addTo(m_textVertexes);
    tr.addTo(m_textVertexes);
    br.addTo(m_textVertexes);

    // we have to align chars on pixel boundaries or they distort
    x += ceil(desc->m_advanceX);
    y += ceil(desc->m_advanceY);
  }
}

//--------------------------------------------------------------------
// output graphics
void mgGLGenSurface::flush()
{
  drawGraphics();
}

//--------------------------------------------------------------------
// measure a string
void mgGLGenSurface::stringExtent(
  const mgGenFont* font,
  const char* text,
  int textLen,
  mgPoint& endPt,
  mgRectangle& bounds)
{
  double x = 0;
  double y = 0;
  double lx, ly, hx, hy;
  lx = ly = INT_MAX;
  hx = hy = 0;

  // for each char in text
  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    // get letter info
    BOOL resize;
    mgCharDesc* desc = getChar(font, letter, resize);
    if (resize)
    {
      // clear the cache and enlarge the texture
      resizeCharTexture(); 
      desc = getChar(font, letter, resize);
      if (resize)
        continue;  // skip char
    }
    if (desc == NULL)
      continue;

    // we have to align chars on pixel boundaries or they distort
    x += ceil(desc->m_advanceX);
    y += ceil(desc->m_advanceY);

    lx = min(lx, x+desc->m_posnX);
    ly = min(ly, y-desc->m_posnY);
    hx = max(hx, x+desc->m_posnX+desc->m_imageWidth);
    hy = max(hy, y-desc->m_posnY+desc->m_imageHeight);
  }
  endPt.m_x = (int) x;
  endPt.m_y = (int) y;

  bounds.m_x = (int) lx;
  bounds.m_y = (int) ly;
  bounds.m_width = (int) ceil(hx) - bounds.m_x;
  bounds.m_height = (int) ceil(hy) - bounds.m_y;
}

//--------------------------------------------------------------------
// measure a string (total advance)
int mgGLGenSurface::stringWidth(
  const mgGenFont* font,
  const char* text,
  int textLen)
{
  double x = 0;

  // for each char in text
  int textPosn = 0;
  while (textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    // get letter info
    BOOL resize;
    mgCharDesc* desc = getChar(font, letter, resize);
    if (resize)
    {
      // clear the cache and enlarge the texture
      resizeCharTexture(); 
      desc = getChar(font, letter, resize);
      if (resize)
        continue;  // skip char
    }
    if (desc == NULL)
      continue;

    // we have to align chars on pixel boundaries or they distort
    x += ceil(desc->m_advanceX);
  }
  return (int) x;
}

//--------------------------------------------------------------------
// return number of chars which fit in width
int mgGLGenSurface::stringFit(
  const mgGenFont* font,
  const char* text,
  int textLen,
  int width)
{
  double x = 0;

  // for each char in text
  int textPosn = 0;
  while (x < width && textPosn < textLen)
  {
    int utfCount;
    int letter = mgString::fromUTF8(text+textPosn, utfCount);
    textPosn += utfCount;

    // get letter info
    BOOL resize;
    mgCharDesc* desc = getChar(font, letter, resize);
    if (resize)
    {
      // clear the cache and enlarge the texture
      resizeCharTexture(); 
      desc = getChar(font, letter, resize);
      if (resize)
        continue;  // skip char
    }
    if (desc == NULL)
      continue;

    // we have to align chars on pixel boundaries or they distort
    x += ceil(desc->m_advanceX);
  }
  return textPosn;
}

//--------------------------------------------------------------
// set dimensions of surface
void mgGLGenSurface::setSurfaceSize(
  int width,
  int height)
{
  mgGenSurface::setSurfaceSize(width, height);

  if (m_singleTile)
    resizeSingleImage(width, height);
  else resizeTiledImage(width, height);
}

//--------------------------------------------------------------
// set dimensions of image
void mgGLGenSurface::resizeSingleImage(
  int width,
  int height)
{
  deleteTiles();

  m_horzTiles = 1;
  m_vertTiles = 1;

  // create a single tile of requested size.  
  // =-= should be power of two for devices that require that.
  m_tiles = new mgTextureImage*[1];
  m_tiles[0] = mgDisplay->createTextureMemory(width, height,
                      MG_MEMORY_FORMAT_RGBA, m_inworld);

  mgDisplay->renderToTexture(m_tiles[0], false);
  glScissor(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
  glClear(GL_COLOR_BUFFER_BIT);
}

//--------------------------------------------------------------
// resize a tiled image
void mgGLGenSurface::resizeTiledImage(
  int width,
  int height)
{
  // figure number of tiles required
  int newHorzTiles = (width+SURFACE_TILE_SIZE-1)/SURFACE_TILE_SIZE;
  int newVertTiles = (height+SURFACE_TILE_SIZE-1)/SURFACE_TILE_SIZE;
  
  // if same number of tiles
  if (m_horzTiles * m_vertTiles == newHorzTiles * newVertTiles)
  {
    // adjust dimensions of tile array
    m_horzTiles = newHorzTiles;
    m_vertTiles = newVertTiles;
    return;
  }

  int tileCount = newHorzTiles*newVertTiles;
  int oldTileCount = m_horzTiles * m_vertTiles;

  // reallocate the tile array
  mgTextureImage** newTiles = new mgTextureImage*[tileCount];
  
  // copy any old tiles we can reuse
  for (int i = 0; i < min(tileCount, oldTileCount); i++)
  {
    newTiles[i] = m_tiles[i];
  }
  
  // initialize additional new tiles
  for (int i = oldTileCount; i < tileCount; i++)
  {
    newTiles[i] = mgDisplay->createTextureMemory(SURFACE_TILE_SIZE, SURFACE_TILE_SIZE,
                      MG_MEMORY_FORMAT_RGBA, m_inworld);
  }
    
  // delete unused old tiles
  for (int i = tileCount; i < oldTileCount; i++)
  {
    delete m_tiles[i];
  }
  
  // switch to new tiles
  delete m_tiles;
  m_tiles = NULL;

  m_tiles = newTiles;
  m_horzTiles = newHorzTiles;
  m_vertTiles = newVertTiles;

  // clear all the tiles
  for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
  {
    mgDisplay->renderToTexture(m_tiles[i], false);
    glScissor(0, 0, SURFACE_TILE_SIZE, SURFACE_TILE_SIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

//--------------------------------------------------------------
// delete all the tiles
void mgGLGenSurface::deleteTiles()
{
  if (m_tiles == NULL)
    return;

  for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
  {
    delete m_tiles[i];
    m_tiles[i] = NULL;
  }

  delete m_tiles;
  m_tiles = NULL;
}

//--------------------------------------------------------------------
// create buffers, ready to send to display
void mgGLGenSurface::createBuffers()
{
  if (m_tiles == NULL)
    return;
    
  if (m_singleTile)
  {
    if (m_tiles[0] == NULL)
    {
      m_tiles[0] = mgDisplay->createTextureMemory(m_surfaceWidth, m_surfaceHeight, MG_MEMORY_FORMAT_RGBA, m_inworld);

      mgDisplay->renderToTexture(m_tiles[0], false);
      glScissor(0, 0, m_surfaceWidth, m_surfaceHeight);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
      glClear(GL_COLOR_BUFFER_BIT);
    }
  }
  else
  {
    for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
    {
      if (m_tiles[i] == NULL)
        m_tiles[i] = mgDisplay->createTextureMemory(SURFACE_TILE_SIZE, SURFACE_TILE_SIZE, MG_MEMORY_FORMAT_RGBA, m_inworld);
    }

    // clear all the tiles
    for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
    {
      mgDisplay->renderToTexture(m_tiles[i], false);
      glScissor(0, 0, SURFACE_TILE_SIZE, SURFACE_TILE_SIZE);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f );
      glClear(GL_COLOR_BUFFER_BIT);
    }
  }
}

//--------------------------------------------------------------------
// delete any display buffers
void mgGLGenSurface::deleteBuffers()
{
  if (m_tiles == NULL)
    return;

  for (int i = 0; i < m_vertTiles * m_horzTiles; i++)
  {
    delete m_tiles[i];
    m_tiles[i] = NULL;
  }
}

//--------------------------------------------------------------------
// draw the surface as an overlay
void mgGLGenSurface::drawOverlay(
  int x,
  int y) const
{
  if (m_singleTile)
  {
    mgDisplay->drawOverlayTexture(m_tiles[0], x, y, m_surfaceWidth, m_surfaceHeight);
    return;
  }

  // handle multi-tile case
  for (int i = 0; i < m_vertTiles; i++)
  {
    int ty = SURFACE_TILE_SIZE * i;
    for (int j = 0; j < m_horzTiles; j++)
    {
      int tx = SURFACE_TILE_SIZE * j;
      mgTextureImage* tile = m_tiles[i*m_horzTiles+j];
      mgDisplay->drawOverlayTexture(tile, x+tx, y+ty, tile->m_width, tile->m_height);
    }
  }
}

//--------------------------------------------------------------------
// repair damage
void mgGLGenSurface::repair(
  mgRectangle& bounds)
{
  if (m_pendingGraphics)
    drawGraphics();
  mgGenSurface::repair(bounds);

  mgDisplay->renderToDisplay();
}

