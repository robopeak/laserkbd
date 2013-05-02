/*
 *  Simple UI System for OpenCV
 *  Button Object
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#include "common.h"
#include "cv_common.h"


#include "cvui_base.h"
#include "cvui_button.h"



namespace rp{ namespace cvui {


Button::Button()
    : _hover_img(NULL)
    , _active_img(NULL)
    , _normal_img(NULL)
    , _state_msk(0)
    , _listener_data(NULL)
    , _listener(NULL)
{

}

Button::~Button()
{

}


void Button::setNormalImage(IplImage * img)
{
    if (!img) return;
    _normal_img = img;
    this->setSize(img->width, img->height);
}

void Button::setHoveredImage(IplImage * img)
{
    if (!img) return;
    _hover_img = img;
}

void Button::setActiveImage(IplImage * img)
{
    if (!img) return;
    _active_img = img;
}

void Button::renderSelf(IplImage* frame)
{
    IplImage * render_img = _normal_img;
    do {
        if (_state_msk & STATE_ACTIVED) {
            if (_active_img) {
                render_img = _active_img;
            } else if (_hover_img) {
                render_img = _hover_img;
            }
            break;
        }

        if (_state_msk & STATE_HOVERED) {
            if (_hover_img) {
                render_img = _hover_img;
            } else {
                render_img = _active_img;
            }
            break;
        }
    } while(0);
    if (!render_img) return ;
    // copy the image to the frame
    assert(frame->depth == render_img->depth);
    cv_copyTo(render_img, frame, cvRect(_x, _y, _width, _height));
    RenderObject::renderSelf(frame);
}


void Button::setListener(ui_evt_listener listener, void * data)
{
    _listener_data = data;
    _listener = listener;
}

bool Button::handleMouseEnter(int button)
{
    _state_msk |= STATE_HOVERED;
    this->setDirtyFlag();
    return true;
}

bool Button::handleMouseLeave(int button)
{
    _state_msk &= ~(STATE_HOVERED | STATE_ACTIVED);
    this->setDirtyFlag();
    return true;
}

bool Button::handleMousePress(int button, int x, int y) 
{
    _state_msk |= STATE_ACTIVED;
    this->setDirtyFlag();

    if (_listener) {
        _listener(_listener_data, button, x, y);
    }
    return true;
}

bool Button::handleMouseRelease(int button, int x, int y)
{
    _state_msk &= ~STATE_ACTIVED;
    this->setDirtyFlag();
    return true;
}

}}