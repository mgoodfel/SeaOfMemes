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
#if !defined(MGDX9SERVICES_H) && defined(SUPPORT_DX9)
#define MGDX9SERVICES_H

#include "mgDisplayServices.h"

class mgDX9TextureImage;
class mgDX9TextureArray;
class mgDX9State;

// services offered by the display framework to the application
class mgDX9Services : public mgDisplayServices
{ 
public:
  //----------------------- rendering methods ------------------

  // can the display repeat textures
  virtual BOOL canRepeatTextures();

  // allocate an index array
  virtual mgIndexBuffer* newIndexBuffer(
    int size,
    BOOL dynamic);

  // draw a texture to the overlay
  virtual void drawOverlayTexture(
    const mgTextureImage* texture,
    int x,
    int y,
    int width,
    int height);

  // create a 2D graphics surface
  virtual mgSurface* createOverlaySurface();

  // create a 2D graphics surface
  virtual mgSurface* createTextureSurface();

  //----------------------- rendering state ------------------

  // set light direction
  virtual void setLightDir(
    double x,
    double y,
    double z);

  // set light ambient
  virtual void setLightAmbient(
    double r,
    double g,
    double b);

  // set light color
  virtual void setLightColor(
    double r,
    double g,
    double b);

  // set material color
  virtual void setMatColor(
    double r,
    double g,
    double b);

  // set model transform
  virtual void setModelTransform(
    mgMatrix4& xform);

  // turn lighting on 
  virtual void setLighting(
    BOOL enable);

  // set culling 
  virtual void setCulling(
    BOOL enable);

  // set zenable
  virtual void setZEnable(
    BOOL enable);

  // set transparent
  virtual void setTransparent(
    BOOL enable);

  // set cursor texture
  virtual void setCursorTexture(
    const char* fileName,
    int hotX,
    int hotY);

  // save screen to file
  virtual void screenShot(
    const char* fileName);

  // exit the application
  virtual void exitApp();

  //----------------------- options from profile, set at initialization ------------------

  // enable graphics timing with immediate mode
  virtual void setGraphicsTiming(
    BOOL enabled);

  // set initial window position
  virtual void setWindowPosn(
    int x,
    int y,
    int width,
    int height);

  // set multisample flag
  virtual void setMultiSample(
    BOOL multiSample);

  //----------------------- mgPlatform only methods ------------------
  // we can't just make mgPlatform a friend, and make these protected,
  // since it's an unknown subclass of mgPlatform that calls these methods.

  // constructor
  mgDX9Services();

  // destructor
  virtual ~mgDX9Services();

  // initialize graphics
  virtual void initGraphics();

  // terminate graphics
  virtual void termGraphics();

  // initialize rendering context
  virtual void initGraphicsContext();

  // terminate rendering context
  virtual void termGraphicsContext();

  // release device resources 
  virtual void deleteBuffers();

  // reclaim device resources
  virtual void createBuffers();

  // reset the display
  virtual void displayReset();

  // toggle fullscreen mode
  virtual void toggleFullscreen();

  // initialize view 
  virtual void initView();

  // set projection matricies
  virtual void setProjection();

  // animate the view
  virtual BOOL animateView(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // draw the view
  virtual void drawView();

  // update graphics in window
  virtual void swapBuffers();

  // set mouse mode
  virtual void setMouseRelative(
    BOOL relative);

protected:
  mgDX9State* m_state;

  BOOL m_deviceActive;
  BOOL m_deviceResized;
  mgDX9TextureImage* m_cursorTexture;
  BOOL m_lighting;

  // draw the cursor
  virtual void drawCursor();

  // set up light and fog
  void setLightAndFog();
};

#endif
