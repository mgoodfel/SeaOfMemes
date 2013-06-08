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
 * \brief SubPixel defines
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */


#ifndef SUBPIXEL_H
#define SUBPIXEL_H

//! A global define for all fillers adjusting the subpixel count.
/*! Valid values are 3 (for 8 samples) 4 (for 16 samples) and 5 (for 32 samples).
 */
#define SUBPIXEL_SHIFT 3

/*
 8x8 sparse supersampling mask:

 [][][][][]##[][] 5
 ##[][][][][][][] 0
 [][][]##[][][][] 3
 [][][][][][]##[] 6
 []##[][][][][][] 1
 [][][][]##[][][] 4
 [][][][][][][]## 7
 [][]##[][][][][] 2

 16x16 sparse supersampling mask:

 []##[][][][][][][][][][][][][][] 1
 [][][][][][][][]##[][][][][][][] 8
 [][][][]##[][][][][][][][][][][] 4
 [][][][][][][][][][][][][][][]## 15
 [][][][][][][][][][][]##[][][][] 11
 [][]##[][][][][][][][][][][][][] 2
 [][][][][][]##[][][][][][][][][] 6
 [][][][][][][][][][][][][][]##[] 14
 [][][][][][][][][][]##[][][][][] 10
 [][][]##[][][][][][][][][][][][] 3
 [][][][][][][]##[][][][][][][][] 7 
 [][][][][][][][][][][][]##[][][] 12
 ##[][][][][][][][][][][][][][][] 0
 [][][][][][][][][]##[][][][][][] 9
 [][][][][]##[][][][][][][][][][] 5
 [][][][][][][][][][][][][]##[][] 13

 32x32 sparse supersampling mask

 [][][][][][][][][][][][][][][][][][][][][][][][][][][][]##[][][] 28
 [][][][][][][][][][][][][]##[][][][][][][][][][][][][][][][][][] 13
 [][][][][][]##[][][][][][][][][][][][][][][][][][][][][][][][][] 6
 [][][][][][][][][][][][][][][][][][][][][][][]##[][][][][][][][] 23
 ##[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][] 0
 [][][][][][][][][][][][][][][][][]##[][][][][][][][][][][][][][] 17
 [][][][][][][][][][]##[][][][][][][][][][][][][][][][][][][][][] 10
 [][][][][][][][][][][][][][][][][][][][][][][][][][][]##[][][][] 27
 [][][][]##[][][][][][][][][][][][][][][][][][][][][][][][][][][] 4
 [][][][][][][][][][][][][][][][][][][][][]##[][][][][][][][][][] 21
 [][][][][][][][][][][][][][]##[][][][][][][][][][][][][][][][][] 14
 [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]## 31
 [][][][][][][][]##[][][][][][][][][][][][][][][][][][][][][][][] 8
 [][][][][][][][][][][][][][][][][][][][][][][][][]##[][][][][][] 25
 [][][][][][][][][][][][][][][][][][]##[][][][][][][][][][][][][] 18
 [][][]##[][][][][][][][][][][][][][][][][][][][][][][][][][][][] 3
 [][][][][][][][][][][][]##[][][][][][][][][][][][][][][][][][][] 12
 [][][][][][][][][][][][][][][][][][][][][][][][][][][][][]##[][] 29
 [][][][][][][][][][][][][][][][][][][][][][]##[][][][][][][][][] 22
 [][][][][][][]##[][][][][][][][][][][][][][][][][][][][][][][][] 7
 [][][][][][][][][][][][][][][][]##[][][][][][][][][][][][][][][] 16
 []##[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][] 1
 [][][][][][][][][][][][][][][][][][][][][][][][][][]##[][][][][] 26
 [][][][][][][][][][][]##[][][][][][][][][][][][][][][][][][][][] 11
 [][][][][][][][][][][][][][][][][][][][]##[][][][][][][][][][][] 20
 [][][][][]##[][][][][][][][][][][][][][][][][][][][][][][][][][] 5
 [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]##[] 30
 [][][][][][][][][][][][][][][]##[][][][][][][][][][][][][][][][] 15
 [][][][][][][][][][][][][][][][][][][][][][][][]##[][][][][][][] 24
 [][][][][][][][][]##[][][][][][][][][][][][][][][][][][][][][][] 9
 [][]##[][][][][][][][][][][][][][][][][][][][][][][][][][][][][] 2
 [][][][][][][][][][][][][][][][][][][]##[][][][][][][][][][][][] 19
*/

#define SUBPIXEL_COUNT (1 << SUBPIXEL_SHIFT)

#if SUBPIXEL_SHIFT == 3
#define SUBPIXEL_DATA unsigned char
#define WINDING_DATA unsigned __int64
#define SUBPIXEL_COVERAGE(a) (mgCoverageTable[(a)])

#define SUBPIXEL_OFFSET_0 (5.0f/8.0f)
#define SUBPIXEL_OFFSET_1 (0.0f/8.0f)
#define SUBPIXEL_OFFSET_2 (3.0f/8.0f)
#define SUBPIXEL_OFFSET_3 (6.0f/8.0f)
#define SUBPIXEL_OFFSET_4 (1.0f/8.0f)
#define SUBPIXEL_OFFSET_5 (4.0f/8.0f)
#define SUBPIXEL_OFFSET_6 (7.0f/8.0f)
#define SUBPIXEL_OFFSET_7 (2.0f/8.0f)

#define SUBPIXEL_OFFSET_RATIONAL_0 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_RATIONAL_1 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_RATIONAL_2 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_RATIONAL_3 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_RATIONAL_4 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_RATIONAL_5 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_RATIONAL_6 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_RATIONAL_7 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_7)

#define SUBPIXEL_OFFSET_FIXED_0 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_FIXED_1 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_FIXED_2 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_FIXED_3 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_FIXED_4 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_FIXED_5 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_FIXED_6 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_FIXED_7 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_7)

#define SUBPIXEL_OFFSETS { SUBPIXEL_OFFSET_RATIONAL_0,\
                           SUBPIXEL_OFFSET_RATIONAL_1,\
                           SUBPIXEL_OFFSET_RATIONAL_2,\
                           SUBPIXEL_OFFSET_RATIONAL_3,\
                           SUBPIXEL_OFFSET_RATIONAL_4,\
                           SUBPIXEL_OFFSET_RATIONAL_5,\
                           SUBPIXEL_OFFSET_RATIONAL_6,\
                           SUBPIXEL_OFFSET_RATIONAL_7 }

#define SUBPIXEL_OFFSETS_FIXED { SUBPIXEL_OFFSET_FIXED_0,\
                                 SUBPIXEL_OFFSET_FIXED_1,\
                                 SUBPIXEL_OFFSET_FIXED_2,\
                                 SUBPIXEL_OFFSET_FIXED_3,\
                                 SUBPIXEL_OFFSET_FIXED_4,\
                                 SUBPIXEL_OFFSET_FIXED_5,\
                                 SUBPIXEL_OFFSET_FIXED_6,\
                                 SUBPIXEL_OFFSET_FIXED_7 }

#define SUBPIXEL_FULL_COVERAGE 0xff
#elif SUBPIXEL_SHIFT == 4
#define SUBPIXEL_DATA unsigned short
#define SUBPIXEL_COVERAGE(a) (mgCoverageTable[(a) & 0xff] + mgCoverageTable[((a) >> 8) & 0xff])

#define SUBPIXEL_OFFSET_0  (1.0f/16.0f)
#define SUBPIXEL_OFFSET_1  (8.0f/16.0f)
#define SUBPIXEL_OFFSET_2  (4.0f/16.0f)
#define SUBPIXEL_OFFSET_3  (15.0f/16.0f)
#define SUBPIXEL_OFFSET_4  (11.0f/16.0f)
#define SUBPIXEL_OFFSET_5  (2.0f/16.0f)
#define SUBPIXEL_OFFSET_6  (6.0f/16.0f)
#define SUBPIXEL_OFFSET_7  (14.0f/16.0f)
#define SUBPIXEL_OFFSET_8  (10.0f/16.0f)
#define SUBPIXEL_OFFSET_9  (3.0f/16.0f)
#define SUBPIXEL_OFFSET_10 (7.0f/16.0f)
#define SUBPIXEL_OFFSET_11 (12.0f/16.0f)
#define SUBPIXEL_OFFSET_12 (0.0f/16.0f)
#define SUBPIXEL_OFFSET_13 (9.0f/16.0f)
#define SUBPIXEL_OFFSET_14 (5.0f/16.0f)
#define SUBPIXEL_OFFSET_15 (13.0f/16.0f)

#define SUBPIXEL_OFFSET_RATIONAL_0 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_RATIONAL_1 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_RATIONAL_2 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_RATIONAL_3 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_RATIONAL_4 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_RATIONAL_5 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_RATIONAL_6 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_RATIONAL_7 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_7)
#define SUBPIXEL_OFFSET_RATIONAL_8 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_8)
#define SUBPIXEL_OFFSET_RATIONAL_9 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_9)
#define SUBPIXEL_OFFSET_RATIONAL_10 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_10)
#define SUBPIXEL_OFFSET_RATIONAL_11 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_11)
#define SUBPIXEL_OFFSET_RATIONAL_12 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_12)
#define SUBPIXEL_OFFSET_RATIONAL_13 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_13)
#define SUBPIXEL_OFFSET_RATIONAL_14 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_14)
#define SUBPIXEL_OFFSET_RATIONAL_15 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_15)

#define SUBPIXEL_OFFSET_FIXED_0 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_FIXED_1 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_FIXED_2 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_FIXED_3 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_FIXED_4 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_FIXED_5 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_FIXED_6 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_FIXED_7 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_7)
#define SUBPIXEL_OFFSET_FIXED_8 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_8)
#define SUBPIXEL_OFFSET_FIXED_9 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_9)
#define SUBPIXEL_OFFSET_FIXED_10 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_10)
#define SUBPIXEL_OFFSET_FIXED_11 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_11)
#define SUBPIXEL_OFFSET_FIXED_12 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_12)
#define SUBPIXEL_OFFSET_FIXED_13 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_13)
#define SUBPIXEL_OFFSET_FIXED_14 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_14)
#define SUBPIXEL_OFFSET_FIXED_15 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_15)

#define SUBPIXEL_OFFSETS { SUBPIXEL_OFFSET_RATIONAL_0,\
                           SUBPIXEL_OFFSET_RATIONAL_1,\
                           SUBPIXEL_OFFSET_RATIONAL_2,\
                           SUBPIXEL_OFFSET_RATIONAL_3,\
                           SUBPIXEL_OFFSET_RATIONAL_4,\
                           SUBPIXEL_OFFSET_RATIONAL_5,\
                           SUBPIXEL_OFFSET_RATIONAL_6,\
                           SUBPIXEL_OFFSET_RATIONAL_7,\
                           SUBPIXEL_OFFSET_RATIONAL_8,\
                           SUBPIXEL_OFFSET_RATIONAL_9,\
                           SUBPIXEL_OFFSET_RATIONAL_10,\
                           SUBPIXEL_OFFSET_RATIONAL_11,\
                           SUBPIXEL_OFFSET_RATIONAL_12,\
                           SUBPIXEL_OFFSET_RATIONAL_13,\
                           SUBPIXEL_OFFSET_RATIONAL_14,\
                           SUBPIXEL_OFFSET_RATIONAL_15 }

#define SUBPIXEL_OFFSETS_FIXED { SUBPIXEL_OFFSET_FIXED_0,\
                                 SUBPIXEL_OFFSET_FIXED_1,\
                                 SUBPIXEL_OFFSET_FIXED_2,\
                                 SUBPIXEL_OFFSET_FIXED_3,\
                                 SUBPIXEL_OFFSET_FIXED_4,\
                                 SUBPIXEL_OFFSET_FIXED_5,\
                                 SUBPIXEL_OFFSET_FIXED_6,\
                                 SUBPIXEL_OFFSET_FIXED_7,\
                                 SUBPIXEL_OFFSET_FIXED_8,\
                                 SUBPIXEL_OFFSET_FIXED_9,\
                                 SUBPIXEL_OFFSET_FIXED_10,\
                                 SUBPIXEL_OFFSET_FIXED_11,\
                                 SUBPIXEL_OFFSET_FIXED_12,\
                                 SUBPIXEL_OFFSET_FIXED_13,\
                                 SUBPIXEL_OFFSET_FIXED_14,\
                                 SUBPIXEL_OFFSET_FIXED_15 }

#define SUBPIXEL_FULL_COVERAGE 0xffff
#elif SUBPIXEL_SHIFT == 5
#define SUBPIXEL_DATA DWORD
#define SUBPIXEL_COVERAGE(a) (mgCoverageTable[(a) & 0xff] + mgCoverageTable[((a) >> 8) & 0xff] + mgCoverageTable[((a) >> 16) & 0xff] + mgCoverageTable[((a) >> 24) & 0xff])

#define SUBPIXEL_OFFSET_0  (28.0f/32.0f)
#define SUBPIXEL_OFFSET_1  (13.0f/32.0f)
#define SUBPIXEL_OFFSET_2  (6.0f/32.0f)
#define SUBPIXEL_OFFSET_3  (23.0f/32.0f)
#define SUBPIXEL_OFFSET_4  (0.0f/32.0f)
#define SUBPIXEL_OFFSET_5  (17.0f/32.0f)
#define SUBPIXEL_OFFSET_6  (10.0f/32.0f)
#define SUBPIXEL_OFFSET_7  (27.0f/32.0f)
#define SUBPIXEL_OFFSET_8  (4.0f/32.0f)
#define SUBPIXEL_OFFSET_9  (21.0f/32.0f)
#define SUBPIXEL_OFFSET_10 (14.0f/32.0f)
#define SUBPIXEL_OFFSET_11 (31.0f/32.0f)
#define SUBPIXEL_OFFSET_12 (8.0f/32.0f)
#define SUBPIXEL_OFFSET_13 (25.0f/32.0f)
#define SUBPIXEL_OFFSET_14 (18.0f/32.0f)
#define SUBPIXEL_OFFSET_15 (3.0f/32.0f)
#define SUBPIXEL_OFFSET_16 (12.0f/32.0f)
#define SUBPIXEL_OFFSET_17 (29.0f/32.0f)
#define SUBPIXEL_OFFSET_18 (22.0f/32.0f)
#define SUBPIXEL_OFFSET_19 (7.0f/32.0f)
#define SUBPIXEL_OFFSET_20 (16.0f/32.0f)
#define SUBPIXEL_OFFSET_21 (1.0f/32.0f)
#define SUBPIXEL_OFFSET_22 (26.0f/32.0f)
#define SUBPIXEL_OFFSET_23 (11.0f/32.0f)
#define SUBPIXEL_OFFSET_24 (20.0f/32.0f)
#define SUBPIXEL_OFFSET_25 (5.0f/32.0f)
#define SUBPIXEL_OFFSET_26 (30.0f/32.0f)
#define SUBPIXEL_OFFSET_27 (15.0f/32.0f)
#define SUBPIXEL_OFFSET_28 (24.0f/32.0f)
#define SUBPIXEL_OFFSET_29 (9.0f/32.0f)
#define SUBPIXEL_OFFSET_30 (2.0f/32.0f)
#define SUBPIXEL_OFFSET_31 (19.0f/32.0f)

#define SUBPIXEL_OFFSET_RATIONAL_0 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_RATIONAL_1 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_RATIONAL_2 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_RATIONAL_3 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_RATIONAL_4 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_RATIONAL_5 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_RATIONAL_6 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_RATIONAL_7 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_7)
#define SUBPIXEL_OFFSET_RATIONAL_8 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_8)
#define SUBPIXEL_OFFSET_RATIONAL_9 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_9)
#define SUBPIXEL_OFFSET_RATIONAL_10 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_10)
#define SUBPIXEL_OFFSET_RATIONAL_11 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_11)
#define SUBPIXEL_OFFSET_RATIONAL_12 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_12)
#define SUBPIXEL_OFFSET_RATIONAL_13 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_13)
#define SUBPIXEL_OFFSET_RATIONAL_14 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_14)
#define SUBPIXEL_OFFSET_RATIONAL_15 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_15)
#define SUBPIXEL_OFFSET_RATIONAL_16 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_16)
#define SUBPIXEL_OFFSET_RATIONAL_17 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_17)
#define SUBPIXEL_OFFSET_RATIONAL_18 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_18)
#define SUBPIXEL_OFFSET_RATIONAL_19 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_19)
#define SUBPIXEL_OFFSET_RATIONAL_20 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_20)
#define SUBPIXEL_OFFSET_RATIONAL_21 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_21)
#define SUBPIXEL_OFFSET_RATIONAL_22 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_22)
#define SUBPIXEL_OFFSET_RATIONAL_23 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_23)
#define SUBPIXEL_OFFSET_RATIONAL_24 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_24)
#define SUBPIXEL_OFFSET_RATIONAL_25 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_25)
#define SUBPIXEL_OFFSET_RATIONAL_26 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_26)
#define SUBPIXEL_OFFSET_RATIONAL_27 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_27)
#define SUBPIXEL_OFFSET_RATIONAL_28 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_28)
#define SUBPIXEL_OFFSET_RATIONAL_29 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_29)
#define SUBPIXEL_OFFSET_RATIONAL_30 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_30)
#define SUBPIXEL_OFFSET_RATIONAL_31 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_31)

#define SUBPIXEL_OFFSET_FIXED_0 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_FIXED_1 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_FIXED_2 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_FIXED_3 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_FIXED_4 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_FIXED_5 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_FIXED_6 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_FIXED_7 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_7)
#define SUBPIXEL_OFFSET_FIXED_8 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_8)
#define SUBPIXEL_OFFSET_FIXED_9 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_9)
#define SUBPIXEL_OFFSET_FIXED_10 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_10)
#define SUBPIXEL_OFFSET_FIXED_11 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_11)
#define SUBPIXEL_OFFSET_FIXED_12 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_12)
#define SUBPIXEL_OFFSET_FIXED_13 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_13)
#define SUBPIXEL_OFFSET_FIXED_14 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_14)
#define SUBPIXEL_OFFSET_FIXED_15 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_15)
#define SUBPIXEL_OFFSET_FIXED_16 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_16)
#define SUBPIXEL_OFFSET_FIXED_17 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_17)
#define SUBPIXEL_OFFSET_FIXED_18 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_18)
#define SUBPIXEL_OFFSET_FIXED_19 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_19)
#define SUBPIXEL_OFFSET_FIXED_20 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_20)
#define SUBPIXEL_OFFSET_FIXED_21 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_21)
#define SUBPIXEL_OFFSET_FIXED_22 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_22)
#define SUBPIXEL_OFFSET_FIXED_23 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_23)
#define SUBPIXEL_OFFSET_FIXED_24 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_24)
#define SUBPIXEL_OFFSET_FIXED_25 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_25)
#define SUBPIXEL_OFFSET_FIXED_26 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_26)
#define SUBPIXEL_OFFSET_FIXED_27 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_27)
#define SUBPIXEL_OFFSET_FIXED_28 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_28)
#define SUBPIXEL_OFFSET_FIXED_29 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_29)
#define SUBPIXEL_OFFSET_FIXED_30 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_30)
#define SUBPIXEL_OFFSET_FIXED_31 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_31)

#define SUBPIXEL_OFFSETS { SUBPIXEL_OFFSET_RATIONAL_0,\
                           SUBPIXEL_OFFSET_RATIONAL_1,\
                           SUBPIXEL_OFFSET_RATIONAL_2,\
                           SUBPIXEL_OFFSET_RATIONAL_3,\
                           SUBPIXEL_OFFSET_RATIONAL_4,\
                           SUBPIXEL_OFFSET_RATIONAL_5,\
                           SUBPIXEL_OFFSET_RATIONAL_6,\
                           SUBPIXEL_OFFSET_RATIONAL_7,\
                           SUBPIXEL_OFFSET_RATIONAL_8,\
                           SUBPIXEL_OFFSET_RATIONAL_9,\
                           SUBPIXEL_OFFSET_RATIONAL_10,\
                           SUBPIXEL_OFFSET_RATIONAL_11,\
                           SUBPIXEL_OFFSET_RATIONAL_12,\
                           SUBPIXEL_OFFSET_RATIONAL_13,\
                           SUBPIXEL_OFFSET_RATIONAL_14,\
                           SUBPIXEL_OFFSET_RATIONAL_15,\
                           SUBPIXEL_OFFSET_RATIONAL_16,\
                           SUBPIXEL_OFFSET_RATIONAL_17,\
                           SUBPIXEL_OFFSET_RATIONAL_18,\
                           SUBPIXEL_OFFSET_RATIONAL_19,\
                           SUBPIXEL_OFFSET_RATIONAL_20,\
                           SUBPIXEL_OFFSET_RATIONAL_21,\
                           SUBPIXEL_OFFSET_RATIONAL_22,\
                           SUBPIXEL_OFFSET_RATIONAL_23,\
                           SUBPIXEL_OFFSET_RATIONAL_24,\
                           SUBPIXEL_OFFSET_RATIONAL_25,\
                           SUBPIXEL_OFFSET_RATIONAL_26,\
                           SUBPIXEL_OFFSET_RATIONAL_27,\
                           SUBPIXEL_OFFSET_RATIONAL_28,\
                           SUBPIXEL_OFFSET_RATIONAL_29,\
                           SUBPIXEL_OFFSET_RATIONAL_30,\
                           SUBPIXEL_OFFSET_RATIONAL_31 }

#define SUBPIXEL_OFFSETS_FIXED { SUBPIXEL_OFFSET_FIXED_0,\
                                 SUBPIXEL_OFFSET_FIXED_1,\
                                 SUBPIXEL_OFFSET_FIXED_2,\
                                 SUBPIXEL_OFFSET_FIXED_3,\
                                 SUBPIXEL_OFFSET_FIXED_4,\
                                 SUBPIXEL_OFFSET_FIXED_5,\
                                 SUBPIXEL_OFFSET_FIXED_6,\
                                 SUBPIXEL_OFFSET_FIXED_7,\
                                 SUBPIXEL_OFFSET_FIXED_8,\
                                 SUBPIXEL_OFFSET_FIXED_9,\
                                 SUBPIXEL_OFFSET_FIXED_10,\
                                 SUBPIXEL_OFFSET_FIXED_11,\
                                 SUBPIXEL_OFFSET_FIXED_12,\
                                 SUBPIXEL_OFFSET_FIXED_13,\
                                 SUBPIXEL_OFFSET_FIXED_14,\
                                 SUBPIXEL_OFFSET_FIXED_15,\
                                 SUBPIXEL_OFFSET_FIXED_16,\
                                 SUBPIXEL_OFFSET_FIXED_17,\
                                 SUBPIXEL_OFFSET_FIXED_18,\
                                 SUBPIXEL_OFFSET_FIXED_19,\
                                 SUBPIXEL_OFFSET_FIXED_20,\
                                 SUBPIXEL_OFFSET_FIXED_21,\
                                 SUBPIXEL_OFFSET_FIXED_22,\
                                 SUBPIXEL_OFFSET_FIXED_23,\
                                 SUBPIXEL_OFFSET_FIXED_24,\
                                 SUBPIXEL_OFFSET_FIXED_25,\
                                 SUBPIXEL_OFFSET_FIXED_26,\
                                 SUBPIXEL_OFFSET_FIXED_27,\
                                 SUBPIXEL_OFFSET_FIXED_28,\
                                 SUBPIXEL_OFFSET_FIXED_29,\
                                 SUBPIXEL_OFFSET_FIXED_30,\
                                 SUBPIXEL_OFFSET_FIXED_31 }

#define SUBPIXEL_FULL_COVERAGE 0xffffffff
#endif

#endif
