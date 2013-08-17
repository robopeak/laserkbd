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
#include "libusb.h"

#include <locale.h>

const char * GetPlatformName()
{
#ifdef _WIN64
    return "win64";
#else
    return "win32";
#endif
}

static const unsigned char xor_keyring[6] = {0x02, 0x4D, 0x8E, 0x1C, 0x48, 0x3F,};

// Get the first network mac as the platform id, xor it for privacy protection
bool GetPlatformIdentification(platform_id_t & id)
{
  
    STARTUPINFO si; 

    FILE * peer_fd = _popen("chcp 437 && ipconfig /all", "r");

    if (!peer_fd) return false;


    bool ans = false;
    char linebuffer[2048];

#define FINDMSK_WIRELESS 0x1
#define FINDMSK_ETHERNET 0x2

#define PREFIX_CMP(_src_, _prefix_) \
    strncmp(_src_, _prefix_, strlen(_prefix_))

    unsigned int currentAdaptor = 0;

    while (fgets(linebuffer, sizeof(linebuffer), peer_fd)) {
        if (PREFIX_CMP(linebuffer, "Wireless LAN adapter")==0) {
            currentAdaptor = FINDMSK_WIRELESS;

        } else if (PREFIX_CMP(linebuffer, "Ethernet adapter")==0) {
            currentAdaptor = FINDMSK_ETHERNET;

        } else if (PREFIX_CMP(linebuffer, "   Physical Address")==0) {
            if (currentAdaptor) {
                // the first mac addr for wireless or ethernet card has been found, store it
                for (int pos = strlen("   Physical Address"); pos < strlen(linebuffer); ++pos) {
                    if (linebuffer[pos] == ':') {
                        while (linebuffer[++pos]==' ' && linebuffer[pos]!='\0');
                        int tmpbuf[6];
                        sscanf(linebuffer+pos, "%02x-%02x-%02x-%02x-%02x-%02x",
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

            }
        } else {
            if (linebuffer[0] != '\t' && linebuffer[0] != ' ' && linebuffer[0] != '\r'
                && linebuffer[0] != '\n' && linebuffer[0] != '\0') {
                currentAdaptor = 0;
            }
        }
    }

_final:

    _pclose(peer_fd);

    return ans;
}

bool IsTargetProductPresent()
{
   

    bool ans = false;

    if (libusb_init(NULL)) return false;
    do {
        libusb_device ** dev_list = NULL;

        size_t listcnt = libusb_get_device_list(NULL, &dev_list);

        if (listcnt == 0 || !dev_list) break;

        for (size_t pos = 0; pos < listcnt; ++pos) 
        {
            libusb_device_descriptor desc;
            if (libusb_get_device_descriptor(dev_list[pos], &desc)) continue;
            if (desc.idVendor== release_usb_vid && desc.idProduct==release_usb_pid
                && desc.bcdDevice == release_usb_hw_rev) {
                    ans = true;
                    break;
            }
        }


        libusb_free_device_list(dev_list, 0);

    } while(0);
    libusb_exit(NULL);    
    return ans;
}
