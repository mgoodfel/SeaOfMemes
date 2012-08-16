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
#include "GenericStyle/mgGenericStyle.h"
#include "GenericStyle/mgGenericLabel.h"

#include "GenericStyle/mgLabelFace.h"
#include "GenericStyle/mgButtonFace.h"

//--------------------------------------------------------------
// constructor
mgGenericStyle::mgGenericStyle(
  mgSurface* surface,
  const char* settingsFile)
{
  m_surface = surface;

  m_labelDisabled = new mgLabelFace(this, false);
  m_labelEnabled = new mgLabelFace(this, true);

  m_buttonDisabled = new mgButtonFace(this, MG_GENERIC_BUTTON_DISABLED);
  m_buttonUp = new mgButtonFace(this, MG_GENERIC_BUTTON_UP);
  m_buttonHover = new mgButtonFace(this, MG_GENERIC_BUTTON_HOVER);
  m_buttonDown = new mgButtonFace(this, MG_GENERIC_BUTTON_DOWN);

  setAttr(MG_STYLE_ALL, "font", m_surface->createFont("sansation-10-i"));
}

//--------------------------------------------------------------
// destructor
mgGenericStyle::~mgGenericStyle()
{
  delete m_labelDisabled;
  m_labelDisabled = NULL;
  delete m_labelEnabled;
  m_labelEnabled = NULL;

  delete m_buttonDisabled;
  m_buttonDisabled = NULL;
  delete m_buttonUp;
  m_buttonUp = NULL;
  delete m_buttonHover;
  m_buttonHover = NULL;
  delete m_buttonDown;
  m_buttonDown = NULL;
}

//--------------------------------------------------------------
// get a control face
mgGenericFace* mgGenericStyle::getFace(
  const char* name)
{
  if (_stricmp(name, MG_GENERIC_LABEL_ENABLED) == 0)
    return m_labelEnabled;
  if (_stricmp(name, MG_GENERIC_LABEL_DISABLED) == 0)
    return m_labelDisabled;

  if (_stricmp(name, MG_GENERIC_BUTTON_DISABLED) == 0)
    return m_buttonDisabled;
  if (_stricmp(name, MG_GENERIC_BUTTON_UP) == 0)
    return m_buttonUp;
  if (_stricmp(name, MG_GENERIC_BUTTON_HOVER) == 0)
    return m_buttonHover;
  if (_stricmp(name, MG_GENERIC_BUTTON_DOWN) == 0)
    return m_buttonDown;

  throw new mgException("GenericStyle does not support requested face %s", name);
}

//--------------------------------------------------------------
// create a label control
mgLabelControl* mgGenericStyle::createLabel(
  mgControl* parent,
  const char* name, 
  const char* label)
{
  mgGenericLabel* cntl = new mgGenericLabel(parent);
  cntl->setName(name);
  cntl->setLabel(label);

  return cntl;
}

//--------------------------------------------------------------
// create a field control
mgFieldControl* mgGenericStyle::createField(
  mgControl* parent,
  const char* name)
{
  return NULL;
}

//--------------------------------------------------------------
// create a button control
mgButtonControl* mgGenericStyle::createButton(
  mgControl* parent,
  const char* name, 
  const char* label,
  const char* iconName)
{
  mgGenericButton* cntl = new mgGenericButton(parent);
  cntl->setName(name);
  cntl->setLabel(label);
  cntl->setIcon(iconName);

  return cntl;
}

//--------------------------------------------------------------
// create a checkbox control
mgCheckboxControl* mgGenericStyle::createCheckbox(
  mgControl* parent,
  const char* name, 
  const char* label)
{
  return NULL;
/*
  mgGenericCheckbox* cntl = new mgGenericCheckbox(parent);
  cntl->setName(name);
  cntl->setLabel(label);

  return cntl;
*/
}

//--------------------------------------------------------------
// create a console control
mgConsoleControl* mgGenericStyle::createConsole(
  mgControl* parent,
  const char* name)
{
  return NULL;
}

//--------------------------------------------------------------
// create a stack control
mgStackControl* mgGenericStyle::createStack(
  mgControl* parent,
  const char* name)
{
  return NULL;
}

#ifdef WORKED
//--------------------------------------------------------------
// return a color
void mgGenericStyle::getColor(
  const char* name,
  mgColor& color)
{
  if (_stricmp(name, MG_GENERIC_LABEL_DISABLED) == 0)
    color = mgColor("gray");
  else if (_stricmp(name, MG_GENERIC_LABEL_ENABLED) == 0)
    color = mgColor("white");

  else if (_stricmp(name, MG_GENERIC_BUTTON_DISABLED) == 0)
    color = mgColor("gray");
  else if (_stricmp(name, MG_GENERIC_BUTTON_UP) == 0)
    color = mgColor("white");
  else if (_stricmp(name, MG_GENERIC_BUTTON_HOVER) == 0)
    color = mgColor("red");
  else if (_stricmp(name, MG_GENERIC_BUTTON_DOWN) == 0)
    color = mgColor("black");
}

//--------------------------------------------------------------
// return a frame resource
const mgFrame* mgGenericStyle::getFrame(
  const char* name)
{
  if (_stricmp(name, MG_GENERIC_LABEL_DISABLED) == 0)
    return NULL;
  else if (_stricmp(name, MG_GENERIC_LABEL_ENABLED) == 0)
    return NULL;

  else if (_stricmp(name, MG_GENERIC_BUTTON_DISABLED) == 0)
  {
    return mgRectFrame::createFrame(m_surface, mgFlatEdge, 2, mgColor("lightGray"), 
             mgSolidPaint::createPaint(m_surface, mgColor("gray")), 
             mgFrameMargin(10));
  }
  else if (_stricmp(name, MG_GENERIC_BUTTON_UP) == 0)
  {
    return mgRectFrame::createFrame(m_surface, mgRaisedEdge, 2, mgColor("lightGray"), 
             mgSolidPaint::createPaint(m_surface, mgColor(200, 200, 255)), 
             mgFrameMargin(10));
  }
  else if (_stricmp(name, MG_GENERIC_BUTTON_HOVER) == 0)
  {
    return mgRectFrame::createFrame(m_surface, mgFlatEdge, 2, mgColor("lightGray"), 
             mgSolidPaint::createPaint(m_surface, mgColor(100, 100, 128)), 
             mgFrameMargin(10));
  }
  else if (_stricmp(name, MG_GENERIC_BUTTON_DOWN) == 0)
  {
    return mgRectFrame::createFrame(m_surface, mgLoweredEdge, 2, mgColor("lightGray"), 
             mgSolidPaint::createPaint(m_surface, mgColor(255, 100, 128)), 
             mgFrameMargin(10));
  }

  return NULL;
}

#endif
