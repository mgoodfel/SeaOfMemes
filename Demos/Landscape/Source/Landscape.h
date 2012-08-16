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
#ifndef LANDSCAPE_H
#define LANDSCAPE_H

class StarrySky;
class Terrain;
class HelpUI;

const int GRID_TERRAIN    = 0;
const int GRID_CELLS      = 1;
const int GRID_CHUNKS     = 2;
const int GRID_STYLES     = 3;  // count of styles

#include "MovementApp.h"

class Landscape : public MovementApp
{
public:
  // constructor
  Landscape();

  // destructor
  virtual ~Landscape();

  //--- start of MovementApp subclass

  // key press
  virtual BOOL moveKeyDown(
    int keyCode,
    int modifiers);

  // key typed
  virtual BOOL moveKeyChar(
    int keyCode,
    int modifiers);

  //--- end of MovementApp subclass

  //--- start of mgApplication interface

  // initialize application
  virtual void appInit();

  // terminate application
  virtual void appTerm();

  // delete any display buffers
  virtual void appDeleteBuffers();

  // create buffers, ready to send to display
  virtual void appCreateBuffers();

  // update animation 
  virtual BOOL appViewAnimate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass
    
  // render the view
  virtual void appViewDraw();

  // report status for debug log
  virtual void appDebugStats(
    mgString& status);

  //--- end of mgApplication interface

protected:
  HelpUI* m_help;
  int m_threadCount;                      // max thread count

  mgPtrArray m_buildList;                 // list of chunks needing rebuild
  mgOSLock* m_buildLock;                  // access to rebuild list
  mgOSEvent* m_buildEvent;                // update to rebuild list
  mgOSThread* m_buildThreads;             // rebuild threads
  BOOL m_shutdown;

  mgPoint3 m_lightDir;
  mgPoint3 m_lightColor;
  mgPoint3 m_lightAmbient;

  mgPoint3 m_fogColor;
  double m_fogTopHeight;
  double m_fogTopInten;
  double m_fogBotHeight;
  double m_fogBotInten;
  double m_fogMaxDist;

  double m_eyeHeight;

  StarrySky* m_sky;

  BOOL m_mapView;
  int m_mapGrid;
  int m_terrainGrid;

  mgTextureImage* m_waterTexture;
  mgTextureImage* m_terrainTexture;
  mgTextureImage* m_gridTexture;
  mgTextureImage* m_newTexture;

  Terrain* m_terrain[3][3];
  int m_maxCount;

  BOOL m_daylight;

  // load texture patterns from options
  virtual void loadTextures();

  // add terrain if eye moved near edges
  virtual void checkTerrainBounds();

  // change resolution of terrain based on distance
  virtual void checkTerrainResolution();

  // update edge stepping
  virtual void checkTerrainEdges(
    Terrain* chunk);

  // create all terrain buffers
  virtual BOOL createTerrainBuffers();

  // delete all terrain buffers
  virtual void rebuildTerrainBuffers();

  // get list of terrain that needs rebuild
  virtual void terrainBuildList();

  // update the terrain after movement
  virtual BOOL updateTerrain();

  // draw the terrain
  virtual void renderTerrain();

  // return resolution of terrain at point
  virtual int resolutionAt(
    int x,
    int z);

  // rebuild a chunk of terrain off the buildList
  virtual BOOL rebuildTerrain();

  // set day or night
  virtual void setDayLight(
    BOOL daylight);

  friend void buildThreadProc(mgOSThread* thread, void* threadArg1, void* threadArg2);
};

#endif
