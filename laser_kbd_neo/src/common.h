/*
 *  Common Includes
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once 

#if defined(WIN32)
#include "port/win32/inc/arch_inc.h"
#elif defined (_MACOS)
#include "port/macos/inc/arch_inc.h"
#elif defined (__GNUC__)
#include "port/linux/inc/arch_inc.h"
#else
#error "unsupported target"
#endif

#include <stdio.h>

// Device Config
#include "dev_conf.h"


#include <vector>
#include <string>

#include <time.h>


#include "utils.h"
