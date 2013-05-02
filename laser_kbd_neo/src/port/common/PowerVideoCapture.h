/*
 *  Laser Keyboard
 *  Powerful Video Capture Class
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */
#pragma once

class PowerVideoCapture
{

public:
    static PowerVideoCapture * CreateCaptureByID( int id);
    static size_t EnumCaptureDevices(std::vector<std::string> & list);

public:
    virtual ~PowerVideoCapture() {}
    virtual bool setImageSize(int width, int height) = 0;
    virtual bool getImageSize(int & width, int & height) = 0;
    virtual bool setExposureVal(long level) = 0;
    virtual IplImage* retrieveFrame() = 0;


protected:
    PowerVideoCapture() {}
};