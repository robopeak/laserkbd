/*
 *  Utilities for CV
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#pragma once

void cv_copyTo(IplImage* src, IplImage * dest, const CvRect& region);

void cv_textOut(IplImage* img, int x, int y, const char * txt, CvScalar color = cvScalar(255,255,255), float size = 1.0, bool centerToPoint = false);