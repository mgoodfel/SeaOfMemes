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

#ifndef MGCONTROL_H
#define MGCONTROL_H

class mgTopControl;
class mgLayoutManager;
class mgStyle;

class mgControlListener;
class mgMouseListener;
class mgKeyListener;
class mgFocusListener;
class mgTimeListener;

/*
  The superclass of all controls.  Controls are created as children of an
  existing parent.  There will be an mgTopControl instance created on the
  rendering surface.  Controls have names and rectangular bounds.  They 
  can draw on the surface and respond to various input events delivered
  by "Listeners."
*/
class mgControl
{
public:
  // constructor
  mgControl(
    mgControl* parent,
    const char* cntlName = NULL);
    
  // destructor
  virtual ~mgControl();
  
  // set the name
  virtual void setName(
    const char* cntlName);

  // get the name
  virtual const char* getName() const
  {
    return (const char*) m_cntlName; 
  }
  
  // return parent control
  virtual mgControl* getParent() const;

  // return control to use as parent for menu panes, other popups
  virtual mgControl* getPage();
  
  // set layout manager
  virtual void setLayout(
    mgLayoutManager* layout);
  
  // return layout manager
  virtual mgLayoutManager* getLayout();
  
  // return UI style 
  virtual mgStyle* getStyle();
  
  // update layout of children
  virtual void updateLayout();
  
  // set control location
  virtual void setLocation(
    int x,
    int y);
    
  // set control location
  virtual void setLocation(
    const mgPoint& pt)
  {
    setLocation(pt.m_x, pt.m_y);
  }
    
  // get control location
  virtual void getLocation(
    int& x,
    int& y) const;
  
  // get control location
  virtual void getLocation(
    mgPoint& pt) const
  {
    getLocation(pt.m_x, pt.m_y);
  }
  
  // set control size
  virtual void setSize(
    int width,
    int height);
    
  // set control size
  virtual void setSize(
    const mgDimension& size)
  {
    setSize(size.m_width, size.m_height);
  }
    
  // get control size
  virtual void getSize(
    int& width,
    int& height) const;

  // get control size
  virtual void getSize(
    mgDimension& size) const
  {
    getSize(size.m_width, size.m_height);
  }
  
  // set control bounds
  virtual void setBounds(
    const mgRectangle& bounds);
  
  // get control bounds
  virtual void getBounds(
    mgRectangle& bounds);
  
  // set visible flag
  virtual void setVisible(
    BOOL visible);
    
  // return visible flag
  virtual BOOL getVisible();

  // set enabled flag
  virtual void setEnabled(
    BOOL enabled);
    
  // get enabled flag
  virtual BOOL getEnabled();

  // add a child to top of stack
  virtual void addChild(
    mgControl* child);
    
  // add child to bottom of stack
  virtual void addChildToBottom(
    mgControl* child);
    
  // remove a child
  virtual void removeChild(
    mgControl* child);
  
  // raise to top of siblings
  virtual void raiseToTop();
  
  // sink to bottom of siblings
  virtual void sinkToBottom();
  
  // get child count
  virtual int childCount() const;
  
  // get nth child
  virtual mgControl* getChild(
    int index) const;

  // damage rectangle within control
  virtual void damage(
    int x,
    int y,
    int width,
    int height);
  
  // damage rectangle within control
  virtual void damage(
    const mgRectangle& bounds);
  
  // damage entire rectangle of control
  virtual void damage();
  
  // get graphics context to draw with
  virtual mgContext* newContext();

  // get rendering surface
  virtual mgSurface* getSurface();

//  // set cursor to use for this control
//  virtual void setCursor(
//    const TKCursor* cursor);
    
//  // get cursor pattern for this control
//  virtual const TKCursor* getCursor();
  
  // repaint background
  virtual void paintBackground(
    mgContext* gc);
    
  // repaint foreground
  virtual void paintForeground(
    mgContext* gc);
    
  // repaint children of control
  virtual void paintChildren(
    mgContext* gc,
    int clipLeft,
    int clipTop,
    int clipRight,
    int clipBottom);
    
  // paint control and all children
  virtual void paintControl(
    mgContext* gc,
    int clipLeft,
    int clipTop,
    int clipRight,
    int clipBottom);
    
  // paint content of control
  virtual void paint(
    mgContext* gc);

  // get minimum size of control
  virtual void minimumSize(
    mgDimension& size);

  // get preferred size of control
  virtual void preferredSize(
    mgDimension& size);
    
  // compute size at width.  return false if not implemented
  virtual BOOL preferredSizeAtWidth(
    int width,
    mgDimension& size);

  // return true if control is ancestor of arg
  virtual BOOL isAncestorOf(
    mgControl* cntl);

  // convert local coordinates to topControl coordinates
  virtual void getLocationInTop(
    mgPoint& point);

  // convert local coordinates to ancestor coordinates
  virtual void getLocationInAncestor(
    mgControl* ancestor,
    mgPoint& point);

  // convert local coordinates to ancestor coordinates
  virtual void getLocationInAncestor(
    mgControl* ancestor,
    mgRectangle& rect);

  // find control under point
  virtual mgControl* findControlAtPoint(
    int x,
    int y);

  // return true if control accepts key focus
  virtual BOOL acceptsKeyFocus();

  // take the key focus
  virtual void takeKeyFocus();

  // release the key focus
  virtual void releaseKeyFocus();

  // return true if control is key focus
  virtual BOOL isKeyFocus();

  // set the key focus
  virtual void setKeyFocus(
    mgControl* focus);

  // return the key focus
  virtual mgControl* getKeyFocus();

  // return the mouse focus
  virtual mgControl* getMouseFocus();

  // return true if control is mouse focus
  virtual BOOL isMouseFocus();

  // add a control listener
  virtual void addControlListener(
    mgControlListener* listener);

  // remove a control listener
  virtual void removeControlListener(
    mgControlListener* listener);

  // add a mouse listener
  virtual void addMouseListener(
    mgMouseListener* listener);

  // remove a mouse listener
  virtual void removeMouseListener(
    mgMouseListener* listener);

  // add a key listener
  virtual void addKeyListener(
    mgKeyListener* listener);

  // remove a key listener
  virtual void removeKeyListener(
    mgKeyListener* listener);

   // add a focus listener
  virtual void addFocusListener(
    mgFocusListener* listener);

  // remove a focus listener
  virtual void removeFocusListener(
    mgFocusListener* listener);

  // add time listener
  virtual void addTimeListener(
    mgTimeListener* listener);

  // remove time listener
  virtual void removeTimeListener(
    mgTimeListener* listener);

protected:
//  const TKCursor* m_cursor;               // cursor pattern, or NULL
  mgLayoutManager* m_layout;              // layout manager, or NULL
  
  mgControl* m_parent;
  mgPtrArray* m_children;

  mgPtrArray* m_controlListeners;
  mgPtrArray* m_mouseListeners;
  mgPtrArray* m_keyListeners;
  mgPtrArray* m_focusListeners;
  
  // send show/hide notifications for children
  virtual void notifyVisible(
    BOOL show);

  // send resize to control listeners
  virtual void dispatchControlResize(
    void* source);

  // send move to control listeners
  virtual void dispatchControlMove(
    void* source);

  // send show to control listeners
  virtual void dispatchControlShow(
    void* source);

  // send hide to control listeners
  virtual void dispatchControlHide(
    void* source);

  // send enable to control listeners
  virtual void dispatchControlEnable(
    void* source);

  // send disable to control listeners
  virtual void dispatchControlDisable(
    void* source);

  // send delete to control listeners
  virtual void dispatchControlDelete(
    void* source);

  // send addChild to control listeners
  virtual void dispatchControlAddChild(
    void* source);

  // send removeChild to control listeners
  virtual void dispatchControlRemoveChild(
    void* source);

  // send enter to mouse listeners
  virtual void dispatchMouseEnter(
    void* source,
    int x,
    int y);

  // send exited to mouse listeners
  virtual void dispatchMouseExit(
    void* source);

  // send down to mouse listeners
  virtual void dispatchMouseDown(
    void* source,
    int x,
    int y,
    int modifiers,
    int button);

  // send up to mouse listeners
  virtual void dispatchMouseUp(
    void* source,
    int x,
    int y,
    int modifiers,
    int button);

  // mouse clicked
  virtual void dispatchMouseClick(
    void* source,
    int x,
    int y,
    int modifiers,
    int button,
    int clickCount);

  // mouse dragged
  virtual void dispatchMouseDrag(
    void* source,
    int x,
    int y,
    int modifiers);

  // mouse moved
  virtual void dispatchMouseMove(
    void* source,
    int x,
    int y,
    int modifiers);

  // send down to key listeners
  virtual void dispatchKeyDown(
    void* source,
    int key,
    int modifiers);

  // send up to key listeners
  virtual void dispatchKeyUp(
    void* source,
    int key,
    int modifiers);

  // send char to key listeners
  virtual void dispatchKeyChar(
    void* source,
    int key,
    int modifiers);

  // send gained to focus listeners
  virtual void dispatchFocusGained(
    void* source);
    
  // send lost to focus listeners
  virtual void dispatchFocusLost(
    void* source);
    
#ifdef WORKED
  // inform topcontrol of cursor change
  virtual void cursorChanged(
    mgControl* source,
    const TKCursor* cursor);
#endif

private:
  mgString m_cntlName;
  
  int m_x;
  int m_y;
  int m_width;
  int m_height;
  BOOL m_visible;
  BOOL m_enabled;

  friend class mgTopControl;
};

#endif
