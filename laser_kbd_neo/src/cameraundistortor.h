/* 
 *  Camera Distortion Compensation Logic
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

// OpenCV is required

class CameraUndistortHelper
{
public:
    CameraUndistortHelper(int sx, int sy, int dx, int dy);
    ~CameraUndistortHelper();

    bool loadCalibrationfromFile(const char * camerainfofile, const char * cameradistortfile);
    void undistortImage(const IplImage * input, IplImage * output);

protected:

    bool    _is_loaded;
    int     _frame_sx, _frame_sy;
    int     _frame_dx, _frame_dy;
    CvMat * _intrinsic_cam;
    CvMat * _intrinsic_distort;
    CvMat * _new_cam_matrix;
    CvMat *  _cam_map_x, *_cam_map_y;
};