/* 
 *  Camera Distortion Compensation Logic
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"

#define SAFE_MAT_RELEASE(p) \
    do {if (p) cvReleaseMat(&(p));}while(0)

CameraUndistortHelper::CameraUndistortHelper(int sx, int sy, int dx, int dy)
: _is_loaded(false)
, _frame_sx(sx)
, _frame_sy(sy)
, _frame_dx(dx)
, _frame_dy(dy)
, _intrinsic_cam(NULL)
, _intrinsic_distort(NULL)
, _new_cam_matrix(NULL)
, _cam_map_x(NULL)
, _cam_map_y(NULL)
{
    _cam_map_x = cvCreateMat(_frame_dy, _frame_dx, CV_32F);
    _cam_map_y = cvCreateMat(_frame_dy, _frame_dx, CV_32F);
    _new_cam_matrix = cvCreateMat(3,3,CV_32F);
}

CameraUndistortHelper::~CameraUndistortHelper()
{
    SAFE_MAT_RELEASE(_intrinsic_cam);
    SAFE_MAT_RELEASE(_intrinsic_distort);
    SAFE_MAT_RELEASE(_cam_map_x);
    SAFE_MAT_RELEASE(_cam_map_y);
    SAFE_MAT_RELEASE(_new_cam_matrix);
}

bool CameraUndistortHelper::loadCalibrationfromFile(const char * camerainfofile, const char * cameradistortfile){
    SAFE_MAT_RELEASE(_intrinsic_cam);
    SAFE_MAT_RELEASE(_intrinsic_distort);
    _is_loaded = false;
    _intrinsic_cam = (CvMat*)cvLoad(camerainfofile);
    _intrinsic_distort  = (CvMat*)cvLoad(cameradistortfile);

    if (!_intrinsic_cam || !_intrinsic_distort) return false;

    cvGetOptimalNewCameraMatrix(_intrinsic_cam, _intrinsic_distort,
        cvSize(_frame_sx, _frame_sy), 1, _new_cam_matrix, cvSize(_frame_dx, _frame_dy));

    cvInitUndistortRectifyMap(_intrinsic_cam, _intrinsic_distort, NULL, 
        _new_cam_matrix, _cam_map_x, _cam_map_y);

    _is_loaded = true;
    return true;
}

void CameraUndistortHelper::undistortImage(const IplImage * input, IplImage * output)
{
    if (!_is_loaded) {
        // scale the source to the target direct
        ::cvResize(input, output);
        return;
    }
    cvRemap(input, output, _cam_map_x, _cam_map_y);
}
