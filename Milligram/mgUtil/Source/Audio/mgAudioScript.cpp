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

#ifdef EMSCRIPTEN

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgAudioScript.h"

class mgAudioScriptSound
{
public:
  mgString m_fileName;
  BOOL m_looping;
  BOOL m_moving;

  // constructor
  mgAudioScriptSound(
    const char* fileName,
    BOOL moving,
    BOOL looping);
  
  // destructor
  virtual ~mgAudioScriptSound();
};

//--------------------------------------------------------------
// sound constructor
mgAudioScriptSound::mgAudioScriptSound(
  const char* fileName,
  BOOL moving,
  BOOL looping)
{
  m_fileName = fileName;
  m_moving = moving;
  m_looping = looping;
}

//--------------------------------------------------------------
// sound destructor
mgAudioScriptSound::~mgAudioScriptSound()
{
}

//--------------------------------------------------------------
// constructor
mgAudioScript::mgAudioScript()
{
  // initialize all the sound slots
  for (int i = 0; i < MG_AUDIO_MAX_SOUNDS; i++)
    m_sounds[i] = NULL;
  m_nextSoundId = 0;
}

//--------------------------------------------------------------
// destructor
mgAudioScript::~mgAudioScript()
{
  // delete any remaining sounds
  for (int i = 0; i < MG_AUDIO_MAX_SOUNDS; i++)
  {
    if (m_sounds[i] != NULL)
    {
      delete m_sounds[i];
      m_sounds[i] = NULL;
    }
  }
}

//--------------------------------------------------------------
// get a sound by slot
mgAudioScriptSound* mgAudioScript::getSound(
  int slot)
{
  if (slot < 0 || slot >= MG_AUDIO_MAX_SOUNDS)
    return NULL;
  mgAudioScriptSound* sound = (mgAudioScriptSound*) m_sounds[slot];
  return sound;
}

//--------------------------------------------------------------
// find slot for new sound
int mgAudioScript::newSound(
  mgAudioScriptSound* sound)
{
  int slot = m_nextSoundId;
  while (true)
  {
    if (m_sounds[slot] == NULL)
      break;
    slot = (slot+1)%MG_AUDIO_MAX_SOUNDS;
    // if we've used all slots, complain
    if (slot == m_nextSoundId)  
      throw new mgException("All %d sound slots are in use", MG_AUDIO_MAX_SOUNDS);
  }

  // assign sound to slot
  m_sounds[slot] = sound;

  m_nextSoundId = (slot+1)%MG_AUDIO_MAX_SOUNDS;

  return slot;
}

//--------------------------------------------------------------
// set listener coordinates
void mgAudioScript::setListenerLocation(
  double x,
  double y,
  double z)
{
}

//--------------------------------------------------------------
// set listener velocity
void mgAudioScript::setListenerVelocity(
  double x,
  double y,
  double z)
{
}

//--------------------------------------------------------------
// set listener orientation
void mgAudioScript::setListenerOrientation(
  double upx,
  double upy,
  double upz,
  double atx,
  double aty,
  double atz)
{
}

//--------------------------------------------------------------
// set listener gain
void mgAudioScript::setListenerGain(
  double gain)
{
}

//--------------------------------------------------------------
// create a backgound sound (can be stereo or mono)
int mgAudioScript::createBackgroundSound(
  const char* fileName,
  BOOL looping)
{
  int slot = newSound(new mgAudioScriptSound(fileName, false, looping));
  return slot;
}

//--------------------------------------------------------------
// create a moving sound (must be mono)
int mgAudioScript::createMovingSound(
  const char* fileName,
  BOOL looping)
{
  int slot = newSound(new mgAudioScriptSound(fileName, true, looping));
  return slot;
}


//--------------------------------------------------------------
// delete a sound
void mgAudioScript::deleteSound(
  int slot)
{
  mgAudioScriptSound* sound = getSound(slot);
  if (sound != NULL)
  {
    delete sound;
    m_sounds[slot] = NULL;
  }
}

//--------------------------------------------------------------
// start sound playing
void mgAudioScript::startSound(
  int slot)
{
}

//--------------------------------------------------------------
// pause sound
void mgAudioScript::pauseSound(
  int slot)
{
}

//--------------------------------------------------------------
// reset sound to beginning
void mgAudioScript::resetSound(
  int slot)
{
}

//--------------------------------------------------------------
// move sound coordinates
void mgAudioScript::setSoundLocation(
  int slot,
  double x,
  double y,
  double z)
{
}

//--------------------------------------------------------------
// move sound velocity
void mgAudioScript::setSoundVelocity(
  int slot,
  double x,
  double y,
  double z)
{
}

//--------------------------------------------------------------
// change sound intensity
void mgAudioScript::setSoundGain(
  int slot,
  double gain)   
{
}

//--------------------------------------------------------------
// change sound pitch
void mgAudioScript::setSoundPitch(
  int slot,
  double pitch)   
{
}

//--------------------------------------------------------------
// set sound gain range
void mgAudioScript::setSoundGainRange(
  int slot,
  double minGain,
  double maxGain)
{
}

//--------------------------------------------------------------
// set sound rolloff
void mgAudioScript::setSoundRolloff(
  int slot,
  double minDistance,   // default is 0.0
  double maxDistance,   // default is infinity
  double rolloff)       // default is 1.0
{
}

//--------------------------------------------------------------
// set sound direction
void mgAudioScript::setSoundDirection(
  int slot,
  double x,
  double y,
  double z)
{
}

//--------------------------------------------------------------
// set sound cone
void mgAudioScript::setSoundCone(
  int slot,
  double innerAngle,    // default is 360 degrees
  double outerAngle,    // default is 360 degrees
  double outerGain)     // default is 0.0
{
}

#endif
