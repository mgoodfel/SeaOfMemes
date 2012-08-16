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
#ifndef SPEEDUI_H
#define SPEEDUI_H

#include "mgGUI/Include/GUI/mgToolkit.h"

const int LANDING_NONE = 0;
const int LANDING_RANGE = 1;
const int LANDING_ON = 2;

const int UNITS_MILES = 0;
const int UNITS_KM = 1;

class SpeedUI 
{
public:
  // constructor
  SpeedUI(
    const mgOptionsFile& options);

  // destructor
  virtual ~SpeedUI();

  // set the units
  virtual void setUnits(
    int units);

  // set the speed
  virtual void setSpeed(
    double speed);

  // set the range
  virtual void setRange(
    double range);

  // set landing indicator
  virtual void setLanding(
    int state);

  // view resized
  virtual void viewResized(
    int width,
    int height);

  // update ui graphics.  return true if changed
  virtual BOOL animate(
    double now, 
    double since);

  // draw the ui
  virtual void render();

  // return top control
  virtual mgTopControl* getTop()
  { 
    return m_top;
  }

protected:
  int m_viewWidth;
  int m_viewHeight;
  mgTextureSurface* m_surface;
  mgStyle* m_style;
  mgTopControl* m_top;

  mgLabelControl* m_speedLbl;
  mgLabelControl* m_rangeLbl;

  int m_units;
  double m_speed;
  double m_range;
  int m_landingState;

  // comma format speed
  virtual void commaFormat(
    mgString& line,
    const char* suffix,
    int value);

  // format the speed
  virtual void formatSpeed();

  // format the range
  virtual void formatRange();
};

#endif
