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

#include "GUI/mgToolkit.h"
#include "GenericStyle/mgGenericLabel.h"
#include "GenericStyle/mgGenericStyle.h"
#include "GenericStyle/mgLabelFace.h"

//--------------------------------------------------------------
// constructor
mgGenericLabel::mgGenericLabel(
  mgControl* parent)
: mgLabelControl(parent)
{
  m_maxLabel = NULL;

  // get the style
  mgGenericStyle* style = (mgGenericStyle*) getStyle();
  if (style == NULL)
    throw new mgException("GenericLabel created with style=NULL");
  m_enabledFace = style->getFace(MG_GENERIC_LABEL_ENABLED);
  m_disabledFace = style->getFace(MG_GENERIC_LABEL_DISABLED);
}

//--------------------------------------------------------------
// destructor
mgGenericLabel::~mgGenericLabel()
{
  delete m_maxLabel;
  m_maxLabel = NULL;
}

//--------------------------------------------------------------
// set the label
void mgGenericLabel::setLabel(
  const char* text)
{
  m_label = text;
  m_args.setAt("label", (const char*) m_label);
}

//--------------------------------------------------------------
// set the preferred size from a string
void mgGenericLabel::setMaxLabel(
  const char* text)
{
  if (m_maxLabel == NULL)
    m_maxLabel = new mgString(text);
  else *m_maxLabel = text;
}

//--------------------------------------------------------------
// return minimum size
void mgGenericLabel::minimumSize(
  mgDimension& size)
{
  preferredSize(size);
}  

//--------------------------------------------------------------
// return preferred size
void mgGenericLabel::preferredSize(
  mgDimension& size)
{
  if (m_maxLabel != NULL)
    m_args.setAt("label", (const char*) *m_maxLabel);
  else m_args.setAt("label", (const char*) m_label);

  mgContext* gc = newContext();
  m_enabledFace->preferredSize(gc, m_args, size);
  delete gc;
}  

//--------------------------------------------------------------
// paint content of control
void mgGenericLabel::paint(
  mgContext* gc)
{
  mgDimension size(m_width, m_height);
  // draw the face with the label string
  m_args.setAt("label", (const char*) m_label);

  if (getEnabled())
    m_enabledFace->paint(gc, m_args, size);
  else m_disabledFace->paint(gc, m_args, size);
}  

