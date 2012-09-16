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
#ifndef BVHFILE_H
#define BVHFILE_H

const int CHANNEL_XROT = 0;
const int CHANNEL_YROT = 1;
const int CHANNEL_ZROT = 2;
const int CHANNEL_XPOS = 3;
const int CHANNEL_YPOS = 4;
const int CHANNEL_ZPOS = 5;

class BVHNode
{
public:
  BVHNode* m_parent;

  mgString m_name;
  mgPoint3 m_offset;
  mgPtrArray m_children;

  int m_channelCount;
  int m_channels[10];

  // constructor
  BVHNode();

  // destructor
  virtual ~BVHNode();

  // return count of descendents
  virtual int nodeCount();
};


class BVHFile
{
public:
  BVHNode* m_root;
  int m_totalChannels;
  int m_frameCount;
  double m_frameTime;

  double* m_samples;

  // constructor
  BVHFile();

  // destructor
  virtual ~BVHFile();

  // parse the file
  virtual void parseFile(
    const char* fileName);

  // parse a buffer
  virtual void parseBuffer(
    const char* buffer,
    int bufferLen);

protected:
  int m_linenum;
  int m_state;
  mgString m_token;
  int m_count;

  BVHNode* m_currentNode;

  // unexpected token
  virtual void unexpectedToken(
    const char* token);

  // process a token
  virtual void processToken(
    const char* token);
};

#endif

