/*
 *  Laser Keyboard
 *  Powerful Video Capture Class
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#include "common.h"
#include "cv_common.h"
#include "port/common/PowerVideoCapture.h"
#import "port/macos/3rd/opencv/cap_qtkit.h"

class PowerVideoCapture_QTKit : public PowerVideoCapture{
public:
    virtual ~PowerVideoCapture_QTKit();
    virtual bool setImageSize(int width, int height);
    virtual bool getImageSize(int & width, int & height) ;
    virtual bool setExposureVal(long level) ;
    virtual IplImage* retrieveFrame() ;
    PowerVideoCapture_QTKit(int idx);
    
    bool    isAlive();
protected:
    
    
    int    _deviceidx;
    IplImage* _cached_frame;
    QTKitCapture _capture;
};


// ----

PowerVideoCapture * PowerVideoCapture::CreateCaptureByID( int id)
{

    if (id < 0) return NULL;

    PowerVideoCapture_QTKit * newborn = new PowerVideoCapture_QTKit(id);
    if (!newborn) return NULL;
    
    if (!newborn->isAlive()) goto badthing_happened;

    return newborn;

badthing_happened:
    delete newborn;
    return NULL;
}

size_t PowerVideoCapture::EnumCaptureDevices(std::vector<std::string> & list)
{
    return QTKitCapture::listDevices(list);
}


// ----

PowerVideoCapture_QTKit::PowerVideoCapture_QTKit(int idx)
    : _deviceidx(idx)
    , _cached_frame(NULL)
    , _capture(idx)
{

}

bool PowerVideoCapture_QTKit::isAlive() {
    return _capture.didStart();
}

PowerVideoCapture_QTKit::~PowerVideoCapture_QTKit()
{
    _deviceidx = -1;

}

bool PowerVideoCapture_QTKit::setImageSize(int width, int height)
{
    if( width > 0 && height > 0 )
    {
        return _capture.setProperty(CV_CAP_PROP_FRAME_WIDTH, (double)width);
        _capture.setProperty(CV_CAP_PROP_FRAME_HEIGHT, (double)height);
    }
    return false;
}


bool PowerVideoCapture_QTKit::getImageSize(int & width, int & height)
{
    width = (int)_capture.getProperty(CV_CAP_PROP_FRAME_WIDTH);
    height = (int)_capture.getProperty(CV_CAP_PROP_FRAME_HEIGHT);
    return true;
}


bool PowerVideoCapture_QTKit::setExposureVal(long level)
{
    return _capture.setExposureLevel(level);
}



IplImage* PowerVideoCapture_QTKit::retrieveFrame()
{
    _capture.grabFrame();
    return _capture.retrieveFrame(0);
}
