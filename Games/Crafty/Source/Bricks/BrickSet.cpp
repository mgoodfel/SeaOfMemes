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

#include "BrickSet.h"

//--------------------------------------------------------------
// constructor
BrickSet::BrickSet()
{
  for (int i = 0; i < DEFN_COUNT; i++)
    m_defns[i] = NULL;

  // build the direction matricies
  buildDirections();
}

//--------------------------------------------------------------
// destructor
BrickSet::~BrickSet()
{
  for (int i = 0; i < DEFN_COUNT; i++)
  {
    delete m_defns[i];
    m_defns[i] = NULL;
  }
}

//--------------------------------------------------------------
// build the direction matricies
void BrickSet::buildDirections()
{
  // build the first four directions.  rotate around center of cube
  for (int i = 1; i < 4; i++)
  {
    m_directions[i].translate(-0.5, 0, -0.5);
    m_directions[i].rotateYDeg(90*i);
    m_directions[i].translate(0.5, 0, 0.5);
  }
}

//--------------------------------------------------------------
// constructor
BrickDefn::BrickDefn()
{
  m_hasOpaque = false;
  m_hasTransparent = false;

  for (int i = 0; i < SUBDEFN_COUNT; i++)
    m_subDefns[i] = NULL;
}

//--------------------------------------------------------------
// destructor
BrickDefn::~BrickDefn() 
{
  for (int i = 0; i < SUBDEFN_COUNT; i++)
  {
    delete m_subDefns[i];
    m_subDefns[i] = NULL;
  }
}

//--------------------------------------------------------------
// constructor
BrickSubDefn::BrickSubDefn()
{
  m_xmin = m_xmax = -1;
  m_ymin = m_ymax = -1;
  m_zmin = m_zmax = -1;
  m_shapeDefn = NULL;
  m_light = false;
}

//--------------------------------------------------------------
// destructor
BrickSubDefn::~BrickSubDefn() 
{
}
