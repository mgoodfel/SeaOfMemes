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

#include "mgLZW.h"

const int MAX_LZW_BITS = 12; // maximum LZW code size (4096 symbols)
const int LZW_TABLE_SIZE = 1 << MAX_LZW_BITS;
const int HASH_SIZE = 5003; // hash table size for 80% occupancy 

typedef int HashInt; // must hold -2*HSIZE..2*HSIZE 

#define MAXCODE(nBits) (((mgLZWCode) 1 << (nBits)) - 1)

#define HASH_ENTRY(prefix,suffix)  ((((mgLZWHash) (prefix)) << 8) | (suffix))

//
// The LZW hash table consists of two parallel arrays:
//   hash_code[i]	code of symbol in slot i, or 0 if empty slot
//   hash_value[i]	symbol's value; undefined if empty slot
// where slot values (i) range from 0 to HSIZE-1.  The symbol value is
// its prefix symbol's code concatenated with its suffix character.
//
// Algorithm:  use open addressing double hashing (no chaining) on the
// prefix code / suffix character combination.  We do a variant of Knuth's
// algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
// secondary probe.
//

//-----------------------------------------------------------------
// constructor
mgLZWEncode::mgLZWEncode()
{
  m_hashCode = new mgLZWCode[HASH_SIZE];
  m_hashValue = new mgLZWHash[HASH_SIZE];

  // init all the state variables
  m_bits = m_initBits = 9;  // data size + 1
  m_maxCode = MAXCODE(m_bits);
  m_clearCode = ((mgLZWCode) 1 << (m_initBits - 1));
  m_EOFCode = m_clearCode + 1;
  m_freeCode = m_clearCode + 2;
  m_firstByte = TRUE;	// no waiting symbol yet

  // init output buffering vars
  m_curAccum = 0;
  m_curBits = 0;

  clearHash();        // clear hash table
}

//-----------------------------------------------------------------
// destructor
mgLZWEncode::~mgLZWEncode()
{
  delete m_hashCode;
  delete m_hashValue;
}

//-----------------------------------------------------------------
// clear hash table
void mgLZWEncode::clearHash()
{
  // it's sufficient to clear hash codes
  memset(m_hashCode, 0, HASH_SIZE * sizeof(mgLZWCode));
}

//-----------------------------------------------------------------
// reset compressor and issue a clear code
void mgLZWEncode::clearBlock()
{
  clearHash();			// delete all the symbols 

  m_freeCode = m_clearCode + 2;
  writeCode(m_clearCode);	// inform decoder 
  m_bits = m_initBits;	// reset code size 
  m_maxCode = MAXCODE(m_bits);
}

//-----------------------------------------------------------------
// compress a byte
void mgLZWEncode::compressByte(
  BYTE nByte)
{
  if (m_firstByte) 
  {	
    // need to initialize nWaitingCode 
    m_waitingCode = nByte;
    m_firstByte = FALSE;
    return;
  }

  // Probe hash table to see if a symbol exists for 
  // waiting_code followed by nByte.

  HashInt i = ((HashInt) nByte << (MAX_LZW_BITS-8)) + m_waitingCode;
  HashInt nDisp;

  // i is less than twice 2**MAX_LZW_BITS, therefore less than twice HSIZE
  if (i >= HASH_SIZE)
    i -= HASH_SIZE;

  mgLZWHash lProbeValue = HASH_ENTRY(m_waitingCode, nByte);
  // is first probed slot not empty?
  if (m_hashCode[i] != 0) 
  { 
    if (m_hashValue[i] == lProbeValue) 
    {
      m_waitingCode = m_hashCode[i];
      return;
    }
    if (i == 0)			// secondary hash (after G. Knott) 
      nDisp = 1;
    else nDisp = HASH_SIZE - i;

    for (;;) 
    {
      i -= nDisp;
      if (i < 0)
        i += HASH_SIZE;
      if (m_hashCode[i] == 0)
        break;			// hit empty slot
      if (m_hashValue[i] == lProbeValue) 
      {
	      m_waitingCode = m_hashCode[i];
        return;
      }
    }
  }

  // here when hashtable[i] is an empty slot; desired symbol not in table 
  writeCode(m_waitingCode);

  if (m_freeCode < LZW_TABLE_SIZE) 
  {
    m_hashCode[i] = m_freeCode++; // add symbol to hashtable
    m_hashValue[i] = lProbeValue;
  } 
  else clearBlock();

  m_waitingCode = nByte;
}

//-----------------------------------------------------------------
// write an LZW code 
void mgLZWEncode::writeCode(
  mgLZWCode nCode)
{
  // Uses nCurAccum and nCurBits to reblock into 8-bit bytes
  m_curAccum |= ((long) nCode) << m_curBits;
  m_curBits += m_bits;

  while (m_curBits >= 8) 
  {
    writeLZWByte(m_curAccum & 0xFF);
    m_curAccum >>= 8;
    m_curBits -= 8;
  }

  // If the next entry is going to be too big for the code size,
  // then increase it, if possible.  We do this here to ensure
  // that it's done in sync with the decoder's codesize increases.

  if (m_freeCode > m_maxCode) 
  {
    m_bits++;
    if (m_bits == MAX_LZW_BITS)
      m_maxCode = LZW_TABLE_SIZE; // free_code will never exceed this
    else m_maxCode = MAXCODE(m_bits);
  }
}


//-----------------------------------------------------------------
// start of data
void mgLZWEncode::compressInit()
{
  // write an initial Clear code
  writeCode(m_clearCode);
}

//-----------------------------------------------------------------
// end of data
void mgLZWEncode::compressTerm()
{
  // Flush out the buffered code 
  if (!m_firstByte)
    writeCode(m_waitingCode);

  // Send an EOF code 
  writeCode(m_EOFCode);

  // Flush the bit-packing buffer 
  if (m_curBits > 0)
    writeLZWByte(m_curAccum & 0xFF);
}

