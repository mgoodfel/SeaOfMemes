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
#ifndef BRICKSET_H
#define BRICKSET_H

const int SHAPE_CUBE      = 0;
const int SHAPE_SLAB      = 1;
const int SHAPE_CAP       = 2;
const int SHAPE_STAIR     = 3;
const int SHAPE_COLUMN    = 4;
const int SHAPE_DEFN      = 5;

const int BRICK_FACE_XMIN = 0;
const int BRICK_FACE_XMAX = 1;
const int BRICK_FACE_YMIN = 2;
const int BRICK_FACE_YMAX = 3;
const int BRICK_FACE_ZMIN = 4;
const int BRICK_FACE_ZMAX = 5;

const int DIRECTION_COUNT = 4;
const int DEFN_COUNT = 256;
const int SUBDEFN_COUNT = 16;

const int MAX_BRICK_TEXTURES = 1024;  // limited by shader interface for int shaders

// textures for a brick type
class BrickDefn;
class BrickSubDefn;
class ShapeDefn;

class BrickSet
{
public:
  mgMatrix4 m_directions[DIRECTION_COUNT];
  BrickDefn* m_defns[DEFN_COUNT];

  // constructor
  BrickSet();

  // destructor
  virtual ~BrickSet();

protected:
  // build direction matricies
  void buildDirections();
};

class BrickDefn
{
public:
  int m_shape;

  // transparent flags for each face
  BOOL m_trans[6];
  
  // summary flag for transparency
  BOOL m_hasOpaque;
  BOOL m_hasTransparent;

  // subtypes based on second byte of brick id
  BrickSubDefn* m_subDefns[SUBDEFN_COUNT];

  // constructor
  BrickDefn();

  // destructor
  virtual ~BrickDefn();
};

// subdefinition for brick type
class BrickSubDefn
{
public:
  ShapeDefn* m_shapeDefn;

  // orientation
  int m_dir;

  BOOL m_light;

  int m_xmin;
  int m_xmax;
  int m_ymin;
  int m_ymax;
  int m_zmin;
  int m_zmax;

  // constructor
  BrickSubDefn();

  // destructor
  virtual ~BrickSubDefn();
};

#endif
