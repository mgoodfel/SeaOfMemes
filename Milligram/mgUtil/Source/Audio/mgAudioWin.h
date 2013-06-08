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
#if !defined(MGAUDIOWIN_H) && defined(WIN32)
#define MGAUDIOWIN_H

#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

#include "Audio/mgAudio.h"

class mgAudioWinSound;

class mgAudioWin : public mgAudio
{
public:
  mgAudioWinSound* m_sounds[MG_AUDIO_MAX_SOUNDS];
  int m_nextSoundId;

  LPDIRECTSOUND8 m_sound; 
  LPDIRECTSOUNDBUFFER m_primaryBuffer;
  LPDIRECTSOUND3DLISTENER8 m_listener;

  mgOSLock* m_lock;

  // buffer for sound data
  char* m_pcmData;
  int m_dataLen;

  BOOL m_shutdown;
  mgOSThread* m_audioThread;   

  // constructor
  mgAudioWin();

  // destructor
  virtual ~mgAudioWin();

  // create a backgound sound (can be stereo or mono)
  virtual int createBackgroundSound(
    const char* fileName,
    BOOL looping);

  // create a moving sound (must be mono)
  virtual int createMovingSound(
    const char* fileName,
    BOOL looping);

  // delete a sound
  virtual void deleteSound(
    int sound);

  // set listener coordinates
  virtual void setListenerLocation(
    double x,
    double y,
    double z);

  // set listener velocity
  virtual void setListenerVelocity(
    double x,
    double y,
    double z);

  // set listener orientation
  virtual void setListenerOrientation(
    double upx,
    double upy,
    double upz,
    double atx,
    double aty,
    double atz);

  // set listener gain
  virtual void setListenerGain(
    double gain);

  // start sound playing
  virtual void startSound(
    int sound);

  // pause sound
  virtual void pauseSound(
    int sound);

  // reset sound to beginning
  virtual void resetSound(
    int sound);

  // move sound coordinates
  virtual void setSoundLocation(
    int sound,
    double x,
    double y,
    double z);
    
  // set sound velocity
  virtual void setSoundVelocity(
    int sound,
    double x,
    double y,
    double z);
    
  // set sound intensity
  virtual void setSoundGain(
    int sound,
    double gain);  

  // set sound pitch
  virtual void setSoundPitch(
    int sound,
    double pitch);  

  // set sound gain range
  virtual void setSoundGainRange(
    int sound,
    double minGain,        // default is 0.0
    double maxGain);       // default is 1.0

  // set sound rolloff
  virtual void setSoundRolloff(
    int sound,
    double minDistance,   // default is 0.0
    double maxDistance,   // default is infinity
    double rolloff);      // default is 1.0

  // set sound directgion
  virtual void setSoundDirection(
    int sound,
    double x,
    double y,
    double z);
    
  // set sound cone
  virtual void setSoundCone(
    int sound,
    double innerAngle,    // default is 360 degrees
    double outerAngle,    // default is 360 degrees
    double outerGain);    // default is 0.0

  // get a sound by slot
  mgAudioWinSound* getSound(
    int slot);

  // find slot for new sound
  int newSound(
    mgAudioWinSound* sound);

};

#endif
