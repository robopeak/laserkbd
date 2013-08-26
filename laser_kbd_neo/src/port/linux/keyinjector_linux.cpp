/*
 * Copyright (C) 2013 Deepin, Inc.                                              
 *                    Leslie Zhai <zhaixiang@linuxdeepin.com> 
 */

#include "common.h"
#include "port/common/keyinjector.h"
#include "config_mgr.h"

extern ConfigBundle         g_config_bundle;

class OSKeyInjector_Linux : public OSKeyInjector
{
public :
    OSKeyInjector_Linux() {


    }

    virtual ~OSKeyInjector_Linux() {

    }


    virtual bool injectKeyEvents( const std::vector<KeyEventDesc> & intputlist)
    {
        return true;
    }

};

OSKeyInjector* OSKeyInjector::g_inst = NULL;

OSKeyInjector * OSKeyInjector::GetInstance() {
    if (OSKeyInjector::g_inst) {
        return OSKeyInjector::g_inst;
    }

    OSKeyInjector::g_inst = new OSKeyInjector_Linux();
    return OSKeyInjector::g_inst;
}


void OSKeyInjector::ReleaseInstance()
{
    if (OSKeyInjector::g_inst) {
        delete OSKeyInjector::g_inst;
        OSKeyInjector::g_inst = NULL;
    }
}
