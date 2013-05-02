/*
 *  Laser Keyboard
 *  Product Identification Supporting Lib
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#include "common.h"
#include "port/common/productid.h"


#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>

#include <locale.h>

const char * GetPlatformName()
{
    return "macOS";
}

static const unsigned char xor_keyring[6] = {0x02, 0x4D, 0x8E, 0x1C, 0x48, 0x3F,};

// Get the first network mac as the platform id, xor it for privacy protection
bool GetPlatformIdentification(platform_id_t & id)
{
  
    FILE * peer_fd = popen("ifconfig -L", "r");

    if (!peer_fd) return false;


    bool ans = false;
    char linebuffer[2048];



    while (fgets(linebuffer, sizeof(linebuffer), peer_fd)) {
        std::vector<std::string> dest;
        split(linebuffer, dest);
        if (dest.size() < 2) continue;
        
        if (dest[0].compare("\tether")==0) {
            int tmpbuf[6];
            dest[1]+="\n";
            sscanf(dest[1].c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
                   &tmpbuf[0],
                   &tmpbuf[1],
                   &tmpbuf[2],
                   &tmpbuf[3],
                   &tmpbuf[4],
                   &tmpbuf[5]);
            
            for (int i=0; i<6; ++i) {
                id.id_buf[i] = (unsigned char)tmpbuf[i] ^ xor_keyring[i];
            }
            ans = true;
            goto _final;
        }
        
    }

_final:

    pclose(peer_fd);

    return ans;
}

bool IsTargetProductPresent()
{

        
    
    CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
    CFNumberRef numberRef;

    
    long pid, vid, rev;
    pid = release_usb_pid;
    vid = release_usb_vid;
    rev = release_usb_hw_rev;
    
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vid);
    CFDictionarySetValue( matchingDict, CFSTR(kUSBVendorID), numberRef );
    CFRelease(numberRef);
    
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pid);
    CFDictionarySetValue( matchingDict, CFSTR(kUSBProductID), numberRef );
    CFRelease(numberRef);
    
    
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &rev);
    CFDictionarySetValue( matchingDict, CFSTR(kUSBDeviceReleaseNumber), numberRef );
    CFRelease(numberRef);
    
    io_service_t camera = IOServiceGetMatchingService( kIOMasterPortDefault, matchingDict );
    if (camera) return true;
    else return false;
}
