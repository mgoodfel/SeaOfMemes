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

#include "mgGenCharCache.h"
#include "Graphics2D/Surfaces/mgGenSurface.h"

#include "ft2build.h"
#include FT_FREETYPE_H

struct mgGenCharCacheEntry
{
  const void* font;
  int letter;
  mgGenCharDefn* value;
};

const int GROW_LIMIT = 66;  // percent

//--------------------------------------------------------------
// constructor
mgGenCharDefn::mgGenCharDefn()
{
  m_bitmapData = NULL;
}

//--------------------------------------------------------------
// destructor
mgGenCharDefn::~mgGenCharDefn()
{
  delete m_bitmapData;
  m_bitmapData = NULL;
}

//--------------------------------------------------------------
// constructor
mgGenCharCache::mgGenCharCache()
{
  m_tableSize = 97;
  m_tableCount = 0;
  m_entries = new mgGenCharCacheEntry[m_tableSize];
  
  for (int i = 0; i < m_tableSize; i++)
  {
    m_entries[i].font = NULL;
    m_entries[i].letter = 0;
    m_entries[i].value = NULL;
  }
}

//--------------------------------------------------------------
// destructor
mgGenCharCache::~mgGenCharCache()
{
  int posn = getStartPosition();
  while (posn != -1)
  {
    const void* font;
    DWORD letter;
    mgGenCharDefn* defn;
    getNextAssoc(posn, font, letter, defn);
    delete defn;
  }
  removeAll();
  delete m_entries;
}

//--------------------------------------------------------------
// get a character.  returns NULL if not found
const mgGenCharDefn* mgGenCharCache::getChar(
  void* font,
  DWORD letter)
{
  mgGenCharDefn* defn;
  if (lookup(font, letter, defn))
    return defn;

  // look for the character in the font
  FT_Face face = (FT_Face) font;

  FT_UInt glyphIndex = FT_Get_Char_Index(face, letter);

  int error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER ); 
  if (error != 0)
  {
    mgDebug("FT_Load_Glyph returns %d", error);
    return NULL;
  }

  FT_GlyphSlot slot = face->glyph;
  FT_Bitmap bitmap = slot->bitmap;

  defn = new mgGenCharDefn();

  defn->m_advanceX = (slot->advance.x+31)/64;
  defn->m_advanceY = (slot->advance.y+31)/64;

  defn->m_bitmapX = slot->bitmap_left;
  defn->m_bitmapY = slot->bitmap_top;
  defn->m_bitmapWidth = bitmap.width;
  defn->m_bitmapHeight = bitmap.rows;

  // =-= for now, just allocate memory for bitmap
  defn->m_bitmapData = new BYTE[defn->m_bitmapWidth * defn->m_bitmapHeight];

  // copy data from FreeType bitmap
  for (int i = 0; i < bitmap.rows; i++)
  {
    BYTE* defnLine = defn->m_bitmapData + defn->m_bitmapWidth * i;
    BYTE* bitmapLine = bitmap.buffer + bitmap.pitch * i;
    memcpy(defnLine, bitmapLine, defn->m_bitmapWidth);
  }

  // save in hash table
  setAt(font, letter, defn);

  return defn;
}

//--------------------------------------------------------------------
// lookup a key
BOOL mgGenCharCache::lookup(
  const void* font,
  DWORD letter,
  mgGenCharDefn*& value) const
{
  // hash the key for first probe into table
  int index = hash(font, letter) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgGenCharCacheEntry *entry = (mgGenCharCacheEntry *) &m_entries[index];
    if (entry->font == NULL)
      return false;
      
    if (entry->font == font && entry->letter == letter)
    {
      value = entry->value;
      return true;
    }

    // try next entry
    index++;
    if (index >= m_tableSize)
      index = 0;
      
    // if we've wrapped
    if (index == startIndex)
      return false;  // no match on full table
  }
}

//--------------------------------------------------------------------
// set/change a key
void mgGenCharCache::setAt(
  const void* font,
  DWORD letter,
  mgGenCharDefn* value)
{
  if (m_tableCount * 100 > m_tableSize * GROW_LIMIT)
    grow();
    
  // hash the key for first probe into table
  int index = hash(font, letter) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgGenCharCacheEntry *entry = (mgGenCharCacheEntry *) &m_entries[index];
    if (entry->font == NULL)
    {
      // set entry
      entry->font = font;
      entry->letter = letter;
      entry->value = value;
      m_tableCount++;
      return;
    }
    
    if (entry->font == font && entry->letter == letter)
    {
      // existing key changed
      entry->value = value;
      return;
    }
    
    // look in next entry
    index++;
    if (index >= m_tableSize)
      index = 0;
      
    // if we've wrapped
    if (index == startIndex)
    {
      // table is full.  shouldn't happen with grow test at top
      grow();
      setAt(font, letter, value);
      return;
    }
  }
}

//--------------------------------------------------------------------
// remove a key
void mgGenCharCache::removeKey(
  const void* font,
  DWORD letter)
{
  // hash the key for first probe into table
  int index = hash(font, letter) % m_tableSize;
  int startIndex = index;
  while (true)
  {
    mgGenCharCacheEntry *entry = (mgGenCharCacheEntry *) &m_entries[index];
    if (entry->font == font && entry->letter == letter)
    {
      // remove the key
      m_entries[index].font = NULL;
      m_entries[index].letter = 0;
      m_entries[index].value = NULL;
      m_tableCount--;
      return;
    }
    
    // look in next entry
    index++;
    if (index >= m_tableSize)
      index = 0;
      
    // if we've wrapped, key not found
    if (index == startIndex)
      return;
  }
}
    
//--------------------------------------------------------------------
// start iteration through map
int mgGenCharCache::getStartPosition() const
{
  // find first non-empty entry
  for (int index = 0; index < m_tableSize; index++)
  {
    mgGenCharCacheEntry *entry = (mgGenCharCacheEntry *) &m_entries[index];
    if (entry->font != NULL)
      return index;
  }
  return -1;
}

//--------------------------------------------------------------------
// get next association
void mgGenCharCache::getNextAssoc(
  int& posn, 
  const void*& font,
  DWORD& letter,
  mgGenCharDefn*& value) const
{
  int index = posn;
  index = max(0, min(m_tableSize-1, index));
  mgGenCharCacheEntry *entry = (mgGenCharCacheEntry *) &m_entries[index];
  if (entry->font == NULL)
    return;  // bad posn
  font = entry->font;
  letter = entry->letter;
  value = entry->value;
  
  // find the next key, if any
  index++;
  while (index < m_tableSize)
  {
    mgGenCharCacheEntry *entry = (mgGenCharCacheEntry *) &m_entries[index];
    if (entry->font != NULL)
    {
      posn = index;
      return;
    }
    else index++;
  }
  // no next key found
  posn = -1;
}

//--------------------------------------------------------------------
// remove all keys
void mgGenCharCache::removeAll() 
{
  // delete all the entries
  for (int i = 0; i < m_tableSize; i++)
  {
    mgGenCharCacheEntry *entry = (mgGenCharCacheEntry *) &m_entries[i];
    if (entry->font != NULL)
    {
      entry->font = NULL;
      entry->letter = 0;
      entry->value = NULL;
      m_tableCount--;
    }
  }
}

//--------------------------------------------------------------------
// hash a key
int mgGenCharCache::hash(
  const void* font,
  DWORD letter) const
{
  // pointer xor with letter as key
  return abs((int) (letter ^ (int) (size_t) font));
}

//--------------------------------------------------------------------
// grow the table
void mgGenCharCache::grow()
{
  mgGenCharCacheEntry* oldEntries = m_entries;
  int oldSize = m_tableSize;
  
  m_tableSize = 1+2*m_tableSize;
  m_entries = new mgGenCharCacheEntry[m_tableSize];
  for (int i = 0; i < m_tableSize; i++)
  {
    m_entries[i].font = NULL;
    m_entries[i].letter = 0;
    m_entries[i].value = NULL;
  }
    
  for (int i = 0; i < oldSize; i++)
  {
    mgGenCharCacheEntry *entry = &oldEntries[i];
    if (entry->font != NULL)
      setAt(entry->font, entry->letter, entry->value);
  }
  // done with old table
  delete oldEntries;
}
