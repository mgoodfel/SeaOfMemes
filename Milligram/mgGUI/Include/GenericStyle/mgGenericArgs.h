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

#ifndef MGGENERICARGS_H
#define MGGENERICARGS_H

/*
  Hold small list of arguments identified by string keys.  Use mgMapStringToString
  for large tables with persistent values.  This just maps pointers to pointers
  using an array.  All keys and values are assumed allocated and freed outside
  the GenericArgs instance.
*/

const int MG_GENERIC_ARGS_SMALL_SIZE = 32;

class mgGenericArgs
{
public:
  // constructor
  mgGenericArgs();

  // destructor
  virtual ~mgGenericArgs();

  // set a key
  void setAt(
    const char* key,
    const char* value);

  // find a key
  BOOL lookup(
    const char* key,
    mgString& value) const;

protected:
  const char* m_smallArgs[MG_GENERIC_ARGS_SMALL_SIZE*2];
  const char** m_args;
  int m_count;
  int m_size;
};

#endif
