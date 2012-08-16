/*
  Copyright (C) 1995-2011 by Michael J. Goodfellow

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

#include "Util/VertexTerrain.h"
#include "Util/SimplexNoise.h"
#include "SolarSystem.h"
#include "GenerationShip.h"

const double SHIP_LENGTH = 2000.0 / SYSTEM_FAR_SCALE;
const double SHIP_RADIUS = 400.0 / SYSTEM_FAR_SCALE;
const double LIGHT_RADIUS = 5.0 / SYSTEM_FAR_SCALE;
const double WALL_HEIGHT = 15.0 / SYSTEM_FAR_SCALE;

const double WATER_LEVEL = 7.0;
const double MIN_HEIGHT = 5.0;

//--------------------------------------------------------------
// constructor
GenerationShip::GenerationShip(
  const mgOptionsFile& options,
  const mgPoint3& center)
{
  mgVertex::loadShader("litTexture");
  mgVertex::loadShader("unlitTexture");
  VertexTerrain::loadShader("terrain");

  mgString fileName;
  options.getFileName("waterTexture", options.m_sourceFileName, "", fileName);
  m_waterTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("lightTexture", options.m_sourceFileName, "", fileName);
  m_lightTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("shellTexture", options.m_sourceFileName, "", fileName);
  m_shellTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("terrainTexture", options.m_sourceFileName, "", fileName);
  m_terrainTexture = mgDisplay->loadTexture(fileName);

  m_center = center;
  m_samples = 510;

  m_shellIndexes = NULL;
  m_shellVertexes = NULL;

  m_terrainIndexes = NULL;
  m_terrainVertexes = NULL;

  m_waterIndexes = NULL;
  m_waterVertexes = NULL;

  m_lightIndexes = NULL;
  m_lightVertexes = NULL;

  mgDebug("Ship radius = %g km (%g miles)", SHIP_RADIUS, SHIP_RADIUS/1.609);
  double val = 2*PI*SHIP_RADIUS*SHIP_LENGTH;
  mgDebug("Surface area of ship = %g sq km (%g sq mi)", val, val/1.609);
}

//--------------------------------------------------------------
// destructor
GenerationShip::~GenerationShip()
{
  deleteBuffers();
}
    
//--------------------------------------------------------------
// generate noise
double GenerationShip::noise(
  double x,
  double y)
{
  int octaves = 8;
  double scale = 1.0;
  double amplitude = 1.0;

  double v = 0.0;
  for (int o = 0; o < octaves; o++)
  {
    double noise = SimplexNoise::noise(x * scale, y * scale);
    v += noise * amplitude;
    scale *= 2.0;
    amplitude *= 0.5;
  }
  return v;
}

//--------------------------------------------------------------
// generate noise
double GenerationShip::noise(
  double x,
  double y,
  double z)
{
  int octaves = 10;
  double scale = 1.0;
  double amplitude = 1.0;

  double v = 0.0;
  for (int o = 0; o < octaves; o++)
  {
    double noise = SimplexNoise::noise(x*scale, y*scale, z*scale);
    v += noise * amplitude;
    scale *= 2.0;
    amplitude *= 0.5;
  }
  return v;
}

//--------------------------------------------------------------
// create ship shell
void GenerationShip::createShell()
{
  int capsteps = m_samples/10;
  int capsize = (1+capsteps)*(1+capsteps);

  int steps = m_samples/10;
  int vertexSize = 1+steps;

  // vertexes are top and bottom of cylinder plus endcaps plus walls
  m_shellVertexes = mgVertex::newBuffer(vertexSize*2 + 2*capsize + 3*2*4);

  // build outside walls
  mgVertex v;
  for (int i = 0; i < vertexSize; i++)
  {
    double angle = (2*PI*i)/steps;
    double x = cos(angle);
    double y = sin(angle);

    // normal points out
    v.m_nx = (float) x;
    v.m_ny = (float) y;
    v.m_nz = 0.0f;

    // top point
    v.m_px = (float) (m_center.x + SHIP_RADIUS * x);
    v.m_py = (float) (m_center.y + SHIP_RADIUS * y);
    v.m_pz = (float) (m_center.z - SHIP_LENGTH/2);

    v.m_tx = (float) (i*6*SHIP_RADIUS*SYSTEM_FAR_SCALE/steps);
    v.m_ty = 0.0f;

    v.addTo(m_shellVertexes);

    // bottom point
    v.m_pz = (float) (m_center.z + SHIP_LENGTH/2);

    v.m_ty = (float) (SHIP_LENGTH*SYSTEM_FAR_SCALE);

    v.addTo(m_shellVertexes);
  }

  // create indexes
  int slice = steps/3;
  int start = steps/6;
  m_shellIndexes = mgDisplay->newIndexBuffer(steps*6 + 2*capsize*capsize*6 + 6*3*2*4);
  for (int i = 0; i < steps; i++)
  {
    if ((i+start)%slice == 0)
      continue;

    int index = i*2;
    m_shellIndexes->addIndex(index);  // tl
    m_shellIndexes->addIndex(index+2);  // tr
    m_shellIndexes->addIndex(index+1);  // bl

    m_shellIndexes->addIndex(index+1);  // bl
    m_shellIndexes->addIndex(index+2);  // tr
    m_shellIndexes->addIndex(index+3);  // br
  }

  int vertexBase;

  // create window walls
  double insideRadius = SHIP_RADIUS - WALL_HEIGHT;
  for (int i = start+1; i < steps; i+=slice)
  {
    vertexBase = m_shellVertexes->getLength();

    // left side wall
    double angle = (2*PI*i)/steps;
    double x = cos(angle);
    double y = sin(angle);

    // normal points right
    v.m_nx = (float) -y;
    v.m_ny = (float) x;
    v.m_nz = 0.0f;

    // top point
    v.m_px = (float) (m_center.x + SHIP_RADIUS * x);
    v.m_py = (float) (m_center.y + SHIP_RADIUS * y);
    v.m_pz = (float) (m_center.z - SHIP_LENGTH/2);

    v.m_tx = 0.0f;
    v.m_ty = 0.0f;

    v.addTo(m_shellVertexes);

    v.m_px = (float) (m_center.x + insideRadius * x);
    v.m_py = (float) (m_center.y + insideRadius * y);
    v.m_tx = (float) (WALL_HEIGHT*SYSTEM_FAR_SCALE);

    v.addTo(m_shellVertexes);

    // bottom point
    v.m_px = (float) (m_center.x + SHIP_RADIUS * x);
    v.m_py = (float) (m_center.y + SHIP_RADIUS * y);
    v.m_pz = (float) (m_center.z + SHIP_LENGTH/2);

    v.m_tx = 0.0f;
    v.m_ty = (float) (SHIP_LENGTH*SYSTEM_FAR_SCALE);

    v.addTo(m_shellVertexes);

    v.m_px = (float) (m_center.x + insideRadius * x);
    v.m_py = (float) (m_center.y + insideRadius * y);
    v.m_tx = (float) (WALL_HEIGHT*SYSTEM_FAR_SCALE);

    v.addTo(m_shellVertexes);

    // right side wall
    angle = (2*PI*(i+1))/steps;
    x = cos(angle);
    y = sin(angle);

    // normal points left
    v.m_nx = (float) y;
    v.m_ny = (float) -x;
    v.m_nz = 0.0f;

    // top point
    v.m_px = (float) (m_center.x + SHIP_RADIUS * x);
    v.m_py = (float) (m_center.y + SHIP_RADIUS * y);
    v.m_pz = (float) (m_center.z - SHIP_LENGTH/2);

    v.m_tx = 0.0f;
    v.m_ty = 0.0f;
             
    v.addTo(m_shellVertexes);

    // inside point
    v.m_px = (float) (m_center.x + insideRadius * x);
    v.m_py = (float) (m_center.y + insideRadius * y);
    v.m_tx = (float) (WALL_HEIGHT*SYSTEM_FAR_SCALE);

    v.addTo(m_shellVertexes);

    // bottom point
    v.m_px = (float) (m_center.x + SHIP_RADIUS * x);
    v.m_py = (float) (m_center.y + SHIP_RADIUS * y);
    v.m_pz = (float) (m_center.z + SHIP_LENGTH/2);

    v.m_tx = 0.0f;
    v.m_ty = (float) (SHIP_LENGTH*SYSTEM_FAR_SCALE);

    v.addTo(m_shellVertexes);

    // inside point
    v.m_px = (float) (m_center.x + insideRadius * x);
    v.m_py = (float) (m_center.y + insideRadius * y);
    v.m_tx = (float) WALL_HEIGHT;

    v.addTo(m_shellVertexes);

    int index = vertexBase;
    m_shellIndexes->addIndex(index);  // tl
    m_shellIndexes->addIndex(index+2);  // tr
    m_shellIndexes->addIndex(index+1);  // bl

    m_shellIndexes->addIndex(index+1);  // bl
    m_shellIndexes->addIndex(index+2);  // tr
    m_shellIndexes->addIndex(index+3);  // br

    index = vertexBase+4;
    m_shellIndexes->addIndex(index);  // tl
    m_shellIndexes->addIndex(index+2);  // tr
    m_shellIndexes->addIndex(index+1);  // bl

    m_shellIndexes->addIndex(index+1);  // bl
    m_shellIndexes->addIndex(index+2);  // tr
    m_shellIndexes->addIndex(index+3);  // br
  }

  // create top cap
  vertexBase = m_shellVertexes->getLength();

  for (int i = 0; i <= capsteps; i++)
  {
    double z = i/(double) capsteps;
    double zradius = SHIP_RADIUS * sqrt(1.0 - z*z);
    for (int j = 0; j <= capsteps; j++)
    {
      double angle = (2*PI*j)/capsteps;
      double x = cos(angle);
      double y = sin(angle);

      v.m_nx = (float) x;
      v.m_ny = (float) y;
      v.m_nz = 0.0f;

      v.m_px = (float) (m_center.x + zradius*x);
      v.m_py = (float) (m_center.y + zradius*y);
      v.m_pz = (float) (m_center.z + SHIP_LENGTH/2 + z*SHIP_RADIUS/2);

      v.m_tx = (float) (j*6*SHIP_RADIUS*SYSTEM_FAR_SCALE/capsteps);
      v.m_ty = (float) (z*SHIP_RADIUS*SYSTEM_FAR_SCALE/2);

      v.addTo(m_shellVertexes);
    }
  }

  m_shellIndexes->addGrid(vertexBase, capsteps+1, capsteps, capsteps, true);

  vertexBase = m_shellVertexes->getLength();

  for (int i = 0; i <= capsteps; i++)
  {
    double z = i/(double) capsteps;
    double zradius = SHIP_RADIUS * sqrt(1.0 - z*z);
    for (int j = 0; j <= capsteps; j++)
    {
      double angle = (2*PI*j)/capsteps;
      double x = cos(angle);
      double y = sin(angle);

      v.m_nx = (float) x;
      v.m_ny = (float) y;
      v.m_nz = 0.0f;

      v.m_px = (float) (m_center.x + zradius*x);
      v.m_py = (float) (m_center.y + zradius*y);
      v.m_pz = (float) (m_center.z - (SHIP_LENGTH/2 + z*SHIP_RADIUS/2));

      v.m_tx = (float) (j*6*SHIP_RADIUS*SYSTEM_FAR_SCALE/capsteps);
      v.m_ty = (float) (z*SHIP_RADIUS*SYSTEM_FAR_SCALE/2);

      v.addTo(m_shellVertexes);
    }
  }

  m_shellIndexes->addGrid(vertexBase, capsteps+1, capsteps, capsteps, true);
}

//--------------------------------------------------------------
// create water
void GenerationShip::createWater()
{
  int steps = m_samples/10;
  int vertexSize = 1+steps;

  // vertexes are top and bottom of cylinder
  m_waterVertexes = mgVertex::newBuffer(vertexSize*2);

  double radius = SHIP_RADIUS - WATER_LEVEL/SYSTEM_FAR_SCALE;
  mgVertex v;
  for (int i = 0; i < vertexSize; i++)
  {
    double angle = (2*PI*i)/steps;
    double x = cos(angle);
    double y = sin(angle);

    // normal points out
    v.m_nx = (float) x;
    v.m_ny = (float) y;
    v.m_nz = 0.0f;

    // top point
    v.m_px = (float) (m_center.x + radius * x);
    v.m_py = (float) (m_center.y + radius * y);
    v.m_pz = (float) (m_center.z - SHIP_LENGTH/2);

    v.m_tx = (float) (i*6*radius*SYSTEM_FAR_SCALE/steps);
    v.m_ty = 0.0f;

    v.addTo(m_waterVertexes);

    // bottom point
    v.m_pz = (float) (m_center.z + SHIP_LENGTH/2);

    v.m_ty = (float) (SHIP_LENGTH*SYSTEM_FAR_SCALE);

    v.addTo(m_waterVertexes);
  }

  // create indexes
  int slice = steps/3;
  int start = steps/6;
  m_waterIndexes = mgDisplay->newIndexBuffer(steps*6);
  for (int i = 0; i < steps; i++)
  {
    if ((i+start)%slice == 0)
      continue;

    int index = i*2;
    m_waterIndexes->addIndex(index);  // tl
    m_waterIndexes->addIndex(index+2);  // tr
    m_waterIndexes->addIndex(index+1);  // bl

    m_waterIndexes->addIndex(index+1);  // bl
    m_waterIndexes->addIndex(index+2);  // tr
    m_waterIndexes->addIndex(index+3);  // br
  }
}

//--------------------------------------------------------------
// create sun
void GenerationShip::createLight()
{
  int steps = m_samples/10;
  int vertexSize = 1+steps;

  // vertexes are top and bottom of cylinder
  m_lightVertexes = mgVertex::newBuffer(vertexSize*2);

  double radius = LIGHT_RADIUS;
  mgVertex v;
  for (int i = 0; i < vertexSize; i++)
  {
    double angle = (2*PI*i)/steps;
    double x = cos(angle);
    double y = sin(angle);

    // normal points out
    v.m_nx = (float) x;
    v.m_ny = (float) y;
    v.m_nz = 0.0f;

    // top point
    v.m_px = (float) (m_center.x + radius * x);
    v.m_py = (float) (m_center.y + radius * y);
    v.m_pz = (float) (m_center.z - (SHIP_LENGTH/2 + SHIP_RADIUS/2));

    v.m_tx = (float) (i*6*radius/steps);
    v.m_ty = 0.0f;

    v.addTo(m_lightVertexes);

    // bottom point
    v.m_pz = (float) (m_center.z + SHIP_LENGTH/2 + SHIP_RADIUS/2);

    v.m_ty = (float) (SHIP_LENGTH + SHIP_RADIUS);

    v.addTo(m_lightVertexes);
  }

  // create indexes
  m_lightIndexes = mgDisplay->newIndexBuffer(steps*6);
  for (int i = 0; i < steps; i++)
  {
    int index = i*2;
    m_lightIndexes->addIndex(index);  // tl
    m_lightIndexes->addIndex(index+2);  // tr
    m_lightIndexes->addIndex(index+1);  // bl

    m_lightIndexes->addIndex(index+1);  // bl
    m_lightIndexes->addIndex(index+2);  // tr
    m_lightIndexes->addIndex(index+3);  // br
  }
}

//--------------------------------------------------------------
// create the polygons for a height map
void GenerationShip::addPolygons(
  double* heights,
  int width,
  int height,
  mgVertexBuffer* vertexes)
{
  // generate the vertex list.
  int heightSize = 3+width;  // -1 to width+1
  int vertexSize = 1+width;

  float step = (float) ((2*PI*SHIP_RADIUS)/width);

  VertexTerrain v;
  v.setColor(133, 133, 133);  // gray rock
  for (int i = 0; i <= height; i++)
  {
    double z = -0.5 + (i/(double) height);
    for (int j = 0; j <= width; j++)
    {
      int index = (i+1)*heightSize + (j+1);

      double angle = (2*PI*j)/width;
      double x = cos(angle);
      double y = sin(angle);

      double ht = heights[index];

      v.m_px = (float) (m_center.x + x*(SHIP_RADIUS-ht));
      v.m_py = (float) (m_center.y + y*(SHIP_RADIUS-ht));
      v.m_pz = (float) (m_center.z + z*SHIP_LENGTH);

      float slope, len;
      v.m_nx = v.m_ny = v.m_nz = 0.0f;
      slope = (float) (heights[index-heightSize]-ht);
      len = sqrt(slope*slope+step*step);
      v.m_nx += slope/len;
      v.m_ny += step/len;

      slope = (float) (heights[index+heightSize] - ht);
      len = sqrt(slope*slope+step*step);
      v.m_nx += -slope/len;
      v.m_ny += step/len;

      slope = (float) (heights[index-1] - ht);
      len = sqrt(slope*slope+step*step);
      v.m_nz += slope/len;
      v.m_ny += step/len;

      slope = (float) (heights[index+1] - ht);
      len = sqrt(slope*slope+step*step);
      v.m_nz += -slope/len;
      v.m_ny += step/len;

      v.m_nx /= 4.0f;
      v.m_ny /= 4.0f;
      v.m_nz /= 4.0f;

      v.m_tx = (float) i;
      v.m_ty = (float) j;

      ht *= SYSTEM_FAR_SCALE;
      if (ht > 50)
      {
        v.setColor(133, 133, 133, 250);  // gray rock
      }
      else if (ht > WATER_LEVEL+3)  
      {
        v.setColor(62, 124, 85, 200); // green grass
      }
      else if (ht > WATER_LEVEL-3)    
      {
        v.setColor(255, 204, 161, 150);   // brown sand
      }
      else v.setColor(135, 104, 73, 100);  // brown dirt

      v.addTo(vertexes);
    }
  }
}

//--------------------------------------------------------------
// add indexes for a grid of points
void GenerationShip::addGridIndexes(
  mgIndexBuffer* indexes,
  int vertexBase,
  int width,
  int height,
  int widthStart,
  int widthLen,
  BOOL outward)
{
  int vertexSize = width+1;
  for (int i = 0; i < height; i++)
  {
    for (int j = widthStart; j < widthStart+widthLen; j++)
    {
      int index = vertexBase + i*vertexSize+j;

      indexes->addIndex(index);  // tl
      if (outward)
      {
        indexes->addIndex(index+1);  // tr
        indexes->addIndex(index+vertexSize);  // bl
      }
      else
      {
        indexes->addIndex(index+vertexSize);  // bl
        indexes->addIndex(index+1);  // tr
      }

      indexes->addIndex(index+vertexSize);  // bl
      if (outward)
      {
        indexes->addIndex(index+1);  // tr
        indexes->addIndex(index+vertexSize+1);  // br
      }
      else
      {
        indexes->addIndex(index+vertexSize+1);  // br
        indexes->addIndex(index+1);  // tr
      }
    }
  }
}

//--------------------------------------------------------------
// create terrain
void GenerationShip::createTerrain()
{
  double ox = 1.0;// + m_center.x / 5000.0;
  double oy = 0.0; // m_center.x / 5000.0;
  double oz = 0.0; // m_center.x / 5000.0;

  int width = m_samples;
  int height = (int) ((m_samples * SHIP_LENGTH) / (2*PI*SHIP_RADIUS));

  int wall1 = width/6 + 10;
  int wall2 = wall1 + width/3;
  int wall3 = wall2 + width/3;

  int pointSize = width+3;  // -1 to width+1 
  double* heights = new double[(height+3)*pointSize];
  for (int i = -1; i <= height+1; i++)
  {
    double z = -0.5 + (i/(double) height);
    double nz = 2*z * SHIP_LENGTH / (2*SHIP_RADIUS);
    for (int j = -1; j <= width+1; j++)
    {
      double angle = (2*PI*j)/width;
      double x = cos(angle);
      double y = sin(angle);

      double thickness = noise(x/2.5+ox, y/2.5+oy, nz/2.5+oz);
      double ht = MIN_HEIGHT + max(0, 30+ 30* thickness);

      int wallDist = abs(j-wall1);
      wallDist = min(wallDist, abs(j-wall2));
      wallDist = min(wallDist, abs(j-wall3));
//      if (wallDist > 10)
//        wallDist = 100;
      ht = min(ht, wallDist*3);

      heights[(j+1)+pointSize*(i+1)] = ht / SYSTEM_FAR_SCALE;
    }
  }

  int vertexCount = (height+1)*(width+1);
  m_terrainVertexes = VertexTerrain::newBuffer(vertexCount);
  m_terrainIndexes = mgDisplay->newIndexBuffer(6*height*width, false, vertexCount > 65535);

  addPolygons(heights, width, height, m_terrainVertexes);

  int start = width/10;
  start = start/6;
  start = (start+2)*10;
  addGridIndexes(m_terrainIndexes, 0, width, height, start, width/3-10, false);
  addGridIndexes(m_terrainIndexes, 0, width, height, start+width/3, width/3-10, false);
  addGridIndexes(m_terrainIndexes, 0, width, height, start+2*width/3, width/3-start, false);
  addGridIndexes(m_terrainIndexes, 0, width, height, 0, start-10, false);

  delete heights;
}

//--------------------------------------------------------------
// create buffers ready to send to display
void GenerationShip::createBuffers()
{
  createShell();
  createWater();
  createLight();
  createTerrain();
}

//--------------------------------------------------------------
// delete display buffers
void GenerationShip::deleteBuffers()
{
  delete m_shellIndexes;
  m_shellIndexes = NULL;

  delete m_shellVertexes;
  m_shellVertexes = NULL;

  delete m_terrainIndexes;
  m_terrainIndexes = NULL;

  delete m_terrainVertexes;
  m_terrainVertexes = NULL;

  delete m_waterIndexes;
  m_waterIndexes = NULL;

  delete m_waterVertexes;
  m_waterVertexes = NULL;

  delete m_lightIndexes;
  m_lightIndexes = NULL;

  delete m_lightVertexes;
  m_lightVertexes = NULL;
}

//--------------------------------------------------------------
// animate object
BOOL GenerationShip::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// draw opaque data
void GenerationShip::render()
{
  mgDisplay->setCulling(false);
  
  mgDisplay->setShader("litTexture");
  mgDisplay->setTexture(m_shellTexture);
  mgDisplay->draw(MG_TRIANGLES, m_shellVertexes, m_shellIndexes);

  mgDisplay->setShader("terrain");
  mgDisplay->setTexture(m_terrainTexture);
  mgDisplay->draw(MG_TRIANGLES, m_terrainVertexes, m_terrainIndexes);

  mgDisplay->setMatColor(1.0, 1.0, 1.0);
  mgDisplay->setShader("unlitTexture");
  mgDisplay->setTexture(m_lightTexture);
  mgDisplay->draw(MG_TRIANGLES, m_lightVertexes, m_lightIndexes);

  mgDisplay->setCulling(true);
}

//--------------------------------------------------------------
// draw transparent data
void GenerationShip::renderTransparent()
{
  mgDisplay->setCulling(false);
  mgDisplay->setLighting(false);

  mgDisplay->setShader("unlitTexture");
  mgDisplay->setTexture(m_waterTexture);
  mgDisplay->draw(MG_TRIANGLES, m_waterVertexes, m_waterIndexes);

  mgDisplay->setMatColor(0.8, 0.8, 0.8);

  mgDisplay->setLighting(true);
  mgDisplay->setCulling(true);
}
