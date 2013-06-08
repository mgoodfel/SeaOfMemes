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
 * \brief mgNonZeroMask is used for tracking the mask data of non-zero winding buffer.
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */


#ifndef NON_ZERO_MASK_H
#define NON_ZERO_MASK_H

// Main define for data format
// Default value is char for data.
// #define NON_ZERO_MASK_USE_SHORT_DATA

// More defines built from these
#ifndef NON_ZERO_MASK_USE_SHORT_DATA
#define NON_ZERO_MASK_DATA_UNIT char
#define NON_ZERO_MASK_DATA_BITS 8
#else
#define NON_ZERO_MASK_DATA_UNIT short
#define NON_ZERO_MASK_DATA_BITS 16
#endif

//! A struct for tracking the mask data of non-zero winding buffer.
typedef struct mgNonZeroMask
{
    SUBPIXEL_DATA mMask;
    NON_ZERO_MASK_DATA_UNIT mBuffer[SUBPIXEL_COUNT];
} mgNonZeroMask;

#endif // !NON_ZERO_MASK_H
