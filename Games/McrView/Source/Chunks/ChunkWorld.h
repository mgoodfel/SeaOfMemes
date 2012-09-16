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
#ifndef CHUNKWORLD_H
#define CHUNKWORLD_H

class HitObj;
class BrickSetFile;
class BrickSet;
class ShapeDefn;
class ChunkObj;
class ChunkOrigin;
class MinecraftRegion;

class McrView; // =-= debug 

#include "ChunkList.h"

class ChunkWorld 
{
public:
  // constructor
  ChunkWorld(
    const mgOptionsFile& options);

  // destructor
  virtual ~ChunkWorld();

  // delete any display buffers
  virtual void deleteBuffers();

  // create buffers ready to send to display
  virtual void createBuffers();

  // set the fog color
  virtual void setFogColor(
    const mgPoint3& color)
  {
    m_fogColor.x = color.x;
    m_fogColor.y = color.y;
    m_fogColor.z = color.z;
    m_fogColor.w = 1.0;
  }

  // set fog height
  virtual void setFogHeight(
    double botHeight, 
    double topHeight)
  {
    m_fogBotHeight = botHeight;
    m_fogTopHeight = topHeight;
  }

  // set fog intensity
  virtual void setFogInten(
    double botInten, 
    double topInten)
  {
    m_fogBotInten = botInten;
    m_fogTopInten = topInten;
  }

  // set fog distance
  virtual void setFogDist(
    double maxDist)
  {
    m_fogMaxDist = maxDist;
  }

  // set the sky color
  virtual void setSkyColor(
    const mgPoint3& color);

  // set the torch color
  virtual void setTorchColor(
    const mgPoint3& color)
  {
    m_torchColor = color;
  }

  // set the eye point
  virtual void setEyePt(
    const mgPoint3& eyePt);

  // update animation 
  virtual BOOL animate(
    double now,                       // current time (ms)
    double since);                    // milliseconds since last pass

  // render the world
  virtual void render();

  // render the world transparent data
  virtual void renderTransparent();

  // write stats to mgDebug
  virtual void debugStats(
    mgString& status);

  // return triangle count for current scene
  virtual int triangleCount();

protected:
  // minecraft region dir
  mgString m_regionDir;

  BrickSet* m_brickSet;                 // brick definitions
  mgMapStringToPtr m_shapes;            // loaded shapes

  mgMapStringToPtr m_textureIds;        // texture filename to index
  int m_nextTextureId;
  mgStringArray m_textureFiles;  
  mgTextureArray* m_textureArray;       // array of textures to use

  // limits on memory use
  int m_systemMemLimit;
  int m_displayMemLimit;

  int m_viewDistance;                     // max view distance
  int m_sortCount;                        // number of chunks to sort per refresh
  int m_threadCount;                      // max thread count

  mgOSLock* m_chunkLock;                  // access to chunks 
  mgOSEvent* m_chunkEvent;                // update to chunk work list
  mgOSThread* m_chunkThreads;             // chunk loading threads

  BOOL m_chunksChanged;                   // chunks added or deleted from view
  BOOL m_shutdown;

  // hashtable of all MinecraftRegion seen
  mgMapXYZToPtr m_regionTable;
  mgPtrArray m_openRegions;

  // hashtable of all ChunkObj seen
  mgMapXYZToPtr m_chunkTable;             

  // view distance chunk offsets
  ChunkOrigin* m_viewList;
  int m_viewListSize;
  int m_viewListCount;

  // requested chunk list
  ChunkList m_requestList;

  // LRU list of chunks
  ChunkList m_LRUList;

  // memory use
  int m_displayMemUsed;
  int m_systemMemUsed;

  // current eye point
  mgPoint3 m_eyePt;

  mgPoint3 m_skyColor;
  mgPoint3 m_torchColor;
  mgPoint4 m_fogColor;
  double m_fogBotHeight;
  double m_fogBotInten;
  double m_fogTopHeight;
  double m_fogTopInten;
  double m_fogMaxDist;

  mgTextureImage* m_waterTexture;
  mgVertexBuffer* m_waterVertexes;
  mgVertexBuffer* m_horizonVertexes;

  // load the shaders
  virtual void loadShaders();

  // find the minecraft world directory
  virtual void findWorldDir(
    const mgOptionsFile& options);

  // create lock, event, threads
  virtual void createWorkers();

  // assign an id to a texture
  virtual int findTexture(
    const char* fileName);

  // read a brick set for the world
  virtual void readBrickSet(
    BrickSetFile& brickSetFile);

  // combine minor and major definitions, check for missing info
  virtual void checkBrickSet();

  // read a shape file, convert to ShapeDefn
  virtual ShapeDefn* readShape(
    const char* fileName);

  // create view chunk offsets to distance
  virtual void createViewList(
    int distance);

  // check system and display memory use
  virtual void checkMemory();

  // add to request list
  virtual void requestChunk(
    ChunkObj* chunk);

  // reset the request list
  virtual void resetRequestList();

  // take a request off the queue
  virtual ChunkObj* dequeueRequest();

  // process one chunk in request queue
  virtual void processRequest(
    ChunkObj* chunk);

  // load chunks from active list
  virtual void chunkLoader();

  // return a minecraft region
  virtual MinecraftRegion* getRegion(
    int regionX,
    int regionZ);

  // get a chunk of the world
  virtual ChunkObj* getChunk(
    int chunkX,
    int chunkZ);

  // find/create a chunk of the world
  ChunkObj* createChunk(
    int chunkX,
    int chunkZ);

  // return true if all neighbors in memory
  virtual BOOL loadNeighbors(
    ChunkObj* chunk);

  // update the edges of the chunk from neighbors
  virtual void updateEdges(
    ChunkObj* chunk);

  // create water vertexes
  virtual void createWater();

  // create horizon vertexes
  virtual void createHorizon();

  // draw water as standin for chunk
  virtual void drawWater(
    int viewX,
    int viewZ);

  // draw horizon beyond chunks
  virtual void drawHorizon(
    int viewX,
    int viewZ);

  // find the water texture in the brick set
  virtual void loadWaterTexture(
    BrickSetFile& brickSetFile);

  friend class ChunkObj;
  friend void chunkThreadProc(mgOSThread* thread, void* threadArg1, void* threadArg2);

  friend class McrView;  // =-= debug
};

#endif
