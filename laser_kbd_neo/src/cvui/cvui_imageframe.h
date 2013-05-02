/*
 *  Simple UI System for OpenCV
 *  Image Frame Object
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

namespace rp{ namespace cvui {

class ImageFrame : public RenderObject
{
public:
    ImageFrame() ;
    virtual ~ImageFrame();

    void setImage(IplImage * img);
    virtual void renderSelf(IplImage* frame);

protected:
    IplImage * _img;
};


}}