//
//  DontHitMeAppDelegate.h
//  DontHitMe
//
//  Created by Michael Goodfellow on 3/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DontHitMeAppDelegate : NSObject <NSApplicationDelegate> {
  NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end
