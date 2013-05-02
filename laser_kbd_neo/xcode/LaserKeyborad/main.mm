//
//  main.m
//  LaserKeyborad MacOS Entry
//
//  Created by Shikai Chen on 13-3-27.
//  Copyright (c) 2013 RoboPeak. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <unistd.h>
#include <sys/stat.h>

char PLATFORM_RES_PREFIX[1024];
char PLATFORM_CONFIG_PREFIX[1024];
bool _macos_quit_flag = false;
extern int cpp_entry(int argc, char * argv[]);


@interface AppEventListener : NSObject
{
    
}

//handler for the quit apple event
- (void)handleQuitEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent;

- (void)registerQuitListener;

- (void)unregisterQuitListener;

@end

@implementation AppEventListener

- (id)init {
    [super init];
    
    return self;
}




- (void)deadlloc {
    
    [super dealloc];
}

- (void)registerQuitListener
{
    //install the custom quit event handler
    NSAppleEventManager* appleEventManager = [NSAppleEventManager sharedAppleEventManager];
    [appleEventManager setEventHandler:self andSelector:@selector(handleQuitEvent:withReplyEvent:) forEventClass:kCoreEventClass andEventID:kAEQuitApplication];

}

- (void)unregisterQuitListener
{
    NSAppleEventManager* appleEventManager = [NSAppleEventManager sharedAppleEventManager];
    [appleEventManager removeEventHandlerForEventClass:kCoreEventClass andEventID:kAEQuitApplication];
}

- (void)handleQuitEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent {
    _macos_quit_flag = true;
}

@end


int main(int argc, char *argv[])
{
    
    AppEventListener * listener = [[AppEventListener alloc] init];
    [listener registerQuitListener];
    
    NSBundle * bundle = [NSBundle mainBundle];
    NSString * bundlepath = [bundle bundlePath];

    sprintf(PLATFORM_RES_PREFIX, "%s/Contents/Resources/", [bundlepath UTF8String]);

    
    NSArray * docPaths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    
    if (!docPaths || [docPaths count] == 0) {
        // not found? use user home instead
        sprintf(PLATFORM_CONFIG_PREFIX, "%s/.rp_laserkbd/", getenv("HOME"));
    } else {
        NSString * dir = [docPaths objectAtIndex:0];
        sprintf(PLATFORM_CONFIG_PREFIX, "%s/rp_laserkbd/", [dir UTF8String]);
        mkdir(PLATFORM_CONFIG_PREFIX, 0755);
    }
    
    for (int pos = 0; pos < argc; ++pos ){
        printf("%d: %s\n", pos, argv[pos]);
        
    }
    
    int ans= cpp_entry(1,argv);
    
    [listener unregisterQuitListener];
    [listener release];
    
    return ans;
}
