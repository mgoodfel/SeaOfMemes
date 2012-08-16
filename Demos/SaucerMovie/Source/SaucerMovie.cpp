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
const char* mgProgram = "SaucerMovie";
const char* mgProgramVersion = "Part 60";

#include "StarrySky.h"
#include "Saucer.h"
#include "DeathRay.h"
#include "SaucerMovie.h"
#include "HelpUI.h"

const double INVALID_TIME = -1.0;
const int BOTH_BUTTONS = MG_EVENT_MOUSE1_DOWN | MG_EVENT_MOUSE2_DOWN;

const double NOISE_RANGE = 100.0;
const double NOISE_NEAR = 20.0;
const double NOISE_FAR = 500.0;

//--------------------------------------------------------------
// create and return an instance of your application
mgApplication *mgCreateApplication()
{
  // set working directory to dir containing options
  mgOSFindWD("options.xml");

  return new SaucerMovie();
}

//--------------------------------------------------------------
// constructor
SaucerMovie::SaucerMovie()
{
  m_help = NULL;
  m_earthTexture = NULL;
  m_earthVertexes = NULL;
  m_sky = NULL;
  m_audio = NULL;
  m_badSaucer = NULL;
  m_killer1 = NULL;
  m_killer2 = NULL;
}

//--------------------------------------------------------------
// destructor
SaucerMovie::~SaucerMovie()
{
}

//--------------------------------------------------------------
// initialize application
void SaucerMovie::appInit()
{
  MovementApp::appInit();

  mgString title;
  title.format("%s, %s", mgProgram, mgProgramVersion);
  mgPlatform->setWindowTitle(title);

  // load the shaders we might use
  mgVertex::loadShader("litTexture");
  mgVertex::loadShader("unlitTexture");
  mgVertex::loadShader("litTextureCube");
  mgVertex::loadShader("unlitTextureCube");
  mgVertexTA::loadShader("litTextureArray");
  mgVertexTA::loadShader("unlitTextureArray");

  mgString fileName;
  m_options.getFileName("planetImage", m_options.m_sourceFileName, "", fileName);
  m_earthTexture = mgDisplay->loadTexture(fileName);

  m_sky = new StarrySky(m_options);
  m_nebula = false;
  m_sky->enableSkyBox(m_nebula);
  m_sky->enableStars(true);
//  m_sky->enableSun(true);

  m_audio = mgAudio::create();
  m_music = -1;

  // get sound files to use
  m_options.getFileName("saucerSound", m_options.m_sourceFileName, "", m_saucerSound);
  m_options.getFileName("raySound", m_options.m_sourceFileName, "", m_raySound);
  m_options.getFileName("music", m_options.m_sourceFileName, "", m_musicSound);

  m_movieScene = -1;
  m_movieTime = 0.0;

  m_eyeRotX = 21.0;
  m_eyeRotY = -90.0;
  m_eyeRotZ = 0.0;
  m_eyePt = mgPoint3(75, 0, -1000.0);

  m_moveSpeed = 20.0/1000;             // units per ms // 100.0/1000;

  // create the help pane
  m_help = new HelpUI(m_options);
  m_help->setDebugApp(this);
  setUI(m_help);
}

//--------------------------------------------------------------
// destructor
void SaucerMovie::appTerm()
{
  for (int i = 0; i < m_saucers.length(); i++)
  {
    Saucer* saucer = (Saucer*) m_saucers[i];
    delete saucer;
  }
  m_saucers.removeAll();

  for (int i = 0; i < m_rays.length(); i++)
  {
    DeathRay* ray = (DeathRay*) m_rays[i];
    delete ray;
  }
  m_rays.removeAll();

  delete m_sky;
  m_sky = NULL;

  delete m_audio;
  m_audio = NULL;

  delete m_help;
  m_help = NULL;

  MovementApp::appTerm();
}

//--------------------------------------------------------------------
// place a saucer in the cloud
void SaucerMovie::placeSaucer(
  Saucer* saucer)
{
  BOOL tooClose = true;
  while (tooClose)
  {
    saucer->m_origin.x = -150+300*mgRandom();
    saucer->m_origin.y = -150+300*mgRandom();
    saucer->m_origin.z = -1050 - 1000*mgRandom();
    tooClose = false;
    for (int j = 0; j < m_saucers.length(); j++)
    {
      Saucer* other = (Saucer*) m_saucers[j];
      mgPoint3 dist(other->m_origin);
      dist.subtract(saucer->m_origin);
      if (dist.length() < 10)
      {
        tooClose = true;
        break;
      }
    }
  }
}

//--------------------------------------------------------------------
// load texture patterns from options
void SaucerMovie::createEarth()
{
  m_earthVertexes = mgVertex::newBuffer(6);
  mgVertex v;
  v.setNormal(0, 0, -1);

  double w = 1536/2.0;
  double h = 1200/2.0;

  v.setPoint(-w, h, 0);  // tl
  v.setTexture(0, 0);
  m_earthVertexes->addVertex(&v);

  v.setPoint(w, h, 0);   // tr
  v.setTexture(1, 0);
  m_earthVertexes->addVertex(&v);

  v.setPoint(-w, -h, 0);  // bl
  v.setTexture(0, 1);
  m_earthVertexes->addVertex(&v);


  v.setPoint(-w, -h, 0);  // bl 
  v.setTexture(0, 1);
  m_earthVertexes->addVertex(&v);

  v.setPoint(w, h, 0);  // tr
  v.setTexture(1, 0);
  m_earthVertexes->addVertex(&v);

  v.setPoint(w, -h, 0);  // br
  v.setTexture(1, 1);
  m_earthVertexes->addVertex(&v);
}

const mgPoint3 INIT_SAUCERS[] = {
  mgPoint3(100, 10, -1100.0),
  mgPoint3(120, -10, -1200.0),
  mgPoint3(110, 5, -1300.0),
  mgPoint3(90, -5, -1350.0),
  mgPoint3(0, 0, 0),
};

//--------------------------------------------------------------------
// do a step of movie animation
void SaucerMovie::animateMovie(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  m_movieTime += since;

  // initialize movie
  switch (m_movieScene)
  {
    case 0:  // initialize movie
    {
      // reset old saucer list
      for (int i = 0; i < m_saucers.length(); i++)
      {
        Saucer* saucer = (Saucer*) m_saucers[i];
        if (saucer->m_sound != -1)
          m_audio->deleteSound(saucer->m_sound);

        delete saucer;
      }
      m_saucers.removeAll();

      m_eyePt = mgPoint3(75, 0, -1000.0);
      m_eyeRotX = 0.0;
      m_eyeRotY = -90;
      m_eyeChanged = true;
      m_movieScene = 1;
      m_movieTime = 0.0;

      if (m_music != -1)
        m_audio->deleteSound(m_music);

      m_music = m_audio->createBackgroundSound(m_musicSound, false);
      m_audio->startSound(m_music);
    }

    case 1:  // watch stars
    {
      if (m_movieTime > 9000)
      {
        for (int i = 0; ; i++)
        {
          if (INIT_SAUCERS[i].z >= 0)
            break;
          Saucer* saucer = new Saucer(m_options);
          saucer->m_origin = INIT_SAUCERS[i];
          saucer->createBuffers();
          m_saucers.add(saucer);
        }
        m_movieScene = 2;
        m_movieTime = 0.0;
      }
      break;
    }

    case 2: // watch saucers go by
    {
      if (m_movieTime > 15000)
      {
        // create bad saucer
        m_badSaucer = new Saucer(m_options);
        m_badSaucer->m_origin.z = -1100;
        m_badSaucer->m_origin.x = 130;
        m_badSaucer->m_origin.y = -20;
        m_badSaucer->m_slow = 1;
        m_saucers.add(m_badSaucer);
        m_movieScene = 3;
        m_movieTime = 0.0;

        m_badSaucer->m_sound = m_audio->createMovingSound(m_saucerSound, true);
        m_audio->setSoundLocation(m_badSaucer->m_sound, 
          m_badSaucer->m_origin.x, m_badSaucer->m_origin.y, m_badSaucer->m_origin.z);
        m_audio->startSound(m_badSaucer->m_sound);
      }
      break;
    }

    case 3: // bad saucer slows
    {
      if (m_badSaucer->m_slow == 100)
      {
        // create first killer
        m_killer1 = new Saucer(m_options);
        m_killer1->m_origin.z = -1105;
        m_killer1->m_origin.x = 135;
        m_killer1->m_origin.y = 10;
        m_killer1->m_slow = 1;
        m_killer1->createBuffers();
        m_saucers.add(m_killer1);

        m_movieScene = 4;
        m_movieTime = 0.0;
        m_audio->pauseSound(m_badSaucer->m_sound);
        m_badSaucer->stopTexture();
      }
      else 
      {
        m_audio->setSoundLocation(m_badSaucer->m_sound, 
          m_badSaucer->m_origin.x, m_badSaucer->m_origin.y, m_badSaucer->m_origin.z);
      }
      break;
    }

    case 4: // wait for killers
    {
      if (m_movieTime > 1500)
      {
        // create second killer
        m_killer2 = new Saucer(m_options);
        m_killer2->m_origin.z = -1095;
        m_killer2->m_origin.x = 115;
        m_killer2->m_origin.y = 10;
        m_killer2->m_slow = 1;
        m_killer2->createBuffers();
        m_saucers.add(m_killer2);

        m_movieScene = 5;
        m_movieTime = 0.0;
      }
      break;
    }

    case 5: // first killer stops
    {
      if (m_killer1->m_slow == 100)
      {
        // start firing
        m_badSaucer->m_burnout = 1;
        DeathRay* ray = new DeathRay(m_options);
        ray->createBuffers();
        ray->aimRay(m_killer1->m_origin, m_badSaucer->m_origin);
        m_rays.add(ray);

        ray->m_sound = m_audio->createMovingSound(m_raySound, true);
        m_audio->setSoundRolloff(ray->m_sound, NOISE_NEAR, NOISE_FAR, 1.0);
        m_audio->setSoundLocation(ray->m_sound, 
            m_killer1->m_origin.x, m_killer1->m_origin.y, m_killer1->m_origin.z);
        m_audio->startSound(ray->m_sound);

        m_movieScene = 6;
        m_movieTime = 0.0;
      }
      break;
    }

    case 6: // second killer stops
    {
      if (m_killer2->m_slow == 100)
      {
        // start firing
        DeathRay* ray = new DeathRay(m_options);
        ray->createBuffers();
        ray->aimRay(m_killer2->m_origin, m_badSaucer->m_origin);
        m_rays.add(ray);

        ray->m_sound = m_audio->createMovingSound(m_raySound, true);
        m_audio->setSoundRolloff(ray->m_sound, NOISE_NEAR, NOISE_FAR, 1.0);
        m_audio->setSoundLocation(ray->m_sound, 
          m_killer2->m_origin.x, m_killer2->m_origin.y, m_killer2->m_origin.z);
        m_audio->startSound(ray->m_sound);

        m_movieScene = 7;
        m_movieTime = 0.0;
      }
      break;
    }

    case 7: // bad saucer fully heated
    {
      if (m_badSaucer->m_burnout == 100)
      {
        for (int i = 0; i < m_rays.length(); i++)
        {
          DeathRay* ray = (DeathRay*) m_rays[i];
          m_audio->deleteSound(ray->m_sound);
          delete ray;
        }
        m_rays.removeAll();
        m_movieScene = 8;
        m_movieTime = 0.0;
      }
      break;
    }

    case 8: // bad saucer destroyed
    {
      if (m_badSaucer->m_destroyed)
      {
        m_killer1->m_slow = -100;
        m_killer2->m_slow = -100;

        m_cameraTrack.removeAll();
        m_cameraTrack.addVertex(mgPoint3(0.0, -90.0, 0), mgPoint3(0.0, -90.0, 0+1));
        m_cameraTrack.addVertex(mgPoint3(21, -18.0, 8), mgPoint3(21, -18.0, 8-1));
        m_trackLen = m_cameraTrack.getLength();

        //  srand(100);  // for 500 saucers
        srand(5656);

        // create a cloud of saucers that aren't hitting one another
        for (int i = 0; i < 200; i++)
        {
          Saucer* saucer = new Saucer(m_options);
          saucer->createBuffers();
          placeSaucer(saucer);
          m_saucers.add(saucer);
        }

        m_movieScene = 9;
        m_movieTime = 0.0;
      }
      break;
    }

    case 9: // watch saucers go by
    {
      if (m_movieTime > 4000)
      {
        m_movieScene = 10;
        m_movieTime = 0.0;
      }
      break;
    }

    case 10: // turn eye towards earth
    {
      double progress = m_movieTime/8000;
      progress = min(1.0, progress);
      mgPoint3 pt;
      m_cameraTrack.splinePt(progress * m_trackLen, pt);
      m_eyeRotX = pt.x;
      m_eyeRotY = pt.y;
      m_eyeChanged = true;
      if (progress >= 1.0)
        m_movieScene = -1;
      break;
    }
  }
}

//--------------------------------------------------------------------
// animate the view
BOOL SaucerMovie::appViewAnimate(
  double now,                       // current time (ms)
  double since)                     // milliseconds since last pass
{
  double frontX, frontY, frontZ;
  m_eyeMatrix.invertPt(0.0, 0.0, 1.0, frontX, frontY, frontZ);
  double topX, topY, topZ;
  m_eyeMatrix.invertPt(0.0, 1.0, 0.0, topX, topY, topZ);

  m_audio->setListenerLocation(m_eyePt.x, m_eyePt.y, m_eyePt.z);
  m_audio->setListenerOrientation(topX, topY, topZ, frontX, frontY, frontZ);

  // if there are saucers, there is animation
  BOOL changed = m_saucers.length() > 0;
  if (m_movieScene >= 0)
  {
    animateMovie(now, since);
    changed = true;
  }

  // animate and move the saucers
  for (int i = m_saucers.length()-1; i >= 0; i--)
  {
    Saucer* saucer = (Saucer*) m_saucers[i];
    saucer->animate(now, since);
    
    mgPoint3 delta(m_eyePt);
    delta.subtract(saucer->m_origin);
    double dist = delta.length();

    if (dist < NOISE_RANGE)
    {
      if (saucer->m_sound == -1)
      {
        saucer->m_sound = m_audio->createMovingSound(m_saucerSound, true);
        m_audio->setSoundRolloff(saucer->m_sound, NOISE_NEAR, NOISE_FAR, 1.0);
        m_audio->setSoundLocation(saucer->m_sound, 
          saucer->m_origin.x, saucer->m_origin.y, saucer->m_origin.z);
        m_audio->startSound(saucer->m_sound);
      }
      else
      {
        // update position
        m_audio->setSoundLocation(saucer->m_sound, 
          saucer->m_origin.x, saucer->m_origin.y, saucer->m_origin.z);
      }
    }
    else
    {
      if (saucer->m_sound != -1)
      {
        m_audio->deleteSound(saucer->m_sound);
        saucer->m_sound = -1;
      }
    }

    // recreate saucers as they get too close to earth
    if (saucer->m_origin.z > -100)
    {
      m_saucers.removeAt(i);
      placeSaucer(saucer);
      m_saucers.add(saucer);
    }
  }

  // animate the death rays
  for (int i = 0; i < m_rays.length(); i++)
  {
    DeathRay* ray = (DeathRay*) m_rays[i];
    ray->animate(now, since);
  }

  // update move/turn 
  if (MovementApp::appViewAnimate(now, since))
    changed = true;

  return changed;
}

//--------------------------------------------------------------------
// render the view
void SaucerMovie::appViewDraw()
{
  // draw the sky
  if (m_sky != NULL)
  {
    mgDisplay->setTransparent(true);
    mgDisplay->setZEnable(false);

    mgDisplay->setEyeMatrix(m_eyeMatrix);
    mgDisplay->setEyePt(mgPoint3(0,0,0));

    mgMatrix4 identity;
    mgDisplay->setModelTransform(identity);

    m_sky->render();

    mgDisplay->setTransparent(false);
    mgDisplay->setZEnable(true);
  }

  mgDisplay->setEyeMatrix(m_eyeMatrix);

  mgDisplay->setEyePt(m_eyePt);

  // draw the planet
  if (m_earthVertexes != NULL)
  {
    mgMatrix4 model;
    mgDisplay->setModelTransform(model);

    mgDisplay->setTransparent(true);
    mgDisplay->setShader("unlitTexture");
    mgDisplay->setMatColor(1, 1, 1);
    mgDisplay->setTexture(m_earthTexture);
    mgDisplay->draw(MG_TRIANGLES, m_earthVertexes);
    mgDisplay->setTransparent(false);
  }

  // draw the saucers
  for (int i = 0; i < m_saucers.length(); i++)
  {
    Saucer* saucer = (Saucer*) m_saucers[i];
    saucer->render();
  }

  // draw the death rays
  for (int i = 0; i < m_rays.length(); i++)
  {
    DeathRay* ray = (DeathRay*) m_rays[i];
    ray->render();
  }
}

//-----------------------------------------------------------------------------
// create buffers, ready to send to display
void SaucerMovie::appCreateBuffers()
{
  m_sky->createBuffers();

  createEarth();

  for (int i = 0; i < m_saucers.length(); i++)
  {
    Saucer* saucer = (Saucer*) m_saucers[i];
    saucer->createBuffers();
  }

  for (int i = 0; i < m_rays.length(); i++)
  {
    DeathRay* ray = (DeathRay*) m_rays[i];
    ray->createBuffers();
  }
}

//-----------------------------------------------------------------------------
// delete any display buffers
void SaucerMovie::appDeleteBuffers()
{
  m_sky->deleteBuffers();

  delete m_earthVertexes;
  m_earthVertexes = NULL;

  for (int i = 0; i < m_saucers.length(); i++)
  {
    Saucer* saucer = (Saucer*) m_saucers[i];
    saucer->deleteBuffers();
  }

  for (int i = 0; i < m_rays.length(); i++)
  {
    DeathRay* ray = (DeathRay*) m_rays[i];
    ray->deleteBuffers();
  }
}

//-----------------------------------------------------------------------------
// key press
BOOL SaucerMovie::moveKeyDown(
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
      if (m_ui != NULL)
        m_help->toggleConsole();
      return true;

    case MG_EVENT_KEY_F5:
      mgDebug("stop");
      return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// key typed
BOOL SaucerMovie::moveKeyChar(
  int keyCode,
  int modifiers)
{
  switch (keyCode)
  {
    case 'M':
    case 'm':
      m_movieScene = 0;
      return true;

    case 'N':
    case 'n':
      m_nebula = !m_nebula;
      m_sky->enableSkyBox(m_nebula);
      m_eyeChanged = true;  // force update
      return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// report status for debug log
void SaucerMovie::appDebugStats(
  mgString& status)
{
  // format program status into this string.  displayed when graphicsTiming option is true.
}
