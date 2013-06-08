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
#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

const double SYSTEM_FAR_SCALE = 5000.0;  
const double SYSTEM_MEDIUM_SCALE = 128.0;
const double SYSTEM_MEDIUM_DIST = 1024.0 * 1000.0;  // meters
const double SYSTEM_NEAR_DIST = 64.0 * 1000.0;  // meters

const double PLANET_RADIUS = 6000.0*1000.0;  // meters
const double BELT_RADIUS = 3.0*PLANET_RADIUS;

const double MOON_RADIUS = 1500.0*1000.0;    // meters

const double RING_ANGLE = 60.0;
const double RING_RADIUS = 2400.0*1000.0;    // meters
const double RING_WIDTH = (RING_RADIUS*2*PI*32)/2048;
const double RING_WALL_HEIGHT = RING_WIDTH / 50.0;
const double RING_WALL_WIDTH = RING_WALL_HEIGHT/5.0;

const double MOON_DISTANCE = 25.0*1000.0*1000.0;    // meters

const int COORDS_SPACE    = 0;
const int COORDS_PLANET   = 1;
const int COORDS_MOON     = 2;
const int COORDS_RING     = 3;
const int COORDS_BELT     = 4;

class Belt;
class Moon;
class Planet;
class Ring;
class Earth;

class SolarSystem
{
public:
  double m_planetDayLen;
  double m_moonDayLen;
  double m_ringDayLen;
  double m_moonMonthLen;
  double m_beltMonthLen;

  // constructor
  SolarSystem(
    const mgOptionsFile& options);
  
  virtual ~SolarSystem();
    
  // delete display buffers
  virtual void deleteBuffers();

  // create buffers ready to send to display
  virtual void createBuffers();

  // set the sun direction
  virtual void setSunDir(
    const mgPoint3& sunDir)
  {
    m_sunDir = sunDir;
  }

  // animate object
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // draw the world
  virtual void render();

  // return distance to nearest object
  virtual double nearestObject(
    int& nearestObj);

  // set object for coordinates
  virtual void setCoordSystem(
    int coordObject);

  // change coordinate system
  virtual void changeCoords(
    int newCoords);

  // set local coordinates
  virtual void setCoordPosn(
    const mgPoint3& posn);

  // set eye angle
  virtual void setEyeAngle(
    double xrot,
    double yrot);

  // get eye position
  void getEyePt(
    mgPoint3& eyePt)
  {
    eyePt = m_eyePt;
  }
    
  // get eye rotation
  void getEyeRot(
    mgPoint3& eyeRot)
  {
    eyeRot.x = m_eyeRotX;
    eyeRot.y = m_eyeRotY;
    eyeRot.z = m_eyeRotZ;
  }
    
  // get eye matrix
  void getEyeMatrix(
    mgMatrix4& eyeMatrix)
  {
    eyeMatrix = m_eyeMatrix;
  }
    
  // update position of eye
  virtual void moveEye(
    double forwards,
    double sideways, 
    double vertical);
                             
  // update direction of eye
  virtual void turnEye(
    double xrot,
    double yrot);

protected:
  mgPoint3 m_sunDir;

  Earth* m_earth;
  Planet* m_planet;
  Belt* m_belt;
  Moon* m_moon;
  Ring* m_ring;

  double m_planetDay;
  double m_moonDay;
  double m_ringDay;

  double m_moonMonth;
  double m_beltMonth;

  int m_coordSystem;
  mgPoint3 m_coordPosn;

  mgPoint3 m_eyePt;
  mgMatrix4 m_eyeMatrix;
  double m_eyeRotX;
  double m_eyeRotY;
  double m_eyeRotZ;

  // render system in far mode
  virtual void renderFar(
    const mgPoint3& moonCenter);

  // render moon and ring in far mode
  virtual void renderMoonAndRingFar(
    const mgPoint3& moonCenter);

  // render planet and belt in far mode
  virtual void renderPlanetAndBeltFar();

  // update the eye point and matrix
  virtual void updateEye();

  // get view transform under coordinate system
  virtual void getCoordTransform(
    int coordSystem,
    mgMatrix4& coords);

  // get the avatar orientation under coordinate system
  virtual void getCoordBasis(
    int coordSystem,
    mgMatrix4& transform);

  // remap eye angles to new coordinate space
  virtual void remapEyeAngles(
    int newCoords);
};

#endif
