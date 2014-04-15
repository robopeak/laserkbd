/*
 *  Laser Keyboard 
 *  Device Configuration
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

// Height of images captured by the camera 
const int SRC_IMG_HEIGHT = 480;
// Width of images captured by the camera 
const int SRC_IMG_WIDTH  = 640;




// Image size of the undistorted image
const int RECTIFIED_IMG_H = SRC_IMG_HEIGHT + 200;
const int RECTIFIED_IMG_W = SRC_IMG_WIDTH + 200;


const int BLOB_MIN_SIZE = 9;
const int BLOB_MAX_SIZE = 100*100;

const int MAX_TRACKED_BLOB_CNT = 10;
const int BLOB_THRESHOLD_PWR = 80;

//Size of the rectified projection image
const int PROJECTED_WIDTH = 500;
const int PROJECTED_HEIGHT = 300;

const int KBD_IMG_HEIGHT = 300;
const int KBD_IMG_WIDTH = SRC_IMG_WIDTH;

const int MAX_SENSE_DESTANCE = PROJECTED_HEIGHT-10;


// desired camera exposure level
const int CAM_EXPOSURE_LEVEL = -5;
const int CAM_EXPOSURE_LEVELv2 = -9;


const int REFIRE_DELAY    = 1000;
const int REFIRE_INTERVAL = 100;

static const char * FILEPATH_PREDEFINE_CAMERA_INTRINSICS = "intrinsics_cam.xml";
static const char * FILEPATH_PREDEFINE_CAMERA_DISTORT    = "intrinsics_distort.xml";


static const char * FILEPATH_CAMERA_INTRINSICS = "config/intrinsics_cam.xml";
static const char * FILEPATH_CAMERA_DISTORT    = "config/intrinsics_distort.xml";
static const char * FILEPATH_LOC_CALIB_DATA    = "loc_data.xml";
static const char * FILEPATH_CONFIG_FILE       = "config/general.txt";
static const char * FILEPATH_LOC_CALIB_FOLDER  = "config/";

static const char * FILEPATH_RESOURCE_COMMON_FOLDER = "res/common/";
static const char * FILEPATH_RESOURCE_LANG_FOLDER = "res/lang/";

static const char * FILEPATH_RESOURCE_PREDEFINE_FOLDER = "res/prefined/";
static const char * FILEPATH_RESOURCE_SOUND_FOLDER = "res/sound/";

static const char * FILEPATH_CONFIG_NEWMODEL_PREFIX = "v2_";

// sketch pad related
const int SKETCH_AREA_HEIGHT = 500;
const int SKETCH_AREA_WIDTH = 1000;
const int SKETCH_AUTOCLEAR_TIMEOUT = 10; //10 sec


// release option
#define RELEASE_PRODUCT_TYPE "laserkbd"
#define RELEASE_VENDOR_TYPE  "DFRobot"
#define RELEASE_VERSION_INFO "1.2"
#define UPDATE_POST_URL      "http://update.robopeak.com/check_update"



const unsigned short release_usb_pid = 0x3450;
const unsigned short release_usb_vid = 0x0ac8;
const unsigned short release_usb_hw_rev = 0x0900;
