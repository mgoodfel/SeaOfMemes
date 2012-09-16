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

/*
  Description:
    Parse and display BVH character animation
*/

#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

// identify the program for the framework log
const char* mgProgramName = "BVHView";
const char* mgProgramVersion = "Part 60";

#include "BVHView.h"
#include "HelpUI.h"
#include "BVHFile.h"

const double INVALID_TIME = -1.0;

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set working directory to dir containing options
  mgOSFindWD("options.xml");

  return new BVHView();
}

//--------------------------------------------------------------
// constructor
BVHView::BVHView()
{
  m_limbTexture = NULL;
  m_limbIndexes = NULL;
  m_limbVertexes = NULL;
  m_floorTexture = NULL;
  m_floorVertexes = NULL;
  m_help = NULL;
}

//--------------------------------------------------------------
// destructor
BVHView::~BVHView()
{
}

//--------------------------------------------------------------------
// initialize application
void BVHView::appInit()
{
  MovementApp::appInit();

  mgString title;
  title.format("%s, %s", mgProgramName, mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // load the shaders we use
  mgVertex::loadShader("litTexture");

  // load texture patterns
  loadTextures();

  // create vertex and index buffers
  m_limbIndexes = NULL;
  m_limbVertexes = NULL;
  m_floorVertexes = NULL;

  // create the help pane
  m_help = new HelpUI(m_options);
  m_help->setDebugApp(this);
  m_help->toggleHelp();
  setUI(m_help);

  mgString fileName;
  m_options.getFileName("model", m_options.m_sourceFileName, "", fileName);
  m_bvh = new BVHFile();
//  m_bvh->parseFile("143_25.bvh");  // wave
//  m_bvh->parseFile("143_32.bvh");  // walk
//  m_bvh->parseFile("143_17.bvh");  // up steps
//  m_bvh->parseFile("143_10.bvh");  // pick up box
//  m_bvh->parseFile("94_05.bvh");  // dance
  m_bvh->parseFile(fileName);
  mgDebug("%d channels, %d frames, time = %g", 
    m_bvh->m_totalChannels, m_bvh->m_frameCount, m_bvh->m_frameTime * m_bvh->m_frameCount);
}

//--------------------------------------------------------------------
// terminate application
void BVHView::appTerm()
{
  delete m_help;
  m_help = NULL;

  MovementApp::appTerm();
}

//--------------------------------------------------------------------
// load texture patterns from options
void BVHView::loadTextures()
{
  mgString fileName;

  m_options.getFileName("limb", m_options.m_sourceFileName, "", fileName);
  m_limbTexture = mgDisplay->loadTexture(fileName);

  // load floor texture
  m_options.getFileName("floor", m_options.m_sourceFileName, "", fileName);
  m_floorTexture = mgDisplay->loadTexture(fileName);
}

//--------------------------------------------------------------------
// animate the view
BOOL BVHView::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  if (m_bvh != NULL && m_limbVertexes != NULL)
  {
    int frame = (int) (now / (1000*m_bvh->m_frameTime));
    frame %= m_bvh->m_frameCount;
    double* samples = m_bvh->m_samples + frame * m_bvh->m_totalChannels;
    poseAvatar(samples);
  }
  MovementApp::appViewAnimate(now, since);
  return true;
}

//--------------------------------------------------------------------
// nothing to do
void BVHView::appViewDraw()
{
  mgDisplay->setEyeMatrix(m_eyeMatrix);

  // set eye point above and in front of cube
  mgDisplay->setEyePt(m_eyePt);

  // draw the floor
  if (m_floorVertexes != NULL)
  {
    mgMatrix4 floorModel;
    mgDisplay->setModelTransform(floorModel);

    mgDisplay->setShader("litTexture");
    mgDisplay->setTexture(m_floorTexture);
    mgDisplay->draw(MG_TRIANGLES, m_floorVertexes);
  }

  if (m_limbIndexes != NULL)
  {
    mgMatrix4 cubeModel;
    cubeModel.scale(0.01);
    mgDisplay->setModelTransform(cubeModel);

    // set drawing parameters
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setCulling(false);
    mgDisplay->setTransparent(false);

    // draw triangles using texture and shader
    mgDisplay->setShader("litTexture");
    mgDisplay->setTexture(m_limbTexture);
    mgDisplay->draw(MG_TRIANGLES, m_limbVertexes, m_limbIndexes);
  }
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void BVHView::appCreateBuffers()
{
  createAvatar();
  createFloor();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void BVHView::appDeleteBuffers()
{
  delete m_limbIndexes;
  m_limbIndexes = NULL;
  delete m_limbVertexes;
  m_limbVertexes = NULL;

  delete m_floorVertexes;
  m_floorVertexes = NULL;
}

//-----------------------------------------------------------------------------
// create the avatar
void BVHView::createAvatar()
{
  int count = m_bvh->m_root->nodeCount();

  // create vertexes for cube.  six sides * four vertexes
  m_limbVertexes = mgVertexTA::newBuffer(6*4*count, true);

  // create indexes for cube triangles.  six sides times two triangles times three points
  m_limbIndexes = mgDisplay->newIndexBuffer(6*2*3*count, true);

  mgMatrix4 xform;
  xform.translate(0, 0, 0);
  double* samples = NULL;
  createJoint(xform, m_bvh->m_root, samples);
}

//-----------------------------------------------------------------------------
// pose the avatar
void BVHView::poseAvatar(
  double* samples)
{
  m_limbVertexes->reset();
  m_limbIndexes->reset();

  mgMatrix4 xform;
  xform.translate(0, 0, 0);
  createJoint(xform, m_bvh->m_root, samples);
}

//-----------------------------------------------------------------------------
// create a joint of avatar
void BVHView::createJoint(
  const mgMatrix4& xform, 
  BVHNode* node,
  double*& samples)
{
  mgMatrix4 jointXform(xform);
  mgMatrix4 offset;
  offset.translate(node->m_offset);
  jointXform.leftMultiply(offset);
  if (samples != NULL)
  {
    for (int i = 0; i < node->m_channelCount; i++)
    {
      double value = *samples++;
      switch (node->m_channels[i])
      {
        case CHANNEL_XPOS:
        { 
          mgMatrix4 translate;
          translate._41 += value;
          jointXform.leftMultiply(translate);
          break;
        }
        case CHANNEL_YPOS: 
        { 
          mgMatrix4 translate;
          translate._42 += value;
          jointXform.leftMultiply(translate);
          break;
        }
        case CHANNEL_ZPOS: 
        { 
          mgMatrix4 translate;
          translate._43 += value;
          jointXform.leftMultiply(translate);
          break;
        }
        case CHANNEL_XROT: 
        {
          mgMatrix4 rotate;
          rotate.rotateXDeg(value);
          jointXform.leftMultiply(rotate);
          break;
        }
        case CHANNEL_YROT: 
        {
          mgMatrix4 rotate;
          rotate.rotateYDeg(value);
          jointXform.leftMultiply(rotate);
          break;
        }
        case CHANNEL_ZROT: 
        {
          mgMatrix4 rotate;
          rotate.rotateZDeg(value);
          jointXform.leftMultiply(rotate);
          break;
        }
      }
    }
  }

  mgPoint3 origin, extent;
  xform.mapPt(0, 0, 0, origin.x, origin.y, origin.z);
  jointXform.mapPt(0, 0, 0, extent.x, extent.y, extent.z);
  extent.subtract(origin);

  if (node != m_bvh->m_root)
    addLimb(origin, extent);

  for (int i = 0; i < node->m_children.length(); i++)
  {
    BVHNode* child = (BVHNode*) node->m_children[i];
    createJoint(jointXform, child, samples);
  }
}

//-----------------------------------------------------------------------------
// add limb to vertex and index buffers
void BVHView::addLimb(
  const mgPoint3& origin,
  const mgPoint3& extent)
{
  double len = extent.length();
  mgPoint3 zaxis(extent);
  zaxis.normalize();
  mgPoint3 xaxis(1, 1, 0);
  xaxis.normalize();
  xaxis.cross(zaxis);
  xaxis.normalize();
  mgPoint3 yaxis(zaxis);
  yaxis.cross(xaxis);
  yaxis.normalize();

  xaxis.scale(0.5);
  yaxis.scale(0.5);
  zaxis.scale(0.5);
  len *= 2.0;

  mgPoint3 ptLLL(origin.x - xaxis.x - yaxis.x - zaxis.x,
                 origin.y - xaxis.y - yaxis.y - zaxis.y,
                 origin.z - xaxis.z - yaxis.z - zaxis.z);
  mgPoint3 ptLLH(origin.x - xaxis.x - yaxis.x + zaxis.x*len,
                 origin.y - xaxis.y - yaxis.y + zaxis.y*len,
                 origin.z - xaxis.z - yaxis.z + zaxis.z*len);
  mgPoint3 ptLHL(origin.x - xaxis.x + yaxis.x - zaxis.x,
                 origin.y - xaxis.y + yaxis.y - zaxis.y,
                 origin.z - xaxis.z + yaxis.z - zaxis.z);
  mgPoint3 ptLHH(origin.x - xaxis.x + yaxis.x + zaxis.x*len,
                 origin.y - xaxis.y + yaxis.y + zaxis.y*len,
                 origin.z - xaxis.z + yaxis.z + zaxis.z*len);
  mgPoint3 ptHLL(origin.x + xaxis.x - yaxis.x - zaxis.x,
                 origin.y + xaxis.y - yaxis.y - zaxis.y,
                 origin.z + xaxis.z - yaxis.z - zaxis.z);
  mgPoint3 ptHLH(origin.x + xaxis.x - yaxis.x + zaxis.x*len,
                 origin.y + xaxis.y - yaxis.y + zaxis.y*len,
                 origin.z + xaxis.z - yaxis.z + zaxis.z*len);
  mgPoint3 ptHHL(origin.x + xaxis.x + yaxis.x - zaxis.x,
                 origin.y + xaxis.y + yaxis.y - zaxis.y,
                 origin.z + xaxis.z + yaxis.z - zaxis.z);
  mgPoint3 ptHHH(origin.x + xaxis.x + yaxis.x + zaxis.x*len,
                 origin.y + xaxis.y + yaxis.y + zaxis.y*len,
                 origin.z + xaxis.z + yaxis.z + zaxis.z*len);

  mgVertex tl, tr, bl, br;
  int index;

  // all sides have same texture coordinate for u,v
  tl.setTexture(0, 0);
  tr.setTexture(1, 0);
  bl.setTexture(0, 1);
  br.setTexture(1, 1);

  // x min side
  index = m_limbVertexes->getLength();

  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(ptLHH);
  tr.setPoint(ptLHL);
  bl.setPoint(ptLLH);
  br.setPoint(ptLLL);

  tl.addTo(m_limbVertexes);
  tr.addTo(m_limbVertexes);
  bl.addTo(m_limbVertexes);
  br.addTo(m_limbVertexes);

  // build indexes for a rectangle
  m_limbIndexes->addRectIndex(index);

  // x max side
  index = m_limbVertexes->getLength();

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setPoint(ptHHL);
  tr.setPoint(ptHHH);
  bl.setPoint(ptHLL);
  br.setPoint(ptHLH);

  tl.addTo(m_limbVertexes);
  tr.addTo(m_limbVertexes);
  bl.addTo(m_limbVertexes);
  br.addTo(m_limbVertexes);

  // build indexes for a rectangle
  m_limbIndexes->addRectIndex(index);

  // y min side
  index = m_limbVertexes->getLength();

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  tl.setPoint(ptLLL);
  tr.setPoint(ptHLL);
  bl.setPoint(ptLLH);
  br.setPoint(ptHLH);

  tl.addTo(m_limbVertexes);
  tr.addTo(m_limbVertexes);
  bl.addTo(m_limbVertexes);
  br.addTo(m_limbVertexes);

  // build indexes for a rectangle
  m_limbIndexes->addRectIndex(index);

  // y max side
  index = m_limbVertexes->getLength();

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(ptLHH);
  tr.setPoint(ptHHH);
  bl.setPoint(ptLHL);
  br.setPoint(ptHHL);

  tl.addTo(m_limbVertexes);
  tr.addTo(m_limbVertexes);
  bl.addTo(m_limbVertexes);
  br.addTo(m_limbVertexes);

  // build indexes for a rectangle
  m_limbIndexes->addRectIndex(index);

  // z min side
  index = m_limbVertexes->getLength();

  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setPoint(ptLHL);
  tr.setPoint(ptHHL);
  bl.setPoint(ptLLL);
  br.setPoint(ptHLL);

  tl.addTo(m_limbVertexes);
  tr.addTo(m_limbVertexes);
  bl.addTo(m_limbVertexes);
  br.addTo(m_limbVertexes);

  // build indexes for a rectangle
  m_limbIndexes->addRectIndex(index);

  // z max side
  index = m_limbVertexes->getLength();

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setPoint(ptHHH);
  tr.setPoint(ptLHH);
  bl.setPoint(ptHLH);
  br.setPoint(ptLLH);

  tl.addTo(m_limbVertexes);
  tr.addTo(m_limbVertexes);
  bl.addTo(m_limbVertexes);
  br.addTo(m_limbVertexes);

  // build indexes for a rectangle
  m_limbIndexes->addRectIndex(index);
}

//-----------------------------------------------------------------------------
// create vertex buffer for floor
void BVHView::createFloor()
{
  // create vertexes for floor.  six vertexes for two triangles
  m_floorVertexes = mgVertex::newBuffer(6);

  mgVertex tl, tr, bl, br;

  tl.setTexture(0, 0);
  tr.setTexture(20, 0);
  bl.setTexture(0, 20);
  br.setTexture(20, 20);

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(-20, 0,  20);
  tr.setPoint( 20, 0,  20);
  bl.setPoint(-20, 0, -20);
  br.setPoint( 20, 0, -20);

  tl.addTo(m_floorVertexes);
  tr.addTo(m_floorVertexes);
  bl.addTo(m_floorVertexes);

  bl.addTo(m_floorVertexes);
  tr.addTo(m_floorVertexes);
  br.addTo(m_floorVertexes);
}

//-----------------------------------------------------------------------------
// key press
BOOL BVHView::moveKeyDown(
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case MG_EVENT_KEY_F1:
      if (m_help != NULL)
        m_help->toggleHelp();
      return true;

    case MG_EVENT_KEY_F2:
      if (m_help != NULL)
        m_help->toggleConsole();
      return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// describe variables. 
void BVHView::debugListVariables(
  mgStringArray& varNames,
  mgStringArray& helpText) 
{
  // add the superclass variables
  MovementApp::debugListVariables(varNames, helpText);

  // add a variable of our own
  varNames.add("animate");
  helpText.add("set animation \"on\" or \"off\"");
}

//-----------------------------------------------------------------------------
// describe functions.  
void BVHView::debugListFunctions(
  mgStringArray& funcNames,
  mgStringArray& funcParms,
  mgStringArray& helpText) 
{
  // add the superclass functions
  MovementApp::debugListFunctions(funcNames, funcParms, helpText);

  // add a function of our own
  funcNames.add("reset");
  funcParms.add("");
  helpText.add("reset scene");
}

//-----------------------------------------------------------------------------
// return value of variable
void BVHView::debugGetVariable(
  const char* varName,
  mgString& value) 
{
  MovementApp::debugGetVariable(varName, value);
}

//-----------------------------------------------------------------------------
// set a variable
void BVHView::debugSetVariable(
  const char* varName,
  const char* value,
  mgString& reply) 
{
  MovementApp::debugSetVariable(varName, value, reply);
}

//-----------------------------------------------------------------------------
// call a function
void BVHView::debugCallFunction(
  const char* funcName,
  mgStringArray& args,
  mgString& reply) 
{
  // process function call
  if (_stricmp(funcName, "reset") == 0)
  {
    m_eyePt = mgPoint3(0, 2.5, -3.5);
    m_eyeRotX = 0.0;
    m_eyeRotY = 0.0;
    m_eyeRotZ = 0.0;
    m_eyeChanged = true;
    reply = "ok";
  }

  else MovementApp::debugCallFunction(funcName, args, reply);
}
