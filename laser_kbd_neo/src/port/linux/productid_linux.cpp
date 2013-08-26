/*
 * Copyright (C) 2013 Deepin, Inc.                                              
 *                    Leslie Zhai <zhaixiang@linuxdeepin.com>
 */

#include <usb.h>

#include "common.h"
#include "port/common/productid.h"

const char * GetPlatformName()
{
#ifdef __GNUC__
    return "Linux Deepin";
#else
    return "Linux";
#endif
}

static const unsigned char xor_keyring[6] = {0x02, 0x4D, 0x8E, 0x1C, 0x48, 0x3F,};

// Get the first network mac as the platform id, xor it for privacy protection
bool GetPlatformIdentification(platform_id_t & id)
{
    return true;
}

bool IsTargetProductPresent()
{
    return true;
}
