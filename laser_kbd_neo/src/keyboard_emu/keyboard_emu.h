/*
 *  Keyboard event emulator and trigger
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

#include "layout_provider.h"

class KeyboardEmu {
public:
    KeyboardEmu(KeyLayoutProvider & provider);
    ~KeyboardEmu();
    void onKeyboardTick(const std::vector<TrackPoint> &, bool injectevent = true);

    const std::vector<int> & getPressedKeyIdx() { return _last_key_pressed; }
protected:
    bool injectKeyArray(const std::vector<int> &);
    IplImage* _kdb_img;
    std::vector<int> _last_key_pressed;
    unsigned long _last_fire_time;
    bool _is_refire_mode ;
    KeyLayoutProvider & _provider;
};