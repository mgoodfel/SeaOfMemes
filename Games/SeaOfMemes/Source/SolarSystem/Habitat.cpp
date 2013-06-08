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
#include "SolarSystem.h"
#include "Habitat.h"

const double LIGHT_RADIUS = 5.0 / SYSTEM_FAR_SCALE;

const double WATER_LEVEL = 7.0;
const double MIN_HEIGHT = 0.0;

//--------------------------------------------------------------
// constructor
Habitat::Habitat(
  const mgOptionsFile& options,
  const mgPoint3& center,
  double radius)
{
  m_lightShader = mgVertex::loadShader("unlitTexture");
  m_waterShader = mgVertex::loadShader("unlitTexture");
  m_terrainShader = VertexTerrain::loadShader("terrain");
  m_shellShader = VertexTerrain::loadShader("terrain");

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
  m_radius = radius;
  m_samples = 64;

  m_terrainRadius = m_radius/2.0;
  m_terrainLength = 2*m_radius*0.75;
  m_variation = 10.0/SYSTEM_FAR_SCALE;
  m_outsideHeight = 0.4;
  m_insideHeight = 0.2;

  m_shellIndexes = NULL;
  m_shellVertexes = NULL;

  m_terrainIndexes = NULL;
  m_terrainVertexes = NULL;

  m_waterIndexes = NULL;
  m_waterVertexes = NULL;

  m_lightIndexes = NULL;
  m_lightVertexes = NULL;
}

//--------------------------------------------------------------
// destructor
Habitat::~Habitat()
{
  deleteBuffers();
}
    
//--------------------------------------------------------------
// return outside diameter of object
void Habitat::shellPt(
  mgPoint3& pt,
  double x,
  double y,
  double z)
{                                                      
  double ox = m_center.x / SYSTEM_FAR_SCALE;
  double oy = m_center.y / SYSTEM_FAR_SCALE;
  double oz = m_center.z / SYSTEM_FAR_SCALE;

  double dist = sqrt(x*x + y*y + z*z);
  double nx = x/dist;
  double ny = y/dist;
  double nz = z/dist;

  double ht = m_radius - m_outsideHeight*mgSimplexNoise::noiseSum(10, nx+ox, ny+oy, nz+oz);
  double thickness = m_variation*mgSimplexNoise::noise(nx*5+ox, ny*5+oy, nz*5+oz);
  ht += thickness;

  // cut the ends of the central shaft
  if ((x*x + y*y) < 0.04)
    ht = 0.0;

  pt.x = nx*ht;
  pt.y = ny*ht;
  pt.z = nz*ht;
}

//--------------------------------------------------------------
// create the polygons for a height map
void Habitat::addShellPolygons(
  mgPoint3* points,
  BOOL outward)
{
  // generate the vertex list.
  int depthSize = m_samples+3;  // -1 to size+1
  int vertexSize = m_samples+1;   // 0 to size

  int vertexBase = m_shellVertexes->getLength();

  VertexTerrain v;
  v.setColor(133, 133, 133);  // gray rock
  for (int i = 0; i < vertexSize; i++)
  {
    for (int j = 0; j < vertexSize; j++)
    {
      int index = (i+1)*depthSize + (j+1);
      mgPoint3 pt = points[index];
      double ht = pt.length() - m_radius;
      v.m_px = (float) (pt.x + m_center.x);
      v.m_py = (float) (pt.y + m_center.y);
      v.m_pz = (float) (pt.z + m_center.z);

      mgPoint3 westPt = points[index-depthSize];
      mgPoint3 eastPt = points[index+depthSize];
      mgPoint3 northPt = points[index-1];
      mgPoint3 southPt = points[index+1];
      northPt.subtract(pt);
      southPt.subtract(pt);
      westPt.subtract(pt);
      eastPt.subtract(pt);

      mgPoint3 normal(northPt);
      normal.cross(westPt);
      normal.normalize();

      v.m_nx = (float) normal.x;
      v.m_ny = (float) normal.y;
      v.m_nz = (float) normal.z;

      normal = westPt;
      normal.cross(southPt);
      normal.normalize();

      v.m_nx += (float) normal.x;
      v.m_ny += (float) normal.y;
      v.m_nz += (float) normal.z;

      normal = southPt;
      normal.cross(eastPt);
      normal.normalize();

      v.m_nx += (float) normal.x;
      v.m_ny += (float) normal.y;
      v.m_nz += (float) normal.z;

      normal = eastPt;
      normal.cross(northPt);
      normal.normalize();

      v.m_nx += (float) normal.x;
      v.m_ny += (float) normal.y;
      v.m_nz += (float) normal.z;

      if (outward)
      {
        v.m_nx /= 4.0f;
        v.m_ny /= 4.0f;
        v.m_nz /= 4.0f;
      }
      else
      {
        v.m_nx /= -4.0f;
        v.m_ny /= -4.0f;
        v.m_nz /= -4.0f;
      }

      v.m_tx = (float) i;
      v.m_ty = (float) j;

      v.addTo(m_shellVertexes);
    }
  }

  for (int i = 0; i < m_samples; i++)
  {
    for (int j = 0; j < m_samples; j++)
    {
      int index = vertexBase + i*vertexSize+j;

      mgPoint3 tl = points[(i+1)*depthSize + (j+1)];
      mgPoint3 tr = points[(i+1)*depthSize + (j+2)];
      mgPoint3 bl = points[(i+2)*depthSize + (j+1)];
      mgPoint3 br = points[(i+2)*depthSize + (j+2)];
      if (tl.length() < 0.001 ||
          tr.length() < 0.001 ||
          bl.length() < 0.001 ||
          br.length() < 0.001)
        continue;

      m_shellIndexes->addIndex(index);  // tl
      m_shellIndexes->addIndex(index+1);  // tr
      m_shellIndexes->addIndex(index+vertexSize);  // bl

      m_shellIndexes->addIndex(index+vertexSize);  // bl
      m_shellIndexes->addIndex(index+1);  // tr
      m_shellIndexes->addIndex(index+vertexSize+1);  // br
    }
  }
}

//--------------------------------------------------------------
// create outside surface for chunk
void Habitat::createShell()
{
  // build shell as deformed sphere
  VertexTerrain v;
  int lenSamples = m_samples;
  int shellSamples = lenSamples+10;  // add barrier points
  int arcSamples = m_samples*2;

  int vertexCount = (shellSamples+1)*(arcSamples+1);
  m_shellVertexes = VertexTerrain::newBuffer(vertexCount);

  double exitHeight = m_radius * sin((PI*20)/180.0);

  mgPoint3* points = new mgPoint3[(arcSamples+3)*(lenSamples+3)];

  // for each point in circle around shell
  int rowSize = lenSamples+3;
  for (int i = -1; i <= arcSamples+1; i++)
  {
    double arcAngle = (2*PI*i)/arcSamples;      
    double x = cos(arcAngle);
    double y = sin(arcAngle);

    // for each point along z axis of shell
    for (int j = -1; j <= lenSamples+1; j++)
    {
      double lenAngle = 20+(140.0*j)/lenSamples;
      lenAngle = (PI*lenAngle)/180.0;
      double z = cos(lenAngle);
      double height = m_radius * sin(lenAngle);
      height -= m_outsideHeight*mgSimplexNoise::noiseSum(10, x, y, z);
      height += m_variation*mgSimplexNoise::noise(x*5, y*5, z*5);
      height = max(height, exitHeight*1.01);

      int index = (i+1)*rowSize+(j+1);
      points[index].x = m_center.x + x*height;
      points[index].y = m_center.y + y*height;
      points[index].z = m_center.z + z*m_radius;
    }
  }

  // for each point in circle around shell
  for (int i = 0; i <= arcSamples; i++)
  {
    double arcAngle = (2*PI*i)/arcSamples;      
    double x = cos(arcAngle);
    double y = sin(arcAngle);

    v.m_ty = (float) (i*2*PI*m_radius)/arcSamples;

    // for each point along z axis of shell
    for (int j = 0; j <= shellSamples; j++)
    {
      double lenAngle, height, z;

      if (j == 0)
      {
        // inside bottom corner of top end
        height = m_terrainRadius;
        z = m_terrainLength/2;
        v.setColor(255, 255, 255);
        v.m_tx = (float) height;
        v.setNormal(0, 0, -1.0);

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z);
      }
      else if (j == 1)
      {
        // inside top corner of top end
        lenAngle = (PI*20)/180.0;
        height = m_radius * sin(lenAngle);
        z = m_terrainLength/2;
        v.setColor(255, 255, 255);
        v.setNormal(0, 0, -1.0);
        v.m_tx = (float) height;

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z);
      }
      else if (j == 2)
      {
        // inside top corner of top end
        lenAngle = (PI*20)/180.0;
        height = m_radius * sin(lenAngle);
        z = m_terrainLength/2;
        v.setColor(128, 128, 255);
        v.setNormal(-x, -y, 0.0);
        v.m_tx = 0.0f;

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z);
      }
      else if (j == 3)
      {
        // inside top corner of top end
        lenAngle = (PI*20)/180.0;
        height = m_radius * sin(lenAngle);
        z = m_radius * cos(lenAngle);
        v.setColor(128, 128, 255);
        v.setNormal(-x, -y, 0.0);
        v.m_tx = (float) (m_radius - m_terrainLength/2);

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z);
      }
      else if (j == 4)
      {
        // outside edge
        lenAngle = (PI*20)/180.0;
        height = m_radius * sin(lenAngle);
        z = cos(lenAngle);
        v.setColor(133, 133, 133);  // gray rock
        v.setNormal(x, y, z);
        v.m_tx = (float) (m_radius - m_terrainLength/2);

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z*m_radius);
      }

      else if (j == shellSamples-4)
      {
        // outside edge
        lenAngle = (PI*160)/180.0;
        height = m_radius * sin(lenAngle);
        z = cos(lenAngle);
        v.setColor(133, 133, 133);  // gray rock
        v.setNormal(x, y, z);
        v.m_tx = (float) (m_radius - m_terrainLength/2);

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z*m_radius);
      }
      else if (j == shellSamples-3)
      {
        // inside top corner of bottom end
        lenAngle = (PI*160)/180.0;
        height = m_radius * sin(lenAngle);
        z = m_radius * cos(lenAngle);
        v.setColor(128, 128, 255);
        v.setNormal(-x, -y, 0.0);
        v.m_tx = (float) (m_radius - m_terrainLength/2);

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z);
      }
      else if (j == shellSamples-2)
      {
        // inside top corner of bottom end
        lenAngle = (PI*160)/180.0;
        height = m_radius * sin(lenAngle);
        z = -m_terrainLength/2;
        v.setColor(128, 128, 255);
        v.setNormal(-x, -y, 0.0);
        v.m_tx = 0.0f;

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z);
      }
      else if (j == shellSamples-1)
      {
        // inside top corner of bottom end
        lenAngle = (PI*160)/180.0;
        height = m_radius * sin(lenAngle);
        z = -m_terrainLength/2;
        v.setColor(255, 255, 255);
        v.setNormal(0.0, 0.0, 1.0);
        v.m_tx = (float) height;

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z);
      }
      else if (j == shellSamples)
      {
        // inside bottom corner of bottom end
        height = m_terrainRadius;
        z = -m_terrainLength/2;
        v.setColor(255, 255, 255);
        v.setNormal(0.0, 0.0, 1.0);

        v.m_tx = (float) height;

        v.m_px = (float) (m_center.x + x*height);
        v.m_py = (float) (m_center.y + y*height);
        v.m_pz = (float) (m_center.z + z);
      }
      else
      {
        // point on outside of shell
        v.setColor(133, 133, 133);  // gray rock

        v.m_tx = (float) (j-5);

        int index = (i+1)*rowSize + (j-5+1);
        mgPoint3 pt = points[index];

        v.m_px = (float) pt.x;
        v.m_py = (float) pt.y;
        v.m_pz = (float) pt.z;

        mgPoint3 westPt = points[index-rowSize];
        mgPoint3 eastPt = points[index+rowSize];
        mgPoint3 northPt = points[index-1];
        mgPoint3 southPt = points[index+1];
        northPt.subtract(pt);
        southPt.subtract(pt);
        westPt.subtract(pt);
        eastPt.subtract(pt);

        mgPoint3 normal(northPt);
        normal.cross(westPt);
        normal.normalize();

        v.m_nx = (float) normal.x;
        v.m_ny = (float) normal.y;
        v.m_nz = (float) normal.z;

        normal = westPt;
        normal.cross(southPt);
        normal.normalize();

        v.m_nx += (float) normal.x;
        v.m_ny += (float) normal.y;
        v.m_nz += (float) normal.z;

        normal = southPt;
        normal.cross(eastPt);
        normal.normalize();

        v.m_nx += (float) normal.x;
        v.m_ny += (float) normal.y;
        v.m_nz += (float) normal.z;

        normal = eastPt;
        normal.cross(northPt);
        normal.normalize();

        v.m_nx += (float) normal.x;
        v.m_ny += (float) normal.y;
        v.m_nz += (float) normal.z;

        v.m_nx /= 4.0f;
        v.m_ny /= 4.0f;
        v.m_nz /= 4.0f;
      }

      v.addTo(m_shellVertexes);
    }
  }

  m_shellIndexes = mgDisplay->newIndexBuffer(6*arcSamples*shellSamples, false, vertexCount > 65535);  
  m_shellIndexes->addGrid(0, shellSamples+1, arcSamples, shellSamples, true);

  delete points;
}

//--------------------------------------------------------------
// create water
void Habitat::createWater()
{
  int steps = m_samples;
  int vertexSize = 1+steps;

  // vertexes are top and bottom of cylinder
  m_waterVertexes = mgVertex::newBuffer(vertexSize*2);

  double waterRadius = m_terrainRadius - WATER_LEVEL/SYSTEM_FAR_SCALE;
  mgVertex v;
  for (int i = 0; i < vertexSize; i++)
  {
    double angle = (2*PI*i)/steps;
    double x = cos(angle);
    double y = sin(angle);

    // normal points in
    v.m_nx = (float) -x;
    v.m_ny = (float) -y;
    v.m_nz = 0.0f;

    // top point
    v.m_px = (float) (m_center.x + waterRadius * x);
    v.m_py = (float) (m_center.y + waterRadius * y);
    v.m_pz = (float) (m_center.z - m_terrainLength/2);

    v.m_tx = (float) (i*6*waterRadius*SYSTEM_FAR_SCALE/steps);
    v.m_ty = 0.0f;

    v.addTo(m_waterVertexes);

    // bottom point
    v.m_pz = (float) (m_center.z + m_terrainLength/2);

    v.m_ty = (float) (m_terrainLength*SYSTEM_FAR_SCALE);

    v.addTo(m_waterVertexes);
  }

  // create indexes
  m_waterIndexes = mgDisplay->newIndexBuffer(steps*6);
  for (int i = 0; i < steps; i++)
  {
    int index = i*2;
    m_waterIndexes->addIndex(index);  // tl
    m_waterIndexes->addIndex(index+1);  // bl
    m_waterIndexes->addIndex(index+2);  // tr

    m_waterIndexes->addIndex(index+1);  // bl
    m_waterIndexes->addIndex(index+3);  // br
    m_waterIndexes->addIndex(index+2);  // tr
  }
}

//--------------------------------------------------------------
// create lighting
void Habitat::createLight()
{
  int steps = 10;
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
    v.m_pz = (float) (m_center.z - m_terrainLength/2);

    v.m_tx = (float) (i*6*radius/steps);
    v.m_ty = 0.0f;

    v.addTo(m_lightVertexes);

    // bottom point
    v.m_pz = (float) (m_center.z + m_terrainLength/2);

    v.m_ty = (float) (m_terrainLength);

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
void Habitat::addTerrainPolygons(
  double* heights,
  int width,
  int height,
  mgVertexBuffer* vertexes)
{
  // generate the vertex list.
  int heightSize = 3+width;  // -1 to width+1
  int vertexSize = 1+width;

  float cellStep = (float) ((2*PI*m_terrainRadius)/width);

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

      v.m_px = (float) (m_center.x + x*(m_terrainRadius-ht));
      v.m_py = (float) (m_center.y + y*(m_terrainRadius-ht));
      v.m_pz = (float) (m_center.z + z*m_terrainLength);

      float slope, len;
      mgPoint3 normal;
      slope = (float) (heights[index-heightSize]-ht);
      len = sqrt(slope*slope+cellStep*cellStep);
      normal.x += slope/len;
      normal.y += cellStep/len;

      slope = (float) (heights[index+heightSize] - ht);
      len = sqrt(slope*slope+cellStep*cellStep);
      normal.x += -slope/len;
      normal.y += cellStep/len;

      slope = (float) (heights[index-1] - ht);
      len = sqrt(slope*slope+cellStep*cellStep);
      normal.z += slope/len;
      normal.y += cellStep/len;

      slope = (float) (heights[index+1] - ht);
      len = sqrt(slope*slope+cellStep*cellStep);
      normal.z += -slope/len;
      normal.y += cellStep/len;

      normal.normalize();
      v.m_nx = (float) normal.x;
      v.m_ny = (float) normal.y;
      v.m_nz = (float) normal.z;

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
// create terrain
void Habitat::createTerrain()
{
  double ox = m_center.x / 5000.0;
  double oy = m_center.y / 5000.0;
  double oz = m_center.z / 5000.0;

  int width = m_samples*10;
  int height = (int) ((width*m_terrainLength) / (2*PI*m_terrainRadius));

  int pointSize = width+3;  // -1 to width+1 
  double* heights = new double[(height+3)*pointSize];
  for (int i = -1; i <= height+1; i++)
  {
    double z = -0.5 + (i/(double) height);
    double nz = 2*z * m_terrainLength / (2*m_terrainRadius);
    for (int j = -1; j <= width+1; j++)
    {
      double angle = (2*PI*j)/width;
      double x = cos(angle);
      double y = sin(angle);

      double thickness = mgSimplexNoise::noiseSum(10, x/2.5+ox, y/2.5+oy, nz/2.5+oz);
      double ht = MIN_HEIGHT + max(0, 30+200* thickness);

      heights[(j+1)+pointSize*(i+1)] = ht / SYSTEM_FAR_SCALE;
    }
  }

  int vertexCount = (height+1)*(width+1);
  m_terrainVertexes = VertexTerrain::newBuffer(vertexCount);
  m_terrainIndexes = mgDisplay->newIndexBuffer(6*height*width, false, vertexCount > 65535);

  addTerrainPolygons(heights, width, height, m_terrainVertexes);

  m_terrainIndexes->addGrid(0, width+1, height, width, false);

  delete heights;
}

//--------------------------------------------------------------
// create buffers ready to send to display
void Habitat::createBuffers()
{
  createShell();
  createWater();
  createLight();
  createTerrain();
}

//--------------------------------------------------------------
// delete display buffers
void Habitat::deleteBuffers()
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
BOOL Habitat::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// draw opaque data
void Habitat::render()
{
  mgDisplay->setCulling(false);
  
  mgDisplay->setShader(m_shellShader);
  mgDisplay->setTexture(m_terrainTexture);
  mgDisplay->draw(MG_TRIANGLES, m_shellVertexes, m_shellIndexes);

  mgDisplay->setShader(m_terrainShader);
  mgDisplay->setTexture(m_terrainTexture);
  mgDisplay->draw(MG_TRIANGLES, m_terrainVertexes, m_terrainIndexes);

  mgDisplay->setMatColor(1.0, 1.0, 1.0);
  mgDisplay->setShader(m_lightShader);
  mgDisplay->setTexture(m_lightTexture);
  mgDisplay->draw(MG_TRIANGLES, m_lightVertexes, m_lightIndexes);

  mgDisplay->setCulling(true);
}

//--------------------------------------------------------------
// draw transparent data
void Habitat::renderTransparent()
{
  mgDisplay->setShader(m_waterShader);
  mgDisplay->setTexture(m_waterTexture);
  mgDisplay->draw(MG_TRIANGLES, m_waterVertexes, m_waterIndexes);

  mgDisplay->setMatColor(0.8, 0.8, 0.8);
}
