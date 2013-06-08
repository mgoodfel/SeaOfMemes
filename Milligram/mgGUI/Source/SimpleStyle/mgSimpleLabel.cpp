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

#include "GUI/mgToolkit.h"
#include "SimpleStyle/mgSimpleLabel.h"
#include "SimpleStyle/mgSimpleStyle.h"

//--------------------------------------------------------------
// constructor
mgSimpleLabel::mgSimpleLabel(
  mgControl* parent,
  const char* cntlName)
: mgLabelControl(parent, cntlName)
{
  m_maxLabel = NULL;

  mgStyle* style = getStyle();

  // assume style has set all our format elements
  style->getFontAttr(MG_STYLE_LABEL, getName(), "font", m_font);
  style->getColorAttr(MG_STYLE_LABEL, getName(), "textColor", m_color);
  style->getColorAttr(MG_STYLE_LABEL, getName(), "disTextColor", m_disColor);
}

//--------------------------------------------------------------
// destructor
mgSimpleLabel::~mgSimpleLabel()
{
  delete m_maxLabel;
  m_maxLabel = NULL;
}

//--------------------------------------------------------------
// set the label
void mgSimpleLabel::setLabel(
  const char* text)
{
  if (m_label.equals(text))
    return;  // no change

  m_label = text;
  damage();
}

//--------------------------------------------------------------
// set the preferred size from a string
void mgSimpleLabel::setMaxLabel(
  const char* text)
{
  if (m_maxLabel == NULL)
    m_maxLabel = new mgString(text);
  else *m_maxLabel = text;
}

//--------------------------------------------------------------
// return minimum size
void mgSimpleLabel::minimumSize(
  mgDimension& size)
{
  preferredSize(size);
}  

//--------------------------------------------------------------
// return preferred size
void mgSimpleLabel::preferredSize(
  mgDimension& size)
{
  mgString label(m_label);
  if (m_maxLabel != NULL)
    label = *m_maxLabel;

  size.m_width = m_font->stringWidth(label, label.length());
  size.m_height = m_font->getHeight();
}  

//--------------------------------------------------------------
// paint content of control
void mgSimpleLabel::paint(
  mgContext* gc)
{
  mgDimension size;
  getSize(size);

  if (getEnabled())
    gc->setTextColor(m_color);
  else gc->setTextColor(m_disColor);

  gc->setFont(m_font);

  gc->setAlphaMode(MG_ALPHA_MERGE);
  gc->drawString(m_label, m_label.length(), 0, m_font->getAscent());
}  

