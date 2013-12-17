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
/* FIXME: what about Linux Virtual Keysym? */
#define VK_ESCAPE               65307
#define VK_BACK                 65288
#define VK_CAPITAL              2 //non-spec
#define VK_TAB                  '\t'
#define VK_OEM_1                ';'
#define VK_RETURN               65293
#define VK_LSHIFT               65505 //non-spec
#define VK_OEM_3                '`'
#define VK_OEM_5                '\\'
#define VK_OEM_PERIOD           '.'
#define VK_OEM_2                '/'
#define VK_UP                   65362 //non-spec
#define VK_RSHIFT               65506 //non-spec
#define VK_CONTROL              65507 //non-spec
#define VK_MENU                 65383    //non-spec , alt
#define VK_OEM_4                '['
#define VK_OEM_6                ']'
#define VK_SPACE                ' '
#define VK_DELETE               65535 //non-spec
#define VK_LEFT                 65361 //non-spec
#define VK_DOWN                 65364 //non-spec
#define VK_RIGHT                65363 //non-spec
#define VK_OEM_7                '\'' 
#define VK_OEM_COMMA            ','
