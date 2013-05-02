/*
 *  Common Includes for Windows
 *  By Shikai Chen (csk@live.com)
 */

#pragma once

#pragma warning (disable: 4996)
#pragma warning (disable: 4305)
#define _CRT_SECURE_NO_WARNINGS


#include <direct.h>
#include <windows.h>

static inline int mkdir(const char * path, int mode){
    return _mkdir(path); //ignore the mode flags
}

static inline unsigned long getms() {
    return timeGetTime();
}

static inline void gotoURL(const char * url) {
    ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);

}

static inline const char * getPlatformResPrefix() {
    return "";
}

static inline const char * getPlatformConfigPrefix() {
    return "";
}

static inline bool checkPlatformExitFlag() {
    return false;
}
