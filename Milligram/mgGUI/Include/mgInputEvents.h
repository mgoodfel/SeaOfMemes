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
#ifndef MGINPUTEVENTS_H
#define MGINPUTEVENTS_H

// MUST match mgPlatform version

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

const int MG_EVENT_KEY_F1         = 0x40000001;
const int MG_EVENT_KEY_F2         = 0x40000002;
const int MG_EVENT_KEY_F3         = 0x40000003;
const int MG_EVENT_KEY_F4         = 0x40000004;
const int MG_EVENT_KEY_F5         = 0x40000005;
const int MG_EVENT_KEY_F6         = 0x40000006;
const int MG_EVENT_KEY_F7         = 0x40000007;
const int MG_EVENT_KEY_F8         = 0x40000008;
const int MG_EVENT_KEY_F9         = 0x40000009;
const int MG_EVENT_KEY_F10        = 0x4000000A;
const int MG_EVENT_KEY_F11        = 0x4000000B;
const int MG_EVENT_KEY_F12        = 0x4000000C;

const int MG_EVENT_KEY_LEFT       = 0x4000000D;
const int MG_EVENT_KEY_RIGHT      = 0x4000000E;
const int MG_EVENT_KEY_UP         = 0x4000000F;
const int MG_EVENT_KEY_DOWN       = 0x40000010;

const int MG_EVENT_KEY_INSERT     = 0x40000011;
const int MG_EVENT_KEY_DELETE     = 0x40000012;
const int MG_EVENT_KEY_HOME       = 0x40000013;
const int MG_EVENT_KEY_END        = 0x40000014;
const int MG_EVENT_KEY_PAGEUP     = 0x40000015;
const int MG_EVENT_KEY_PAGEDN     = 0x40000016;

const int MG_EVENT_KEY_RETURN     = 0x40000017;
const int MG_EVENT_KEY_TAB        = 0x40000018;
const int MG_EVENT_KEY_ESCAPE     = 0x40000019;
const int MG_EVENT_KEY_BACKSPACE  = 0x40000020;

#endif
