/*
 *  Laser Keyboard
 *  Platform language state supporting lib
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#include "common.h"
#include "port/common/platform_language.h"




const char * GetBestMatchedLanguageString()
{
    CFArrayRef langid_arr = CFLocaleCopyPreferredLanguages();
    if (langid_arr) {
        
        CFStringRef  firstPreferred = (CFStringRef)CFArrayGetValueAtIndex(langid_arr, 0);
        char stringbuf[100];
        if (CFStringGetCString(firstPreferred, stringbuf, 100, kCFStringEncodingASCII)) {
            if (strncmp(stringbuf, "zh-", strlen("zh-"))==0) {
                    return "cn";
            }
            
        }
        CFRelease(langid_arr);
    }
    return "en";

}