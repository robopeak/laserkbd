/*
 *  Simple UI System for OpenCV
 *  Image Frame Object
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"


#include "cvui_base.h"
#include "cvui_imageframe.h"



namespace rp{ namespace cvui {


ImageFrame::ImageFrame() 
    : _img(NULL)
{

}

ImageFrame::~ImageFrame()
{

}


void ImageFrame::setImage(IplImage * img)
{
    if (!img) return;
    // set self size according to the image size
    this->setSize(img->width, img->height);
    _img = img;
}

void ImageFrame::renderSelf(IplImage* frame)
{
    if (!_img) return;

    // copy the image to the frame
    assert(frame->depth == _img->depth);

    cv_copyTo(_img, frame, cvRect(_x, _y, _width, _height));
    RenderObject::renderSelf(frame);
}

}}