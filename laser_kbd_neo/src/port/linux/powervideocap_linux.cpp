/*                                                                              
 * Copyright (C) 2014 - 2015 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 * Copyright (C) 2013 Deepin, Inc.                                                 
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
#include <linux/videodev2.h>                                                    
#include <libv4l2.h>                                                            
#include <fcntl.h>

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
    if (dp) {
        // TODO: travel the video devices
        while (ep = readdir(dp)) {
            if (strstr(ep->d_name, "video"))
                list.insert(list.begin(), "/dev/" + std::string(ep->d_name));
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

//-----------------------------------------------------------------------------
// TODO: set video capture resolution
// 
// NOTE: please read the vidcap_set function in the source code 
// v4l-utils-1.0.1/utils/v4l2-ctl/v4l2-ctl-vidcap.cpp
// 
// FAQ: 
// Q: Why do not use opencv`s cvXXX API? such as 
// cvSetCaptureProperty(arg1, CV_CAP_PROP_FRAME_WIDTH, arg3)
// A: opencv set capture width does not work for Linux via USB1!!!
//-----------------------------------------------------------------------------
bool PowerVideoCapture_Linux::setImageSize(int width, int height)
{
    bool ret = true;
    char buf[PATH_MAX] = {0};
    int fd = -1;
    struct v4l2_format fmt;

    snprintf(buf, sizeof(buf) - 1, "/dev/video%d", _deviceidx);

    if ((fd = v4l2_open(buf, O_RDWR)) == -1) {
        std::cout << "ERROR: fail to open device " << buf << std::endl;
        return false;
    }

    if (width && height) {
        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = width;
        fmt.fmt.pix.height      = height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        if (v4l2_ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)  
            std::cout << "ERROR: fail to set resolution" << std::endl;
    }
    
    v4l2_close(fd);
    fd = -1;
    return ret;
}

bool PowerVideoCapture_Linux::getImageSize(int & width, int & height)
{
    width = (int)cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH);
    height = (int)cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT);
    return true;
}

bool PowerVideoCapture_Linux::setExposureVal(long level)
{
    /* Check video device whether or not support exposure_auto, exposure_absolute 
     * v4l2-ctl -w --all | grep exposure 
     */
    char buf[PATH_MAX] = {0};
    int fd = -1;
    struct v4l2_control ctrl;

    snprintf(buf, sizeof(buf) - 1, "/dev/video%d", _deviceidx);    
                                                                                
    if ((fd = v4l2_open(buf, O_RDWR)) == -1) 
       return false; 
                                                                                
    ctrl.id     = V4L2_CID_EXPOSURE_AUTO;                                           
    ctrl.value  = V4L2_EXPOSURE_MANUAL;                                          
    if (v4l2_ioctl(fd, VIDIOC_S_CTRL, &ctrl) == -1)                             
        std::cout << "DEBUG: fail to set exposure_auto" << std::endl; 
                                                                                
    ctrl.id     = V4L2_CID_EXPOSURE_ABSOLUTE;                                       
    ctrl.value  = 10000.0f * pow((double)2.0f, (double)level);
    if (v4l2_ioctl(fd, VIDIOC_S_CTRL, &ctrl) == -1) 
        std::cout << "DEBUG: fail to set exposure_absolute" << std::endl;
                                                                                
    v4l2_close(fd);                                                             
    fd = -1;

    return true;
}

IplImage* PowerVideoCapture_Linux::retrieveFrame()
{
    cvGrabFrame(_capture);
    return cvRetrieveFrame(_capture, 0);
}
