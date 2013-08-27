/*                                                                              
 * Copyright (C) 2013 Deepin, Inc.                                                 
 *               2013 Leslie Zhai                                         
 *                                                                              
 * Author:     Leslie Zhai <zhaixiang@linuxdeepin.com>                           
 * Maintainer: Leslie Zhai <zhaixiang@linuxdeepin.com>                           
 *                                                                              
 * This program is free software: you can redistribute it and/or modify         
 * it under the terms of the GNU General Public License as published by         
 * the Free Software Foundation, either version 3 of the License, or            
 * any later version.                                                           
 *                                                                              
 * This program is distributed in the hope that it will be useful,              
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                
 * GNU General Public License for more details.                                 
 *                                                                              
 * You should have received a copy of the GNU General Public License            
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.        
 */

#include <fstream>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <libusb.h>

#include "common.h"
#include "port/common/productid.h"

const char* GetPlatformName()
{
    return "linux";
}

static const unsigned char xor_keyring[6] = {0x02, 0x4D, 0x8E, 0x1C, 0x48, 0x3F,};

// Get the first network mac as the platform id, xor it for privacy protection
bool GetPlatformIdentification(platform_id_t & id)
{
    DIR* dp = NULL;                                                             
    struct dirent* ep = NULL;
    const char* sys_net_path = "/sys/class/net";
    char address_path[PATH_MAX]= {0};
    std::ifstream address_file;
    std::string address_str = "";
    int buf[6] = {0};
    int i;
                                                                                
    dp = opendir(sys_net_path);                                                       
    if (dp)                                                                     
    {                                                                           
        while (ep = readdir(dp))                                                
        {                                                                       
            if (strstr(ep->d_name, "eth") || strstr(ep->d_name, "wlan")) 
            {
                snprintf(address_path, PATH_MAX, "%s/%s/address", sys_net_path, ep->d_name);
                address_file.open(address_path);
                address_file >> address_str;
                sscanf(address_str.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", 
                       &buf[0], 
                       &buf[1], 
                       &buf[2], 
                       &buf[3], 
                       &buf[4], 
                       &buf[5]);
                for (i = 0; i < 6; ++i) 
                    id.id_buf[i] = (unsigned char)buf[i] ^ xor_keyring[i];
                address_file.close();
                break;
            } 
        }                                                                       
        closedir(dp);                                                           
        dp = NULL;                                                              
        ep = NULL;                                                              
    }
    
    return true;
}

bool IsTargetProductPresent()
{
    bool ans = false;
    libusb_device** dev_list = NULL;
    size_t listcnt = 0;
    size_t pos;

    if (libusb_init(NULL)) 
        return false;

    do 
    {                                                                        
        listcnt = libusb_get_device_list(NULL, &dev_list);               
        if (listcnt == 0 || !dev_list) 
            break;                                   
        for (pos = 0; pos < listcnt; ++pos)                              
        {                                                                       
            libusb_device_descriptor desc;                                      
            if (libusb_get_device_descriptor(dev_list[pos], &desc)) 
                continue;   
            if (desc.idVendor  == release_usb_vid && 
                desc.idProduct == release_usb_pid && 
                desc.bcdDevice == release_usb_hw_rev) 
            {                      
                ans = true;                                                 
                break;                                                      
            }                                                                   
        }
        libusb_free_device_list(dev_list, 0);
    } while (0);                                                                 
    // FIXME: libusbx: warning [libusb_exit] application left some devices open
    //libusb_exit(NULL);                                                          
    return ans;
}
