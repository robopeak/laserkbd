/*
 *  multi-touch sketch pad
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#include "../common.h"
#include "../cv_common.h"
#include "../keyboard_emu/layout_provider.h"
#include "sketch_pad.h"
#include <vector>



const CvScalar colorArr[10] = 
{
    cvScalar(128,128,255),
    cvScalar(0xff,0,0xf6),
    cvScalar(255,0,248),
    cvScalar(255,168,0),
    cvScalar(255,240,0),
    cvScalar(96,255,0),
    cvScalar(0,138,0),
    cvScalar(255,25,161),
    cvScalar(13,233,220),
    cvScalar(142,129,20),
};

SketchPad::SketchPad(KeyLayoutProvider & p)
: _provider(p)
{
    int winWidth = p.getLayoutSize().width*2;
    _sketch_img = cvCreateImage(cvSize(winWidth,SKETCH_AREA_HEIGHT), 8, 3);
    cvFillImage(_sketch_img, 0xFFFFFF);
}

SketchPad::~SketchPad()
{
    cvReleaseImage(&_sketch_img);
    
    if (cvGetWindowHandle("sketchpad")) 
        cvDestroyWindow("sketchpad");
}

void SketchPad::s_onMouse(int mouse_event, int x, int y,int, void* pThis)
{
    SketchPad * This = reinterpret_cast<SketchPad *>(pThis);
    if (mouse_event == CV_EVENT_RBUTTONDOWN) {
        This->onClear();
    }
}

void SketchPad::onClear()
{
    idle_time = time(NULL);
    cvFillImage(_sketch_img, 0xFFFFFF);
    cvShowImage("sketchpad", _sketch_img);   
}

void SketchPad::onSketchPadTick(const std::vector<TrackPoint> & points)
{
   
    if (points.size() == 0)
    {
#if 0
        if ( time(NULL) - idle_time > SKETCH_AUTOCLEAR_TIMEOUT)
        {
            onClear();
        }
#endif        
    } else {
        for (int pos=0; pos<points.size() && pos<10; ++pos)
        {
            CvPoint2D32f posInWindow = _provider.keyboardPosToWindowPos(cvPoint2D32f(points[pos].x, points[pos].y));
            posInWindow.x *= 2;//(float)SKETCH_AREA_WIDTH/_provider.getLayoutSize().width ;
            cvCircle(_sketch_img, cvPoint(posInWindow.x, posInWindow.y*2), points[pos].pressure*0.8,
                colorArr[pos], -1, CV_AA );
        }

        ::cv_textOut(_sketch_img, 4,20, "Click the right mouse button to clear", cvScalar(0x90,0x90,0x90), 1.6);

        cvShowImage("sketchpad", _sketch_img);
        cvSetMouseCallback("sketchpad", &SketchPad::s_onMouse, this);
        idle_time = time(NULL);
    }
}
