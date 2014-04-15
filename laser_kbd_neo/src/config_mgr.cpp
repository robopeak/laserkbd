/*
 *  Laser Keyboard 
 *  Config Manager
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"

#include "config_mgr.h"


ConfigBundle::ConfigBundle(ConfigMgr & mgr) 
    : _mgr(mgr)
{
    loadDefaultValues();
}

    
void ConfigBundle::flush() 
{
#define STORE_INT_BY_FIELD(_x_) \
    _mgr.setInt( #_x_, _x_)

    STORE_INT_BY_FIELD(exposure_level);
    STORE_INT_BY_FIELD(keyrefire_delay);
    STORE_INT_BY_FIELD(keyrefire_interval);
    STORE_INT_BY_FIELD(default_cam_id);
    STORE_INT_BY_FIELD(playsound);
}

void ConfigBundle::load() 
{
#define LOAD_INT_BY_FIELD(_x_) \
    do{ \
        int tmp; \
        if (_mgr.getInt( #_x_, tmp)) { \
            _x_ = tmp; \
        } \
    } while(0)

    LOAD_INT_BY_FIELD(exposure_level);
    LOAD_INT_BY_FIELD(keyrefire_delay);
    LOAD_INT_BY_FIELD(keyrefire_interval);
    LOAD_INT_BY_FIELD(default_cam_id);
    LOAD_INT_BY_FIELD(playsound);
}

void ConfigBundle::loadDefaultValues() 
{
    exposure_level  = CAM_EXPOSURE_LEVEL;
    keyrefire_delay = REFIRE_DELAY;
    keyrefire_interval = REFIRE_INTERVAL;
    default_cam_id  = 0;
    playsound = 1;

    model_id = 0;
}

//----

ConfigMgr::ConfigMgr()
{

}

ConfigMgr::~ConfigMgr()
{
    _nukeData();
}

void ConfigMgr::_nukeData()
{
    for (int pos = 0; pos < _bloblist.size(); ++pos) {
        delete _bloblist[pos];
    }

    _bloblist.clear();
    _blobmap.clear();

}




bool ConfigMgr::loadConfigFromFile()
{
    std::string filePath = getPlatformConfigPrefix();
    filePath += FILEPATH_CONFIG_FILE;
    FILE * file = fopen(filePath.c_str(), "r");
    if (!file) return false;
    
    char linebuffer[1024];
    std::vector<std::string> arglist;
    while (fgets(linebuffer, sizeof(linebuffer), file)) {
        arglist.clear();

        for (int pos = 0; pos < strnlen(linebuffer, sizeof(linebuffer)); ++pos) {
            if (linebuffer[pos] == '\n' || linebuffer[pos] == '\r' 
                || linebuffer[pos] == '\0') {
                linebuffer[pos] = '\0';
                int tokencount = split(linebuffer, arglist, '=');

                if (tokencount == 2) {
                    std::string key = trim(arglist[0].c_str());
                    std::string val = trim(arglist[1].c_str());

                    _setKeyVal(key.c_str(), val.c_str());
                }

                break;
            }
        }
    }

    fclose(file);

    return true;
}

void ConfigMgr::flushConfigFile()
{
    std::string filePath = getPlatformConfigPrefix();
    filePath += FILEPATH_CONFIG_FILE;
    FILE * file = fopen(filePath.c_str(), "w");
    if (!file) return;

    for (int pos = 0; pos < _bloblist.size(); ++pos) {
        fprintf(file, "%s = %s\n", _bloblist[pos]->key.c_str(), _bloblist[pos]->val.c_str());
    }

    fclose(file);
}


void ConfigMgr::_setKeyVal(const char * key, const char * val)
{
    std::map<std::string, configBlob *>::const_iterator itr
        = _blobmap.find(key);

    if (itr != _blobmap.end()) {
        itr->second->val = val;
        return;
    }

    configBlob * newborn = new configBlob();
    newborn->key = key;
    newborn->val = val;

    _bloblist.push_back(newborn);
    _blobmap[key] = newborn;
}

std::string * ConfigMgr::_getKeyVal(const char * key)
{
    std::map<std::string, configBlob *>::const_iterator itr
        = _blobmap.find(key);

    if (itr != _blobmap.end()) {
        return &itr->second->val;
    }

    return NULL;
}

bool ConfigMgr::getInt(const char * key, int & val)
{
    std::string * strval = _getKeyVal(key);

    if (!strval) return false;

    val = strtol(strval->c_str(), NULL, 10);

    return true;
}


bool ConfigMgr::getFloat(const char * key, float & val)
{
    std::string * strval = _getKeyVal(key);

    if (!strval) return false;

    val = (float)strtod(strval->c_str(), NULL);

    return true;

}


void ConfigMgr::setInt(const char * key, int val)
{
    char buf[1024];
    sprintf(buf, "%d" , val);
    _setKeyVal(key, buf);
}

void ConfigMgr::setFloat(const char * key, float val)
{
    char buf[1024];
    sprintf(buf, "%.8f", val);
    _setKeyVal(key, buf);
}