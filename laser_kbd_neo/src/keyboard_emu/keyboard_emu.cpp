/*
 *  Keyboard event emulator and trigger
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "../common.h"
#include "../cv_common.h"
#include "keyboard_emu.h"

#include "port/common/keyinjector.h"
#include "config_mgr.h"

#include <vector>


using namespace std;

extern ConfigBundle         g_config_bundle;


KeyboardEmu::KeyboardEmu(KeyLayoutProvider & provider)
: _last_fire_time(0)
, _is_refire_mode(false)
, _provider(provider)
{
    
}

KeyboardEmu::~KeyboardEmu()
{
    OSKeyInjector::ReleaseInstance();
}


void KeyboardEmu::onKeyboardTick(const std::vector<TrackPoint> & points, bool injectevent)
{
    if (points.size() == 0 && _last_key_pressed.size() == 0) return;
    std::vector<int> key_pressed;
    key_pressed.clear();

    for (int pos=0; pos<points.size(); ++pos)
    {
        int current;
        current = _provider.findKeyIdxByPoint (points[pos].x, points[pos].y) ;
        if (current>=0) {
            key_pressed.push_back(current);
        }
    }
    if (injectevent) {
        injectKeyArray(key_pressed);
    }
    _last_key_pressed = key_pressed;
}

bool KeyboardEmu::injectKeyArray(const std::vector<int> & key_list)
{
    unsigned long long currentbit = 0;
    unsigned long long lastbit = 0;
    
    unsigned long long released_bit = 0;
    unsigned long long pressed_bit = 0;
    unsigned long long common_bit = 0;
    for (int pos =0; pos < _last_key_pressed.size(); ++pos)
    {
        assert(_last_key_pressed[pos] < sizeof(lastbit)*8);
        lastbit |= ((unsigned long long )0x1<<_last_key_pressed[pos]);
    }

    for (int pos =0; pos < key_list.size(); ++pos)
    {
        assert(key_list[pos] < sizeof(currentbit)*8);
        currentbit |= ((unsigned long long )0x1<<key_list[pos]);
    }

    common_bit = currentbit & lastbit;
    released_bit = lastbit & (~(common_bit));
    pressed_bit = currentbit & (~(common_bit));

   



    std::vector<KeyEventDesc> inputArray;

    // send  key release events

    for (int pos=0; pos<sizeof(released_bit)*8; ++pos)
    {
        if ( released_bit & ((unsigned long long )0x1 << pos) ) {
            inputArray.push_back(KeyEventDesc(KEY_EVENT_RELEASED, _provider.getKeyAt(pos)->lcase_val));
        }
    }

    // send  key press events
    for (int pos=0; pos<sizeof(pressed_bit)*8; ++pos)
    {
        if ( pressed_bit & ((unsigned long long )0x1 << pos) ) {
            inputArray.push_back(KeyEventDesc(KEY_EVENT_PRESSED, _provider.getKeyAt(pos)->lcase_val));
        }
    }

    
    if (inputArray.size())  {
        _is_refire_mode = false;
        _last_fire_time = getms() ;
        OSKeyInjector::GetInstance()->injectKeyEvents(inputArray);
    }

    if (!_is_refire_mode && getms() - _last_fire_time < g_config_bundle.keyrefire_delay) {
        return true;
    }

    _is_refire_mode = true;

    if (getms() - _last_fire_time > g_config_bundle.keyrefire_interval) {
        // send refire
        _last_fire_time = getms() ;

        // send  key press events
        for (int pos=0; pos<sizeof(common_bit)*8; ++pos)
        {
            if ( common_bit & ((unsigned long long )0x1 << pos) ) {
                inputArray.push_back(KeyEventDesc(KEY_EVENT_PRESSED, _provider.getKeyAt(pos)->lcase_val));
            }
        }
    }
    
    if (inputArray.size())  {
        OSKeyInjector::GetInstance()->injectKeyEvents(inputArray);
    }

    return true;

}