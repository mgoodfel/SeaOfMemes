/*
 Based on code from the GLFullscreen sample program, apple developer site. 
 */
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

#import "mgDisplayLink.h"
#import "MgOpenGLView.h"

@implementation DisplayLink

// This is the renderer output callback function
static CVReturn MgDisplayLinkCallback(
  CVDisplayLinkRef displayLink, 
  const CVTimeStamp* now, 
  const CVTimeStamp* outputTime, 
  CVOptionFlags flagsIn, 
  CVOptionFlags* flagsOut, 
  void* displayLinkContext)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  [(MgOpenGLView*)displayLinkContext drawView];

	[pool release];
  return kCVReturnSuccess;
}

- (void) setupDisplayLink:(MgOpenGLView*)view 
  context:(NSOpenGLContext*)context
  pixelFormat:(NSOpenGLPixelFormat*)format
{
  openGLContext = context;
  pixelFormat = format;
  
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	
	// Set the renderer output callback function
	CVDisplayLinkSetOutputCallback(displayLink, &MgDisplayLinkCallback, view);
	
	// Set the display link for the current renderer
	CGLContextObj cglContext = [openGLContext CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = [pixelFormat CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

  CVDisplayLinkStart(displayLink);
}

- (BOOL) isRunning
{
  return CVDisplayLinkIsRunning(displayLink);
}

- (void) lock
{
  CGLLockContext([openGLContext CGLContextObj]);
}

- (void) unlock
{
  CGLUnlockContext([openGLContext CGLContextObj]);
}

- (void) dealloc
{
  // top and release the display link
  CVDisplayLinkStop(displayLink);
  CVDisplayLinkRelease(displayLink);
  
  [super dealloc];
}


@end
