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
#include "SimpleStyle/mgSimpleStyle.h"
#include "SimpleStyle/mgSimpleLabel.h"
#include "SimpleStyle/mgSimpleField.h"
#include "SimpleStyle/mgSimpleButton.h"
#include "SimpleStyle/mgSimpleCheckbox.h"
#include "SimpleStyle/mgSimpleList.h"
#include "SimpleStyle/mgSimpleConsole.h"
#include "SimpleStyle/mgSimpleStack.h"
#include "SimpleStyle/mgSimpleSplit.h"
#include "SimpleStyle/mgSimpleTabbed.h"
#include "SimpleStyle/mgSimpleScrollPane.h"
#include "SimpleStyle/mgSimpleDesktop.h"

//--------------------------------------------------------------
// constructor
mgSimpleStyle::mgSimpleStyle(
  mgSurface* surface)
{
  m_surface = surface;

  const mgFont* font = NULL;
  const mgPaint* paint = NULL;
  const mgFrame* frame = NULL;
  mgColor upColor, hoverColor, downColor, disColor, baseColor;

  setAttr(MG_STYLE_ALL, "font", m_surface->createFont("Arial-10"));

  //--------- Label
  setAttr(MG_STYLE_LABEL, "textColor", mgColor("black"));
  setAttr(MG_STYLE_LABEL, "disTextColor", mgColor("gray"));

  //--------- Button
  baseColor = mgColor(0/255.0, 192/255.0, 255/255.0);
  paint = mgSolidPaint::createPaint(surface, baseColor);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, baseColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_BUTTON, "upFrame", frame);
  setAttr(MG_STYLE_BUTTON, "upColor", mgColor("black"));

  baseColor = mgColor(0/255.0, 128/255.0, 255/255.0);
  paint = mgSolidPaint::createPaint(surface, baseColor);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, baseColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_BUTTON, "hoverFrame", frame);
  setAttr(MG_STYLE_BUTTON, "hoverColor", mgColor("black"));

  baseColor = mgColor(0/255.0, 64/255.0, 255/255.0);
  paint = mgSolidPaint::createPaint(surface, baseColor);
  frame = mgRectFrame::createFrame(surface, mgLoweredEdge, 1, baseColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_BUTTON, "downFrame", frame);
  setAttr(MG_STYLE_BUTTON, "downColor", mgColor("black"));

  baseColor = mgColor(192/255.0, 192/255.0, 192/255.0);
  paint = mgSolidPaint::createPaint(surface, baseColor);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, baseColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_BUTTON, "disFrame", frame);
  setAttr(MG_STYLE_BUTTON, "disColor", mgColor("black"));

  //--------- Checkbox
  baseColor = mgColor(0/255.0, 192/255.0, 255/255.0);
  paint = mgSolidPaint::createPaint(surface, baseColor);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, baseColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_CHECKBOX, "upFrame", frame);
  setAttr(MG_STYLE_CHECKBOX, "upColor", mgColor("black"));

  baseColor = mgColor(0/255.0, 128/255.0, 255/255.0);
  paint = mgSolidPaint::createPaint(surface, baseColor);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, baseColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_CHECKBOX, "hoverFrame", frame);
  setAttr(MG_STYLE_CHECKBOX, "hoverColor", mgColor("black"));

  baseColor = mgColor(0/255.0, 64/255.0, 255/255.0);
  paint = mgSolidPaint::createPaint(surface, baseColor);
  frame = mgRectFrame::createFrame(surface, mgLoweredEdge, 1, baseColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_CHECKBOX, "downFrame", frame);
  setAttr(MG_STYLE_CHECKBOX, "downColor", mgColor("black"));

  baseColor = mgColor(192/255.0, 192/255.0, 192/255.0);
  paint = mgSolidPaint::createPaint(surface, baseColor);
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, baseColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_CHECKBOX, "disFrame", frame);
  setAttr(MG_STYLE_CHECKBOX, "disColor", mgColor("black"));

  //--------- Console
  setAttr(MG_STYLE_CONSOLE, "font", m_surface->createFont("Courier-10"));
  paint = mgSolidPaint::createPaint(surface, mgColor(0/255.0, 128/255.0, 255/255.0));
  frame = mgRectFrame::createFrame(surface, mgRaisedEdge, 1, mgColor("black"), paint, mgFrameMargin(2)); 
  setAttr(MG_STYLE_CONSOLE, "frame", frame);
  setAttr(MG_STYLE_CONSOLE, "textColor", mgColor("black"));

  //--------- Field
  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_FIELD, "upFrame", frame);
  setAttr(MG_STYLE_FIELD, "upColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(220/255.0, 220/255.0, 255/255.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_FIELD, "hoverFrame", frame);
  setAttr(MG_STYLE_FIELD, "hoverColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(200/255.0, 200/255.0, 255/255.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_FIELD, "downFrame", frame);
  setAttr(MG_STYLE_FIELD, "downColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(192/255.0, 192/255.0, 192/255.0));
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

  paint = mgSolidPaint::createPaint(surface, mgColor(220/255.0, 220/255.0, 255/255.0));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_LIST, "itemHoverFrame", frame);
  setAttr(MG_STYLE_LIST, "itemHoverColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(128/255.0, 128/255.0, 255/255.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_LIST, "itemDownFrame", frame);
  setAttr(MG_STYLE_LIST, "itemDownColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor(225/255.0, 225/255.0, 225/255.0));
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

  paint = mgSolidPaint::createPaint(surface, mgColor(220/255.0, 220/255.0, 255/255.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "lineHoverFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(128/255.0, 128/255.0, 255/255.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "lineDownFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_SCROLLBAR, "pageUpFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(220/255.0, 220/255.0, 255/255.0));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_SCROLLBAR, "pageHoverFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(128/255.0, 128/255.0, 255/255.0));
  frame = mgFrame::createFrame(surface, paint, mgFrameMargin(2));
  setAttr(MG_STYLE_SCROLLBAR, "pageDownFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(192/255.0, 192/255.0, 192/255.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "sliderUpFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(165/255.0, 165/255.0, 255/255.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "sliderHoverFrame", frame);

  paint = mgSolidPaint::createPaint(surface, mgColor(128/255.0, 128/255.0, 255/255.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(1));
  setAttr(MG_STYLE_SCROLLBAR, "sliderDownFrame", frame);

  // split control
  mgColor divColor("black");
  paint = mgSolidPaint::createPaint(surface, divColor);
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, divColor, paint, mgFrameMargin(5)); 
  setAttr(MG_STYLE_SPLIT, "divider", frame);
  // =-= cursor

  // tabbed control
  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_TABBED, "upFrame", frame);
  setAttr(MG_STYLE_TABBED, "upColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor("blue"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("blue"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_TABBED, "downFrame", frame);
  setAttr(MG_STYLE_TABBED, "downColor", mgColor("white"));

  paint = mgSolidPaint::createPaint(surface, mgColor(0.8, 0.8, 0.8));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor(0.8, 0.8, 0.8), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_TABBED, "hoverFrame", frame);
  setAttr(MG_STYLE_TABBED, "hoverColor", mgColor("black"));

  paint = mgSolidPaint::createPaint(surface, mgColor("white"));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(2));
  setAttr(MG_STYLE_TABBED, "disFrame", frame);
  setAttr(MG_STYLE_TABBED, "disColor", mgColor("black"));

  // window 
  setAttr(MG_STYLE_WINDOW, "font", m_surface->createFont("Arial-12-B"));
  paint = mgSolidPaint::createPaint(surface, mgColor(0.7, 0.7, 1.0));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor(0.7, 0.7, 1.0), paint, mgFrameMargin(10));
  setAttr(MG_STYLE_WINDOW, "onFrame", frame);
  setAttr(MG_STYLE_WINDOW, "onColor", mgColor("white"));

  paint = mgSolidPaint::createPaint(surface, mgColor(0.4, 0.4, 0.4));
  frame = mgRectFrame::createFrame(surface, mgFlatEdge, 1, mgColor("black"), paint, mgFrameMargin(10));
  setAttr(MG_STYLE_WINDOW, "offFrame", frame);
  setAttr(MG_STYLE_WINDOW, "offColor", mgColor("white"));
}

//--------------------------------------------------------------
// destructor
mgSimpleStyle::~mgSimpleStyle()
{
}

//--------------------------------------------------------------
// create a label control
mgLabelControl* mgSimpleStyle::createLabel(
  mgControl* parent,
  const char* cntlName, 
  const char* label)
{
  mgSimpleLabel* cntl = new mgSimpleLabel(parent, cntlName);
  cntl->setLabel(label);

  return cntl;
}

//--------------------------------------------------------------
// create a field control
mgFieldControl* mgSimpleStyle::createField(
  mgControl* parent,
  const char* cntlName)
{
  mgSimpleField* cntl = new mgSimpleField(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a button control
mgButtonControl* mgSimpleStyle::createButton(
  mgControl* parent,
  const char* cntlName, 
  const char* label,
  const char* iconName)
{
  mgSimpleButton* cntl = new mgSimpleButton(parent, cntlName);
  cntl->setLabel(label);
  cntl->setIcon(iconName);

  return cntl;
}

//--------------------------------------------------------------
// create a checkbox control
mgCheckboxControl* mgSimpleStyle::createCheckbox(
  mgControl* parent,
  const char* cntlName, 
  const char* label)
{
  mgSimpleCheckbox* cntl = new mgSimpleCheckbox(parent, cntlName);
  cntl->setLabel(label);

  return cntl;
}

//--------------------------------------------------------------
// create a list control
mgListControl* mgSimpleStyle::createList(
  mgControl* parent,
  const char* cntlName)
{
  mgSimpleList* cntl = new mgSimpleList(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a ScrollPane control
mgScrollPaneControl* mgSimpleStyle::createScrollPane(
  mgControl* parent,
  const char* cntlName)
{
  mgSimpleScrollPane* cntl = new mgSimpleScrollPane(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a console control
mgConsoleControl* mgSimpleStyle::createConsole(
  mgControl* parent,
  const char* cntlName)
{
  mgSimpleConsole* cntl = new mgSimpleConsole(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a stack control
mgStackControl* mgSimpleStyle::createStack(
  mgControl* parent,
  const char* cntlName)
{
  mgSimpleStack* cntl = new mgSimpleStack(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a split control
mgSplitControl* mgSimpleStyle::createSplit(
  mgControl* parent,
  const char* cntlName)
{
  mgSimpleSplit* cntl = new mgSimpleSplit(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a tabbed control
mgTabbedControl* mgSimpleStyle::createTabbed(
  mgControl* parent,
  const char* cntlName)
{
  mgSimpleTabbed* cntl = new mgSimpleTabbed(parent, cntlName);

  return cntl;
}

//--------------------------------------------------------------
// create a desktop control
mgDesktopControl* mgSimpleStyle::createDesktop(
  mgControl* parent,
  const char* cntlName)
{
  mgSimpleDesktop* cntl = new mgSimpleDesktop(parent, cntlName);

  return cntl;
}

