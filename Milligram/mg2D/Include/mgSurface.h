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
#ifndef MGSURFACE_H
#define MGSURFACE_H

#include "mgTypes2D.h"

class mgContext;

/*
  The superclass of all surfaces.  Creates and holds various resources
  (applications can add to the resource list), create an mgContext
  instance for drawing, and handles damage and repair to the surface
  graphics.
*/

class mgSurface
{
public:
  // create a drawing context
  virtual mgContext* newContext() = 0;

  // convert points to pixels on this device
  virtual int points(
    double ptSize) const = 0;
      
  // create font
  virtual const mgFont* createFont(
    const char* faceName,
    int size,
    BOOL bold,
    BOOL italic) = 0;

  // create font from "arial-10-b-i" style fontspec
  virtual const mgFont* createFont(
    const char* fontSpec) = 0;

  // create a brush
  virtual const mgBrush* createBrush(
    const mgColor& color) = 0;
    
  // create a brush
  virtual const mgBrush* createBrush(
    double r, 
    double g, 
    double b,
    double a = 1.0) = 0;
    
  // create a brush
  virtual const mgBrush* createBrush(
    const char* colorSpec) = 0;

  // create pen from color
  virtual const mgPen* createPen(
    double thick, 
    const mgColor& color) = 0;

  // create named color pen
  virtual const mgPen* createPen(
    double thick,
    const char* colorSpec) = 0;

  // create pen from color
  virtual const mgPen* createPen(
    double thick,
    double r, 
    double g, 
    double b,
    double a = 1.0) = 0;
    
  // create an image
  virtual const mgImage* createImage(
    const char* fileName) = 0;

  // create an icon
  virtual const mgIcon* createIcon(
    const char* fileName) = 0;

  // save a resource
  virtual void saveResource(
    const mgResource* data) = 0;

  // find a resource
  virtual mgResource* findResource(
    const char* name) const = 0;

  // remove a resource
  virtual void removeResource(
    const mgResource* data) = 0;

  // damage entire bitmap
  virtual void damageAll() = 0;

  // damage a rectangle 
  virtual void damage(
    int x,
    int y,
    int width,
    int height) = 0;

  // get damage bounds
  virtual void getDamage(
    mgRectangle& bounds) const = 0;
  
  virtual BOOL isDamaged() const = 0;

  // repair damage
  virtual void repair(
    mgRectangle& bounds) = 0;

  // set dimensions of surface
  virtual void setSurfaceSize(
    int width,
    int height) = 0;

  // get dimensions of surface
  virtual void getSurfaceSize(
    int& width,
    int& height) const = 0;
};

#endif
