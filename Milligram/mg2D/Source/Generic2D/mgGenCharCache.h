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
#ifndef MGGENCHARCACHE_H
#define MGGENCHARCACHE_H

class mgGenCharDefn
{
public:
  // constructor
  mgGenCharDefn();

  // destructor
  virtual ~mgGenCharDefn();

  int m_advanceX;               // horz increment
  int m_advanceY;               // vertical increment

  int m_bitmapX;                // bearing x
  int m_bitmapY;                // bearing y

  int m_bitmapWidth;
  int m_bitmapHeight;
  
  BYTE* m_bitmapData;
};

class mgGenCharCache
{
public:
  // constructor
  mgGenCharCache();

  // destructor
  virtual ~mgGenCharCache();

  // get a character.  returns NULL if not found
  const mgGenCharDefn* getChar(
    void* font,
    DWORD letter);

protected:
  int m_tableSize;
  int m_tableCount;
  struct mgGenCharCacheEntry* m_entries;
  
  // hash the key 
  int hash(
    const void* font,
    DWORD letter) const;
  
  // grow the table
  void grow();

  // lookup value of key
  BOOL lookup(
    const void* font,
    DWORD letter,
    mgGenCharDefn*& value) const;
  
  // set new value of key
  void setAt(
    const void* font,
    DWORD letter,
    mgGenCharDefn* value);

  // remove a key
  void removeKey(
    const void* font,
    DWORD letter);
    
  // get position of first association
  int getStartPosition() const;
  
  // get next association in table
  void getNextAssoc(
    int &posn, 
    const void*& font,
    DWORD& letter,
    mgGenCharDefn*& value) const;

  // remove all entries
  void removeAll(); 
};

#endif
