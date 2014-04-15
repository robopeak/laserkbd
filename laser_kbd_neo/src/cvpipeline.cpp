/*
 *  Laser Keyboard
 *  CV system, image pipeline
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"



#include "keyboard_emu/keyboard_emu.h"
#include "sketchpad/sketch_pad.h"
#include "cvpipeline.h"
#include "resource_mgr.h"
#include "config_mgr.h"

static const char * WINDOWNAME_RECTIFIED = "rectified";
static const char * WINDOWNAME_ORIGINAL = "original";
static const char * WINDOWNAME_PROJECTED = "projected";
static const char * WINDOWNAME_STATUS = "Laser Keyboard Console";

static const float SCALED_SRC_IMG_WIDTH = PROJECTED_WIDTH;
static const float SCALED_SRC_IMG_HEIGHT = (SCALED_SRC_IMG_WIDTH/SRC_IMG_WIDTH) * SRC_IMG_HEIGHT;

static const int UI_BANNER_HEIGHT = 74;

extern ResourceMgr          g_resouce_mgr;
extern ConfigBundle         g_config_bundle;


enum {
    CALIBRATION_STAGE_NONE = 0,
    CALIBRATION_STAGE_WORKING = 1,
    CALIBRATION_STAGE_REQUIRED = 2,
};

VisionPipeLine::VisionPipeLine(PowerVideoCapture * input)
: _input(input)
, _currentFrame(NULL)
, _grayFrame(NULL)
, _rectified(NULL)
, _projected(NULL)
, _final_disp(NULL)
, _undistortor(SRC_IMG_WIDTH,SRC_IMG_HEIGHT,RECTIFIED_IMG_W,RECTIFIED_IMG_H)
, _hoveredBlobID(-1)
, _noLenCalibration(false)
, _kbd_emu(_layout_provider)
, _sketch_pad(_layout_provider)
, _currentWorkingMode(MODE_UNKNOWN)
{
    _calibrationData.calibration_stage = CALIBRATION_STAGE_NONE;
    _calibrationData.isAvail = false;


#define APPEND_CALIBRATION_KEY(_ch_) \
    _calibration_keylist.push_back( _layout_provider.getKeyIdxByKeyVal(_ch_))

    APPEND_CALIBRATION_KEY('1');
    APPEND_CALIBRATION_KEY('5');
    APPEND_CALIBRATION_KEY('9');
    APPEND_CALIBRATION_KEY(VK_OEM_PLUS);
    APPEND_CALIBRATION_KEY(VK_CAPITAL);
    APPEND_CALIBRATION_KEY('D');
    APPEND_CALIBRATION_KEY('J');
    APPEND_CALIBRATION_KEY(VK_RETURN);
    APPEND_CALIBRATION_KEY(VK_CONTROL);
    APPEND_CALIBRATION_KEY(VK_OEM_4);
    APPEND_CALIBRATION_KEY(VK_DELETE);
    APPEND_CALIBRATION_KEY(VK_RIGHT);
}

VisionPipeLine::~VisionPipeLine()
{

    _uicontainer.clearAllObjects();

    cvDestroyWindow(WINDOWNAME_STATUS);

    if (_grayFrame) ::cvReleaseImage(&_grayFrame);
    if (_rectified) ::cvReleaseImage(&_rectified);
    if (_projected) ::cvReleaseImage(&_projected);
    if (_final_disp) ::cvReleaseImage(&_final_disp);
}


bool VisionPipeLine::init()
{
    // query the first frame
    _currentFrame = _input->retrieveFrame();
    if (!_currentFrame) return false;

    _currentframeSz.height = _currentFrame->height;
    _currentframeSz.width = _currentFrame->width;

    if (_currentframeSz.height != SRC_IMG_HEIGHT || _currentframeSz.width != SRC_IMG_WIDTH)
    {
        printf("Please set your camera to the resolution %dx%d\n", SRC_IMG_WIDTH, SRC_IMG_HEIGHT);
        return false;
    }

    std::string filepath_cam = getPlatformConfigPrefix();
    std::string filepath_distort = getPlatformConfigPrefix();
    filepath_cam+=FILEPATH_CAMERA_INTRINSICS;
    filepath_distort+=FILEPATH_CAMERA_DISTORT;
    try {
        
        if (!_undistortor.loadCalibrationfromFile(filepath_cam.c_str(), filepath_distort.c_str()))
        {
            // no user defined calib data found, load predefined
            filepath_cam = getPlatformResPrefix();
            filepath_distort = getPlatformResPrefix();

            filepath_cam+= FILEPATH_RESOURCE_PREDEFINE_FOLDER;
            filepath_distort+= FILEPATH_RESOURCE_PREDEFINE_FOLDER;

            filepath_cam+= RELEASE_VENDOR_TYPE;
            filepath_distort+= RELEASE_VENDOR_TYPE;

            filepath_cam+= "/";
            filepath_distort+="/";

            if (g_config_bundle.model_id == 1) {
                filepath_cam+= FILEPATH_CONFIG_NEWMODEL_PREFIX;
                filepath_distort+= FILEPATH_CONFIG_NEWMODEL_PREFIX;
            }

            filepath_cam+= FILEPATH_PREDEFINE_CAMERA_INTRINSICS;
            filepath_distort+= FILEPATH_PREDEFINE_CAMERA_DISTORT;
            if (!_undistortor.loadCalibrationfromFile(filepath_cam.c_str(), filepath_distort.c_str()))
            {
                printf("warning, no camera calibration file found.\n");
                _noLenCalibration = true;
            }
        }
    } 
    catch(...){
        printf("warning, unexpected error happens during loading the camera calibration files.\n");
        _noLenCalibration = true;
    }

    _grayFrame = cvCreateImage(cvSize(RECTIFIED_IMG_W,RECTIFIED_IMG_H), 8, 1);
    _rectified = cvCreateImage(cvSize(RECTIFIED_IMG_W,RECTIFIED_IMG_H), 8, 3);
    _projected = cvCreateImage(cvSize(PROJECTED_WIDTH,PROJECTED_HEIGHT), 8, 3);
    
    
    _final_disp = cvCreateImage(cvSize(PROJECTED_WIDTH + SRC_IMG_WIDTH, SRC_IMG_HEIGHT + KBD_IMG_HEIGHT ), 8, 3);

    //cvNamedWindow(WINDOWNAME_RECTIFIED);
    //cvNamedWindow(WINDOWNAME_ORIGINAL);
    //cvNamedWindow(WINDOWNAME_PROJECTED);
    cvNamedWindow(WINDOWNAME_STATUS);

    cvSetMouseCallback(WINDOWNAME_STATUS, &VisionPipeLine::s_onMouse, this);
   
    
    _ui_banner_frame.setImage(g_resouce_mgr.getImageRes("main_banner.png"));
    _ui_btn_keyboard_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.keyboard.png"));
    _ui_btn_keyboard_mode.setActiveImage(g_resouce_mgr.getImageRes("btn.keyboard.active.png"));

    _ui_btn_touchpad_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.pad.png"));
    _ui_btn_touchpad_mode.setActiveImage(g_resouce_mgr.getImageRes("btn.pad.active.png"));

    _ui_btn_calib_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.calibration.png"));
    _ui_btn_calib_mode.setActiveImage(g_resouce_mgr.getImageRes("btn.calibration.active.png"));

    _ui_btn_keyboard_mode.moveTo((_final_disp->width-(_ui_btn_keyboard_mode.getWidth()+5)*3)/2,
        (_ui_banner_frame.getHeight() - _ui_btn_keyboard_mode.getHeight())/2);

    _ui_btn_touchpad_mode.moveTo(_ui_btn_keyboard_mode.getRight()+5, _ui_btn_keyboard_mode.getY());
    _ui_btn_calib_mode.moveTo(_ui_btn_touchpad_mode.getRight()+5, _ui_btn_keyboard_mode.getY());


    _ui_btn_rplogo.setNormalImage(g_resouce_mgr.getImageRes("logobtn.png"));
    _ui_btn_rplogo.setActiveImage(g_resouce_mgr.getImageRes("logobtn.active.png"));

    _ui_btn_upgrade.setNormalImage(g_resouce_mgr.getImageRes("btn.update.png"));
    _ui_btn_upgrade.setActiveImage(g_resouce_mgr.getImageRes("btn.update.active.png"));


    _ui_btn_rplogo.moveTo(_final_disp->width-_ui_btn_rplogo.getWidth(), 0);
    _ui_btn_upgrade.moveTo(0, 0);

    _ui_btn_upgrade.setVisible(false);

    _uicontainer.addRenderObject(&_ui_banner_frame);
    _uicontainer.addRenderObject(&_ui_btn_keyboard_mode);
    _uicontainer.addRenderObject(&_ui_btn_touchpad_mode);
    _uicontainer.addRenderObject(&_ui_btn_calib_mode);
    
    _uicontainer.addRenderObject(&_ui_btn_rplogo);
    _uicontainer.addRenderObject(&_ui_btn_upgrade);

    _ui_btn_keyboard_mode.setListener(&VisionPipeLine::s_on_keyboardmode, this);
    _ui_btn_touchpad_mode.setListener(&VisionPipeLine::s_on_touchpadmode, this);
    _ui_btn_calib_mode.setListener(&VisionPipeLine::s_on_calibrationmode, this);

    _ui_btn_rplogo.setListener(&VisionPipeLine::s_on_rplogo_clicked, this);
    _ui_btn_upgrade.setListener(&VisionPipeLine::s_on_upgrade_clicked, this);

    if (!loadLocalizationCalibrationData()) {
        setWorkingMode(MODE_CALIBRATION);
    }
    return true;
}

bool VisionPipeLine::heartBeat()
{
    return  stage_grabFrame() 
         && stage_preProcessing()
         && stage_workingModeTick()
         && stage_postProcessing()
         && stage_display();
}


void VisionPipeLine::setWorkingMode(working_mode_t newmode)
{
    if (!_calibrationData.isAvail) {
        // force entering calibration mode
        newmode = MODE_CALIBRATION;
    }

    if (newmode == MODE_CALIBRATION && _currentWorkingMode!=newmode) {
        onEnterCalibrationStage();
    }

    if (_currentWorkingMode!=newmode) {
        _currentWorkingMode = newmode;
        switch(newmode) {
        case MODE_KEYBOARD:
            _ui_btn_keyboard_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.keyboard.active.png"));
            _ui_btn_touchpad_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.pad.png"));
            _ui_btn_calib_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.calibration.png"));

            break;
        case MODE_SKETCHPAD:
            _ui_btn_keyboard_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.keyboard.png"));
            _ui_btn_touchpad_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.pad.active.png"));
            _ui_btn_calib_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.calibration.png"));

            break;
        case MODE_CALIBRATION:
            _ui_btn_keyboard_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.keyboard.png"));
            _ui_btn_touchpad_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.pad.png"));
            _ui_btn_calib_mode.setNormalImage(g_resouce_mgr.getImageRes("btn.calibration.active.png"));

            break;
        }
    }
}


bool VisionPipeLine::stage_grabFrame()
{
    _currentFrame = _input->retrieveFrame();
    if (!_currentFrame) return false;
    return true;
}

bool VisionPipeLine::stage_preProcessing()
{
    // undistort and rectify the image
    _undistortor.undistortImage(_currentFrame, _rectified);
    
    // convert to gray level 
    cvCvtColor(_rectified,_grayFrame,CV_BGR2GRAY);
    // LPF
    cvSmooth( _grayFrame, _grayFrame, CV_GAUSSIAN, 5, 5 );

    // To Binary
    cvThreshold(_grayFrame, _grayFrame, BLOB_THRESHOLD_PWR, 255, CV_THRESH_TOZERO);
    //     cvShowImage("gray", gray);

    
    return stage_findingBlob()
        && stage_localization();
}

bool VisionPipeLine::stage_workingModeTick()
{
    switch (_currentWorkingMode) 
    {
    case MODE_CALIBRATION:
        break;

    case MODE_SKETCHPAD:
        _sketch_pad.onSketchPadTick(_trackpoints);
    case MODE_KEYBOARD:
        _kbd_emu.onKeyboardTick(_trackpoints, _currentWorkingMode==MODE_KEYBOARD?true:false);
        break;
    }

    return true;
}

void  VisionPipeLine::onRenderKeyboard()
{
    cvSetImageROI(_final_disp, cv::Rect(SCALED_SRC_IMG_WIDTH, SRC_IMG_HEIGHT + UI_BANNER_HEIGHT, KBD_IMG_WIDTH, KBD_IMG_HEIGHT ));
    
    if (_currentWorkingMode == MODE_CALIBRATION) {
        std::vector<int> flashedbtn;

        if (!(time(NULL) % 2)) {
            
            if (_calibrationData.bindedPoints.size() < _calibration_keylist.size())
                flashedbtn.push_back(_calibration_keylist[_calibrationData.bindedPoints.size()]);
                  

        }

         _layout_provider.renderLayout(_final_disp, flashedbtn);
    } else {
        _layout_provider.renderLayout(_final_disp, _kbd_emu.getPressedKeyIdx());
    }    
    cvResetImageROI(_final_disp);  
}

bool VisionPipeLine::stage_postProcessing()
{
    draw_markBlobs();

    if (_currentWorkingMode == MODE_CALIBRATION) calibrationPostStage();

    cv_copyTo(_currentFrame, _final_disp, cv::Rect(0, UI_BANNER_HEIGHT, SCALED_SRC_IMG_WIDTH,  SCALED_SRC_IMG_HEIGHT));
    cv_copyTo(_rectified, _final_disp, cv::Rect(SCALED_SRC_IMG_WIDTH, UI_BANNER_HEIGHT, SRC_IMG_WIDTH, SRC_IMG_HEIGHT ));
    
    cv_copyTo(_projected, _final_disp, cv::Rect(0, (UI_BANNER_HEIGHT + _final_disp->height + SCALED_SRC_IMG_HEIGHT - PROJECTED_HEIGHT)/2, PROJECTED_WIDTH, PROJECTED_HEIGHT ));

    onRenderKeyboard();

    // draw Window Description

    cv_textOut(_final_disp, 2, 10 + UI_BANNER_HEIGHT, WINDOWNAME_ORIGINAL);


    cv_textOut(_final_disp, SCALED_SRC_IMG_WIDTH + 2, 10 + UI_BANNER_HEIGHT, WINDOWNAME_RECTIFIED);

    cv_textOut(_final_disp, 2, SCALED_SRC_IMG_HEIGHT + 10 + UI_BANNER_HEIGHT, WINDOWNAME_PROJECTED);

    // draw Rect as boundary 
    cvDrawRect(_final_disp, cvPoint(0,UI_BANNER_HEIGHT), cvPoint(SCALED_SRC_IMG_WIDTH, SCALED_SRC_IMG_HEIGHT+UI_BANNER_HEIGHT)
        , cvScalar(255,255,0));

    cvDrawRect(_final_disp, cvPoint(SCALED_SRC_IMG_WIDTH,UI_BANNER_HEIGHT), cvPoint(SCALED_SRC_IMG_WIDTH + SRC_IMG_WIDTH, SRC_IMG_HEIGHT + UI_BANNER_HEIGHT)
        , cvScalar(255,255,0));

    ::cvLine(_final_disp, cvPoint(SCALED_SRC_IMG_WIDTH,UI_BANNER_HEIGHT), cvPoint(SCALED_SRC_IMG_WIDTH,_final_disp->height+UI_BANNER_HEIGHT)
        , cvScalar(255,255,0, 0));
    return true;
}

bool VisionPipeLine::stage_display()
{
   // cvShowImage(WINDOWNAME_ORIGINAL, _currentFrame);
  //  cvShowImage(WINDOWNAME_RECTIFIED, _rectified);
   // cvShowImage(WINDOWNAME_PROJECTED, _projected);

    _uicontainer.renderObjects(_final_disp);

    cvShowImage(WINDOWNAME_STATUS, _final_disp);
    cvSetMouseCallback(WINDOWNAME_STATUS, &VisionPipeLine::s_onMouse, this);

    int key = cvWaitKey(1);
    if (key == VK_ESCAPE)
        return false;
    return true;
}

bool VisionPipeLine::stage_findingBlob()
{
    FindBlobs(_grayFrame, _bloblist, BLOB_MIN_SIZE, BLOB_MAX_SIZE);

    // test whether a blob is pointed by the user mouse

    _hoveredBlobID = -1;
    for (int pos = 0; pos < _bloblist.size(); ++pos) {
        if ( _bloblist[pos]._box.contains(_lastMousePoint)) {
            _hoveredBlobID = pos;
            break;
        }
    }

    return true;
}

bool VisionPipeLine::stage_localization()
{
    _trackpoints.clear();
    for (int pos =0; pos<_bloblist.size() && pos<MAX_TRACKED_BLOB_CNT; ++pos)
    {
        CvPoint2D32f destPos = _posEstimator(_bloblist[pos]._center);
        CvPoint2D32f destPosR = _posEstimator(cvPoint2D32f(_bloblist[pos]._center.x, _bloblist[pos]._center.y + sqrt(_bloblist[pos]._area)));
        float pressure = abs(destPosR.y - destPos.y);

        if (pressure < 5) continue;

        _trackpoints.push_back( TrackPoint(destPos.x, destPos.y, pressure));
    
    }

    return true;
}


void VisionPipeLine::draw_markBlobs()
{
    // draw founded blobs
    for (int pos = 0; pos < _bloblist.size() && pos < MAX_TRACKED_BLOB_CNT; ++pos)
    {
        if (pos == _hoveredBlobID) {
            cvDrawRect(_rectified, cvPoint(_bloblist[pos]._box.x, _bloblist[pos]._box.y), 
            cvPoint(_bloblist[pos]._box.x+_bloblist[pos]._box.width, _bloblist[pos]._box.y+_bloblist[pos]._box.height), 
            cvScalar(0,0,255), 3);
        } else {
            cvDrawRect(_rectified, cvPoint(_bloblist[pos]._box.x, _bloblist[pos]._box.y), 
            cvPoint(_bloblist[pos]._box.x+_bloblist[pos]._box.width, _bloblist[pos]._box.y+_bloblist[pos]._box.height), 
            cvScalar(0,255,255));
        }

        

        cvDrawCircle(_rectified, cvPoint(_bloblist[pos]._center.x, _bloblist[pos]._center.y), 
            2, cvScalar(255,255,0));

        char msg[100];
        sprintf(msg, "%d: (%.2f, %.2f)", pos, _bloblist[pos]._center.x, _bloblist[pos]._center.y);
        cv_textOut(_rectified, _bloblist[pos]._box.x, _bloblist[pos]._box.y+_bloblist[pos]._box.height+10, msg);
    }

    // draw blob marks on the rectified projection plane
    cvFillImage(_projected, 0);

    char msg[100];

    for (int pos =0; pos<_trackpoints.size(); ++pos)
    {        
        CvPoint2D32f posInWindow = _layout_provider.keyboardPosToWindowPos(cvPoint2D32f(_trackpoints[pos].x, _trackpoints[pos].y));
        posInWindow.x *= (float)PROJECTED_WIDTH/_layout_provider.getLayoutSize().width ;
        posInWindow.y *= (float)PROJECTED_WIDTH/_layout_provider.getLayoutSize().width ;
        
        posInWindow.y += (PROJECTED_HEIGHT-_layout_provider.getLayoutSize().height)/2;
        cvDrawCircle(_projected, cvPoint(posInWindow.x, posInWindow.y), _trackpoints[pos].pressure, cvScalar(255,255,0));
        sprintf(msg, "%d:(%.2f,%.2f)", pos, posInWindow.x, posInWindow.y);
        cv_textOut(_projected, posInWindow.x,posInWindow.y+20, msg);
    }
}


void  VisionPipeLine::onEnterCalibrationStage(){
    _calibrationData.isAvail =false;
    _calibrationData.bindedPoints.clear();
    _calibrationData.calibration_stage = CALIBRATION_STAGE_WORKING;
}

void  VisionPipeLine::onLeaveCalibrationStage(){
    onCalibrationReady();
    storeLocalizationCalibrationData();
    setWorkingMode(MODE_KEYBOARD);
}

void  VisionPipeLine::calibrationPostStage() {
    // draw a flashing frame to notify the user
    int flash_bit = time(NULL) % 2;
    if (flash_bit) {
        cvDrawRect(_rectified, cvPoint(3,3), cvPoint(_rectified->width-2, _rectified->height-2),
            cvScalar(0,0,255), 3);

        cv_textOut(_rectified, 40, 40, "CALIBRARION IN PROGRESS", cvScalar(0, 255,255), 3);
    }
    cv_textOut(_rectified, 40, 60, "1. Place your finger to the location indicated by the flashing key button" ,cvScalar(255, 255,255), 1.8);
    cv_textOut(_rectified, 40, 80, "2. Click the related blob.",cvScalar(255, 255,255), 1.8);
}

void  VisionPipeLine::onCalibrationReady()
{
    _calibrationData.isAvail = true;

    std::vector<CvPoint2D32f> destPts;

    for (int pos=0; pos < _calibration_keylist.size(); ++pos) {
        destPts.push_back(cvPoint2D32f( _layout_provider.getKeyAt(_calibration_keylist[pos])->x, 
        _layout_provider.getKeyAt(_calibration_keylist[pos])->y));
    }

  
    _posEstimator.setTransformParameters(_calibrationData.bindedPoints, destPts);

}

bool  VisionPipeLine::loadLocalizationCalibrationData() {
    CvMat * calibMat;
    std::string filepath = getPlatformConfigPrefix();
    filepath+= FILEPATH_LOC_CALIB_FOLDER;
    if (g_config_bundle.model_id == 1) {
        filepath+= FILEPATH_CONFIG_NEWMODEL_PREFIX;
    }

    filepath+=FILEPATH_LOC_CALIB_DATA;
    try {
        calibMat = (CvMat*)cvLoad(filepath.c_str());

    } catch(...) {
        printf("Cannot load the localization calibration data.\n"
            "Invalid file format.\n");
        return false;
    }

    if (!calibMat) {
        printf("Cannot load the localization calibration data.\n");
        return false;
    }

    cv::Mat_<float> mat = (cv::Mat_<float>)calibMat;
    if (mat.rows != _calibration_keylist.size() || mat.cols != 2) {
        printf("Invalid localization calibration data format.\n");
        cvReleaseMat(&calibMat);
        return false;
    }

    _calibrationData.bindedPoints.clear();
    for (int pos =0; pos < _calibration_keylist.size(); ++pos) {
        CvPoint2D32f pt;
        pt.x = mat(pos,0);
        pt.y = mat(pos,1);
        _calibrationData.bindedPoints.push_back(pt);
    }
    cvReleaseMat(&calibMat);
    onCalibrationReady();
    return true;
}

bool  VisionPipeLine::storeLocalizationCalibrationData() {
    if (_calibrationData.bindedPoints.size() < 4) {
        return false;
    }

    cv::Mat_<float> mat(_calibrationData.bindedPoints.size(),2);

    for (int pos = 0; pos < _calibrationData.bindedPoints.size() ;pos ++) {
        mat(pos, 0) = _calibrationData.bindedPoints[pos].x;
        mat(pos, 1) = _calibrationData.bindedPoints[pos].y;
    }

    std::string filepath = getPlatformConfigPrefix();

    filepath+= FILEPATH_LOC_CALIB_FOLDER;
    if (g_config_bundle.model_id == 1) {
        filepath+= FILEPATH_CONFIG_NEWMODEL_PREFIX;
    }

    filepath+=FILEPATH_LOC_CALIB_DATA;
    try {
        CvMat tmpMat = (CvMat)mat;
        cvSave(filepath.c_str(), &tmpMat);
    } catch (...) {
        printf("Cannot save the localization data to external files.\n");
    }
    return true;
}

void  VisionPipeLine::_onMouseMove(int x, int y)
{
    y-=UI_BANNER_HEIGHT;
    _lastMousePoint.x = (x - SCALED_SRC_IMG_WIDTH)*((float)RECTIFIED_IMG_W/SRC_IMG_WIDTH);
    _lastMousePoint.y = y * ((float)RECTIFIED_IMG_H/SRC_IMG_HEIGHT);
}

void  VisionPipeLine::_onLMouseDown(int x, int y)
{
    if (_currentWorkingMode != MODE_CALIBRATION) return;

    if (_calibrationData.calibration_stage != CALIBRATION_STAGE_WORKING) return;

    if (_hoveredBlobID != -1) {
        CvPoint2D32f pt = cvPoint2D32f(_bloblist[_hoveredBlobID]._center.x,_bloblist[_hoveredBlobID]._center.y);
        _calibrationData.bindedPoints.push_back(pt);
        if (_calibrationData.bindedPoints.size() == _calibration_keylist.size()) {
            onLeaveCalibrationStage();
        }

    }
}


void VisionPipeLine::s_onMouse(int mouse_event, int x, int y,int, void* pThis)
{
    VisionPipeLine * This = reinterpret_cast<VisionPipeLine *>(pThis);

    switch (mouse_event) {
        case CV_EVENT_LBUTTONDOWN:
            This->_onLMouseDown(x,y);
            This->_uicontainer.handleMousePress(1, x, y);
            break;
        case CV_EVENT_MOUSEMOVE:
            This->_onMouseMove(x,y);
            This->_uicontainer.handleMouseMove(1, x, y);
            break;
        case CV_EVENT_LBUTTONUP:
            This->_uicontainer.handleMouseRelease(1, x, y);
            break;
    }
}

void VisionPipeLine::s_on_keyboardmode(void * data, int button, int x, int y)
{
    VisionPipeLine * This = reinterpret_cast<VisionPipeLine *>(data);
    This->setWorkingMode(MODE_KEYBOARD);
}

void VisionPipeLine::s_on_touchpadmode(void * data, int button, int x, int y)
{
    VisionPipeLine * This = reinterpret_cast<VisionPipeLine *>(data);
    This->setWorkingMode(MODE_SKETCHPAD);
}

void VisionPipeLine::s_on_calibrationmode(void * data, int button, int x, int y)
{
    VisionPipeLine * This = reinterpret_cast<VisionPipeLine *>(data);
    This->setWorkingMode(MODE_CALIBRATION);
}

void VisionPipeLine::s_on_rplogo_clicked(void * data, int button, int x, int y)
{
    gotoURL("http://www.robopeak.com");
}

void VisionPipeLine::s_on_upgrade_clicked(void * data, int button, int x, int y)
{
    VisionPipeLine * This = reinterpret_cast<VisionPipeLine *>(data);

    gotoURL(This->_upgrade_url.c_str());
}


void VisionPipeLine::setUpgradeAvailable(const std::string &url)
{
    _upgrade_url = url;
    _ui_btn_upgrade.setVisible(true);
}