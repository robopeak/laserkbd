/*
 *   Laser Keyboard
 *   Commond definition of the CV system
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#pragma once

// OpenCV Related
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

// Common Data Structures
struct TrackPoint 
{
    float x, y;
    float pressure;
    TrackPoint(): x(0), y(0), pressure(0) {}
    TrackPoint(float _x, float _y, float _p): x(_x), y(_y), pressure(_p) {}
};

#include "cv_utils.h"
#include "blobfinder.h"
#include "cameraundistortor.h"
#include "cvpositionest.h"
#include "port/common/PowerVideoCapture.h"