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

#ifndef MGSTYLE_H
#define MGSTYLE_H

/*
  This is the parent of all UI styles.  A style must create control instances, 
  and MAY also return fonts, colors, etc. for a set of controls.
*/

/* 
  Use these constants to set an attribute on all instances of a particular
  control type.  ex: setAttr(MG_STYLE_LABEL, "font", aFont) would set the
  font for all labels.  This can be overriden for a particular label by
  calling setAttr("myLabel", "font", otherFont).

  A style may or may not honor attributes set by the programmer.

  These names are also the cntlClass argument to the get*Attr methods.
*/
static const char* MG_STYLE_ALL       = "*ALL*";
static const char* MG_STYLE_LABEL     = "*LABEL*";
static const char* MG_STYLE_BUTTON    = "*BUTTON*";
static const char* MG_STYLE_CHECKBOX  = "*CHECKBOX*";
static const char* MG_STYLE_FIELD     = "*FIELD*";
static const char* MG_STYLE_CONSOLE   = "*CONSOLE*";
static const char* MG_STYLE_STACK     = "*STACK*";
static const char* MG_STYLE_LIST      = "*LIST*";
static const char* MG_STYLE_SCROLLBAR = "*SCROLLBAR*";

class mgControl;
class mgLabelControl;
class mgButtonControl;
class mgCheckboxControl;
class mgFieldControl;
class mgConsoleControl;
class mgStackControl;

class mgFont;
class mgColor;
class mgBrush;
class mgPen;
class mgIcon;
class mgPaint;
class mgFrame;

class mgStyle
{
public:
  // constructor
  mgStyle();

  // destructor
  virtual ~mgStyle();

  // create a label control
  virtual mgLabelControl* createLabel(
    mgControl* parent,
    const char* name, 
    const char* label) = 0;

  // create a field control
  virtual mgFieldControl* createField(
    mgControl* parent,
    const char* name) = 0;

  // create a button control
  virtual mgButtonControl* createButton(
    mgControl* parent,
    const char* name, 
    const char* label,
    const char* iconName) = 0;

  // create a checkbox control
  virtual mgCheckboxControl* createCheckbox(
    mgControl* parent,
    const char* name, 
    const char* label) = 0;

  // create a console control
  virtual mgConsoleControl* createConsole(
    mgControl* parent,
    const char* name) = 0;

  // create a stack control
  virtual mgStackControl* createStack(
    mgControl* parent,
    const char* name) = 0;

  // set a string attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    const char* value);

  // set an integer attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    int value);

  // set a floating attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    double value);

  // set a font attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    const mgFont* font);

  // set a color attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    const mgColor& color);

  // set a pen attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    const mgPen* pen);

  // set a brush attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    const mgBrush* brush);

  // set an icon attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    const mgIcon* icon);

  // set a paint attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    const mgPaint* paint);

  // set a frame attribute
  virtual void setAttr(
    const char* cntlName,
    const char* attrName,
    const mgFrame* frame);

  // return a string attribute
  virtual BOOL getStringAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    mgString& value);

  // return an integer attribute
  virtual BOOL getIntegerAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    int& value);

  // return a floating attribute
  virtual BOOL getDoubleAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    double& value);

  // return a font attribute
  virtual BOOL getFontAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    const mgFont*& value);

  // return a color
  virtual BOOL getColorAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    mgColor& color);

  // return a brush attribute
  virtual BOOL getBrushAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    const mgBrush*& value);

  // return a pen attribute
  virtual BOOL getPenAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    const mgPen*& value);

  // return an icon attribute
  virtual BOOL getIconAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    const mgIcon*& value);

  // return a paint attribute
  virtual BOOL getPaintAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    const mgPaint*& value);

  // return a frame attribute
  virtual BOOL getFrameAttr(
    const char* cntlClass,
    const char* cntlName,
    const char* attrName,
    const mgFrame*& value);

protected:
  mgMapStringToString m_stringAttrs;
  mgMapStringToDWord m_dwordAttrs;
  mgMapStringToDouble m_doubleAttrs;
  mgMapStringToPtr m_fontAttrs;
  mgMapStringToDWord m_colorAttrs;
  mgMapStringToPtr m_penAttrs;
  mgMapStringToPtr m_brushAttrs;
  mgMapStringToPtr m_iconAttrs;
  mgMapStringToPtr m_paintAttrs;
  mgMapStringToPtr m_frameAttrs;
};

#endif
