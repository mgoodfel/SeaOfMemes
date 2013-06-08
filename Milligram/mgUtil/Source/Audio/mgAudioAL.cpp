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

// This could be used for Win32 as well, if we built the OpenAL library under Win.
// isntead, see mgAudioWin, which uses DirectSound.

#if defined(__APPLE__) || defined(__unix__)

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#ifdef __APPLE__
#include <OpenAL/alc.h>
#include <OpenAL/al.h>
#include <OpenAL/MacOSX_OALExtensions.h>
#endif

#ifdef __unix__
#include "AL/alc.h"
#include "AL/al.h"
#include "AL/alext.h"
#endif

#include "mgVorbisRead.h"

#include "mgAudioAL.h"

// background thread
void mgAudioALThread(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2);

const int SOUND_BUFFERS = 3;

class mgAudioALSound
{
public:
  mgString m_fileName;
  int m_slot;
  ALuint m_format;
  ALuint m_rate;
  ALuint m_channels;

  BOOL m_looping;
  BOOL m_moving;
  BOOL m_playing;
  BOOL m_loaded;
  BOOL m_delete;

  mgPoint3 m_location;
  mgPoint3 m_velocity;
  mgPoint3 m_direction;
  double m_gain;
  double m_pitch;
  double m_minGain;
  double m_maxGain;
  double m_minDistance;
  double m_maxDistance;
  double m_rolloff;
  double m_innerAngle;
  double m_outerAngle;
  double m_outerGain;

  // open AL state
  ALuint m_source;
  ALuint m_buffers[SOUND_BUFFERS];

  // Vorbis read state
  void* m_vorbisState;

  // constructor
  mgAudioALSound(
    const char* fileName,
    BOOL moving,
    BOOL looping);
  
  // destructor
  virtual ~mgAudioALSound();

  // fill a buffer.  return bytes read
  virtual int fillBuffer(
    char* pcmData,
    int dataLen);

  // load initial buffers
  virtual BOOL initialLoad(
    mgAudioAL* audio);
};

//--------------------------------------------------------------
// sound constructor
mgAudioALSound::mgAudioALSound(
  const char* fileName,
  BOOL moving,
  BOOL looping)
{
  m_fileName = fileName;

  m_location = mgPoint3(0,0,0);
  m_velocity = mgPoint3(0,0,0);
  m_direction = mgPoint3(0,0,0);
  m_gain = 1.0;
  m_pitch = 1.0;
  m_minGain = 0.0;
  m_maxGain = 1.0;
  m_minDistance = 0.0;
  m_maxDistance = INT_MAX;
  m_rolloff = 1.0;
  m_innerAngle = 360.0;
  m_outerAngle = 360.0;
  m_outerGain = 0.0;

  m_moving = moving;
  m_looping = looping;
  m_playing = false;
  m_loaded = false;
  m_delete = false;

  m_vorbisState = NULL;
  m_source = -1;
  for (int i = 0; i < SOUND_BUFFERS; i++)
    m_buffers[i] = -1;
}

//--------------------------------------------------------------
// fill a buffer.  return bytes read
int mgAudioALSound::fillBuffer(
  char* pcmData,
  int dataLen)
{
  int posn = 0;
  while (posn < dataLen)
  {
    // read some more source data
    int readLen = dataLen - posn;
    int bytesRead = mgVorbisRead(m_vorbisState, pcmData + posn, readLen);
    if (bytesRead <= 0)
    {
      // source file ended
      if (m_looping)
      {
        // back to start
        mgVorbisReset(m_vorbisState);
      }
      else return posn;
    }
    else posn += bytesRead;
  }
  return posn;
}

//--------------------------------------------------------------
// load initial buffers
BOOL mgAudioALSound::initialLoad(
  mgAudioAL* audio)
{
  int channels, samplesPerSec;
  m_vorbisState = mgVorbisOpen(m_fileName, channels, samplesPerSec);
  if (m_vorbisState == NULL)
    return false;

  m_channels = channels;
  m_rate = samplesPerSec;
  m_format = (m_channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

  // create sound buffers and load them
  alGenBuffers(SOUND_BUFFERS, m_buffers);
  alGenSources(1, &m_source);

  for (int i = 0; i < SOUND_BUFFERS; i++)
  {
    // Make sure we get some data to give to the buffer
    int bytesRead = fillBuffer(audio->m_pcmData, audio->m_dataLen);
    if (bytesRead <= 0)
      break;

    // load OpenAL buffer, and queue it for source
    alBufferData(m_buffers[i], m_format, audio->m_pcmData, bytesRead, m_rate);
    alSourceQueueBuffers(m_source, 1, &m_buffers[i]);
  }

  alSource3f(m_source, AL_POSITION, (float) m_location.x, (float) m_location.y, (float) m_location.z);
  alSource3f(m_source, AL_VELOCITY, (float) m_velocity.x, (float) m_velocity.y, (float) m_velocity.z);
  alSource3f(m_source, AL_DIRECTION, (float) m_direction.x, (float) m_direction.y, (float) m_direction.z);

  alSourcef(m_source, AL_CONE_INNER_ANGLE, (float) m_innerAngle);
  alSourcef(m_source, AL_CONE_OUTER_ANGLE, (float) m_outerAngle);
  alSourcef(m_source, AL_CONE_OUTER_GAIN, (float) m_outerGain);

  alSourcef(m_source, AL_REFERENCE_DISTANCE, (float) m_minDistance);
  alSourcef(m_source, AL_MAX_DISTANCE, (float) m_maxDistance);
  alSourcef(m_source, AL_ROLLOFF_FACTOR, (float) m_rolloff);

  alSourcef(m_source, AL_MIN_GAIN, (float) m_minGain);
  alSourcef(m_source, AL_MAX_GAIN, (float) m_maxGain);
  alSourcef(m_source, AL_GAIN, (float) m_gain);

  alSourcef(m_source, AL_PITCH, (float) m_pitch);

  return true;
}

//--------------------------------------------------------------
// sound destructor
mgAudioALSound::~mgAudioALSound()
{
  if (m_loaded)
  {
    alSourceStop(m_source);

    // delete buffers and sources
    alDeleteSources(1, &m_source);
    alDeleteBuffers(SOUND_BUFFERS, m_buffers); 
  }

  if (m_vorbisState != NULL)
  {
    mgVorbisClose(m_vorbisState);
    m_vorbisState = NULL;
  }
}

//--------------------------------------------------------------
// constructor
mgAudioAL::mgAudioAL()
{
  // initialize all the sound slots
  for (int i = 0; i < MG_AUDIO_MAX_SOUNDS; i++)
    m_sounds[i] = NULL;
  m_nextSoundId = 0;

  ALCdevice* device = alcOpenDevice(NULL);
  m_ALDevice = device;

  ALCint major, minor;

  mgDebug("Info for device \"%s\"", alcGetString(device, ALC_DEVICE_SPECIFIER));

  alcGetIntegerv(device, ALC_MAJOR_VERSION, 1, &major);
  alcGetIntegerv(device, ALC_MINOR_VERSION, 1, &minor);
  mgDebug("ALC version: %d.%d", major, minor);
    
  m_ALContext = alcCreateContext(device, NULL);
  if (m_ALContext == NULL || alcMakeContextCurrent((ALCcontext*) m_ALContext) == ALC_FALSE)
    throw new mgException("Failed to set a AL context.");

  mgDebug("OpenAL vendor string: %s", alGetString(AL_VENDOR));
  mgDebug("OpenAL renderer string: %s", alGetString(AL_RENDERER));
  mgDebug("OpenAL version string: %s", alGetString(AL_VERSION));

  // create the buffer for loaded audio -- max samples * 2 seconds * stereo * 2-bytes per sample
  m_dataLen = 44100*2*2*2;
  m_pcmData = new char[m_dataLen];

  m_lock = mgOSLock::create();

  // create the background thread
  m_shutdown = false;
  m_audioThread = mgOSThread::create(1, mgAudioALThread, 
    mgOSThread::PRIORITY_LOW, this, NULL);
}

//--------------------------------------------------------------
// destructor
mgAudioAL::~mgAudioAL()
{
  // shut down the background thread
  m_shutdown = true;
  m_audioThread->waitForEnd(2000.0);  // wait for end

  delete m_audioThread;
  m_audioThread = NULL;

  // delete any remaining sounds
  for (int i = 0; i < MG_AUDIO_MAX_SOUNDS; i++)
  {
    if (m_sounds[i] != NULL)
    {
      delete m_sounds[i];
      m_sounds[i] = NULL;
    }
  }

  delete m_pcmData;
  m_pcmData = NULL;

  ALCdevice* device = (ALCdevice*) m_ALDevice;
  if (device == NULL)
    return;  // nothing to do
  ALCcontext* context = (ALCcontext*) m_ALContext;

  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);

  delete m_lock;
  m_lock = NULL;
}

//--------------------------------------------------------------
// background thread
void mgAudioALThread(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2)
{
  mgAudioAL* audio = (mgAudioAL*) threadArg1;

  // while not shutdown
  while (!audio->m_shutdown)
  {
    int slot = 0;
    while (slot < MG_AUDIO_MAX_SOUNDS)
    {
      // find the next sound that is playing
      mgAudioALSound* sound = NULL;
      audio->m_lock->lock();
      while (slot < MG_AUDIO_MAX_SOUNDS)
      {
        sound = audio->m_sounds[slot++];

        // ignore empty slot
        if (sound == NULL)
          continue;

        // delete the sound if marked
        if (sound->m_delete)
        {
  //        mgDebug("delete sound %s", (const char*) sound->m_fileName);
          delete sound;
          audio->m_sounds[slot-1] = NULL;
          continue;
        }

        // if playing, we have a sound to check
        if (sound->m_playing)
          break;
      }
      audio->m_lock->unlock();
      if (slot >= MG_AUDIO_MAX_SOUNDS || audio->m_shutdown)
        break;

      // if not loaded, load the sound
      if (!sound->m_loaded)
      {
        if (sound->initialLoad(audio))
        {                       
//          mgDebug("load sound %s", (const char*) sound->m_fileName);
          // start playing
          sound->m_loaded = true;
          alSourcePlay(sound->m_source);        
        }
        else
        {
          // can't load.  delete the sound
          mgDebug("Can't load sound %s", (const char*) sound->m_fileName);
          sound->m_delete = true;
          continue;
        }
      }

      // check to see if sounds needs more data
      ALint processed = 0;
      alGetSourcei(sound->m_source, AL_BUFFERS_PROCESSED, &processed);
      if (processed > 0)
      {
        ALuint bufferId = 0;
        alSourceUnqueueBuffers(sound->m_source, 1, &bufferId);

        // read some more source data
        int bytesRead = sound->fillBuffer(audio->m_pcmData, audio->m_dataLen);
        if (bytesRead <= 0)
          break;

//        mgDebug("queue %d bytes for %s", bytesRead, (const char*) sound->m_fileName);

        // load a buffer and queue it
        alBufferData(bufferId, sound->m_format, audio->m_pcmData, bytesRead, sound->m_rate);
        alSourceQueueBuffers(sound->m_source, 1, &bufferId);

        if (alGetError() != AL_NO_ERROR)
        {
          mgDebug("AL error");
          sound->m_playing = false;
        }
      }
    }
#ifdef WIN32    
    Sleep(100);
#endif
#if defined(__unix__) || defined(__APPLE__)
    struct timespec req, rem;
    req.tv_sec = 0;
    req.tv_nsec = 100*1000*1000;
    nanosleep(&req, &rem);
#endif
  }
}

//--------------------------------------------------------------
// get a sound by slot
mgAudioALSound* mgAudioAL::getSound(
  int slot)
{
  if (slot < 0 || slot >= MG_AUDIO_MAX_SOUNDS)
    return NULL;
  m_lock->lock();
  mgAudioALSound* sound = (mgAudioALSound*) m_sounds[slot];
  m_lock->unlock();
  return sound;
}

//--------------------------------------------------------------
// find slot for new sound
int mgAudioAL::newSound(
  mgAudioALSound* sound)
{
  m_lock->lock();
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
  m_lock->unlock();

  return slot;
}

//--------------------------------------------------------------
// set listener coordinates
void mgAudioAL::setListenerLocation(
  double x,
  double y,
  double z)
{
  float point[3];
  point[0] = (float) x;
  point[1] = (float) y;
  point[2] = (float) z;
  alListenerfv(AL_POSITION, point);
}

//--------------------------------------------------------------
// set listener velocity
void mgAudioAL::setListenerVelocity(
  double x,
  double y,
  double z)
{
  float point[3];
  point[0] = (float) x;
  point[1] = (float) y;
  point[2] = (float) z;
  alListenerfv(AL_VELOCITY, point);
}

//--------------------------------------------------------------
// set listener orientation
void mgAudioAL::setListenerOrientation(
  double upx,
  double upy,
  double upz,
  double atx,
  double aty,
  double atz)
{
  float points[6];
  points[0] = (float) atx;
  points[1] = (float) aty;
  points[2] = (float) atz;
  points[3] = (float) upx;
  points[4] = (float) upy;
  points[5] = (float) upz;
  alListenerfv(AL_ORIENTATION, points);
}

//--------------------------------------------------------------
// set listener gain
void mgAudioAL::setListenerGain(
  double gain)
{
  alListenerf(AL_GAIN, (float) gain);
}

//--------------------------------------------------------------
// create a backgound sound (can be stereo or mono)
int mgAudioAL::createBackgroundSound(
  const char* fileName,
  BOOL looping)
{
  int slot = newSound(new mgAudioALSound(fileName, false, looping));
  return slot;
}

//--------------------------------------------------------------
// create a moving sound (must be mono)
int mgAudioAL::createMovingSound(
  const char* fileName,
  BOOL looping)
{
  int slot = newSound(new mgAudioALSound(fileName, true, looping));
  return slot;
}

//--------------------------------------------------------------
// delete a sound
void mgAudioAL::deleteSound(
  int slot)
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  sound->m_delete = true;
  sound->m_playing = false;

  // stop playback
  if (sound->m_loaded)
    alSourceStop(sound->m_source);
}

//--------------------------------------------------------------
// start sound playing
void mgAudioAL::startSound(
  int slot)
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  if (sound->m_loaded)
    alSourcePlay(sound->m_source);
  sound->m_playing = true;
}

//--------------------------------------------------------------
// pause sound
void mgAudioAL::pauseSound(
  int slot)
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  if (sound->m_loaded)
    alSourcePause(sound->m_source);
  sound->m_playing = false;
}

//--------------------------------------------------------------
// reset sound to beginning
void mgAudioAL::resetSound(
  int slot)
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  sound->m_playing = false;
  if (sound->m_loaded)
  {
    alSourceStop(sound->m_source);
    alSourceRewind(sound->m_source);
    mgVorbisReset(sound->m_vorbisState);
  }
}

//--------------------------------------------------------------
// move sound coordinates
void mgAudioAL::setSoundLocation(
  int slot,
  double x,
  double y,
  double z)
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_location.x = x;
  sound->m_location.y = y;
  sound->m_location.z = z;

  if (sound->m_loaded)
    alSource3f(sound->m_source, AL_POSITION, (float) x, (float) y, (float) z);
}

//--------------------------------------------------------------
// move sound velocity
void mgAudioAL::setSoundVelocity(
  int slot,
  double x,
  double y,
  double z)
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_velocity.x = x;
  sound->m_velocity.y = y;
  sound->m_velocity.z = z;

  if (sound->m_loaded)
    alSource3f(sound->m_source, AL_VELOCITY, (float) x, (float) y, (float) z);
}

//--------------------------------------------------------------
// change sound intensity
void mgAudioAL::setSoundGain(
  int slot,
  double gain)   
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_gain = gain;
  if (sound->m_loaded)
    alSourcef(sound->m_source, AL_GAIN, (float) gain);
}

//--------------------------------------------------------------
// change sound pitch
void mgAudioAL::setSoundPitch(
  int slot,
  double pitch)   
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL || pitch == 0.0)
    return;

  sound->m_pitch = pitch;
  if (sound->m_loaded)
    alSourcef(sound->m_source, AL_PITCH, (float) pitch);
}

//--------------------------------------------------------------
// set sound gain range
void mgAudioAL::setSoundGainRange(
  int slot,
  double minGain,
  double maxGain)
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_minGain = minGain;
  sound->m_maxGain = maxGain;

  if (sound->m_loaded)
  {
    alSourcef(sound->m_source, AL_MIN_GAIN, (float) minGain);
    alSourcef(sound->m_source, AL_MAX_GAIN, (float) maxGain);
  }
}

//--------------------------------------------------------------
// set sound rolloff
void mgAudioAL::setSoundRolloff(
  int slot,
  double minDistance,   // default is 0.0
  double maxDistance,   // default is infinity
  double rolloff)       // default is 1.0
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  sound->m_minDistance = minDistance;
  sound->m_maxDistance = maxDistance;
  sound->m_rolloff = rolloff;

  if (sound->m_loaded)
  {
    alSourcef(sound->m_source, AL_REFERENCE_DISTANCE, (float) minDistance);
    alSourcef(sound->m_source, AL_MAX_DISTANCE, (float) maxDistance);
    alSourcef(sound->m_source, AL_ROLLOFF_FACTOR, (float) rolloff);
  }
}

//--------------------------------------------------------------
// set sound direction
void mgAudioAL::setSoundDirection(
  int slot,
  double x,
  double y,
  double z)
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_direction.x = x;
  sound->m_direction.y = y;
  sound->m_direction.z = z;

  if (sound->m_loaded)
    alSource3f(sound->m_source, AL_DIRECTION, (float) x, (float) y, (float) z);
}

//--------------------------------------------------------------
// set sound cone
void mgAudioAL::setSoundCone(
  int slot,
  double innerAngle,    // default is 360 degrees
  double outerAngle,    // default is 360 degrees
  double outerGain)     // default is 0.0
{
  mgAudioALSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_innerAngle = innerAngle;
  sound->m_outerAngle = outerAngle;
  sound->m_outerGain = outerGain;

  if (sound->m_loaded)
  {
    alSourcef(sound->m_source, AL_CONE_INNER_ANGLE, (float) innerAngle);
    alSourcef(sound->m_source, AL_CONE_OUTER_ANGLE, (float) outerAngle);
    alSourcef(sound->m_source, AL_CONE_OUTER_GAIN, (float) outerGain);
  }
}

#endif
