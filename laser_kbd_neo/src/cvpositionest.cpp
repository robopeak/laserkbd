/*
 *  Blob Real Position Estimator
* 
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"

PositionEstimator::PositionEstimator()
: _perspectiveTransMat(NULL)
{
    _perspectiveTransMat = cvCreateMat(3,3,CV_32FC1);
}

PositionEstimator::~PositionEstimator()
{
    if (_perspectiveTransMat) ::cvReleaseMat(&_perspectiveTransMat);
}

void PositionEstimator::setTransformParameters(const std::vector<CvPoint2D32f> & src,
    const std::vector<CvPoint2D32f> & dest)

{
    assert(src.size() == dest.size());
    if (src.size() < 4) return;
    
    CvMat * src_ptList = cvCreateMat(src.size(),1,CV_32FC2);
    CvMat * dest_ptList = cvCreateMat(dest.size(),1,CV_32FC2);
    

    for (int pos = 0; pos < src.size(); pos++) {
        CV_MAT_ELEM(*src_ptList, CvPoint2D32f,pos, 0)  = src[pos];
        CV_MAT_ELEM(*dest_ptList, CvPoint2D32f,pos, 0)  = dest[pos];
    }

    cvFindHomography(src_ptList, dest_ptList, _perspectiveTransMat);

    cvReleaseMat(&src_ptList);
    cvReleaseMat(&dest_ptList);

}

CvPoint2D32f PositionEstimator::operator()(const CvPoint2D32f & src)
{
    CvMat * srcVec =cvCreateMat(1,1, CV_32FC2);
    CvMat * destVec = cvCreateMat(1,1, CV_32FC2);
    
    
    CV_MAT_ELEM(*srcVec, CvPoint2D32f,0, 0)  = src;
    cvPerspectiveTransform(srcVec, destVec, _perspectiveTransMat);



    CvPoint2D32f ans = CV_MAT_ELEM(*destVec, CvPoint2D32f,0, 0);

    cvReleaseMat(&srcVec);
    cvReleaseMat(&destVec);

    return ans;
}