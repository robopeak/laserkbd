/*
 *  Utilities for CV
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"

void cv_textOut(IplImage* img, int x, int y, const char * txt, CvScalar color, float size, bool centerToPoint)
{
    CvFont  stdFont;
    cvInitFont(&stdFont,CV_FONT_VECTOR0,0.3*size,0.4*size, 0, 1);
    
    if (centerToPoint) {
        int baseline=0;
        CvSize textSize;
        cvGetTextSize(txt, &stdFont, &textSize,  &baseline);
        x-=textSize.width/2;
        y+=textSize.height/2;
    }

    cvPutText(img, txt, cvPoint(x,y),&stdFont, color);
}

void cv_copyTo(IplImage* src, IplImage * dest, const CvRect& region)
{
    cvSetImageROI(dest, region);

    cvResize(src, dest);
    cvResetImageROI(dest);
}