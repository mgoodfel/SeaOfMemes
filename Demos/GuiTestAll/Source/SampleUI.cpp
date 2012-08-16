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

#include "SampleUI.h"
#include "mgGUI/Include/mgGUI.h"

//--------------------------------------------------------------
// constructor
SampleUI::SampleUI(
  mgSurface* surface,
  const char* helpFile)
{
  // save the rendering surface
  m_surface = surface;

  // create a style object, which creates controls
  m_style = new mgUglyStyle(m_surface);

  // create the top control that holds the ui
  m_top = new mgTopControl(m_surface, m_style);

  // create the top two panels -- console/help and controls
  m_stack = createLeftSide(m_top, helpFile);
  mgControl* panel = createControlPanel(m_top);

  // now create a table layout for the top control
  mgTableLayout* topLayout = new mgTableLayout(m_top);
  m_top->setLayout(topLayout);

  topLayout->newRow();
  topLayout->setRowWeight(0);

  // left side is the stack
  topLayout->addCell(m_stack, mgTableAlignHFill, mgTableAlignVFill, 1, 1);

  // right side is the control panel.  set the weight so it doesn't stretch
  topLayout->addCell(panel, mgTableAlignLeft, mgTableAlignTop, 1, 1);
  topLayout->setColWeight(0);  

  // add another row with an empty control in it.  Otherwise, since the UI
  // table takes up the entire window, the console will extend to the bottom.
  // note the top row has weight=0, and the new row has weight 100.

  topLayout->newRow();
  topLayout->setRowWeight(100);
  topLayout->addCell(new mgControl(m_top), mgTableAlignHFill, mgTableAlignVFill, 1, 2);

  topLayout->endRow();

  // start the UI showing the help
  m_stack->selectPane("help");

  /*
    The surface and top control will be resized in the main program when
    the window size changes.  That will mark the surface damaged, and
    it will be repaired in the main loop when idle.
  */
}

//--------------------------------------------------------------
// destructor
SampleUI::~SampleUI()
{
  delete m_top;
  m_top = NULL;

  delete m_style;
  m_style = NULL;
}

//--------------------------------------------------------------
// create the left side of the UI 
mgStackControl* SampleUI::createLeftSide(
  mgControl* parent,
  const char* helpFile)
{
  // create a stack to hold help, console or nothing. 
  mgStackControl* stack = m_style->createStack(m_top, "theStack");

  mgControl* noPane = stack->addPane("none");
  mgControl* consolePane = stack->addPane("console");
  mgControl* helpPane = stack->addPane("help");

  // create a console called "theConsole" using the style.  we report events here
  const mgFont* consoleFont = m_surface->createFont("Courier-10");
  m_style->setAttr("theConsole", "font", consoleFont);
  m_console = m_style->createConsole(consolePane, "theConsole");

  // create an input line for use with the console
  m_style->setAttr("input", "font", consoleFont);
  m_input = m_style->createField(consolePane, "input");
  m_input->enableHistory(true);    // support scrolling through old input
  m_input->addChangeListener(this);

  // create a table layout to put the input below the console
  mgTableLayout* consoleLayout = new mgTableLayout(consolePane);
  consolePane->setLayout(consoleLayout);

  consoleLayout->newRow();
  consoleLayout->setRowWeight(100);
  consoleLayout->addCell(m_console, mgTableAlignHFill, mgTableAlignVFill, 1, 1);

  consoleLayout->newRow();
  consoleLayout->setRowWeight(0);
  consoleLayout->addCell(m_input, mgTableAlignHFill, mgTableAlignTop, 1, 1);
  consoleLayout->endRow();

  // add a line to the console so we know it's there
  for (int i = 0; i < 50; i++)
  {
    mgString text;
    text.format("This is line %d with very long text which doesn't mean anything really, but will reformat nicely.", i+1);
    m_console->addLine(mgColor("white"), NULL, text);
  }
//  m_console->addLine(mgColor("white"), NULL, "Ready!");

  // -------- create a form to hold help text
  m_help = new mgFormPane(helpPane);
  m_help->setDefaultFontFace("Georgia");
  m_help->setDefaultTextColor(mgColor("black"));
  m_help->parseFormFile(helpFile);

  // set the background and frame of the help
  const mgPaint* helpBack = mgSolidPaint::createPaint(m_surface, mgColor(200, 200, 255));
  const mgFrame* helpFrame = mgRectFrame::createFrame(m_surface, mgFlatEdge, 2, mgColor("black"), helpBack, mgFrameMargin(10));
  m_help->setFrame(helpFrame);

  return stack;
}

//--------------------------------------------------------------
// create the control panel
mgControl* SampleUI::createControlPanel(
  mgControl* parent)
{
  // create a child panel to hold controls. 
  m_controlPanel = new mgControl(parent);

  // create some more controls using the style
  mgControl* title = m_style->createLabel(m_controlPanel, "title", "Position");
  mgLabelControl* xlbl = m_style->createLabel(m_controlPanel, "xlbl", "x: ");
  m_xvalue = m_style->createLabel(m_controlPanel, "xvalue", "99999999");
  mgLabelControl* ylbl = m_style->createLabel(m_controlPanel, "ylbl", "y: ");
  m_yvalue = m_style->createLabel(m_controlPanel, "yvalue", "99999999");

  mgButtonControl* btn1 = m_style->createButton(m_controlPanel, "theButton", "Button", NULL);
  mgCheckboxControl* chk1 = m_style->createCheckbox(m_controlPanel, "chk", "Checkbox");
  mgFieldControl* fld1 = m_style->createField(m_controlPanel, "fld");

  // attach handlers to inform this class of ui events.  The listeners call
  // the guiAction and guiSelection methods below.
  btn1->addActionListener(this);
  chk1->addSelectListener(this);
  fld1->addChangeListener(this);

  // create a layout manager, attached to the panel
  mgTableLayout* panelLayout = new mgTableLayout(m_controlPanel);
  m_controlPanel->setLayout(panelLayout);

  // create a background for the entire panel
  const mgPaint* panelBack = mgSolidPaint::createPaint(m_surface, mgColor("gray"));
  const mgFrame* panelFrame = mgFrame::createFrame(m_surface, panelBack, mgFrameMargin(10));
  panelLayout->setFrame(panelFrame);

  // create an empty frame to act as margin around controls.
  const mgFrame* margin = mgFrame::createFrame(m_surface, NULL, mgFrameMargin(10));

  // add controls to the panel layout: rows and columns like HTML tr and td.
  // first row, first cell spans 2 columns
  panelLayout->newRow();
  panelLayout->setRowBottomInset(10);
  panelLayout->addCell(title, mgTableAlignHFill, mgTableAlignVFill, 1, 2);

  // x: label and value
  panelLayout->newRow();
  panelLayout->setRowBottomInset(10);
  panelLayout->addCell(xlbl, mgTableAlignHFill, mgTableAlignVFill, 1, 1);
  panelLayout->addCell(m_xvalue, mgTableAlignHFill, mgTableAlignVFill, 1, 1);

  // y: label and value
  panelLayout->newRow();
  panelLayout->setRowBottomInset(10);
  panelLayout->addCell(ylbl, mgTableAlignHFill, mgTableAlignVFill, 1, 1);
  panelLayout->addCell(m_yvalue, mgTableAlignHFill, mgTableAlignVFill, 1, 1);

  // button and checkbox
  panelLayout->newRow();
  panelLayout->addCell(btn1, mgTableAlignHCenter, mgTableAlignVCenter, 1, 1);
  panelLayout->setCellFrame(margin);
  panelLayout->addCell(chk1, mgTableAlignHFill, mgTableAlignVFill, 1, 1);
  panelLayout->setCellFrame(margin);

  // field in two columns
  panelLayout->newRow();
  panelLayout->addCell(fld1, mgTableAlignHFill, mgTableAlignVFill, 1, 2);
  panelLayout->setCellFrame(margin);

  panelLayout->endRow();

  return m_controlPanel;
}

//--------------------------------------------------------------
// animate the UI
void SampleUI::animate(
  double now,
  double since)
{
  if (m_top != NULL)
    m_top->animate(now, since);
}

//--------------------------------------------------------------
// resize the UI
void SampleUI::resize(
  int width,
  int height)
{
  m_surface->setSurfaceSize(width, height);
  m_top->surfaceResized(width, height);

  // we are going to take mouse presses on the control panel and on 
  // the console input line, so remember their locations
  mgDimension size;
  m_stack->getSize(size);
  m_consoleBounds = mgRectangle(0, 0, size.m_width, size.m_height);
  m_stack->getLocationInAncestor(m_top, m_consoleBounds);
  
  m_controlPanel->getSize(size);
  m_controlBounds = mgRectangle(0, 0, size.m_width, size.m_height);
  m_controlPanel->getLocationInAncestor(m_top, m_controlBounds);
}

//--------------------------------------------------------------
// set sample statistics
void SampleUI::setValue(
  double x,
  double y)
{
  mgString str;
  str.format("%10.2f", x);
  m_xvalue->setLabel(str);

  str.format("%10.2f", y);
  m_yvalue->setLabel(str);
}

//--------------------------------------------------------------
// return true if we have a key focus
BOOL SampleUI::hasKeyFocus()
{
  return m_top->getKeyFocus() != NULL;
}

//--------------------------------------------------------------
// return true if we have a mouse focus
BOOL SampleUI::hasMouseFocus()
{
  return m_top->getMouseFocus() != NULL;
}

//--------------------------------------------------------------
// return true if mouse point at this location is for UI
BOOL SampleUI::useMousePoint(
  int x,
  int y)
{
  mgPoint pt(x, y);
  if (m_console->getVisible() && m_consoleBounds.containsPt(pt))
    return true;
  if (m_controlBounds.containsPt(pt))
    return true;
  return false;
}

//--------------------------------------------------------------
// toggle console visibility
void SampleUI::toggleConsole()
{
  mgString selected;
  m_stack->getSelected(selected);

  if (selected.equalsIgnoreCase("console"))
    m_stack->selectPane("none");
  else m_stack->selectPane("console");
}

//--------------------------------------------------------------
// toggle help visibility
void SampleUI::toggleHelp()
{
  mgString selected;
  m_stack->getSelected(selected);

  if (selected.equalsIgnoreCase("help"))
    m_stack->selectPane("none");
  else m_stack->selectPane("help");
}

//--------------------------------------------------------------
// handle action from ui control
void SampleUI::guiAction(
  void* source,
  const char* name)
{
  mgString msg;
  msg.format("button %s pressed", name);
  m_console->addLine(mgColor("white"), NULL, msg);
}

//--------------------------------------------------------------
// handle selection from ui control
void SampleUI::guiSelection(
  void* source,
  const char* name,
  BOOL state)
{
  mgString msg;
  msg.format("checkbox %s is %s", name, state ? "on":"off");
  m_console->addLine(mgColor("white"), NULL, msg);
}

//--------------------------------------------------------------
// handle change from ui control
void SampleUI::guiChange(
  void* source,
  const char* name)
{
  mgString value;
  mgFieldControl* field = (mgFieldControl*) source;
  field->getText(value);
  field->reset();

  // if this is the input line under the console, just echo it
  if (field == m_input)
  {
    m_console->addLine(mgColor("blue"), NULL, value);
  }
  else
  {
    // lose the key focus after input
    field->releaseKeyFocus();

    // the field in the controls is treated like other controls
    mgString msg;
    msg.format("field %s changed to '%s'", name, (const char*) value);
    m_console->addLine(mgColor("white"), NULL, msg);
  }
}

