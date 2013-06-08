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
#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "Util/mgErrorMsg.h"

//--------------------------------------------------------------
// constructor
mgErrorMsg::mgErrorMsg(
  const char* msgId,
  const char* varNames,
  const char* format,
  va_list args)
{
  m_msgId = msgId;
  m_varNames = varNames;
  if (m_varNames.isEmpty())
    return;  // nothing more to do

  if (!m_varNames.endsWith(","))
    m_varNames += ",";

  // change to internal delimiter
  mgString delim(MG_ERROR_DELIM_STR);
  char letter[MG_MAX_LETTER];
  int posn = 0;
  while (posn < m_varNames.length())
  {
    int next = m_varNames.nextLetter(posn, letter);
    if (strcmp(letter, ",") == 0)
      next = m_varNames.setLetter(posn, delim);
    posn = next;
  }

  mgString delimFormat(format);
  if (!delimFormat.endsWith(","))
    delimFormat += ",";

  // change to internal delimiter
  posn = 0;
  while (posn < delimFormat.length())
  {
    int next = delimFormat.nextLetter(posn, letter);
    if (strcmp(letter, ",") == 0)
      next = delimFormat.setLetter(posn, delim);
    posn = next;
  }

  m_varValues.formatV(delimFormat, args);
}

//--------------------------------------------------------------
// constructor
mgErrorMsg::mgErrorMsg(
  const char* msgId,
  const char* varNames,
  const char* format,
  ...)
{
  m_msgId = msgId;
  m_varNames = varNames;
  if (m_varNames.isEmpty())
    return;  // nothing more to do

  if (!m_varNames.endsWith(","))
    m_varNames += ",";

  // change to internal delimiter
  mgString delim(MG_ERROR_DELIM_STR);
  char letter[MG_MAX_LETTER];
  int posn = 0;
  while (posn < m_varNames.length())
  {
    int next = m_varNames.nextLetter(posn, letter);
    if (strcmp(letter, ",") == 0)
      next = m_varNames.setLetter(posn, delim);
    posn = next;
  }

  mgString delimFormat(format);
  if (!delimFormat.endsWith(","))
    delimFormat += ",";

  // change to internal delimiter
  posn = 0;
  while (posn < delimFormat.length())
  {
    int next = delimFormat.nextLetter(posn, letter);
    if (strcmp(letter, ",") == 0)
      next = delimFormat.setLetter(posn, delim);
    posn = next;
  }

  va_list args;
  va_start(args, format);
  m_varValues.formatV(delimFormat, args);
}

//--------------------------------------------------------------
// destructor
mgErrorMsg::~mgErrorMsg()
{
}

//--------------------------------------------------------------
// add variables to message
void mgErrorMsg::addVars(
  const char* varNames,
  const char* format,
  ...)
{
  mgString newNames(varNames);
  if (!newNames.endsWith(","))
    newNames += ",";

  // change to internal delimiter
  mgString delim(MG_ERROR_DELIM_STR);
  char letter[MG_MAX_LETTER];
  int posn = 0;
  while (posn < newNames.length())
  {
    int next = newNames.nextLetter(posn, letter);
    if (strcmp(letter, ",") == 0)
      next = newNames.setLetter(posn, delim);
    posn = next;
  }
  m_varNames += newNames;

  mgString delimFormat(format);
  if (!delimFormat.endsWith(","))
    delimFormat += ",";

  // change to internal delimiter
  posn = 0;
  while (posn < delimFormat.length())
  {
    int next = delimFormat.nextLetter(posn, letter);
    if (strcmp(letter, ",") == 0)
      next = delimFormat.setLetter(posn, delim);
    posn = next;
  }

  va_list args;
  va_start(args, format);
  mgString newValues;
  newValues.formatV(delimFormat, args);

  m_varValues += newValues;
}
