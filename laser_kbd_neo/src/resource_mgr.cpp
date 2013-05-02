/*
 *  Laser Keyboard
 *  Resource Loader and Manager
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#include "common.h"
#include "cv_common.h"

#include "resource_mgr.h"
#include "port/common/platform_language.h"

ResourceMgr::ResourceMgr()
{

}

ResourceMgr::~ResourceMgr()
{
    releaseStuffs();
}


IplImage * ResourceMgr::getImageRes(const char * imagename)
{
    // find whether the resource has been loaded

    std::map<std::string, IplImage *>::const_iterator itr 
        = _image_pool.find(imagename);

    if (itr != _image_pool.end()) {
        return itr->second;
    }


    //load resources based on the following order:
    // a. current language
    // b. common
    std::string loadpath;
    loadpath  = getPlatformResPrefix();
    loadpath+= FILEPATH_RESOURCE_LANG_FOLDER;
    loadpath+= GetBestMatchedLanguageString();
    loadpath+= "/";
    loadpath+= imagename;

    IplImage * ans = cvLoadImage(loadpath.c_str());

    if (ans) {
        _image_pool[imagename] = ans;
        return ans;
    }

    loadpath  = getPlatformResPrefix();
    loadpath+= FILEPATH_RESOURCE_COMMON_FOLDER;
    loadpath+= imagename;
    ans = cvLoadImage(loadpath.c_str());
    if (ans) {
        _image_pool[imagename] = ans;
    }
    return ans;
}

void ResourceMgr::releaseStuffs()
{
    std::map<std::string, IplImage *>::iterator itr 
        = _image_pool.begin();

    while (itr != _image_pool.end()) {
        cvReleaseImage(&itr->second);
        ++itr;
    }
}