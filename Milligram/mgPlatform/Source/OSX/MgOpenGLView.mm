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

#import "MgOpenGLView.h"

#include "stdafx.h"
#include "OSX/mgOSXServices.h"
#include "mgApplication.h"
#include <OpenGL/gl.h>
//#include "Graphics3D/GL33/mgGL33Misc.h"
//#include <OpenGL/gl3.h>

static const char* mgGLVersionFound = "?";

//--------------------------------------------------------------------
// return GL version found
const char* mgOpenGLVersion(void)
{
  return mgGLVersionFound;
}

//--------------------------------------------------------------------
// called from mgOSXServices when user toggles between MMO and FPS
// mouse modes.
void updateCursorTracksMouse(void)
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (platform->m_mouseInside)
    CGAssociateMouseAndMouseCursorPosition(!platform->m_mouseRelative);
}

@implementation MgOpenGLView


//--------------------------------------------------------------------
// return openGLContext
- (NSOpenGLContext*) openGLContext
{
  return openGLContext;
}

//--------------------------------------------------------------------
// return pixelFormat
- (NSOpenGLPixelFormat*) pixelFormat
{
  return pixelFormat;
}

//--------------------------------------------------------------------
// initialize view area.  Create OpenGL context
- (id) initWithFrame:(NSRect)frame
{
  NSOpenGLPixelFormatAttribute attribs_3x2[] =
  {
    NSOpenGLPFAAccelerated,
		NSOpenGLPFANoRecovery,
//    NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFADepthSize, 16,
    NSOpenGLPFAStencilSize, 1,
    NSOpenGLPFAOpenGLProfile,
		NSOpenGLProfileVersion3_2Core,
    NSOpenGLPFAMaximumPolicy,
		0
  };


  NSOpenGLPixelFormatAttribute attribs_2x1[] =
  {
		kCGLPFAAccelerated,
		kCGLPFANoRecovery,
//		kCGLPFADoubleBuffer,
		kCGLPFAColorSize, 24,
		kCGLPFADepthSize, 16,
    NSOpenGLPFAStencilSize, 1,
    NSOpenGLPFAMaximumPolicy,
		0
  };
	
  pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs_3x2];
  if (pixelFormat != NULL)
    mgGLVersionFound = "3.2";
  else 
  {
    mgDebug("NSOpenGLPixelFormat initWithAttributes 3.2 returns null");
    pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs_2x1];
    if (pixelFormat != NULL)
      mgGLVersionFound = "2.1";
    else
    {
      mgDebug("NSOpenGLPixelFormat initWithAttributes 2.1 returns null");
      // =-= issue alert and die.
    }
  }

  openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat
                   shareContext:nil];
                   
  [[self openGLContext] makeCurrentContext];
  
  self = [super initWithFrame:frame];
  if (self == nil)
  {
    mgDebug("super initWithFrame failed.");
    return self;
  }

  // notify us of size changes
  [[NSNotificationCenter defaultCenter] addObserver:self 
    selector:@selector(reshape)
    name:NSViewGlobalFrameDidChangeNotification
    object: self];

  trackingArea = nil;
  
  // create display link to handle refresh
  displayLink = [DisplayLink alloc];
  [displayLink setupDisplayLink: self context:[self openGLContext] pixelFormat:[self pixelFormat]];

  return self;
}

//--------------------------------------------------------------------
// awake from interface builder.  create framework instance
- (void) awakeFromNib
{
  // write some debug info from Cocoa
  mgDebug(":Machine CPU count: %d", (int) [NSProcessInfo processInfo].processorCount);
  NSString* version = [NSProcessInfo processInfo].operatingSystemVersionString;
  mgDebug(":OS OSX Version: %s", [version cStringUsingEncoding:NSUTF8StringEncoding]);

  mgOSXServices* platform = NULL;
  try
  {
    [displayLink lock];
    
    // initialize platform services
    mgOSXServices* platform = new mgOSXServices();

    // create the application
    platform->m_theApp = mgCreateApplication();

    // tell application to set up display
    platform->m_theApp->appRequestDisplay();

    // initialize the display 
    platform->initDisplay();

    // initialize the application
    platform->m_theApp->appInit();
    platform->m_theApp->appViewResized(platform->m_windowWidth, platform->m_windowHeight);

    // create buffers
    platform->m_theApp->appCreateBuffers();

    platform->initView();

    platform->m_active = true;
    platform->m_initialized = true;

    // set swap interval
    mgDebug("graphicsTiming = %s", platform->m_swapImmediate?"true":"false");
    GLint swapInt = platform->m_swapImmediate ? 0 : 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    mgString windowTitle;
    platform->getWindowTitle(windowTitle);
    NSString* title = [[NSString alloc] initWithUTF8String: windowTitle];
    [[self window] setTitle:title];
    [title release];

    [displayLink unlock];
  }
  catch (mgErrorMsg* e)
  {
    mgString text;
    platform->getErrorTable()->msgText(text, e);
    mgDebug("%s", (const char*) text);

    NSString* msg = [[[NSString alloc] initWithUTF8String:(const char*) text] autorelease];
    NSAlert* alert = [NSAlert alertWithMessageText:msg
                                     defaultButton:@"OK" alternateButton:nil otherButton:nil
                         informativeTextWithFormat:msg];
    [alert runModal];
    delete platform;
    mgPlatform = platform = NULL;
  }
  catch (mgException* e)
  {
    mgDebug("exception: %s", (const char*) e->m_message);
    
    NSString* msg = [[[NSString alloc] initWithUTF8String:(const char*) e->m_message] autorelease];
    NSAlert* alert = [NSAlert alertWithMessageText:msg
                                     defaultButton:@"OK" alternateButton:nil otherButton:nil
                         informativeTextWithFormat:msg];
    [alert runModal];
    delete platform;
    mgPlatform = platform = NULL;
  }
}

- (void) removeFromSuperview
{
  mgDebug("removeFromSuperview");
}

//--------------------------------------------------------------------
// remove the cursor tracking area
- (void) removeTrackingArea
{
  // remove any old tracking area
  if (trackingArea != nil)
  {
    [self removeTrackingArea:trackingArea];
    [trackingArea release];
    trackingArea = nil;
  }
}
  
//--------------------------------------------------------------------
// set the cursor tracking area
- (void) createTrackingArea
{
  [self removeTrackingArea];
  
  // create new tracking area
  trackingArea = [NSTrackingArea alloc];
  [trackingArea
   initWithRect:[self bounds]
   options: 
   (
    NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | // NSTrackingCursorUpdate |
    NSTrackingActiveWhenFirstResponder // | NSTrackingInVisibleRect | NSTrackingAssumeInside
    )
   owner:self userInfo:nil];
  
  [self addTrackingArea:trackingArea];
}

//--------------------------------------------------------------------
// update cursor tracking area after view resize
- (void) updateTrackingAreas
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;
  [super updateTrackingAreas];
  if (platform != NULL && platform->m_mouseInside)  
    [self createTrackingArea];
}

//--------------------------------------------------------------------
// lock focus to the view window
- (void) lockFocus
{
  [super lockFocus];
  if ([[self openGLContext] view] != self)
    [[self openGLContext] setView:self]; 
}

//--------------------------------------------------------------------
// connect to display and initialize framework
- (void) prepareOpenGL
{
  [displayLink lock];

  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  // =-= can this be done in awakeFromNIB?
  try
  {
    GLint depth = 16;
    [pixelFormat getValues:&depth forAttribute:NSOpenGLPFADepthSize forVirtualScreen:0];
    platform->setDepthBits(depth);
    [pixelFormat getValues:&depth forAttribute:NSOpenGLPFAStencilSize forVirtualScreen:0];
    mgDebug("stencil size = %d", depth);
  }
  catch (mgException* e)
  {
    mgDebug("exception: %s", (const char*) e->m_message);
   
    NSString* msg = [[[NSString alloc] initWithUTF8String:(const char*) e->m_message] autorelease];
    NSAlert* alert = [NSAlert alertWithMessageText:msg
                      defaultButton:@"OK" alternateButton:nil otherButton:nil
                      informativeTextWithFormat:msg];
    [alert runModal];
    delete platform;
    mgPlatform = platform = NULL;
  }

  [displayLink unlock];
}

//--------------------------------------------------------------------
// view is opaque
- (BOOL) isOpaque
{
  return YES;
}

//--------------------------------------------------------------------
// view has resized
- (void) reshape
{
  [displayLink lock];

  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  NSRect rect = [self bounds];
  if (platform != NULL)
    platform->displayResized((int) rect.size.width, (int) rect.size.height);
	[[self openGLContext] update];

  [displayLink unlock];
}

//--------------------------------------------------------------------
// redraw view
- (void) drawRect:(NSRect)dirty
{
  // ignore if the display link is still running
  if (![displayLink isRunning])
    [self drawView];
}

//--------------------------------------------------------------------
// draw view contents
- (void) drawView
{
  [displayLink lock];

  [[self openGLContext] makeCurrentContext];

  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (platform != NULL)
    platform->m_theApp->appIdle();

  [displayLink unlock];
}

//--------------------------------------------------------------------
// become first reponder to keys, etc.
- (BOOL) acceptsFirstResponder
{
  [[self window] makeFirstResponder:self];
  return YES;
}

//--------------------------------------------------------------------
// start our own cursor tracking
- (void) takeCursor:(NSEvent*)theEvent
{
  // the Cocoa framework doesn't seem to be consistent about mouse handling.
  // we don't get acceptsFirstMouse, mouseEntered or updateCursor unless 
  // the app starts with the mouse outside the view.  So call this method
  // whenever we see any mouse event when we don't think we have the cursor
  mgDebug("takeCursor");

  [self mouseEntered:theEvent];
  [self createTrackingArea];
}

//--------------------------------------------------------------------
// release cursor 
- (void) releaseCursor
{
  CGAssociateMouseAndMouseCursorPosition(true);
  [self removeTrackingArea];
  [self mouseExited:nil];
}

//--------------------------------------------------------------------
// accept first mouse down 
- (BOOL) acceptsFirstMouse:(NSEvent *)theEvent
{
  mgDebug("acceptsFirstMouse %d", (int) [theEvent type]);
  
  return YES;
}

//--------------------------------------------------------------------
// mouse has entered the view
- (void) mouseEntered:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  mgDebug("mouseEntered");

  [displayLink lock];

  if (platform->m_mouseInside)
    platform->mouseExited();

  NSRect rect = [self bounds];
  
  NSPoint loc = [theEvent locationInWindow];
  NSPoint pt = [self convertPoint:loc fromView:nil];  
  int x = pt.x;
  int y = rect.size.height - pt.y;
  platform->mouseEntered(x, y);

  [displayLink unlock];

  [NSCursor hide];
  updateCursorTracksMouse();
}

//--------------------------------------------------------------------
// mouse has exited the view
- (void) mouseExited:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (platform != NULL)
  {
    [displayLink lock];

    mgDebug("mouseExited");
    if (platform->m_mouseInside)
      platform->mouseExited();

    [displayLink unlock];
  }
  
  updateCursorTracksMouse();
  [NSCursor unhide];
}

//--------------------------------------------------------------------
// mouse left button press
- (void) mouseDown:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (platform == NULL)
    return;
  if (!platform->m_mouseInside)
    [self takeCursor:theEvent];

  mgDebug("mouseDown");

  [displayLink lock];
  platform->mouseDown(MG_EVENT_MOUSE1_DOWN);  // left button
  [displayLink unlock];
}

//--------------------------------------------------------------------
// mouse left button release
- (void) mouseUp:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (mgPlatform == NULL)
    return;
  mgDebug("mouseUp");
  [displayLink lock];
  platform->mouseUp(MG_EVENT_MOUSE1_DOWN);  // left button
  [displayLink unlock];
}

//--------------------------------------------------------------------
// mouse right button press
- (void) rightMouseDown:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (platform == NULL)
    return;
  if (!platform->m_mouseInside)
    [self takeCursor:theEvent];

  mgDebug("right mouse down");
  [displayLink lock];
  platform->mouseDown(MG_EVENT_MOUSE2_DOWN);  // right button
  [displayLink unlock];
}

//--------------------------------------------------------------------
// mouse right button release
- (void) rightMouseUp:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (platform == NULL)
    return;
  mgDebug("right mouse up");
  [displayLink lock];
  platform->mouseUp(MG_EVENT_MOUSE2_DOWN);  // right button
  [displayLink unlock];
}

//--------------------------------------------------------------------
// mouse move with left button down
- (void) mouseDragged:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (platform == NULL)
    return;
  NSRect rect = [self bounds];
  
  NSPoint loc = [theEvent locationInWindow];
  NSPoint pt = [self convertPoint:loc fromView:nil];  
  int x = pt.x;
  int y = rect.size.height - pt.y;

  [displayLink lock];
  platform->mouseDrag(x, y, theEvent.deltaX, theEvent.deltaY);
  [displayLink unlock];
}

//--------------------------------------------------------------------
// mouse move with buttons up
- (void) mouseMoved:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;
  if (platform == NULL)
    return;
  if (!platform->m_mouseInside)
    return;
    
  NSRect rect = [self bounds];

  NSPoint loc = [theEvent locationInWindow];
  NSPoint pt = [self convertPoint:loc fromView:nil];  
  int x = pt.x;
  int y = rect.size.height - pt.y;
  [displayLink lock];
  platform->mouseMove(x, y, theEvent.deltaX, theEvent.deltaY);
  [displayLink unlock];
}

//--------------------------------------------------------------------
// mouse move with right button down
- (void) rightMouseDragged:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;

  if (platform == NULL)
    return;
  NSRect rect = [self bounds];
  
  NSPoint loc = [theEvent locationInWindow];
  NSPoint pt = [self convertPoint:loc fromView:nil];  
  int x = pt.x;
  int y = rect.size.height - pt.y;
  [displayLink lock];
  platform->mouseDrag(x, y, theEvent.deltaX, theEvent.deltaY);
  [displayLink unlock];
}

//--------------------------------------------------------------------
// mouse scroll wheel turned
- (void) scrollWheel:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;
  if (platform == NULL)
    return;
  mgDebug("wheel (%d, %d, %d) (%d, %d)", theEvent.deltaX, theEvent.deltaY, theEvent.deltaZ,
    theEvent.data1, theEvent.data2);
  [displayLink lock];
  platform->mouseWheel(theEvent.deltaZ);
  [displayLink unlock];
}

//--------------------------------------------------------------------
// translate mac keycodes for special characters
- (int) translateKey:(int)keyCode
{
  switch (keyCode)
  {
    case NSF1FunctionKey: return MG_EVENT_KEY_F1;
    case NSF2FunctionKey: return MG_EVENT_KEY_F2;
    case NSF3FunctionKey: return MG_EVENT_KEY_F3;
    case NSF4FunctionKey: return MG_EVENT_KEY_F4;
    case NSF5FunctionKey: return MG_EVENT_KEY_F5;
    case NSF6FunctionKey: return MG_EVENT_KEY_F6;
    case NSF7FunctionKey: return MG_EVENT_KEY_F7;
    case NSF8FunctionKey: return MG_EVENT_KEY_F8;
    case NSF9FunctionKey: return MG_EVENT_KEY_F9;
    case NSF10FunctionKey: return MG_EVENT_KEY_F10;
    case NSF11FunctionKey: return MG_EVENT_KEY_F11;
    case NSF12FunctionKey: return MG_EVENT_KEY_F12;
      
    case NSUpArrowFunctionKey: return MG_EVENT_KEY_UP;
    case NSDownArrowFunctionKey: return MG_EVENT_KEY_DOWN;
    case NSLeftArrowFunctionKey: return MG_EVENT_KEY_LEFT;
    case NSRightArrowFunctionKey: return MG_EVENT_KEY_RIGHT;
      
    case NSInsertFunctionKey: return MG_EVENT_KEY_INSERT;
    case NSDeleteFunctionKey: return MG_EVENT_KEY_DELETE;
    case NSHomeFunctionKey: return MG_EVENT_KEY_HOME;
    case NSEndFunctionKey: return MG_EVENT_KEY_END;
    case NSPageUpFunctionKey: return MG_EVENT_KEY_PAGEUP;
    case NSPageDownFunctionKey: return MG_EVENT_KEY_PAGEDN;
      
    case 0x1b: return MG_EVENT_KEY_ESCAPE;
    case 0x0D: return MG_EVENT_KEY_RETURN;
    case 0x7F: return MG_EVENT_KEY_BACKSPACE;
    case 0x08: return MG_EVENT_KEY_TAB;
    default: mgDebug("unknown key %02x", keyCode);
  }
  return 0;
}

//--------------------------------------------------------------------
// translate keyboard modifiers to framework flags
- (int) translateModifiers:(NSUInteger)modifiers
{
  int flags = 0;
  if (modifiers & NSShiftKeyMask)
    flags |= MG_EVENT_SHIFT_DOWN;
  if (modifiers & NSControlKeyMask)
    flags |= MG_EVENT_CNTL_DOWN;
  if (modifiers & NSCommandKeyMask)
    flags |= MG_EVENT_META_DOWN;
  
  return flags;  
}

//--------------------------------------------------------------------
// key pressed
- (void) keyDown:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;
  if (platform == NULL)
    return;
  [displayLink lock];

  int flags = [self translateModifiers: [theEvent modifierFlags]];
  if ([theEvent isARepeat])
    flags |= MG_EVENT_KEYREPEAT;
  
  unichar c = [[theEvent characters] characterAtIndex: 0];
  int specialKey = [self translateKey:c];
//  if (specialKey == MG_EVENT_KEY_ESCAPE)
//  {
//    [self releaseCursor];
//  }
//  else 
  platform->keyDown(specialKey, flags, (int) c);

  [displayLink unlock];
}

//--------------------------------------------------------------------
// key released
- (void) keyUp:(NSEvent *)theEvent
{
  mgOSXServices* platform = (mgOSXServices*) mgPlatform;
  if (platform == NULL)
    return;

  [displayLink lock];
  int flags = [self translateModifiers: [theEvent modifierFlags]];
  unichar c = [[theEvent characters] characterAtIndex: 0];
  int specialKey = [self translateKey:c];
  platform->keyUp(specialKey, flags, (int) c);
  [displayLink unlock];
}

//--------------------------------------------------------------------
// view destructor
- (void) dealloc
{
  [displayLink release];
  
  [openGLContext release];
  [pixelFormat release];
  
  [[NSNotificationCenter defaultCenter] removeObserver:self
    name:NSViewGlobalFrameDidChangeNotification object:self];
  [super dealloc];
}

@end
