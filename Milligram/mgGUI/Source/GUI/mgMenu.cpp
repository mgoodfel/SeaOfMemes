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

#include "stdafx.h"

#ifdef DEBUG_MEMORY
const char THIS_FILE[] = __FILE__;
#define new new(THIS_FILE, __LINE__)
#endif

#include "GUI/mgMenu.h"

const int MGMENU_SEPARATOR = 0;
const int MGMENU_ACTION = 1;
const int MGMENU_TOGGLE = 2;
const int MEMENU_SUBMENU = 3;

class mgMenuItem
{
public:
  int m_type;
  mgString m_label;
  const mgIcon* m_icon;
  mgString m_name;
  int m_accelChar;
  mgString m_keyEquiv;

  mgMenu* m_subMenu;

  BOOL m_enabled;
  BOOL m_state;

  // constructor
  mgMenuItem();

  // destructor
  virtual ~mgMenuItem();
};

//--------------------------------------------------------------
// constructor
mgMenuItem::mgMenuItem()
{
  m_icon = NULL;
  m_accelChar = 0;
  m_subMenu = NULL;
  m_enabled = true;
  m_state = false;
}

//--------------------------------------------------------------
// constructor
mgMenuItem::~mgMenuItem()
{
  delete m_subMenu;
  m_subMenu = NULL;
}

//--------------------------------------------------------------
// constructor
mgMenu::mgMenu()
{
}

//--------------------------------------------------------------
// destructor
mgMenu::~mgMenu()
{
  for (int i = 0; i < m_items.length(); i++)
  {
    mgMenuItem* item = (mgMenuItem*) m_items[i];
    delete item;
  }
  m_items.removeAll();
}

//--------------------------------------------------------------
// add menu item
void mgMenu::addMenuAction(
  const char* label,
  const mgIcon* icon,
  const char* name,
  int accelChar,
  const char* keyEquiv)
{
  mgMenuItem* item = new mgMenuItem;
  item->m_type = MGMENU_ACTION;
  item->m_label = label;
  item->m_icon = icon;
  item->m_name = name;
  item->m_accelChar = accelChar;
  item->m_keyEquiv = keyEquiv;

  m_items.add(item);
}

//--------------------------------------------------------------
// add check menu item
void mgMenu::addMenuToggle(
  const char* label,
  const char* name,
  int accelChar,
  const char* keyEquiv)
{
  mgMenuItem* item = new mgMenuItem;
  item->m_type = MGMENU_TOGGLE;
  item->m_label = label;
  item->m_name = name;
  item->m_accelChar = accelChar;
  item->m_keyEquiv = keyEquiv;

  m_items.add(item);
}

//--------------------------------------------------------------
// add submenu item
void mgMenu::addSubMenu(
  const char* label,
  mgMenu* subMenu, 
  int accelChar)
{
  mgMenuItem* item = new mgMenuItem;
  item->m_type = MGMENU_ACTION;
  item->m_label = label;
  item->m_subMenu = subMenu;
  item->m_accelChar = accelChar;

  m_items.add(item);
}

//--------------------------------------------------------------
// add separator
void mgMenu::addSeparator()
{
  mgMenuItem* item = new mgMenuItem;
  item->m_type = MGMENU_SEPARATOR;

  m_items.add(item);
}

//--------------------------------------------------------------
// enable/disable item
void mgMenu::setItemEnabled(
  const char* name,
  BOOL enabled)
{
  for (int i = 0; i < m_items.length(); i++)
  {
    mgMenuItem* item = (mgMenuItem*) m_items[i];
    if (item->m_name.equalsIgnoreCase(name))
    {
      item->m_enabled = enabled;
      break;
    }
  }
}

//--------------------------------------------------------------
// set toggle item state
void mgMenu::setToggleState(
  const char* name,
  BOOL state)
{
  for (int i = 0; i < m_items.length(); i++)
  {
    mgMenuItem* item = (mgMenuItem*) m_items[i];
    if (item->m_name.equalsIgnoreCase(name))
    {
      item->m_state = state;
      break;
    }
  }
}
