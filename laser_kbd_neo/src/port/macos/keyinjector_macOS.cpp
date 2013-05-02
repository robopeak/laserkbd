/*
 *  Keyboard event injector for Win32
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "port/common/keyinjector.h"
#include <ApplicationServices/ApplicationServices.h>

#define KEY_FN 0x92

static CGKeyCode unifiedKeyCodeToCGKeyCode(int unifiedkey) {

    switch (unifiedkey) {
    case  'A': return 0;
    case  'S': return 1;
    case  'D': return 2;
    case  'F': return 3;
    case  'H': return 4;
    case  'G': return 5;
    case  'Z': return 6;
    case  'X': return 7;
    case  'C': return 8;
    case  'V': return 9;
    // what is 10?
    case  'B': return 11;
    case  'Q': return 12;
    case  'W': return 13;
    case  'E': return 14;
    case  'R': return 15;
    case  'Y': return 16;
    case  'T': return 17;
    case  '1': return 18;
    case  '2': return 19;
    case  '3': return 20;
    case  '4': return 21;
    case  '6': return 22;
    case  '5': return 23;
    case  VK_OEM_PLUS: return 24;
    case  '9': return 25;
    case  '7': return 26;
    case  VK_OEM_MINUS: return 27;
    case  '8': return 28;
    case  '0': return 29;
    case  VK_OEM_6: return 30;
    case  'O': return 31;
    case  'U': return 32;
    case  VK_OEM_4: return 33;
    case  'I': return 34;
    case  'P': return 35;
   // case  VK_RETURN: return 36;
    case  'L': return 37;
    case  'J': return 38;
    case  VK_OEM_7: return 39;
    case  'K': return 40;
    case  VK_OEM_1: return 41;
    case  VK_OEM_5: return 42;
    case  ',': return 43;
    case  VK_OEM_2: return 44;
    case  'N': return 45;
    case  'M': return 46;
    case  VK_OEM_PERIOD: return 47;
    case  VK_TAB: return 48;
    case  VK_SPACE: return 49;
    case  VK_OEM_3: return 50;
    case  VK_DELETE: return 117;
    case  VK_RETURN: return 52;
    case  VK_ESCAPE: return 53;
    
    // some more missing codes abound, reserved I presume, but it would
    // have been helpful for Apple to have a document with them all listed
    
    case  VK_LEFT: return 123;
    case  VK_RIGHT: return 124;
    case  VK_DOWN: return 125;
    case  VK_UP: return 126;
    
            
    case VK_BACK: return 51;
    case VK_CAPITAL: return 0x39;
    }
    
    return -1;
}

static unsigned int getMaskFlagByKey(int key) {
    switch (key) {
        case VK_LSHIFT:
        case VK_RSHIFT:
            return kCGEventFlagMaskShift;
        case KEY_FN:
            return kCGEventFlagMaskCommand;
        case VK_CONTROL:
            return kCGEventFlagMaskControl;
        case VK_MENU:
            return kCGEventFlagMaskAlternate;
    }
    return 0;
}


static bool isMaskKey(int key) {
    return getMaskFlagByKey(key)!=0;
}


class OSKeyInjector_MacOS : public OSKeyInjector
{
public :
    OSKeyInjector_MacOS()
    : maskkey_flag(0){


    }

    virtual ~OSKeyInjector_MacOS() {

    }


    virtual bool injectKeyEvents( const std::vector<KeyEventDesc> & inputlist)
    {
        if (!inputlist.size()) return false;
        
        for (int pos = 0; pos < inputlist.size(); ++pos) {
            
            // as CGEventPost doesn't allow us to inject the Caps lock event,
            // we have to workaround it...
            if (inputlist[pos].keyval == VK_CAPITAL) {
                if (inputlist[pos].type==KEY_EVENT_PRESSED) {
                    if (maskkey_flag & kCGEventFlagMaskAlphaShift) {
                        maskkey_flag &= ~kCGEventFlagMaskAlphaShift;
                    } else {
                        maskkey_flag |= kCGEventFlagMaskAlphaShift;
                    }
                }
                continue;
            }
            
            if (isMaskKey(inputlist[pos].keyval)) {
                unsigned int maskflg = getMaskFlagByKey(inputlist[pos].keyval);
                if (inputlist[pos].type==KEY_EVENT_PRESSED) {
                    maskkey_flag |= maskflg;
                } else {
                    maskkey_flag &= ~(maskflg);
                }
                continue;
            }
            
            CGEventRef eventRef;
            CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
            CGKeyCode keycode = unifiedKeyCodeToCGKeyCode(inputlist[pos].keyval);
            
            if (keycode == -1) continue;
            
            eventRef = CGEventCreateKeyboardEvent(source, keycode, inputlist[pos].type==KEY_EVENT_PRESSED?true:false);
            CGEventSetFlags(eventRef, maskkey_flag);
            
            
            //Apparently, a bug in xcode requires this next line
            CGEventSetType(eventRef, inputlist[pos].type==KEY_EVENT_PRESSED?kCGEventKeyDown:kCGEventKeyUp);
            CGEventPost(kCGHIDEventTap, eventRef);
            CFRelease(eventRef);
            CFRelease(source);

        }
        return true;
    }

protected:
    unsigned long maskkey_flag;
};




//---

OSKeyInjector* OSKeyInjector::g_inst = NULL;

OSKeyInjector * OSKeyInjector::GetInstance() {
    // FIXME: race condition will occur when this function is used in 
    //        multi-thread envirnoment

    if (OSKeyInjector::g_inst) {
        return OSKeyInjector::g_inst;
    }

    OSKeyInjector::g_inst = new OSKeyInjector_MacOS();
    return OSKeyInjector::g_inst;
}


void OSKeyInjector::ReleaseInstance()
{
    // FIXME: race condition will occur when this function is used in 
    //        multi-thread envirnoment

    if (OSKeyInjector::g_inst) {
        delete OSKeyInjector::g_inst;
        OSKeyInjector::g_inst = NULL;
    }
}