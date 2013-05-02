/*
 *  Blob Real Position Estimator
* 
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once


class PositionEstimator 
{
public:
    PositionEstimator();
    ~PositionEstimator();

    void setTransformParameters(const std::vector<CvPoint2D32f> & src,
        const std::vector<CvPoint2D32f> & dest);
    
    CvPoint2D32f operator()(const CvPoint2D32f &);

protected:
    CvMat* _perspectiveTransMat;
};