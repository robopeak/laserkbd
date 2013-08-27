/*                                                                              
 * Copyright (C) 2013 Deepin, Inc.                                                 
 *               2013 Leslie Zhai                                                  
 *                                                                              
 * Author:     Leslie Zhai <zhaixiang@linuxdeepin.com>                           
 * Maintainer: Leslie Zhai <zhaixiang@linuxdeepin.com>                           
 *                                                                              
 * This program is free software: you can redistribute it and/or modify         
 * it under the terms of the GNU General Public License as published by         
 * the Free Software Foundation, either version 3 of the License, or            
 * any later version.                                                           
 *                                                                              
 * This program is distributed in the hope that it will be useful,              
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                
 * GNU General Public License for more details.                                 
 *                                                                              
 * You should have received a copy of the GNU General Public License            
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.        
 */

#include <dirent.h>
#include <string.h>

#include "common.h"
#include "cv_common.h"
#include "port/common/PowerVideoCapture.h"

class PowerVideoCapture_Linux : public PowerVideoCapture 
{
public:
    virtual ~PowerVideoCapture_Linux();
    virtual bool setImageSize(int width, int height);
    virtual bool getImageSize(int & width, int & height);
    virtual bool setExposureVal(long level);
    virtual IplImage* retrieveFrame();
    PowerVideoCapture_Linux(int idx);
    bool    isAlive();

protected:
    int    _deviceidx;
    CvCapture* _capture;
};

PowerVideoCapture* PowerVideoCapture::CreateCaptureByID(int id)
{
    if (id < 0) 
        return NULL;

    PowerVideoCapture_Linux * newborn = new PowerVideoCapture_Linux(id);
    if (!newborn) 
        return NULL;
    
    if (!newborn->isAlive()) 
    {
        delete newborn;
        newborn = NULL;
        return NULL;
    }

    return newborn;
}

size_t PowerVideoCapture::EnumCaptureDevices(std::vector<std::string> & list)
{
    DIR* dp = NULL;
    struct dirent* ep = NULL;

    dp = opendir("/dev");
    if (dp) 
    {
        while (ep = readdir(dp)) 
        {
            if (strstr(ep->d_name, "video"))
                list.push_back("/dev/" + std::string(ep->d_name));
        }
        closedir(dp);
        dp = NULL;
        ep = NULL;
    }
    
    return list.size();
}

PowerVideoCapture_Linux::PowerVideoCapture_Linux(int idx)
    : _deviceidx(idx)
{
    _capture = cvCaptureFromCAM(idx);
}

bool PowerVideoCapture_Linux::isAlive() {
    return _capture ? true : false;
}

PowerVideoCapture_Linux::~PowerVideoCapture_Linux()
{
    cvReleaseCapture(&_capture);
    _deviceidx = -1;
}

bool PowerVideoCapture_Linux::setImageSize(int width, int height)
{
    int ret = 0;

    if (width && height)
    {
        ret = cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH, (double)width);
        ret = cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT, (double)height);
    }

    return ret ? true : false;
}

bool PowerVideoCapture_Linux::getImageSize(int & width, int & height)
{
    width = (int)cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH);
    height = (int)cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT);
    return true;
}

bool PowerVideoCapture_Linux::setExposureVal(long level)
{
    /* FIXME: HIGHGUI ERROR: V4L: Property Exposure(15) not supported by device
    cvSetCaptureProperty(_capture, 
                         CV_CAP_PROP_EXPOSURE, 
                         10000.0f * pow((double)2.0f, (double)level));
    */
    return true;
}

IplImage* PowerVideoCapture_Linux::retrieveFrame()
{
    cvGrabFrame(_capture);
    return cvRetrieveFrame(_capture, 0);
}
