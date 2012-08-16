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

#include "SpeedUI.h"
#include "mgGUI/Include/mgGUI.h"

//--------------------------------------------------------------
// constructor
SpeedUI::SpeedUI(
  const mgOptionsFile& options)
{
  m_speed = 0.0;
  m_range = 0.0;
  m_units = UNITS_KM;
  m_landingState = LANDING_NONE;

  // create a rendering surface
  m_surface = mgDisplay->createOverlaySurface();

  // create a style object, which creates controls
  m_style = new mgUglyStyle(m_surface);

  // create the top control that holds the ui
  m_top = new mgTopControl(m_surface, m_style);
  mgTableLayout* layout = new mgTableLayout(m_top);
  m_top->setLayout(layout);

  // set fonts and colors
  mgString fontName, colorName;
  options.getString("speedFont", "Courier-10", fontName);
  const mgFont* speedFont = m_surface->createFont(fontName);

  options.getString("speedColor", "white", colorName);
  mgColor speedColor(colorName);

  m_style->setAttr("speedHead", "font", speedFont);
  m_style->setAttr("speedHead", "textColor", speedColor);
  m_style->setAttr("speedLbL", "font", speedFont);
  m_style->setAttr("speedLbl", "textColor", speedColor);

  options.getString("rangeFont", "Courier-10", fontName);
  const mgFont* rangeFont = m_surface->createFont(fontName);

  options.getString("rangeColor", "white", colorName);
  mgColor rangeColor(colorName);

  m_style->setAttr("rangeHead", "font", rangeFont);
  m_style->setAttr("rangeHead", "textColor", rangeColor);
  m_style->setAttr("rangeLbL", "font", rangeFont);
  m_style->setAttr("rangeLbl", "textColor", rangeColor);

  // create the labels
  mgLabelControl* txt1 = m_style->createLabel(m_top, "speedHead", "Speed:");
  m_speedLbl = m_style->createLabel(m_top, "speedLbl", "");
  m_speedLbl->setMaxLabel("9,999,999 mph");
  mgLabelControl* txt2 = m_style->createLabel(m_top, "rangeHead", "Range:");
  m_rangeLbl = m_style->createLabel(m_top, "rangeLbl", "");
  m_rangeLbl->setMaxLabel("9,999,999 mi");

  // layout controls
  layout->addCell(txt1, mgTableAlignLeft, mgTableAlignTop, 1, 1);
  layout->addCell(m_speedLbl, mgTableAlignLeft, mgTableAlignTop, 1, 1);

  layout->newRow();
  layout->addCell(txt2, mgTableAlignLeft, mgTableAlignTop, 1, 1);
  layout->addCell(m_rangeLbl, mgTableAlignLeft, mgTableAlignTop, 1, 1);

  layout->endRow();

  // set the help pane to preferred size
  mgDimension size;
  m_top->preferredSize(size);
  m_top->surfaceResized(size.m_width, size.m_height);
  m_surface->setSurfaceSize(size.m_width, size.m_height);
}

//--------------------------------------------------------------
// destructor
SpeedUI::~SpeedUI()
{
  delete m_top;
  m_top = NULL;

  delete m_style;
  m_style = NULL;

  delete m_surface;
  m_surface = NULL;
}

//--------------------------------------------------------------
// view has been resized
void SpeedUI::viewResized(
  int width,
  int height)
{
  m_viewWidth = width;
  m_viewHeight = height;
}

//--------------------------------------------------------------
// update the ui
BOOL SpeedUI::animate(
  double now,
  double since)
{
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
void SpeedUI::render()
{
  if (m_surface == NULL)
    return;

  mgDisplay->setTransparent(true);
  mgDisplay->setZEnable(false);

  mgDimension size;
  m_top->getSize(size);

  // draw at top-right
  mgDisplay->drawOverlaySurface(m_surface, m_viewWidth - size.m_width, 0);
}

//--------------------------------------------------------------
// comma format speed
void SpeedUI::commaFormat(
  mgString& line,
  const char* suffix,
  int value)
{
  line.format("%d %s", value, (const char*) suffix);

  // add the commas
  int posn = line.find(0, ' ')-3;
  while (posn > 0)
  {
    line.insertAt(posn, ",", 1);
    posn -= 3;
  }
}

//--------------------------------------------------------------
// format the speed
void SpeedUI::formatSpeed()
{
  mgString line;
  switch (m_units)
  {
    case UNITS_MILES: 
      commaFormat(line, "mph", (int) floor(0.5+0.621371192*m_speed*3600.0));
      break;
    case UNITS_KM:
      commaFormat(line, "kph", (int) floor(0.5+m_speed*3600.0));
      break;
  }
  m_speedLbl->setLabel(line);
}

//--------------------------------------------------------------
// format the range
void SpeedUI::formatRange()
{
  mgString line;
  switch (m_units)
  {
    case UNITS_MILES: 
      commaFormat(line, "mi", (int) floor(0.5+0.621371192*m_range/1000.0));
      break;
    case UNITS_KM:
      commaFormat(line, "km", (int) floor(0.5+m_range/1000.0));
      break;
  }
  m_rangeLbl->setLabel(line);
}

//--------------------------------------------------------------
// set the units
void SpeedUI::setUnits(
  int units)
{
  if (m_units == units)
    return;

  m_units = units;

  formatSpeed();
  formatRange();
}

//--------------------------------------------------------------
// set the speed
void SpeedUI::setSpeed(
  double speed)
{
  if (m_speed == speed)
    return;

  m_speed = speed;
  formatSpeed();
}

//--------------------------------------------------------------
// set the range
void SpeedUI::setRange(
  double range)
{
  if (m_range == range)
    return;

  m_range = range;
  formatRange();
}

//--------------------------------------------------------------
// set landing indicator
void SpeedUI::setLanding(
  int state)
{
  if (m_landingState == state)
    return;

  m_landingState = state;
}

