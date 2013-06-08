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
 * \brief mgNonZeroMaskC is a class for tracking the mask data of non-zero winding buffer.
 *
 *
 * $Id: $
 * $Date: $
 * $Revision: $
 */


#ifndef NON_ZERO_MASK_C_H
#define NON_ZERO_MASK_C_H

// Define for choosing 64-bit composite size instead of 32-bit
// This has speed effect only if the executable is built for 64-bit processor,
// otherwise 64-bit values are in fact handled as two 32-bit values.
// #define NON_ZERO_MASK_USE_UINT64_COMPOSITE


#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_UNIT unsigned long
#else
#define NON_ZERO_MASK_COMPOSITE_UNIT unsigned __int64
#endif


// Possible values for composite count
// data      composite     count
// 1         4             8            = 2
// 1         4             16           = 4
// 1         4             32           = 8
// 1         8             8            = 1
// 1         8             16           = 2
// 1         8             32           = 4
// 2         4             8            = 4
// 2         4             16           = 8
// 2         4             32           = 16
// 2         8             8            = 2
// 2         8             16           = 4
// 2         8             32           = 8

// These are the macros for packing the toggle bits from the composite values to lowest
// bits of the integer. These depend on the subpixel count, non-zero mask data size
// and the composite data type. The bits are packed down with the maximum efficiency,
// not caring about their order. This means that the scheme works only with unweighted
// sampling. If each sample has different weight, the weight calculations has to take
// the order into account.

#if SUBPIXEL_COUNT == 8
#ifndef NON_ZERO_MASK_USE_SHORT_DATA
        // byte per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 2
        // long per composite
        // pattern is ......AB......CD......EF......GH
        // a |= a >> 14;
        // pattern is ......AB......CD....ABEF....CDGH
        // a |= a >> 4;
        // pattern is ......AB......CD....ABEFABEFCDGH
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 14; (a) |= (a) >> 4;
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 1
        // uint64 per composite
        // pattern is .......A.......B.......C.......D.......E.......F.......G.......H
        // a |= a >> 31;
        // pattern is .......A.......B.......C.......D......AE......BF......CG......DH
        // a |= a >> 14;
        // pattern is .......A.......B.......C.......D......AE......BF....AECG....BFDH
        // a |= a >> 4;
        // pattern is .......A.......B.......C.......D......AE......BF....AECGAECGBFDH
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 31; (a) |= (a) >> 14; (a) |= (a) >> 4;
#endif
#else
        // short per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 4
        // long per composite
        // pattern is ............ABCD............EFGH
        // a |= a >> 12;
        // pattern is ............ABCD........ABCDEFGH
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 12;
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 2
        // uint64 per composite
        // pattern is ..............AB..............CD..............EF..............GH
        // a |= a >> 30;
        // pattern is ..............AB..............CD............ABEF............CDGH
        // a |= a >> 12;
        // pattern is ..............AB..............CD............ABEF........ABEFCDGH
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 30; (a) |= (a) >> 12;
#endif
#endif
#endif

#if SUBPIXEL_COUNT == 16
#ifndef NON_ZERO_MASK_USE_SHORT_DATA
        // byte per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 4
        // long per composite
        // pattern is ....ABCD....EFGH....IJKL....MNOP
        // a |= a >> 12;
        // pattern is ....ABCD....EFGHABCDIJKLEFGHMNOP
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 12;
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 2
        // uint64 per composite
        // pattern is ......AB......CD......EF......GH......IJ......KL......MN......OP
        // a |= a >> 30;
        // pattern is ......AB......CD......EF......GH....ABIJ....CDKL....EFMN....GHOP
        // a |= a >> 12;
        // pattern is ......AB......CD......EF......GH....ABIJ....CDKLABIJEFMNCDKLGHOP
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 30; (a) |= (a) >> 12;
#endif
#else
        // short per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 8
        // long per composite
        // pattern is ........ABCDEFGH........IJKLMNOP
        // a |= a >> 8;
        // pattern is ........ABCDEFGHABCDEFGHIJKLMNOP
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 8;
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 4
        // uint64 per composite
        // pattern is ............ABCD............EFGH............IJKL............MNOP
        // a |= a >> 28;
        // pattern is ............ABCD............EFGH........ABCDIJKL........EFGHMNOP
        // a |= a >> 8;
        // pattern is ............ABCD............EFGH........ABCDIJKLABCDIJKLEFGHMNOP
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 28; (a) |= (a) >> 8;
#endif
#endif
#endif

#if SUBPIXEL_COUNT == 32
#ifndef NON_ZERO_MASK_USE_SHORT_DATA
        // byte per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 8
        // long per composite
        // pattern is ABCDEFGHIJKLMNOPQRSTUVWXYZ123456
#define NON_ZERO_MASK_COMPOSE_MASK(a)
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 4
        // uint64 per composite
        // pattern is ....ABCD....EFGH....IJKL....MNOP....QRST....UVWX....YZ12....3456
        // tmask |= tmask >> 28;
        // pattern is ....ABCD....EFGH....IJKL....MNOPABCDQRSTEFGHUVWXIJKLYZ12MNOP3456
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 28;
#endif
#else
        // short per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 16
        // long per composite
        // pattern is ABCDEFGHIJKLMNOPQRSTUVWXYZ123456
#define NON_ZERO_MASK_COMPOSE_MASK(a)
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 8
        // uint64 per composite
        // pattern is ........ABCDEFGH........IJKLMNOP........QRSTUVWX........YZ123456
        tmask |= tmask >> 24;
        // pattern is ........ABCDEFGH........IJKLMNOPABCDEFGHQRSTUVWXIJKLMNOPYZ123456
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 24;
#endif
#endif
#endif

//! A structure for containing the composite mask data, definable to various sizes.
typedef struct NonZeroMaskCompositeData
{
    NON_ZERO_MASK_COMPOSITE_UNIT mValue0;
#if NON_ZERO_MASK_COMPOSITE_COUNT > 1
    NON_ZERO_MASK_COMPOSITE_UNIT mValue1;
#endif
#if NON_ZERO_MASK_COMPOSITE_COUNT > 2
    NON_ZERO_MASK_COMPOSITE_UNIT mValue2;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue3;
#endif
#if NON_ZERO_MASK_COMPOSITE_COUNT > 4
    NON_ZERO_MASK_COMPOSITE_UNIT mValue4;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue5;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue6;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue7;
#endif
#if NON_ZERO_MASK_COMPOSITE_COUNT > 8
    NON_ZERO_MASK_COMPOSITE_UNIT mValue8;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue9;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue10;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue11;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue12;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue13;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue14;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue15;
#endif
} NonZeroMaskCompositeData;


#ifndef NON_ZERO_MASK_USE_SHORT_DATA
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
static const DWORD NonZeroMaskHighBitOff = 0x7f7f7f7f;
static const DWORD NonZeroMaskLowBitsOff = 0x80808080;
#else // NON_ZERO_MASK_USE_UINT64_COMPOSITE
static const unsigned __int64 NonZeroMaskHighBitOff = 0x7f7f7f7f7f7f7f7f;
static const unsigned __int64 NonZeroMaskLowBitsOff = 0x8080808080808080;
#endif // NON_ZERO_MASK_USE_UINT64_COMPOSITE
#else // NON_ZERO_MASK_USE_SHORT_DATA
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
static const DWORD NonZeroMaskHighBitOff = 0x7fff7fff;
static const DWORD NonZeroMaskLowBitsOff = 0x80008000;
#else // NON_ZERO_MASK_USE_UINT64_COMPOSITE
static const unsigned __int64 NonZeroMaskHighBitOff = 0x7fff7fff7fff7fff;
static const unsigned __int64 NonZeroMaskLowBitsOff = 0x8000800080008000;
#endif // NON_ZERO_MASK_USE_UINT64_COMPOSITE
#endif // NON_ZERO_MASK_USE_SHORT_DATA


//! An implementation for tracking the mask data of non-zero winding buffer.
/*! It builds the masks using parallelism within a command - for instance 32-bit integer
 *  can be used for performing 4 8-bit operations.
 */
class mgNonZeroMaskC
{
public:
    static inline void reset(mgNonZeroMask &aSource, mgNonZeroMask &aDestination)
    {
        NonZeroMaskCompositeData &sourceComposite = *((NonZeroMaskCompositeData *)aSource.mBuffer);
        NonZeroMaskCompositeData &destinationComposite = *((NonZeroMaskCompositeData *)aDestination.mBuffer);

        register NON_ZERO_MASK_COMPOSITE_UNIT tmask = 0;
        register NON_ZERO_MASK_COMPOSITE_UNIT t;

        t = sourceComposite.mValue0;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue0 = t;
        sourceComposite.mValue0 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 1);
        tmask |= t;

#if NON_ZERO_MASK_COMPOSITE_COUNT > 1
        t = sourceComposite.mValue1;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue1 = t;
        sourceComposite.mValue1 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 2);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 2
        t = sourceComposite.mValue2;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue2 = t;
        sourceComposite.mValue2 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 3);
        tmask |= t;

        t = sourceComposite.mValue3;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue3 = t;
        sourceComposite.mValue3 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 4);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 4
        t = sourceComposite.mValue4;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue4 = t;
        sourceComposite.mValue4 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 5);
        tmask |= t;

        t = sourceComposite.mValue5;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue5 = t;
        sourceComposite.mValue5 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 6);
        tmask |= t;

        t = sourceComposite.mValue6;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue6 = t;
        sourceComposite.mValue6 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 7);
        tmask |= t;

        t = sourceComposite.mValue7;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue7 = t;
        sourceComposite.mValue7 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 8
        t >>= (NON_ZERO_MASK_DATA_BITS - 8);
#endif
        tmask |= t;
#endif

// As indicated by the table earlier, this case can be reached in a single
// case: 2 bytes per unit, 4 bytes in composite, 32 bits in mask
// This means that NON_ZERO_MASK_DATA_BITS is always 16.
#if NON_ZERO_MASK_COMPOSITE_COUNT > 8
        t = sourceComposite.mValue8;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue8 = t;
        sourceComposite.mValue8 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 9);
        tmask |= t;

        t = sourceComposite.mValue9;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue9 = t;
        sourceComposite.mValue9 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 10);
        tmask |= t;

        t = sourceComposite.mValue10;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue10 = t;
        sourceComposite.mValue10 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 11);
        tmask |= t;

        t = sourceComposite.mValue11;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue11 = t;
        sourceComposite.mValue11 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 12);
        tmask |= t;

        t = sourceComposite.mValue12;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue12 = t;
        sourceComposite.mValue12 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 13);
        tmask |= t;

        t = sourceComposite.mValue13;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue13 = t;
        sourceComposite.mValue13 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 14);
        tmask |= t;

        t = sourceComposite.mValue14;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue14 = t;
        sourceComposite.mValue14 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 15);
        tmask |= t;

        t = sourceComposite.mValue15;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue15 = t;
        sourceComposite.mValue15 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 16
        t >>= (NON_ZERO_MASK_DATA_BITS - 16);
#endif
        tmask |= t;
#endif

        NON_ZERO_MASK_COMPOSE_MASK(tmask);

        aDestination.mMask = (SUBPIXEL_DATA)tmask;
        aSource.mMask = 0;
    }


    static inline void init(mgNonZeroMask &aSource, mgNonZeroMask &aDestination)
    {
        NonZeroMaskCompositeData &sourceComposite = *((NonZeroMaskCompositeData *)aSource.mBuffer);
        NonZeroMaskCompositeData &destinationComposite = *((NonZeroMaskCompositeData *)aDestination.mBuffer);

        register NON_ZERO_MASK_COMPOSITE_UNIT tmask = 0;
        register NON_ZERO_MASK_COMPOSITE_UNIT t;

        t = sourceComposite.mValue0;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue0 = t;
        sourceComposite.mValue0 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 1);
        tmask |= t;

#if NON_ZERO_MASK_COMPOSITE_COUNT > 1
        t = sourceComposite.mValue1;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue1 = t;
        sourceComposite.mValue1 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 2);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 2
        t = sourceComposite.mValue2;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue2 = t;
        sourceComposite.mValue2 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 3);
        tmask |= t;

        t = sourceComposite.mValue3;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue3 = t;
        sourceComposite.mValue3 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 4);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 4
        t = sourceComposite.mValue4;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue4 = t;
        sourceComposite.mValue4 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 5);
        tmask |= t;

        t = sourceComposite.mValue5;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue5 = t;
        sourceComposite.mValue5 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 6);
        tmask |= t;

        t = sourceComposite.mValue6;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue6 = t;
        sourceComposite.mValue6 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 7);
        tmask |= t;

        t = sourceComposite.mValue7;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue7 = t;
        sourceComposite.mValue7 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 8
        t >>= (NON_ZERO_MASK_DATA_BITS - 8);
#endif
        tmask |= t;
#endif

// As indicated by the table earlier, this case can be reached in a single
// case: 2 bytes per unit, 4 bytes in composite, 32 bits in mask
// This means that NON_ZERO_MASK_DATA_BITS is always 16.
#if NON_ZERO_MASK_COMPOSITE_COUNT > 8
        t = sourceComposite.mValue8;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue8 = t;
        sourceComposite.mValue8 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 9);
        tmask |= t;

        t = sourceComposite.mValue9;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue9 = t;
        sourceComposite.mValue9 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 10);
        tmask |= t;

        t = sourceComposite.mValue10;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue10 = t;
        sourceComposite.mValue10 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 11);
        tmask |= t;

        t = sourceComposite.mValue11;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue11 = t;
        sourceComposite.mValue11 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 12);
        tmask |= t;

        t = sourceComposite.mValue12;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue12 = t;
        sourceComposite.mValue12 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 13);
        tmask |= t;

        t = sourceComposite.mValue13;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue13 = t;
        sourceComposite.mValue13 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 14);
        tmask |= t;

        t = sourceComposite.mValue14;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue14 = t;
        sourceComposite.mValue14 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 15);
        tmask |= t;

        t = sourceComposite.mValue15;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue15 = t;
        sourceComposite.mValue15 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 16
        t >>= (NON_ZERO_MASK_DATA_BITS - 16);
#endif
        tmask |= t;
#endif

        NON_ZERO_MASK_COMPOSE_MASK(tmask);

        aDestination.mMask = (SUBPIXEL_DATA)tmask;
        aSource.mMask = 0;
    }


    static inline void apply(mgNonZeroMask &aSource, mgNonZeroMask &aDestination)
    {
        NonZeroMaskCompositeData &sourceComposite = *((NonZeroMaskCompositeData *)aSource.mBuffer);
        NonZeroMaskCompositeData &destinationComposite = *((NonZeroMaskCompositeData *)aDestination.mBuffer);

        register NON_ZERO_MASK_COMPOSITE_UNIT tmask = 0;
        register NON_ZERO_MASK_COMPOSITE_UNIT t,p;

        t = destinationComposite.mValue0 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue0 + (sourceComposite.mValue0 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue0 = 0;
        destinationComposite.mValue0 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 1);
        tmask |= t;

#if NON_ZERO_MASK_COMPOSITE_COUNT > 1
        t = destinationComposite.mValue1 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue1 + (sourceComposite.mValue1 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue1 = 0;
        destinationComposite.mValue1 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 2);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 2
        t = destinationComposite.mValue2 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue2 + (sourceComposite.mValue2 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue2 = 0;
        destinationComposite.mValue2 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 3);
        tmask |= t;

        t = destinationComposite.mValue3 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue3 + (sourceComposite.mValue3 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue3 = 0;
        destinationComposite.mValue3 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 4);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 4
        t = destinationComposite.mValue4 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue4 + (sourceComposite.mValue4 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue4 = 0;
        destinationComposite.mValue4 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 5);
        tmask |= t;

        t = destinationComposite.mValue5 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue5 + (sourceComposite.mValue5 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue5 = 0;
        destinationComposite.mValue5 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 6);
        tmask |= t;

        t = destinationComposite.mValue6 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue6 + (sourceComposite.mValue6 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue6 = 0;
        destinationComposite.mValue6 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 7);
        tmask |= t;

        t = destinationComposite.mValue7 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue7 + (sourceComposite.mValue7 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue7 = 0;
        destinationComposite.mValue7 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 8
        t >>= (NON_ZERO_MASK_DATA_BITS - 8);
#endif
        tmask |= t;
#endif

// As indicated by the table earlier, this case can be reached in a single
// case: 2 bytes per unit, 4 bytes in composite, 32 bits in mask
// This means that NON_ZERO_MASK_DATA_BITS is always 16 below.
#if NON_ZERO_MASK_COMPOSITE_COUNT > 8
        t = destinationComposite.mValue8 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue8 + (sourceComposite.mValue8 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue8 = 0;
        destinationComposite.mValue8 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 9);
        tmask |= t;

        t = destinationComposite.mValue9 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue9 + (sourceComposite.mValue9 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue9 = 0;
        destinationComposite.mValue9 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 10);
        tmask |= t;

        t = destinationComposite.mValue10 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue10 + (sourceComposite.mValue10 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue10 = 0;
        destinationComposite.mValue10 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 11);
        tmask |= t;

        t = destinationComposite.mValue11 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue11 + (sourceComposite.mValue11 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue11 = 0;
        destinationComposite.mValue11 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 12);
        tmask |= t;

        t = destinationComposite.mValue12 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue12 + (sourceComposite.mValue12 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue12 = 0;
        destinationComposite.mValue12 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 13);
        tmask |= t;

        t = destinationComposite.mValue13 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue13 + (sourceComposite.mValue13 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue13 = 0;
        destinationComposite.mValue13 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 14);
        tmask |= t;

        t = destinationComposite.mValue14 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue14 + (sourceComposite.mValue14 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue14 = 0;
        destinationComposite.mValue14 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 15);
        tmask |= t;

        t = destinationComposite.mValue15 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue15 + (sourceComposite.mValue15 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue15 = 0;
        destinationComposite.mValue15 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 16
        t >>= (NON_ZERO_MASK_DATA_BITS - 16);
#endif
        tmask |= t;
#endif

        NON_ZERO_MASK_COMPOSE_MASK(tmask);

        aDestination.mMask ^= (SUBPIXEL_DATA)tmask;
        aSource.mMask = 0;
    }
};

#endif // !NON_ZERO_MASK_C_H
