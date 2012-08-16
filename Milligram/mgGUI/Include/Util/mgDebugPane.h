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

/*
-Description:

  A DebugPane displays a scrolling console and input line.  It parses commands
  out of the input for displaying variables and calling functions.  Variables
  and Functions are implemented via a subclass and should be used to present
  debug information for apps.  Not intended for end-users.
*/

#ifndef MGDEBUGPANE_H
#define MGDEBUGPANE_H

#include "../GUI/mgControl.h"
#include "../GUI/mgChangeListener.h"
#include "../GUI/mgControlListener.h"

class mgConsoleControl;
class mgFieldControl;

class mgDebugInterface
{
public:
  // intro text
  virtual void debugIntro(
    mgString& text) = 0;

  // help text
  virtual void debugHelp(
    mgString& text) = 0;

  // describe variables.
  virtual void debugListVariables(
    mgStringArray& varName,
    mgStringArray& helpText) = 0;

  // describe functions.
  virtual void debugListFunctions(
    mgStringArray& funcName,
    mgStringArray& funcParms,
    mgStringArray& helpText) = 0;

  // return value of variable
  virtual void debugGetVariable(
    const char* varName,
    mgString& value) = 0;

  // set a variable
  virtual void debugSetVariable(
    const char* varName,
    const char* value,
    mgString& reply) = 0;

  // call a function
  virtual void debugCallFunction(
    const char* funcName,
    mgStringArray& args,
    mgString& reply) = 0;
};

class mgDebugPane : public mgControl, public mgChangeListener, public mgControlListener
{
public:
  // constructor
  mgDebugPane(
    mgControl* parent,
    const char* cntlName = NULL);

  // destructor
  virtual ~mgDebugPane();

  // set debug app
  virtual void setDebugApp(
    mgDebugInterface* app);

  // add a message to the console
  virtual void addLine(
    const mgColor& color,
    const mgFont* font,
    const char* message);

  // set number of lines default height
  virtual void setConsoleLines(
    int lineCount);

  // set history
  virtual void setConsoleHistory(
    int lineCount);

  // get minimum size of control
  virtual void minimumSize(
    mgDimension& size);

  // get preferred size of control
  virtual void preferredSize(
    mgDimension& size);
    
protected:
  mgDebugInterface* m_debugApp;

  mgStringArray m_varNames;
  mgStringArray m_varHelp;
  mgStringArray m_funcNames;
  mgStringArray m_funcParms;
  mgStringArray m_funcHelp;

  mgConsoleControl* m_console;
  mgFieldControl* m_input;

  // update layout of children
  virtual void updateLayout();
  
  // control value changed
  virtual void guiChange(
    void* source,
    const char* name);

  // list defined variables
  virtual void listVariables();

  // list defined functions
  virtual void listFunctions();

  // count parameters
  virtual int parmCount(
    const char* parms);

  // parse show variable value(s)
  virtual BOOL parseGetVariable(
    const char* input);

  // parse set variable
  virtual BOOL parseSetVariable(
    const char* input);

  // parse call function
  virtual BOOL parseCallFunction(
    const char* input);

  // process input line
  virtual void processInput(
    const mgString& input);

  // control resize
  virtual void controlResize(
    void* source)
  {}

  // control moved
  virtual void controlMove(
    void* source)
  {}

  // control shown
  virtual void controlShow(
    void* source);

  // control hidden
  virtual void controlHide(
    void* source)
  {}

  // control enabled
  virtual void controlEnable(
    void* source)
  {}

  // control disabled
  virtual void controlDisable(
    void* source)
  {}

  // control deleted
  virtual void controlDelete(
    void* source)
  {}
};

#endif
