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

#include "Util/VertexTerrain.h"
#include "PlanetTerrain.h"
#include "PlanetLandscape.h"

const int HORIZON_SIZE = 65536;  // must be power of two.  base unit assumed = 1 meter.
const int WATER_SIZE = 4096;  // must divide evenly into HORIZON_SIZE

//--------------------------------------------------------------
// constructor
PlanetLandscape::PlanetLandscape()
{
  // load shaders
  m_terrainShader = VertexTerrain::loadShader("terrain");
  m_waterShader = mgVertex::loadShader("water");

  mgString fileName;

/*
  m_options.getFileName("waterTexture", m_options.m_sourceFileName, "", fileName);
  m_waterTexture = mgDisplay->loadTexture(fileName);

  m_options.getFileName("terrainTexture", m_options.m_sourceFileName, "", fileName);
  m_terrainTexture = mgDisplay->loadTexture(fileName);

  m_options.getFileName("gridTexture", m_options.m_sourceFileName, "", fileName);
  m_gridTexture = mgDisplay->loadTexture(fileName);

  m_options.getFileName("highlightTexture", m_options.m_sourceFileName, "", fileName);
  m_newTexture = mgDisplay->loadTexture(fileName);
*/

  // create the sky
  m_lightDir = mgPoint3(0.0, 0.5, 1.0);
  m_lightColor = mgPoint3(0.8, 0.8, 0.8);
  m_lightAmbient = mgPoint3(0.2, 0.2, 0.2);

  m_fogColor = mgPoint3(0.8, 0.8, 0.8);
  m_fogMaxDist = 13000.0;
  m_fogTopHeight = 2000.0; // WATER_LEVEL + 128.0;// m_fogMaxDist;
  m_fogBotHeight = WATER_LEVEL; 
  m_fogBotInten = 1.0; 
  m_fogTopInten = 0.8;
}


//--------------------------------------------------------------
// destructor
PlanetLandscape::~PlanetLandscape()
{
}

//--------------------------------------------------------------------
// generate the initial terrain
void PlanetLandscape::initTerrain()
{
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      TerrainCell* chunk = new PlanetTerrain(
        (x-1) * HORIZON_SIZE - HORIZON_SIZE/2, 
        (z-1) * HORIZON_SIZE - HORIZON_SIZE/2,
        HORIZON_SIZE, HORIZON_SIZE);

//      chunk->createHeightmap();
      m_terrain[x][z] = chunk;
    }
  }

  // subdivide terrain around the eye  =-= 
  checkTerrainResolution(mgPoint3(0,0,0));
}

//--------------------------------------------------------------------
// draw the scene
void PlanetLandscape::renderTerrain(
  const mgPoint3& renderEyePt)
{
  mgDisplay->setCulling(false);

  mgDisplay->setTexture(m_terrainTexture);
  mgDisplay->setShader(m_terrainShader);
  mgDisplay->setShaderUniform(m_terrainShader, "fogColor", mgPoint4(m_fogColor.x, m_fogColor.y, m_fogColor.z, 1.0));
  mgDisplay->setShaderUniform(m_terrainShader, "fogBotHeight", (float) (m_fogBotHeight - renderEyePt.y));
  mgDisplay->setShaderUniform(m_terrainShader, "fogBotInten", (float) m_fogBotInten);
  mgDisplay->setShaderUniform(m_terrainShader, "fogTopHeight", (float) (m_fogTopHeight - renderEyePt.y));
  mgDisplay->setShaderUniform(m_terrainShader, "fogTopInten", (float) m_fogTopInten);
  mgDisplay->setShaderUniform(m_terrainShader, "fogMaxDist", (float) m_fogMaxDist);

  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      m_terrain[x][z]->render(renderEyePt, m_terrainTexture);
    }
  }

  mgDisplay->setTransparent(true);
  mgDisplay->setShader(m_waterShader);
  mgDisplay->setShaderUniform(m_waterShader, "fogColor", m_fogColor);
  mgDisplay->setShaderUniform(m_waterShader, "fogBotHeight", (float) (m_fogBotHeight - renderEyePt.y));
  mgDisplay->setShaderUniform(m_waterShader, "fogBotInten", (float) m_fogBotInten);
  mgDisplay->setShaderUniform(m_waterShader, "fogTopHeight", (float) (m_fogTopHeight - renderEyePt.y));
  mgDisplay->setShaderUniform(m_waterShader, "fogTopInten", (float) m_fogTopInten);
  mgDisplay->setShaderUniform(m_waterShader, "fogMaxDist", (float) m_fogMaxDist);

  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      m_terrain[x][z]->renderTransparent(renderEyePt, m_waterTexture);
    }
  }

  mgDisplay->setTransparent(false);
}

//--------------------------------------------------------------------
// add terrain if eye moved near edges
void PlanetLandscape::checkTerrainBounds(
  const mgPoint3& eyePt)
{
  // if we're in rightmost cell, extend to right
  if (eyePt.x > terrainAt(2, 0)->m_originX)
  {
    for (int z = 0; z <= 2; z++)
    {
      // delete first cell, shift others left
      delete m_terrain[0][z];
      m_terrain[0][z] = m_terrain[1][z];
      m_terrain[1][z] = m_terrain[2][z];

      // add new terrain cell at right
      int originX = terrainAt(1, z)->m_originX + HORIZON_SIZE;
      int originZ = terrainAt(1, z)->m_originZ;
      TerrainCell* chunk = new PlanetTerrain(originX, originZ, HORIZON_SIZE, HORIZON_SIZE);

      m_terrain[2][z] = chunk;
    }
  }

  // if we're in leftmost cell, extend to left
  else if (eyePt.x < terrainAt(1, 0)->m_originX)
  {
    for (int z = 0; z <= 2; z++)
    {
      // delete last cell, shift others right
      delete m_terrain[2][z];
      m_terrain[2][z] = m_terrain[1][z];
      m_terrain[1][z] = m_terrain[0][z];

      // add new terrain cell at right
      int originX = terrainAt(1, z)->m_originX - HORIZON_SIZE;
      int originZ = terrainAt(1, z)->m_originZ;
      TerrainCell* chunk = new PlanetTerrain(originX, originZ, HORIZON_SIZE, HORIZON_SIZE);

      m_terrain[0][z] = chunk;
    }
  }

  // if we're in bottommost cell, extend to bottom
  if (eyePt.z > terrainAt(0, 2)->m_originZ)
  {
    for (int x = 0; x <= 2; x++)
    {
      // delete top cell, shift others up
      delete m_terrain[x][0];
      m_terrain[x][0] = m_terrain[x][1];
      m_terrain[x][1] = m_terrain[x][2];

      // add new terrain cell at bottom
      int originX = terrainAt(x, 1)->m_originX;
      int originZ = terrainAt(x, 1)->m_originZ + HORIZON_SIZE;
      TerrainCell* chunk = new PlanetTerrain(originX, originZ, HORIZON_SIZE, HORIZON_SIZE);

      m_terrain[x][2] = chunk;
    }
  }

  // if we're in topmost cell, extend to top
  else if (eyePt.z < terrainAt(0, 1)->m_originZ)
  {
    for (int x = 0; x <= 2; x++)
    {
      // delete bottom cell, shift others down
      delete m_terrain[x][2];
      m_terrain[x][2] = m_terrain[x][1];
      m_terrain[x][1] = m_terrain[x][0];

      // add new terrain cell at bottom
      int originX = terrainAt(x, 1)->m_originX;
      int originZ = terrainAt(x, 1)->m_originZ - HORIZON_SIZE;
      TerrainCell* chunk = new PlanetTerrain(originX, originZ, HORIZON_SIZE, HORIZON_SIZE);

      m_terrain[x][0] = chunk;
    }
  }
}

//--------------------------------------------------------------------
// return resolution of terrain at point
int PlanetLandscape::resolutionAt(
  int ptx,
  int ptz)
{
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      PlanetTerrain* chunk = terrainAt(x, z);
      if (ptx >= chunk->m_originX && ptx < (chunk->m_originX + chunk->m_extentX) &&
          ptz >= chunk->m_originZ && ptz < (chunk->m_originZ + chunk->m_extentZ))
        return chunk->resolutionAt(ptx, ptz);
    }
  }

  // if outside grid, return min resolution
  return HORIZON_SIZE;
}

//--------------------------------------------------------------------
// update edge stepping
void PlanetLandscape::checkTerrainEdges(
  TerrainCell* cell)
{
  PlanetTerrain* chunk = (PlanetTerrain*) cell;
  if (chunk->m_children[0] == NULL)
  {
    BOOL updateChunk = false;

    // we only care about stepping in high res chunks next to lower res ones
    int xminStep = resolutionAt(chunk->m_originX - 1, chunk->m_originZ) / chunk->m_extentX;
    xminStep = max(1, xminStep);

    if (xminStep != chunk->m_xminStep)
    {
      chunk->m_xminStep = xminStep;
      updateChunk = true;
    }

    int xmaxStep = resolutionAt(chunk->m_originX + chunk->m_extentX, chunk->m_originZ) / chunk->m_extentX;
    xmaxStep = max(1, xmaxStep);

    if (xmaxStep != chunk->m_xmaxStep)
    {
      chunk->m_xmaxStep = xmaxStep;
      updateChunk = true;
    }

    int zminStep = resolutionAt(chunk->m_originX, chunk->m_originZ - 1) / chunk->m_extentZ;
    zminStep = max(1, zminStep);

    if (zminStep != chunk->m_zminStep)
    {
      chunk->m_zminStep = zminStep;
      updateChunk = true;
    }

    int zmaxStep = resolutionAt(chunk->m_originX, chunk->m_originZ + chunk->m_extentZ) / chunk->m_extentZ;
    zmaxStep = max(1, zmaxStep);

    if (zmaxStep != chunk->m_zmaxStep)
    {
      chunk->m_zmaxStep = zmaxStep;
      updateChunk = true;
    }

    if (updateChunk)
      chunk->deleteBuffers();
  }
  else
  {
    // recurse to children
    for (int i = 0; i < 4; i++)
      checkTerrainEdges(chunk->m_children[i]);
  }
}

