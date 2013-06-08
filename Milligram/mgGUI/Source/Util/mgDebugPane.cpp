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

/*
 -Description: 
   See mgDebugPane.h
*/

#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "GUI/mgToolkit.h"
#include "Util/mgDebugPane.h"

const mgColor ECHO_COLOR = mgColor("blue");
const mgColor OUTPUT_COLOR = mgColor("white");
const mgColor ERROR_COLOR = mgColor("red");

//--------------------------------------------------------------
// constructor
mgDebugPane::mgDebugPane(
  mgControl* parent,
  const char* cntlName)
: mgControl(parent, cntlName)
{
  m_debugApp = NULL;

  mgStyle* style = getStyle();

  // build the console and input line children
  m_console = style->createConsole(this, "mgDebugConsole");
  m_input = style->createField(this, "mgDebugInput");
  m_input->enableHistory(true);

  // listen to input line
  m_input->addChangeListener((mgChangeListener*) this);

  addControlListener((mgControlListener*) this);
}

//--------------------------------------------------------------
// destructor
mgDebugPane::~mgDebugPane()
{
}

//--------------------------------------------------------------
// set debug app
void mgDebugPane::setDebugApp(
  mgDebugInterface* app)
{
  m_debugApp = app;

  // reset variable and function list
  m_varNames.removeAll();
  m_varHelp.removeAll();
  m_funcNames.removeAll();
  m_funcParms.removeAll();
  m_funcHelp.removeAll();

  if (m_debugApp == NULL)
    return;

  // get variable list
  m_debugApp->debugListVariables(m_varNames, m_varHelp);

  // get function list
  m_debugApp->debugListFunctions(m_funcNames, m_funcParms, m_funcHelp);

  // write the intro line
  mgString text;
  m_debugApp->debugIntro(text);
  m_console->addLine(OUTPUT_COLOR, NULL, text);
}

//--------------------------------------------------------------
// add a message to the console
void mgDebugPane::addLine(
  const mgColor& color,
  const mgFont* font,
  const char* message)
{
  m_console->addLine(color, font, message);
}

//--------------------------------------------------------------
// set number of lines default height
void mgDebugPane::setConsoleLines(
  int lineCount)
{
  m_console->setDisplayLines(lineCount);
}

//--------------------------------------------------------------
// set history
void mgDebugPane::setConsoleHistory(
  int lineCount)
{
  m_console->setHistoryLines(lineCount);
}

//--------------------------------------------------------------
// return minimum size
void mgDebugPane::minimumSize(
  mgDimension& size)
{
  mgDimension consoleSize, inputSize;
  m_console->minimumSize(consoleSize);
  m_input->minimumSize(inputSize);

  size.m_width = max(consoleSize.m_width, inputSize.m_width);
  size.m_height = consoleSize.m_height + inputSize.m_height;
}  

//--------------------------------------------------------------
// return preferred size
void mgDebugPane::preferredSize(
  mgDimension& size)
{
  mgDimension consoleSize, inputSize;
  m_console->preferredSize(consoleSize);
  m_input->preferredSize(inputSize);

  size.m_width = max(consoleSize.m_width, inputSize.m_width);
  size.m_height = consoleSize.m_height + inputSize.m_height;
}  

//--------------------------------------------------------------
// update layout of controls
void mgDebugPane::updateLayout()
{
  mgDimension size;
  getSize(size);

  mgDimension inputSize;
  m_input->preferredSize(inputSize);

  int consoleHeight = size.m_height - inputSize.m_height;
  m_console->setSize(size.m_width, consoleHeight);
  m_input->setSize(size.m_width, inputSize.m_height);

  m_input->setLocation(0, consoleHeight);
}

//--------------------------------------------------------------
// list defined variables
void mgDebugPane::listVariables()
{
  if (m_varNames.length() == 0)
  {
    m_console->addLine(OUTPUT_COLOR, NULL, "No variables defined.");
    return;
  }

  m_console->addLine(OUTPUT_COLOR, NULL, "Variables:");

  // get the name lengths so we can line up comments
  int nameLen = 0;
  for (int i = 0; i < m_varNames.length(); i++)
  {
    int len = (int) strlen(m_varNames[i]);
    nameLen = max(nameLen, len);
  }

  // keep lengths in range for prettier output
  nameLen = max(10, min(20, nameLen));

  // display variable and help text 
  mgString line;
  mgString varName;
  for (int i = 0; i < m_varNames.length(); i++)
  {
    varName = m_varNames[i];

    // pad out name to align comments
    while (varName.length() < nameLen)
      varName += " ";

    line.format("  %s   // %s", (const char*) varName, m_varHelp[i]);
    m_console->addLine(OUTPUT_COLOR, NULL, line);
  }
}

//--------------------------------------------------------------
// list defined functions
void mgDebugPane::listFunctions()
{
  if (m_funcNames.length() == 0)
  {
    m_console->addLine(OUTPUT_COLOR, NULL, "No functions defined.");
    return;
  }

  m_console->addLine(OUTPUT_COLOR, NULL, "Functions:");

  int nameLen = 0;
  for (int i = 0; i < m_funcNames.length(); i++)
  {
    int len = (int) (strlen(m_funcNames[i]) + strlen(m_funcParms[i]) + 2);
    nameLen = max(nameLen, len);
  }
  nameLen = max(15, min(30, nameLen));

  // display variable and help text 
  mgString funcName;

  mgString line;
  for (int i = 0; i < m_funcNames.length(); i++)
  {
    funcName.format("%s(%s)", m_funcNames[i], m_funcParms[i]);
    while (funcName.length() < nameLen)
      funcName += " ";
      
    line.format("  %s   // %s", (const char*) funcName, m_funcHelp[i]);
    m_console->addLine(OUTPUT_COLOR, NULL, line);
  }
}

//--------------------------------------------------------------
// show variable value(s)
BOOL mgDebugPane::parseGetVariable(
  const char* input)
{
  // parse "varname", "varname*"
  // allow var to match all of var.x, var.y, etc.

  mgString query;
  BOOL wildcard = false;

  const int GETVAR_START        = 0;
  const int GETVAR_TOKEN        = 1;
  const int GETVAR_DONE         = 2;

  int state = GETVAR_START;
  for (int i = 0; ; i++)
  {
    char c = input[i];
    if (c == '\0')
      break;
    switch (state)
    {
      case GETVAR_START:
        if (isalnum(c))
        {
          state = GETVAR_TOKEN;
          query += c;
        }
        else if (c == '*')
        {
          state = GETVAR_DONE;
          wildcard = true;
        }
        else if (!isspace(c))
          return false;    // invalid char
        break;

      case GETVAR_TOKEN:
        if (isalnum(c) || c == '.')
          query += c;
        else if (c == '*')
        {
          state = GETVAR_DONE;
          wildcard = true;
        }
        else if (isspace(c))
          state = GETVAR_DONE;
        else return false;  // invalid char
        break;

      case GETVAR_DONE:
        // must be last char
        if (!isspace(c))
          return false;  // invalid char
        break;
    }
  }

  // dotted form of name, to match all elements
  query.makeLower();
  mgString queryDot(query);
  queryDot += ".";

  // get the list of variables to print
  mgString varName, varValue;
  mgString lowname;
  mgString helpText;
  mgString line;

  BOOL found = false;
  for (int i = 0; i < m_varNames.length(); i++)
  {
    lowname = m_varNames[i];
    lowname.makeLower();
    if (lowname.equals(query) || 
        lowname.startsWith(queryDot) ||
        (wildcard && lowname.startsWith(query)))
    {
      found = true;
      varName = m_varNames[i];
      varValue.empty();
      m_debugApp->debugGetVariable(varName, varValue);
      line.format("%s = %s", (const char*) varName, (const char*) varValue);
      m_console->addLine(OUTPUT_COLOR, NULL, line);
    }
  }

  if (!found)
  {
    line.format("variable '%s' not found", input);
    m_console->addLine(OUTPUT_COLOR, NULL, line);
  }

  return true;
}

//--------------------------------------------------------------
// set a variable
BOOL mgDebugPane::parseSetVariable(
  const char* input)
{
  // parse "var = value" or "var = \"value\""
  mgString setName;
  mgString setValue;

  const int SETVAR_START        = 0;
  const int SETVAR_TOKEN        = 1;
  const int SETVAR_EQUAL        = 2;
  const int SETVAR_VALUE        = 3;
  const int SETVAR_QUOTE        = 4;
  const int SETVAR_BS           = 5;
  const int SETVAR_DONE         = 6;

  int state = SETVAR_START;
  for (int i = 0; ; i++)
  {
    char c = input[i];
    if (c == '\0')
      break;
    switch (state)
    {
      case SETVAR_START:
        if (isalnum(c))
        {
          state = SETVAR_TOKEN;
          setName += c;
        }
        else if (!isspace(c))
          return false;    // invalid char
        break;

      case SETVAR_TOKEN:
        if (isalnum(c) || c == '.')
          setName += c;

        else if (c == '=')
          state = SETVAR_VALUE;

        else if (isspace(c))
          state = SETVAR_EQUAL;

        else return false;  // invalid char
        break;

      case SETVAR_EQUAL:
        if (c == '=')
          state = SETVAR_VALUE;
        else if (!isspace(c))
          return false;  // syntax error
        break;

      case SETVAR_VALUE:
        if (c == '"')
        {
          setValue.trim();
          if (setValue.length() > 0)
            return false;
          state = SETVAR_QUOTE;
        }
        else setValue += c;
        break;

      case SETVAR_QUOTE:
        if (c == '\\')
          state = SETVAR_BS;
        else if (c == '"')
          state = SETVAR_DONE;
        else setValue += c;
        break;

      case SETVAR_BS:
        // escape one character
        setValue += c;
        state = SETVAR_QUOTE;
        break;

      case SETVAR_DONE:
        if (!isspace(c))
          return false;
        break;
    }
  }

  setValue.trim();

  BOOL found = false;
  mgString varName;
  mgString line;

  for (int i = 0; !found && i < m_varNames.length(); i++)
  {
    varName = m_varNames[i];
    if (setName.equalsIgnoreCase(varName))
    {
      found = true;
      m_debugApp->debugSetVariable(varName, setValue, line);
      m_console->addLine(OUTPUT_COLOR, NULL, line);
    }
  }

  if (!found)
  {
    line.format("variable '%s' not found", (const char*) setName);
    m_console->addLine(OUTPUT_COLOR, NULL, line);
  }
  return true;
}

//--------------------------------------------------------------
// count parameters
int mgDebugPane::parmCount(
  const char* parms)
{
  int count = 0;
  BOOL hasParms = false;
  for (int i = 0; ; i++)
  {
    char c = parms[i];
    if (c == '\0')
      break;

    if (c == ',')
      count++;
    else if (!isspace(c))
      hasParms = true;
  }

  if (hasParms)
    return count+1;
  else return 0;
}

//--------------------------------------------------------------
// call a function
BOOL mgDebugPane::parseCallFunction(
  const char* input)
{
  // parse func(value, value, ... )

  mgString callName;
  mgStringArray argList;
  mgString argValue;

  const int CALL_START        = 0;
  const int CALL_NAME         = 1;
  const int CALL_NAME_END     = 2;
  const int CALL_ARG          = 3;
  const int CALL_QUOTE        = 4;
  const int CALL_BS           = 5;
  const int CALL_ARG_END      = 6;
  const int CALL_DONE         = 7;

  int state = CALL_START;
  for (int i = 0; ; i++)
  {
    char c = input[i];
    if (c == '\0')
      break;
    switch (state)
    {
      case CALL_START:
        if (isalnum(c))
        {
          state = CALL_NAME;
          callName += c;
        }
        else if (!isspace(c))
          return false;    // invalid char
        break;

      case CALL_NAME:
        if (isalnum(c))
          callName += c;

        else if (c == '(')
          state = CALL_ARG;

        else if (isspace(c))
          state = CALL_NAME_END;

        else return false;  // invalid char
        break;

      case CALL_NAME_END:
        if (c == '(')
          state = CALL_ARG;
        else if (!isspace(c))
          return false;  // syntax error
        break;

      case CALL_ARG:
        if (c == '"')
        {
          argValue.trim();
          if (argValue.length() > 0)
            return false;
          state = CALL_QUOTE;
        }
        else if (c == ',')
        {
          argList.add(argValue);
          argValue.empty();
        }
        else if (c == ')')
        {
          argList.add(argValue);
          argValue.empty();
          state = CALL_DONE;
        }
        else argValue += c;
        break;

      case CALL_QUOTE:
        if (c == '\\')
          state = CALL_BS;
        else if (c == '"')
        {
          argList.add(argValue);
          argValue.empty();
          state = CALL_ARG_END;
        }
        else argValue += c;
        break;

      case CALL_BS:
        // escape one character
        argValue += c;
        state = CALL_QUOTE;
        break;

      case CALL_ARG_END:
        if (c == ',')
          state = CALL_ARG;
        else if (c == ')')
          state = CALL_DONE;
        break;

      case CALL_DONE:
        if (!isspace(c))
          return false;
        break;
    }
  }

  BOOL found = false;
  mgString funcName;
  mgString funcParms;
  int minArgCount;
  mgString line;

  for (int i = 0; !found && i < m_funcNames.length(); i++)
  {
    funcName = m_funcNames[i];
    funcParms = m_funcParms[i];
    if (funcName.equalsIgnoreCase(callName))
    {
      found = true;
      minArgCount = parmCount(funcParms);
      if (minArgCount > argList.length())
        line.format("%s(%s) -- not enough arguments", (const char*) funcName, (const char*) funcParms);
      else 
      {
        m_debugApp->debugCallFunction(funcName, argList, line);
      }

      m_console->addLine(OUTPUT_COLOR, NULL, line);
    }
  }

  if (!found)
  {
    line.format("function '%s()' not found", (const char*) callName);
    m_console->addLine(OUTPUT_COLOR, NULL, line);
  }

  return true;
}

//--------------------------------------------------------------
// process input line
void mgDebugPane::processInput(
  const mgString& input)
{
  mgString verb;
  int posn = input.getToken(0, " *(=", verb);

  if (verb.equalsIgnoreCase("help"))
  {
    mgString helpText;
    m_debugApp->debugHelp(helpText);
    m_console->addLine(OUTPUT_COLOR, NULL, helpText);
  }
  else if (verb.equalsIgnoreCase("list"))
  {
    listVariables();
    listFunctions();
  }
  else
  {
    if (!parseGetVariable(input))
    {
      if (!parseSetVariable(input))
      {
        if (!parseCallFunction(input))
          m_console->addLine(ERROR_COLOR, NULL, "Unknown command.");
      }
    }
  }
}

//--------------------------------------------------------------
// control value changed
void mgDebugPane::guiChange(
  void* source,
  const char* name)
{
  mgString value;
  mgFieldControl* field = (mgFieldControl*) source;
  field->getText(value);
  field->reset();

  if (m_debugApp == NULL)
  {
    m_console->addLine(ERROR_COLOR, NULL, "mgDebugPane::setDebugApp() not called.");
    return;
  }

  value.trim();
  // echo the input
  if (value.length() > 0)
  {
    m_console->addLine(ECHO_COLOR, NULL, value);
    processInput(value);
  }
  else 
  {
    m_debugApp->debugIntro(value);
    m_console->addLine(OUTPUT_COLOR, NULL, value);
  }
}

//--------------------------------------------------------------
// control shown
void mgDebugPane::controlShow(
  void* source)
{
  // take the key focus
  m_input->takeKeyFocus();
}

