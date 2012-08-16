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

#include "GenericStyle/mgGenericArgs.h"

//--------------------------------------------------------------
// constructor
mgGenericArgs::mgGenericArgs()
{
  m_args = m_smallArgs;
  m_size = MG_GENERIC_ARGS_SMALL_SIZE;
  m_count = 0;
}

//--------------------------------------------------------------
// destructor
mgGenericArgs::~mgGenericArgs()
{
  if (m_args != m_smallArgs)
    delete m_args;
  m_args = NULL;
}

//--------------------------------------------------------------
// set a key
void mgGenericArgs::setAt(
  const char* key,
  const char* value)
{
  // search for key
  for (int i = 0; i < m_count; i++)
  {
    if (_stricmp(key, m_args[i*2]) == 0)
    {
      // replace existing value
      m_args[i*2+1] = value;
      return;
    } 
  }

  // allocate more space if required
  if (m_count >= m_size)
  {
    m_size *= 2;
    const char**newArgs = new const char*[m_size];
    memcpy(newArgs, m_args, m_size*2*sizeof(const char*));
    if (m_args != m_smallArgs)
      delete m_args;
    m_args = newArgs;
  }

  // add new value
  m_args[2*m_count] = key;
  m_args[2*m_count+1] = value;
  m_count++;
}

//--------------------------------------------------------------
// find a key
BOOL mgGenericArgs::lookup(
  const char* key,
  mgString& value) const
{
  // search for key
  for (int i = 0; i < m_count; i++)
  {
    if (_stricmp(key, m_args[i*2]) == 0)
    {
      // return value
      value = m_args[i*2+1];
      return true;
    } 
  }
  return false;
}
