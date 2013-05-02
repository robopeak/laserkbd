/*
 *  Simple UI System for OpenCV
 *  Button Object
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

namespace rp{ namespace cvui {


class Button : public RenderObject
{
public:
    enum {
        STATE_ACTIVED = 0x2,
        STATE_HOVERED = 0x1,
    };
    Button() ;
    virtual ~Button();

    void setNormalImage(IplImage * img);
    void setHoveredImage(IplImage * img);
    void setActiveImage(IplImage * img);
    virtual void renderSelf(IplImage* frame);

    void setListener(ui_evt_listener listener, void * data = NULL);

    virtual bool handleMouseEnter(int button);
    virtual bool handleMouseLeave(int button);
    virtual bool handleMousePress(int button, int x, int y) ;
    virtual bool handleMouseRelease(int button, int x, int y);

protected:
    IplImage * _hover_img;
    IplImage * _active_img;
    IplImage * _normal_img;

    int _state_msk; // 0-normal 1-hovered 2-active

    void *          _listener_data;
    ui_evt_listener _listener;
};


}}