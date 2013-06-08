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
#ifndef MGIMAGEIO_H
#define MGIMAGEIO_H

#include "JpegLib/Include/jpeglib.h"
#include "JpegLib/Include/cdjpeg.h"
#include "LibPNG/Include/png.h"

class mgImageWrite;

// image decompress object
class mgImageRead
{
public:
  int m_errorCode;                  // error code, or zero
  mgImageWrite *m_writer;           // output writer
  int m_width;                      // width of image
  int m_height;                     // height of image

  // open a file to be read
  virtual BOOL open(
    const char* fileName) = 0;      // name of file to open

  // do a step of work 
  virtual BOOL decodeStep(
    int &y,                         // current scanlines complete
    int &height) = 0;               // amount to go

  // constructor
  mgImageRead(
    mgImageWrite *writer)
  { 
    m_errorCode = 0;
    m_writer = writer;
  }

  // destructor
  virtual ~mgImageRead()
  {}    

protected:
  FILE* m_inFile;                   // open input file
};

// JPG read object
class mgJPGRead : public mgImageRead
{
public:
  // open a file to be read
  virtual BOOL open(
    const char* fileName);          // name of file to open

  // do a step of work 
  virtual BOOL decodeStep(
    int &y,                         // current scanlines complete
    int &height);                   // amount to go

  // constructor
  mgJPGRead(
    mgImageWrite *writer);

  // destructor
  virtual ~mgJPGRead();

protected:
  struct jpeg_error_mgr m_CompErr;  // error handler 
  struct jpeg_decompress_struct m_CompInfo; // decompress state 
  BYTE* m_RGBAline;                 // RGBA line
};

// gif read object
class mgGIFRead : public mgImageRead
{
public:
  // open a file to be read
  virtual BOOL open(
    const char* fileName); // name of file to open

  // do a step of work 
  virtual BOOL decodeStep(
    int &y,                    // current scanlines complete
    int &height);              // amount to go

  // constructor
  mgGIFRead(
    mgImageWrite *writer);

  // destructor
  virtual ~mgGIFRead();

protected:
  cjpeg_source_ptr m_srcMgr;        // gif reader state
  struct jpeg_error_mgr m_CompErr;  // error handler 
  struct jpeg_compress_struct m_CompInfo; // compress state 
  BYTE* m_RGBAline;                 // RGBA line
};

// BMP read object
class mgBMPRead : public mgImageRead
{
public:
  // open a file to be read
  virtual BOOL open(
    const char* fileName); // name of file to open

  // do a step of work 
  virtual BOOL decodeStep(
    int &y,                    // current scanlines complete
    int &height);              // amount to go

  // constructor
  mgBMPRead(
    mgImageWrite *writer);

  // destructor
  virtual ~mgBMPRead();

protected:
  BYTE* m_RGBAline;                 // RGBA line
  int m_bitCount;                   // bits per pixel
  int m_lineLen;                    // line length with padding
  BOOL m_reverse;                   // scanlines in reverse order
  BYTE* m_data;                     // image data 
  int m_y;                          // next scanline to read

  // read WORD data out of file
  WORD readWord(
    FILE* inFile);

  // read DWORD data out of file
  DWORD readDWord(
    FILE* inFile);
};

// PNG read object
class mgPNGRead : public mgImageRead
{
public:
  // open a file to be read
  virtual BOOL open(
    const char* fileName);          // name of file to open

  // do a step of work 
  virtual BOOL decodeStep(
    int &y,                         // current scanlines complete
    int &height);                   // amount to go

  // constructor
  mgPNGRead(
    mgImageWrite *writer);

  // destructor
  virtual ~mgPNGRead();

protected:
  png_structp m_state;
  png_infop m_info; // decompress state 
  BYTE* m_RGBAline;                 // RGBA line
  int m_y;
};

// write decoded image
class mgImageWrite
{
public:
  int m_errorCode;                  // error code, or 0

  mgImageWrite()
  {
    m_errorCode = 0;
  }

  virtual ~mgImageWrite()
  {
  }

  // start image output
  virtual BOOL outputStart(
    int width,                      // width of image
    int height) = 0;                // height of image

  // for each scanline
  virtual BOOL outputLine(
    int y,                          // scanline index
    BYTE* RGBAline,                 // R, G, B, A values
    int len) = 0;                   // length of line

  // end of image
  virtual BOOL outputEnd() = 0;
};

class mgDiffuse;
class mgGIFFileWrite;

// write as GIF, Chat color table
class mgGIFWrite : public mgImageWrite
{
public:
  mgString m_fileName;              // name of file to write

  // constructor and destructor
  mgGIFWrite();
  virtual ~mgGIFWrite();

  // start image output
  virtual BOOL outputStart(
    int width,                      // width of image
    int height);                    // height of image

  // for each scanline
  virtual BOOL outputLine(
    int y,                          // scanline index
    BYTE* RGBAline,                 // R, G, B, A values
    int len);                       // length of line

  // end of image
  virtual BOOL outputEnd();

protected:
  FILE* m_outFile;                  // open output file
  mgDiffuse *m_diffuse;             // error diffusion state
  mgGIFFileWrite *m_gifWrite;       // GIF file writer
  BYTE* m_RGBline;                  // RGB line
  BYTE* m_target;                   // diffusion target 1 byte
};

// write as JPG
class mgJPGWrite : public mgImageWrite
{
public:
  mgString m_fileName;              // name of file to write

  // constructor and destructor
  mgJPGWrite();
  virtual ~mgJPGWrite();

  // start image output
  virtual BOOL outputStart(
    int width,                      // width of image
    int height);                    // height of image

  // for each scanline
  virtual BOOL outputLine(
    int y,                          // scanline index
    BYTE* RGBAline,                 // R, G, B, A values
    int len);                       // length of line

  // end of image
  virtual BOOL outputEnd();

protected:
  FILE* m_outFile;                  // open output file
  struct jpeg_error_mgr m_CompErr;  // error handler 
  struct jpeg_compress_struct m_CompInfo;
  BYTE* m_RGBline;                  // RGB line
};


// write as in-memory array
class mgRGBWrite : public mgImageWrite
{
public:
  int m_width;
  int m_height;
  BYTE* m_data;

  // constructor and destructor
  mgRGBWrite();
  virtual ~mgRGBWrite();

  // start image output
  virtual BOOL outputStart(
    int width,                      // width of image
    int height);                    // height of image

  // for each scanline
  virtual BOOL outputLine(
    int y,                          // scanline index
    BYTE* RGBAline,                 // R, G, B, A values
    int len);                       // length of line

  // end of image
  virtual BOOL outputEnd();
};

// write as in-memory array
class mgRGBAWrite : public mgImageWrite
{
public:
  int m_width;
  int m_height;
  BYTE* m_data;

  // constructor and destructor
  mgRGBAWrite();
  virtual ~mgRGBAWrite();

  // start image output
  virtual BOOL outputStart(
    int width,                      // width of image
    int height);                    // height of image

  // for each scanline
  virtual BOOL outputLine(
    int y,                          // scanline index
    BYTE* RGBAline,                 // R, G, B, A values
    int len);                       // length of line

  // end of image
  virtual BOOL outputEnd();
};

// write the alpha channel of 32-bit memory from red component
class mgAlphaWrite : public mgImageWrite
{
public:
  // constructor and destructor
  mgAlphaWrite(
    int width,
    int height,
    BYTE* imageData);
  virtual ~mgAlphaWrite();

  // start image output
  virtual BOOL outputStart(
    int width,                      // width of image
    int height);                    // height of image

  // for each scanline
  virtual BOOL outputLine(
    int y,                          // scanline index
    BYTE* RGBAline,                 // R, G, B, A values
    int len);                       // length of line

  // end of image
  virtual BOOL outputEnd();
  
protected:  
  int m_width;
  int m_height;
  BYTE* m_imageData;
};

#endif

