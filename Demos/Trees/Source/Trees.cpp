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

// identify the program for the framework log
const char* mgProgramName = "Trees";
const char* mgProgramVersion = "Part 60";

#include "HelpUI.h"
#include "Trees.h"

const int BRANCH_SIDES = 9;    // number of sides for branch cylinders
const double BRANCH_SCALE = 1/10.0;

const double INVALID_TIME = -1.0;

BOOL trace = false;

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set working directory to dir containing options
  mgOSFindWD("options.xml");

  return new Trees();
}

//--------------------------------------------------------------
// constructor
Trees::Trees()
{
  m_help = NULL;
  m_leafTexture = NULL;
  m_leafIndexes = NULL;
  m_leafVertexes = NULL;
  m_branchTexture = NULL;
  m_branchIndexes = NULL;
  m_branchVertexes = NULL;
  m_floorTexture = NULL;
  m_floorVertexes = NULL;
  m_skyTexture = NULL;
  m_skyVertexes = NULL;
}

//--------------------------------------------------------------
// destructor
Trees::~Trees()
{
}

//--------------------------------------------------------------
// initialize app
void Trees::appInit()
{
  MovementApp::appInit();

  mgString title;
  title.format("%s, %s", mgProgramName, mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // load shaders we'll use
  mgVertex::loadShader("litTexture");
  mgVertex::loadShader("unlitTexture");
  mgVertexTA::loadShader("litTextureArray");

  // load texture patterns
  loadTextures();

  m_eyePt = mgPoint3(120, 50.0, 0.0);
  m_eyeRotX = 10.0;
  m_eyeRotY = 90.0;
  m_moveSpeed = 25.0/1000.0;  

  // create initial "leaf" points and branches
  initTree();

  m_runAnimation = false;
  m_animateGrowth = 0.0;

  // create the help pane
  m_help = new HelpUI(m_options);
  m_help->setDebugApp(this);
  setUI(m_help);
}

//--------------------------------------------------------------------
// terminate application
void Trees::appTerm()
{
  delete m_help;
  m_help = NULL;

  MovementApp::appTerm();
}

//-----------------------------------------------------------------------------
// key press
BOOL Trees::moveKeyDown(
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

    case MG_EVENT_KEY_F4:
      m_tree.colonize();
      m_tree.branchSizes(false, m_lastCount, 1.0);
      renderTree();
      m_eyeChanged = true;
      return true;

    case MG_EVENT_KEY_F5:
      m_runAnimation = !m_runAnimation;
      return true;

    case MG_EVENT_KEY_F6:
      mgDebug("%d branches", m_tree.getBranchCount());
      return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void Trees::appCreateBuffers()
{
  createFloor();
  createSky();
  renderTree();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void Trees::appDeleteBuffers()
{
  delete m_leafIndexes;  m_leafIndexes = NULL;
  delete m_leafVertexes;  m_leafVertexes = NULL;

  delete m_branchIndexes;  m_branchIndexes = NULL;
  delete m_branchVertexes;  m_branchVertexes = NULL;

  delete m_floorVertexes;  m_floorVertexes = NULL;

  delete m_skyVertexes;  m_skyVertexes = NULL;
}

//--------------------------------------------------------------------
// load texture patterns from options
void Trees::loadTextures()
{
  mgStringArray leafNames;
  mgString fileName;

  // load "leaf" textures
  m_options.getFileName("leafunused", m_options.m_sourceFileName, "leaf-unused.jpg", fileName);
  leafNames.add(fileName);

  m_options.getFileName("leafused", m_options.m_sourceFileName, "leaf-used.jpg", fileName);
  leafNames.add(fileName);

  m_leafTexture = mgDisplay->loadTextureArray(leafNames);
  m_leafIndexes = NULL;
  m_leafVertexes = NULL;

  m_options.getFileName("branch", m_options.m_sourceFileName, "branch.jpg", fileName);
  m_branchTexture = mgDisplay->loadTexture(fileName);
  m_branchIndexes = NULL;
  m_branchVertexes = NULL;

  m_options.getFileName("ground", m_options.m_sourceFileName, "ground.jpg", fileName);
  m_floorTexture = mgDisplay->loadTexture(fileName);
  m_floorVertexes = NULL;

  m_options.getFileName("sky", m_options.m_sourceFileName, "sky.jpg", fileName);
  m_skyTexture = mgDisplay->loadTexture(fileName);
  m_skyVertexes = NULL;
}

//--------------------------------------------------------------------
// initialize leaf points and branches
void Trees::initTree()
{
  // generate a bunch of leaf points
  srand(12145688);
  mgPoint3 pt;
  double size = 200.0;
  for (int i = 0; i < 3000; i++)
  {
    while (true)
    {
      pt.x = size*mgRandom() - size/2;
      pt.y = size*mgRandom() - size/2;
      pt.z = size*mgRandom() - size/2;
      double len = sqrt(pt.x*pt.x + pt.y*pt.y + pt.z*pt.z);
      if (len > 0 && len < size/3) // height/5)
        break;
    }
    pt.y += 100;  // centering
    m_tree.addLeaf(pt);
  }

  // generate the stems
  int stems = 1;
  double range = size*0.6;
  for (int j = 0; j < stems; j++)
  {
    for (int i = 0; i < 25; i++)
    {
      pt.x = j*range/stems + range/(stems*2) - range/2;
      pt.y = i * GROW_DISTANCE;
      pt.z = 0.0;
      int parent = (i > 0) ? m_tree.getBranchCount()-1 : -1; 
      m_tree.addBranch(pt, parent);
    }
  }

  m_lastCount = m_tree.getBranchCount();

  // set size of branches
  m_tree.branchSizes(false, 0, 1.0);
}

//--------------------------------------------------------------------
// animate the view
BOOL Trees::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  BOOL changed = MovementApp::appViewAnimate(now, since);
  changed |= m_runAnimation;

  if (m_runAnimation)
  {
    m_animateGrowth += since/1000;
    if (m_animateGrowth >= 1.0)
    {
      m_lastCount = m_tree.getBranchCount();
      if (!m_tree.colonize())
        m_runAnimation = false;
      else m_animateGrowth -= 1.0;
    }
    m_tree.branchSizes(m_runAnimation, m_lastCount, m_animateGrowth);
    renderTree();
  }

  return changed;
}

//--------------------------------------------------------------------
// render the view
void Trees::appViewDraw()
{
  // draw sky
  if (m_skyVertexes != NULL)
  {
    mgDisplay->setEyePt(mgPoint3(0,0,0));
    mgDisplay->setEyeMatrix(m_eyeMatrix);

    mgDisplay->setZEnable(false);
    mgDisplay->setCulling(false);
    mgMatrix4 model;
    mgDisplay->setModelTransform(model);

    mgDisplay->setShader("unlitTexture");
    mgDisplay->setTexture(m_skyTexture);
    mgDisplay->draw(MG_TRIANGLES, m_skyVertexes);

    mgDisplay->setZEnable(true);
  }

  mgDisplay->setEyePt(m_eyePt);
  mgDisplay->setEyeMatrix(m_eyeMatrix);

  mgDisplay->setLightDir(100, 100, 0);
  // draw the floor
  if (m_floorVertexes != NULL)
  {
    mgMatrix4 floorModel;
    mgDisplay->setModelTransform(floorModel);

    mgDisplay->setShader("litTexture");
    mgDisplay->setTexture(m_floorTexture);
    mgDisplay->draw(MG_TRIANGLES, m_floorVertexes);
  }

  if (m_leafIndexes != NULL)
  {
    // set drawing parameters
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setCulling(true);
    mgDisplay->setTransparent(false);

    // draw triangles using texture and shader
    mgDisplay->setShader("litTextureArray");
    mgDisplay->setTexture(m_leafTexture);
    mgDisplay->draw(MG_TRIANGLES, m_leafVertexes, m_leafIndexes);
  }

  if (m_branchIndexes != NULL)
  {
    // set drawing parameters
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setCulling(true);
    mgDisplay->setTransparent(false);

    // draw triangles using texture and shader
    mgDisplay->setShader("litTexture");
    mgDisplay->setTexture(m_branchTexture);
    mgDisplay->draw(MG_TRIANGLES, m_branchVertexes, m_branchIndexes);
  }
}

//-----------------------------------------------------------------------------
// create buffers for rendering tree
void Trees::renderTree()
{
  if (m_leafIndexes != NULL)
  {
    delete m_leafIndexes;    m_leafIndexes = NULL;
    delete m_leafVertexes;    m_leafVertexes = NULL;
  }

  if (m_branchIndexes != NULL)
  {
    delete m_branchIndexes;    m_branchIndexes = NULL;
    delete m_branchVertexes;    m_branchVertexes = NULL;
  }

  createCubeBuffers(m_tree.getLeafCount(), m_leafVertexes, m_leafIndexes);
  for (int i = 0; i < m_tree.getLeafCount(); i++)
  {
    Leaf* leaf = m_tree.getLeaf(i);
    double color = leaf->m_active ? 0.0 : 1.0;
    if (leaf->m_active)
      addCube(m_leafVertexes, m_leafIndexes, leaf->m_pt, 0.5, 0.0); // color);
  }

  createBranchBuffers(m_tree.getBranchCount(), BRANCH_SIDES, m_branchVertexes, m_branchIndexes);

  mgPoint3* branchPts = new mgPoint3[m_tree.getBranchCount()];
  double* branchWidths = new double[m_tree.getBranchCount()];

  for (int i = m_tree.getBranchCount()-1; i >= 0; i--)
  {
    Branch* branch = m_tree.getBranch(i);
    if (branch->m_main != -1)
      continue;  // parent of some other branch
    int child = i;

    branchPts[0] = branch->m_pt;
    branchWidths[0] = 0.0;

    // do animation.  if i > lastCount, it's a new branch and must have a parent
    if (m_runAnimation && i > m_lastCount)
    {
      Branch* parent = m_tree.getBranch(branch->m_parent);
      mgPoint3 v(branch->m_pt);
      v.subtract(parent->m_pt);
      v.scale(m_animateGrowth);
      v.add(parent->m_pt);
      branchPts[0] = v;
    }

    int count = 0;
    while (branch->m_parent != -1)
    {
      Branch* parent = m_tree.getBranch(branch->m_parent);
      branchPts[count+1] = parent->m_pt;
      branchWidths[count+1] = sqrt(parent->m_area);
      count++;

      // if this is a root
      if (parent->m_main != child)
      {
        // fix base to reflect root, not parent width
        branchWidths[count] = sqrt(branch->m_area);
        break;
      }
      child = branch->m_parent;
      branch = parent;
    }
    if (count > 0)
      addBranch(m_branchVertexes, m_branchIndexes, count+1, BRANCH_SIDES, branchWidths, branchPts);
  }

  delete branchPts; branchPts = NULL;
  delete branchWidths; branchWidths = NULL;
}

//-----------------------------------------------------------------------------
// add a cube to the buffers
void Trees::addCube(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  const mgPoint3& center,
  double size,
  double tz)
{
  mgVertexTA tl, tr, bl, br;
  int index;

  // all sides have same texture coordinate for u,v
  tl.setTexture(0, 0);
  tr.setTexture(1, 0);
  bl.setTexture(0, 1);
  br.setTexture(1, 1);
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = (float) tz;

  // x min side
  index = vertexes->getLength();

  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(center.x-size, center.y+size, center.z+size);
  tr.setPoint(center.x-size, center.y+size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z+size);
  br.setPoint(center.x-size, center.y-size, center.z-size);

  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);

  // build indexes for a rectangle
  indexes->addRectIndex(index);

  // x max side
  index = vertexes->getLength();

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setPoint(center.x+size, center.y+size, center.z-size);
  tr.setPoint(center.x+size, center.y+size, center.z+size);
  bl.setPoint(center.x+size, center.y-size, center.z-size);
  br.setPoint(center.x+size, center.y-size, center.z+size);

  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);

  // build indexes for a rectangle
  indexes->addRectIndex(index);

  // y min side
  index = vertexes->getLength();

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  tl.setPoint(center.x-size, center.y-size, center.z-size);
  tr.setPoint(center.x+size, center.y-size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z+size);
  br.setPoint(center.x+size, center.y-size, center.z+size);

  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);

  // build indexes for a rectangle
  indexes->addRectIndex(index);

  // y max side
  index = vertexes->getLength();

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(center.x-size, center.y+size, center.z+size);
  tr.setPoint(center.x+size, center.y+size, center.z+size);
  bl.setPoint(center.x-size, center.y+size, center.z-size);
  br.setPoint(center.x+size, center.y+size, center.z-size);

  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);

  // build indexes for a rectangle
  indexes->addRectIndex(index);

  // z min side
  index = vertexes->getLength();

  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setPoint(center.x-size, center.y+size, center.z-size);
  tr.setPoint(center.x+size, center.y+size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z-size);
  br.setPoint(center.x+size, center.y-size, center.z-size);

  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);
  br.addTo(vertexes);

  // build indexes for a rectangle
  indexes->addRectIndex(index);

  // z max side
  index = vertexes->getLength();

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setPoint(center.x+size, center.y+size, center.z+size);
  tr.setPoint(center.x-size, center.y+size, center.z+size);
  bl.setPoint(center.x+size, center.y-size, center.z+size);
  br.setPoint(center.x-size, center.y-size, center.z+size);

  tl.addTo(vertexes);
  tr.addTo(vertexes);
  bl.addTo(vertexes);                 
  br.addTo(vertexes);

  // build indexes for a rectangle
  indexes->addRectIndex(index);
}

//-----------------------------------------------------------------------------
// create the buffers for a set of cubes
void Trees::createCubeBuffers(
  int cubeCount,
  mgVertexBuffer*& vertexes,
  mgIndexBuffer*& indexes)
{
  // create vertexes for cube.  six sides * four vertexes
  vertexes = mgVertexTA::newBuffer(cubeCount*6*4);

  // create indexes for cube triangles.  six sides times two triangles times three points
  indexes = mgDisplay->newIndexBuffer(cubeCount*6*2*3, false, true);
}

//-----------------------------------------------------------------------------
// get axis vectors for cross-section at point
void Trees::getAxis(
  mgPoint3& from,
  mgPoint3& to,
  mgPoint3& xaxis,
  mgPoint3& zaxis)
{
  // get vector from to A to B
  mgPoint3 yaxis(to);
  yaxis.subtract(from);
  yaxis.normalize();

  // cross products to get axis
  xaxis = yaxis;
  xaxis.cross(mgPoint3(0, 0, 1));
  xaxis.normalize();

  zaxis = xaxis;
  zaxis.cross(yaxis);
  zaxis.normalize();
}

//-----------------------------------------------------------------------------
// add a branch
void Trees::addBranch(
  mgVertexBuffer* vertexes,
  mgIndexBuffer* indexes,
  int count,
  int steps,
  double* branchWidths,
  mgPoint3* branchPts)
{
  int baseIndex = vertexes->getLength();
  double radius;
  mgPoint3 normal, from, to, xaxis, zaxis;
  mgVertex v;

  for (int i = 0; i <= steps; i++)
  {
    double angle = (2*PI*i)/steps;
    double x = cos(angle);
    double z = sin(angle);
    double height = 0.0;

    // add interior points
    for (int j = count-1; j > 0; j--)
    {
      // find axis at next segment
      from = branchPts[j];
      to = branchPts[j-1];
      radius = branchWidths[j];

      mgPoint3 oldNormal(normal);

      // get normal
      getAxis(from, to, xaxis, zaxis);
      normal.x = x*xaxis.x + z*zaxis.x;
      normal.y = x*xaxis.y + z*zaxis.y;
      normal.z = x*xaxis.z + z*zaxis.z;

      if (j < count-1)
      {
        normal.add(oldNormal);
        normal.scale(0.5);
      }

      // set point
      v.m_px = (float) (from.x + radius * normal.x);
      v.m_py = (float) (from.y + radius * normal.y);
      v.m_pz = (float) (from.z + radius * normal.z);

      // normal faces out
      v.m_nx = (float) normal.x;
      v.m_ny = (float) normal.y;
      v.m_nz = (float) normal.z;

      // texture coordinates
      v.m_tx = (float) (BRANCH_SCALE*2*PI*i)/steps;
      v.m_ty = (float) (BRANCH_SCALE*height);

      v.addTo(vertexes);

      mgPoint3 delta(to);
      delta.subtract(from);
      height += delta.length();
    }

    // add the top row
    from = branchPts[1];
    to = branchPts[0];
    radius = branchWidths[0];
    getAxis(from, to, xaxis, zaxis);

    // multiply by axis
    normal.x = x*xaxis.x + z*zaxis.x;
    normal.y = x*xaxis.y + z*zaxis.y;
    normal.z = x*xaxis.z + z*zaxis.z;

    // normal faces out
    v.m_nx = (float) normal.x;
    v.m_ny = (float) normal.y;
    v.m_nz = (float) normal.z;

    // set point
    v.m_px = (float) (to.x + radius * normal.x);
    v.m_py = (float) (to.y + radius * normal.y);
    v.m_pz = (float) (to.z + radius * normal.z);

    // texture coordinates
    v.m_tx = (float) (BRANCH_SCALE*2*PI*i)/steps;
    v.m_ty = (float) (BRANCH_SCALE*height);

    v.addTo(vertexes);
  }


  // add the indexes
  for (int i = 0; i < steps; i++)
  {
    int index = baseIndex + i*count;
    for (int j = 0; j < count-1; j++)
    {
      indexes->addIndex(index);  // tl
      indexes->addIndex(index+1);  // bl
      indexes->addIndex(index+count);  // tr

      indexes->addIndex(index+1);  // bl
      indexes->addIndex(index+count+1);  // br
      indexes->addIndex(index+count);  // tr

      index++;
    }
  }
}

//-----------------------------------------------------------------------------
// create the buffers for a set of cubes
void Trees::createBranchBuffers(
  int branchCount,
  int steps,
  mgVertexBuffer*& vertexes,
  mgIndexBuffer*& indexes)
{
  // create vertexes for cube.  six sides * four vertexes
  vertexes = mgVertex::newBuffer(branchCount*(steps+1)*2);

  // create indexes for cylinder triangles.  steps times two triangles times three points
  indexes = mgDisplay->newIndexBuffer(branchCount*steps*2*3, false, true);
}

//-----------------------------------------------------------------------------
// create vertex buffer for floor
void Trees::createFloor()
{
  // create vertexes for floor.  six vertexes for two triangles
  m_floorVertexes = mgVertex::newBuffer(6);

  mgVertex tl, tr, bl, br;

  tl.setTexture(   0,  0);
  tr.setTexture( 2,  0);
  bl.setTexture(   0, 2);
  br.setTexture( 2, 2);

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(-1000, 0,  1000);
  tr.setPoint( 1000, 0,  1000);
  bl.setPoint(-1000, 0, -1000);
  br.setPoint( 1000, 0, -1000);

  tl.addTo(m_floorVertexes);
  tr.addTo(m_floorVertexes);
  bl.addTo(m_floorVertexes);

  bl.addTo(m_floorVertexes);
  tr.addTo(m_floorVertexes);
  br.addTo(m_floorVertexes);
}

//-----------------------------------------------------------------------------
// create sky vertexes
void Trees::createSky()
{
  m_skyVertexes = mgVertex::newBuffer(6*6);

  mgPoint3 center(0,0,0);
  double size = 100.0;
  mgVertex tl, tr, bl, br;
  int index;

  // all sides have same texture coordinate for u,v
  tl.setTexture(0, 0);
  tr.setTexture(1, 0);
  bl.setTexture(0, 1);
  br.setTexture(1, 1);

  // x min side
  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(center.x-size, center.y+size, center.z+size);
  tr.setPoint(center.x-size, center.y+size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z+size);
  br.setPoint(center.x-size, center.y-size, center.z-size);

  tl.addTo(m_skyVertexes);
  bl.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  bl.addTo(m_skyVertexes);
  br.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  // x max side
  index = m_skyVertexes->getLength();

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setPoint(center.x+size, center.y+size, center.z-size);
  tr.setPoint(center.x+size, center.y+size, center.z+size);
  bl.setPoint(center.x+size, center.y-size, center.z-size);
  br.setPoint(center.x+size, center.y-size, center.z+size);

  tl.addTo(m_skyVertexes);
  bl.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  bl.addTo(m_skyVertexes);
  br.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  // y min side
  index = m_skyVertexes->getLength();

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  tl.setPoint(center.x-size, center.y-size, center.z-size);
  tr.setPoint(center.x+size, center.y-size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z+size);
  br.setPoint(center.x+size, center.y-size, center.z+size);

  tl.addTo(m_skyVertexes);
  bl.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  bl.addTo(m_skyVertexes);
  br.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  // y max side
  index = m_skyVertexes->getLength();

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(center.x-size, center.y+size, center.z+size);
  tr.setPoint(center.x+size, center.y+size, center.z+size);
  bl.setPoint(center.x-size, center.y+size, center.z-size);
  br.setPoint(center.x+size, center.y+size, center.z-size);

  tl.addTo(m_skyVertexes);
  bl.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  bl.addTo(m_skyVertexes);
  br.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  // z min side
  index = m_skyVertexes->getLength();

  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setPoint(center.x-size, center.y+size, center.z-size);
  tr.setPoint(center.x+size, center.y+size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z-size);
  br.setPoint(center.x+size, center.y-size, center.z-size);

  tl.addTo(m_skyVertexes);
  bl.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  bl.addTo(m_skyVertexes);
  br.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  // z max side
  index = m_skyVertexes->getLength();

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setPoint(center.x+size, center.y+size, center.z+size);
  tr.setPoint(center.x-size, center.y+size, center.z+size);
  bl.setPoint(center.x+size, center.y-size, center.z+size);
  br.setPoint(center.x-size, center.y-size, center.z+size);

  tl.addTo(m_skyVertexes);
  bl.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);

  bl.addTo(m_skyVertexes);
  br.addTo(m_skyVertexes);
  tr.addTo(m_skyVertexes);
}

