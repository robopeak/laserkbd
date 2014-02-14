/*
 * Common Includes for Linux Deepin
 * Copyright (C) 2013 Deepin, Inc.
 *                    Leslie Zhai <zhaixiang@linuxdeepin.com>
 */

#include <stdio.h>
#include <sys/time.h>

static inline unsigned long getms() 
{
    timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
}

static inline void gotoURL(const char* url) 
{
}

static inline const char* getPlatformResPrefix() 
{
    return "./";
}

static inline const char* getPlatformConfigPrefix() 
{
    return "./";
}

static inline bool checkPlatformExitFlag() 
{
    return false;
}

#ifndef _countof
#define _countof(x)    (sizeof(x) / sizeof(x[0]))
#endif

// Virtual Key defines
#define VK_MULTIPLY             '*'
#define VK_OEM_MINUS            '-'
#define VK_ADD                  '+'
#define VK_OEM_PLUS             '='
/* TODO it is based on /usr/share/X11/xkb/keycodes/evdev */
#define VK_ESCAPE               9
#define VK_BACK                 22
#define VK_CAPITAL              66 //non-spec
#define VK_TAB                  '\t'
#define VK_OEM_1                ';'
#define VK_RETURN               36
#define VK_LSHIFT               50 //non-spec
#define VK_OEM_3                '`'
#define VK_OEM_5                '\\'
#define VK_OEM_PERIOD           '.'
#define VK_OEM_2                '/'
#define VK_UP                   111 //non-spec
#define VK_RSHIFT               62 //non-spec
#define VK_CONTROL              37 //non-spec
#define VK_MENU                 135    //non-spec , alt
#define VK_OEM_4                '['
#define VK_OEM_6                ']'
#define VK_SPACE                ' '
#define VK_DELETE               119 //non-spec
#define VK_LEFT                 113 //non-spec
#define VK_DOWN                 116 //non-spec
#define VK_RIGHT                114 //non-spec
#define VK_OEM_7                '\'' 
#define VK_OEM_COMMA            ','
