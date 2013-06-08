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

#include "Tower.h"
#include "Wreck.h"

//--------------------------------------------------------------
// constructor
Wreck::Wreck(
  const mgOptionsFile& options)
{
  m_indexes = NULL;
  m_vertexes = NULL;

  mgString fileName;
  options.getFileName("wreck-shell", options.m_sourceFileName, "wreck-shell.jpg", fileName);

  m_texture = mgDisplay->loadTexture(fileName);

  for (int i = 0; i < 6; i++)
  {
    // omit the wrecked towers
    if (i == 2 || i == 5)
      m_towers.add(NULL);
    else
    {
      Tower* tower = new Tower(options, true);
      m_towers.add(tower);
    }
  }
}

//--------------------------------------------------------------
// destructor
Wreck::~Wreck()
{
  deleteBuffers();

  for (int i = 0; i < m_towers.length(); i++)
  {
    Tower* tower = (Tower*) m_towers[i];
    delete tower;
  }
  m_towers.removeAll();
}

//-----------------------------------------------------------------------------
// set normals of points on grid
void Wreck::setNormals(
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
// build a surface 
void Wreck::buildSurface(
  mgBezier& bodySpline,
  double bodyScale,
  int bodySteps,
  mgBezier& lenSpline,
  double lenScale,
  int lenSteps,
  const mgMatrix4& xform,
  BOOL outwards)
{
  double bodyLen = bodySpline.getLength();
  double lenLen = lenSpline.getLength();

  mgVertex* points = new mgVertex[(bodySteps+1) * (lenSteps+1)];

  mgPoint3 lenPt, bodyPt, xpt;
  for (int i = 0; i <= lenSteps; i++)
  {
    lenSpline.splinePt((lenLen * i)/lenSteps, lenPt);
    double ht = lenPt.y - lenPt.z;
    for (int j = 0; j <= bodySteps; j++)
    {
      bodySpline.splinePt((bodyLen * j)/bodySteps, bodyPt);

      mgVertex* v = &points[i*(bodySteps+1)+j];
      mgPoint3 pt(lenScale * (ht * bodyScale * bodyPt.x), 
                  lenScale * (lenPt.z + ht * bodyScale * bodyPt.y), 
                  lenScale * lenPt.x);
      xform.mapPt(pt, xpt);
      v->setPoint(xpt.x, xpt.y, xpt.z);

      v->setTexture(2*j/(double) bodySteps, 2*i/(double) lenSteps);
    }
  }

  // set all the normals
  setNormals(points, lenSteps+1, bodySteps+1, outwards);

  int vertexBase = m_vertexes->getLength();
  for (int i = 0; i < (lenSteps+1)*(bodySteps+1); i++)
    m_vertexes->addVertex(&points[i]);

  m_indexes->addGrid(vertexBase, bodySteps+1, lenSteps, bodySteps, outwards);

  delete points;
}

//-----------------------------------------------------------------------------
// create ring
void Wreck::createRing(
  mgMatrix4& xform,
  int ringSteps,
  int circleSteps)
{
  mgBezier ringSpline;
  ringSpline.addVertex(mgPoint3(  0.0,  4.0,  0.0),     mgPoint3(  0.0+13,  4.0,       0.0));
  ringSpline.addVertex(mgPoint3( 13.0,  0.0,  0.0),     mgPoint3( 13.0,     0.0+4,     0.0));
  ringSpline.addVertex(mgPoint3(  0.0, -4.0,  0.0),     mgPoint3(  0.0+13, -4.0,       0.0));

  double ringLen = ringSpline.getLength();

  mgVertex* points = new mgVertex[(ringSteps+1) * (circleSteps+1)];

  mgPoint3 ringPt, xpt;
  for (int i = 0; i <= circleSteps; i++)
  {
    double angle = (i*2*PI)/circleSteps;
    for (int j = 0; j <= ringSteps; j++)
    {
      ringSpline.splinePt((ringLen * j)/ringSteps, ringPt);

      mgVertex* v = &points[i*(ringSteps+1)+j];
      mgPoint3 pt(cos(angle) * ringPt.x, 
                  sin(angle) * ringPt.x,
                  ringPt.y);
      xform.mapPt(pt, xpt);
      v->setPoint(xpt.x, xpt.y, xpt.z);

      v->setTexture(xpt.z/100 + 2*j/(double) ringSteps, 2*i/(double) circleSteps);
    }
  }

  // set all the normals
  setNormals(points, circleSteps+1, ringSteps+1, true);

  int vertexBase = m_vertexes->getLength();
  for (int i = 0; i < (circleSteps+1)*(ringSteps+1); i++)
    m_vertexes->addVertex(&points[i]);

  m_indexes->addGrid(vertexBase, ringSteps+1, circleSteps, ringSteps, true);

  delete points;
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Wreck::createSpine(
  int sideSteps,
  int outSteps)
{
  mgBezier outSpline;
  outSpline.addVertex(mgPoint3(  0.0, 10.0,   9.5),    mgPoint3(  0.0,    10.0+10,   9.5-3));
  outSpline.addVertex(mgPoint3( 10.0, 40.0,   0.0),    mgPoint3( 10.0-10, 40.0,      0.0));
  outSpline.addVertex(mgPoint3( 30.0, 10.0,   0.0),    mgPoint3( 30.0-10, 10.0,      0.0));
  outSpline.addVertex(mgPoint3( 60.0, 10.0,   0.0),    mgPoint3( 60.0,    10.0,      0.0));
  outSpline.addVertex(mgPoint3(180.0, 10.0,   0.0),    mgPoint3(180.0,    10.0,      0.0));
  outSpline.addVertex(mgPoint3(180.0, 10.0,   0.0),    mgPoint3(180.0-10, 10.0,      0.0));
  outSpline.addVertex(mgPoint3(210.0, 30.0,   5.0),    mgPoint3(210.0-10, 30.0,      5.0));
  outSpline.addVertex(mgPoint3(240.0, 30.0,   5.0),    mgPoint3(240.0-10, 30.0,      5.0));
  outSpline.addVertex(mgPoint3(235.0, 17.5,  17.0),    mgPoint3(235.0,    17.5+5,   17.0-5));

  mgBezier sideSpline;
  double left = 150*PI/180.0;
  double lx = 10*cos(left);  
  double ly = 10*sin(left);

  double right = 30*PI/180.0;
  double rx = 10*cos(right);
  double ry = 10*sin(right);

  sideSpline.addVertex(mgPoint3(0.0,    0.0,    0.0),       mgPoint3(0.0-1,   0.0+1,    0.0));
  sideSpline.addVertex(mgPoint3(lx+2,   ly+1,   0.0),       mgPoint3(lx+2,    ly+1-2,   0.0));
  sideSpline.addVertex(mgPoint3(0.0,    10.0,   0.0),       mgPoint3(0-2.5,   10,       0.0));
  sideSpline.addVertex(mgPoint3(rx-2,   ry+1,   0.0),       mgPoint3(rx-2,    ry+1+2,   0.0));
  sideSpline.addVertex(mgPoint3(0.0,    0.0,    0.0),       mgPoint3(0.0+1,   0.0+1,    0.0));

  for (int i = 0; i < 3; i++)
  {
    mgMatrix4 rotate;
    rotate.rotateZDeg(i*120);
    buildSurface(sideSpline, 0.1, sideSteps, outSpline, 1.0, outSteps, rotate, false);
  }
}

//-----------------------------------------------------------------------------
// create vertex and index buffers
void Wreck::createBuffers()
{
  m_shader = mgVertex::loadShader("litTexture");

  int sideSteps = 50;
  int outSteps = 240;

  int ringSteps = 30;
  int circleSteps = 50;

  int vertexCount = 3* (outSteps+1) * (sideSteps+1);
  int indexCount = 3*6*outSteps*sideSteps;

  vertexCount += 3* (circleSteps+1) * (ringSteps+1);
  indexCount += 3*6*circleSteps*ringSteps;

  m_vertexes = mgVertex::newBuffer(vertexCount);
  m_indexes = mgDisplay->newIndexBuffer(indexCount);

  for (int i = 0; i < 3; i++)
  {
    mgMatrix4 model;
    model.translate(0, 0, 65+40*i);
    createRing(model, ringSteps, circleSteps);
  }

  createSpine(sideSteps, outSteps);

  for (int i = 0; i < m_towers.length(); i++)
  {
    Tower* tower = (Tower*) m_towers[i];
    if (tower != NULL)
      tower->createBuffers();
  }
}

//-----------------------------------------------------------------------------
// delete vertex and index buffers
void Wreck::deleteBuffers()
{
  delete m_indexes;
  m_indexes = NULL;
  delete m_vertexes;
  m_vertexes = NULL;

  for (int i = 0; i < m_towers.length(); i++)
  {
    Tower* tower = (Tower*) m_towers[i];
    if (tower != NULL)
      tower->deleteBuffers();
  }
}

//-----------------------------------------------------------------------------
// render the wreck
void Wreck::render()
{
  // set drawing parameters
  mgDisplay->setMatColor(1.0, 1.0, 1.0);
//  mgDisplay->setCulling(false);
  mgDisplay->setTransparent(false);

  // draw triangles using texture and shader
  mgDisplay->setShader(m_shader);
  mgDisplay->setTexture(m_texture);
  mgDisplay->draw(MG_TRIANGLES, m_vertexes, m_indexes);

//  mgDisplay->setCulling(true);

  mgMatrix4 baseModel;
  mgDisplay->getModelTransform(baseModel);

  Tower* tower = NULL;
  mgMatrix4 model;
  for (int i = 0; i < 3; i++)
  {
    tower = (Tower*) m_towers[i*2+0];
    if (tower != NULL)
    {
      model.loadIdentity();
      model.rotateXDeg(-90);
      model.translate(0, 11, 65+40*i);
      model.rotateZDeg(120*i);
      mgDisplay->appendModelTransform(model);
      tower->render();
      mgDisplay->setModelTransform(baseModel);
    }

    tower = (Tower*) m_towers[i*2+1];
    if (tower != NULL)
    {
      model.loadIdentity();
      model.rotateXDeg(-90);
      model.rotateZDeg(180);
      model.translate(0, -11, 65+40*i);
      model.rotateZDeg(120*i);
      mgDisplay->appendModelTransform(model);
      tower->render();
      mgDisplay->setModelTransform(baseModel);
    }
  }
}
