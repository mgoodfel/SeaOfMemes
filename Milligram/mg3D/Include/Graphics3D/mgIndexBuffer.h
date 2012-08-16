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
#ifndef MGINDEXBUFFER_H
#define MGINDEXBUFFER_H

class mgIndexBuffer
{
public:
  // destructor
  virtual ~mgIndexBuffer();

  // add an index
  void addIndex(
    int index)
  {
    // if no room in buffer, throw exception
    if (!canAdd(1))
      throw new mgErrorMsg("mgIndexFull", "", "");
    if (m_longIndexes)
      m_longData[m_count++] = (UINT32) index;
    else m_shortData[m_count++] = (UINT16) index;
  }

  // add index pattern for a rectangle
  void addRectIndex(
    int baseVertex)
  {
    // if no room in buffer, throw exception
    if (!canAdd(6))
      throw new mgErrorMsg("mgIndexFull", "", "");

    if (m_longIndexes)
    {
      UINT32* longData = &m_longData[m_count];
      longData[0] = (UINT32) baseVertex;       // tl
      longData[1] = (UINT32) (baseVertex+1);     // tr
      longData[2] = (UINT32) (baseVertex+2);     // bl
      longData[3] = (UINT32) (baseVertex+2);     // bl
      longData[4] = (UINT32) (baseVertex+1);     // tr
      longData[5] = (UINT32) (baseVertex+3);     // br
    }
    else
    {
      UINT16* shortData = &m_shortData[m_count];
      shortData[0] = (UINT16) baseVertex;       // tl
      shortData[1] = (UINT16) (baseVertex+1);     // tr
      shortData[2] = (UINT16) (baseVertex+2);     // bl
      shortData[3] = (UINT16) (baseVertex+2);     // bl
      shortData[4] = (UINT16) (baseVertex+1);     // tr
      shortData[5] = (UINT16) (baseVertex+3);     // br
    }
    m_count+=6;
  }

  // add indexes for a grid of points
  void addGrid(
    int vertexBase,         // starting index
    int rowSize,            // length of row
    int rows,               // total row count
    int cols,               // total col count
    BOOL outward);          // true to face outward

  // return current length
  int getLength() const
  {
    return m_count;
  }

  // return true if room for indexes
  BOOL canAdd(
    int count) const
  {
    return (m_count + count) <= m_size;
  }

  // return size of indexes
  int indexSize() const
  {
    return (int) (m_longIndexes ? sizeof(UINT32) : sizeof(UINT16));
  }

  // reset buffer for reuse
  virtual void reset();

  // move buffer to display.  deleted from memory.
  virtual void loadDisplay() = 0;

  // delete buffer from display
  virtual void unloadDisplay() = 0;

protected:
  // we could subclass this with LongIndexBuffer and ShortIndexBuffer, 
  // but then we'd pay the price of a virtual function call as opposed
  // to just testing the type in the two methods above.
  void* m_data;
  UINT16* m_shortData;
  UINT32* m_longData;

  int m_size;
  int m_count;
  BOOL m_dynamic;
  BOOL m_longIndexes;

  // constructor
  mgIndexBuffer(
    int size,
    BOOL dynamic,
    BOOL longIndexes);
};

#endif
