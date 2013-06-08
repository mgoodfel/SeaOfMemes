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
#ifndef MGMAPDWORDTODWORD_H
#define MGMAPDWORDTODWORD_H

/*
  Hashes DWORD keys to DWORD values.
*/
class mgMapDWordToDWord 
{
public:
  // constructor
  mgMapDWordToDWord();
  
  // destructor
  virtual ~mgMapDWordToDWord();
  
  // return count of entries
  virtual int length() const
  {
    return m_tableCount;
  }

  // lookup value of key
  virtual BOOL lookup(
    DWORD key, 
    DWORD& value) const;
  
  // set new value of key
  virtual void setAt(
    DWORD key, 
    DWORD value);

  // remove a key
  virtual void removeKey(
    DWORD key);
    
  // get position of first association
  virtual int getStartPosition() const;
  
  // get next association in table
  virtual void getNextAssoc(
    int &posn, 
    DWORD& key, 
    DWORD& value) const;

  // remove all entries
  virtual void removeAll(); 

protected:
  int m_tableSize;
  int m_tableCount;
  struct mgMapDWordToDWordEntry* m_entries;
  
  // hash the key 
  int hash(
    DWORD key) const;
  
  // grow the table
  void grow();
};

#endif
