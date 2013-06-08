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
 * \brief PolygonUnrollB.h contains defines for unrolled loops. This version uses immediate data for everything.
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */


#ifndef POLYGON_UNROLL_B_H_INCLUDED
#define POLYGON_UNROLL_B_H_INCLUDED

#define EVENODD_LINE_UNROLL_INIT() \
        int __evenodd_line_xp__;

#define EVENODD_LINE_UNROLL_0(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_0; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x01;

#define EVENODD_LINE_UNROLL_1(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_1; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x02;

#define EVENODD_LINE_UNROLL_2(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_2; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x04;

#define EVENODD_LINE_UNROLL_3(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_3; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x08;

#define EVENODD_LINE_UNROLL_4(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_4; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x10;

#define EVENODD_LINE_UNROLL_5(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_5; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x20;

#define EVENODD_LINE_UNROLL_6(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_6; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x40;

#define EVENODD_LINE_UNROLL_7(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_7; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x80;

#if SUBPIXEL_COUNT > 8

#define EVENODD_LINE_UNROLL_8(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_8; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x0100;

#define EVENODD_LINE_UNROLL_9(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_9; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x0200;

#define EVENODD_LINE_UNROLL_10(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_10; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x0400;

#define EVENODD_LINE_UNROLL_11(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_11; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x0800;

#define EVENODD_LINE_UNROLL_12(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_12; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x1000;

#define EVENODD_LINE_UNROLL_13(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_13; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x2000;

#define EVENODD_LINE_UNROLL_14(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_14; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x4000;

#define EVENODD_LINE_UNROLL_15(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_15; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x8000;

#endif // !SUBPIXEL_COUNT > 8

#if SUBPIXEL_COUNT > 16

#define EVENODD_LINE_UNROLL_16(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_16; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00010000;

#define EVENODD_LINE_UNROLL_17(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_17; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00020000;

#define EVENODD_LINE_UNROLL_18(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_18; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00040000;

#define EVENODD_LINE_UNROLL_19(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_19; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00080000;

#define EVENODD_LINE_UNROLL_20(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_20; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00100000;

#define EVENODD_LINE_UNROLL_21(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_21; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00200000;

#define EVENODD_LINE_UNROLL_22(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_22; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00400000;

#define EVENODD_LINE_UNROLL_23(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_23; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00800000;

#define EVENODD_LINE_UNROLL_24(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_24; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x01000000;

#define EVENODD_LINE_UNROLL_25(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_25; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x02000000;

#define EVENODD_LINE_UNROLL_26(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_26; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x04000000;

#define EVENODD_LINE_UNROLL_27(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_27; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x08000000;

#define EVENODD_LINE_UNROLL_28(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_28; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x10000000;

#define EVENODD_LINE_UNROLL_29(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_29; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x20000000;

#define EVENODD_LINE_UNROLL_30(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_30; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x40000000;

#define EVENODD_LINE_UNROLL_31(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_31; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x80000000;

#endif // !SUBPIXEL_COUNT > 16

// Macros for non-zero fill.

#define NONZERO_LINE_UNROLL_INIT() \
        int __nonzero_line_xp__;

#define NONZERO_LINE_UNROLL_0(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_0; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x01; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[0] += aWinding;

#define NONZERO_LINE_UNROLL_1(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_1; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x02; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[1] += aWinding;

#define NONZERO_LINE_UNROLL_2(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_2; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x04; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[2] += aWinding;

#define NONZERO_LINE_UNROLL_3(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_3; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x08; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[3] += aWinding;

#define NONZERO_LINE_UNROLL_4(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_4; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x10; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[4] += aWinding;

#define NONZERO_LINE_UNROLL_5(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_5; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x20; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[5] += aWinding;

#define NONZERO_LINE_UNROLL_6(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_6; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x40; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[6] += aWinding;

#define NONZERO_LINE_UNROLL_7(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_7; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x80; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[7] += aWinding;

#if SUBPIXEL_COUNT > 8

#define NONZERO_LINE_UNROLL_8(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_8; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x0100; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[8] += aWinding;

#define NONZERO_LINE_UNROLL_9(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_9; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x0200; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[9] += aWinding;

#define NONZERO_LINE_UNROLL_10(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_10; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x0400; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[10] += aWinding;

#define NONZERO_LINE_UNROLL_11(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_11; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x0800; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[11] += aWinding;

#define NONZERO_LINE_UNROLL_12(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_12; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x1000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[12] += aWinding;

#define NONZERO_LINE_UNROLL_13(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_13; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x2000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[13] += aWinding;

#define NONZERO_LINE_UNROLL_14(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_14; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x4000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[14] += aWinding;

#define NONZERO_LINE_UNROLL_15(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_15; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x8000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[15] += aWinding;

#endif // !SUBPIXEL_COUNT > 8

#if SUBPIXEL_COUNT > 16

#define NONZERO_LINE_UNROLL_16(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_16; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00010000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[16] += aWinding;

#define NONZERO_LINE_UNROLL_17(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_17; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00020000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[17] += aWinding;

#define NONZERO_LINE_UNROLL_18(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_18; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00040000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[18] += aWinding;

#define NONZERO_LINE_UNROLL_19(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_19; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00080000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[19] += aWinding;

#define NONZERO_LINE_UNROLL_20(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_20; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00100000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[20] += aWinding;

#define NONZERO_LINE_UNROLL_21(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_21; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00200000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[21] += aWinding;

#define NONZERO_LINE_UNROLL_22(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_22; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00400000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[22] += aWinding;

#define NONZERO_LINE_UNROLL_23(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_23; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00800000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[23] += aWinding;

#define NONZERO_LINE_UNROLL_24(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_24; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x01000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[24] += aWinding;

#define NONZERO_LINE_UNROLL_25(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_25; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x02000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[25] += aWinding;

#define NONZERO_LINE_UNROLL_26(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_26; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x04000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[26] += aWinding;

#define NONZERO_LINE_UNROLL_27(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_27; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x08000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[27] += aWinding;

#define NONZERO_LINE_UNROLL_28(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_28; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x10000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[28] += aWinding;

#define NONZERO_LINE_UNROLL_29(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_29; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x20000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[29] += aWinding;

#define NONZERO_LINE_UNROLL_30(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_30; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x40000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[30] += aWinding;

#define NONZERO_LINE_UNROLL_31(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_31; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x80000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[31] += aWinding;

#endif // !SUBPIXEL_COUNT > 16

#endif // !POLYGON_UNROLL_B_H_INCLUDED
