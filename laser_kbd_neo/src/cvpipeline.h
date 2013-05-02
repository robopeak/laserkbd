/*
 *  Laser Keyboard
 *  CV system, image pipeline
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

#include "cvui/cvui.h"

struct CalibratonData {
    int  calibration_stage;
    bool isAvail;
    std::vector<CvPoint2D32f> bindedPoints;
};

class VisionPipeLine {

public:
    enum working_mode_t {
        MODE_UNKNOWN     = 0,
        MODE_CALIBRATION = 1,
        MODE_KEYBOARD    = 2,
        MODE_SKETCHPAD   = 3,

    };


    VisionPipeLine(PowerVideoCapture * input);
    ~VisionPipeLine();

    bool init();
    bool heartBeat();

    void setWorkingMode(working_mode_t newmode);
    void setUpgradeAvailable(const std::string &url);

protected:
    bool stage_grabFrame();
    bool stage_preProcessing();
    bool stage_workingModeTick();
    bool stage_postProcessing();
    bool stage_display();

    bool stage_findingBlob();
    bool stage_localization();

    void draw_markBlobs();


    void  onEnterCalibrationStage();
    void  onLeaveCalibrationStage();

    void  calibrationPostStage();
    bool  loadLocalizationCalibrationData();
    bool  storeLocalizationCalibrationData();

    void  onCalibrationReady();

    void  onRenderKeyboard();

protected:
    PowerVideoCapture * _input;

    CvSize     _currentframeSz;
    IplImage*  _currentFrame;
    IplImage*  _grayFrame;
    IplImage*  _rectified;
    IplImage*  _projected;

    IplImage*  _final_disp;

    std::vector<Blob_t>      _bloblist;
    std::vector<TrackPoint>  _trackpoints;

    CameraUndistortHelper _undistortor;
    KeyLayoutProvider     _layout_provider;
    PositionEstimator     _posEstimator;
    CalibratonData        _calibrationData;

    std::vector<int>      _calibration_keylist;

    void                  _onMouseMove(int x, int y);
    void                  _onLMouseDown(int x, int y);

    static void s_onMouse(int mouse_event, int x, int y,int, void* pThis);
    CvPoint               _lastMousePoint;
    int                   _hoveredBlobID;
    bool                  _noLenCalibration;

    static void           s_on_keyboardmode(void * data, int button, int x, int y);
    static void           s_on_touchpadmode(void * data, int button, int x, int y);
    static void           s_on_calibrationmode(void * data, int button, int x, int y);
    static void           s_on_rplogo_clicked(void * data, int button, int x, int y);
    static void           s_on_upgrade_clicked(void * data, int button, int x, int y);



protected:

    rp::cvui::RenderContainer _uicontainer;
    rp::cvui::ImageFrame      _ui_banner_frame;
    rp::cvui::Button          _ui_btn_keyboard_mode;
    rp::cvui::Button          _ui_btn_touchpad_mode;
    rp::cvui::Button          _ui_btn_calib_mode;
    rp::cvui::Button          _ui_btn_rplogo;
    rp::cvui::Button          _ui_btn_upgrade;

    KeyboardEmu _kbd_emu;
    SketchPad   _sketch_pad;

    working_mode_t _currentWorkingMode;

    std::string    _upgrade_url;
};