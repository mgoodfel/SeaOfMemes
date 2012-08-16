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

#ifdef __APPLE__
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
typedef int SOCKET;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;
typedef struct sockaddr SOCKADDR;
#define closesocket close
#endif

//#include "LogUI.h"

const int MAX_LOG_LENGTH = 40960;

#ifdef WIN32
//--------------------------------------------------------------
// wait for parent process to end, get return code
DWORD waitForParent(
  const char* commandLine)
{
  // extract the process id of the parent
  DWORD processId;
  if (1 != sscanf(commandLine, "%d", &processId))
  {
    mgDebug("can't parse process id -- '%s'", commandLine);
    return 0;
  }

  // get a handle and wait for parent to end
  HANDLE parent = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, processId);
  if (parent == NULL)
  {
    mgDebug("can't get parent handle, GetLastError = %d", GetLastError());
    return 0;
  }
  WaitForSingleObject(parent, INFINITE);

  DWORD processRC = 0;
  GetExitCodeProcess(parent, &processRC);
  CloseHandle(parent);

  return processRC;
}
#endif

#if defined(__unix__) || defined(__APPLE__)

#endif

//--------------------------------------------------------------
// convert to urlEncode format
void urlEncode(
  mgString& formData,
  const char* logData,
  int logLen)
{
  const char* HEXDIGITS = "0123456789ABCDEF";
  for (int i = 0; i < logLen; i++)
  {
    char c = logData[i];
    if (c == ' ')
      formData += '+';
    else if (isalnum(c) || c == '.')
      formData += c;
    else
    {
      // Escape: next 2 chars are hex representation of 
      // the actual character 
      formData += '%';
      formData += HEXDIGITS[c / 16];
      formData += HEXDIGITS[c % 16];
    }
  }
}

//--------------------------------------------------------------
// write the log to the log server
void writeLog(
  const char* hostAddr,
  const char* logData,
  int logLen)
{
#ifdef WIN32
  // Initialize Winsock
  WSADATA wsaData;
  int wsrc = WSAStartup(MAKEWORD(2, 0), &wsaData);
  if (wsrc != NO_ERROR) 
    mgDebug("mgSendLog: Windows socket initialization failed.");
#endif

  // Create a SOCKET for connecting to server
  SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == INVALID_SOCKET) 
  {
    mgDebug("mgSendLog: Failed to create socket.");
    return;
  }

  // parse host name into name:port
  const char* colon = strchr(hostAddr, ':');
  mgString serverName;
  int serverPort;
  if (colon != NULL)
  {
    serverName.write(hostAddr, colon-hostAddr);
    sscanf(colon+1, "%d", &serverPort);
  }
  else 
  {
    serverName = hostAddr;
    serverPort = 8080;
  }

  // look up host name
  struct hostent* host = gethostbyname(serverName);
  if (host == NULL || host->h_addr_list[0] == NULL)
  {
    mgDebug("mgSendLog: gethostbyname(%s) failed.", (const char*) serverName);
    return;
  }

  struct sockaddr_in serviceAddr;
  memset(&serviceAddr, 0, sizeof(serviceAddr));
  serviceAddr.sin_family = AF_INET;
  serviceAddr.sin_addr = *(struct in_addr *) host->h_addr_list[0];
  serviceAddr.sin_port = htons(serverPort);

  // connect to host on port
  int rc = connect(sock, (SOCKADDR*) &serviceAddr, sizeof(serviceAddr));
  if (rc == SOCKET_ERROR)
  {
    mgDebug("mgSendLog: connect failed.");
    closesocket(sock);
    return;
  }

  // urlencode the form data 
  mgString formData("log=");
  urlEncode(formData, logData, logLen);

  // send request
  mgString line;
  mgString request;
  request += "POST /logs/report HTTP/1.1\r\n";
  line.format("Host: %s\r\n", hostAddr);
  request += line;
  line.format("Content-Length: %d\r\n", formData.length());
  request += line;
  request += "Content-Type: application/x-www-form-urlencoded\r\n";

  request += "\r\n";
  request += formData;

  const char* requestData = (const char*) request;
  int requestLen = request.length();
  int posn = 0;
  while (posn < requestLen)
  {
    int bytesSent = send(sock, posn+requestData, requestLen-posn, 0);
    if (bytesSent == SOCKET_ERROR)
    {
      mgDebug("mgSendLog: send failed on socket.");
      closesocket(sock);
      return;
    }
    posn += bytesSent;
  }

  // read reply
  mgString version;
  BYTE buffer[2048];
  posn = 0;
  int used = 0;
  while (true)
  {
    int bytesRead = recv(sock, (char*) buffer+posn, sizeof(buffer)-posn, 0);
    if (bytesRead == SOCKET_ERROR)
    {
      mgDebug("mgSengLog: error on recv reply from host");
      break;
    }
    else if (bytesRead <= 0)
      break;
    posn += bytesRead;
  }
  closesocket(sock);
  
  mgDebug("mgSendLog sent %d bytes to %s:%d", request.length(), (const char*) serverName, serverPort);

#ifdef WIN32
  WSACleanup();
#endif
}

#ifdef WIN32
//--------------------------------------------------------------
// main entry
int WINAPI WinMain( 
  HINSTANCE hInstance,            // Instance
  HINSTANCE hPrevInstance,        // Previous Instance
  LPSTR lpCmdLine,                // Command Line Parameters
  int nCmdShow)                   // Window Show State
{
#ifdef DEBUG_MEMORY
  mgDebugMemoryInit();
#endif

  // wait for parent process to end, get rc
  DWORD parentRC = 0;
  if (strlen(lpCmdLine) > 0)
    parentRC = waitForParent(lpCmdLine);
#endif

#if defined(__unix__) || defined(__APPLE__)
#include <semaphore.h>

//--------------------------------------------------------------
// main entry
int main(
  int argc,
  char** argv)
{
#ifdef DEBUG_MEMORY
  mgDebugMemoryInit();
#endif

  DWORD parentRC = 0;
  if (argc > 1)
  {
    sscanf(argv[1], "%d", &parentRC);
  }
#endif


//--------------------------------------------------------------
// common-platform code:

  // find the errors.txt file and make that the current directory
  try
  {
    mgOSFindWD("errors.txt");
  }
  catch (...)
  {
    // if we can't find errors file, DON'T create one by writing an error message!
    return 0;
  }

  if (parentRC != 0)
    mgDebug(":Session exit: rc=%d", parentRC);

  // try to open the errors file
  FILE* errors = fopen("errors.txt", "rb");
  if (errors == NULL)
    return 0;

  // read the errors file into memory
  fseek(errors, 0, SEEK_END);
  long logLen = ftell(errors);
  logLen = min(logLen, MAX_LOG_LENGTH);

  char* logData = new char[logLen+1];
  fseek(errors, 0, SEEK_SET);
  size_t bytesRead = fread(logData, 1, logLen, errors);
  if (bytesRead == 0)
    return 0;

  logData[logLen] = '\0';
  fclose(errors);

  // since we're running in the same directory as the parent,
  // our log messages will be written to errors.txt.  identify start 
  // of new messages.
  mgDebug("------mgSendLog");

  // read the options
  mgOptionsFile options;

  // use defaults if there are no log options
  try
  {
    mgString optionsFile;
    if (mgOSFindFile(optionsFile, "logoptions.xml"))
      options.parseFile(optionsFile);
  }
  catch (...)
  {
  }

  mgString hostAddr;
  options.getString("loghost", "localhost:8080", hostAddr);

  mgString action;
  options.getString("action", "prompt", action);
  if (action.equalsIgnoreCase("never"))
    return 0;

  if (action.equalsIgnoreCase("prompt") || 
      (action.equalsIgnoreCase("error") && parentRC != 0))
  {
//    displayUI();  // rc, log data, user input
    // return if cancel from ui
  }

  // send the log and user input to the host
  writeLog(hostAddr, logData, logLen);

  delete logData;  
  logData = NULL;

#ifdef DEBUG_MEMORY
  // display all memory leaks
  mgDebugMemory();
#endif

  return 0;
}
