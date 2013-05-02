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
 //   LCID localeid = GetSystemDefaultLCID();
    LANGID langid = GetUserDefaultUILanguage ();
    switch (langid & 0xF) {
    case 0x04: // zh related
        return "cn";
    default :
        return "en";
    }

}