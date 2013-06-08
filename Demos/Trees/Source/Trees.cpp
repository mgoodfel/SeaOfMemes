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
const char* mgProgramName = "Trees";
const char* mgProgramVersion = "Part 83";

#include "HelpUI.h"
#include "StarrySky.h"
#include "Trees.h"

const int BRANCH_SIDES = 5;    // number of sides for branch cylinders
const double BRANCH_SCALE = 1/10.0;
const int TREE_POINTS = 300;

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
  m_sky = NULL;

  m_leafTexture = NULL;
  m_leafIndexes = NULL;
  m_leafVertexes = NULL;
  m_branchTexture = NULL;
  m_branchIndexes = NULL;
  m_branchVertexes = NULL;
  m_floorTexture = NULL;
  m_floorVertexes = NULL;
  m_shapeTexture = NULL;
  m_shapeIndexes = NULL;
  m_shapeVertexes = NULL;
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
  m_floorShader = mgVertex::loadShader("litTexture");
  m_branchShader = mgVertex::loadShader("litTexture");
  m_leafShader = mgVertexTA::loadShader("litTextureArray");
  m_shapeShader = mgVertex::loadShader("litTexture");

  // load texture patterns
  loadTextures();

  m_eyePt = mgPoint3(150, 40, 150); 
  m_eyeRotX = 7; 
  m_eyeRotY = 135;
  m_moveSpeed = 25.0/1000.0;  

  // create initial "leaf" points and branches
//  initShape();
  initTree();

  m_runAnimation = true;
  m_animateGrowth = 0.0;

  mgPoint3 lightDir(0, 1, 1);
  lightDir.normalize();
  mgDisplay->setLightDir(lightDir);
  mgDisplay->setLightAmbient(0.6, 0.6, 0.6);

  m_sky = new StarrySky(m_options);
  m_sky->enableStars(false);
  m_sky->enableSkyBox(true);
  m_sky->enableSun(true);

  m_sky->setSunDir(lightDir);

  m_sky->enableMoon(false);
  m_sky->setMoonDir(lightDir);

  // create the help pane
  m_help = new HelpUI(m_options);
  m_help->setDebugApp(this);
  setUI(m_help);

  // =-= turn off help for debug
  m_help->toggleHelp();
}

//--------------------------------------------------------------------
// terminate application
void Trees::appTerm()
{
  delete m_sky;
  m_sky = NULL;

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
      m_tree.computeBranchSizes(false, m_lastCount, 1.0);
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
  if (m_sky != NULL)
    m_sky->createBuffers();

  createFloor();
//  createShape();
  renderTree();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void Trees::appDeleteBuffers()
{
  if (m_sky != NULL)
    m_sky->deleteBuffers();

  delete m_leafIndexes;  
  m_leafIndexes = NULL;
  delete m_leafVertexes;  
  m_leafVertexes = NULL;

  delete m_branchIndexes;  
  m_branchIndexes = NULL;
  delete m_branchVertexes;  
  m_branchVertexes = NULL;

  delete m_floorVertexes;  
  m_floorVertexes = NULL;

  delete m_shapeIndexes;  
  m_shapeIndexes = NULL;
  delete m_shapeVertexes;  
  m_shapeVertexes = NULL;
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

/*
  m_options.getFileName("shape", m_options.m_sourceFileName, "shape.jpg", fileName);
  m_shapeTexture = mgDisplay->loadTexture(fileName);
  m_shapeVertexes = NULL;
  m_shapeIndexes = NULL;
*/
}

//--------------------------------------------------------------------
// initialize leaf points and branches
void Trees::initTree()
{
  // generate a bunch of leaf points
//  srand(12145688);
  mgPoint3 pt;
  mgPoint3 base;
  base.x = 0.0; // 180*(mgRandom()-0.5);
  base.z = 0.0; // 180*(mgRandom()-0.5);

  double lowY = 99999.0;
  double highY = 0.0;

  BOOL pine = mgRandom() > 0.5;
  if (pine)
  {
    double height = 100+200*mgRandom();
    double width = 50+100*mgRandom();
    base.y = 10*mgRandom();
    for (int i = 0; i < TREE_POINTS; i++)
    {
      // give the tree a definite boundary by generating
      // random points and throwing away ones near the outside edge.
      while (true)
      {
        pt.x = mgRandom()-0.5;
        pt.y = mgRandom();
        pt.z = mgRandom()-0.5;
        double len = sqrt(pt.x*pt.x + pt.z*pt.z);
        if (len < 1-pt.y)
          break;
      }
      pt.x = base.x + pt.x * width;
      pt.y = base.y + pt.y * height;
      pt.z = base.z + pt.z * width;
      lowY = min(lowY, pt.y);
      highY = max(highY, pt.y);
      m_tree.addLeaf(pt);
    }
  }
  else
  {
    double height = 100+200*mgRandom();
    double width = 50+300*mgRandom();
    base.y = height/2+50*mgRandom();
    for (int i = 0; i < TREE_POINTS; i++)
    {
      // give the tree a definite boundary by generating
      // random points and throwing away ones near the outside edge.
      while (true)
      {
        pt.x = mgRandom()-0.5;
        pt.y = mgRandom()-0.5;
        pt.z = mgRandom()-0.5;
        double len = pt.length();
        if (len < 0.4)
          break;
      }
      pt.x = base.x + pt.x * width;
      pt.y = base.y + pt.y * height;
      pt.z = base.z + pt.z * width;
      lowY = min(lowY, pt.y);
      highY = max(highY, pt.y);
      m_tree.addLeaf(pt);
    }
  }

  // generate the stems
  double top = pine ? highY : lowY;
  pt.x = base.x;
  pt.z = base.z;
  for (double y = 0.0; y < top; y += GROW_DISTANCE)
  {
    pt.y = y;
    int parent = (y > 0) ? m_tree.getBranchCount()-1 : -1; 
    m_tree.addBranch(pt, parent);
  }

  m_lastCount = m_tree.getBranchCount();

  // set size of branches
  m_tree.computeBranchSizes(false, 0, 1.0);
}

//--------------------------------------------------------------------
// build face
void buildFace(
  Colonization& tree,
  const mgPoint3& center,
  const mgPoint3& xaxis,
  const mgPoint3& yaxis)
{
  for (int i = 0; i < 1000; i++)
  {
    double x = 2*(mgRandom()-0.5);
    double y = 2*(mgRandom()-0.5);
    mgPoint3 pt(center);
    pt.x += xaxis.x * x + yaxis.x * y;
    pt.y += xaxis.y * x + yaxis.y * y;
    pt.z += xaxis.z * x + yaxis.z * y;
    tree.addLeaf(pt);
  }
}

//--------------------------------------------------------------------
// initialize leaf points and branches around shape
void Trees::initShape()
{
  // generate a bunch of leaf points
  srand(12145688);
  mgMatrix4 normal;
  normal.rotateXDeg(45);
  normal.rotateZDeg(45);
  normal.scale(70);
  mgMatrix4 translate(normal);
  translate.translate(0, 140, 0);

  mgPoint3 center, xaxis, yaxis;

  normal.mapPt(   mgPoint3(0, 0, 1), xaxis);
  normal.mapPt(   mgPoint3(0, 1, 0), yaxis);
  translate.mapPt(mgPoint3(1, 0, 0), center);
  buildFace(m_tree, center, xaxis, yaxis);  // +x

  normal.mapPt(   mgPoint3(0, 0, -1), xaxis);
  normal.mapPt(   mgPoint3(0, 1, 0), yaxis);
  translate.mapPt(mgPoint3(-1, 0, 0), center);
  buildFace(m_tree, center, xaxis, yaxis);  // -x

  normal.mapPt(   mgPoint3(1, 0, 0), xaxis);
  normal.mapPt(   mgPoint3(0, 1, 0), yaxis);
  translate.mapPt(mgPoint3(0, 0, 1), center);
  buildFace(m_tree, center, xaxis, yaxis);  // +z

  normal.mapPt(   mgPoint3(-1, 0, 0), xaxis);
  normal.mapPt(   mgPoint3(0, 1, 0), yaxis);
  translate.mapPt(mgPoint3(0, 0, -1), center);
  buildFace(m_tree, center, xaxis, yaxis);  // -z

  normal.mapPt(   mgPoint3(1, 0, 0), xaxis);
  normal.mapPt(   mgPoint3(0, 0, 1), yaxis);
  translate.mapPt(mgPoint3(0, 1, 0), center);
  buildFace(m_tree, center, xaxis, yaxis);  // +y

  normal.mapPt(   mgPoint3(1, 0, 0), xaxis);
  normal.mapPt(   mgPoint3(0, 0, -1), yaxis);
  translate.mapPt(mgPoint3(0, -1, 0), center);
  buildFace(m_tree, center, xaxis, yaxis);  // -y

  // generate the stems
  mgPoint3 corner, pt;
  translate.mapPt(mgPoint3(-1, -1, 1), corner);
  pt = corner;
  pt.y = 0;
  for (int i = 0; pt.y < corner.y; i++)
  {
    int parent = (i > 0) ? m_tree.getBranchCount()-1 : -1; 
    m_tree.addBranch(pt, parent);
    pt.y += GROW_DISTANCE;
  }

  m_lastCount = m_tree.getBranchCount();

  // set size of branches
  m_tree.computeBranchSizes(false, 0, 1.0);
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
    m_animateGrowth += since/400;
    if (m_animateGrowth >= 1.0)
    {
      m_lastCount = m_tree.getBranchCount();
      if (!m_tree.colonize())
      {
//        m_runAnimation = false;
        m_tree.reset();
        initTree();
      }
      else m_animateGrowth -= 1.0;
    }
    m_tree.computeBranchSizes(m_runAnimation, m_lastCount, m_animateGrowth);
    renderTree();
  }

  return changed;
}

//--------------------------------------------------------------------
// render the view
void Trees::appViewDraw()
{
  // draw sky
  if (m_sky != NULL)
  {
    mgDisplay->setEyePt(mgPoint3(0,0,0));
    mgDisplay->setEyeMatrix(m_eyeMatrix);

    m_sky->render();
  }

  mgDisplay->setEyePt(m_eyePt);
  mgDisplay->setEyeMatrix(m_eyeMatrix);

  mgDisplay->setMatColor(1.0, 1.0, 1.0);
  mgDisplay->setCulling(true);
  mgDisplay->setTransparent(false);

  // draw the floor
  if (m_floorVertexes != NULL)
  {
    mgMatrix4 floorModel;
    mgDisplay->setModelTransform(floorModel);

    mgDisplay->setShader(m_floorShader);
    mgDisplay->setTexture(m_floorTexture);
    mgDisplay->draw(MG_TRIANGLES, m_floorVertexes);
  }

  // draw the shape
  if (m_shapeVertexes != NULL)
  {
    mgMatrix4 shapeModel;
    shapeModel.rotateXDeg(45);
    shapeModel.rotateZDeg(45);
    shapeModel.scale(70);
    shapeModel.translate(0, 140, 0);
    mgDisplay->setModelTransform(shapeModel);

    mgDisplay->setShader(m_shapeShader);
    mgDisplay->setTexture(m_shapeTexture);
    mgDisplay->draw(MG_TRIANGLES, m_shapeVertexes, m_shapeIndexes);
  }

  mgMatrix4 treeModel;
  mgDisplay->setModelTransform(treeModel);

  if (m_leafIndexes != NULL)
  {
    // draw triangles using texture and shader
    mgDisplay->setTransparent(true);
    mgDisplay->setShader(m_leafShader);
    mgDisplay->setTexture(m_leafTexture);
    mgDisplay->draw(MG_TRIANGLES, m_leafVertexes, m_leafIndexes);
    mgDisplay->setTransparent(false);
  }

  if (m_branchIndexes != NULL)
  {
    // draw triangles using texture and shader
    mgDisplay->setShader(m_branchShader);
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

  createLeafBuffers();

  // get the number of branch points
  int pointCount = 0;
  for (int i = m_tree.getBranchCount()-1; i >= 0; i--)
  {
    Branch* branch = m_tree.getBranch(i);
    if (branch->m_path != -1)
      continue;  // parent of some other branch
    int child = i;

    int count = 0;
    while (branch->m_parent != -1)
    {
      Branch* parent = m_tree.getBranch(branch->m_parent);
      count++;

      // if this is a root
      if (parent->m_path != child)
        break;

      child = branch->m_parent;
      branch = parent;
    }
    if (count > 0)
      pointCount += count+1;
  }

  // create vertexes for cube.  six sides * four vertexes
  m_branchVertexes = mgVertex::newBuffer(pointCount*(BRANCH_SIDES+1));

  // create indexes for cylinder triangles.  steps times two triangles times three points
  m_branchIndexes = mgDisplay->newIndexBuffer(pointCount*BRANCH_SIDES*2*3);

  mgPoint3* branchPts = new mgPoint3[m_tree.getBranchCount()];
  double* branchWidths = new double[m_tree.getBranchCount()];

  for (int i = m_tree.getBranchCount()-1; i >= 0; i--)
  {
    Branch* branch = m_tree.getBranch(i);
    if (branch->m_path != -1)
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
      if (parent->m_path != child)
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
void Trees::addLeaf(
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
  index = m_leafVertexes->getLength();

  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(center.x-size, center.y+size, center.z+size);
  tr.setPoint(center.x-size, center.y+size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z+size);
  br.setPoint(center.x-size, center.y-size, center.z-size);

  tl.addTo(m_leafVertexes);
  tr.addTo(m_leafVertexes);
  bl.addTo(m_leafVertexes);
  br.addTo(m_leafVertexes);

  // build m_leafIndexes for a rectangle
  m_leafIndexes->addRectIndex(index);

  // x max side
  index = m_leafVertexes->getLength();

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setPoint(center.x+size, center.y+size, center.z-size);
  tr.setPoint(center.x+size, center.y+size, center.z+size);
  bl.setPoint(center.x+size, center.y-size, center.z-size);
  br.setPoint(center.x+size, center.y-size, center.z+size);

  tl.addTo(m_leafVertexes);
  tr.addTo(m_leafVertexes);
  bl.addTo(m_leafVertexes);
  br.addTo(m_leafVertexes);

  // build m_leafIndexes for a rectangle
  m_leafIndexes->addRectIndex(index);

  // y min side
  index = m_leafVertexes->getLength();

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  tl.setPoint(center.x-size, center.y-size, center.z-size);
  tr.setPoint(center.x+size, center.y-size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z+size);
  br.setPoint(center.x+size, center.y-size, center.z+size);

  tl.addTo(m_leafVertexes);
  tr.addTo(m_leafVertexes);
  bl.addTo(m_leafVertexes);
  br.addTo(m_leafVertexes);

  // build m_leafIndexes for a rectangle
  m_leafIndexes->addRectIndex(index);

  // y max side
  index = m_leafVertexes->getLength();

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(center.x-size, center.y+size, center.z+size);
  tr.setPoint(center.x+size, center.y+size, center.z+size);
  bl.setPoint(center.x-size, center.y+size, center.z-size);
  br.setPoint(center.x+size, center.y+size, center.z-size);

  tl.addTo(m_leafVertexes);
  tr.addTo(m_leafVertexes);
  bl.addTo(m_leafVertexes);
  br.addTo(m_leafVertexes);

  // build m_leafIndexes for a rectangle
  m_leafIndexes->addRectIndex(index);

  // z min side
  index = m_leafVertexes->getLength();

  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setPoint(center.x-size, center.y+size, center.z-size);
  tr.setPoint(center.x+size, center.y+size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z-size);
  br.setPoint(center.x+size, center.y-size, center.z-size);

  tl.addTo(m_leafVertexes);
  tr.addTo(m_leafVertexes);
  bl.addTo(m_leafVertexes);
  br.addTo(m_leafVertexes);

  // build m_leafIndexes for a rectangle
  m_leafIndexes->addRectIndex(index);

  // z max side
  index = m_leafVertexes->getLength();

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setPoint(center.x+size, center.y+size, center.z+size);
  tr.setPoint(center.x-size, center.y+size, center.z+size);
  bl.setPoint(center.x+size, center.y-size, center.z+size);
  br.setPoint(center.x-size, center.y-size, center.z+size);

  tl.addTo(m_leafVertexes);
  tr.addTo(m_leafVertexes);
  bl.addTo(m_leafVertexes);                 
  br.addTo(m_leafVertexes);

  // build m_leafIndexes for a rectangle
  m_leafIndexes->addRectIndex(index);
}

//-----------------------------------------------------------------------------
// create the buffers for the leaves
void Trees::createLeafBuffers()
{
  int cubeCount = m_tree.getLeafCount();

  // create vertexes for cube.  six sides * four vertexes
  m_leafVertexes = mgVertexTA::newBuffer(cubeCount*6*4);

  // create indexes for cube triangles.  six sides times two triangles times three points
  m_leafIndexes = mgDisplay->newIndexBuffer(cubeCount*6*2*3);

  for (int i = 0; i < cubeCount; i++)
  {
    Leaf* leaf = m_tree.getLeaf(i);
    double color = leaf->m_active ? 0.0 : 1.0;
    double size = 0.5; // leaf->m_active ? 0.5 : 3.0;
//    if (leaf->m_active)
      addLeaf(leaf->m_pt, size, color);
  }
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
  zaxis.set(1/1.414, -1/1.414, 0);
  zaxis.cross(yaxis);
  zaxis.normalize();

  xaxis = yaxis;
  xaxis.cross(zaxis);
  xaxis.normalize();
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
// create vertex buffer for floor
void Trees::createFloor()
{
  // create vertexes for floor.  six vertexes for two triangles
  m_floorVertexes = mgVertex::newBuffer(6*6*40*40);

  mgVertex tl, tr, bl, br;
  double lx, hx, ly, hy, lz, hz;
  const double size = 20;
  const double thick = size;

  // nearby terrain
  for (int x = -20; x < 20; x++)
  {
    for (int z = -20; z < 20; z++)
    {
      tl.setTexture(0, 1);
      tr.setTexture(1, 1);
      bl.setTexture(0, 0);
      br.setTexture(1, 0);

      tl.setNormal(0, 1, 0);
      tr.setNormal(0, 1, 0);
      bl.setNormal(0, 1, 0);
      br.setNormal(0, 1, 0);

      lx = x*size;
      hx = (x+1)*size;
      lz = z*size;
      hz = (z+1)*size;

      hy = mgSimplexNoise::noiseSum(10, lx, lz); 
      ly = hy-thick;

      // y max side
      tl.setPoint(lx, hy, hz);
      tr.setPoint(hx, hy, hz);
      bl.setPoint(lx, hy, lz);
      br.setPoint(hx, hy, lz);

      tl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      bl.addTo(m_floorVertexes);

      bl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      br.addTo(m_floorVertexes);

      tl.setTexture(0, hy/size);
      tr.setTexture(1, hy/size);
      bl.setTexture(0, ly/size);
      br.setTexture(1, ly/size);

      // x min side
      tl.setNormal(-1, 0, 0);
      tr.setNormal(-1, 0, 0);
      bl.setNormal(-1, 0, 0);
      br.setNormal(-1, 0, 0);

      tl.setPoint(lx, hy, hz);
      tr.setPoint(lx, hy, lz);
      bl.setPoint(lx, ly, hz);
      br.setPoint(lx, ly, lz);

      tl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      bl.addTo(m_floorVertexes);

      bl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      br.addTo(m_floorVertexes);

      // x max side
      tl.setNormal(1, 0, 0);
      tr.setNormal(1, 0, 0);
      bl.setNormal(1, 0, 0);
      br.setNormal(1, 0, 0);

      tl.setPoint(hx, hy, lz);
      tr.setPoint(hx, hy, hz);
      bl.setPoint(hx, ly, lz);
      br.setPoint(hx, ly, hz);

      tl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      bl.addTo(m_floorVertexes);

      bl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      br.addTo(m_floorVertexes);

      // z min side
      tl.setNormal(0, 0, -1);
      tr.setNormal(0, 0, -1);
      bl.setNormal(0, 0, -1);
      br.setNormal(0, 0, -1);

      tl.setPoint(lx, hy, lz);
      tr.setPoint(hx, hy, lz);
      bl.setPoint(lx, ly, lz);
      br.setPoint(hx, ly, lz);

      tl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      bl.addTo(m_floorVertexes);

      bl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      br.addTo(m_floorVertexes);

      // z max side
      tl.setNormal(0, 0, 1);
      tr.setNormal(0, 0, 1);
      bl.setNormal(0, 0, 1);
      br.setNormal(0, 0, 1);

      tl.setPoint(hx, hy, hz);
      tr.setPoint(lx, hy, hz);
      bl.setPoint(hx, ly, hz);
      br.setPoint(lx, ly, hz);

      tl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      bl.addTo(m_floorVertexes);

      bl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      br.addTo(m_floorVertexes);

      // y min side
      tl.setTexture(0, 0);
      tr.setTexture(1, 0);
      bl.setTexture(0, 1);
      br.setTexture(1, 1);

      tl.setNormal(0, -1, 0);
      tr.setNormal(0, -1, 0);
      bl.setNormal(0, -1, 0);
      br.setNormal(0, -1, 0);

      tl.setPoint(lx, ly, lz);
      tr.setPoint(hx, ly, lz);
      bl.setPoint(lx, ly, hz);
      br.setPoint(hx, ly, hz);

      tl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      bl.addTo(m_floorVertexes);

      bl.addTo(m_floorVertexes);
      tr.addTo(m_floorVertexes);
      br.addTo(m_floorVertexes);
    }
  }

}

//-----------------------------------------------------------------------------
// create shape
void Trees::createShape()
{
  // create vertexes for cube.  six sides * four vertexes
  m_shapeVertexes = mgVertexTA::newBuffer(6*4);

  // create indexes for cube triangles.  six sides times two triangles times three points
  m_shapeIndexes = mgDisplay->newIndexBuffer(6*2*3, false, false);

  mgPoint3 center(0, 0, 0);
  double size = 1.0;
  mgVertex tl, tr, bl, br;
  int index;

  // all sides have same texture coordinate for u,v
  tl.setTexture(0, 0);
  tr.setTexture(1, 0);
  bl.setTexture(0, 1);
  br.setTexture(1, 1);

  // x min side
  index = m_shapeVertexes->getLength();

  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(center.x-size, center.y+size, center.z+size);
  tr.setPoint(center.x-size, center.y+size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z+size);
  br.setPoint(center.x-size, center.y-size, center.z-size);

  tl.addTo(m_shapeVertexes);
  tr.addTo(m_shapeVertexes);
  bl.addTo(m_shapeVertexes);
  br.addTo(m_shapeVertexes);

  // build m_shapeIndexes for a rectangle
  m_shapeIndexes->addRectIndex(index);

  // x max side
  index = m_shapeVertexes->getLength();

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setPoint(center.x+size, center.y+size, center.z-size);
  tr.setPoint(center.x+size, center.y+size, center.z+size);
  bl.setPoint(center.x+size, center.y-size, center.z-size);
  br.setPoint(center.x+size, center.y-size, center.z+size);

  tl.addTo(m_shapeVertexes);
  tr.addTo(m_shapeVertexes);
  bl.addTo(m_shapeVertexes);
  br.addTo(m_shapeVertexes);

  // build m_shapeIndexes for a rectangle
  m_shapeIndexes->addRectIndex(index);

  // y min side
  index = m_shapeVertexes->getLength();

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  tl.setPoint(center.x-size, center.y-size, center.z-size);
  tr.setPoint(center.x+size, center.y-size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z+size);
  br.setPoint(center.x+size, center.y-size, center.z+size);

  tl.addTo(m_shapeVertexes);
  tr.addTo(m_shapeVertexes);
  bl.addTo(m_shapeVertexes);
  br.addTo(m_shapeVertexes);

  // build m_shapeIndexes for a rectangle
  m_shapeIndexes->addRectIndex(index);

  // y max side
  index = m_shapeVertexes->getLength();

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(center.x-size, center.y+size, center.z+size);
  tr.setPoint(center.x+size, center.y+size, center.z+size);
  bl.setPoint(center.x-size, center.y+size, center.z-size);
  br.setPoint(center.x+size, center.y+size, center.z-size);

  tl.addTo(m_shapeVertexes);
  tr.addTo(m_shapeVertexes);
  bl.addTo(m_shapeVertexes);
  br.addTo(m_shapeVertexes);

  // build m_shapeIndexes for a rectangle
  m_shapeIndexes->addRectIndex(index);

  // z min side
  index = m_shapeVertexes->getLength();

  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setPoint(center.x-size, center.y+size, center.z-size);
  tr.setPoint(center.x+size, center.y+size, center.z-size);
  bl.setPoint(center.x-size, center.y-size, center.z-size);
  br.setPoint(center.x+size, center.y-size, center.z-size);

  tl.addTo(m_shapeVertexes);
  tr.addTo(m_shapeVertexes);
  bl.addTo(m_shapeVertexes);
  br.addTo(m_shapeVertexes);

  // build m_shapeIndexes for a rectangle
  m_shapeIndexes->addRectIndex(index);

  // z max side
  index = m_shapeVertexes->getLength();

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setPoint(center.x+size, center.y+size, center.z+size);
  tr.setPoint(center.x-size, center.y+size, center.z+size);
  bl.setPoint(center.x+size, center.y-size, center.z+size);
  br.setPoint(center.x-size, center.y-size, center.z+size);

  tl.addTo(m_shapeVertexes);
  tr.addTo(m_shapeVertexes);
  bl.addTo(m_shapeVertexes);                 
  br.addTo(m_shapeVertexes);

  // build m_shapeIndexes for a rectangle
  m_shapeIndexes->addRectIndex(index);
}

