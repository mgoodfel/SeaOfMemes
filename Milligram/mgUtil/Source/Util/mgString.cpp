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
// handle growing character buffer.
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Util/mgString.h"

const int MAX_UTF_CHAR = 7;  // 6 chars plus ending null

// =-= what to do on Windows when unicode exceeds 16-bit WCHAR?  
// Currently just casting result.

//-----------------------------------------------------------
// shared initialization
void mgString::init()
{
  m_allocLen = sizeof(m_shortData);
  m_allocLen--;  // leave room for ending null
  m_data =  m_shortData;
  m_dataLen = 0;
  m_data[m_dataLen] = '\0';
  m_growBy = 128;
}

//-----------------------------------------------------------
// constructors
mgString::mgString()
{
  init();
}

//-----------------------------------------------------------
// construct from constant string
mgString::mgString(
  const char* str)
{
  init();

  if (str != NULL)
    write(str, (int) strlen(str));
}

//-----------------------------------------------------------
// construct from constant string
mgString::mgString(
  const WCHAR* str)
{
  init();

  if (str != NULL)
    write(str, (int) wcslen(str));
}

//-----------------------------------------------------------
// construct from constant string
mgString::mgString(
  const char* str, 
  int len)
{
  init();

  if (str != NULL)
    write(str, len);
}

//-----------------------------------------------------------
// construct from constant string
mgString::mgString(
  const WCHAR* str, 
  int len)
{
  init();

  if (str != NULL)
    write(str, len);
}

//-----------------------------------------------------------
// construct from constant string
mgString::mgString(
  const mgString& other)
{
  init();

  write((const char*) other, other.length());
}

//-----------------------------------------------------------
// destructor 
mgString::~mgString()
{
  if (m_data != m_shortData)
    delete m_data;
}

//-----------------------------------------------------------
// grow buffer to hold new length
void mgString::grow(
  int newLen)     // length needed
{
  while (m_allocLen < newLen)
  {
    m_allocLen += m_growBy;
  }

  char* newData = new char[m_allocLen+1];
  memcpy(newData, m_data, m_dataLen+1);
  if (m_data != m_shortData)
    delete m_data;

  m_data = newData;
}

//-----------------------------------------------------------
// set amount to grow by when buffer full
void mgString::setGrowBy(
  int len)
{ 
  m_growBy = len; 
}

//-----------------------------------------------------------
// set allocation length of string buffer
void mgString::setAllocLength(
  int len)
{ 
  grow(len); 
}

//-----------------------------------------------------------
// convert to UTF8 format
void mgString::toUTF8(
  char* target,
  int& len,
  int value)
{
  if ((value >> 7) == 0)
  {
    target[0] = (char) (0xFF & value);
    len = 1;
  }
  else if ((value >> 11) == 0)
  {
    target[0] = 0xC0 | (char) (value >> 6);
    target[1] = 0x80 | (char) (value & 0x3F);
    len = 2;
  }
  else if ((value >> 16) == 0)
  {
    target[0] = 0xE0 | (char) (value >> 12);
    target[1] = 0x80 | (char) ((value >> 6) & 0x3F);
    target[2] = 0x80 | (char) (value & 0x3F);
    len = 3;
  }
  else if ((value >> 21) == 0)
  {
    target[0] = 0xF0 | (char) (value >> 18);
    target[1] = 0x80 | (char) ((value >> 12) & 0x3F);
    target[2] = 0x80 | (char) ((value >> 6) & 0x3F);
    target[3] = 0x80 | (char) (value & 0x3F);
    len = 4;
  }
  else if ((value >> 26) == 0)
  {
    target[0] = 0xF8 | (char) (value >> 24);
    target[1] = 0x80 | (char) ((value >> 18) & 0x3F);
    target[2] = 0x80 | (char) ((value >> 12) & 0x3F);
    target[3] = 0x80 | (char) ((value >> 6) & 0x3F);
    target[4] = 0x80 | (char) (value & 0x3F);
    len = 5;
  }
  else
  {
    target[0] = 0xFC | (char) (value >> 29);
    target[1] = 0x80 | (char) ((value >> 24) & 0x3F);
    target[2] = 0x80 | (char) ((value >> 18) & 0x3F);
    target[3] = 0x80 | (char) ((value >> 12) & 0x3F);
    target[4] = 0x80 | (char) ((value >> 6) & 0x3F);
    target[5] = 0x80 | (char) (value & 0x3F);
    len = 6;
  }
}

//-----------------------------------------------------------
// return length of UTF-8 encoded char
int mgString::lenUTF8(
  int value)
{
  if ((value >> 7) == 0)
    return 1;
  else if ((value >> 11) == 0)
    return 2;
  else if ((value >> 16) == 0)
    return 3;
  else if ((value >> 21) == 0)
    return 4;
  else if ((value >> 26) == 0)
    return 5;
  else return 6;
}

//-----------------------------------------------------------
// return length of UTF-8 encoded string
int mgString::lenUTF8(
  const WCHAR* source,
  int len)
{
  int outLen = 0;
  for (int i = 0; i < len; i++)
  {
    DWORD value = source[i];
    if ((value >> 7) == 0)
      outLen++;
    else if ((value >> 11) == 0)
      outLen += 2;
    else if ((value >> 16) == 0)
      outLen += 3;
    else if ((value >> 21) == 0)
      outLen += 4;
    else if ((value >> 26) == 0)
      outLen += 5;
    else outLen += 6;
  }

  return outLen;
}

//-----------------------------------------------------------
// concatenation
mgString& mgString::write(
  const char* data,          // data to append
  int len)                   // length of data
{
  if (m_allocLen < m_dataLen + len)
    grow(m_dataLen + len);

  memcpy(m_data + m_dataLen, data, len);
  m_dataLen += len;
  m_data[m_dataLen] = '\0';

  return *this;
}

//-----------------------------------------------------------
// concatenation
mgString& mgString::write(
  const WCHAR* data,         // data to append
  int wideLen)               // number of chars
{
  int len = lenUTF8(data, wideLen);

  if (m_allocLen < m_dataLen + len)
    grow(m_dataLen + len);

  for (int i = 0; i < wideLen; i++)
  {
    int utfLen;
    toUTF8(m_data + m_dataLen, utfLen, data[i]);
    m_dataLen += utfLen;
  }
  m_data[m_dataLen] = '\0';

  return *this;
}

//-----------------------------------------------------------
// assign to single char
mgString& mgString::operator=(
  char c)
{
  if (1 > m_allocLen)
    grow(1);

  m_dataLen = 1;
  m_data[0] = c;
  m_data[m_dataLen] = '\0';

  return *this;
}

//-----------------------------------------------------------
// assign to single char
mgString& mgString::operator=(
  WCHAR c)
{
  int len = lenUTF8(&c, 1);

  if (m_allocLen < len)
    grow(len);

  toUTF8(m_data, m_dataLen, c);
  m_data[m_dataLen] = '\0';

  return *this;
}

//-----------------------------------------------------------
// assign to string
mgString& mgString::operator=(
  const char* str)
{
  empty();
  if (str != NULL)
    write(str, (int) strlen(str));
  return *this;
}

//-----------------------------------------------------------
// assign to string
mgString& mgString::operator=(
  const WCHAR* str)
{
  empty();
  if (str != NULL)
    write(str, (int) wcslen(str));
  return *this;
}

//-----------------------------------------------------------
// assign to string
mgString& mgString::operator=(
  const mgString& other)
{
  if (other.m_dataLen > m_allocLen)
    grow(other.m_dataLen);

  memcpy(m_data, other.m_data, other.m_dataLen+1);
  m_dataLen = other.m_dataLen;

  return *this;
}

//-----------------------------------------------------------
// append char to string
mgString& mgString::operator+=(
  char c)
{
  if (m_allocLen < m_dataLen+1)
    grow(m_dataLen+1);

  m_data[m_dataLen++] = c;
  m_data[m_dataLen] = '\0';  // keep null terminated

  return *this;
}

//-----------------------------------------------------------
// append char to string
mgString& mgString::operator+=(
  WCHAR c)
{
  write(&c, 1);
  return *this;
}

//-----------------------------------------------------------
// append string to string
mgString& mgString::operator+=(
  const char* str)
{  
  if (str != NULL)
    write(str, (int) strlen(str)); 
  return *this;
}

//-----------------------------------------------------------
// append string to string
mgString& mgString::operator+=(
  const WCHAR* str)
{  
  if (str != NULL)
    write(str, (int) wcslen(str)); 
  return *this;
}

//-----------------------------------------------------------
// append string to string
mgString& mgString::operator+=(
  const mgString& str)
{  
  return write((const char*) str, str.length()); 
}

//-----------------------------------------------------------
// return next letter in a string.  This may be more than a
// single Unicode character, if combining characters were supported.
// posn=0 for start, returns len when complete
int mgString::nextLetter(
  int posn,
  char* letter) const
{
  if (letter != NULL)
    letter[0] = '\0';

  if (posn < 0)
    posn = 0;
  else if (posn >= m_dataLen)
    return m_dataLen;

  // currently every unicode character is a letter.  If we handled
  // combining characters, this would not be true.
  int utfLen;
  fromUTF8(m_data+posn, utfLen);
  if (letter != NULL)
  {
    memcpy(letter, m_data + posn, utfLen);
    letter[utfLen] = '\0';
  }
  posn += utfLen;

  return posn;
}

//-----------------------------------------------------------
// return previous letter in a string.  This may be more than a
// single Unicode character, if combining characters were supported.
// posn=length() for start, returns -1 when complete
int mgString::prevLetter(
  int posn,
  char* letter) const
{
  if (letter != NULL)
    letter[0] = '\0';

  if (posn <= 0)
    return 0;
  if (posn > m_dataLen)
    posn = m_dataLen;

  // currently every unicode character is a letter.  If we handled
  // combining characters, this would not be true.

  // find start of previous char
  while (posn >= 0)
  {
    posn--;
    int c = 0xFF & m_data[posn];
    // if non-utf char
    if ((c & 0x80) == 0)
      break;
    // if utf start char
    if ((c & 0xC0) == 0xC0)
      break;
  }
  int utfLen;
  fromUTF8(m_data+posn, utfLen);
  if (letter != NULL)
  {
    memcpy(letter, m_data + posn, utfLen);
    letter[utfLen] = '\0';
  }

  return posn;
}

//-----------------------------------------------------------
// return number of 'letters' in a string.  Multiple unicode characters
// can make up a letter.
int mgString::countLetters() const
{
  // currently every unicode character is a letter.  If we handled
  // combining characters, this would not be true.

  int count = 0;
  for (int i = 0; i < m_dataLen; i++)
  {
    int c = 0xFF & m_data[i];
    // if non-utf char
    if ((c & 0x80) == 0)
      count++;
    // if utf start char
    if ((c & 0xC0) == 0xC0)
      count++;
  }
  return count;
}

//-----------------------------------------------------------
// return Unicode string.  caller owns storage
void mgString::toWCHAR(
  WCHAR*& target,
  int& len) const
{
  len = lenWCHAR(m_data, m_dataLen);
  target = new WCHAR[len+1];

  int posn = 0;
  for (int i = 0; i < len; i++)
  {
    int utfCount;
    target[i] = (WCHAR) fromUTF8(m_data+posn, utfCount);
    posn += utfCount;
  }
  target[len] = L'\0';
}

//-----------------------------------------------------------
// compare to a string
int mgString::compareN(
  int len,                            // length to compare
  const char* str) const
{ 
  return strncmp(m_data, str, len); 
}

//-----------------------------------------------------------
// compare to a string
int mgString::compareN(
  int len,                            // length to compare
  const WCHAR* str) const
{ 
  if (str == NULL)
    return -1;  // no match

  int posn = 0;
  for (int i = 0; i < len; i++)
  {
    WCHAR otherChar = str[i];
    int utfCount;
    WCHAR ourChar = (WCHAR) fromUTF8(m_data+posn, utfCount);
    posn += utfCount;

    if (otherChar == L'\0')
      return ourChar;
    else if (ourChar == L'\0')
      return -otherChar;
    else if (ourChar != otherChar)
      return ourChar - otherChar;
  }
  return 0;  // count ran out
}

//-----------------------------------------------------------
// compare to a string
BOOL mgString::equals(
  const char* str) const
{ 
  if (str == NULL)
    return m_dataLen == 0;
  return strcmp(m_data, str) == 0;
}

//-----------------------------------------------------------
// compare to a string
BOOL mgString::equals(
  const WCHAR* str) const
{ 
  if (str == NULL)
    return m_dataLen == 0;
  return compareN(m_dataLen, str) == 0;
}

//-----------------------------------------------------------
// compare ignore case
BOOL mgString::equalsIgnoreCase(
  const char* str) const
{
  if (str == NULL)
    return m_dataLen == 0;
  return _stricmp(m_data, str) == 0;
}

//-----------------------------------------------------------
// compare ignore case
BOOL mgString::equalsIgnoreCase(
  const WCHAR* str) const
{
  if (str == NULL)
    return m_dataLen == 0;
  int posn = 0;
  for (int i = 0; ; i++)
  {
    WCHAR otherChar = str[i];
    int utfCount;
    WCHAR ourChar = (WCHAR) fromUTF8(m_data+posn, utfCount);
    posn += utfCount;

    if (otherChar == L'\0')
      return ourChar == L'\0';
    else if (ourChar == L'\0')
      return otherChar == L'\0';
    else if (towlower(ourChar) != towlower(otherChar))
      return false;
  }
}

//-----------------------------------------------------------
// return true if starts with string
BOOL mgString::startsWith(
  const char* str) const
{
  if (str == NULL)
    return true;
  int len = (int) strlen(str);
  return strncmp(m_data, str, len) == 0;
}    

//-----------------------------------------------------------
// return true if starts with string
BOOL mgString::startsWith(
  const WCHAR* str) const
{
  if (str == NULL)
    return true;
  int len = (int) wcslen(str);
  int posn = 0;
  for (int i = 0; i < len; i++)
  {
    WCHAR otherChar = str[i];
    int utfCount;
    WCHAR ourChar = (WCHAR) fromUTF8(m_data+posn, utfCount);
    posn += utfCount;

    if (otherChar == L'\0')
      return false;
    else if (ourChar == L'\0')
      return false;
    else if (ourChar != otherChar)
      return false;
  }
  return true;
}    

//-----------------------------------------------------------
// return true if ends with string
BOOL mgString::endsWith(
  const char* str) const
{
  if (str == NULL)
    return true;
  int len = (int) strlen(str);
  if (len > m_dataLen)
    return false;

  return strncmp(m_data+m_dataLen-len, str, len) == 0;
}    

//-----------------------------------------------------------
// return true if ends with string
BOOL mgString::endsWith(
  const WCHAR* string) const
{
  // =-= implement
  return false;
}

//-----------------------------------------------------------
// find character
int mgString::find(
  int posn,                 // letter position
  char c) const
{
  while (posn < m_dataLen)
  {
    if (m_data[posn] == c)
      return posn;
    posn++;
  }
  return -1;  // not found
}

//-----------------------------------------------------------
// find character
int mgString::find(
  int posn,                 // letter position
  WCHAR c) const
{
  while (posn < m_dataLen)
  {
    int utfCount;
    WCHAR strChar = (WCHAR) fromUTF8(m_data + posn, utfCount);
    if (strChar == c)
      return posn;
    posn += utfCount;
  }
  return -1;  // not found
}

//-----------------------------------------------------------
// find substring
int mgString::find(
  int posn,                 // letter position
  const char* str,
  int len) const
{
  if (str == NULL)
    return -1;
  if (len == -1)
    len = (int) strlen(str);

  // check every position for match
  for (; posn <= m_dataLen-len; posn++)
  {
    if (strncmp(m_data+posn, str, len) == 0)
      return posn;
  }
  return -1;
}
  
//-----------------------------------------------------------
// find substring
int mgString::find(
  int posn,                 // letter position
  const WCHAR* str,
  int len) const
{
  // =-= compare wide version of string with converted wide version of this?
  return -1;
}
  
//-----------------------------------------------------------
// find last occurance of character
int mgString::reverseFind(
  int posn,                     // letter position
  char c) const
{
  posn = min(posn, m_dataLen);
  while (posn > 0)
  {
    posn--;
    if (m_data[posn] == c)
      return posn;
  }
  return -1;  // not found
}

//-----------------------------------------------------------
// find last occurance of character
int mgString::reverseFind(
  int posn,                     // letter position
  WCHAR c) const
{
  posn = min(posn, m_dataLen);
  while (posn > 0)
  {
    posn--;
    if (m_data[posn] == c)
      return posn;
  }
  return -1;  // not found
}

//-----------------------------------------------------------
// find last occurance of substring
int mgString::reverseFind(
  int posn,                     // letter position
  const char* str,
  int len) const
{
  if (str == NULL)
    return -1;
  if (len == -1)
    len = (int) strlen(str);

  // check every position for match
  for (int i = m_dataLen-len; i >= 0; i--)
  {
    if (strncmp(m_data+i, str, len) == 0)
      return i;
  }
  return -1;
}
  
//-----------------------------------------------------------
// find last occurance of substring
int mgString::reverseFind(
  int posn,                     // letter position
  const WCHAR* str,
  int len) const
{
  return -1;
}
  
//-----------------------------------------------------------
// remove leading blanks
void mgString::trimLeft()
{
  // find first non-blank char.
  int i;
  for (i = 0; i < m_dataLen; i++)
  {
    char c = m_data[i];
    // if a utf char
    if ((c & 0x80) != 0)
      break;
    if (!isspace(c))
      break;
  }

  if (i > 0)
  {
    memmove(m_data, m_data+i, m_dataLen-i);
    m_dataLen -= i;
    m_data[m_dataLen] = '\0';
  }
}

//-----------------------------------------------------------
// remove trailing blanks
void mgString::trimRight()
{
  // find last non-blank char.
  int i;
  for (i = m_dataLen-1; i >= 0; i--)
  {
    char c = m_data[i];
    // if a utf char
    if ((c & 0x80) != 0)
      break;
    if (!isspace(c))
      break;
  }

  m_dataLen = i+1;
  m_data[m_dataLen] = '\0';
}

//-----------------------------------------------------------
// return true if char posn is a space
BOOL mgString::isSpace(
  int posn) const
{
  // =-= convert these to wide chars first?
  char c = m_data[posn];
  // if a utf char
  if ((c & 0x80) != 0)
    return false;
  return isspace(c) != 0;
}

//-----------------------------------------------------------
// return true if char posn is a digit
BOOL mgString::isDigit(
  int posn) const
{
  // =-= convert these to wide chars first?
  char c = m_data[posn];
  // if a utf char
  if ((c & 0x80) != 0)
    return false;
  return isdigit(c) != 0;
}

//-----------------------------------------------------------
// return true if char posn is a hex digit
BOOL mgString::isHexDigit(
  int posn) const
{
  // =-= convert these to wide chars first?
  char c = m_data[posn];
  // if a utf char
  if ((c & 0x80) != 0)
    return false;
  return isxdigit(c) != 0;
}

//-----------------------------------------------------------
// return true if char posn is alpha numeric
BOOL mgString::isAlphaNumeric(
  int posn) const
{
  // =-= convert these to wide chars first?
  char c = m_data[posn];
  // if a utf char
  if ((c & 0x80) != 0)
    return false;
  return isalnum(c) != 0;
}

//-----------------------------------------------------------
// convert to lower case
void mgString::makeLower()
{
  for (int i = 0; i < m_dataLen; i++)
  {
    char c = m_data[i];
    // if not a utf char
    if ((c & 0x80) == 0)
      m_data[i] = (char) tolower(c);
  }
}


//-----------------------------------------------------------
// delete chars
void mgString::deleteAt(
  int from,                  // starting position
  int len)                   // legnth
{
  from = max(0, min(m_dataLen, from));
  len = min(m_dataLen - from, len);
  if (len <= 0)
    return;

  memmove(m_data+from, m_data+from+len, m_dataLen-(from+len));
  m_dataLen -= len;
  m_data[m_dataLen] = '\0';
}

//-----------------------------------------------------------
// insert into string
void mgString::insertAt(
  int posn,                   // where to insert
  const char* str,            // string to insert
  int len)                    // length of string
{
  if (str == NULL)
    return;
  if (posn < 0 || posn > m_dataLen)
    return;

  if (len == -1)
    len = (int) strlen(str);

  if (m_dataLen + len > m_allocLen)
    grow(m_dataLen + len);

  memmove(m_data+posn+len, m_data+posn, m_dataLen - posn);
  memcpy(m_data+posn, str, len);
  m_dataLen += len;
  m_data[m_dataLen] = '\0';
}

//-----------------------------------------------------------
// replace a letter 
int mgString::setLetter(
  int posn,                   // letter offset
  const char* letter,         // letter UTF8 string
  int len)                    // length of string
{
  if (letter == NULL)
    return -1;
  if (posn < 0 || posn >= m_dataLen)
    return -1;

  if (len == -1)
    len = (int) strlen(letter);

  // get length of letter
  int next = nextLetter(posn);

  deleteAt(posn, next-posn);
  insertAt(posn, letter, len);

  next = posn + len;
  return next;
}

//-----------------------------------------------------------
// replace a letter, return next position
int mgString::setLetter(
  int posn,                   // letter offset
  const int* chars,           // unicode characters
  int len)                    // count of characters
{
  if (chars == NULL)
    return -1;
  if (posn < 0 || posn >= m_dataLen)
    return -1;

  if (len > MG_MAX_LETTER_CHARS)
    return -1;  // throw exception

  // get length of letter at position
  int next = nextLetter(posn);

  deleteAt(posn, next-posn);

  char letter[MG_MAX_LETTER];
  int lposn = 0;
  for (int i = 0; i < len; i++)
  {
    int utfLen;
    toUTF8(letter+lposn, utfLen, chars[i]);
    lposn += utfLen;
  }
  letter[lposn] = '\0';
  insertAt(posn, letter, lposn);

  next = posn + lposn;
  return next;
}

//-----------------------------------------------------------
// delete letters
void mgString::deleteLettersAt(
  int posn,                   // starting letter offset
  int len)                    // count of letters
{
  int start = posn;
  for (int i = 0; i < len; i++)
  {
    posn = nextLetter(posn);
    if (posn == m_dataLen)
      break;
  }
  deleteAt(start, posn-start);
}

  
//-----------------------------------------------------------
// extract substring
void mgString::substring(
  mgString& target,
  int start,
  int len) const
{
  target.empty();
  if (start < 0 || len < 0 || start+len > m_dataLen)
    return;  // nothing to do
  target.write(m_data+start, len);
}

//-----------------------------------------------------------
// extract substring
void mgString::substring(
  mgString& target,
  int start) const
{
  target.empty();
  if (start < 0 || start >= m_dataLen)
    return;
  target.write(m_data+start, m_dataLen-start); 
}

//-----------------------------------------------------------------------------
// get next token, return ending position
int mgString::getToken(
  int posn,
  const char* delims,
  mgString& token) const
{
  token.empty();

  // skip leading delims
  while (posn < m_dataLen)
  {
    char c = m_data[posn];
    // if a utf char
    if ((c & 0x80) != 0)
      break;
    if (strchr(delims, c) == NULL)
      break;
    posn++;
  }

  // read chars until first delim or eos
  while (posn < m_dataLen)
  {
    char c = m_data[posn];
    // utf chars are not delims
    if ((c & 0x80) == 0 && strchr(delims, c) != NULL)
      break;
    token += c;
    posn++;
  }

  return posn;
}

//-----------------------------------------------------------
// scan string and extract items
int mgString::scan(
  const char* fmt,
  ...) const
{
  va_list args;
  va_start(args, fmt);
  return 0;
}

//-----------------------------------------------------------
// scan string and extract items
int mgString::scan(
  const WCHAR* fmt,
  ...) const
{
  return 0;
}

//-----------------------------------------------------------
// format string to buffer
void mgString::format(
  const char* fmt, 
  ...)
{
  va_list args;
  va_start(args, fmt);

#ifdef WIN32
  int len = _vscprintf(fmt, args);
#endif
#if defined(__APPLE__) || defined(__unix__) || defined(EMSCRIPTEN)
  va_list copy;
  va_copy(copy, args);
  int len = vsnprintf(NULL, 0, fmt, copy);
#endif

  if (len > m_allocLen)
    grow(len);
  vsprintf(m_data, fmt, args);
  m_dataLen = len;
  m_data[m_dataLen] = '\0';
}
    
//-----------------------------------------------------------
// format string to buffer
void mgString::formatV(
  const char* fmt, 
  va_list args)
{
#ifdef WIN32
  int len = _vscprintf(fmt, args);
#endif
#if defined(__APPLE__) || defined(__unix__) || defined(EMSCRIPTEN)
  va_list copy;
  va_copy(copy, args);
  int len = vsnprintf(NULL, 0, fmt, copy);
#endif

  if (len > m_allocLen)
    grow(len);
  int written = vsprintf(m_data, fmt, args);
  m_dataLen = written;
  m_data[m_dataLen] = '\0';
}

//-----------------------------------------------------------
// format string to buffer
void mgString::format(
  const WCHAR* fmt, 
  ...)
{
  // windows: need to allocate wide char result, convert to UTF8
  // linux: need version of printf that takes wide format string.
  // mac: ?
}
    
//-----------------------------------------------------------
// format string to buffer
void mgString::formatV(
  const WCHAR* fmt, 
  va_list args)
{
  // windows: need to allocate wide char result, convert to UTF8
  // linux: need version of printf that takes wide format string.
  // mac: ?
}

//-----------------------------------------------------------
// convert a single char from UTF8 to WCHAR
int mgString::fromUTF8(
  const char* source,
  int &charCount)
{
  int utfBits = 0;
  int utfCount = 0;
  charCount = 0;
  while (true)
  {
    int c = 0xFF & source[charCount];
    charCount++;
    if (c == '\0')
      return 0;

    // if not UTF-8 byte
    if ((c & 0x80) == 0)
      return c;

    // if a continuation byte
    if ((c & 0xC0) == 0x80)
    {
      // continuation byte
      utfBits = (utfBits << 6) | (0x3F & c);
      utfCount--;

      if (utfCount < 0)
        return c;  //  no start byte, or used too many.  ignore

      // if we've read a character
      if (utfCount == 0)
        return utfBits;
    }

    // if 2-byte start code
    else if ((c & 0xE0) == 0xC0)
    {
      utfCount = 1;
      utfBits = 0x1F & c;
    }
    // if 3-byte start code
    else if ((c & 0xF0) == 0xE0)
    {
      utfCount = 2;
      utfBits = 0x0F & c;
    }
    // if 4-byte start code
    else if ((c & 0xF8) == 0xF0)
    {
      utfCount = 3;
      utfBits = 0x07 & c;
    }
    // if 5-byte start code
    else if ((c & 0xFC) == 0xF8)
    {
      utfCount = 4;
      utfBits = 0x03 & c;
    }
    // if 6-byte start code
    else if ((c & 0xFE) == 0xFC)
    {
      utfCount = 5;
      utfBits = 0x01 & c;
    }
    else return c;  // invalid UTF-8 byte.  ignore
  }
}

//-----------------------------------------------------------
// return length of string in WCHAR
int mgString::lenWCHAR(
  const char* source,
  int len)
{
  int wideLen = 0;
  int utfCount = 0;
  for (int i = 0; i < len; i++)
  {
    int c = 0xFF & source[i];
    if (c == '\0')
      break;

    // if not UTF-8 byte
    if ((c & 0x80) == 0)
    {
      wideLen++;
      continue;
    }

    // if a continuation byte
    if ((c & 0xC0) == 0x80)
    {
      utfCount--;

      if (utfCount < 0)
      {
        // bad UTF-8 byte -- too many continuation.  copy and reset
        wideLen++;
        utfCount = 0;
        continue;
      }

      // if we've read a character
      if (utfCount == 0)
        wideLen++;
    }

    // if 2-byte start code
    else if ((c & 0xE0) == 0xC0)
      utfCount = 1;

    // if 3-byte start code
    else if ((c & 0xF0) == 0xE0)
      utfCount = 2;

    // if 4-byte start code
    else if ((c & 0xF8) == 0xF0)
      utfCount = 3;

    // if 5-byte start code
    else if ((c & 0xFC) == 0xF8)
      utfCount = 4;

    // if 6-byte start code
    else if ((c & 0xFE) == 0xFC)
      utfCount = 5;

    else 
    {
      // bad UTF-8 byte -- does not match start char or continuation
      // copy and reset
      wideLen++;
      utfCount = 0;
    }
  }
  return wideLen;
}

//-----------------------------------------------------------
// convert to Unicode string.  caller owns storage
void mgString::toWCHAR(
  const char* source,
  WCHAR*& target,
  int& len)
{
  int sourceLen = (int) strlen(source);
  len = lenWCHAR(source, sourceLen);
  target = new WCHAR[len+1];

  int posn = 0;
  for (int i = 0; i < len; i++)
  {
    int utfCount;
    target[i] = (WCHAR) fromUTF8(source+posn, utfCount);
    posn += utfCount;
  }
  target[len] = L'\0';
}

    
    
