//
//  GuiTestAllAppDelegate.h
//  GuiTestAll
//
//  Created by Michael Goodfellow on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface GuiTestAllAppDelegate : NSObject <NSApplicationDelegate> {
  NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end
