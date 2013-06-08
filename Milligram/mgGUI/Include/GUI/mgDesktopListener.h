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

#ifndef MGDESKTOPLISTENER_H
#define MGDESKTOPLISTENER_H

class mgControl;

/*
  Abstract interface which delivers events from the mgDesktopControl
*/
class mgDesktopListener
{
public:
  // window activated
  virtual void desktopActivate(
    mgControl* contentPane) = 0;

  // window close 
  virtual void desktopClose(
    mgControl* contentPane) = 0;

  // window minimize
  virtual void desktopMinimize(
    mgControl* contentPane) = 0;

  // window maximize
  virtual void desktopMaximize(
    mgControl* contentPane) = 0;

  // window resize
  virtual void desktopResize(
    mgControl* contentPane) = 0;

  // window move
  virtual void desktopMove(
    mgControl* contentPane) = 0;
};

#endif
