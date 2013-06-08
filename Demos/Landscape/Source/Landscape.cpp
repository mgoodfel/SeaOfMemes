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

// identify the program for the framework log
const char* mgProgramName = "Landscape";
const char* mgProgramVersion = "Part 83";

#include "StarrySky.h"
#include "VertexTerrain.h"
#include "Terrain.h"
#include "Landscape.h"
#include "HelpUI.h"

const double MIN_AVATAR_SPEED = 2.5/1000.0;   // units per ms
const double MAX_AVATAR_SPEED = MIN_AVATAR_SPEED * 128.0;  // units per ms

const int HORIZON_SIZE = 65536;  // must be power of two.  base unit assumed = 1 meter.
const int WATER_SIZE = 4096;  // must divide evenly into HORIZON_SIZE
const double MAPVIEW_HEIGHT = 20000.0;  // meters
const double MIN_HEIGHT = 1.0;  // meters above ground

//--------------------------------------------------------------
// rebuild chunks of terrain
void buildThreadProc(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2);

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set working directory to dir containing options
  mgOSFindWD("options.xml");

  return new Landscape();
}

//--------------------------------------------------------------
// constructor
Landscape::Landscape()
{
  m_buildThreads = NULL;
  m_buildEvent = NULL;
  m_buildLock = NULL;

  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      m_terrain[x][z] = NULL;
    }
  }

  m_sky = NULL;
  m_help = NULL;
}

//--------------------------------------------------------------
// destructor
Landscape::~Landscape()
{
}

//--------------------------------------------------------------
// initialize application
void Landscape::appInit()
{
  MovementApp::appInit();

  mgString title;
  title.format("%s, %s", mgProgramName, mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // create the help pane
  m_help = new HelpUI(m_options);
  m_help->setDebugApp(this);
  setUI(m_help);

  // load textures
  loadTextures();

  // load shaders
  m_terrainShader = VertexTerrain::loadShader("terrain");
  m_waterShader = mgVertex::loadShader("water");

  // create the sky
  m_lightDir = mgPoint3(0.0, 0.5, 1.0);
  m_lightColor = mgPoint3(0.8, 0.8, 0.8);
  m_lightAmbient = mgPoint3(0.4, 0.4, 0.4);

  m_fogColor = mgPoint3(0.8, 0.8, 0.8);
  m_fogMaxDist = 30000.0;
  m_fogTopHeight = 2000.0; // WATER_LEVEL + 128.0;// m_fogMaxDist;
  m_fogBotHeight = WATER_LEVEL; 
  m_fogBotInten = 1.0; 
  m_fogTopInten = 0.8;

  m_sky = new StarrySky(m_options);
  m_sky->setFogInten(m_fogBotInten, m_fogTopInten);

  m_sky->setMoonDir(m_lightDir);
  m_sky->setSunDir(m_lightDir);

  setDayLight(true);

  m_maxCount = 0;  // number of terrain chunks in use

  // generate the initial terrain
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      Terrain* chunk = new Terrain(
        (x-1) * HORIZON_SIZE - HORIZON_SIZE/2, 
        (z-1) * HORIZON_SIZE - HORIZON_SIZE/2,
        HORIZON_SIZE, HORIZON_SIZE);

      chunk->createHeightmap();
      m_terrain[x][z] = chunk;
    }
  }

  // subdivide terrain around the eye
  checkTerrainResolution();

  m_mapView = false;
  m_terrainGrid = GRID_TERRAIN;
  m_mapGrid = GRID_CHUNKS;

  m_eyeHeight = 1.75;
  m_eyePt = mgPoint3(-12000, 0, -12000); // nice area for initial coordinate

  double ht = Terrain::heightAtPt(m_eyePt.x, m_eyePt.z);
  ht = max(ht, WATER_LEVEL);
  m_eyePt.y = ht + m_eyeHeight;

  m_eyeRotX = 4.4;
  m_eyeRotY = 180;
  m_eyeRotZ = 0.0;

  // init threads
  m_threadCount = m_options.getInteger("threadCount", 1); 
#ifdef EMSCRIPTEN
  m_threadCount = 0;
#endif

  m_buildLock = mgOSLock::create();

  m_shutdown = false;
  if (m_threadCount > 0)
  {
    m_buildEvent = mgOSEvent::create();

    m_buildThreads = mgOSThread::create(m_threadCount, buildThreadProc, 
      mgOSThread::PRIORITY_LOW, this, NULL);
  }
}

//--------------------------------------------------------------
// terminate application
void Landscape::appTerm()
{
  m_shutdown = true;
  if (m_buildThreads != NULL)
  {
    m_buildEvent->signal();
    m_buildThreads->waitForEnd(200.0);  // wait for end

    delete m_buildThreads;
    m_buildThreads = NULL;

    delete m_buildEvent;
    m_buildEvent = NULL;
  }

  delete m_buildLock;
  m_buildLock = NULL;

  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      delete m_terrain[x][z];
      m_terrain[x][z] = NULL;
    }
  }

  delete m_sky;
  m_sky = NULL;

  delete m_help;
  m_help = NULL;

  MovementApp::appTerm();
}

//--------------------------------------------------------------------
// load texture patterns from options
void Landscape::loadTextures()
{
  mgString fileName;

  m_options.getFileName("waterTexture", m_options.m_sourceFileName, "waterTexture", fileName);
  m_waterTexture = mgDisplay->loadTexture(fileName);

  m_options.getFileName("terrainTexture", m_options.m_sourceFileName, "terrainTexture", fileName);
  m_terrainTexture = mgDisplay->loadTexture(fileName);

  m_options.getFileName("gridTexture", m_options.m_sourceFileName, "gridTexture", fileName);
  m_gridTexture = mgDisplay->loadTexture(fileName);

  m_options.getFileName("highlightTexture", m_options.m_sourceFileName, "highlightTexture", fileName);
  m_newTexture = mgDisplay->loadTexture(fileName);
}

//--------------------------------------------------------------------
// return resolution of terrain at point
int Landscape::resolutionAt(
  int ptx,
  int ptz)
{
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      Terrain* chunk = m_terrain[x][z];
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
void Landscape::checkTerrainEdges(
  Terrain* chunk)
{
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

//--------------------------------------------------------------------
// add terrain if eye moved near edges
void Landscape::checkTerrainBounds()
{
  // if we're in rightmost cell, extend to right
  if (m_eyePt.x > m_terrain[2][0]->m_originX)
  {
    for (int z = 0; z <= 2; z++)
    {
      // delete first cell, shift others left
      delete m_terrain[0][z];
      m_terrain[0][z] = m_terrain[1][z];
      m_terrain[1][z] = m_terrain[2][z];

      // add new terrain cell at right
      int originX = m_terrain[1][z]->m_originX + HORIZON_SIZE;
      int originZ = m_terrain[1][z]->m_originZ;
      Terrain* chunk = new Terrain(originX, originZ, HORIZON_SIZE, HORIZON_SIZE);

      m_terrain[2][z] = chunk;
    }
  }

  // if we're in leftmost cell, extend to left
  else if (m_eyePt.x < m_terrain[1][0]->m_originX)
  {
    for (int z = 0; z <= 2; z++)
    {
      // delete last cell, shift others right
      delete m_terrain[2][z];
      m_terrain[2][z] = m_terrain[1][z];
      m_terrain[1][z] = m_terrain[0][z];

      // add new terrain cell at right
      int originX = m_terrain[1][z]->m_originX - HORIZON_SIZE;
      int originZ = m_terrain[1][z]->m_originZ;
      Terrain* chunk = new Terrain(originX, originZ, HORIZON_SIZE, HORIZON_SIZE);

      m_terrain[0][z] = chunk;
    }
  }

  // if we're in bottommost cell, extend to bottom
  if (m_eyePt.z > m_terrain[0][2]->m_originZ)
  {
    for (int x = 0; x <= 2; x++)
    {
      // delete top cell, shift others up
      delete m_terrain[x][0];
      m_terrain[x][0] = m_terrain[x][1];
      m_terrain[x][1] = m_terrain[x][2];

      // add new terrain cell at bottom
      int originX = m_terrain[x][1]->m_originX;
      int originZ = m_terrain[x][1]->m_originZ + HORIZON_SIZE;
      Terrain* chunk = new Terrain(originX, originZ, HORIZON_SIZE, HORIZON_SIZE);

      m_terrain[x][2] = chunk;
    }
  }

  // if we're in topmost cell, extend to top
  else if (m_eyePt.z < m_terrain[0][1]->m_originZ)
  {
    for (int x = 0; x <= 2; x++)
    {
      // delete bottom cell, shift others down
      delete m_terrain[x][2];
      m_terrain[x][2] = m_terrain[x][1];
      m_terrain[x][1] = m_terrain[x][0];

      // add new terrain cell at bottom
      int originX = m_terrain[x][1]->m_originX;
      int originZ = m_terrain[x][1]->m_originZ - HORIZON_SIZE;
      Terrain* chunk = new Terrain(originX, originZ, HORIZON_SIZE, HORIZON_SIZE);

      m_terrain[x][0] = chunk;
    }
  }
}

//--------------------------------------------------------------------
// change resolution of terrain based on distance
void Landscape::checkTerrainResolution()
{
  int count = 0;
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      m_terrain[x][z]->checkResolution(m_eyePt);
      count += m_terrain[x][z]->count();
    }
  }

  m_maxCount = max(m_maxCount, count);
}

//--------------------------------------------------------------
// sort the rebuild array, smallest to largest extent
int rebuildSort(
  const void* a, 
  const void* b)
{
  const Terrain* chunkA = *(Terrain**) a;
  const Terrain* chunkB = *(Terrain**) b;

  return chunkB->m_extentX - chunkA->m_extentX;
}

//--------------------------------------------------------------------
// get list of terrain that needs rebuild
void Landscape::terrainBuildList()
{
  m_buildList.removeAll();

  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      m_terrain[x][z]->buildList(m_buildList);
    }
  }
  m_buildList.sort(rebuildSort);
}

//--------------------------------------------------------------
// rebuild chunks of terrain
void buildThreadProc(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2)
{
  Landscape* landscape = (Landscape*) threadArg1;

  while (!landscape->m_shutdown)
  {
    landscape->m_buildEvent->wait(100.0);  // check shutdown every 100 ms

    // while there's work
    while (!landscape->m_shutdown)
    {
      landscape->m_buildLock->lock();
      BOOL moreWork = landscape->rebuildTerrain();
      landscape->m_buildLock->unlock();

      if (!moreWork)
        break;
    }
  }
}

//--------------------------------------------------------------
// rebuild a chunk of terrain off the buildList
BOOL Landscape::rebuildTerrain()
{
  BOOL moreWork = false;

  // rebuild a chunk off the build list
  if (m_buildList.length() > 0)
  {
    Terrain* chunk = (Terrain*) m_buildList.pop();
    chunk->createHeightmap();
    chunk->m_rebuildBuffers = true;

    moreWork = m_buildList.length() > 0;
  }

  return moreWork;
}

//--------------------------------------------------------------------
// update the terrain after movement
BOOL Landscape::updateTerrain()
{
  BOOL changed = false;

  m_buildLock->lock();

  checkTerrainBounds();
  checkTerrainResolution();

  // check the edges on all chunks
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      checkTerrainEdges(m_terrain[x][z]);
    }
  }

  // get list of chunks needing rebuild (finer res height map)
  terrainBuildList();

  // if we requested chunks, activate threads
  if (m_buildList.length() > 0)
  {
    if (m_buildThreads != NULL)
    {
      // signal the threads to look at buildList
      m_buildEvent->signal();
    }
    else rebuildTerrain();
  }

  changed |= createTerrainBuffers();

  m_buildLock->unlock();

  return changed;
}

//--------------------------------------------------------------------
// animate the view
BOOL Landscape::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  // construct eye matrix
  m_eyeMatrix.loadIdentity();
  if (m_mapView)
  {
    m_eyeMatrix.rotateZDeg(m_eyeRotZ);
    m_eyeMatrix.rotateYDeg(m_eyeRotY);
  }
  else
  {
    m_eyeMatrix.rotateZDeg(m_eyeRotZ);
    m_eyeMatrix.rotateYDeg(m_eyeRotY);
    m_eyeMatrix.rotateXDeg(m_eyeRotX);
  }

  // update position
  BOOL changed = updateMovement(now, since);

  // update vertical position of eye based on landscape
  if (changed)
  {
    double ht = Terrain::heightAtPt(m_eyePt.x, m_eyePt.z);
    ht = max(ht, WATER_LEVEL);

    m_eyePt.y = max(m_eyePt.y, ht + MIN_HEIGHT);
  }

  // mark changed if mouse moved
  changed |= m_eyeChanged;
  m_eyeChanged = false;

  // animate the sky
  changed |= m_sky->animate(now, since);

  // update the terrain after movement
  changed |= updateTerrain();

  if (m_ui != NULL)
    changed |= m_ui->animate(now, since);

  return changed;
}

//--------------------------------------------------------------------
// draw the scene
void Landscape::renderTerrain()
{
  mgPoint3 renderEyePt(m_eyePt);
  if (m_mapView)
    renderEyePt.y = MAPVIEW_HEIGHT;

  mgTextureImage* texture = NULL;
  mgTextureImage* highlight = NULL;
  int gridStyle = m_mapView ? m_mapGrid : m_terrainGrid;
  switch (gridStyle)
  {
    case GRID_TERRAIN: 
      texture = m_terrainTexture;
      highlight = texture;  // don't highlight new terrain
      break;

    case GRID_CELLS: 
    case GRID_CHUNKS: 
      texture = m_gridTexture;
      highlight = m_newTexture;
      break;
  }

  mgDisplay->setCulling(false);

  mgDisplay->setTexture(texture);
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
      m_terrain[x][z]->render(renderEyePt, texture, highlight);
    }
  }

  if (!m_mapView && gridStyle == GRID_TERRAIN)
  {
    mgDisplay->setTransparent(true);
    mgDisplay->setShader(m_waterShader);
    mgDisplay->setShaderUniform(m_waterShader, "fogColor", mgPoint4(m_fogColor.x, m_fogColor.y, m_fogColor.z, 1.0));
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
}

//--------------------------------------------------------------------
// render the view
void Landscape::appViewDraw()
{
  mgPoint3 renderEyePt(m_eyePt);
  if (m_mapView)
    renderEyePt.y = MAPVIEW_HEIGHT;

  // draw the sky
  mgDisplay->setFrontAndBack(0.25, 16384);

  mgMatrix4 identity;
  m_eyeMatrix.loadIdentity();
  if (m_mapView)
  {
    m_eyeMatrix.rotateZDeg(m_eyeRotZ);
    m_eyeMatrix.rotateYDeg(m_eyeRotY);
    m_eyeMatrix.rotateXDeg(-90);
  }
  else
  {
    m_eyeMatrix.rotateZDeg(m_eyeRotZ);
    m_eyeMatrix.rotateYDeg(m_eyeRotY);
    m_eyeMatrix.rotateXDeg(m_eyeRotX);
  }

  mgDisplay->setEyeMatrix(m_eyeMatrix);
  mgDisplay->setEyePt(mgPoint3(0,0,0));

  mgDisplay->setModelTransform(identity);
  mgDisplay->setCulling(true);
  
  m_sky->setFogColor(m_fogColor);
  m_sky->setFogHeight(m_fogBotHeight - renderEyePt.y, m_fogTopHeight - renderEyePt.y);
  m_sky->setFogDist(HORIZON_SIZE, m_fogMaxDist);
  m_sky->render();

  // reset state after any changes in sky render
  mgDisplay->setLightDir(m_lightDir.x, m_lightDir.y, m_lightDir.z);
  mgDisplay->setMatColor(1.0, 1.0, 1.0);
  mgDisplay->setLightColor(m_lightColor.x, m_lightColor.y, m_lightColor.z);
  mgDisplay->setLightAmbient(m_lightAmbient.x, m_lightAmbient.y, m_lightAmbient.z);
  mgDisplay->setTransparent(false);

  mgDisplay->setEyeMatrix(m_eyeMatrix);
  mgDisplay->setEyePt(mgPoint3(0,0,0));  // we use relative coordinates in render

  if (mgDisplay->getDepthBits() == 32)
  {
    mgDisplay->setFrontAndBack(0.25, 65536*4);

    renderTerrain();
  }
  else if (mgDisplay->getDepthBits() >= 24)
  {
    mgDisplay->setFrontAndBack(256, 65536*4);

    renderTerrain();

    mgDisplay->setFrontAndBack(0.25, 256);
    mgDisplay->clearBuffer(MG_DEPTH_BUFFER);

    renderTerrain();
  }
  else
  {
    mgDisplay->setFrontAndBack(4096, 65536*4);

    renderTerrain();

    mgDisplay->setFrontAndBack(32, 4096);
    mgDisplay->clearBuffer(MG_DEPTH_BUFFER);

    renderTerrain();

    mgDisplay->setFrontAndBack(0.25, 32);
    mgDisplay->clearBuffer(MG_DEPTH_BUFFER);

    renderTerrain();
  }
}

//-----------------------------------------------------------------------------
// create all terrain buffers
BOOL Landscape::createTerrainBuffers()
{
  BOOL changed = false;
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      changed |= m_terrain[x][z]->createBuffers(m_mapView, m_mapView ? m_mapGrid : m_terrainGrid);
    }
  }
  return changed;
}

//-----------------------------------------------------------------------------
// delete all terrain buffers
void Landscape::rebuildTerrainBuffers()
{
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      m_terrain[x][z]->rebuildBuffers();
    }
  }
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void Landscape::appCreateBuffers()
{
  m_sky->createBuffers();

  createTerrainBuffers();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void Landscape::appDeleteBuffers()
{
  m_sky->deleteBuffers();

  // delete the terrain buffers
  for (int x = 0; x <= 2; x++)
  {
    for (int z = 0; z <= 2; z++)
    {
      m_terrain[x][z]->deleteBuffers();
    }
  }
}

//-----------------------------------------------------------------------------
// key press
BOOL Landscape::moveKeyDown(
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case MG_EVENT_KEY_F1:
      if (m_ui != NULL)
        m_help->toggleHelp();
      return true;

    case MG_EVENT_KEY_F2:
      if (m_ui != NULL)
        m_help->toggleConsole();
      return true;

    case MG_EVENT_KEY_F6:
      mgDebug("stop");
      return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// key typed
BOOL Landscape::moveKeyChar(
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case 'n':
    case 'N':
      setDayLight(!m_daylight);
      return true;

    case 'G':
    case 'g':
      // change the grid style
      if (m_mapView)
        m_mapGrid = (m_mapGrid+1) % GRID_STYLES;
      else m_terrainGrid = (m_terrainGrid+1) % GRID_STYLES;

      rebuildTerrainBuffers();
      createTerrainBuffers();
      m_eyeChanged = true;  // update the view
      return true;

    case 'M':
    case 'm':
      m_mapView = !m_mapView;

      if (m_mapGrid != m_terrainGrid)
      {
        rebuildTerrainBuffers();
        createTerrainBuffers();
      }

//      if (m_mapView)
//        mgDisplay->setCursorTexture(m_mapCursor->m_texture, m_mapCursor->m_hotX, m_mapCursor->m_hotY);
//      else mgDisplay->setCursorTexture(m_viewCursor->m_texture, m_viewCursor->m_hotX, m_viewCursor->m_hotY);

      m_eyeChanged = true;  // update the view
      return true;

    case '=': 
    case '+':
    {
      m_moveSpeed = m_moveSpeed * 2.0;
      m_moveSpeed = min(MAX_AVATAR_SPEED, m_moveSpeed);
      if (m_help != NULL)
        m_help->addLine("Speed = %g kph (%g mph)", floor(0.5+m_moveSpeed*60*60), floor(0.5+m_moveSpeed*60*60*0.621371192));
      return true;
    }
      
    case '-':
    {
      m_moveSpeed = m_moveSpeed / 2.0;
      m_moveSpeed = max(MIN_AVATAR_SPEED, m_moveSpeed);
      if (m_help != NULL)
        m_help->addLine("Speed = %g kph (%g mph)", floor(0.5+m_moveSpeed*60*60), floor(0.5+m_moveSpeed*60*60*0.621371192));
      break;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
// report status for debug log
void Landscape::appDebugStats(
  mgString& status)
{
  status.format("max count = %d", m_maxCount);
}

//-----------------------------------------------------------------------------
// set day or night
void Landscape::setDayLight(
  BOOL daylight)
{
  m_daylight = daylight;

  m_sky->enableFog(true);
  if (m_daylight)
  {
    m_lightColor = mgPoint3(0.8, 0.8, 0.8);
    m_fogColor = mgPoint3(0.8, 0.8, 0.8);
  }
  else 
  {
    m_lightColor = mgPoint3(0.2, 0.2, 0.2);
    m_fogColor = mgPoint3(0.2, 0.2, 0.2);
  }

  m_sky->enableSkyBox(m_daylight);
  m_sky->enableSun(m_daylight);

  m_sky->enableStars(!m_daylight);
  m_sky->enableMoon(!m_daylight);

  m_eyeChanged = true;
}
