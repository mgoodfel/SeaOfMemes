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

#include "HelpUI.h"
#include "mgGUI/Include/mgGUI.h"
#include "mgGUI/Include/Util/mgDebugPane.h"

//--------------------------------------------------------------
// constructor
HelpUI::HelpUI(
  const mgOptionsFile& options)
{
  m_style = NULL;
  m_top = NULL;
  m_stack = NULL;
  m_help = NULL;
  m_debug = NULL;

  mgString helpFile;
  options.getFileName("helpFile", options.m_sourceFileName, "help.xml", helpFile);

  // create a rendering surface
  m_overlay = mgDisplay->createOverlaySurface();

  // if no 2d graphics, nothing we can do
  if (m_surface == NULL)
    return;

  // create a style object, which creates controls
  m_surface = m_overlay->getSurface();
  m_style = new mgSimpleStyle(m_surface);

  // create the top control that holds the ui
  m_top = new mgTopControl(m_surface, m_style);
  m_top->setLayout(new mgColumnLayout(m_top));

  // create a stack to hold help, console or nothing. 
  m_stack = m_style->createStack(m_top, "theStack");
  m_stack->setSameSize(false);

  mgControl* noPane = m_stack->addPane("none");
  mgControl* consolePane = m_stack->addPane("console");
  mgControl* helpPane = m_stack->addPane("help");

  m_debug = new mgDebugPane(consolePane, "debug");

  m_help = new mgFormPane(helpPane);
  m_help->setDefaultFontFace("Georgia");
  m_help->setDefaultTextColor(mgColor("black"));
  m_help->parseFormFile(helpFile);

  // set the background and frame of the help
  const mgPaint* helpBack = mgSolidPaint::createPaint(m_surface, mgColor(200/255.0, 200/255.0, 255/255.0));
  const mgFrame* helpFrame = mgRectFrame::createFrame(m_surface, mgFlatEdge, 2, mgColor("black"), helpBack, mgFrameMargin(10));
  m_help->setFrame(helpFrame);

  // start with help open
  m_stack->selectPane("help");
}

//--------------------------------------------------------------
// destructor
HelpUI::~HelpUI()
{
  delete m_top;
  m_top = NULL;

  delete m_style;
  m_style = NULL;

  delete m_overlay;
  m_overlay = NULL;
  m_surface = NULL;
}

//--------------------------------------------------------------
// set application for debug console
void HelpUI::setDebugApp(
  mgDebugInterface* app)
{
  if (m_debug != NULL)
    m_debug->setDebugApp(app);
}

//--------------------------------------------------------------
// add a line to the debug console
void HelpUI::addLine(
  const char* format,
  ...)
{
  mgString line;
  va_list args;
  va_start(args, format);

  line.formatV(format, args);
  if (m_debug != NULL)
    m_debug->addLine(mgColor("white"), NULL, line);
}

//--------------------------------------------------------------
// view has been resized
void HelpUI::viewResized(
  int width,
  int height)
{
  // if no 2d graphics, nothing we can do
  if (m_surface == NULL)
    return;

  // set the help pane to min of preferred size and view size
  mgDimension helpSize;
  m_help->preferredSize(helpSize);
  helpSize.m_width = min(helpSize.m_width, width);
  helpSize.m_height = min(helpSize.m_height, height);
  m_help->setSize(helpSize.m_width, helpSize.m_height);

  mgDimension debugSize;
  m_debug->preferredSize(debugSize);
  debugSize.m_width = min(debugSize.m_width, width);
  debugSize.m_height = min(debugSize.m_height, height);
  m_debug->setSize(debugSize.m_width, debugSize.m_height);

  m_debugBounds = mgRectangle(0, 0, debugSize.m_width, debugSize.m_height);

  mgDimension size;
  size.m_width = max(helpSize.m_width, debugSize.m_width);  
  size.m_height = max(helpSize.m_height, debugSize.m_height);  

  m_top->surfaceResized(size.m_width, size.m_height);
  m_surface->setSurfaceSize(size.m_width, size.m_height);
}

//--------------------------------------------------------------
// update the ui
BOOL HelpUI::animate(
  double now,
  double since)
{
  if (m_top != NULL)
    m_top->animate(now, since);

  // if the ui needs an update
  if (m_surface != NULL && m_surface->isDamaged())
  {
    mgRectangle bounds;
    m_surface->getDamage(bounds);
    m_top->surfacePaint(bounds);
    m_surface->repair(bounds);
    return true;
  }
  else return false;
}

//--------------------------------------------------------------
// render the ui
void HelpUI::render()
{
  // if no 2d graphics, nothing we can do
  if (m_surface == NULL)
    return;

  mgDisplay->setTransparent(true);
  mgDisplay->setZEnable(false);

  if (m_surface != NULL)
    m_overlay->drawOverlay(0, 0);
}

//--------------------------------------------------------------
// toggle console visibility
void HelpUI::toggleConsole()
{
  // if no 2d graphics, nothing we can do
  if (m_surface == NULL)
    return;

  mgString selected;
  m_stack->getSelected(selected);

  if (selected.equalsIgnoreCase("console"))
    m_stack->selectPane("none");
  else m_stack->selectPane("console");
}

//--------------------------------------------------------------
// toggle help visibility
void HelpUI::toggleHelp()
{
  // if no 2d graphics, nothing we can do
  if (m_surface == NULL)
    return;

  mgString selected;
  m_stack->getSelected(selected);

  if (selected.equalsIgnoreCase("help"))
    m_stack->selectPane("none");
  else m_stack->selectPane("help");
}

//--------------------------------------------------------------
// reset focus
void HelpUI::resetFocus()
{
  if (m_top != NULL)
    m_top->setKeyFocus(NULL);
}

//--------------------------------------------------------------
// true if ui has key focus
BOOL HelpUI::hasKeyFocus()
{
  if (m_top != NULL)
    return m_top->getKeyFocus() != NULL;
  else return false;
}

//--------------------------------------------------------------
// true if has mouse focus
BOOL HelpUI::hasMouseFocus()
{
  if (m_top != NULL)
    return m_top->getMouseFocus() != NULL;
  else return false;
}

//--------------------------------------------------------------
// true if ui uses mouse point
BOOL HelpUI::usesMousePoint(
  int x, 
  int y)
{
  mgPoint pt(x, y);
  if (m_debug != NULL && m_debug->getVisible() && m_debugBounds.containsPt(pt))
    return true;
  return false;
}
