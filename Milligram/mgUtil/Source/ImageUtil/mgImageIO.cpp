/*
  Copyright (C) 1995-2012 by Michael J. Goodfellow

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

#include "mgDiffuse.h"
#include "mgGIF3.h"

#include "ImageUtil/mgImageIO.h"

//--------------------------------------------------------
// if we get any kind of error during image decode 
METHODDEF(void) 
ImageErrorExit (j_common_ptr cinfo)
{
  throw new mgErrorMsg("imgJpgLib", "", "");
}

//--------------------------------------------------------
// supress output to stderr from jpeg library
METHODDEF(void) 
ImageOutputMessage (j_common_ptr cinfo)
{
}

//--------------------------------------------------------
// constructor and destructor
mgJPGRead::mgJPGRead(
  mgImageWrite *writer)
  : mgImageRead(writer)
{
  m_RGBAline = NULL;
  m_inFile = NULL;

  m_CompInfo.err = jpeg_std_error(&m_CompErr);  // standard error handler
  m_CompErr.error_exit = ImageErrorExit;
  m_CompErr.output_message = ImageOutputMessage;

  // init decompression and set source to file 
  jpeg_create_decompress(&m_CompInfo);
}

//--------------------------------------------------------
// destructor
mgJPGRead::~mgJPGRead()
{
  delete m_RGBAline;
  m_RGBAline = NULL;

  jpeg_finish_decompress(&m_CompInfo);
  jpeg_destroy_decompress(&m_CompInfo);
  if (m_inFile != NULL)
    fclose(m_inFile);
}

//--------------------------------------------------------
// open a JPG file to be read
BOOL mgJPGRead::open(
  const char* fileName)   // name of file to open
{
  // if file name exists 
  m_inFile = mgOSFileOpen(fileName, "rb");
  if (m_inFile == NULL)
    throw new mgErrorMsg("imgBadOpen", "", "");

  jpeg_stdio_src(&m_CompInfo, m_inFile);

  // read the dimensions of the source image 
  jpeg_read_header(&m_CompInfo, true);
  
  // if there's been an error, return 
  if (// m_CompInfo.err->num_warnings > 0 || 
      m_CompInfo.image_width == 0 || m_CompInfo.image_height == 0)
    return false;
  
  m_RGBAline = new BYTE[m_CompInfo.image_width * 4];
  m_width = m_CompInfo.image_width;
  m_height = m_CompInfo.image_height;

  if (!m_writer->outputStart(m_CompInfo.image_width, m_CompInfo.image_height))
  {
    return false;
  }
  
  // set color map for output
  if (m_CompInfo.out_color_space != JCS_GRAYSCALE)
    m_CompInfo.out_color_space = JCS_RGB;
  m_CompInfo.quantize_colors = false;
  
  // start decompression
  jpeg_calc_output_dimensions(&m_CompInfo);
  jpeg_start_decompress(&m_CompInfo);

  return true;
}

//--------------------------------------------------------
// do a step of work on JPG decode
BOOL mgJPGRead::decodeStep(
  int &y,                    // current scanlines complete
  int &height)               // amount to go
{
  // do a strip of the image 
  int nCount = 128;  // size of a strip 
  while (nCount-- > 0 && m_CompInfo.output_scanline < m_CompInfo.output_height) 
  {
    jpeg_read_scanlines(&m_CompInfo, &m_RGBAline, 1);
    if (m_CompInfo.out_color_space == JCS_GRAYSCALE)
    {
      // repeat grey to create RGB
      BYTE g;
      BYTE* output = m_RGBAline+4*m_CompInfo.output_width;
      for (int i = m_CompInfo.output_width-1; i >= 0; i--)
      {
        *--output = 255;  // alpha
        *--output = g = m_RGBAline[i];  // b
        *--output = g;                  // g
        *--output = g;                  // r
      }
    }
    else
    {
      // insert alpha values into RGB buffer
      BYTE* output = m_RGBAline+4*m_CompInfo.output_width;
      for (int i = m_CompInfo.output_width-1; i >= 0; i--)
      {
        *--output = 255;  // alpha
        *--output = m_RGBAline[i*3+2];  // b
        *--output = m_RGBAline[i*3+1];  // g
        *--output = m_RGBAline[i*3];    // r
      }
    }
    if (!m_writer->outputLine(m_CompInfo.output_scanline-1, m_RGBAline, m_CompInfo.output_width))
      return false;
  }
  y = m_CompInfo.output_scanline;
  height = m_CompInfo.output_height;
  if (y < height)
    return true;

  m_writer->outputEnd();

  return false;
}

//--------------------------------------------------------
// constructor
mgGIFRead::mgGIFRead(
  mgImageWrite *writer)
  : mgImageRead(writer)
{
  m_inFile = NULL;
  m_srcMgr = NULL;
  m_RGBAline = NULL;

  m_CompInfo.err = jpeg_std_error(&m_CompErr);  // standard error handler
  m_CompErr.error_exit = ImageErrorExit;
  m_CompErr.output_message = ImageOutputMessage;

  // start compression to initialize memory management 
  jpeg_create_compress(&m_CompInfo);
}

//--------------------------------------------------------
// destructor
mgGIFRead::~mgGIFRead()
{
  if (m_srcMgr != NULL)
    (*m_srcMgr->finish_input) (&m_CompInfo, m_srcMgr);

  jpeg_destroy_compress(&m_CompInfo);

  if (m_inFile != NULL)
    fclose(m_inFile);

  delete m_RGBAline;
  m_RGBAline = NULL;
}

//--------------------------------------------------------
// open a GIF file to be read
BOOL mgGIFRead::open(
  const char* fileName)   // name of file to open
{
  // if file name exists 
  m_inFile = mgOSFileOpen(fileName, "rb");
  if (m_inFile == NULL)
    throw new mgErrorMsg("imgBadOpen", "", "");

  // Figure out the input file format, and set up to read it. 
  m_srcMgr = jinit_read_gif(&m_CompInfo);
  m_srcMgr->input_file = m_inFile;

  // Read the input file header to obtain file size & colorspace. 
  (*m_srcMgr->start_input) (&m_CompInfo, m_srcMgr);
  (*m_CompInfo.mem->realize_virt_arrays) ((j_common_ptr) &m_CompInfo);

  m_width = m_CompInfo.image_width;
  m_height = m_CompInfo.image_height;

  m_RGBAline = new BYTE[m_CompInfo.image_width * 4];

  if (!m_writer->outputStart(m_CompInfo.image_width, m_CompInfo.image_height))
    return false;

  return true;
}

//--------------------------------------------------------
// do a step of work on GIF decode
BOOL mgGIFRead::decodeStep(
  int &y,                    // current scanlines complete
  int &height)               // amount to go
{
  // do a strip of the image 
  int nCount = 128;  // size of a strip 
  while (nCount-- > 0 && m_CompInfo.next_scanline < m_CompInfo.image_height) 
  {
    int nNumScanlines = (*m_srcMgr->get_pixel_rows) (&m_CompInfo, m_srcMgr);
    for (int i = 0; i < nNumScanlines; i++)
    {
      // insert alpha values into RGB buffer
      BYTE* input = (BYTE*) m_srcMgr->buffer[i];
      BYTE* output = m_RGBAline;
      for (int j = 0; j < (int) m_CompInfo.image_width; j++)
      {
        *output++ = *input++;  // r
        *output++ = *input++;  // g 
        *output++ = *input++;  // b
        *output++ = 255;  // alpha
      }

      if (!m_writer->outputLine(m_CompInfo.next_scanline, m_RGBAline, m_CompInfo.image_width))
        return false;
  
      m_CompInfo.next_scanline++;
    }
  }
  y = m_CompInfo.next_scanline;
  height = m_CompInfo.image_height;
  if (y < height)
    return true;

  m_writer->outputEnd();

  return false;
}

//--------------------------------------------------------
// constructor 
mgBMPRead::mgBMPRead(
  mgImageWrite *writer)
  : mgImageRead(writer)
{
  m_inFile = NULL;
  m_RGBAline = NULL;
  m_reverse = false;
  m_data = NULL;
}

//--------------------------------------------------------
// destructor
mgBMPRead::~mgBMPRead()
{
  delete m_RGBAline;
  m_RGBAline = NULL;

  delete m_data;
  m_data = NULL;

  if (m_inFile != NULL)
    fclose(m_inFile);
}

// Windows bmp file header
typedef struct _BitmapFileHeader
{
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BitmapFileHeader;

// windows bmp image header
typedef struct _BitmapInfoHeader
{
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BitmapInfoHeader;

#define BITMAP_COMPRESSION_RGB 0L

//--------------------------------------------------------
// read WORD data out of file
WORD mgBMPRead::readWord(
  FILE* inFile)
{
  BYTE a = (BYTE) fgetc(inFile);
  BYTE b = (BYTE) fgetc(inFile);

  return b << 8 | a;
}

//--------------------------------------------------------
// read DWORD data out of file
DWORD mgBMPRead::readDWord(
  FILE* inFile)
{
  BYTE a = (BYTE) fgetc(inFile);
  BYTE b = (BYTE) fgetc(inFile);
  BYTE c = (BYTE) fgetc(inFile);
  BYTE d = (BYTE) fgetc(inFile);

  return (d << 24) | (c << 16) | (b << 8) | a;
}

//--------------------------------------------------------
// open a BMP file to be read
BOOL mgBMPRead::open(
  const char* fileName)   // name of file to open
{
  // if file name exists 
  m_inFile = mgOSFileOpen(fileName, "rb");
  if (m_inFile == NULL)
    throw new mgErrorMsg("imgBadOpen", "", "");

  BitmapFileHeader fileHeader;
  fileHeader.bfType = readWord(m_inFile);
  fileHeader.bfSize = readDWord(m_inFile);
  fileHeader.bfReserved1 = readWord(m_inFile);
  fileHeader.bfReserved2 = readWord(m_inFile);
  fileHeader.bfOffBits = readDWord(m_inFile);
  if (fileHeader.bfType != 0x4D42)
    throw new mgErrorMsg("imgNotBmp", "filename", "%s", (const char*) fileName);

  BitmapInfoHeader infoHeader;
  infoHeader.biSize = readDWord(m_inFile);
  infoHeader.biWidth = (int) readDWord(m_inFile);
  infoHeader.biHeight = (int) readDWord(m_inFile);
  infoHeader.biPlanes = readWord(m_inFile);
  infoHeader.biBitCount = readWord(m_inFile);
  infoHeader.biCompression = readDWord(m_inFile);
  infoHeader.biSizeImage = readDWord(m_inFile);

  // handle uncompressed 24 and 32 bit images
  if (infoHeader.biCompression != BITMAP_COMPRESSION_RGB ||
      (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32))
    throw new mgErrorMsg("imgBMPDepth", "filename", "%s", (const char*) fileName);

  m_width = infoHeader.biWidth;
  m_height = abs(infoHeader.biHeight);
  m_bitCount = infoHeader.biBitCount;
  m_reverse = infoHeader.biHeight > 0;

  // read the image data
  DWORD imageSize = m_height * m_width * m_bitCount/8;
  if (infoHeader.biSizeImage != 0 && infoHeader.biSizeImage != imageSize)
    throw new mgErrorMsg("imgBmpSize", "filename,calcSize,declSize", "%s,%d,%d", 
      (const char*) fileName, imageSize, infoHeader.biSizeImage);

  m_data = new BYTE[imageSize];
  fseek(m_inFile, fileHeader.bfOffBits, SEEK_SET);
  size_t bytesRead = fread(m_data, 1, imageSize, m_inFile);
  if (bytesRead != imageSize)
    throw new mgErrorMsg("imgBmpRead", "filename,size", "%s,%d", (const char*) fileName, imageSize);

  fclose(m_inFile);
  m_inFile = NULL;

  m_RGBAline = new BYTE[m_width * 4];
  m_y = 0;
  m_lineLen = m_width * m_bitCount/8;
  m_lineLen = 4*((m_lineLen+3)/4);

  if (!m_writer->outputStart(m_width, m_height))
    return false;

  return true;
}

//--------------------------------------------------------
// do a step of work on BMP decode
BOOL mgBMPRead::decodeStep(
  int &y,                    // current scanlines complete
  int &height)               // amount to go
{
  BYTE* input;
  if (m_reverse)
    input = m_data + ((m_height-1)-m_y)*m_lineLen;
  else input = m_data + m_y * m_lineLen;

  // do a line of image

  // insert alpha values into RGB buffer
  BYTE* output = m_RGBAline;
  if (m_bitCount == 32)
  {
    // copy BGRA from bitmap to RGBA output line
    for (int j = 0; j < (int) m_width; j++)
    {
      *output++ = input[2];  // r
      *output++ = input[1];  // g 
      *output++ = input[0];  // b
      *output++ = input[3];  // alpha
      input += 4;
    }
  }
  else if (m_bitCount == 24)
  {
    // insert alpha into output line
    for (int j = 0; j < (int) m_width; j++)
    {
      *output++ = input[2];  // r
      *output++ = input[1];  // g 
      *output++ = input[0];  // b
      *output++ = 255;  // alpha
      input += 3;
    }
  }

  if (!m_writer->outputLine(m_y, m_RGBAline, m_width))
    return false;

  y = ++m_y;
  height = m_height;
  if (y < height)
    return true;

  m_writer->outputEnd();
  return false;
}

//--------------------------------------------------------
// constructor
mgGIFWrite::mgGIFWrite()
{
  m_outFile = NULL;
  m_gifWrite = NULL;
  m_diffuse = NULL;
  m_target = NULL;
  m_RGBline = NULL;
}

//--------------------------------------------------------
// destructor
mgGIFWrite::~mgGIFWrite()
{
  if (m_outFile != NULL)
    fclose(m_outFile);

  delete m_gifWrite;
  m_gifWrite = NULL;

  delete m_diffuse;
  m_diffuse = NULL;

  delete m_target;
  m_target = NULL;

  delete m_RGBline;
  m_RGBline = NULL;
}

//--------------------------------------------------------
// initialize target image
BOOL mgGIFWrite::outputStart(
  int width,                 // width of image
  int height)                // height of image
{
  m_outFile = mgOSFileOpen(m_fileName, "wb");
  if (m_outFile == NULL)
    throw new mgErrorMsg("imgBadOpen", "", "");

  // initialize error diffusion
  m_target = new BYTE[width];

  m_diffuse = new mgDiffuse(0, width);
  m_gifWrite = new mgGIFFileWrite;
  m_gifWrite->m_outFile = m_outFile;

  m_gifWrite->writeHeader(width, height);

  m_RGBline = new BYTE[width*3];

  // build standard color table
  DWORD alColors[6*6*6];
  int i = 0;
  for (int r = 0; r < 6; r++)
    for (int g = 0; g < 6; g++)
      for (int b = 0; b < 6; b++)
        alColors[i++] = ((r*51) << 16) | ((g*51) << 8) | (b*51);
  m_gifWrite->writeColorTable(alColors, 6*6*6);

  m_gifWrite->writeImageHeader(width, height);

  return true;
}

//--------------------------------------------------------
// for each scanline
BOOL mgGIFWrite::outputLine(
  int y,                     // scanline index
  BYTE* line,                // R, G, B, A values
  int len)                   // length of line
{
  // convert input RGBA to RGB 
  BYTE* input = line;
  BYTE* output = m_RGBline;
  for (int i = 0; i < len; i++)
  {
    *output++ = *input++;  // r
    *output++ = *input++;  // g
    *output++ = *input++;  // b
    input++;  // skip alpha
  }

  m_diffuse->ConvertLine(m_RGBline, m_target);
  m_gifWrite->writeLine(m_target, len);

  return true;
}

//--------------------------------------------------------
// end of image
BOOL mgGIFWrite::outputEnd()
{
  m_gifWrite->writeTerm();

  if (m_outFile != NULL)
    fclose(m_outFile);
  m_outFile = NULL;

  return true;
}

//--------------------------------------------------------
// constructor 
mgJPGWrite::mgJPGWrite()
{
  m_outFile = NULL;
  m_RGBline = NULL;
}

//--------------------------------------------------------
// destructor
mgJPGWrite::~mgJPGWrite()
{
  if (m_outFile != NULL)
    fclose(m_outFile);

  delete m_RGBline;
  m_RGBline = NULL;
}

//--------------------------------------------------------
// initialize target image
BOOL mgJPGWrite::outputStart(
  int width,                 // width of image
  int height)                // height of image
{
  m_outFile = mgOSFileOpen(m_fileName, "wb");
  if (m_outFile == NULL)
    throw new mgErrorMsg("imgBadOpen", "", "");

  m_CompInfo.err = jpeg_std_error(&m_CompErr);  // standard error handler
  m_CompErr.error_exit = ImageErrorExit;
  m_CompErr.output_message = ImageOutputMessage;

  // allocate and initialize JPEG compression object 
  jpeg_create_compress(&m_CompInfo);
  jpeg_stdio_dest(&m_CompInfo, m_outFile);

  m_CompInfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&m_CompInfo);
  m_CompInfo.in_color_space = JCS_RGB;
  m_CompInfo.input_components = 3;
  m_CompInfo.data_precision = 8;
  m_CompInfo.image_width = width;
  m_CompInfo.image_height = height;
  jpeg_default_colorspace(&m_CompInfo);

  // Start compressor, write complete file 
  jpeg_start_compress(&m_CompInfo, true);

  return true;
}

//--------------------------------------------------------
// for each scanline
BOOL mgJPGWrite::outputLine(
  int y,                     // scanline index
  BYTE* line,                // R, G, B, A values
  int len)                   // length of line
{
  // convert input RGBA to RGB 
  BYTE* input = line;
  BYTE* output = m_RGBline;
  for (int i = 0; i < len; i++)
  {
    *output++ = *input++;  // r
    *output++ = *input++;  // g
    *output++ = *input++;  // b
    input++;  // skip alpha
  }

  jpeg_write_scanlines(&m_CompInfo, &m_RGBline, 1);

  return true;
}

//--------------------------------------------------------
// end of image
BOOL mgJPGWrite::outputEnd()
{
  jpeg_finish_compress(&m_CompInfo);
  jpeg_destroy_compress(&m_CompInfo);

  fclose(m_outFile);
  m_outFile = NULL;

  return true;
}

///--------------------------------------------------------
// constructor and destructor
mgRGBWrite::mgRGBWrite()
{
}

mgRGBWrite::~mgRGBWrite()
{
}

//--------------------------------------------------------
// initialize target image
BOOL mgRGBWrite::outputStart(
  int width,                 // width of image
  int height)                // height of image
{
  m_width = width;
  m_height = height;
  m_data = new BYTE[m_width * m_height * 3];

  return true;
}

//--------------------------------------------------------
// for each scanline
BOOL mgRGBWrite::outputLine(
  int y,                     // scanline index
  BYTE* line,                // R, G, B, A values
  int len)                   // length of line
{
  // copy RGBA input to RGB output
  len = len*3;  // len is bytes, not pixels

  BYTE* input = line;
  BYTE* output = m_data+m_width*3*y;
  for (int i = 0; i < len; i++)
  {
    *output++ = *input++;  // r
    *output++ = *input++;  // g
    *output++ = *input++;  // b
    input++;  // skip alpha
  }

  return true;
}

//--------------------------------------------------------
// end of image
BOOL mgRGBWrite::outputEnd()
{
  return true;
}


///--------------------------------------------------------
// constructor and destructor
mgRGBAWrite::mgRGBAWrite()
{
  m_data = NULL;
}

mgRGBAWrite::~mgRGBAWrite()
{
}

//--------------------------------------------------------
// initialize target image
BOOL mgRGBAWrite::outputStart(
  int width,                 // width of image
  int height)                // height of image
{
  m_width = width;
  m_height = height;
  m_data = new BYTE[m_width * m_height * 4];

  return true;
}

//--------------------------------------------------------
// for each scanline
BOOL mgRGBAWrite::outputLine(
  int y,                     // scanline index
  BYTE* line,                // R, G, B, A values
  int len)                   // length of line
{
  // copy RGBA input to RGBA output
  BYTE* output = m_data+m_width*4*y;
  memcpy(output, line, len*4);

  return true;
}

//--------------------------------------------------------
// end of image
BOOL mgRGBAWrite::outputEnd()
{
  return true;
}

//--------------------------------------------------------
// constructor
mgAlphaWrite::mgAlphaWrite(
  int width,
  int height,
  BYTE* imageData)
{
  m_width = width;
  m_height = height;
  m_imageData = imageData;
}

//--------------------------------------------------------
// destructor
mgAlphaWrite::~mgAlphaWrite()
{
}

//--------------------------------------------------------
// initialize target image
BOOL mgAlphaWrite::outputStart(
  int width,                 // width of image
  int height)                // height of image
{
  if (m_width != width || m_height != height)
    throw new mgErrorMsg("imgAlphaSize", "alphawd,alphaht,rgbwd,rgbht", "%d,%d,%d,%d", width, height, m_width, m_height);

  return true;
}

//--------------------------------------------------------
// for each scanline
BOOL mgAlphaWrite::outputLine(
  int y,                      // scanline index
  BYTE* line,                // R, G, B, A values
  int len)                    // length of line (pixels)
{
  // invert y coordinate for BITMAP
  // y = (m_height -1) - y;

  // copy R byte of RGBA input to BGRA output
  BYTE* input = line;
  BYTE* output = m_imageData+4*len*y;

  for (int i = 0; i < len; i++)
  {
    output[3] = input[0];
    output += 4;
    input += 4;
  }
  return true;
}

//--------------------------------------------------------
// end of image
BOOL mgAlphaWrite::outputEnd()
{
  return true;
}




