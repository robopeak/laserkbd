/*
 *  multi-touch sketch pad
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

class SketchPad
{
public:
    SketchPad(KeyLayoutProvider &);
    ~SketchPad();
    void onSketchPadTick(const std::vector<TrackPoint> & points);

protected:
    IplImage* _sketch_img;
    unsigned long idle_time;

    KeyLayoutProvider & _provider;


    static void s_onMouse(int mouse_event, int x, int y,int, void* pThis);
    void onClear();
};