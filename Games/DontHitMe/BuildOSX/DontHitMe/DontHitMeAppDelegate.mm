//
//  DontHitMeAppDelegate.m
//  DontHitMe
//
//  Created by Michael Goodfellow on 3/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "DontHitMeAppDelegate.h"

@implementation DontHitMeAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  // Insert code here to initialize your application
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
  return YES;
}

@end
