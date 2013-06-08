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

#include "TextFormatter/mgTextDraw.h"

//-----------------------------------------------------------------
// constructor
mgTextDraw::mgTextDraw(
  mgTextBuffer* buffer,               // text to format
  mgTextPage* page,                   // formatting target
  mgRectangle &clip,                  // clipping rectangle
  int originX,                        // origin of output
  int originY, 
  int pageWidth)                      // target page width      
: mgTextFormat(buffer, page, false, originX, originY, pageWidth)
{
  m_clip = clip;
  m_originX = originX;
  m_originY = originY;
}

//-----------------------------------------------------------------
// destructor
mgTextDraw::~mgTextDraw()
{
}

//-----------------------------------------------------------------
// draw a line of text, return false to stop formatting
void mgTextDraw::outputLine(
  int left,                    // x position 
  int top,                     // y position
  int height,                  // height of line
  int baseline,                // baseline of text
  mgPtrArray& lineFrags)       // line of fragments to write
{
  int y = m_originY + top;
  if (y+height < m_clip.top())
    return;

  if (y > m_clip.bottom())
  {
    m_clipped = true;  // off bottom
    return;
  }

  for (int i = 0; i < lineFrags.length(); i++)
  {
    mgFragDesc* frag = (mgFragDesc*) lineFrags[i];
    int x = m_originX + left + frag->m_x;
    int y = m_originY + top+baseline;

    m_page->drawString(frag->m_font, frag->m_color, x, y, frag->m_text, frag->m_len);

#ifdef WORKED
    // draw underline for anchor
    if (frag->m_anchor != NULL)
    {
      int descent = m_pane->getLinkDescent();
//      m_pane->drawLine(frag->m_color, x, y+descent, 
//        x+frag->m_width, y+descent);
    }
#endif
  }
}
