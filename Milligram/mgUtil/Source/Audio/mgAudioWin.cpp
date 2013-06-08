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

#ifdef WIN32

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgAudioWin.h"
#include "mgVorbisRead.h"

extern HWND mgWindow;

// background thread
void mgAudioWinThread(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2);

class mgAudioWinSound
{
public:
  mgString m_fileName;
  int m_slot;
  int m_format;
  int m_rate;
  int m_channels;

  BOOL m_looping;
  BOOL m_moving;
  BOOL m_playing;
  BOOL m_loaded;
  BOOL m_delete;
  BOOL m_wroteHigh;
  int m_wroteEnd;   // count of end buffers written

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

  LPDIRECTSOUNDBUFFER m_buffer; 
  LPDIRECTSOUND3DBUFFER m_buffer3D;
  DWORD m_bufferLen;

  // Vorbis read state
  void* m_vorbisState;

  // constructor
  mgAudioWinSound(
    const char* fileName,
    BOOL moving,
    BOOL looping);
  
  // destructor
  virtual ~mgAudioWinSound();

  // fill a buffer.  return bytes read
  virtual DWORD fillBuffer(
    DWORD writePosn,
    DWORD writeLen);

  // load initial buffers
  virtual BOOL initialLoad(
    mgAudioWin* audio);
};

//--------------------------------------------------------------
// sound constructor
mgAudioWinSound::mgAudioWinSound(
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
  m_wroteEnd = 0;

  m_vorbisState = NULL;
}

//--------------------------------------------------------------
// fill a buffer.  return bytes read
DWORD mgAudioWinSound::fillBuffer(
  DWORD writePosn,
  DWORD writeLen)
{
  void* startPtr, *wrapPtr;
  DWORD lockLen, wrapLen;

  HRESULT rc = m_buffer->Lock(writePosn, writeLen,
               &startPtr, &lockLen, &wrapPtr, &wrapLen, 
               0);  // flags
  if (rc == DSERR_BUFFERLOST)
  {
    rc = m_buffer->Restore();
    if (SUCCEEDED(rc))
      rc = m_buffer->Lock(writePosn, writeLen,
               &startPtr, &lockLen, &wrapPtr, &wrapLen, 
               0);  // flags
  }
  if (!SUCCEEDED(rc))
    return 0;

  DWORD posn = 0;
  while (posn < writeLen)
  {
    // read some more source data
    int readLen = writeLen - posn;
    int bytesRead = mgVorbisRead(m_vorbisState, (char*) startPtr + posn, readLen);
    if (bytesRead <= 0)
    {
      // source file ended
      if (m_looping)
      {
        // back to start
        mgVorbisReset(m_vorbisState);
      }
      else break;
    }
    else posn += bytesRead;
  }

  // if ended, clear rest of buffer
  if (posn < writeLen)
    memset((char*) startPtr + posn, 0, writeLen-posn);

  m_buffer->Unlock(startPtr, lockLen, wrapPtr, wrapLen);

  return posn;
}

//--------------------------------------------------------------
// load initial buffers
BOOL mgAudioWinSound::initialLoad(
  mgAudioWin* audio)
{
  m_vorbisState = mgVorbisOpen(m_fileName, m_channels, m_rate);
  if (m_vorbisState == NULL)
    return false;

  // create format
  WAVEFORMATEX format;
  format.cbSize = 0;
  format.nChannels = m_channels;
  format.nSamplesPerSec = m_rate;
  format.wBitsPerSample = 16;
  format.nBlockAlign = format.nChannels * format.wBitsPerSample/8;
  format.wFormatTag = WAVE_FORMAT_PCM;
  format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;

  // create a 2-second buffer
  m_bufferLen = 2*format.nAvgBytesPerSec;

  // Set up DSBUFFERDESC structure. 
  DSBUFFERDESC dsbdesc; 
  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out. 
  dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
  dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 |
                    DSBCAPS_GLOBALFOCUS |
                    DSBCAPS_CTRLPOSITIONNOTIFY |
                    DSBCAPS_CTRLVOLUME |
                    DSBCAPS_LOCSOFTWARE; 

  dsbdesc.dwBufferBytes = m_bufferLen*2;
  dsbdesc.lpwfxFormat = &format;
  dsbdesc.guid3DAlgorithm = DS3DALG_DEFAULT;

  if (m_moving)
    dsbdesc.dwFlags |= DSBCAPS_CTRL3D;

  // Create buffer. 
  HRESULT hr = audio->m_sound->CreateSoundBuffer(&dsbdesc, &m_buffer, NULL); 
  if (hr != DS_OK)
    throw new mgException("CreateSoundBuffer secondary failed");

  // turn off sound so we can set position before play starts
//  m_buffer->SetVolume(DSBVOLUME_MIN);

  if (m_moving)
  {
    hr = m_buffer->QueryInterface(IID_IDirectSound3DBuffer8, 
                                  (LPVOID *)&m_buffer3D);   
    if (hr != DS_OK)
      throw new mgException("QueryInterface 3dBuffer failed");
  }
  else m_buffer3D = NULL;

  // fill the initial buffer 
  DWORD bytesWritten = fillBuffer(0, m_bufferLen*2);
  if (bytesWritten < m_bufferLen*2)
    m_wroteEnd = 1;
  m_wroteHigh = true;

  if (m_moving)
  {
    m_buffer3D->SetMinDistance((D3DVALUE) m_minDistance, DS3D_IMMEDIATE);
    m_buffer3D->SetMaxDistance((D3DVALUE) m_maxDistance, DS3D_IMMEDIATE);
    m_buffer3D->SetVelocity((D3DVALUE) m_velocity.x, (D3DVALUE) m_velocity.y, (D3DVALUE) m_velocity.z, DS3D_IMMEDIATE);
    m_buffer3D->SetPosition((D3DVALUE) m_location.x, (D3DVALUE) m_location.y, (D3DVALUE) m_location.z, DS3D_IMMEDIATE);
  }
  return true;
}

//--------------------------------------------------------------
// sound destructor
mgAudioWinSound::~mgAudioWinSound()
{
  if (m_loaded)
  {
    if (m_buffer3D != NULL)
      m_buffer3D->Release();

    if (m_buffer != NULL)
      m_buffer->Release();
  }

  if (m_vorbisState != NULL)
  {
    mgVorbisClose(m_vorbisState);
    m_vorbisState = NULL;
  }
}

//--------------------------------------------------------------
// constructor
mgAudioWin::mgAudioWin()
{
  // initialize all the sound slots
  for (int i = 0; i < MG_AUDIO_MAX_SOUNDS; i++)
    m_sounds[i] = NULL;
  m_nextSoundId = 0;

  m_lock = mgOSLock::create();

  HRESULT hr = DirectSoundCreate8(NULL, &m_sound, NULL);
  if (hr != DS_OK)
    throw new mgException("DirectSoundCreate failed.");

  hr = m_sound->SetCooperativeLevel(mgWindow, DSSCL_PRIORITY); // DSSCL_NORMAL);
  if (hr != DS_OK)
    throw new mgException("SetCooperativeLevel failed");

  // Set up DSBUFFERDESC structure. 
  DSBUFFERDESC dsbdesc; 
  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out. 
  dsbdesc.dwSize = sizeof(DSBUFFERDESC);
  dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER |
                    DSBCAPS_CTRL3D;
//  dsbdesc.guid3DAlgorithm = DS3DALG_HRTF_FULL;

  hr = m_sound->CreateSoundBuffer(&dsbdesc, &m_primaryBuffer, NULL);
  if (hr != DS_OK)
    throw new mgException("CreateSoundBuffer primary failed");

  WAVEFORMATEX format;
  format.cbSize = 0;
  format.nChannels = 2;
  format.nSamplesPerSec = 44100;
  format.wBitsPerSample = 16;
  format.nBlockAlign = format.nChannels * format.wBitsPerSample/8;
  format.wFormatTag = WAVE_FORMAT_PCM;
  format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;

  hr = m_primaryBuffer->SetFormat(&format); 
  if (hr != DS_OK)
    throw new mgException("SetFormat failed");

  hr = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8,
              (LPVOID *)&m_listener);
  if (hr != DS_OK)
    throw new mgException("QueryInterface for DirectSound3DListener failed");

  hr = m_listener->SetDistanceFactor(0.5f, DS3D_IMMEDIATE);
  if (hr != DS_OK)
    throw new mgException("SetDistanceFactor failed");

  // create the background thread
  m_shutdown = false;
  m_audioThread = mgOSThread::create(1, mgAudioWinThread, 
    mgOSThread::PRIORITY_LOW, this, NULL);
}

//--------------------------------------------------------------
// destructor
mgAudioWin::~mgAudioWin()
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

  delete m_lock;
  m_lock = NULL;

  if (m_listener != NULL)
    m_listener->Release();

  if (m_primaryBuffer != NULL)
    m_primaryBuffer->Release();

  if (m_sound != NULL)
    m_sound->Release();
}

//--------------------------------------------------------------
// background thread
void mgAudioWinThread(
  mgOSThread* thread,
  void* threadArg1,
  void* threadArg2)
{
  mgAudioWin* audio = (mgAudioWin*) threadArg1;

  // while not shutdown
  while (!audio->m_shutdown)
  {
    int slot = 0;
    while (slot < MG_AUDIO_MAX_SOUNDS)
    {
      // find the next sound that is playing
      mgAudioWinSound* sound = NULL;
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
          // start playing
          sound->m_loaded = true;
          sound->m_buffer->Play(0, 0, DSBPLAY_LOOPING);
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
      DWORD playCursor, writeCursor, bytesWritten;
      sound->m_buffer->GetCurrentPosition(&playCursor, &writeCursor);

      // if playing low half of buffer
      if (playCursor < sound->m_bufferLen)
      {
        // make sure high buffer is full
        if (!sound->m_wroteHigh)
        {
          if (sound->m_wroteEnd >= 2)
          {
            sound->m_buffer->Stop();
            sound->m_playing = false;
          }
          else
          {
            bytesWritten = sound->fillBuffer(sound->m_bufferLen, sound->m_bufferLen);
            if (bytesWritten < sound->m_bufferLen)
              sound->m_wroteEnd++;
            sound->m_wroteHigh = true;
          }
        }
      }
      else
      {
        // playing high half of buffer.
        // make sure low buffer is full
        if (sound->m_wroteHigh)
        {
          if (sound->m_wroteEnd >= 2)
          {
            sound->m_buffer->Stop();
            sound->m_playing = false;
          }
          else
          {
            bytesWritten = sound->fillBuffer(0, sound->m_bufferLen);
            if (bytesWritten < sound->m_bufferLen)
              sound->m_wroteEnd++;
            sound->m_wroteHigh = false;
          }
        }
      }
    }

    Sleep(100);
  }
}

//--------------------------------------------------------------
// get a sound by slot
mgAudioWinSound* mgAudioWin::getSound(
  int slot)
{
  if (slot < 0 || slot >= MG_AUDIO_MAX_SOUNDS)
    return NULL;
  m_lock->lock();
  mgAudioWinSound* sound = (mgAudioWinSound*) m_sounds[slot];
  m_lock->unlock();
  return sound;
}

//--------------------------------------------------------------
// find slot for new sound
int mgAudioWin::newSound(
  mgAudioWinSound* sound)
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
void mgAudioWin::setListenerLocation(
  double x,
  double y,
  double z)
{
  m_listener->SetPosition((D3DVALUE) x, (D3DVALUE) y, (D3DVALUE) z, DS3D_IMMEDIATE);
}

//--------------------------------------------------------------
// set listener velocity
void mgAudioWin::setListenerVelocity(
  double x,
  double y,
  double z)
{
  m_listener->SetVelocity((D3DVALUE) x, (D3DVALUE) y, (D3DVALUE) z, DS3D_IMMEDIATE);
}

//--------------------------------------------------------------
// set listener orientation
void mgAudioWin::setListenerOrientation(
  double upx,
  double upy,
  double upz,
  double atx,
  double aty,
  double atz)
{
  m_listener->SetOrientation((D3DVALUE) atx, (D3DVALUE) aty, (D3DVALUE) atz, 
                             (D3DVALUE) upx, (D3DVALUE) upy, (D3DVALUE) upz, 
                             DS3D_IMMEDIATE);
}

//--------------------------------------------------------------
// set listener gain
void mgAudioWin::setListenerGain(
  double gain)
{
}

//--------------------------------------------------------------
// create a backgound sound (can be stereo or mono)
int mgAudioWin::createBackgroundSound(
  const char* fileName,
  BOOL looping)
{
  int slot = newSound(new mgAudioWinSound(fileName, false, looping));
  return slot;
}

//--------------------------------------------------------------
// create a moving sound (must be mono)
int mgAudioWin::createMovingSound(
  const char* fileName,
  BOOL looping)
{
  int slot = newSound(new mgAudioWinSound(fileName, true, looping));
  return slot;
}


//--------------------------------------------------------------
// delete a sound
void mgAudioWin::deleteSound(
  int slot)
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  sound->m_delete = true;
  sound->m_playing = false;

  // stop playback
  if (sound->m_loaded)
  {
    sound->m_buffer->Stop();
  }
}

//--------------------------------------------------------------
// start sound playing
void mgAudioWin::startSound(
  int slot)
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  if (sound->m_loaded)
  {
    sound->m_buffer->Play(0, 0, DSBPLAY_LOOPING);
  }
  sound->m_playing = true;
}

//--------------------------------------------------------------
// pause sound
void mgAudioWin::pauseSound(
  int slot)
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  if (sound->m_loaded)
  {
    sound->m_buffer->Stop();
  }
  sound->m_playing = false;
}

//--------------------------------------------------------------
// reset sound to beginning
void mgAudioWin::resetSound(
  int slot)
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  sound->m_playing = false;
  if (sound->m_loaded)
  {
    // =-= is this safe?  worker thread is filling bufers from vorbisState
    sound->m_buffer->Stop();
    sound->m_buffer->SetCurrentPosition(0);
    mgVorbisReset(sound->m_vorbisState);
  }
}

//--------------------------------------------------------------
// move sound coordinates
void mgAudioWin::setSoundLocation(
  int slot,
  double x,
  double y,
  double z)
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_location.x = x;
  sound->m_location.y = y;
  sound->m_location.z = z;

  if (sound->m_loaded)
  {
    sound->m_buffer3D->SetPosition((D3DVALUE) x, (D3DVALUE) y, (D3DVALUE) z, DS3D_IMMEDIATE);
  }
}

//--------------------------------------------------------------
// move sound velocity
void mgAudioWin::setSoundVelocity(
  int slot,
  double x,
  double y,
  double z)
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_velocity.x = x;
  sound->m_velocity.y = y;
  sound->m_velocity.z = z;

  if (sound->m_loaded)
  {
    sound->m_buffer3D->SetVelocity((D3DVALUE) x, (D3DVALUE) y, (D3DVALUE) z, DS3D_IMMEDIATE);
  }
}

//--------------------------------------------------------------
// change sound intensity
void mgAudioWin::setSoundGain(
  int slot,
  double gain)   
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_gain = gain;
  if (sound->m_loaded)
  {
  }
}

//--------------------------------------------------------------
// change sound pitch
void mgAudioWin::setSoundPitch(
  int slot,
  double pitch)   
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL || pitch == 0.0)
    return;

  sound->m_pitch = pitch;
  if (sound->m_loaded)
  {
  }
}

//--------------------------------------------------------------
// set sound gain range
void mgAudioWin::setSoundGainRange(
  int slot,
  double minGain,
  double maxGain)
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_minGain = minGain;
  sound->m_maxGain = maxGain;

  if (sound->m_loaded)
  {
  }
}

//--------------------------------------------------------------
// set sound rolloff
void mgAudioWin::setSoundRolloff(
  int slot,
  double minDistance,   // default is 0.0
  double maxDistance,   // default is infinity
  double rolloff)       // default is 1.0
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;

  sound->m_minDistance = minDistance;
  sound->m_maxDistance = maxDistance;
  sound->m_rolloff = rolloff;

  if (sound->m_loaded)
  {
    sound->m_buffer3D->SetMinDistance((D3DVALUE) minDistance, DS3D_IMMEDIATE);
    sound->m_buffer3D->SetMaxDistance((D3DVALUE) maxDistance, DS3D_IMMEDIATE);
  }
}

//--------------------------------------------------------------
// set sound direction
void mgAudioWin::setSoundDirection(
  int slot,
  double x,
  double y,
  double z)
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_direction.x = x;
  sound->m_direction.y = y;
  sound->m_direction.z = z;

  if (sound->m_loaded)
  {
  }
}

//--------------------------------------------------------------
// set sound cone
void mgAudioWin::setSoundCone(
  int slot,
  double innerAngle,    // default is 360 degrees
  double outerAngle,    // default is 360 degrees
  double outerGain)     // default is 0.0
{
  mgAudioWinSound* sound = getSound(slot);
  if (sound == NULL)
    return;
  sound->m_innerAngle = innerAngle;
  sound->m_outerAngle = outerAngle;
  sound->m_outerGain = outerGain;

  if (sound->m_loaded)
  {
  }
}

#endif
