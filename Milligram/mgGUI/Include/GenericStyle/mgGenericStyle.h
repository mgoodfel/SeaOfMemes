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

#ifndef MGGENERICSTYLE_H
#define MGGENERICSTYLE_H

#include "GUI/mgStyle.h"

#include "GenericStyle/mgGenericLabel.h"
#include "GenericStyle/mgGenericButton.h"
//#include "GenericStyle/mgGenericField.h"
//#include "GenericStyle/mgGenericCheckbox.h"
//#include "GenericStyle/mgGenericConsole.h"
//#include "GenericStyle/mgGenericStack.h"

class mgGenericFace;
class mgLabelFace;
class mgButtonFace;

#define MG_GENERIC_LABEL_DISABLED "LabelDisabled"
#define MG_GENERIC_LABEL_ENABLED "LabelEnabled"

#define MG_GENERIC_BUTTON_DISABLED "ButtonDisabled"
#define MG_GENERIC_BUTTON_UP "ButtonUp"
#define MG_GENERIC_BUTTON_DOWN "ButtonDown"
#define MG_GENERIC_BUTTON_HOVER "ButtonHover"

class mgGenericStyle : public mgStyle
{
public:
  // constructor
  mgGenericStyle(
    mgSurface* surface,
    const char* settingsFile);

  // destructor
  virtual ~mgGenericStyle();

  // get a control face
  virtual mgGenericFace* getFace(
    const char* name);

  // create a label control
  virtual mgLabelControl* createLabel(
    mgControl* parent,
    const char* name, 
    const char* label);

  // create a field control
  virtual mgFieldControl* createField(
    mgControl* parent,
    const char* name);

  // create a button control
  virtual mgButtonControl* createButton(
    mgControl* parent,
    const char* name, 
    const char* label,
    const char* iconName);

  // create a checkbox control
  virtual mgCheckboxControl* createCheckbox(
    mgControl* parent,
    const char* name, 
    const char* label);

  // create a console control
  virtual mgConsoleControl* createConsole(
    mgControl* parent,
    const char* name);

  // create a stack control
  virtual mgStackControl* createStack(
    mgControl* parent,
    const char* name);

protected:
  mgSurface* m_surface;

  mgLabelFace* m_labelDisabled;
  mgLabelFace* m_labelEnabled;

  mgButtonFace* m_buttonDisabled;
  mgButtonFace* m_buttonUp;
  mgButtonFace* m_buttonHover;
  mgButtonFace* m_buttonDown;
};

#endif
