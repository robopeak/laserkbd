/*
 *  Laser Keyboard
 *  OS Key Events Injector
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

enum KeyEventType {
  KEY_EVENT_RELEASED = 0, 
  KEY_EVENT_PRESSED  = 1,
};

struct KeyEventDesc {
    KeyEventDesc(KeyEventType t, int v) 
        : type(t), keyval(v) {}

    KeyEventDesc(): type(KEY_EVENT_RELEASED), keyval(0) {}

    KeyEventType type;
    int          keyval;
};

class OSKeyInjector
{
public:
    // WARNING: race condition will occur when these functions are used in 
    //          multi-thread envirnoment.

    static OSKeyInjector * GetInstance();
    static void ReleaseInstance();

protected:
    OSKeyInjector() {}

    static  OSKeyInjector* g_inst;

public:
    virtual ~OSKeyInjector() {}

    virtual bool injectKeyEvents( const std::vector<KeyEventDesc> & ) = 0;

};