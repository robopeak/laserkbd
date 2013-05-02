/*
 *  Laser Keyboard
 *  Product Identification Supporting Lib
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once


const char * GetPlatformName();

struct platform_id_t {
    unsigned char id_buf[6];
};

bool GetPlatformIdentification(platform_id_t & id);
bool IsTargetProductPresent();