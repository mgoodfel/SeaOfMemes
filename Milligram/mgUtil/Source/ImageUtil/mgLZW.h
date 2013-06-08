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
#ifndef MGLZW_H
#define MGLZW_H

// this class derived from source in Jpeg Interest group library routine wrgif.c

typedef short mgLZWCode;    // holds max code length, 12 bits
typedef long mgLZWHash;     // must hold (mgLZWCode<<8) | byte 

// an LZW encoder
class mgLZWEncode
{
public: 
  // constructor and destructor
  mgLZWEncode();
  virtual ~mgLZWEncode();
 
  // compress a byte
  void compressByte(
    BYTE nByte);

  // write initial data
  void compressInit();

  // write final data
  void compressTerm();

  // write a byte of data
  virtual void writeLZWByte(
    BYTE nByte) = 0;

protected:
  // State for packing variable-width codes into a bitstream 
  int m_bits;			              // current number of bits/code
  mgLZWCode m_maxCode;		          // maximum code, given nBits
  int m_initBits;                // initial nBits ... restored after clear
  long m_curAccum;		            // holds bits not yet output 
  int m_curBits;			            // # of bits in cur_accum 

  // LZW string construction 
  mgLZWCode m_waitingCode;         // symbol not yet output; may be extendable 
  BOOL m_firstByte;              // if TRUE, waiting_code is not valid 

  // State for LZW code assignment
  mgLZWCode m_clearCode;           // clear code (doesn't change)
  mgLZWCode m_EOFCode;             // EOF code (ditto) 
  mgLZWCode m_freeCode;            // first not-yet-used symbol code 

  // LZW hash table
  mgLZWCode *m_hashCode;           // => hash table of symbol codes 
  mgLZWHash *m_hashValue;          // => hash table of symbol values

  // clear the hash table
  inline void clearHash();

  // reset the compressor and issue clear code
  void clearBlock();

  // write an LZW code 
  void writeCode(
    mgLZWCode nCode);
};

#endif
