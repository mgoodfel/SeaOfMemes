/*!
 * \file
 * Scanline edge-flag algorithm for antialiasing <br>
 * Copyright (c) 2005-2007 Kiia Kallio
 *
 * http://mlab.uiah.fi/~kkallio/antialiasing/
 * 
 * This code is distributed under the three-clause BSD license.
 * Read the LICENSE file or visit the URL above for details.
 *
 * \brief mgSpanExtents is a class for keeping track of minimum and maximum values of a span.
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */


#ifndef SPAN_EXTENTS_H_INCLUDED
#define SPAN_EXTENTS_H_INCLUDED

//! A class for keeping track of minimum and maximum values of a span.
class mgSpanExtents
{
public:
    // Marks a span. aStart and aEnd should be sorted.
    inline void mark(int aStart, int aEnd)
    {
        if (aStart < mMinimum)
            mMinimum = aStart;
        if (aEnd > mMaximum)
            mMaximum = aEnd;
    }

    // Marks a span. aStart and aEnd don't have to be sorted.
    inline void markWithSort(int aStart, int aEnd)
    {
        if (aStart <= aEnd)
            mark(aStart,aEnd);
        else
            mark(aEnd,aStart);
    }

    inline void reset()
    {
        mMinimum = 0x7fffffff;
        mMaximum = 0x80000000;
    }

    int mMinimum;
    int mMaximum;
};

#endif // !SPAN_EXTENTS_H_INCLUDED
