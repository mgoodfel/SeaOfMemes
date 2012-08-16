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
const char* mgProgram = "Crafty";
const char* mgProgramVersion = "Part 60";

#include "StarrySky.h"
#include "Chunks/ChunkWorld.h"
#include "HelpUI.h"

#include "Crafty.h"

const double MIN_AVATAR_SPEED = 2.5/1000.0;   // units per ms
const double MAX_AVATAR_SPEED = MIN_AVATAR_SPEED * 128.0;  // units per ms

const double FOG_BOT_HEIGHT = 64.0;  // water line
const double FOG_TOP_HEIGHT = 128.0;  // top of buildings

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set the working directory to dir containing options
  mgOSFindWD("options.xml");

  return new Crafty();
}

//--------------------------------------------------------------
// constructor
Crafty::Crafty()
{
  m_sky = NULL;
  m_world = NULL;
  m_help = NULL;
}

//--------------------------------------------------------------
// destructor
Crafty::~Crafty()
{
}

//--------------------------------------------------------------
// initialize application
void Crafty::appInit()
{
  MovementApp::appInit();

  mgString title;
  title.format("Crafty, %s", mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // get lighting and material
  m_options.getPoint("matColor", mgPoint3(1,1,1), m_matColor);
  m_options.getPoint("sunlightColor", mgPoint3(0.8, 0.8, 0.8), m_sunlightColor);
  m_options.getPoint("moonlightColor", mgPoint3(0.1, 0.1, 0.1), m_moonlightColor);
  m_options.getPoint("torchlightColor", mgPoint3(0.5, 0.5, 0.5), m_torchlightColor);

  // since the view list goes up as the cube of distance, limit it
  int viewDistance = m_options.getInteger("viewDistance", 300);
  viewDistance = min(450, viewDistance);
  m_fogMaxDist = viewDistance;

  // create the sky
  m_lightDir = mgPoint3(0.0, 0.5, 1.0);
  m_sky = new StarrySky(m_options);
  m_sky->setMoonDir(m_lightDir);
  m_sky->setSunDir(m_lightDir);
  m_sky->setFogInten(1.0, 1.0);
  m_sky->setFogDist(1000.0, m_fogMaxDist);

  m_world = new ChunkWorld(m_options);
  m_world->setTorchColor(m_torchlightColor);
  m_world->setFogHeight(FOG_BOT_HEIGHT, FOG_TOP_HEIGHT);
  m_world->setFogInten(1.0, 1.0);
  m_world->setFogDist(m_fogMaxDist);

  setDaylight(true);
  setFoggy(true);

  m_eyePt =  mgPoint3(721, 72, -179);

  m_eyeRotX = -1.32; // -33;
  m_eyeRotY = -111.56; // -29;
  m_eyeRotZ = 0;

  setDeskMode(true);

  // create the help pane
  m_help = new HelpUI(m_options);
  m_help->setDebugApp(this);
  setUI(m_help);
}

//--------------------------------------------------------------
// terminate application
void Crafty::appTerm()
{
  delete m_sky;
  m_sky = NULL;

  delete m_world;
  m_world = NULL;

  delete m_help;
  m_help = NULL;

  mgBlockPool::freeMemory();

  MovementApp::appTerm();
}

//--------------------------------------------------------------------
// set foggy
void Crafty::setFoggy(
  BOOL foggy)
{
  m_foggy = foggy;
  m_world->setFogDist(m_foggy ? m_fogMaxDist : 2000.0);
  m_sky->enableFog(m_foggy);
  m_eyeChanged = true;
}

//--------------------------------------------------------------------
// set day or night
void Crafty::setDaylight(
  BOOL daylight)
{
  m_daylight = daylight;
  m_sky->enableSkyBox(m_daylight);
  m_sky->enableSun(m_daylight);

  m_sky->enableMoon(!m_daylight);
  m_sky->enableStars(!m_daylight);

  if (m_daylight)
  {
    m_world->setSkyColor(m_sunlightColor);
    m_sky->setFogColor(m_sunlightColor);
  }
  else 
  {
    m_world->setSkyColor(m_moonlightColor);
    m_sky->setFogColor(m_moonlightColor);
  }

  m_eyeChanged = true;
}

//--------------------------------------------------------------------
// animate the view
BOOL Crafty::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  // update position
  BOOL changed = MovementApp::appViewAnimate(now, since);

  // animate the sky
  changed |= m_sky->animate(now, since);

  // animate the world
  m_world->setEyePt(m_eyePt);
  changed |= m_world->animate(now, since);

  return changed;
}

//--------------------------------------------------------------------
// render the view
void Crafty::appViewDraw()
{
  // draw the sky
  mgDisplay->setFrontAndBack(0.25, 16384);

  mgMatrix4 identity;
  m_eyeMatrix.loadIdentity();
  m_eyeMatrix.rotateZDeg(m_eyeRotZ);
  m_eyeMatrix.rotateYDeg(m_eyeRotY);
  m_eyeMatrix.rotateXDeg(m_eyeRotX);

  mgDisplay->setEyeMatrix(m_eyeMatrix);
  mgDisplay->setEyePt(mgPoint3(0,0,0));

  mgDisplay->setModelTransform(identity);
  mgDisplay->setCulling(true);

  m_sky->setFogHeight(FOG_BOT_HEIGHT - m_eyePt.y, FOG_TOP_HEIGHT - m_eyePt.y);
  m_sky->render();

  // reset state after any changes in sky render
  mgDisplay->setLightDir(m_lightDir.x, m_lightDir.y, m_lightDir.z);
  mgDisplay->setMatColor(m_matColor.x, m_matColor.y, m_matColor.z);
  mgDisplay->setLightAmbient(0.0, 0.0, 0.0);
  mgDisplay->setTransparent(false);

  mgDisplay->setEyeMatrix(m_eyeMatrix);
  mgDisplay->setEyePt(m_eyePt);
  mgDisplay->setModelTransform(identity);

  m_world->render();

  mgDisplay->setTransparent(true);

  m_world->renderTransparent();

  mgDisplay->setTransparent(false);
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void Crafty::appCreateBuffers()
{
  m_sky->createBuffers();

  m_world->createBuffers();
}

//-----------------------------------------------------------------------------
// delete any display buffers
void Crafty::appDeleteBuffers()
{
  m_sky->deleteBuffers();

  m_world->deleteBuffers();
}

//-----------------------------------------------------------------------------
// key press
BOOL Crafty::moveKeyDown(
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

    case MG_EVENT_KEY_F6:
      mgDebug("stop");
      return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// key typed
BOOL Crafty::moveKeyChar(
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case 'N':
    case 'n':
      // toggle night/day
      setDaylight(!m_daylight);
      return true;

    case 'F':
    case 'f':
      // toggle fog
      setFoggy(!m_foggy);
      return true;

    case '=': 
    case '+':
    {
      m_moveSpeed = m_moveSpeed * 2.0;
      m_moveSpeed = min(MAX_AVATAR_SPEED, m_moveSpeed);
      if (m_help != NULL)
        m_help->addLine("Speed = %g kph (%g mph)", floor(0.5+m_moveSpeed*60*60), floor(0.5+m_moveSpeed*60*60*0.621371192));
      return true;
    }
      
    case '-':
    {
      m_moveSpeed = m_moveSpeed / 2.0;
      m_moveSpeed = max(MIN_AVATAR_SPEED, m_moveSpeed);
      if (m_help != NULL)
        m_help->addLine("Speed = %g kph (%g mph)", floor(0.5+m_moveSpeed*60*60), floor(0.5+m_moveSpeed*60*60*0.621371192));
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
// report status for debug log
void Crafty::appDebugStats(
  mgString& status)
{
  if (m_world != NULL)
    m_world->debugStats(status);
}


