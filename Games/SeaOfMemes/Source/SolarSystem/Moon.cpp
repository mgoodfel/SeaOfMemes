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

#include "SolarSystem.h"
#include "Moon.h"

//--------------------------------------------------------------
// constructor
Moon::Moon(
  const mgOptionsFile& options,
  double radius)
{
  mgVertex::loadShader("litTextureCube");

  m_radius = radius/SYSTEM_FAR_SCALE;

  mgString xminFace, xmaxFace, yminFace, ymaxFace, zminFace, zmaxFace;
  options.getFileName("moon-xmin", options.m_sourceFileName, "", xminFace);
  options.getFileName("moon-xmax", options.m_sourceFileName, "", xmaxFace);
  options.getFileName("moon-ymin", options.m_sourceFileName, "", yminFace);
  options.getFileName("moon-ymax", options.m_sourceFileName, "", ymaxFace);
  options.getFileName("moon-zmin", options.m_sourceFileName, "", zminFace);
  options.getFileName("moon-zmax", options.m_sourceFileName, "", zmaxFace);

  m_farTexture = mgDisplay->loadTextureCube(xminFace, xmaxFace, yminFace, ymaxFace, zminFace, zmaxFace);
  m_farVertexes = NULL;
  m_farIndexes = NULL;
}

//--------------------------------------------------------------
// destructor
Moon::~Moon()
{
  deleteBuffers();
}
    
//--------------------------------------------------------------
// create buffers ready to send to display
void Moon::createBuffers()
{
  const mgPoint3 center(0,0,0);
  const int steps = 32;

  int vertexSize = 1+steps;

  m_farVertexes = mgVertexTA::newBuffer(6*vertexSize*vertexSize);
  m_farIndexes = mgDisplay->newIndexBuffer(6*6*steps*steps);

  double dx, dy, dz;
  mgVertexTA v;
  int vertexBase;

  // ZMIN side
  vertexBase = m_farVertexes->getLength();

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

      v.m_px = (float) (center.x + m_radius * v.m_nx);
      v.m_py = (float) (center.y + m_radius * v.m_ny);
      v.m_pz = (float) (center.z + m_radius * v.m_nz);

      v.m_tx = v.m_nx;
      v.m_ty = v.m_ny;
      v.m_tz = v.m_nz;

      m_farVertexes->addVertex(&v);
    }
  }

  m_farIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  vertexBase = m_farVertexes->getLength();

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

      v.m_px = (float) (center.x + m_radius * v.m_nx);
      v.m_py = (float) (center.y + m_radius * v.m_ny);
      v.m_pz = (float) (center.z + m_radius * v.m_nz);

      v.m_tx = v.m_nx;
      v.m_ty = v.m_ny;
      v.m_tz = v.m_nz;

      m_farVertexes->addVertex(&v);
    }
  }

  m_farIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  // ZMAX side
  vertexBase = m_farVertexes->getLength();

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

      v.m_px = (float) (center.x + m_radius * v.m_nx);
      v.m_py = (float) (center.y + m_radius * v.m_ny);
      v.m_pz = (float) (center.z + m_radius * v.m_nz);

      v.m_tx = v.m_nx;
      v.m_ty = v.m_ny;
      v.m_tz = v.m_nz;

      m_farVertexes->addVertex(&v);
    }
  }

  m_farIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  // XMAX side
  vertexBase = m_farVertexes->getLength();

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

      v.m_px = (float) (center.x + m_radius * v.m_nx);
      v.m_py = (float) (center.y + m_radius * v.m_ny);
      v.m_pz = (float) (center.z + m_radius * v.m_nz);

      v.m_tx = v.m_nx;
      v.m_ty = v.m_ny;
      v.m_tz = v.m_nz;

      m_farVertexes->addVertex(&v);
    }
  }

  m_farIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  // YMIN side
  vertexBase = m_farVertexes->getLength();

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

      v.m_px = (float) (center.x + m_radius * v.m_nx);
      v.m_py = (float) (center.y + m_radius * v.m_ny);
      v.m_pz = (float) (center.z + m_radius * v.m_nz);

      v.m_tx = v.m_nx;
      v.m_ty = v.m_ny;
      v.m_tz = v.m_nz;

      m_farVertexes->addVertex(&v);
    }
  }

  m_farIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);

  // YMAX side
  vertexBase = m_farVertexes->getLength();

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

      v.m_px = (float) (center.x + m_radius * v.m_nx);
      v.m_py = (float) (center.y + m_radius * v.m_ny);
      v.m_pz = (float) (center.z + m_radius * v.m_nz);

      v.m_tx = v.m_nx;
      v.m_ty = v.m_ny;
      v.m_tz = v.m_nz;

      m_farVertexes->addVertex(&v);
    }
  }

  m_farIndexes->addGrid(vertexBase, vertexSize, steps, steps, true);
}

//--------------------------------------------------------------
// delete display buffers
void Moon::deleteBuffers()
{
  delete m_farVertexes;
  m_farVertexes = NULL;

  delete m_farIndexes;
  m_farIndexes = NULL;
}

//--------------------------------------------------------------
// animate object
BOOL Moon::animate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  return false;
}

//--------------------------------------------------------------
// render far version
void Moon::renderFar()
{
  mgDisplay->setShader("litTextureCube");
  mgDisplay->setTexture(m_farTexture);
  mgDisplay->draw(MG_TRIANGLES, m_farVertexes, m_farIndexes);
}

//--------------------------------------------------------------
// render medium version
void Moon::renderMedium()
{
}

//--------------------------------------------------------------
// render near version
void Moon::renderNear()
{
}



