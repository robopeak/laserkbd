/*
 *  Laser Keyboard 
 *  Config Manager
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

#include <vector>
#include <map>

class ConfigMgr;

struct ConfigBundle {
    ConfigBundle(ConfigMgr & mgr);
    
    void flush();
    void load();
    void loadDefaultValues();
    //---
    int exposure_level;
    int keyrefire_delay;
    int keyrefire_interval;
    int default_cam_id;
    int playsound;

    //
    int model_id;
    ConfigMgr & _mgr;
};

struct configBlob {
    std::string key;
    std::string val;
};

class ConfigMgr {
public:
    

    ConfigMgr();
    ~ConfigMgr();


    bool loadConfigFromFile();
    void flushConfigFile();


    bool getInt(const char * key, int & val);
    bool getFloat(const char * key, float & val);

    void setInt(const char * key, int val);
    void setFloat(const char * key, float val);

protected:

    void _setKeyVal(const char * key, const char * val);
    std::string * _getKeyVal(const char * key);

    void _nukeData();

    

    std::vector<configBlob *> _bloblist;
    std::map<std::string, configBlob *> _blobmap;

};