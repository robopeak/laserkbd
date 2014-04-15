/*
 *  Laser Keyboard
 *  Main Entry
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include <sys/stat.h>
#include <sys/types.h>

#include "common.h"
#include "cv_common.h"

#include "keyboard_emu/keyboard_emu.h"
#include "sketchpad/sketch_pad.h"
#include "cvpipeline.h"
#include "resource_mgr.h"
#include "config_mgr.h"
#include "model_selector.h"
#include "camera_selector.h"
#include "port/common/updatechecker.h"
#include "port/common/productid.h"

using namespace std;

PowerVideoCapture*   cam_input = NULL;
ResourceMgr          g_resouce_mgr;
ConfigMgr            g_config_mgr;
ConfigBundle         g_config_bundle(g_config_mgr);


bool dev_connect(int cameraID)
{
    printf("Capture video signal from camera ID: %d...", cameraID);
 

    cam_input = PowerVideoCapture::CreateCaptureByID(cameraID);

    if (!cam_input){
        printf("Error.\n  Cannot connect to the camera.\n");
        return false;
    }

    // set the resolution and exposure rate
    if (!cam_input->setImageSize(SRC_IMG_WIDTH, SRC_IMG_HEIGHT)) {
        printf("Error.\n Cannot set the camera frame size to %dx%d.\n"
            , SRC_IMG_WIDTH, SRC_IMG_HEIGHT);
        return false;
    }

    if (!cam_input->setExposureVal(g_config_bundle.exposure_level)) {
        printf("Warning.\n Cannot set the camera exposure level to %d.\n"
            , CAM_EXPOSURE_LEVEL);
        //return false;
    }

    printf("OK\n");
    return true; 
}


void dev_disconnect()
{
    delete cam_input;
}


void show_help(int argc, char* argv[])
{
    printf("Laser Keyboard Client.\n"
           "Usage :\n"
           "%1 [options] [camera id]\n\n"
           "Options:\n"
           " -m  <working mode>       Working mode\n"
           "                          Valid values are calib, keyboard, sketch\n"
           "                          Default value is keyboard.\n"
           , argv[0]);
}


#ifndef KEYBOARD_LAYOUT_DEBUG

#ifdef _MACOS
int cpp_entry(int argc, char * argv[])
#else
int main (int argc, char * argv[] )
#endif
{
    int ans = 0;
    int camera_id = -1;
    bool camid_user_specified = false;
    int model_id = 0;
    bool modelid_user_specified = false;
    VisionPipeLine::working_mode_t working_mode = VisionPipeLine::MODE_KEYBOARD; 
    bool opt_show_help = false;
    int pos = 1;

    // create the calibration result folder if it isn't existed.
    std::string config_path;
    config_path = getPlatformConfigPrefix();
    config_path += FILEPATH_LOC_CALIB_FOLDER;
    mkdir(config_path.c_str(), 0755);

    g_config_mgr.loadConfigFromFile();
    g_config_bundle.load();

    camera_id = g_config_bundle.default_cam_id;

    while (pos < argc) {
        char * current_opt = argv[pos];

        if (strcmp(current_opt, "-h") == 0 || strcmp(current_opt, "--help") == 0 ) {
            opt_show_help = true;
        } else if (strcmp(current_opt, "-m") == 0 ) {
            const char * modestr = argv[++pos];
            if (strcmp(modestr, "sketch") == 0) {
                working_mode = VisionPipeLine::MODE_SKETCHPAD;
            }  else if (strcmp(modestr, "calib") == 0) {
                working_mode = VisionPipeLine::MODE_CALIBRATION;
            }
        }  else {
            camera_id = atoi(argv[pos]);
            camid_user_specified = true;
        }
        ++pos;
    }

    if (opt_show_help) {
        show_help(argc, argv);
        return 0;
    }

    ModelSelector modelsel;
    modelsel.setPredefinedModel(model_id, modelid_user_specified);
    model_id = modelsel.doModal();
    g_config_bundle.model_id = model_id;

    if (model_id == 1) {
        if (g_config_bundle.exposure_level > -9) g_config_bundle.exposure_level = -9;
    }

    CameraSelector camsel;
    camsel.setPredefinedCamera(camera_id, camid_user_specified);

    camera_id = camsel.doModal();
    if (camera_id == -1) return 0;

  

    if (!dev_connect(camera_id)) {
        printf("error, cannot connect to the device. \n");
        return -2;
    }


    UpdateChecker::GetInstance()->sendQueryRequest(IsTargetProductPresent()?RELEASE_VENDOR_TYPE:"3rd", RELEASE_PRODUCT_TYPE, RELEASE_VERSION_INFO);
   

    

    g_config_bundle.default_cam_id = camera_id;
    
    // flush config
    g_config_bundle.flush();
    g_config_mgr.flushConfigFile();

    VisionPipeLine pipeline(cam_input);

    if (pipeline.init()) {
        pipeline.setWorkingMode(working_mode);

        std::string updateurl;
        bool update_checked = false;
        while(pipeline.heartBeat() && !checkPlatformExitFlag()) {
            if (UpdateChecker::GetInstance()->isResponseReady() && !update_checked) {
                update_checked = true;
                if (UpdateChecker::GetInstance()->getResponseUrl(updateurl)) {
                    pipeline.setUpgradeAvailable(updateurl);
                }
            }
        }
         
        
    }

    dev_disconnect();
    return ans;
}

#else

int main (int argc, char * argv[] ) 
{
static const int KBD_IMG_HEIGHT = 300;
static const int KBD_IMG_WIDTH = 800;


    IplImage* _kdb_img = cvCreateImage(cvSize(KBD_IMG_WIDTH,KBD_IMG_HEIGHT), 8, 3);
    
    ::cvNamedWindow("Keyboard");
    
    KeyLayoutProvider  _provider;
    
    std::vector<int> empty;
    _provider.renderLayout(_kdb_img, empty);

    int ans =  _provider.findKeyIdxByPoint(53,20);
    printf("%d\n", ans);

    cvShowImage("Keyboard", _kdb_img);
    ::cvWaitKey(0);
    cvReleaseImage(&_kdb_img);
}

#endif
