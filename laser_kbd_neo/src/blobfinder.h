/* 
 *  Blob Finder 
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

#include <vector>

struct Blob_t
{
    cv::Rect    _box;
    cv::Point2f _center;
    float       _area;

    Blob_t(const cv::Rect & box, const cv::Point2f & center, float area)
        :   _box(box), _center(center), _area(area) {}

    Blob_t()
        : _box(0,0,0,0), _center(0,0), _area(0)
    {}
};

void FindBlobs(IplImage *src, std::vector<Blob_t>& blobs, int minArea, int maxArea);
