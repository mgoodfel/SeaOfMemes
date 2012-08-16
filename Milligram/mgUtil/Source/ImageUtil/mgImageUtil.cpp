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

#include "ImageUtil/mgImageIO.h"
#include "ImageUtil/mgImageUtil.h"

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
// load an image file into memory
void mgLoadImage(
  const char* name,
  int &imageWidth,
  int &imageHeight,
  BYTE* &imageData)
{
  mgString fileName(name);

  // call load routine by file type
  int posn = fileName.reverseFind(fileName.length(), ".");
  if (posn == -1)
    throw new mgErrorMsg("imgNoType", "filename", "%s", (const char*) fileName);

  mgString fileType;
  fileName.substring(fileType, posn);

  mgRGBWrite writer;
  mgImageRead *pReader;
  if (fileType.equalsIgnoreCase(".jpg"))
    pReader = new mgJPGRead(&writer);

  else if (fileType.equalsIgnoreCase(".gif"))
    pReader = new mgGIFRead(&writer);

  else if (fileType.equalsIgnoreCase(".bmp"))
    pReader = new mgBMPRead(&writer);

  else throw new mgErrorMsg("imgBadType", "filename,type", "%s,%s", (const char*) fileName, (const char*) fileType);

  // try to open the image for read
  if (!pReader->open(fileName))
  {
    delete pReader;
    throw new mgErrorMsg("imgRead", "filename", "%s", (const char*) fileName);
  }

  // read until complete
  int y, height;
  while (pReader->decodeStep(y, height));

  int nError = pReader->m_errorCode;

  if (nError != 0)
  {
    delete pReader;
    throw new mgErrorMsg("imgRead", "filename", "%s", (const char*) fileName);
  }

  if (writer.m_errorCode != 0)
  {
    delete pReader;
    throw new mgErrorMsg("imgWrite", "filename", "%s", (const char*) fileName);
  }

  if (writer.m_width == 0 || writer.m_height == 0)
  {
    delete pReader;
    throw new mgErrorMsg("imgBadSize", "filename,wd,ht", "%s,%d,%d", (const char*) fileName, writer.m_width, writer.m_height);
  }

  imageWidth = writer.m_width;
  imageHeight = writer.m_height;
  imageData = writer.m_data;

  delete pReader;
}

//--------------------------------------------------------------
// load image into RGBA format
void mgLoadRGBAImage(
  const char* fileName,
  int& width,
  int& height,
  BOOL& hasAlpha,
  BYTE*& data)
{
  width = 0;
  height = 0;
  data = NULL;

  mgString filenameRGB(fileName);
  mgString filenameAlpha;

  // split RGB and alpha images on semicolon
  int semi = filenameRGB.find(0, ";");
  if (semi != -1)
  {
    // find rgb and image file names
    filenameRGB.substring(filenameAlpha, semi+1);
    filenameRGB.deleteAt(semi, filenameRGB.length() - semi);

    // read the two images
    mgRGBAWrite* writer = new mgRGBAWrite();
    mgImageWrite* alphaWriter = NULL;
    try
    {
      mgReadImage(writer, filenameRGB);

      // exception on 0 by 0 image
      if (writer->m_width == 0 || writer->m_height == 0)
        throw new mgErrorMsg("imgBadSize", "wd,ht", "%d,%d", writer->m_width, writer->m_height);

      alphaWriter = new mgAlphaWrite(writer->m_width, writer->m_height, writer->m_data);
      mgReadImage(alphaWriter, filenameAlpha);
    }
    catch (mgErrorMsg* e)
    {
      delete writer;
      delete alphaWriter;
      e->addVars("filename", "%s", (const char*) fileName);
      throw e;
    }

    width = writer->m_width;
    height = writer->m_height;
    data = writer->m_data;
    delete alphaWriter;
    delete writer;
    hasAlpha = true;
  }
  else
  {
    // read rgb file
    mgRGBAWrite* writer = new mgRGBAWrite();

    try
    {
      mgReadImage(writer, filenameRGB);

      // exception on 0 by 0 image
      if (writer->m_width == 0 || writer->m_height == 0)
        throw new mgErrorMsg("imgBadSize", "wd,ht", "%d,%d", writer->m_width, writer->m_height);
    }
    catch (mgErrorMsg* e)
    {
      delete writer;
      e->addVars("filename", "%s", (const char*) fileName);
      throw e;
    }
    width = writer->m_width;
    height = writer->m_height;
    data = writer->m_data;
    delete writer;
    hasAlpha = false;
  }
}

//--------------------------------------------------------------
// read the image file
void mgReadImage(
  mgImageWrite* writer,
  const char* name)
{
  mgString fileName(name);

  // call load routine by file extension
  int posn = fileName.reverseFind(fileName.length(), ".");
  if (posn == -1)
    throw new mgErrorMsg("imgNoType", "filename", "%s", (const char*) fileName);
  
  mgString fileType;
  fileName.substring(fileType, posn);

  mgImageRead *reader = NULL;
  if (fileType.equalsIgnoreCase(".jpg"))
    reader = new mgJPGRead(writer);

  else if (fileType.equalsIgnoreCase(".gif"))
    reader = new mgGIFRead(writer);

  else if (fileType.equalsIgnoreCase(".bmp"))
    reader = new mgBMPRead(writer);

  else throw new mgErrorMsg("imgBadType", "filename,type", "%s,%s", (const char*) fileName, (const char*) fileType);

  // try to open the image for read
  try
  {
    reader->open(fileName);
  }
  catch (mgErrorMsg* e)
  {
    e->addVars("filename", "%s", (const char*) fileName);
    throw e;  
  }

  // read until complete
  int y, height;
  while (reader->decodeStep(y, height));

  int nError = reader->m_errorCode;
  delete reader;

  // exception on errors
  if (nError != 0)
    throw new mgErrorMsg("imgRead", "filename,code", "%s,%d", (const char*) fileName, nError);

  if (writer->m_errorCode != 0)
    throw new mgErrorMsg("imgWrite", "filename,code", "%s,%d", (const char*) fileName, writer->m_errorCode);
}

//--------------------------------------------------------------
// save BGRA image data to a jpg file
void mgWriteBGRAtoJPGFile(
  const char* fileName,
  int width,
  int height,
  const BYTE* pData)
{
  FILE* pOutFile = mgOSFileOpen(fileName, "wb");
  if (pOutFile == NULL)
    throw new mgErrorMsg("imgBadOpen", "filename", "%s", (const char*) fileName);

  struct jpeg_error_mgr compErr;    // error handler 
  struct jpeg_compress_struct compInfo;
  compInfo.err = jpeg_std_error(&compErr);  // standard error handler
  compErr.error_exit = ImageErrorExit;
  compErr.output_message = ImageOutputMessage;

  // allocate and initialize JPEG compression object 
  jpeg_create_compress(&compInfo);
  jpeg_stdio_dest(&compInfo, pOutFile);

  compInfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&compInfo);
  jpeg_set_quality(&compInfo, 99, true);

  compInfo.in_color_space = JCS_RGB;
  compInfo.input_components = 3;
  compInfo.data_precision = 8;
  compInfo.image_width = width;
  compInfo.image_height = height;
  jpeg_default_colorspace(&compInfo);

  // Start compressor, write complete file 
  jpeg_start_compress(&compInfo, TRUE);

  // read scanlines from the screen, top to bottom
  BYTE* line = new BYTE[width*3];
  const BYTE* pSource = pData;
  for (int i = 0; i < height; i++)
  {
    int targetOffset = 0;
    int sourceOffset = 0;
    for (int j = 0; j < width; j++)
    {
      line[targetOffset++] = pSource[sourceOffset+2];
      line[targetOffset++] = pSource[sourceOffset+1];
      line[targetOffset++] = pSource[sourceOffset+0];
      sourceOffset += 4;
    }
    jpeg_write_scanlines(&compInfo, &line, 1);
    pSource += width*4;
  }
  delete line;

  jpeg_finish_compress(&compInfo);
  jpeg_destroy_compress(&compInfo);

  fclose(pOutFile);
}

//--------------------------------------------------------------
// save BGR image data to a jpg file
void mgWriteBGRtoJPGFile(
  const char* fileName,
  int width,
  int height,
  const BYTE* pData)
{
  FILE* pOutFile = mgOSFileOpen(fileName, "wb");
  if (pOutFile == NULL)
    throw new mgErrorMsg("imgBadOpen", "filename", "%s", (const char*) fileName);

  struct jpeg_error_mgr compErr;    // error handler 
  struct jpeg_compress_struct compInfo;
  compInfo.err = jpeg_std_error(&compErr);  // standard error handler
  compErr.error_exit = ImageErrorExit;
  compErr.output_message = ImageOutputMessage;

  // allocate and initialize JPEG compression object 
  jpeg_create_compress(&compInfo);
  jpeg_stdio_dest(&compInfo, pOutFile);

  compInfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&compInfo);
  jpeg_set_quality(&compInfo, 99, true);

  compInfo.in_color_space = JCS_RGB;
  compInfo.input_components = 3;
  compInfo.data_precision = 8;
  compInfo.image_width = width;
  compInfo.image_height = height;
  jpeg_default_colorspace(&compInfo);

  // Start compressor, write complete file 
  jpeg_start_compress(&compInfo, TRUE);

  // read scanlines from the screen, top to bottom
  int sourceSpan = width*3;
  sourceSpan = 4*((sourceSpan+3)/4);

  BYTE* line = new BYTE[width*3];
  const BYTE* pSource = pData;
  for (int i = 0; i < height; i++)
  {
    int targetOffset = 0;
    int sourceOffset = 0;
    for (int j = 0; j < width; j++)
    {
      line[targetOffset++] = pSource[sourceOffset+2];
      line[targetOffset++] = pSource[sourceOffset+1];
      line[targetOffset++] = pSource[sourceOffset+0];
      sourceOffset += 3;
    }
    jpeg_write_scanlines(&compInfo, &line, 1);
    pSource += sourceSpan;
  }
  delete line;

  jpeg_finish_compress(&compInfo);
  jpeg_destroy_compress(&compInfo);

  fclose(pOutFile);
}



