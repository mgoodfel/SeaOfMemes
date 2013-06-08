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
#if !defined(MGWEBGLVERTEXBUFFER_H) && defined(SUPPORT_WEBGL)
#define MGWEBGLVERTEXBUFFER_H

class mgWebGLVertexBuffer : public mgVertexBuffer
{
public:
  // constructor
  mgWebGLVertexBuffer(
    int vertexSize,                   // size of vertex in bytes
    const mgVertexAttrib* attribs,    // array of attributes.  end with offset=-1
    int maxVertexes,                  // max number of vertices
    BOOL dynamic);                    // support reset and reuse

  // destructor
  virtual ~mgWebGLVertexBuffer();

  // reset the buffer
  virtual void reset();

  // move buffer to display.  delete memory.
  virtual void loadDisplay();

  // delete buffer from display
  virtual void unloadDisplay();

protected:
  // vertex data
  mgBufferHandle m_buffer;

  BOOL m_updateBuffer;

  friend class mgWebGLDisplay;
};

#endif
