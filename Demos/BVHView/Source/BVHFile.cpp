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
    Parse BVH character animation file
*/

#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "BVHFile.h"

const int STATE_START = 0;
const int STATE_ROOT        = 1;
const int STATE_JOINT       = 2;
const int STATE_OPEN        = 3;
const int STATE_NODE        = 4;
const int STATE_OFFSET_X    = 5;
const int STATE_OFFSET_Y    = 6;
const int STATE_OFFSET_Z    = 7;
const int STATE_CHANNELS    = 8;
const int STATE_CHANNEL_NAME = 9;
const int STATE_END         = 10;
const int STATE_MOTION      = 11;
const int STATE_FRAMES      = 12;
const int STATE_FRAME_WORD  = 13;
const int STATE_TIME_WORD   = 14;
const int STATE_FRAME_TIME  = 15;
const int STATE_DATA        = 16;
const int STATE_DONE        = 17;

//--------------------------------------------------------------
// constructor
BVHNode::BVHNode()
{
  m_parent = NULL;
  m_channelCount = 0;
}

//--------------------------------------------------------------
// destructor
BVHNode::~BVHNode()
{
  for (int i = 0; i < m_children.length(); i++)
  {
    BVHNode* child = (BVHNode*) m_children[i];
    delete child;
  }
  m_children.removeAll();
}

//--------------------------------------------------------------
// return count of descendents
int BVHNode::nodeCount()
{
  int count = 1;
  for (int i = 0; i < m_children.length(); i++)
  {
    BVHNode* child = (BVHNode*) m_children[i];
    count += child->nodeCount();
  }
  return count;
}

//--------------------------------------------------------------
// constructor
BVHFile::BVHFile()
{
  m_root = NULL;
  m_currentNode = NULL;
  m_frameTime = 0.0;
  m_frameCount = 0;
  m_samples = NULL;
}

//--------------------------------------------------------------
// destructor
BVHFile::~BVHFile()
{
  delete m_root;
  m_root = NULL;

  delete m_samples;
  m_samples = NULL;
}

//--------------------------------------------------------------
// parse the file
void BVHFile::parseFile(
  const char* fileName)
{
  FILE* bvhFile = fopen(fileName, "rt");
  if (bvhFile == NULL)
    throw new mgException("cannot open %s", (const char*) fileName);

  m_state = STATE_START;
  m_linenum = 1;

  char buffer[4096];
  while (true)
  {
    size_t readLen = fread(buffer, 1, sizeof(buffer), bvhFile);
    if (readLen == 0)
      break;
    parseBuffer(buffer, (int) readLen);
  }
  if (!m_token.isEmpty())
    processToken(m_token);

  fclose(bvhFile);

  if (m_root != NULL)
    mgDebug("read %d nodes", m_root->nodeCount());
  mgDebug("frameCount: %d, frameTime: %g, totalChannels: %d", m_frameCount, m_frameTime, m_totalChannels);

  if (m_state != STATE_DONE)
    mgDebug("did not read all samples.  count = %d, expected %d", m_count, m_totalChannels*m_frameCount);
}

//--------------------------------------------------------------
// parse a buffer
void BVHFile::parseBuffer(
  const char* buffer,
  int bufferLen)
{
  for (int i = 0; i < bufferLen; i++)
  {
    char c = buffer[i];
    if (isspace(c))
    {
      if (!m_token.isEmpty())
      {
        processToken(m_token);
        m_token.empty();
      }
      if (c == '\n')
        m_linenum++;
    }
    else if (c == '{')
    {
      if (!m_token.isEmpty())
      {
        processToken(m_token);
        m_token.empty();
      }
      processToken("{");
    }
    else if (c == '{')
    {
      if (!m_token.isEmpty())
      {
        processToken(m_token);
        m_token.empty();
      }
      processToken("}");
    }
    else if (c == ':')
    {
      m_token += c;
      processToken(m_token);
      m_token.empty();
    }
    else m_token += c;
  }
}

//--------------------------------------------------------------
// unexpected token in input
void BVHFile::unexpectedToken(
  const char* token)
{
  throw new mgException("line %d: unexpected token %s", m_linenum, (const char*) token);
}

//--------------------------------------------------------------
// process a token
void BVHFile::processToken(
  const char* token)
{
  double value;

  switch (m_state)
  {
    case STATE_START:
      if (_stricmp(token, "HIERARCHY") == 0)
      {
        m_totalChannels = 0;
        m_state = STATE_ROOT;
      }
      else if (_stricmp(token, "MOTION") == 0)
        m_state = STATE_MOTION;
      else unexpectedToken(token);
      break;

    case STATE_ROOT:
      if (_stricmp(token, "ROOT") == 0)
      {
        m_state = STATE_JOINT;
        m_root = new BVHNode();
        m_root->m_parent = NULL;
        m_currentNode = m_root;
      }
      else unexpectedToken(token);
      break;

    case STATE_JOINT:
      m_currentNode->m_name = token;
      m_state = STATE_OPEN;
      break;

    case STATE_OPEN:
      if (_stricmp(token, "{") == 0)
        m_state = STATE_NODE;
      else unexpectedToken(token);
      break;

    case STATE_NODE:
      if (_stricmp(token, "OFFSET") == 0)
      {
        m_state = STATE_OFFSET_X;
      }
      else if (_stricmp(token, "CHANNELS") == 0)
      {
        m_state = STATE_CHANNELS;
      }
      else if (_stricmp(token, "JOINT") == 0)
      {
        m_state = STATE_JOINT;
        BVHNode* node = new BVHNode();
        node->m_parent = m_currentNode;
        m_currentNode->m_children.add(node);
        m_currentNode = node;
      }
      else if (_stricmp(token, "End") == 0)
      {
        m_state = STATE_END;
      }
      else if (_stricmp(token, "}") == 0)
      {
        m_currentNode = m_currentNode->m_parent;
        if (m_currentNode == NULL)
          m_state = STATE_START;
        else m_state = STATE_NODE;
      }
      else unexpectedToken(token);
      break;

    case STATE_OFFSET_X:
      if (1 != sscanf(token, "%lf", &m_currentNode->m_offset.x))
        unexpectedToken(token);
      m_state = STATE_OFFSET_Y;
      break;

    case STATE_OFFSET_Y:
      if (1 != sscanf(token, "%lf", &m_currentNode->m_offset.y))
        unexpectedToken(token);
      m_state = STATE_OFFSET_Z;
      break;

    case STATE_OFFSET_Z:
      if (1 != sscanf(token, "%lf", &m_currentNode->m_offset.z))
        unexpectedToken(token);
      m_state = STATE_NODE;
      break;

    case STATE_CHANNELS:
      if (1 != sscanf(token, "%d", &m_currentNode->m_channelCount))
        unexpectedToken(token);
      m_state = STATE_CHANNEL_NAME;
      m_count = 0;
      m_totalChannels += m_currentNode->m_channelCount;
      break;

    case STATE_CHANNEL_NAME:
      if (_stricmp(token, "Xposition") == 0)
        m_currentNode->m_channels[m_count++] = CHANNEL_XPOS;
      else if (_stricmp(token, "Yposition") == 0)
        m_currentNode->m_channels[m_count++] = CHANNEL_YPOS;
      else if (_stricmp(token, "Zposition") == 0)
        m_currentNode->m_channels[m_count++] = CHANNEL_ZPOS;

      else if (_stricmp(token, "Xrotation") == 0)
        m_currentNode->m_channels[m_count++] = CHANNEL_XROT;
      else if (_stricmp(token, "Yrotation") == 0)
        m_currentNode->m_channels[m_count++] = CHANNEL_YROT;
      else if (_stricmp(token, "Zrotation") == 0)
        m_currentNode->m_channels[m_count++] = CHANNEL_ZROT;

      else unexpectedToken(token);

      if (m_count == m_currentNode->m_channelCount)
        m_state = STATE_NODE;
      break;

    case STATE_END:
      if (_stricmp(token, "site") == 0)
      {
        BVHNode* node = new BVHNode();
        node->m_parent = m_currentNode;
        m_currentNode->m_children.add(node);
        m_currentNode = node;
        m_state = STATE_OPEN;
      }
      else unexpectedToken(token);  
      break;

    case STATE_MOTION:
      if (_stricmp(token, "Frames:") == 0)
        m_state = STATE_FRAMES;
      else unexpectedToken(token);
      break;

    case STATE_FRAMES:
      if (1 == sscanf(token, "%d", &m_frameCount))
        m_state = STATE_FRAME_WORD;
      else unexpectedToken(token);
      break;

    case STATE_FRAME_WORD: 
      if (_stricmp(token, "Frame") == 0)
        m_state = STATE_TIME_WORD;
      else unexpectedToken(token);
      break;

    case STATE_TIME_WORD:  // seen "Frame" 
      if (_stricmp(token, "Time:") == 0)
        m_state = STATE_FRAME_TIME;
      else unexpectedToken(token);
      break;

    case STATE_FRAME_TIME:
      if (1 == sscanf(token, "%lf", &m_frameTime))
      {
        m_state = STATE_DATA;
        m_count = 0;
        m_samples = new double[m_totalChannels * m_frameCount];
      }
      else unexpectedToken(token);
      break;

    case STATE_DATA:
      // stay in this state until end of samples
      if (1 != sscanf(token, "%lf", &value))
        unexpectedToken(token);
      m_samples[m_count++] = value;
      if (m_count == m_totalChannels * m_frameCount)
        m_state = STATE_DONE;
      break;

    case STATE_DONE:
      unexpectedToken(token);
      break;
  }
}
