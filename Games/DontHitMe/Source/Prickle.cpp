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

#include "Prickle.h"

const double SCALE = 1000.0;
const double VARIATION = 10.0/SCALE;

mgTextureArray* Prickle::m_texture = NULL;
mgVertexBuffer* Prickle::m_vertexes = NULL;
mgIndexBuffer* Prickle::m_indexes = NULL;
mgShader* Prickle::m_shader = NULL;

//--------------------------------------------------------------
// constructor
Prickle::Prickle(
  const mgOptionsFile& options)
{
  m_xAngle = 0.0;
  m_yAngle = 0.0;
  m_zAngle = 0.0;

  m_xTumble = (rand()%100)/10.0;
  m_yTumble = (rand()%100)/10.0;

  // load prickle textures.  must all be same size.
  if (m_texture == NULL)
  {
    mgStringArray fileList;
    mgString fileName;
    mgString optionName;
    for (int i = 0; i < 7; i++)
    {
      optionName.format("prickle%d", i);
      options.getFileName(optionName, options.m_sourceFileName, optionName, fileName);
      fileList.add(fileName);
    }

    m_texture = mgDisplay->loadTextureArray(fileList);
    m_texture->setWrap(MG_TEXTURE_CLAMP, MG_TEXTURE_CLAMP);
  }
}

//--------------------------------------------------------------
// destructor
Prickle::~Prickle()
{
  deleteBuffers();
}

//--------------------------------------------------------------
// delete buffers
void Prickle::deleteBuffers()
{
  delete m_indexes;
  m_indexes = NULL;
  delete m_vertexes;
  m_vertexes = NULL;
}

//--------------------------------------------------------------
// animate
void Prickle::animate(
  double now,
  double since)
{
/*
  m_xAngle += m_xTumble * since/100;
  if (m_xAngle > 180)
    m_xAngle -= 360;
  m_yAngle += m_yTumble * since/100;
  if (m_yAngle > 180)
    m_yAngle -= 360;
*/
}

//--------------------------------------------------------------
// render the object
void Prickle::render()
{
  mgMatrix4 model;
  model.rotateXDeg(m_xAngle);
  model.rotateYDeg(m_yAngle);
  model.rotateZDeg(m_zAngle);
  model.translate(m_origin);

  mgDisplay->setModelTransform(model);

  // set drawing parameters
  mgDisplay->setMatColor(1.0, 1.0, 1.0);

  // draw triangles using texture and shader
  mgDisplay->setShader(m_shader);
  mgDisplay->setTexture(m_texture);
  mgDisplay->draw(MG_TRIANGLES, m_vertexes, m_indexes);
}

//--------------------------------------------------------------
// create buffers
void Prickle::createBuffers()
{
  if (m_vertexes != NULL)
    return;  // already done

  m_shader = mgVertexTA::loadShader("litTextureArray");

  // mesh for each of 6 faces of shell
  int steps = 8;
  int vertexSize = 1+steps;

  // figure vertex and index buffer size
  int vertexCount = 6*vertexSize*vertexSize;
  int indexCount = 6*6*steps*steps;

  // number of spines
  int latCount = 5;
  int lonCount = 5;

  // mesh for each spine
  int spinePoints = 9;
  int spineSteps = 9;
  vertexCount += latCount * lonCount * (spinePoints+1)*(spineSteps+1);
  indexCount += latCount * lonCount * 6*spinePoints*spineSteps;

  m_vertexes = mgVertexTA::newBuffer(vertexCount);
  m_indexes = mgDisplay->newIndexBuffer(indexCount);

  createShell(steps, vertexSize);
  createSpines(latCount, lonCount, spinePoints, spineSteps, 0.1, 1.5);
}

//--------------------------------------------------------------
// create shell
void Prickle::createShell(
  int steps,
  int vertexSize)
{
  double dx, dy, dz;
  mgVertexTA v;
  int vertexBase;

  // ZMIN side
  vertexBase = m_vertexes->getLength();

  for (int i = 0; i <= steps; i++)
  {
    for (int j = 0; j <= steps; j++)
    {
      dx = 0.5 - j/(double) steps;
      dy = i/(double) steps - 0.5;
      dz = -0.5;
      double len = sqrt(dx*dx + dy*dy + dz*dz);

      v.m_nx = (float) (dx/len);
      v.m_ny = (float) (dy/len);
      v.m_nz = (float) (dz/len);

      v.m_px = (float) v.m_nx;
      v.m_py = (float) v.m_ny;
      v.m_pz = (float) v.m_nz;

      v.m_tx = j/(float) steps;
      v.m_ty = 1.0f-(i/(float) steps);
      v.m_tz = 4;  // zmin

      m_vertexes->addVertex(&v);
    }
  }

  m_indexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  vertexBase = m_vertexes->getLength();

  // XMIN side
  for (int i = 0; i <= steps; i++)
  {
    for (int j = 0; j <= steps; j++)
    {
      dx = -0.5;
      dy = i/(double) steps - 0.5;
      dz = j/(double) steps - 0.5;
      double len = sqrt(dx*dx + dy*dy + dz*dz);

      v.m_nx = (float) (dx/len);
      v.m_ny = (float) (dy/len);
      v.m_nz = (float) (dz/len);

      v.m_px = (float) v.m_nx;
      v.m_py = (float) v.m_ny;
      v.m_pz = (float) v.m_nz;

      v.m_tx = j/(float) steps;
      v.m_ty = 1.0f-(i/(float) steps);
      v.m_tz = 0; // xmin

      m_vertexes->addVertex(&v);
    }
  }

  m_indexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  // ZMAX side
  vertexBase = m_vertexes->getLength();

  for (int i = 0; i <= steps; i++)
  {
    for (int j = 0; j <= steps; j++)
    {
      dx = j/(double) steps - 0.5;
      dy = i/(double) steps - 0.5;
      dz = 0.5;
      double len = sqrt(dx*dx + dy*dy + dz*dz);

      v.m_nx = (float) (dx/len);
      v.m_ny = (float) (dy/len);
      v.m_nz = (float) (dz/len);

      v.m_px = (float) v.m_nx;
      v.m_py = (float) v.m_ny;
      v.m_pz = (float) v.m_nz;

      v.m_tx = j/(float) steps;
      v.m_ty = 1.0f-(i/(float) steps);
      v.m_tz = 5; // zmax

      m_vertexes->addVertex(&v);
    }
  }

  m_indexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  // XMAX side
  vertexBase = m_vertexes->getLength();

  for (int i = 0; i <= steps; i++)
  {
    for (int j = 0; j <= steps; j++)
    {
      dx = 0.5;
      dy = i/(double) steps - 0.5;
      dz = 0.5 - j/(double) steps;
      double len = sqrt(dx*dx + dy*dy + dz*dz);

      v.m_nx = (float) (dx/len);
      v.m_ny = (float) (dy/len);
      v.m_nz = (float) (dz/len);

      v.m_px = (float) v.m_nx;
      v.m_py = (float) v.m_ny;
      v.m_pz = (float) v.m_nz;

      v.m_tx = j/(float) steps;
      v.m_ty = 1.0f-(i/(float) steps);
      v.m_tz = 1; // xmax

      m_vertexes->addVertex(&v);
    }
  }

  m_indexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  // YMIN side
  vertexBase = m_vertexes->getLength();

  for (int i = 0; i <= steps; i++)
  {
    for (int j = 0; j <= steps; j++)
    {
      dx = j/(double) steps - 0.5;
      dy = -0.5;
      dz = i/(double) steps - 0.5;        
      double len = sqrt(dx*dx + dy*dy + dz*dz);

      v.m_nx = (float) (dx/len);
      v.m_ny = (float) (dy/len);
      v.m_nz = (float) (dz/len);

      v.m_px = (float) v.m_nx;
      v.m_py = (float) v.m_ny;
      v.m_pz = (float) v.m_nz;

      v.m_tx = j/(float) steps;
      v.m_ty = 1.0f-(i/(float) steps);
      v.m_tz = 2; // ymin

      m_vertexes->addVertex(&v);
    }
  }

  m_indexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  // YMAX side
  vertexBase = m_vertexes->getLength();

  for (int i = 0; i <= steps; i++)
  {
    for (int j = 0; j <= steps; j++)
    {
      dx = 0.5 - i/(double) steps;
      dy = 0.5;
      dz = 0.5 - j/(double) steps;
      double len = sqrt(dx*dx + dy*dy + dz*dz);

      v.m_nx = (float) (dx/len);
      v.m_ny = (float) (dy/len);
      v.m_nz = (float) (dz/len);

      v.m_px = (float) v.m_nx;            
      v.m_py = (float) v.m_ny;            
      v.m_pz = (float) v.m_nz;            
                                                       
      v.m_tx = 1.0f-(i/(float) steps);
      v.m_ty = 1.0f-(j/(float) steps);
      v.m_tz = 3; // ymax

      m_vertexes->addVertex(&v);
    }
  }

  m_indexes->addGrid(vertexBase, vertexSize, steps, steps, true);
}

//--------------------------------------------------------------
// create spines
void Prickle::createSpines(
  int latCount,
  int lonCount,
  int spinePoints,
  int spineSteps,
  double spineRadius,
  double spineLen)
{
  for (int i = 0; i < latCount; i++)
  {
    for (int j = 0; j < lonCount; j++)
    {
      double zangle = (i+1)/(double) (1+latCount);
      double angle = j/(double) lonCount;

      // add some noise to the angles
      zangle += 0.1*mgSimplexNoise::noiseSum(10, zangle, angle, 0.0);
      angle += 0.1*mgSimplexNoise::noiseSum(10, zangle, angle, 1.0);

      zangle = PI*zangle;
      angle = 2*PI*angle;

      double zradius = sin(zangle);
      double z = cos(zangle);

      double x = zradius * cos(angle);
      double y = zradius * sin(angle);

      mgPoint3 origin(x, y, z);
      mgPoint3 zaxis(x, y, z);
      zaxis.normalize();

      mgPoint3 xaxis(0, 0, 1);
      xaxis.cross(zaxis);
      xaxis.normalize();

      mgPoint3 yaxis(zaxis);
      yaxis.cross(xaxis);

      int index = 6+i*lonCount + j;
      createSpine(6, spinePoints, spineSteps, spineLen, spineRadius, origin, xaxis, yaxis, zaxis);
    }
  }
}

//--------------------------------------------------------------
// create a spine
void Prickle::createSpine(
  int index,
  int spinePoints,
  int spineSteps,
  double spineLen,
  double spineRadius,
  const mgPoint3& origin,
  const mgPoint3& xaxis,
  const mgPoint3& yaxis,
  const mgPoint3& zaxis)
{
  int vertexBase = m_vertexes->getLength();

  // the spine is a cylinder with radius from 1.0 to 0.0, and 
  // centers shifted by a noise function.
  mgVertexTA v;
  for (int i = 0; i <= spineSteps; i++)
  {
    // get spine radius
    double z = i/(double) spineSteps;
    z -= 0.01;  // pull down a bit to hide base
    double radius = spineRadius * (1.0-z);

    // shift center by noise
    double dx = radius * mgSimplexNoise::noiseSum(10, origin.x, z*2.0);
    double dy = radius * mgSimplexNoise::noiseSum(10, origin.y, z*2.0);

    // vertexes are circle around point
    for (int j = 0; j <= spinePoints; j++)
    {
      double angle = (2*PI*j)/spinePoints;
      double x = cos(angle);
      double y = sin(angle);

      double px = dx + x*radius;
      double py = dy + y*radius;
      double pz = spineLen*z;

      v.m_px = (float) (origin.x + px*xaxis.x + py*yaxis.x + pz*zaxis.x); 
      v.m_py = (float) (origin.y + px*xaxis.y + py*yaxis.y + pz*zaxis.y); 
      v.m_pz = (float) (origin.z + px*xaxis.z + py*yaxis.z + pz*zaxis.z); 

      // normal is circle point 
      mgPoint3 normal(x * xaxis.x + y*yaxis.x + pz*zaxis.x, 
                      x * xaxis.y + y*yaxis.y + pz*zaxis.y, 
                      x * xaxis.z + y*yaxis.z + pz*zaxis.z);
      normal.normalize();
      v.setNormal(normal.x, normal.y, normal.z);
  
      // texture coordinates 
      v.m_tx = j/(float) spinePoints;
      v.m_ty = i/(float) spineSteps;  
      v.m_tz = (float) index;  

      m_vertexes->addVertex(&v);
    }
  }

  m_indexes->addGrid(vertexBase, spinePoints+1, spineSteps, spinePoints, true);
}

