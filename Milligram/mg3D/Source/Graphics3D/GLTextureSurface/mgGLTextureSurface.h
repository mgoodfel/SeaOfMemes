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
#ifndef MGGLTEXTURESURFACE
#define MGGLTEXTURESURFACE

/*
  An mgSurface subclass, the texture is dynamic and implements the mg2D graphics model.
  All overlay GUI and other graphics is generally done with this class.
*/

class mgGLGenSurface;

class mgGLTextureSurface : public mgTextureSurface
{
public:
  // constructor
  mgGLTextureSurface(
    BOOL singleTile,
    BOOL inworld);

  // destructor
  virtual ~mgGLTextureSurface();

  // return the rendering surface
  virtual mgSurface* getSurface();

  // return texture containing surface data
  virtual mgTextureImage* getTexture();

  // create buffers, ready to send to display
  virtual void createBuffers();
  
  // delete any display buffers
  virtual void deleteBuffers();
  
  // draw the surface as an overlay
  virtual void drawOverlay(
    int x,
    int y) const;

protected:
  mgGLGenSurface* m_surface;
};

#endif

