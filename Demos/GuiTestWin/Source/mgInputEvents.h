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
#ifndef MGINPUTEVENTS_H
#define MGINPUTEVENTS_H

// platform-independent button/keyboard events
const int MG_EVENT_ALT_DOWN    = 0x0001;
const int MG_EVENT_SHIFT_DOWN  = 0x0002;
const int MG_EVENT_CNTL_DOWN   = 0x0004;
const int MG_EVENT_META_DOWN   = 0x0008;

const int MG_EVENT_MOUSE1_DOWN = 0x0010;
const int MG_EVENT_MOUSE2_DOWN = 0x0020;
const int MG_EVENT_MOUSE3_DOWN = 0x0040;

const int MG_EVENT_MOUSE_BUTTONS = 0x0070;

const int MG_EVENT_KEYREPEAT   = 0x0080;

const int MG_EVENT_KEY_F1         = 0x10001;
const int MG_EVENT_KEY_F2         = 0x10002;
const int MG_EVENT_KEY_F3         = 0x10003;
const int MG_EVENT_KEY_F4         = 0x10004;
const int MG_EVENT_KEY_F5         = 0x10005;
const int MG_EVENT_KEY_F6         = 0x10006;
const int MG_EVENT_KEY_F7         = 0x10007;
const int MG_EVENT_KEY_F8         = 0x10008;
const int MG_EVENT_KEY_F9         = 0x10009;
const int MG_EVENT_KEY_F10        = 0x1000A;
const int MG_EVENT_KEY_F11        = 0x1000B;
const int MG_EVENT_KEY_F12        = 0x1000C;

const int MG_EVENT_KEY_LEFT       = 0x1000D;
const int MG_EVENT_KEY_RIGHT      = 0x1000E;
const int MG_EVENT_KEY_UP         = 0x1000F;
const int MG_EVENT_KEY_DOWN       = 0x10010;

const int MG_EVENT_KEY_INSERT     = 0x10011;
const int MG_EVENT_KEY_DELETE     = 0x10012;
const int MG_EVENT_KEY_HOME       = 0x10013;
const int MG_EVENT_KEY_END        = 0x10014;
const int MG_EVENT_KEY_PAGEUP     = 0x10015;
const int MG_EVENT_KEY_PAGEDN     = 0x10016;

const int MG_EVENT_KEY_RETURN     = 0x10017;
const int MG_EVENT_KEY_TAB        = 0x10018;
const int MG_EVENT_KEY_ESCAPE     = 0x10019;
const int MG_EVENT_KEY_BACKSPACE  = 0x10020;

#endif
