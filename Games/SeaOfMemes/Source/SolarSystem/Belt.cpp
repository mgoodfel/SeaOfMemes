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

#include "SolarSystem.h"
#include "Habitat.h"
#include "Belt.h"

class Rock
{
public:
  mgPoint3 m_center;
  double m_radius;

  // constructor
  Rock(
    const mgPoint3& center,
    double radius)
  {
    m_center = center;
    m_radius = radius;
  }
};

const double OUTSIDE_RADIUS = BELT_RADIUS/SYSTEM_FAR_SCALE;
const double INSIDE_RADIUS  = 0.5*OUTSIDE_RADIUS;

const int MAX_VISIBLE_ROCKS = 500;
const double ROCK_INTERVAL = 70.0*1000.0 / SYSTEM_FAR_SCALE;
const int MEDIUM_DIST = 20;  // times ROCK_INTERVAL

const double ROCK_LIMIT = 0.65;
const double NOISE_SCALE = 2048.0;
const double ROCK_SIZE = 5000.0 / SYSTEM_FAR_SCALE;

//--------------------------------------------------------------
// constructor
Belt::Belt(
  const mgOptionsFile& options,
  double radius)
{
  mgVertex::loadShader("unlitTexture");

  m_radius = radius / SYSTEM_FAR_SCALE;

  mgString fileName;
  options.getFileName("belt", options.m_sourceFileName, "", fileName);
  m_farTexture = mgDisplay->loadTexture(fileName);

  m_farVertexes = NULL;
  m_farIndexes = NULL;

  options.getFileName("rockTexture", options.m_sourceFileName, "", fileName);
  m_rockTexture = mgDisplay->loadTexture(fileName);

  // make last eye point invalid so that animate fills in asteroids around eye
  m_lastEyePt = mgPoint3(INT_MAX, INT_MAX, INT_MAX);

//#define COUNT_ASTEROIDS
#ifdef COUNT_ASTEROIDS
  int count = 0;
  int width = 1+(int) floor(OUTSIDE_RADIUS / ROCK_INTERVAL);
  double minRadius = INT_MAX;
  double maxRadius = 0.0;

  const int SIZE_COUNT = 100;
  int sizeBins[SIZE_COUNT];
  for (int i = 0; i < SIZE_COUNT; i++)
    sizeBins[i] = 0;

  for (int x = -width; x <= width; x++)
  {
    for (int y = -100; y <= 100; y++)
    {
      for (int z = -width; z <= width; z++)
      {
        mgPoint3 center(x*ROCK_INTERVAL, y*ROCK_INTERVAL, z*ROCK_INTERVAL);
        double radius;
        if (asteroidPresent(center.x, center.y, center.z, radius))
        {
          count++;
          minRadius = min(minRadius, radius);
          maxRadius = max(maxRadius, radius);
          int size = (int) floor(10*radius);
          size = max(0,min(SIZE_COUNT-1, size));
          sizeBins[size]++;
        }
      }
    }
  }
  mgDebug("min radius = %g, max radius = %g", minRadius, maxRadius);
  mgDebug("%d asteroids", count);
  for (int i = 0; i < SIZE_COUNT; i++)
  {
    mgDebug("%5.1f %5d", i/10.0, sizeBins[i]);
  }
#endif
}

//--------------------------------------------------------------
// destructor
Belt::~Belt()
{
  deleteBuffers();

  for (int i = 0; i < m_rocks.length(); i++)
  {
    Rock* rock = (Rock*) m_rocks[i];
    delete rock;
  }
  m_rocks.removeAll();
}
    
//--------------------------------------------------------------
// create buffer for distant belt
void Belt::createFarBelt()
{
  m_farVertexes = mgVertex::newBuffer(4);
  m_farIndexes = mgDisplay->newIndexBuffer(6);

  mgVertex v;
  v.m_nx = 0.0f;
  v.m_ny = 1.0f;
  v.m_nz = 0.0f;

  // top left
  v.m_tx = 0.0f;
  v.m_ty = 1.0f;

  v.m_px = (float) -m_radius;
  v.m_py = 0.0f;
  v.m_pz = (float) m_radius;

  v.addTo(m_farVertexes);

  // top right
  v.m_tx = 1.0f;
  v.m_ty = 1.0f;

  v.m_px = (float) m_radius;
  v.m_py = 0.0f;
  v.m_pz = (float) m_radius;

  v.addTo(m_farVertexes);

  // bottom left
  v.m_tx = 0.0f;
  v.m_ty = 0.0f;

  v.m_px = (float) -m_radius;
  v.m_py = 0.0f;
  v.m_pz = (float) -m_radius;

  v.addTo(m_farVertexes);

  // bottom right
  v.m_tx = 1.0f;
  v.m_ty = 0.0f;

  v.m_px = (float) m_radius;
  v.m_py = 0.0f;
  v.m_pz = (float) -m_radius;

  v.addTo(m_farVertexes);

  m_farIndexes->addRectIndex(0);
}

//--------------------------------------------------------------
// create buffer for medium belt asteroids
void Belt::addMediumRock(
  const Rock* rock)
{
  int index = m_mediumVertexes->getLength();

  mgVertex v;
  v.m_nx = 0.0f;
  v.m_ny = 1.0f;
  v.m_nz = 0.0f;

  // top left
  v.m_tx = 0.0f;
  v.m_ty = 1.0f;

  v.m_px = (float) (rock->m_center.x -rock->m_radius);
  v.m_py = (float) rock->m_center.y;
  v.m_pz = (float) (rock->m_center.z + rock->m_radius);

  v.addTo(m_mediumVertexes);

  // top right
  v.m_tx = 1.0f;
  v.m_ty = 1.0f;

  v.m_px = (float) (rock->m_center.x + rock->m_radius);
  v.m_py = (float) rock->m_center.y;
  v.m_pz = (float) (rock->m_center.z + rock->m_radius);

  v.addTo(m_mediumVertexes);

  // bottom left
  v.m_tx = 0.0f;
  v.m_ty = 0.0f;

  v.m_px = (float) (rock->m_center.x - rock->m_radius);
  v.m_py = (float) rock->m_center.y;
  v.m_pz = (float) (rock->m_center.z - rock->m_radius);

  v.addTo(m_mediumVertexes);

  // bottom right
  v.m_tx = 1.0f;
  v.m_ty = 0.0f;

  v.m_px = (float) (rock->m_center.x + rock->m_radius);
  v.m_py = (float) rock->m_center.y;
  v.m_pz = (float) (rock->m_center.z - rock->m_radius);

  v.addTo(m_mediumVertexes);

  m_mediumIndexes->addRectIndex(index);
}

//--------------------------------------------------------------
// create display buffers
void Belt::createBuffers()
{          
  createFarBelt();

  m_mediumVertexes = mgVertex::newBuffer(4*MAX_VISIBLE_ROCKS, true);
  m_mediumIndexes = mgDisplay->newIndexBuffer(6*MAX_VISIBLE_ROCKS, true);

  // fill medium buffer with distant visible rocks
  for (int i = 0; i < m_rocks.length(); i++)
  {
    Rock* rock = (Rock*) m_rocks[i];
    addMediumRock(rock);
  }
}

//--------------------------------------------------------------
// delete display buffers
void Belt::deleteBuffers()
{
  delete m_farVertexes;
  m_farVertexes = NULL;

  delete m_farIndexes;
  m_farIndexes = NULL;

  delete m_mediumVertexes;
  m_mediumVertexes = NULL;

  delete m_mediumIndexes;
  m_mediumIndexes = NULL;
}

//--------------------------------------------------------------
// return true if asteroid present at location
BOOL Belt::asteroidPresent(
  double x,
  double y,
  double z,
  double& radius)
{
  x *= NOISE_SCALE/OUTSIDE_RADIUS;
  y *= NOISE_SCALE/OUTSIDE_RADIUS;
  z *= NOISE_SCALE/OUTSIDE_RADIUS;
  double ringDist = sqrt(x*x + z*z) / NOISE_SCALE;
  double density = 0.0;
  if (ringDist > (INSIDE_RADIUS/OUTSIDE_RADIUS) && ringDist < 1.0) 
  {
    density = mgSimplexNoise::noise(ringDist*20, 0.0);
    density = 0.6 + 0.1 * (1.0+density);
  }
  double thickness = pow(0.90, 25*abs(y/NOISE_SCALE));
  double cutoff = mgSimplexNoise::noise(x, y, z);
  cutoff *= thickness * density;
  cutoff = 0.8* (cutoff+1)/2.0;
  if (cutoff > ROCK_LIMIT)
  {
    // pick a radius from some unrelated area of noise
    radius = mgSimplexNoise::noise(2*x, 2*y, 2*z); // -1 to 1
//    radius = 2.0*asin(radius)/PI;   // -1 to 1
    radius = (radius+1.0)/2.0;  // 0 to 1
    radius = 10000.0 * radius;
    return true;
  }
  else return false;
}

int m_testedPoints;
int m_addedRocks;

//--------------------------------------------------------------
// check a volume of space for new asteroids
BOOL Belt::checkVolume(
  int lx, 
  int ly,
  int lz,
  int hx, 
  int hy,
  int hz)
{
//  if (lx <= hx && ly <= hy && lz <= hz)
//    mgDebug("check (%d, %d, %d) to (%d, %d, %d)", lx, ly, lz, hx, hy, hz);

  BOOL rebuild = false;

  for (int x = lx; x <= hx; x++)
  {
    for (int y = ly; y <= hy; y++)
    {
      for (int z = lz; z <= hz; z++)
      {
        m_testedPoints++;
        mgPoint3 center(x*ROCK_INTERVAL, y*ROCK_INTERVAL, z*ROCK_INTERVAL);
        double radius;
        if (asteroidPresent(center.x, center.y, center.z, radius))
        {
          m_addedRocks++;

          // don't add more blocks than we have room for (very unlikely)
          if (m_rocks.length() < MAX_VISIBLE_ROCKS)
          {
            Rock* rock = new Rock(center, radius/SYSTEM_FAR_SCALE);
//            mgDebug("add (%g,%g,%g)", rock->m_center.x, rock->m_center.y, rock->m_center.z);
            m_rocks.add(rock);
          }

          rebuild = true;
        }
      }
    }
  }
  return rebuild;
}

//--------------------------------------------------------------
// animate the object
BOOL Belt::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  int cx = (int) floor((m_eyePt.x / SYSTEM_FAR_SCALE) / ROCK_INTERVAL);
  int cy = (int) floor((m_eyePt.y / SYSTEM_FAR_SCALE) / ROCK_INTERVAL);
  int cz = (int) floor((m_eyePt.z / SYSTEM_FAR_SCALE) / ROCK_INTERVAL);

  int lastcx = (int) floor((m_lastEyePt.x / SYSTEM_FAR_SCALE) / ROCK_INTERVAL);
  int lastcy = (int) floor((m_lastEyePt.y / SYSTEM_FAR_SCALE) / ROCK_INTERVAL);
  int lastcz = (int) floor((m_lastEyePt.z / SYSTEM_FAR_SCALE) / ROCK_INTERVAL);

  // if eye hasn't moved, no change
  if (cx == lastcx && cy == lastcy && cz == lastcz)
  {
    m_lastEyePt = m_eyePt;
    return false;
  }

/*
  mgDebug("eye moved by %g, %g, %g, (%d, %d, %d)", 
      m_eyePt.x - m_lastEyePt.x, 
      m_eyePt.y - m_lastEyePt.y, 
      m_eyePt.z - m_lastEyePt.z, 
      cx-lastcx, cy-lastcy, cz-lastcz);
*/
  // don't test more than a view width of points
  int viewWidth = 2*MEDIUM_DIST+1;
  lastcx = max(cx-viewWidth, min(cx+viewWidth, lastcx));
  lastcy = max(cy-viewWidth, min(cy+viewWidth, lastcy));
  lastcz = max(cz-viewWidth, min(cz+viewWidth, lastcz));

  m_testedPoints = 0;
  m_addedRocks = 0;
  int deletedRocks = 0;

  BOOL rebuild = false;

  // check all the existing rocks against max distance from eye
  for (int i = m_rocks.length()-1; i >= 0; i--)
  {
    Rock* rock = (Rock*) m_rocks[i];
    int dx = abs((int) floor(rock->m_center.x/ROCK_INTERVAL) - cx);
    int dy = abs((int) floor(rock->m_center.y/ROCK_INTERVAL) - cy);
    int dz = abs((int) floor(rock->m_center.z/ROCK_INTERVAL) - cz);
    int dist = max(dx, max(dy, dz));
    if (dist > MEDIUM_DIST)
    {
//      mgDebug("remove (%g,%g,%g) at %d", rock->m_center.x, rock->m_center.y, rock->m_center.z, dist);
      m_rocks.removeAt(i);
      delete rock;
      rebuild = true;
      deletedRocks++;
    }
  }

  // scan for new rocks.  we look in the area of difference between old and
  // new bounding volumes (MEDIUM_DIST). 

  int lx = cx-MEDIUM_DIST;
  int ly = cy-MEDIUM_DIST;
  int lz = cz-MEDIUM_DIST;
  int hx = cx+MEDIUM_DIST;
  int hy = cy+MEDIUM_DIST;
  int hz = cz+MEDIUM_DIST;

  int lastlx = lastcx-MEDIUM_DIST;
  int lastly = lastcy-MEDIUM_DIST;
  int lastlz = lastcz-MEDIUM_DIST;
  int lasthx = lastcx+MEDIUM_DIST;
  int lasthy = lastcy+MEDIUM_DIST;
  int lasthz = lastcz+MEDIUM_DIST;

  // movement in +x
  rebuild |= checkVolume(lasthx+1, ly, lz, hx, hy, hz);
  hx = min(lasthx, hx);
  // movement in -x
  rebuild |= checkVolume(lx, ly, lz, lastlx-1, hy, hz);
  lx = max(lastlx, lx);

  // movement in +y
  rebuild |= checkVolume(lx, lasthy+1, lz, hx, hy, hz);
  hy = min(lasthy, hy);
  // movement in -y
  rebuild |= checkVolume(lx, ly, lz, hx, lastly-1, hz);
  ly = max(lastly, ly);

  // movement in +z
  rebuild |= checkVolume(lx, ly, lasthz+1, hx, hy, hz);
  hz = min(lasthz, hz);
  // movement in -z
  rebuild |= checkVolume(lx, ly, lz, hx, hy, lastlz-1);
  lz = max(lastlz, lz);

  // rebuild the medium rock buffers
  if (rebuild)
  {
    m_mediumVertexes->reset();
    m_mediumIndexes->reset();

    // fill medium buffer with distant visible rocks
    for (int i = 0; i < m_rocks.length(); i++)
    {
      Rock* rock = (Rock*) m_rocks[i];
      addMediumRock(rock);
    }
  }

  m_lastEyePt = m_eyePt;

  mgDebug("tested %d, added %d, removed %d, %d total", m_testedPoints, m_addedRocks, deletedRocks, m_rocks.length());

  return rebuild;
}

//--------------------------------------------------------------
// draw front (towards eye) portion of ring
void Belt::renderFrontFar()
{
}

//--------------------------------------------------------------
// draw front (away from eye) portion of ring
void Belt::renderBackFar()
{
}

//--------------------------------------------------------------
// draw the transparent data
void Belt::renderTransparent()
{
  mgDisplay->setLightAmbient(0.1, 0.1, 0.1);
  mgDisplay->setMatColor(0.8, 0.8, 0.8);

  mgDisplay->setCulling(false);

//  mgDisplay->setShader("unlitTexture");
//  mgDisplay->setTexture(m_farTexture);
//  mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farIndexes);       

  mgDisplay->setShader("unlitTexture");
  mgDisplay->setTexture(m_rockTexture);
  mgDisplay->draw(MG_TRIANGLES, m_mediumVertexes, m_mediumIndexes);       

  mgDisplay->setCulling(true);
}

//--------------------------------------------------------------
// render medium version
void Belt::renderMedium()
{
}

//--------------------------------------------------------------
// render near version
void Belt::renderNear()
{
}
