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
#ifndef MGAUDIO_H
#define MGAUDIO_H

#ifdef WIN32
#pragma comment(lib, "Ogg.lib")
#pragma comment(lib, "Vorbis.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")
#endif

const int MG_AUDIO_MAX_SOUNDS = 256;

class mgAudioSound;

// manage all the sound output for the application.  There should only be
// one instance of this class
class mgAudio
{
public:
  // constructor
  static mgAudio* create();

  // destructor
  virtual ~mgAudio();

  // create a backgound sound (can be stereo or mono)
  virtual int createBackgroundSound(
    const char* fileName,
    BOOL looping) = 0;

  // create a moving sound (must be mono)
  virtual int createMovingSound(
    const char* fileName,
    BOOL looping) = 0;

  // delete a sound
  virtual void deleteSound(
    int sound) = 0;

  // set listener coordinates
  virtual void setListenerLocation(
    double x,
    double y,
    double z) = 0;

  // set listener velocity
  virtual void setListenerVelocity(
    double x,
    double y,
    double z) = 0;

  // set listener orientation
  virtual void setListenerOrientation(
    double upx,
    double upy,
    double upz,
    double atx,
    double aty,
    double atz) = 0;

  // set listener gain
  virtual void setListenerGain(
    double gain) = 0;

  // start sound playing
  virtual void startSound(
    int sound) = 0;

  // pause sound
  virtual void pauseSound(
    int sound) = 0;

  // reset sound to beginning
  virtual void resetSound(
    int sound) = 0;

  // move sound coordinates
  virtual void setSoundLocation(
    int sound,
    double x,
    double y,
    double z) = 0;
    
  // set sound velocity
  virtual void setSoundVelocity(
    int sound,
    double x,
    double y,
    double z) = 0;
    
  // set sound intensity
  virtual void setSoundGain(
    int sound,
    double gain) = 0;  

  // set sound pitch
  virtual void setSoundPitch(
    int sound,
    double pitch) = 0;  

  // set sound gain range
  virtual void setSoundGainRange(
    int sound,
    double minGain,        // default is 0.0
    double maxGain) = 0;       // default is 1.0

  // set sound rolloff
  virtual void setSoundRolloff(
    int sound,
    double minDistance,   // default is 0.0
    double maxDistance,   // default is infinity
    double rolloff) = 0;      // default is 1.0

  // set sound directgion
  virtual void setSoundDirection(
    int sound,
    double x,
    double y,
    double z) = 0;
    
  // set sound cone
  virtual void setSoundCone(
    int sound,
    double innerAngle,    // default is 360 degrees
    double outerAngle,    // default is 360 degrees
    double outerGain) = 0;    // default is 0.0
};

#endif
