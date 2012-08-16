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

#include "Audio/mgVorbisRead.h"

#include "Ogg/Include/ogg.h"
#include "Vorbis/Include/codec.h"

const int VORBIS_BUFFER_SIZE = 4096;

typedef struct _mgVorbisState
{
  FILE* inFile;

  ogg_sync_state   sync; // sync and verify incoming physical bitstream 
  ogg_stream_state stream; // take physical pages, weld into a logical stream of packets 
  ogg_page         page; // one Ogg bitstream page. Vorbis packets are inside 
  ogg_packet       packet; // one raw packet of data for decode 

  vorbis_info      info; // struct that stores all the static vorbis bitstream settings 
  vorbis_comment   comment; // struct that stores all the bitstream user comments 
  vorbis_dsp_state decode; // central working state for the packet->PCM decoder 
  vorbis_block     block; // local working space for packet->PCM decode 

  ogg_int16_t convbuffer[VORBIS_BUFFER_SIZE];
} mgVorbisState;

// forward dcls

// read a packet
void mgVorbisReadPacket(mgVorbisState* state);

// read the vorbis file header
void mgVorbisReadHeader(mgVorbisState* state);

// convert samples to output PCM format
void mgVorbisConvert(mgVorbisState* state, float** pcm, int samples, short* result);

//--------------------------------------------------------------
// open the vorbis file and return state.  
void* mgVorbisOpen(
  const char* fileName,
  int& channels,
  int& samplesPerSec)
{
  mgVorbisState* state = new mgVorbisState;

  state->inFile = fopen(fileName, "rb");
  if (state->inFile == NULL)
    throw new mgException("Could not open file %s", fileName);

  ogg_sync_init(&state->sync); // Now we can read pages 

  // read a buffer of data 
  char* buffer = ogg_sync_buffer(&state->sync, VORBIS_BUFFER_SIZE);
  int bytes = fread(buffer, 1, VORBIS_BUFFER_SIZE, state->inFile);
  ogg_sync_wrote(&state->sync, bytes);
    
  // read the first page, which is guaranteed to be small and only contain
  // the Vorbis stream initial header.
  // We need the first page to get the stream serialno. 
  if (ogg_sync_pageout(&state->sync, &state->page) != 1)
    throw new mgException("Input does not appear to be an Ogg bitstream.");
  
  // Get the serial number and set up the rest of decode. 
  ogg_stream_init(&state->stream, ogg_page_serialno(&state->page));
    
  mgVorbisReadHeader(state);    

  // return format info to caller      
  channels = state->info.channels;
  samplesPerSec = state->info.rate;

  return state;
}

//--------------------------------------------------------------
// close the vorbis file and free state
void mgVorbisClose(
  void* vorbisState)
{
  mgVorbisState* state = (mgVorbisState*) vorbisState;

  // if never opened the file, nothing to do
  if (state->inFile != NULL)
  {
    // ogg_page and ogg_packet structs always point to storage in
    // libvorbis.  They're never freed or manipulated directly 
    vorbis_block_clear(&state->block);
    vorbis_dsp_clear(&state->decode);

    // clean up this logical bitstream; before exit we see if we're
    //   followed by another [chained] 
    
    ogg_stream_clear(&state->stream);
    vorbis_comment_clear(&state->comment);
    vorbis_info_clear(&state->info);  // must be called last 

    // OK, clean up the framer 
    ogg_sync_clear(&state->sync);
  }

  delete state;
}

//--------------------------------------------------------------
// read the vorbis file header
void mgVorbisReadHeader(
  mgVorbisState* state)
{
  // extract the initial header from the first page and verify that the
  // Ogg bitstream is in fact Vorbis data 
    
  // error; stream version mismatch perhaps 
  if (ogg_stream_pagein(&state->stream, &state->page) < 0)
    throw new mgException("Error reading first page of Ogg bitstream data.");
    
  // no page? must not be vorbis 
  if (ogg_stream_packetout(&state->stream, &state->packet) != 1)
    throw new mgException("Error reading initial header packet.");

  // I handle the initial header first instead of just having the code
  // read all three Vorbis headers at once because reading the initial
  // header is an easy way to identify a Vorbis bitstream and it's
  // useful to see that functionality seperated out. 
  vorbis_info_init(&state->info);
  vorbis_comment_init(&state->comment);
 
  // error case; not a vorbis header 
  if (vorbis_synthesis_headerin(&state->info, &state->comment, &state->packet) < 0)
    throw new mgException("This Ogg bitstream does not contain Vorbis audio data.");
    
  // At this point,  we're sure we're Vorbis. We've set up the logical
  // (Ogg) bitstream decoder. Get the comment and codebook headers and
  // set up the Vorbis decoder 
  int rc;
      
  mgVorbisReadPacket(state);
  rc = vorbis_synthesis_headerin(&state->info, &state->comment, &state->packet);
  mgVorbisReadPacket(state);
  rc = vorbis_synthesis_headerin(&state->info, &state->comment, &state->packet);

//#define COMMENTS
#ifdef COMMENTS
  // Throw the comments plus a few lines about the bitstream we're decoding 
  char **ptr=state->comment.user_comments;
  while (*ptr)
  {
    mgDebug("%s", *ptr);
    ++ptr;
  }
  mgDebug("Bitstream is %d channel,  %ldHz", state->info.channels, state->info.rate);
  mgDebug("Encoded by: %s", state->comment.vendor);
#endif
    
  // OK, got and parsed all three headers. 
  // Initialize the Vorbis packet->PCM decoder. 
  if (vorbis_synthesis_init(&state->decode, &state->info) != 0)
    throw new mgException("Error: Corrupt header during playback initialization.");

  // local state for most of the decode so multiple block decodes can
  // proceed in parallel. We could init multiple vorbis_block structures
  // for decode here.
  vorbis_block_init(&state->decode, &state->block);          
}

//--------------------------------------------------------------
// read a packet
void mgVorbisReadPacket(
  mgVorbisState* state)
{
  int rc;
  while (true)
  {
    // read a packet
    rc = ogg_stream_packetout(&state->stream, &state->packet);
    if (rc == 1)
      break;  // we're done
    if (rc < 0)
      mgDebug("error reading packet. rc = %d", rc);

    // get another page
    while (true)
    {
      rc = ogg_sync_pageout(&state->sync, &state->page);
      if (rc == 1)
      {
        rc = ogg_stream_pagein(&state->stream, &state->page);
        if (rc < 0)
          throw new mgException("Error reading page");
        break;
      }
      if (rc == 0)
      {
        // read data into ogg buffer
        char* buffer = ogg_sync_buffer(&state->sync, VORBIS_BUFFER_SIZE);
        int bytesRead = fread(buffer, 1, VORBIS_BUFFER_SIZE, state->inFile);
        ogg_sync_wrote(&state->sync, bytesRead);
//        mgDebug("read %d bytes", bytesRead);
        if (bytesRead == 0)
          throw new mgException("unexpected end of file");
      }
      // if rc is negative, keep reading and hoping for sync
    }
  }
}

//--------------------------------------------------------------
// fill buffer with decoded vorbis pcm (16-bit).  return bytes read
int mgVorbisRead(
  void* vorbisState,
  char* target,
  int readLen)           // length to read (bytes)
{
  mgVorbisState* state = (mgVorbisState*) vorbisState;

  int sampleSize = state->info.channels * sizeof(short);
  int bytesRead = 0;

  while (bytesRead < readLen)
  {
    // decode from whatever we've read already
    float** pcm;
    int samples = vorbis_synthesis_pcmout(&state->decode, &pcm);

    // figure number of samples we need
    int needSamples = (readLen-bytesRead)/sampleSize;
    samples = min(samples, needSamples);

    // process the samples
    if (samples > 0)
    {
      // decode and fill buffer
//      mgDebug("convert %d samples at %g", samples, bytesRead/(double) sampleSize);
      mgVorbisConvert(state, pcm, samples, (short*) (target+bytesRead));
                  
      // tell libvorbis how many samples we actually consumed 
      vorbis_synthesis_read(&state->decode, samples);

      bytesRead += samples*sampleSize;
    }

    // if last packet was end of stream, we're done
    if (state->packet.e_o_s != 0)
      break;

    // if we need more data, read another packet
    if (bytesRead < readLen)
    {
      // decode a packet
      mgVorbisReadPacket(state);
      if (vorbis_synthesis(&state->block, &state->packet) == 0) // test for success! 
        vorbis_synthesis_blockin(&state->decode, &state->block);
    }
  }

  return bytesRead;
}

//--------------------------------------------------------------
// convert samples to output PCM format
void mgVorbisConvert(
  mgVorbisState* state,
  float** pcm,
  int samples,
  short* result)
{
  // convert floats to 16 bit signed ints (host order) and interleave 
  for (int i = 0; i < state->info.channels; i++)
  {
    short* ptr = result+i;
    float* mono = pcm[i];
    for (int j = 0; j < samples; j++)
    {
      int val = (int) floor(mono[j]*32767.0f + 0.5f);

      // might as well guard against clipping 
      val = (val > 32767) ? 32767 : val;
      val = (val < -32768) ? -32768 : val;

      *ptr = (short) val;
      ptr += state->info.channels;
    }
  }
}
                  
//--------------------------------------------------------------
// reset read position to start
void mgVorbisReset(
  void* vorbisState)
{
  mgVorbisState* state = (mgVorbisState*) vorbisState;

  // reset file to start
  fseek(state->inFile, 0, SEEK_SET);

  // reset ogg to start
  ogg_stream_reset(&state->stream);

  // reset vorbis decode
  vorbis_block_clear(&state->block);
  vorbis_dsp_clear(&state->decode);

  vorbis_comment_clear(&state->comment);
  vorbis_info_clear(&state->info);  // must be called last 

  // read a buffer of data 
  char* buffer = ogg_sync_buffer(&state->sync, VORBIS_BUFFER_SIZE);
  int bytes = fread(buffer, 1, VORBIS_BUFFER_SIZE, state->inFile);
  ogg_sync_wrote(&state->sync, bytes);
    
  // read the first page, which is guaranteed to be small and only contain
  // the Vorbis stream initial header.
  // We need the first page to get the stream serialno. 
  if (ogg_sync_pageout(&state->sync, &state->page) != 1)
    throw new mgException("Input does not appear to be an Ogg bitstream.");
  
  // read header
  mgVorbisReadHeader(state);
}


