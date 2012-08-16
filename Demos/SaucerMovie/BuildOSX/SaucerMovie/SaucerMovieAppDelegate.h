//
//  SaucerMovieAppDelegate.h
//  SaucerMovie
//
//  Created by Michael Goodfellow on 3/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface SaucerMovieAppDelegate : NSObject <NSApplicationDelegate> {
  NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end
