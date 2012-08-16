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

#include "Tower.h"

const int FLOORS = 15;
const int FLOOR_STEPS = 100;
const int APT_STEPS = 75;

const double BOTTOM_FLOOR = 5.0+7.5;
const double OFFICE_WIDTH = (12.9*2)/30;
const double OFFICE_LEFT = -12.9;
const double OFFICE_DEPTH = 4.2;

const int MAX_OFFICE = 27;
const double FLOOR_SCALE = 1/30.0;
const double APT_SCALE = 1.0;

//--------------------------------------------------------------
// constructor
Tower::Tower(
  const mgOptionsFile& options,
  BOOL lights)
{
  m_lights = lights;
  m_shellIndexes = NULL;
  m_shellVertexes = NULL;
  m_officeIndexes = NULL;
  m_officeVertexes = NULL;
  m_glassIndexes = NULL;

  // load ball textures.  must all be same size.
  mgString fileName;
  options.getFileName("tower-shell", options.m_sourceFileName, "tower-body.jpg", fileName);
  m_shellTexture = mgDisplay->loadTexture(fileName);

  options.getFileName("tower-glass", options.m_sourceFileName, "tower-glass.jpg", fileName);
  m_glassTexture = mgDisplay->loadTexture(fileName);

  mgStringArray fileList;
  options.getFileName("tower-wall", options.m_sourceFileName, "tower-wall.jpg", fileName);
  fileList.add(fileName);
  options.getFileName("tower-light", options.m_sourceFileName, "tower-light.jpg", fileName);
  fileList.add(fileName);
  options.getFileName("tower-floor", options.m_sourceFileName, "tower-floor.jpg", fileName);
  fileList.add(fileName);
  options.getFileName("tower-wall-dark", options.m_sourceFileName, "tower-wall.jpg", fileName);
  fileList.add(fileName);
  options.getFileName("tower-light-dark", options.m_sourceFileName, "tower-light.jpg", fileName);
  fileList.add(fileName);
  options.getFileName("tower-floor-dark", options.m_sourceFileName, "tower-floor.jpg", fileName);
  fileList.add(fileName);
  options.getFileName("tower-frame", options.m_sourceFileName, "tower-frame.jpg", fileName);
  fileList.add(fileName);

  m_officeTextures = mgDisplay->loadTextureArray(fileList);

  m_floorSpline.addVertex(mgPoint3(-30.0,   0.0,  0.0),     mgPoint3(-30.0,      0.0+10,    0.0));
  m_floorSpline.addVertex(mgPoint3(  0.0,  10.0,  0.0),     mgPoint3(  0.0-30,  10.0,       0.0));
  m_floorSpline.addVertex(mgPoint3( 30.0,   0.0,  0.0),     mgPoint3( 30.0,      0.0+10,    0.0));
  m_floorSpline.addVertex(mgPoint3(  0.0, -10.0,  0.0),     mgPoint3(  0.0+30, -10.0,       0.0));
  m_floorSpline.addVertex(mgPoint3(-30.0,   0.0,  0.0),     mgPoint3(-30.0,      0.0-10,    0.0));

  m_floorLen = m_floorSpline.getLength();

  m_aptSpline.addVertex(mgPoint3(  0.0,   1.5,  0.0),     mgPoint3( 0.0+0,      1.5,     0.0));
  m_aptSpline.addVertex(mgPoint3(  3.0,   1.5,  0.0),     mgPoint3(  3.0-2.5,   1.5,     0.0));
  m_aptSpline.addVertex(mgPoint3(  5.0,   7.5,  0.0),     mgPoint3(  5.0,       7.5-2.5, 0.0));
  m_aptSpline.addVertex(mgPoint3(  5.0,   7.5,  0.0),     mgPoint3(  5.0,       7.5-7.5, 0.0));
  m_aptSpline.addVertex(mgPoint3( 35.0,  15.0,  0.0),     mgPoint3( 35.0-30,    15.0,    0.0));
  m_aptSpline.addVertex(mgPoint3( 65.0,   7.5,  0.0),     mgPoint3( 65.0,       7.5+7.5, 0.0));
  m_aptSpline.addVertex(mgPoint3( 65.0,   0.0,  0.0),     mgPoint3( 65.0,       0.0,     0.0));
  
  m_aptLen = m_aptSpline.getLength();

  /* 
    The spline class returns points based on a 'distance' which is a sum of all the
    segments that define the spline curve, not the actual length of the curve.
    To get regularly spaced points for windows, we search for the input distances
    that produce the correct x values (used for height here).  The portions of
    the shape above and below the floors are then generated from the first and
    last distance point making the floors.
  */

  m_floorDists = new double[APT_STEPS+1];

  // find the floor heights
  for (int i = 0; i <= FLOORS*3; i++)
  {
    m_floorDists[15+i] = findSplineX(m_aptSpline, BOTTOM_FLOOR+i, 12, m_aptLen/2);
  }

  // fill in the rest of the tower
  double bottom = m_floorDists[15];
  double top = m_floorDists[15+FLOORS*3];
  for (int i = 0; i < 15; i++)
  {
    m_floorDists[i] = (bottom*i)/15;
    m_floorDists[16+FLOORS*3+i] = top + (m_aptLen - top)*(i+1)/15.0;
  }
}

//--------------------------------------------------------------
// destructor
Tower::~Tower()
{
  delete m_floorDists;
  m_floorDists = NULL;

  deleteBuffers();
}

//-----------------------------------------------------------------------------
// set normals of points on grid
void Tower::setNormals(
  mgVertex* points,
  int rows,
  int cols,
  BOOL outward)
{
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      // get the point
      int index = i*cols + j;
      mgPoint3 pt(points[index].m_px, points[index].m_py, points[index].m_pz);

      int northIndex = i*cols + ((j == cols-1) ? 1 : j+1);
      mgPoint3 northPt(points[northIndex].m_px, points[northIndex].m_py, points[northIndex].m_pz);
      northPt.subtract(pt);

      int southIndex = i*cols + ((j == 0) ? cols-2 : j-1);
      mgPoint3 southPt(points[southIndex].m_px, points[southIndex].m_py, points[southIndex].m_pz);
      southPt.subtract(pt);

      // normal is average of orthogonal vectors to each neighbor point
      mgVertex* v = &points[index];
      v->m_nx = v->m_ny = v->m_nz = 0.0f;
      mgPoint3 normal;
      if (i > 0)
      {
        int westIndex = (i-1)*cols + j;
        mgPoint3 westPt(points[westIndex].m_px, points[westIndex].m_py, points[westIndex].m_pz);
        westPt.subtract(pt);

        normal = northPt;
        normal.cross(westPt);
        normal.normalize();

        v->m_nx += (float) normal.x;
        v->m_ny += (float) normal.y;
        v->m_nz += (float) normal.z;

        normal = westPt;
        normal.cross(southPt);
        normal.normalize();

        v->m_nx += (float) normal.x;
        v->m_ny += (float) normal.y;
        v->m_nz += (float) normal.z;
      }

      if (i < rows-1)
      {
        // get vector to each neighbor point on grid
        int eastIndex = (i+1)*cols + j;
        mgPoint3 eastPt(points[eastIndex].m_px, points[eastIndex].m_py, points[eastIndex].m_pz);
        eastPt.subtract(pt);

        normal = southPt;
        normal.cross(eastPt);
        normal.normalize();

        v->m_nx += (float) normal.x;
        v->m_ny += (float) normal.y;
        v->m_nz += (float) normal.z;

        normal = eastPt;
        normal.cross(northPt);
        normal.normalize();

        v->m_nx += (float) normal.x;
        v->m_ny += (float) normal.y;
        v->m_nz += (float) normal.z;
      }

      float len = sqrt(v->m_nx*v->m_nx + v->m_ny*v->m_ny + v->m_nz*v->m_nz);
      if (outward)
        len = -len;

      v->m_nx /= len;
      v->m_ny /= len;
      v->m_nz /= len;
    }
  }
}

//-----------------------------------------------------------------------------
// find x value on spline
double Tower::findSplineX(
  mgBezier& spline,
  double target,
  int count,
  double dist)
{
  mgPoint3 pt;
  double step = dist;
  for (int i = 0; i < count; i++)
  {
    spline.splinePt(dist, pt);
    if (pt.x > target)
      dist -= step;
    else if (pt.x < target)
      dist += step;
    else break;  // if equal 
    step /= 2;
  }
  return dist;
}

//-----------------------------------------------------------------------------
// return a shell point
void Tower::shellPoint(
  int i,
  int j,
  mgPoint3& pt)
{
  mgPoint3 aptPt, floorPt;

  m_aptSpline.splinePt(m_floorDists[i], aptPt);
  double ht = aptPt.y - aptPt.z;
  m_floorSpline.splinePt((m_floorLen * j)/FLOOR_STEPS, floorPt);

  pt.x = APT_SCALE * (ht * FLOOR_SCALE * floorPt.x);
  pt.y = APT_SCALE * (aptPt.z + ht * FLOOR_SCALE * floorPt.y);
  pt.z = APT_SCALE * aptPt.x;
}

//-----------------------------------------------------------------------------
// create shell of tower
void Tower::createShell()
{
  int vertexCount = (APT_STEPS+1) * (FLOOR_STEPS+1);
  int indexCount = 6*APT_STEPS*FLOOR_STEPS;
  int glassIndexCount = 6*FLOORS * FLOOR_STEPS;

  m_shellVertexes = mgVertex::newBuffer(vertexCount);
  m_shellIndexes = mgDisplay->newIndexBuffer(indexCount, false, true);
  m_glassIndexes = mgDisplay->newIndexBuffer(glassIndexCount, false, true);

  mgVertex* points = new mgVertex[(FLOOR_STEPS+1) * (APT_STEPS+1)];

  mgPoint3 aptPt, floorPt;
  for (int i = 0; i <= APT_STEPS; i++)
  {
    m_aptSpline.splinePt(m_floorDists[i], aptPt);
    double ht = aptPt.y - aptPt.z;
    for (int j = 0; j <= FLOOR_STEPS; j++)
    {
      m_floorSpline.splinePt((m_floorLen * j)/FLOOR_STEPS, floorPt);

      mgVertex* v = &points[i*(FLOOR_STEPS+1)+j];
      mgPoint3 pt(APT_SCALE * (ht * FLOOR_SCALE * floorPt.x), 
                  APT_SCALE * (aptPt.z + ht * FLOOR_SCALE * floorPt.y), 
                  APT_SCALE * aptPt.x);
      v->setPoint(pt.x, pt.y, pt.z);

      v->setTexture(2*j/(double) FLOOR_STEPS, 2*i/(double) APT_STEPS);
//      v->setTexture((pt.x+pt.y)/40, pt.z/40);
    }
  }

  // set all the normals
  setNormals(points, APT_STEPS+1, FLOOR_STEPS+1, false);

  int vertexBase = m_shellVertexes->getLength();
  for (int i = 0; i < (APT_STEPS+1)*(FLOOR_STEPS+1); i++)
    m_shellVertexes->addVertex(&points[i]);

  int rowSize = FLOOR_STEPS+1;
  for (int i = 0; i < APT_STEPS; i++)
  {
    for (int j = 0; j < FLOOR_STEPS; j++)
    {
      int index = vertexBase + i*rowSize+j;

      // if a window row/col
      if (i > 15 && i < 15+FLOORS*3 && (i-15)%3 != 0 && 
          ((j > 62 && j < 88) || (j > 12 && j < 38)))
      {
        m_glassIndexes->addIndex(index);  // tl
        m_glassIndexes->addIndex(index+rowSize);  // bl
        m_glassIndexes->addIndex(index+1);  // tr

        m_glassIndexes->addIndex(index+rowSize);  // bl
        m_glassIndexes->addIndex(index+rowSize+1);  // br
        m_glassIndexes->addIndex(index+1);  // tr
      }
      else
      {
        m_shellIndexes->addIndex(index);  // tl
        m_shellIndexes->addIndex(index+rowSize);  // bl
        m_shellIndexes->addIndex(index+1);  // tr

        m_shellIndexes->addIndex(index+rowSize);  // bl
        m_shellIndexes->addIndex(index+rowSize+1);  // br
        m_shellIndexes->addIndex(index+1);  // tr
      }
    }
  }

  delete points;
}

//-----------------------------------------------------------------------------
// add an office to the tower
void Tower::addOffice(
  int floor,
  int start,
  int len)
{
  const int TILE_SCALE = 5;
  int wallTile = 0;
  int floorTile = 1;
  int ceilTile = 2;
  if (len < 0)
  {
    wallTile += 3;
    floorTile += 3;
    ceilTile += 3;
    len = -len;
  }

  mgVertexTA tl, tr, bl, br;
  mgPoint3 ptLLL, ptLLH, ptLHL, ptLHH;
  mgPoint3 ptHLL, ptHLH, ptHHL, ptHHH;
  double normal;

  int baseWin, baseFloor;
  // generate the corner points of this segment from splines
  if (floor < FLOORS)
  {
    baseWin = 12;
    baseFloor = 15;
  }
  else
  {
    baseWin = 62;
    baseFloor = 15 - FLOORS*3;
  }

  // for each segment of office
  int segment = start;
  for (int i = 0; i < len; i++)
  {
    // get the front edge of the segment
    shellPoint(baseFloor+floor*3, baseWin+segment, ptLHL);
    shellPoint(baseFloor+floor*3, baseWin+segment+1, ptHHL);
    shellPoint(baseFloor+floor*3+3, baseWin+segment, ptLHH);
    shellPoint(baseFloor+floor*3+3, baseWin+segment+1, ptHHH);

    // construct back edge of segment
    double back = (floor < FLOORS) ? 0.5 : -0.5;
    ptLLL = ptLHL;  ptLLL.y = back;
    ptHLL = ptHHL;  ptHLL.y = back;
    ptLLH = ptLHH;  ptLLH.y = back;
    ptHLH = ptHHH;  ptHLH.y = back;
    segment++;

    // all sides have same texture coordinate for u,v
    tl.setTexture(ptLHH.y/TILE_SCALE, ptLHH.z/TILE_SCALE, wallTile);
    tr.setTexture(ptLHL.y/TILE_SCALE, ptLHL.z/TILE_SCALE, wallTile);
    bl.setTexture(ptLLH.y/TILE_SCALE, ptLLH.z/TILE_SCALE, wallTile);
    br.setTexture(ptLLL.y/TILE_SCALE, ptLLL.z/TILE_SCALE, wallTile);

    
    // if left segment, generate xmin wall
    if (i == 0)
    {
      // build indexes for a rectangle
      m_officeIndexes->addRectIndex(m_officeVertexes->getLength());
      
      // as we wrap around from front to back of tower,
      // normals will point in opposite directions.
      double right = ptHLL.x - ptLLL.x;
      right /= abs(right);
      tl.setNormal(right, 0, 0);
      tr.setNormal(right, 0, 0);
      bl.setNormal(right, 0, 0);
      br.setNormal(right, 0, 0);

      double front = ptLHL.y - ptLLL.y;
      front /= abs(front);

      // front surface is curved.  To move the wall to the right,
      // we need to figure new front surface (y) at that point.
      double topInc = 0.1 * (ptHHH.y - ptLHH.y)/(ptHHH.x - ptLHH.x);
      double botInc = 0.1 * (ptHHL.y - ptLHL.y)/(ptHHL.x - ptLHL.x);
      tl.setPoint(ptLHH.x+right*0.1, ptLHH.y+topInc-front*0.1, ptLHH.z);
      tr.setPoint(ptLHL.x+right*0.1, ptLHL.y+botInc-front*0.1, ptLHL.z);
      bl.setPoint(ptLLH.x+right*0.1, ptLLH.y+topInc, ptLLH.z);
      br.setPoint(ptLLL.x+right*0.1, ptLLL.y+botInc, ptLLL.z);

      tr.addTo(m_officeVertexes);
      tl.addTo(m_officeVertexes);
      br.addTo(m_officeVertexes);
      bl.addTo(m_officeVertexes);

      // add front divider
      m_officeIndexes->addRectIndex(m_officeVertexes->getLength());

      tl.setTexture(ptLHL.x/TILE_SCALE,            ptLHL.z/TILE_SCALE,  6);
      tr.setTexture((ptLHL.x+right*0.1)/TILE_SCALE,  ptLHL.z/TILE_SCALE,  6);
      bl.setTexture(ptLHH.x/TILE_SCALE,            ptLHH.z/TILE_SCALE,  6);
      br.setTexture((ptLHH.x+right*0.1)/TILE_SCALE,  ptLHH.z/TILE_SCALE,  6);

      tl.setNormal(0, front, 0);
      tr.setNormal(0, front, 0);
      bl.setNormal(0, front, 0);
      br.setNormal(0, front, 0);

      tl.setPoint(ptLHL.x,            ptLHL.y-front*0.1,          ptLHL.z);
      tr.setPoint(ptLHL.x+right*0.1,  ptLHL.y+botInc-front*0.1,   ptLHL.z);
      bl.setPoint(ptLHH.x,            ptLHH.y-front*0.1,          ptLHH.z);
      br.setPoint(ptLHH.x+right*0.1,  ptLHH.y+topInc-front*0.1,   ptLHH.z);

      tr.addTo(m_officeVertexes);
      tl.addTo(m_officeVertexes);
      br.addTo(m_officeVertexes);
      bl.addTo(m_officeVertexes);
    }

    // if right segment, generate xmax wall
    if (i == len-1)
    {
      // build indexes for a rectangle
      m_officeIndexes->addRectIndex(m_officeVertexes->getLength());

      double left = ptLLL.x - ptHLL.x;
      left /= abs(left);
      tl.setNormal(left, 0, 0);
      tr.setNormal(left, 0, 0);
      bl.setNormal(left, 0, 0);
      br.setNormal(left, 0, 0);

      double front = ptLHL.y - ptLLL.y;
      front /= abs(front);

      tl.setPoint(ptHHL.x, ptHHL.y-0.1*front, ptHHL.z);
      tr.setPoint(ptHHH.x, ptHHH.y-0.1*front, ptHHH.z);
      bl.setPoint(ptHLL.x, ptHLL.y, ptHLL.z);
      br.setPoint(ptHLH.x, ptHLH.y, ptHLH.z);

      tr.addTo(m_officeVertexes);
      tl.addTo(m_officeVertexes);
      br.addTo(m_officeVertexes);
      bl.addTo(m_officeVertexes);
    }

    tl.setTexture(ptLLL.x/TILE_SCALE, ptLLL.z/TILE_SCALE, wallTile);
    tr.setTexture(ptHLL.x/TILE_SCALE, ptHLL.z/TILE_SCALE, wallTile);
    bl.setTexture(ptLLH.x/TILE_SCALE, ptLLH.z/TILE_SCALE, wallTile);
    br.setTexture(ptHLH.x/TILE_SCALE, ptHLH.z/TILE_SCALE, wallTile);

    // generate back wall

    // build indexes for a rectangle
    m_officeIndexes->addRectIndex(m_officeVertexes->getLength());

    normal = ptLHL.y - ptLLL.y;
    normal /= abs(normal);
    tl.setNormal(0, normal, 0);
    tr.setNormal(0, normal, 0);
    bl.setNormal(0, normal, 0);
    br.setNormal(0, normal, 0);

    tl.setPoint(ptLLL);
    tr.setPoint(ptHLL);
    bl.setPoint(ptLLH);
    br.setPoint(ptHLH);

    tr.addTo(m_officeVertexes);
    tl.addTo(m_officeVertexes);
    br.addTo(m_officeVertexes);
    bl.addTo(m_officeVertexes);

    // generate floor for segment

    tl.setTexture(ptLHL.x/TILE_SCALE, ptLHL.y/TILE_SCALE, floorTile);
    tr.setTexture(ptHHL.x/TILE_SCALE, ptHHL.y/TILE_SCALE, floorTile);
    bl.setTexture(ptLLL.x/TILE_SCALE, ptLLL.y/TILE_SCALE, floorTile);
    br.setTexture(ptHLL.x/TILE_SCALE, ptHLL.y/TILE_SCALE, floorTile);

    m_officeIndexes->addRectIndex(m_officeVertexes->getLength());

    tl.setNormal(0, 0, 1);
    tr.setNormal(0, 0, 1);
    bl.setNormal(0, 0, 1);
    br.setNormal(0, 0, 1);

    tl.setPoint(ptLHL);
    tr.setPoint(ptHHL);
    bl.setPoint(ptLLL);
    br.setPoint(ptHLL);

    tr.addTo(m_officeVertexes);
    tl.addTo(m_officeVertexes);
    br.addTo(m_officeVertexes);
    bl.addTo(m_officeVertexes);

    // generate ceiling for segment
    m_officeIndexes->addRectIndex(m_officeVertexes->getLength());

    tl.setTexture(ptHHH.x/TILE_SCALE, ptHHH.y/TILE_SCALE, ceilTile);
    tr.setTexture(ptLHH.x/TILE_SCALE, ptLHH.y/TILE_SCALE, ceilTile);
    bl.setTexture(ptHLH.x/TILE_SCALE, ptHLH.y/TILE_SCALE, ceilTile);
    br.setTexture(ptLLH.x/TILE_SCALE, ptLLH.y/TILE_SCALE, ceilTile);

    tl.setNormal(0, 0, -1);
    tr.setNormal(0, 0, -1);
    bl.setNormal(0, 0, -1);
    br.setNormal(0, 0, -1);

    tl.setPoint(ptHHH);
    tr.setPoint(ptLHH);
    bl.setPoint(ptHLH);
    br.setPoint(ptLLH);

    tr.addTo(m_officeVertexes);
    tl.addTo(m_officeVertexes);
    br.addTo(m_officeVertexes);
    bl.addTo(m_officeVertexes);
  }
}

//-----------------------------------------------------------------------------
// create offices inside tower
void Tower::createOffices()
{
  int wallCount = 0;

  // allocate array of offices on both sides
  int* offices = new int[2*FLOORS*MAX_OFFICE];
  memset(offices, 0, 2*FLOORS*MAX_OFFICE*sizeof(int));

  for (int floor = 0; floor < 2*FLOORS; floor++)
  {
    int used = 0;
    int j = 0;
    while (used < MAX_OFFICE)
    {
      int len = 4+rand()%7;
      len = min(len, MAX_OFFICE-used);

      // don't orphan small len at end of row
      if ((MAX_OFFICE - (used+len)) < 4)
        len = MAX_OFFICE-used;

      used += len;
      offices[floor*MAX_OFFICE + j] = (m_lights && rand()%2) ? len : -len;
      j++;

      wallCount += len*3;  // top, bottom and back for each segment
      wallCount += 2+1;  // side walls and front divider
    }
  }

  int vertexCount = 4*wallCount;
  int indexCount = 6*wallCount;
  m_officeVertexes = mgVertexTA::newBuffer(vertexCount);
  m_officeIndexes = mgDisplay->newIndexBuffer(indexCount, false, true);

  // add front side offices
  for (int floor = 0; floor < 2*FLOORS; floor++)
  {
    int used = 0;
    for (int j = 0; j < MAX_OFFICE; j++)
    {
      int len = offices[floor*MAX_OFFICE + j];
      if (len == 0)
        break;

      addOffice(floor, used, len);
      used += abs(len);
    }
  }

  delete offices;
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Tower::createBuffers()
{
  createShell();
  createOffices();
}

//-----------------------------------------------------------------------------
// delete vertex and index buffers
void Tower::deleteBuffers()
{
  delete m_shellIndexes;
  m_shellIndexes = NULL;
  delete m_shellVertexes;
  m_shellVertexes = NULL;

  delete m_glassIndexes;
  m_glassIndexes = NULL;

  delete m_officeIndexes;
  m_officeIndexes = NULL;
  delete m_officeVertexes;
  m_officeVertexes = NULL;
}

//-----------------------------------------------------------------------------
// render the wreck
void Tower::render()
{
  // set drawing parameters
  mgDisplay->setMatColor(1.0, 1.0, 1.0);
  mgDisplay->setTransparent(false);

  // draw triangles using texture and shader
  mgDisplay->setShader("litTexture");
  mgDisplay->setTexture(m_shellTexture);
  mgDisplay->draw(MG_TRIANGLES, m_shellVertexes, m_shellIndexes);

  // draw triangles using texture and shader
  mgDisplay->setShader("unlitTextureArray");
  mgDisplay->setTexture(m_officeTextures);
  mgDisplay->draw(MG_TRIANGLES, m_officeVertexes, m_officeIndexes);

  // draw triangles using texture and shader
  mgDisplay->setTransparent(true);
  mgDisplay->setShader("litTexture");
  mgDisplay->setTexture(m_glassTexture);
  mgDisplay->draw(MG_TRIANGLES, m_shellVertexes, m_glassIndexes);

  mgDisplay->setTransparent(false);
}

