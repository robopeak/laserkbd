/*
 *  Common Includes for MacOS
 *  By Shikai Chen (csk@live.com)
 */

#pragma once

#include <sys/stat.h>
#include <sys/time.h>



#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#undef check
#undef MIN

static inline unsigned long getms() {
    timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec*1000 + now.tv_usec/1000;
}

static inline void gotoURL(const char * url) {
    CFURLRef urlref = CFURLCreateWithBytes (
                                         NULL,                        // allocator
                                         (UInt8*)url,     // URLBytes
                                         strlen(url),            // length
                                         kCFStringEncodingASCII,      // encoding
                                         NULL                         // baseURL
                                         );
    LSOpenCFURLRef(urlref,0);
    CFRelease(urlref);

}

extern char PLATFORM_RES_PREFIX[1024];
extern char PLATFORM_CONFIG_PREFIX[1024];
extern bool _macos_quit_flag;

static inline const char * getPlatformResPrefix() {
    return PLATFORM_RES_PREFIX;
}

static inline const char * getPlatformConfigPrefix() {
    return PLATFORM_CONFIG_PREFIX;
}

static inline bool checkPlatformExitFlag() {
    return _macos_quit_flag;
}

#ifndef _countof
#define _countof(x)    (sizeof(x)/sizeof(x[0]))
#endif

// Virtual Key defines
#define VK_MULTIPLY             '*'
#define VK_OEM_MINUS            '-'
#define VK_ADD                  '+'
#define VK_OEM_PLUS             '='
#define VK_ESCAPE               27
#define VK_BACK                 127
#define VK_CAPITAL              2 //non-spec
#define VK_TAB                  '\t'
#define VK_OEM_1                ';'
#define VK_RETURN               13
#define VK_LSHIFT               4 //non-spec
#define VK_OEM_3                '`'
#define VK_OEM_5                '\\'
#define VK_OEM_PERIOD           '.'
#define VK_OEM_2                '/'
#define VK_UP                   5 //non-spec
#define VK_RSHIFT               6 //non-spec
#define VK_CONTROL              7 //non-spec
#define VK_MENU                 8    //non-spec , alt
#define VK_OEM_4                '['
#define VK_OEM_6                ']'
#define VK_SPACE                ' '
#define VK_DELETE               18 //non-spec
#define VK_LEFT                 10 //non-spec
#define VK_DOWN                 11 //non-spec
#define VK_RIGHT                12 //non-spec
#define VK_OEM_7                '\'' 
#define VK_OEM_COMMA            ','