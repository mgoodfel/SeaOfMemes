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
#ifndef MGFORMPARSER_H
#define MGFORMPARSER_H

class mgControl;
class mgFormPane;
class mgTextBuffer;
class mgListTag;

/*
  Parse the XML specification of a Form.
*/
class mgFormParser : public mgXMLParser
{
public:
  mgFormPane* m_form;         // top of form control tree
  mgMapStringToPtr* m_cntlNames;

  // constructor
  mgFormParser(
    mgFormPane* form);

  // constructor
  mgFormParser(
    mgFormPane* form,
    const char* fileName);

  // destructor
  virtual ~mgFormParser();

  // push new text buffer on stack
  virtual void pushText(
    mgTextBuffer* text);

  // pop a text buffer off the stack (does not free)
  virtual void popText();

  // return the top text buffer
  virtual mgTextBuffer* topText();

  // push a new list on the stack
  virtual void pushList(
    mgListTag* list);

  // pop a list tag off the stack (does not free)
  virtual void popList();

  // return top list tag
  virtual mgListTag* topList();

  // push a new parent control on the stack
  virtual void pushCntl(
    mgControl* cntl);

  // pop a control off the stack (does not free)
  virtual void popCntl();

  // return top control
  virtual mgControl* topCntl();

  // set a control name
  virtual void setCntlName(
    const char* name,
    mgControl* control);

  // get control by name
  virtual mgControl* getCntl(
    const char* cntlName);

protected:
  mgPtrArray m_textStack;
  mgPtrArray m_listStack;
  mgPtrArray m_cntlStack;

  // create a tag instance
  virtual mgXMLTag* createTag(
    const char* tagName);

  // handle the top tag (no parent to add it to)
  virtual void processTopTag(
    mgXMLTag* tag);
};

#endif
