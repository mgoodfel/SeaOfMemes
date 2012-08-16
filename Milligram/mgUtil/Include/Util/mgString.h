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
#ifndef MGSTRING_H
#define MGSTRING_H

// A variable-length string class.  Wherever arguments are "const char*", these
// are assumed to be UTF-8 format strings.  WCHAR arguments should be Unicode 
// string constants, which are handled differently under each platform.

// The (const char*) cast returns UTF-8 strings.  The toWCHAR method returns
// a (const wchar_t*), which is only really useful under Windows, where system
// calls expect a string in this format.

// Unfortunately, C++ automatically generates a cast to (const char*) when
// you code string[i] -- it becomes ((const char*) string)[i].  This allows
// the caller to iterate through characters, which is not meaningful in
// a UTF-8 string.

// A 'letter' in the class is one or more Unicode characters (a main character
// and other combining characters) that together form a single symbol in a word.
// Since the internal format is UTF-8, a single letter may be many bytes.

// MG_MAX_LETTER is the maximum number of bytes returned for a single letter.
#define MG_MAX_LETTER_CHARS  8
#define MG_MAX_LETTER  49    // 8 UTF-8 codes (max 6 bytes), plus ending null

class mgString
{
public:
  // constructor
  mgString();

  // constructor
  mgString(
    const char* str);

  // constructor
  mgString(
    const WCHAR* str);

  // constructor
  mgString(
    const char* str, 
    int len);

  // constructor
  mgString(
    const WCHAR* str, 
    int len);

  // constructor
  mgString(
    const mgString& other);

  // destructor 
  virtual ~mgString();

  // allocation increased by growby when string full
  void setGrowBy(
    int len);

  // set memory allocated for string
  void setAllocLength(
    int len);

  // assign to a character
  mgString& operator=(
    char c);

  // assign to a character
  mgString& operator=(
    WCHAR c);

  // assign to a string
  mgString& operator=(
    const char* str);

  // assign to a string
  mgString& operator=(
    const WCHAR* str);

  // assign to a string
  mgString& operator=(
    const mgString& other);

  // append char to string
  mgString& operator+=(
    char c);

  // append char to string
  mgString& operator+=(
    WCHAR c);

  // append to string
  mgString& operator+=(
    const char* str);

  // append to string
  mgString& operator+=(
    const WCHAR* str);

  // append to string
  mgString& operator+=(
    const mgString& buffer);

  // append to string
  mgString& write(
    const char* data,          // data to append
    int len);                  // length of data

  // append to string
  mgString& write(
    const WCHAR* data,          // data to append
    int len);                  // length of data

  // return next letter in a string.  This may be more than a
  // single Unicode character, if combining characters were supported.
  int nextLetter(
    int posn,
    char* letter = NULL) const;

  // return previous letter in a string.  This may be more than a
  // single Unicode character, if combining characters were supported.
  int prevLetter(
    int posn,
    char* letter = NULL) const;

  // return number of 'letters' in a string.  Multiple unicode characters
  // can make up a letter.
  int countLetters() const;

  // reset the buffer to length 0
  void empty()
  { 
    m_dataLen = 0;
    m_data[m_dataLen] = '\0';
  }

  // return the length
  int length() const
  { 
    return m_dataLen; 
  } 

  // return true if empty
  BOOL isEmpty() const
  { 
    return m_dataLen == 0; 
  } 


  // return buffer as const UTF-8 string
  operator const char* () const
  { 
    return (const char*) m_data; 
  } 

  // return Unicode string.  caller owns storage
  void toWCHAR(
    WCHAR*& target,
    int& len) const;
  

  // compare N letters of a string
  int compareN(
    int len,                            // length to compare
    const char* str) const;             // comparison string

  // compare N letters of a string
  int compareN(
    int len,                            // length to compare
    const WCHAR* str) const;            // comparison string

  // compare to a string
  BOOL equals(
    const char* str) const;             // comparison string

  // compare to a string
  BOOL equals(
    const WCHAR* str) const;            // comparison string

  // compare ignore case
  BOOL equalsIgnoreCase(
    const char* str) const;             // comparison string

  // compare ignore case
  BOOL equalsIgnoreCase(
    const WCHAR* str) const;            // comparison string

  // find character
  int find(
    int posn,                           // starting letter position
    char c) const;                      // char to find
    
  // find character
  int find(
    int posn,                           // starting letter position
    WCHAR c) const;                     // char to find
    
  // find substring
  int find(
    int posn,                           // starting letter position
    const char* str,                    // string to find
    int len = -1) const;                // length of string (bytes)
    
  // find substring
  int find(
    int posn,                           // starting letter position
    const WCHAR* str,                   // string to find
    int len = -1) const;                // length of string (unicode chars)
    
  // find last occurance of char
  int reverseFind(
    int posn,                           // ending letter position
    char c) const;                      // char to find
    
  // find last occurance of char
  int reverseFind(
    int posn,                           // ending letter position
    WCHAR c) const;                     // char to find
    
  // find last occurance of substring
  int reverseFind(
    int posn,                           // ending letter position
    const char* str,                    // string to find
    int len = -1) const;                // length (bytes)
    
  // find last occurance of substring
  int reverseFind(
    int posn,                           // ending letter position
    const WCHAR* str,                   // string to find
    int len = -1) const;                // length (unicode chars)
    
  // return true if starts with string
  BOOL startsWith(
    const char* string) const;

  // return true if starts with string
  BOOL startsWith(
    const WCHAR* string) const;

  // return true if ends with string
  BOOL endsWith(
    const char* string) const;
        
  // return true if ends with string
  BOOL endsWith(
    const WCHAR* string) const;
        
  // remove leading white space
  void trimLeft();

  // remove trailing white space
  void trimRight();

  // remove leading and trailing white space
  void trim()
  {
    trimRight();
    trimLeft();
  }

  // return true if char posn is a space
  BOOL isSpace(
    int posn) const;

  // return true if char posn is a digit
  BOOL isDigit(
    int posn) const;

  // return true if char posn is a hex digit
  BOOL isHexDigit(
    int posn) const;

  // return true if char posn is alpha numeric
  BOOL isAlphaNumeric(
    int posn) const;

  // convert to lower case
  void makeLower();

  // delete text
  void deleteAt(
    int posn,                   // starting letter offset
    int len);                   // count of letters

  // insert into string
  void insertAt(
    int posn,                   // letter offset
    const char* str,            // string to insert
    int len = -1);              // length of string

  // replace a letter, return next position
  int setLetter(
    int posn,                   // letter offset
    const char* letter,         // letter codes
    int len = -1);              // length of string

  // replace a letter, return next position
  int setLetter(
    int posn,                   // letter offset
    const int* chars,           // unicode characters
    int len);                   // count of characters

  // delete letters
  void deleteLettersAt(
    int posn,                   // starting letter offset
    int len);                   // count of letters

  // extract substring
  void substring(
    mgString& target,
    int posn,                   // letter offset
    int len) const;             // length (bytes)
    
  // extract from position to end of string
  void substring(
    mgString& target,
    int posn) const;

  // get next token, return ending position
  int getToken(
    int posn,
    const char* delims,
    mgString& token) const;

  // scan string and extract items
  int scan(
    const char* fmt,
    ...) const;

  // scan string and extract items
  int scan(
    const WCHAR* fmt,
    ...) const;

  // format string to buffer
  void format(
    const char* fmt, 
    ...);
    
  // format wide string to buffer
  void format(
    const WCHAR* fmt, 
    ...);
    
  // format string to buffer, variable args
  void formatV(
    const char* fmt, 
    va_list args);
    
  // format wide string to buffer, variable args
  void formatV(
    const WCHAR* fmt, 
    va_list args);
    
  // convert UTF8 string to unicode
  static int fromUTF8(
    const char* source,
    int &utfCount);

  // return length of string in WCHAR
  static int lenWCHAR(
    const char* source,
    int len);

  // return Unicode string.  caller owns storage
  static void toWCHAR(
    const char* source,
    WCHAR*& target,
    int& len);
  
protected:
  int m_allocLen;               // allocation length
  int m_dataLen;                // data length
  int m_growBy;                 // size of increase in alloc
  char* m_data;                 // data to add 
  char m_shortData[64];         // initial data

  // shared initialization
  void init();

  // grow buffer to hold new length
  void grow(
    int newLen);    // length needed

  // return length of UTF-8 encoded char
  static int lenUTF8(
    int c);

  // return length of UTF-8 encoded string
  static int lenUTF8(
    const WCHAR* source,
    int len);

  // convert to UTF8 format
  static void toUTF8(
    char* target,
    int& len,
    int source);
};

#endif
