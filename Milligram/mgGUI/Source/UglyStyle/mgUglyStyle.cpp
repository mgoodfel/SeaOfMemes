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
#include "UglyStyle/mgUglyStyle.h"
#include "UglyStyle/mgUglyLabel.h"
#include "UglyStyle/mgUglyField.h"
#include "UglyStyle/mgUglyButton.h"
#include "UglyStyle/mgUglyCheckbox.h"
#include "UglyStyle/mgUglyConsole.h"
#include "UglyStyle/mgUglyStack.h"
#include "UglyStyle/mgUglyList.h"

//--------------------------------------------------------------
// constructor
mgUglyStyle::mgUglyStyle(
  mgSurface* surface)
{
  m_surface = surface;

  const mgFont* font = NULL;
  const mgPaint* paint = NULL;
  const mgFrame* frame = NULL;
  mgColor upColor, hoverColor, downColor, disColor;

  setAttr(MG_STYLE_ALL, "font", m_surface->createFont("Arial-10"));

  //--------- Label
  setAttr(MG_STYLE_LABEL, "textColor", mgColor("black"));
  setAttr(MG_STYLE_LABEL, "disTextColor", mgColor("gray"));

  //--------- Button
  paint = mgSolidPaint::createPaint(surface, 0, 192, 255);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_BUTTON, "upFrame", frame);
  setAttr(MG_STYLE_BUTTON, "upColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, 0, 128, 255);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_BUTTON, "hoverFrame", frame);
  setAttr(MG_STYLE_BUTTON, "hoverColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, 0, 64, 255);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_BUTTON, "downFrame", frame);
  setAttr(MG_STYLE_BUTTON, "downColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, 192, 192, 192);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_BUTTON, "disFrame", frame);
  setAttr(MG_STYLE_BUTTON, "disColor", mgColor("black"));

  //--------- Checkbox
  paint = mgSolidPaint::createPaint(surface, 0, 192, 255);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_CHECKBOX, "upFrame", frame);
  setAttr(MG_STYLE_CHECKBOX, "upColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, 0, 128, 255);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_CHECKBOX, "hoverFrame", frame);
  setAttr(MG_STYLE_CHECKBOX, "hoverColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, 0, 64, 255);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_CHECKBOX, "downFrame", frame);
  setAttr(MG_STYLE_CHECKBOX, "downColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, 192, 192, 192);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_CHECKBOX, "disFrame", frame);
  setAttr(MG_STYLE_CHECKBOX, "disColor", mgColor("black"));

  //--------- Console
  setAttr(MG_STYLE_CONSOLE, "font", m_surface->createFont("Courier-10"));
  paint = mgSolidPaint::createPaint(surface, 0, 128, 255);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(2)); 
  setAttr(MG_STYLE_CONSOLE, "frame", frame);
  setAttr(MG_STYLE_CONSOLE, "textColor", mgColor("black"));

  //--------- Field
  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_FIELD, "upFrame", frame);
  setAttr(MG_STYLE_FIELD, "upColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(220, 220, 255));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_FIELD, "hoverFrame", frame);
  setAttr(MG_STYLE_FIELD, "hoverColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(200, 200, 255));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_FIELD, "downFrame", frame);
  setAttr(MG_STYLE_FIELD, "downColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(192, 192, 192));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_FIELD, "disFrame", frame);
  setAttr(MG_STYLE_FIELD, "disColor", mgColor("black"));


  //--------- List
  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_LIST, "upFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("gray"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_LIST, "disFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_LIST, "itemUpFrame", frame);
  setAttr(MG_STYLE_LIST, "itemUpColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(220, 220, 255));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_LIST, "itemHoverFrame", frame);
  setAttr(MG_STYLE_LIST, "itemHoverColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(128, 128, 255));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_LIST, "itemDownFrame", frame);
  setAttr(MG_STYLE_LIST, "itemDownColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(225, 225, 225));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_LIST, "itemDisFrame", frame);
  setAttr(MG_STYLE_LIST, "itemDisColor", mgColor("gray"));

  //--------- Scrollbar
  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "upFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("gray"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "disFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "lineUpFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(220, 220, 255));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "lineHoverFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(128, 128, 255));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "lineDownFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_SCROLLBAR, "pageUpFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(220, 220, 255));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_SCROLLBAR, "pageHoverFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(128, 128, 255));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_SCROLLBAR, "pageDownFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(192, 192, 192));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "sliderUpFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(165, 165, 255));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "sliderHoverFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(128, 128, 255));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "sliderDownFrame", frame);

}

//--------------------------------------------------------------
// destructor
mgUglyStyle::~mgUglyStyle()
{
}

//--------------------------------------------------------------
// create a label control
mgLabelControl* mgUglyStyle::createLabel(
  mgControl* parent,
  const char* cntlName, 
  const char* label)
{
  mgUglyLabel* cntl = new mgUglyLabel(parent, cntlName);
  cntl->setLabel(label);

  return cntl;
}

//--------------------------------------------------------------
// create a field control
mgFieldControl* mgUglyStyle::createField(
  mgControl* parent,
  const char* cntlName)
{
  mgUglyField* cntl = new mgUglyField(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a button control
mgButtonControl* mgUglyStyle::createButton(
  mgControl* parent,
  const char* cntlName, 
  const char* label,
  const char* iconName)
{
  mgUglyButton* cntl = new mgUglyButton(parent, cntlName);
  cntl->setLabel(label);
  cntl->setIcon(iconName);

  return cntl;
}

//--------------------------------------------------------------
// create a checkbox control
mgCheckboxControl* mgUglyStyle::createCheckbox(
  mgControl* parent,
  const char* cntlName, 
  const char* label)
{
  mgUglyCheckbox* cntl = new mgUglyCheckbox(parent, cntlName);
  cntl->setLabel(label);

  return cntl;
}

//--------------------------------------------------------------
// create a console control
mgConsoleControl* mgUglyStyle::createConsole(
  mgControl* parent,
  const char* cntlName)
{
  mgUglyConsole* cntl = new mgUglyConsole(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a stack control
mgStackControl* mgUglyStyle::createStack(
  mgControl* parent,
  const char* cntlName)
{
  mgUglyStack* cntl = new mgUglyStack(parent, cntlName);

  return cntl;
}

