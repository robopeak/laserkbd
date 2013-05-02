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
#include "port/win32/3rd/opencv/cap_dshow.h"

class PowerVideoCapture_DShow : public PowerVideoCapture{
public:
    virtual ~PowerVideoCapture_DShow();
    virtual bool setImageSize(int width, int height);
    virtual bool getImageSize(int & width, int & height) ;
    virtual bool setExposureVal(long level) ;
    virtual IplImage* retrieveFrame() ;
    PowerVideoCapture_DShow(int idx);
protected:
    
    
    int    _deviceidx;
    IplImage* _cached_frame;
    
};


// ----
static videoInput _VI;
PowerVideoCapture * PowerVideoCapture::CreateCaptureByID( int id)
{

    if (id < 0) return NULL;

    PowerVideoCapture_DShow * newborn = new PowerVideoCapture_DShow(id);

    int devcount = videoInput::listDevices(true);
    
    if (devcount<=id) goto badthing_happened;

    if (!_VI.setupDevice(id)) goto badthing_happened;

    return newborn;

badthing_happened:
    delete newborn;
    return NULL;
}

size_t PowerVideoCapture::EnumCaptureDevices(std::vector<std::string> & list)
{
    list.clear();

    int devCount =  videoInput::listDevices(true);

    while (devCount--) {
        list.push_back(videoInput::getDeviceName(list.size()));
    }
    
    return list.size();
}


// ----

PowerVideoCapture_DShow::PowerVideoCapture_DShow(int idx)
    : _deviceidx(idx)
    , _cached_frame(NULL)
{

}

PowerVideoCapture_DShow::~PowerVideoCapture_DShow()
{
    _VI.stopDevice(_deviceidx);
    _deviceidx = -1;
    cvReleaseImage(&_cached_frame);
}

bool PowerVideoCapture_DShow::setImageSize(int width, int height)
{
    if( width > 0 && height > 0 )
    {
        if( width != _VI.getWidth(_deviceidx) || height != _VI.getHeight(_deviceidx) )
        {
            _VI.stopDevice(_deviceidx);
            _VI.setupDevice(_deviceidx, width, height);
        }
        return _VI.isDeviceSetup(_deviceidx);
    }
    return false;
}


bool PowerVideoCapture_DShow::getImageSize(int & width, int & height)
{
    width = _VI.getWidth(_deviceidx);
    height = _VI.getHeight(_deviceidx);
    return true;
}

// definition from DShow
enum  CameraControlFlags{ 
  CameraControl_Flags_Auto    = 0x0001,
  CameraControl_Flags_Manual  = 0x0002, 
} ;


bool PowerVideoCapture_DShow::setExposureVal(long level)
{
    return _VI.setVideoSettingCamera(_deviceidx, _VI.propExposure, level, CameraControl_Flags_Manual, false);
}



IplImage* PowerVideoCapture_DShow::retrieveFrame()
{
    if( !_cached_frame || _VI.getWidth(_deviceidx) != _cached_frame->width || _VI.getHeight(_deviceidx) != _cached_frame->height )
    {
        if (_cached_frame)
            cvReleaseImage( &_cached_frame );
        int w = _VI.getWidth(_deviceidx), h = _VI.getHeight(_deviceidx);
        _cached_frame = cvCreateImage( cvSize(w,h), 8, 3 );
    }

#define CameraControl_Flags_Manual 
    _VI.getPixels( _deviceidx, (uchar*)_cached_frame->imageData, false, true );
    return _cached_frame;
}
