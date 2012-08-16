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
    The simplest demo of the framework.  Creates a rotating cube.

    For example of Debug Console use, find 'debugListVariables'
*/

#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

// identify the program for the framework log
const char* mgProgram = "TestCube";
const char* mgProgramVersion = "Part 60";

#include "TestCube.h"
#include "HelpUI.h"

const double INVALID_TIME = -1.0;

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set working directory to dir containing options
  mgOSFindWD("options.xml");

  return new TestCube();
}

//--------------------------------------------------------------
// constructor
TestCube::TestCube()
{
  m_cubeTexture = NULL;
  m_cubeIndexes = NULL;
  m_cubeVertexes = NULL;
  m_floorTexture = NULL;
  m_floorVertexes = NULL;
  m_help = NULL;
}

//--------------------------------------------------------------
// destructor
TestCube::~TestCube()
{
}

//--------------------------------------------------------------------
// initialize application
void TestCube::appInit()
{
  MovementApp::appInit();

  mgString title;
  title.format("%s, %s", mgProgram, mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // load the shaders we use
  mgVertex::loadShader("litTexture");
  mgVertexTA::loadShader("litTextureArray");

  // rotation angle for cube
  m_angle = 0.0;

  // load texture patterns
  loadTextures();

  // create vertex and index buffers
  m_cubeIndexes = NULL;
  m_cubeVertexes = NULL;
  m_floorVertexes = NULL;
  m_animate = true;

  // create the help pane
  m_help = new HelpUI(m_options);
  m_help->setDebugApp(this);
  setUI(m_help);
}

//--------------------------------------------------------------------
// terminate application
void TestCube::appTerm()
{
  delete m_help;
  m_help = NULL;

  MovementApp::appTerm();
}

//--------------------------------------------------------------------
// load texture patterns from options
void TestCube::loadTextures()
{
  mgStringArray faceNames;
  mgString fileName;

  // load cube face textures.  must all be same size.
  m_options.getFileName("xminface", m_options.m_sourceFileName, "face-xmin.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("xmaxface", m_options.m_sourceFileName, "face-xmax.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("yminface", m_options.m_sourceFileName, "face-ymin.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("ymaxface", m_options.m_sourceFileName, "face-ymax.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("zminface", m_options.m_sourceFileName, "face-zmin.jpg", fileName);
  faceNames.add(fileName);

  m_options.getFileName("zmaxface", m_options.m_sourceFileName, "face-zmax.jpg", fileName);
  faceNames.add(fileName);

  m_cubeTexture = mgDisplay->loadTextureArray(faceNames);

  // load floor texture
  m_options.getFileName("floor", m_options.m_sourceFileName, "docs/images/floor.jpg", fileName);
  m_floorTexture = mgDisplay->loadTexture(fileName);
}

//--------------------------------------------------------------------
// animate the view
BOOL TestCube::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  // rotate the cube
  if (m_animate)
    m_angle += since/100.0;

  BOOL changed = MovementApp::appViewAnimate(now, since);
  return changed || m_animate;
}

//--------------------------------------------------------------------
// nothing to do
void TestCube::appViewDraw()
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

  if (m_cubeIndexes != NULL)
  {
    mgMatrix4 cubeModel;
    // stand cube on point
    cubeModel.rotateXDeg(45);
    cubeModel.rotateZDeg(45);
    // rotate the cube based on angle set in animateView
    cubeModel.rotateYDeg(m_angle);
    cubeModel.translate(0, sqrt(3.0), 0);
    mgDisplay->setModelTransform(cubeModel);

    // set drawing parameters
    mgDisplay->setMatColor(1.0, 1.0, 1.0);
    mgDisplay->setCulling(true);
    mgDisplay->setTransparent(false);

    // draw triangles using texture and shader
    mgDisplay->setShader("litTextureArray");
    mgDisplay->setTexture(m_cubeTexture);
    mgDisplay->draw(MG_TRIANGLES, m_cubeVertexes, m_cubeIndexes);
  }
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void TestCube::appCreateBuffers()
{
  createCube();
  createFloor();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void TestCube::appDeleteBuffers()
{
  delete m_cubeIndexes;
  m_cubeIndexes = NULL;
  delete m_cubeVertexes;
  m_cubeVertexes = NULL;

  delete m_floorVertexes;
  m_floorVertexes = NULL;
}

//-----------------------------------------------------------------------------
// create vertex and index buffers for cube
void TestCube::createCube()
{
  // create vertexes for cube.  six sides * four vertexes
  m_cubeVertexes = mgVertexTA::newBuffer(6*4);

  // create indexes for cube triangles.  six sides times two triangles times three points
  m_cubeIndexes = mgDisplay->newIndexBuffer(6*2*3);

  mgVertexTA tl, tr, bl, br;
  int index;

  // all sides have same texture coordinate for u,v
  tl.setTexture(0, 0);
  tr.setTexture(1, 0);
  bl.setTexture(0, 1);
  br.setTexture(1, 1);

  // x min side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 0;  // face-xmin image

  tl.setNormal(-1, 0, 0);
  tr.setNormal(-1, 0, 0);
  bl.setNormal(-1, 0, 0);
  br.setNormal(-1, 0, 0);

  tl.setPoint(-1,  1,  1);
  tr.setPoint(-1,  1, -1);
  bl.setPoint(-1, -1,  1);
  br.setPoint(-1, -1, -1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // x max side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 1;  // face-xmax image

  tl.setNormal(1, 0, 0);
  tr.setNormal(1, 0, 0);
  bl.setNormal(1, 0, 0);
  br.setNormal(1, 0, 0);

  tl.setPoint(1,  1, -1);
  tr.setPoint(1,  1,  1);
  bl.setPoint(1, -1, -1);
  br.setPoint(1, -1,  1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // y min side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 2;  // face-ymin image

  tl.setNormal(0, -1, 0);
  tr.setNormal(0, -1, 0);
  bl.setNormal(0, -1, 0);
  br.setNormal(0, -1, 0);

  tl.setPoint(-1, -1, -1);
  tr.setPoint( 1, -1, -1);
  bl.setPoint(-1, -1,  1);
  br.setPoint( 1, -1,  1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // y max side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 3;  // face-ymax image

  tl.setNormal(0, 1, 0);
  tr.setNormal(0, 1, 0);
  bl.setNormal(0, 1, 0);
  br.setNormal(0, 1, 0);

  tl.setPoint(-1, 1,  1);
  tr.setPoint( 1, 1,  1);
  bl.setPoint(-1, 1, -1);
  br.setPoint( 1, 1, -1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // z min side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 4;  // face-zmin image

  tl.setNormal(0, 0, -1);
  tr.setNormal(0, 0, -1);
  bl.setNormal(0, 0, -1);
  br.setNormal(0, 0, -1);

  tl.setPoint(-1,  1, -1);
  tr.setPoint( 1,  1, -1);
  bl.setPoint(-1, -1, -1);
  br.setPoint( 1, -1, -1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);

  // z max side
  index = m_cubeVertexes->getLength();
  tl.m_tz = tr.m_tz = bl.m_tz = br.m_tz = 5;  // face-zmax image

  tl.setNormal(0, 0, 1);
  tr.setNormal(0, 0, 1);
  bl.setNormal(0, 0, 1);
  br.setNormal(0, 0, 1);

  tl.setPoint( 1,  1, 1);
  tr.setPoint(-1,  1, 1);
  bl.setPoint( 1, -1, 1);
  br.setPoint(-1, -1, 1);

  tl.addTo(m_cubeVertexes);
  tr.addTo(m_cubeVertexes);
  bl.addTo(m_cubeVertexes);
  br.addTo(m_cubeVertexes);

  // build indexes for a rectangle
  m_cubeIndexes->addRectIndex(index);
}

//-----------------------------------------------------------------------------
// create vertex buffer for floor
void TestCube::createFloor()
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

  tl.setPoint(-10, 0,  10);
  tr.setPoint( 10, 0,  10);
  bl.setPoint(-10, 0, -10);
  br.setPoint( 10, 0, -10);

  tl.addTo(m_floorVertexes);
  tr.addTo(m_floorVertexes);
  bl.addTo(m_floorVertexes);

  bl.addTo(m_floorVertexes);
  tr.addTo(m_floorVertexes);
  br.addTo(m_floorVertexes);
}

//-----------------------------------------------------------------------------
// key press
BOOL TestCube::moveKeyDown(
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
void TestCube::debugListVariables(
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
void TestCube::debugListFunctions(
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
void TestCube::debugGetVariable(
  const char* varName,
  mgString& value) 
{
  // return value of variables
  if (_stricmp(varName, "animate") == 0)
  {
    value = m_animate?"on": "off";
  }

  else MovementApp::debugGetVariable(varName, value);
}

//-----------------------------------------------------------------------------
// set a variable
void TestCube::debugSetVariable(
  const char* varName,
  const char* value,
  mgString& reply) 
{
  // set value of variable
  if (_stricmp(varName, "animate") == 0)
  {
    if (_stricmp(value, "on") == 0 || _stricmp(value, "1") == 0)
      m_animate = true;
    else m_animate = false;
  }

  else MovementApp::debugSetVariable(varName, value, reply);
}

//-----------------------------------------------------------------------------
// call a function
void TestCube::debugCallFunction(
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
