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
#ifdef SUPPORT_GL21

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "mgPlatform/Include/mgPlatformServices.h"
#include "Graphics3D/mgVertexAttrib.h"
#include "mgGL21Types.h"

#include "mgGL21Misc.h"

//--------------------------------------------------------------
// check for GL errors
void mgGL21checkError()
{
  BOOL done = false;
  mgString msg;
  for (int i = 0; i < 10 && !done; i++)
  {
    GLenum err = glGetError();
    switch (err)
    {
      case GL_NO_ERROR:
        if (msg.isEmpty())
          msg += "GL no error.  ";
        done = true;
        break;

      case GL_INVALID_ENUM:
        msg += "GL invalid enum.  ";
        break;

      case GL_INVALID_VALUE:
        msg += "GL invalid value.  ";
        break;

      case GL_INVALID_OPERATION:
        msg += "GL invalid operation.  ";
        break;

#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        msg += "GL invalid framebuffer operation.  ";
        break;
#endif

      case GL_OUT_OF_MEMORY:
        msg += "GL out of memory.  ";
        break; 
    }
  }
  mgDebug("%s", (const char*) msg);
}

//--------------------------------------------------------------------------
// get shader log error messages as string
void GL21getShaderLog(
  GLuint shader,
  mgString& logStr)
{
  GLint logLen, retLen;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
  char* logText = new char[logLen];
  glGetShaderInfoLog(shader, logLen, &retLen, logText);
  logStr.empty();
  logStr.write(logText, retLen);
  delete logText;
}

//--------------------------------------------------------------------------
// load source file for a shader
BOOL GL21loadShaderFile(
  const char* fileName,
  GLuint shader)
{
  FILE* sourceFile = mgOSFileOpen(fileName, "rb");
  if (sourceFile == NULL)
    return false;

  fseek(sourceFile, 0, SEEK_END);
  long fileSize = ftell(sourceFile);
  fseek(sourceFile, 0, SEEK_SET);

  // read shader source
  char* buffer = new char[fileSize+1];
  size_t readLen = fread(buffer, 1, fileSize, sourceFile);
  buffer[readLen] = '\0';
  fclose(sourceFile);

  const GLchar* strings[1];
  strings[0] = (GLchar *) buffer;
  // =-= skip any leading UTF-8 BOM
  glShaderSource(shader, 1, strings, NULL);

  // delete shader source
  delete buffer;

  return true;
}

//--------------------------------------------------------------------------
// load and compile a shader
// =-= source code modified from a version in the OpenGL SuperBible
BOOL GL21compileShaderPair(
  const char* shaderDir,
  GLuint hVertexShader,
  const char* vertexFileName, 
  GLuint hFragmentShader,
  const char* fragmentFileName)
{
  GLint testVal;

  // Load them. If fail clean up and return null
  mgString fileName;
  fileName.format("%s%s", (const char*) shaderDir, (const char*) vertexFileName);
  mgOSFixFileName(fileName);
  if (!GL21loadShaderFile(fileName, hVertexShader))
  {
    mgDebug("shader %s not found", (const char*) fileName);
    return false;
  }
  
  mgString log;
  // Compile vertex shader
  glCompileShader(hVertexShader);
  GL21getShaderLog(hVertexShader, log);
  log.trim();
  if (log.isEmpty())
    mgDebug("shader %s compiled", (const char*) fileName);
  else mgDebug("shader %s log:\n%s", (const char*) fileName, (const char*) log);

  // Check for errors
  glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
  if (testVal == GL_FALSE)
  {
    mgDebug("shader %s compilation failed.", (const char*) fileName);
    return false;
  }
    
  fileName.format("%s%s", (const char*) shaderDir, (const char*) fragmentFileName);
  mgOSFixFileName(fileName);
  if (!GL21loadShaderFile(fileName, hFragmentShader))
  {
    mgDebug("shader %s not found", (const char*) fileName);
    return false;
  }
    
  glCompileShader(hFragmentShader);
  GL21getShaderLog(hFragmentShader, log);
  log.trim();
  if (log.isEmpty())
    mgDebug("shader %s compiled", (const char*) fileName);
  else mgDebug("shader %s log:\n%s", (const char*) fileName, (const char*) log);

  glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
  if (testVal == GL_FALSE)
  {
    mgDebug("shader %s compilation failed.", (const char*) fileName);
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
// Load the shader file, with the supplied named attributes
GLuint mgGL21loadShaderPair(
  const char* shaderDir,
  const char* vertexFileName, 
  const char* fragmentFileName, 
  int argCount, 
  ...)
{
  mgString NVshader;
  NVshader.format("%sGL21/NVIDIA/", (const char*) shaderDir);

  mgString ATIshader;
  ATIshader.format("%sGL21/ATI/", (const char*) shaderDir);

  // Create shader objects
  GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  
  // find a version which compiles on this display
  if (!GL21compileShaderPair(NVshader, hVertexShader, vertexFileName, hFragmentShader, fragmentFileName))
    if (!GL21compileShaderPair(ATIshader, hVertexShader, vertexFileName, hFragmentShader, fragmentFileName))
      throw new mgErrorMsg("glBadShader", "vsName,fsName", "%s,%s", (const char*) vertexFileName, (const char*) fragmentFileName);

  // create the shader program
  GLuint uiShaderID = glCreateProgram();

  glAttachShader(uiShaderID, hVertexShader);
  glAttachShader(uiShaderID, hFragmentShader);

  // List of attributes                                   
  va_list attributeList;
  va_start(attributeList, argCount);

  char *nextArg;
  for(int i = 0; i < argCount; i++)
  {
    int index = va_arg(attributeList, int);
    nextArg = va_arg(attributeList, char*);
    glBindAttribLocation(uiShaderID, index, nextArg);
  }

  va_end(attributeList);

  glLinkProgram(uiShaderID);
  
  // These are no longer needed
  glDeleteShader(hVertexShader);
  glDeleteShader(hFragmentShader);  
    
  // Make sure link worked too
  GLint testVal;
  glGetProgramiv(uiShaderID, GL_LINK_STATUS, &testVal);
  if (testVal == GL_FALSE)
  {
    // get the linker log
    GLsizei msgSize, msgLen;
    glGetProgramiv(uiShaderID, GL_INFO_LOG_LENGTH, &msgSize);
    GLchar* message = new char[msgSize];
    glGetProgramInfoLog(uiShaderID, msgSize, &msgLen, message);
    mgDebug("%s", (const char*) message);
    delete message;

    glDeleteProgram(uiShaderID);
    uiShaderID = 0;

    throw new mgErrorMsg("glBadShaderLink", "vsName,fsName", "%s,%s", (const char*) vertexFileName, (const char*) fragmentFileName);
  }

  return uiShaderID;
}

//--------------------------------------------------------------------------
// Load the shader file, with the supplied named attributes
GLuint mgGL21loadShaderPair(
  const char* shaderDir,
  const char* vertexFileName, 
  const char* fragmentFileName, 
  const mgVertexAttrib* attribs)
{
  mgString NVshader;
  NVshader.format("%sGL21/NVIDIA/", (const char*) shaderDir);

  mgString ATIshader;
  ATIshader.format("%sGL21/ATI/", (const char*) shaderDir);

  // Create shader objects
  GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  
  // find a version which compiles on this display
  if (!GL21compileShaderPair(NVshader, hVertexShader, vertexFileName, hFragmentShader, fragmentFileName))
    if (!GL21compileShaderPair(ATIshader, hVertexShader, vertexFileName, hFragmentShader, fragmentFileName))
      throw new mgErrorMsg("glBadShader", "vsName,fsName", "%s,%s", (const char*) vertexFileName, (const char*) fragmentFileName);

  // create the shader program
  GLuint uiShaderID = glCreateProgram();

  glAttachShader(uiShaderID, hVertexShader);
  glAttachShader(uiShaderID, hFragmentShader);

  // bind all the attributes
  for (int i = 0; ; i++)
  {
    if (attribs[i].m_name == NULL)
      break;
    glBindAttribLocation(uiShaderID, i, attribs[i].m_name);
  }

  glLinkProgram(uiShaderID);
  
  // These are no longer needed
  glDeleteShader(hVertexShader);
  glDeleteShader(hFragmentShader);  
    
  // Make sure link worked too
  GLint testVal;
  glGetProgramiv(uiShaderID, GL_LINK_STATUS, &testVal);
  if (testVal == GL_FALSE)
  {
    // get the linker log
    GLsizei msgSize, msgLen;
    glGetProgramiv(uiShaderID, GL_INFO_LOG_LENGTH, &msgSize);
    GLchar* message = new char[msgSize];
    glGetProgramInfoLog(uiShaderID, msgSize, &msgLen, message);
    mgDebug("%s", (const char*) message);
    delete message;

    glDeleteProgram(uiShaderID);
    uiShaderID = 0;

    throw new mgErrorMsg("glBadShaderLink", "vsName,fsName", "%s,%s", (const char*) vertexFileName, (const char*) fragmentFileName);
  }

  return uiShaderID;
}

#endif
