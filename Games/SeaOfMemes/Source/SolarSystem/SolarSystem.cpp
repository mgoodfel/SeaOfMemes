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

#include "Belt.h"
#include "Planet.h"
#include "Moon.h"
#include "Ring.h"
#include "Earth.h"

#include "SolarSystem.h"

//--------------------------------------------------------------
// constructor
SolarSystem::SolarSystem(
  const mgOptionsFile& options)
{
  m_planetDayLen = options.getDouble("planetDayLen", 600.0); // seconds
  if (m_planetDayLen == 0.0)
    m_planetDayLen = 1e10;  // infinity

  m_moonDayLen = options.getDouble("moonDayLen", 300.0); // seconds
  if (m_moonDayLen == 0.0)
    m_moonDayLen = 1e10;  // infinity

  m_ringDayLen = options.getDouble("ringDayLen", 200.0);  // seconds
  if (m_ringDayLen == 0.0)
    m_ringDayLen = 1e10;  // infinity

  m_moonMonthLen = options.getDouble("moonMonthLen", 1000.0);  // seconds
  if (m_moonMonthLen == 0.0)
    m_moonMonthLen = 1e10;  // infinity

  m_beltMonthLen = options.getDouble("beltMonthLen", 1000.0);  // seconds
  if (m_beltMonthLen == 0.0)
    m_beltMonthLen = 1e10;  // infinity

  m_planet = new Planet(options, PLANET_RADIUS);
  m_belt = new Belt(options, BELT_RADIUS);
  m_moon = new Moon(options, MOON_RADIUS);
  m_ring = new Ring(options, RING_RADIUS, RING_WALL_HEIGHT, RING_WALL_WIDTH);

  m_planetDay = 90.0;
  m_moonDay = 0.0;
  m_ringDay = 0.0;

  m_moonMonth = 30.0;
  m_beltMonth = 0.0;

  m_coordSystem = COORDS_PLANET;
  m_coordPosn = mgPoint3(0.0, 2.0, 0.0);
  m_eyeRotX = 0.0;
  m_eyeRotY = 0.0;
  m_eyeRotZ = 0.0;

  mgDebug("Planet radius = %g km (%g miles)", PLANET_RADIUS/1000.0, PLANET_RADIUS/1600.0);
  double val = 4*PI*PLANET_RADIUS*PLANET_RADIUS;
  mgDebug("Surface area of planet = %g million sq km (%g million sq mi)", val/1e12, (val/1e12)/1.609);

  mgDebug("Moon radius = %g km (%g miles)", MOON_RADIUS/1000.0, MOON_RADIUS/1600.0);
  val = 4*PI*MOON_RADIUS*MOON_RADIUS;
  mgDebug("Surface area of moon = %g million sq km (%g million sq mi)", val/1e12, (val/1e12)/1.609);

  mgDebug("Moon distance = %g km (%g miles)", MOON_DISTANCE/1000.0, MOON_DISTANCE/1600.0);

  mgDebug("Ring radius = %g km (%g miles)", RING_RADIUS/1000.0, RING_RADIUS/1600.0);
  val = 2*PI*RING_RADIUS*RING_WIDTH;
  mgDebug("Surface area of ring = %g million sq km (%g million sq mi)", val/1e12, (val/1e12)/1.609);
  mgDebug("Ring wall height = %g m (%g ft)", RING_WALL_HEIGHT, RING_WALL_HEIGHT*3.16);
  mgDebug("Ring wall width = %g m (%g ft)", RING_WALL_WIDTH, RING_WALL_WIDTH*3.16);
}

//--------------------------------------------------------------
// destructor
SolarSystem::~SolarSystem()
{
  deleteBuffers();

  delete m_planet;
  m_planet = NULL;

  delete m_belt;
  m_belt = NULL;

  delete m_moon;
  m_moon = NULL;

  delete m_ring;
  m_ring = NULL;
}

//--------------------------------------------------------------
// delete display buffers
void SolarSystem::deleteBuffers()
{
  m_planet->deleteBuffers();
  m_belt->deleteBuffers();
  m_moon->deleteBuffers();
  m_ring->deleteBuffers();
}

//--------------------------------------------------------------
// create buffers ready to send to display
void SolarSystem::createBuffers()
{
  m_planet->createBuffers();
  m_belt->createBuffers();
  m_moon->createBuffers();
  m_ring->createBuffers();
}

//--------------------------------------------------------------
// animate the system
BOOL SolarSystem::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  BOOL changed = m_planet->animate(now, since);
  changed |= m_belt->animate(now, since);
  changed |= m_moon->animate(now, since);
  changed |= m_ring->animate(now, since);

  m_planetDay += (360.0 * (since/1000.0)) / m_planetDayLen;
  if (m_planetDay > 360.0)
    m_planetDay -= 360.0;

  m_moonDay += (360.0 * (since/1000.0)) / m_moonDayLen;
  if (m_moonDay > 360.0)
    m_moonDay -= 360.0;

  m_ringDay += (360.0 * (since/1000.0)) / m_ringDayLen;
  if (m_ringDay > 360.0)
    m_ringDay -= 360.0;

  m_moonMonth += (360.0 * (since/1000.0)) / m_moonMonthLen;
  if (m_moonMonth > 360.0)
    m_moonMonth -= 360.0;

  m_beltMonth += (360.0 * (since/1000.0)) / m_beltMonthLen;
  if (m_beltMonth > 360.0)
    m_beltMonth -= 360.0;

  // reduce eye rotation on z to zero
  double change = since/25.0;
  if (m_eyeRotZ < -1.0)
    m_eyeRotZ += min(-m_eyeRotZ, change);

  else if (m_eyeRotZ > 1.0)
    m_eyeRotZ -= min(m_eyeRotZ, change);

  else m_eyeRotZ = 0.0;

  updateEye();
  return true;
}

//--------------------------------------------------------------
// draw moon and ring in far mode
void SolarSystem::renderPlanetAndBeltFar()
{
  mgMatrix4 transform;

  // draw back side of belt
  transform.loadIdentity();
  transform.rotateYDeg(m_beltMonth);
  mgDisplay->setModelTransform(transform);
  m_belt->renderBackFar();

  // draw planet
  transform.loadIdentity();
  mgDisplay->setModelTransform(transform);
  m_planet->renderFar(m_planetDay);

  // draw front side of belt
  transform.loadIdentity();
  transform.rotateYDeg(m_beltMonth);
  mgDisplay->setModelTransform(transform);
  m_belt->renderFrontFar();
}

//--------------------------------------------------------------
// draw moon and ring in far mode
void SolarSystem::renderMoonAndRingFar(
  const mgPoint3& moonCenter)
{
  mgMatrix4 transform;

  // draw back side of ring
  transform.loadIdentity();
  transform.rotateYDeg(m_ringDay);
  transform.rotateZDeg(RING_ANGLE);
  transform.translate(moonCenter.x/SYSTEM_FAR_SCALE, moonCenter.y/SYSTEM_FAR_SCALE, moonCenter.z/SYSTEM_FAR_SCALE);
  mgDisplay->setModelTransform(transform);
  m_ring->renderBackFar();

  // draw moon
  transform.loadIdentity();
  transform.rotateYDeg(m_moonDay);
  transform.translate(moonCenter.x/SYSTEM_FAR_SCALE, moonCenter.y/SYSTEM_FAR_SCALE, moonCenter.z/SYSTEM_FAR_SCALE);
  mgDisplay->setModelTransform(transform);
  m_moon->renderFar();

  // draw front side of ring
  transform.loadIdentity();
  transform.rotateYDeg(m_ringDay);
  transform.rotateZDeg(RING_ANGLE);
  transform.translate(moonCenter.x/SYSTEM_FAR_SCALE, moonCenter.y/SYSTEM_FAR_SCALE, moonCenter.z/SYSTEM_FAR_SCALE);
  mgDisplay->setModelTransform(transform);
  m_ring->renderFrontFar();
}

//--------------------------------------------------------------
// render system in far mode
void SolarSystem::renderFar(
  const mgPoint3& moonCenter)
{
  mgPoint3 eyePt(m_eyePt.x / SYSTEM_FAR_SCALE, m_eyePt.y / SYSTEM_FAR_SCALE, m_eyePt.z / SYSTEM_FAR_SCALE);
  mgDisplay->setEyePt(eyePt);
//  mgDisplay->setZEnable(false);

  // get distance from eye to moon center
  mgPoint3 pt(moonCenter);
  pt.subtract(m_eyePt);
  double moonDist = pt.length();

  // planet origin is 0,0,0, so eye pt is distance
  double planetDist = m_eyePt.length();

  // draw farthest object first
  if (moonDist > planetDist)
  {
    renderMoonAndRingFar(moonCenter);
    renderPlanetAndBeltFar();
  }
  else
  {
    renderPlanetAndBeltFar();
    renderMoonAndRingFar(moonCenter);
  }

//  mgDisplay->setZEnable(true);
  mgDisplay->clearBuffer(MG_DEPTH_BUFFER);
}

//--------------------------------------------------------------
// render opaque data
void SolarSystem::render()
{
  // reset state after any changes in sky render
  mgDisplay->setLightDir(m_sunDir.x, m_sunDir.y, m_sunDir.z);
  mgDisplay->setMatColor(1.0, 1.0, 1.0);
  mgDisplay->setLightColor(0.6, 0.6, 0.6);
  mgDisplay->setLightAmbient(0.2, 0.2, 0.2);
  mgDisplay->setTransparent(false);

  double angle = (2*PI*m_moonMonth)/360;
  mgPoint3 moonCenter(MOON_DISTANCE*sin(angle), 0, MOON_DISTANCE*cos(angle));

  renderFar(moonCenter);

  // figure distance to major objects.  
  double planetDist = m_eyePt.length() - PLANET_RADIUS;

  // get distance from point to center of belt
  double ptDist = sqrt(m_eyePt.x*m_eyePt.x + m_eyePt.z*m_eyePt.z);
  ptDist -= BELT_RADIUS;
  double beltDist = sqrt(m_eyePt.y*m_eyePt.y + ptDist*ptDist);

  mgPoint3 pt(m_eyePt);
  pt.subtract(moonCenter);
  double moonDist = pt.length() - MOON_RADIUS;

  // get position relative to ring
  mgPoint3 ringPt;
  mgMatrix4 transform;
  transform.rotateZDeg(-RING_ANGLE);
  transform.mapPt(pt, ringPt);

  // get distance from point to center of ring
  ptDist = sqrt(ringPt.x*ringPt.x + ringPt.z*ringPt.z);
  ptDist -= RING_RADIUS;
  double ringDist = sqrt(ringPt.y*ringPt.y + ptDist*ptDist);

  // only one object can be within medium dist.  render it
  if (planetDist < SYSTEM_MEDIUM_DIST)
    m_planet->renderMedium();
  else if (beltDist < SYSTEM_MEDIUM_DIST)
     m_belt->renderMedium();
  else if (moonDist < SYSTEM_MEDIUM_DIST)
     m_moon->renderMedium();
  else if (ringDist < SYSTEM_MEDIUM_DIST)
     m_ring->renderMedium();

  mgDisplay->clearBuffer(MG_DEPTH_BUFFER);

  // only one object can be within near dist.  render it
  if (planetDist < SYSTEM_NEAR_DIST)
    m_planet->renderNear();
  else if (beltDist < SYSTEM_NEAR_DIST)
     m_belt->renderNear();
  else if (moonDist < SYSTEM_NEAR_DIST)
     m_moon->renderNear();
  else if (ringDist < SYSTEM_NEAR_DIST)
     m_ring->renderNear();
}

#ifdef WORKED
//--------------------------------------------------------------
// render transparent data
void SolarSystem::renderTransparent()
{
  if (m_belt != NULL)
  {
    mgMatrix4 transform;
    transform.rotateYDeg(m_beltMonth);
    mgDisplay->setModelTransform(transform);

    mgDisplay->setTransparent(true);
    m_belt->renderTransparent();
    mgDisplay->setTransparent(false);
  }
}

#endif

//--------------------------------------------------------------
// return distance to nearest object
double SolarSystem::nearestObject(
  int& nearestObj)
{
  // get position of moon
  double angle = (2*PI*m_moonMonth)/360;
  mgPoint3 moonCenter(MOON_DISTANCE*sin(angle), 0, MOON_DISTANCE*cos(angle));

  double planetDist = m_eyePt.length() - PLANET_RADIUS;
  planetDist = max(0.0, planetDist);

  mgPoint3 pt(moonCenter);
  pt.subtract(m_eyePt);
  double moonDist = pt.length() - MOON_RADIUS;
  moonDist = max(0.0, moonDist);

  // get eye in ring-local coordinates
  mgMatrix4 coords;
  coords.rotateYDeg(m_ringDay);
  coords.rotateZDeg(RING_ANGLE);
  coords.translate(moonCenter.x, moonCenter.y, moonCenter.z);

  coords.invertPt(m_eyePt, pt);

  double ringDist = 0.0;
  if (pt.y > RING_WIDTH/2)
  {
    // above ring, take distance to top edge
    double radius = sqrt(pt.x*pt.x + pt.z*pt.z) - RING_RADIUS;
    double ht = pt.y - RING_WIDTH/2;
    ringDist = sqrt(radius*radius + ht*ht);
  }
  else if (pt.y > -RING_WIDTH/2)
  {
    // within ring, distance to base
    ringDist = sqrt(pt.x*pt.x + pt.z*pt.z) - RING_RADIUS;
    ringDist = abs(ringDist);  // inside or out
  }
  else
  {
    // below ring, take distance to bottom edge
    double radius = sqrt(pt.x*pt.x + pt.z*pt.z) - RING_RADIUS;
    double ht = pt.y + RING_WIDTH/2;
    ringDist = sqrt(radius*radius + ht*ht);
  }

  // =-= get eye in belt-local coordinates

  double minDist = planetDist;
  nearestObj = COORDS_PLANET;
  if (moonDist < minDist)
  {
    nearestObj = COORDS_MOON;
    minDist = moonDist;
  }
  if (ringDist < minDist)
  {
    nearestObj = COORDS_RING;
    minDist = ringDist;
  }
  return minDist;
}

//--------------------------------------------------------------
// set object for coordinates
void SolarSystem::setCoordSystem(
  int coordSystem)
{
  m_coordSystem = coordSystem;
}

//--------------------------------------------------------------
// set local coordinates
void SolarSystem::setCoordPosn(
  const mgPoint3& posn)
{
  m_coordPosn = posn;
}

//--------------------------------------------------------------
// set eye angle
void SolarSystem::setEyeAngle(
  double xrot,
  double yrot)
{
  m_eyeRotX = xrot;
  m_eyeRotY = yrot;
}

//--------------------------------------------------------------
// remap eye angles to new coordinate space
void SolarSystem::remapEyeAngles(
  int newCoords)
{
  // transform the eye matrix into the new coordinate system
  mgMatrix4 newEyeMatrix(m_eyeMatrix);

  mgMatrix4 view;
  getCoordTransform(newCoords, view);

  // cancel translation and apply view transform
  view._41 = 0.0;
  view._42 = 0.0;
  view._43 = 0.0;
  newEyeMatrix.leftMultiply(view);

  // get avatar orientation matrix
  mgMatrix4 basis;
  getCoordBasis(newCoords, basis);

  // apply inverse orientation transform
  basis.transpose();
  newEyeMatrix.leftMultiply(basis);

  double rotX, rotX2, rotZ, rotZ2;
  double rotY = asin(newEyeMatrix._31);
  double rotY2 = PI-rotY;
  double denom = cos(rotY);
  double denom2 = cos(rotY2);
  if (denom != 0.0)
  {
    rotX = atan2(-newEyeMatrix._32/denom, newEyeMatrix._33/denom);
    rotX2 = atan2(-newEyeMatrix._32/denom2, newEyeMatrix._33/denom2);
    rotZ = atan2(-newEyeMatrix._21/denom, newEyeMatrix._11/denom);
    rotZ2 = atan2(-newEyeMatrix._21/denom2, newEyeMatrix._11/denom2);
  }
  else
  {
    rotZ = 0.0;
    if (newEyeMatrix._31 == -1.0)
    {
      rotZ = PI/2;
      rotX = atan2(-newEyeMatrix._12, newEyeMatrix._13);
    }
    else
    {
      rotZ = -PI/2;
      rotX = atan2(newEyeMatrix._12, -newEyeMatrix._13);
    }
    rotZ2 = rotZ;
    rotX2 = rotX;
  }

  mgDebug("x = %g or %g", 180*rotX/PI, 180*rotX2/PI);
  mgDebug("y = %g or %g", 180*rotY/PI, 180*rotY2/PI);
  mgDebug("z = %g or %g", 180*rotZ/PI, 180*rotZ2/PI);

  if (abs(rotZ) < abs(rotZ2))
  {
    m_eyeRotX = 180*rotX/PI;
    m_eyeRotY = 180*rotY/PI;
    m_eyeRotZ = 180*rotZ/PI;
  }
  else
  {
    m_eyeRotX = 180*rotX2/PI;
    m_eyeRotY = 180*rotY2/PI;
    m_eyeRotZ = 180*rotZ2/PI;
  }
}

//--------------------------------------------------------------
// change coordinate system
void SolarSystem::changeCoords(
  int newCoords)
{
  if (m_coordSystem == newCoords)
    return;

  // remap the eye angles
  updateEye();
  remapEyeAngles(newCoords);

  m_coordSystem = newCoords;

  // convert eye point to new local coordinates
  mgMatrix4 coords;
  mgPoint3 pt;

  switch (m_coordSystem)
  {
    case COORDS_PLANET:
    {
      coords.rotateYDeg(m_planetDay);

      coords.invertPt(m_eyePt, pt);

      // get height above center of planet
      double ht = pt.length();

      // figure longitude
      double lonRadius = sqrt(pt.z*pt.z + pt.x*pt.x);
      double lonAngle = asin(pt.z/lonRadius);
      if (pt.x < 0)
        lonAngle = PI-lonAngle;
                                                           
      // figure latitude
      double latAngle = asin(pt.y/ht);

      // compute local position from lat/lon
      m_coordPosn.x = ht*cos(latAngle)*cos(lonAngle);
      m_coordPosn.y = ht*sin(latAngle);
      m_coordPosn.z = ht*cos(latAngle)*sin(lonAngle);
      break;
    }

    case COORDS_MOON:
    {
      double angle = (2*PI*m_moonMonth)/360;
      mgPoint3 moonCenter(MOON_DISTANCE*sin(angle), 0, MOON_DISTANCE*cos(angle));
      coords.rotateYDeg(m_moonDay);
      coords.translate(moonCenter.x, moonCenter.y, moonCenter.z);

      coords.invertPt(m_eyePt, pt);

      // get height above center of planet
      double ht = pt.length();

      // figure longitude
      double lonRadius = sqrt(pt.z*pt.z + pt.x*pt.x);
      double lonAngle = asin(pt.z/lonRadius);
      if (pt.x < 0)
        lonAngle = PI-lonAngle;
                                                           
      // figure latitude
      double latAngle = asin(pt.y/ht);

      // compute local position from lat/lon
      m_coordPosn.x = ht*cos(latAngle)*cos(lonAngle);
      m_coordPosn.y = ht*sin(latAngle);
      m_coordPosn.z = ht*cos(latAngle)*sin(lonAngle);
      break;
    }

    case COORDS_RING:
    {
      double angle = (2*PI*m_moonMonth)/360;
      mgPoint3 moonCenter(MOON_DISTANCE*sin(angle), 0, MOON_DISTANCE*cos(angle));

      // get eye in ring-local coordinates
      coords.rotateYDeg(m_ringDay);
      coords.rotateZDeg(RING_ANGLE);
      coords.translate(moonCenter.x, moonCenter.y, moonCenter.z);

      coords.invertPt(m_eyePt, pt);

      double ht = sqrt(pt.z*pt.z + pt.x*pt.x);
      angle = asin(pt.z/ht);
      if (pt.x < 0)
        angle = PI-angle;

      // compute delta from old position
      m_coordPosn.x = ht*cos(angle);
      m_coordPosn.y = pt.y;
      m_coordPosn.z = ht*sin(angle);
      break;
    }

    case COORDS_BELT:
      break;

    case COORDS_SPACE:
      m_coordPosn = m_eyePt;
      break;
  }
}

//--------------------------------------------------------------
// update position of eye
void SolarSystem::moveEye(
  double forwards,
  double sideways, 
  double vertical)
{
  mgPoint3 move;
  switch (m_coordSystem)
  {
    case COORDS_PLANET:
    case COORDS_MOON:
    {
      // only move horizontally based on eye direction
      mgMatrix4 eyeMatrix;
//      eyeMatrix.rotateZDeg(m_eyeRotZ);
      eyeMatrix.rotateYDeg(m_eyeRotY);
//      eyeMatrix.rotateXDeg(m_eyeRotX);

      double ex, ey, ez;
      eyeMatrix.invertPt(sideways, 0.0, forwards, ex, ey, ez);

      // get height above center
      double ht = m_coordPosn.length();

      // figure longitude
      double lonRadius = sqrt(m_coordPosn.z*m_coordPosn.z + m_coordPosn.x*m_coordPosn.x);
      double lonAngle = asin(m_coordPosn.z/lonRadius);
      if (m_coordPosn.x < 0)
        lonAngle = PI-lonAngle;
      lonAngle += ez / ht;
                                                           
      // figure latitude
      double latAngle = asin(m_coordPosn.y/ht);
      latAngle -= ex / ht;

      // adjust for height
      ht += vertical;

      // compute delta from old position
      move.x = ht*cos(latAngle)*cos(lonAngle) - m_coordPosn.x;
      move.y = ht*sin(latAngle)               - m_coordPosn.y;
      move.z = ht*cos(latAngle)*sin(lonAngle) - m_coordPosn.z;
      break;
    }

    case COORDS_RING:
    {
      // only move horizontally based on eye direction
      mgMatrix4 eyeMatrix;
//      eyeMatrix.rotateZDeg(m_eyeRotZ);
      eyeMatrix.rotateYDeg(m_eyeRotY);
//      eyeMatrix.rotateXDeg(m_eyeRotX);
  
      double ex, ey, ez;
      eyeMatrix.invertPt(sideways, 0.0, forwards, ex, ey, ez);

      double ht = sqrt(m_coordPosn.z*m_coordPosn.z + m_coordPosn.x*m_coordPosn.x);
      double angle = asin(m_coordPosn.z/ht);
      if (m_coordPosn.x < 0)
        angle = PI-angle;
      angle += ez / ht;

      // adjust for height
      ht -= vertical;

      // compute delta from old position
      move.x = ht*cos(angle) - m_coordPosn.x;
      move.y = ex;
      move.z = ht*sin(angle) - m_coordPosn.z;
      break;
    }

    case COORDS_BELT:
      break;

    case COORDS_SPACE:
    default:
    {
      // move in direction of eye
      mgMatrix4 eyeMatrix;
      eyeMatrix.rotateZDeg(m_eyeRotZ);
      eyeMatrix.rotateYDeg(m_eyeRotY);
      eyeMatrix.rotateXDeg(m_eyeRotX);
  
      eyeMatrix.invertPt(sideways, vertical, forwards, move.x, move.y, move.z);
      break;
    }
  }

  m_coordPosn.add(move);
}

//--------------------------------------------------------------
// update direction of eye
void SolarSystem::turnEye(
  double xrot,
  double yrot)
{
  m_eyeRotX += xrot;
  m_eyeRotY += yrot;

  // don't allow head to flip over
  m_eyeRotX = min(m_eyeRotX, 90.0);
  m_eyeRotX = max(m_eyeRotX, -90.0);

  // keep direction within range
  if (m_eyeRotY < -180.0)
    m_eyeRotY += 360.0;
  if (m_eyeRotY > 180.0)
    m_eyeRotY -= 360.0;
}

//--------------------------------------------------------------
// get view transform under coordinate system
void SolarSystem::getCoordTransform(
  int coordSystem,
  mgMatrix4& coords)
{
  coords.loadIdentity();

  // get position of moon
  double angle = (2*PI*m_moonMonth)/360;
  mgPoint3 moonCenter(MOON_DISTANCE*sin(angle), 0, MOON_DISTANCE*cos(angle));

  // get coordinate system based on object
  switch (coordSystem)
  {
    case COORDS_PLANET:
      coords.rotateYDeg(m_planetDay);
      break;

    case COORDS_MOON:
      coords.rotateYDeg(m_moonDay);
      coords.translate(moonCenter.x, moonCenter.y, moonCenter.z);
      break;

    case COORDS_RING:
      coords.rotateYDeg(m_ringDay);
      coords.rotateZDeg(RING_ANGLE);
      coords.translate(moonCenter.x, moonCenter.y, moonCenter.z);
      break;

    case COORDS_BELT:
      coords.rotateYDeg(m_beltMonth);
      break;

    case COORDS_SPACE:
      // use identity matrix for coords
      break;
  }
}

//--------------------------------------------------------------
// get avatar orientation under coordinate system
void SolarSystem::getCoordBasis(
  int coordSystem,
  mgMatrix4& basis)
{
  // get position of moon
  double angle = (2*PI*m_moonMonth)/360;
  mgPoint3 moonCenter(MOON_DISTANCE*sin(angle), 0, MOON_DISTANCE*cos(angle));

  // get coordinate system based on object
  mgPoint3 xaxis(1.0, 0.0, 0.0);
  mgPoint3 yaxis(0.0, 1.0, 0.0);
  mgPoint3 zaxis(0.0, 0.0, 1.0);

  switch (coordSystem)
  {
    case COORDS_PLANET:
      // y axis points away from center of planet
      yaxis = m_coordPosn;
      yaxis.normalize();

      // construct z axis orthonal to y
      zaxis = yaxis;
      zaxis.cross(mgPoint3(0.0, 1.0, 0.0));
      zaxis.normalize();

      // construct x axis
      xaxis = yaxis;
      xaxis.cross(zaxis);
      xaxis.normalize();
      break;

    case COORDS_MOON:
      // y axis points away from center of moon
      yaxis = m_coordPosn;
      yaxis.normalize();

      // construct z axis orthonal to y
      zaxis = yaxis;
      zaxis.cross(mgPoint3(0.0, 1.0, 0.0));
      zaxis.normalize();

      // construct x axis
      xaxis = yaxis;
      xaxis.cross(zaxis);
      xaxis.normalize();
      break;

    case COORDS_RING:
      // y axis points towards center of ring
      yaxis.x = -m_coordPosn.x;
      yaxis.z = -m_coordPosn.z;
      yaxis.y = 0.0;
      yaxis.normalize();

      // x axis is across the ring
      xaxis.x = 0.0; xaxis.y = 1.0; xaxis.z = 0.0;

      // construct z axis
      zaxis = xaxis;
      zaxis.cross(yaxis);
      zaxis.normalize();
      break;

    case COORDS_BELT:
      break;

    case COORDS_SPACE:
      // use default axis
      break;
  }

  // apply object surface orientation
  basis._11 = xaxis.x;
  basis._21 = xaxis.y;
  basis._31 = xaxis.z;
  basis._12 = yaxis.x;
  basis._22 = yaxis.y;
  basis._32 = yaxis.z;
  basis._13 = zaxis.x;
  basis._23 = zaxis.y;
  basis._33 = zaxis.z;
}

//--------------------------------------------------------------
// update the eye point and matrix
void SolarSystem::updateEye()
{
  mgMatrix4 view;
  getCoordTransform(m_coordSystem, view);

  // map local coordinates to system coordinates
  view.mapPt(m_coordPosn, m_eyePt);

  m_eyeMatrix.loadIdentity();
  m_eyeMatrix.rotateZDeg(m_eyeRotZ);
  m_eyeMatrix.rotateYDeg(m_eyeRotY);
  m_eyeMatrix.rotateXDeg(m_eyeRotX);

  // get avatar orientation matrix
  mgMatrix4 basis;
  getCoordBasis(m_coordSystem, basis);

  m_eyeMatrix.leftMultiply(basis);

  // cancel translation and apply inverse view transform
  view._41 = 0.0;
  view._42 = 0.0;
  view._43 = 0.0;
  view.transpose();
  m_eyeMatrix.leftMultiply(view);
}

