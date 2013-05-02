/*
 *  Laser Keyboard
 *  Resource Loader and Manager
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#pragma once

#include <map>

class ResourceMgr {
public:
    ResourceMgr();
    ~ResourceMgr();


    IplImage * getImageRes(const char * imagename);

protected:
    void releaseStuffs();

    std::map<std::string, IplImage *> _image_pool;

};