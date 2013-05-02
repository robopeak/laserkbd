//
//  cap_qtkit.h
//  LaserKeyborad
//
//  Created by Shikai Chen on 13-3-27.
//  Copyright (c) 2013年 RoboPeak. All rights reserved.
//

#ifndef LaserKeyborad_cap_qtkit_h
#define LaserKeyborad_cap_qtkit_h


#import <QTKit/QTKit.h>

#ifndef QTKIT_VERSION_7_6_3
#define QTKIT_VERSION_7_6_3         70603
#define QTKIT_VERSION_7_0           70000
#endif

#ifndef QTKIT_VERSION_MAX_ALLOWED
#define QTKIT_VERSION_MAX_ALLOWED QTKIT_VERSION_7_0
#endif

#define DISABLE_AUTO_RESTART 999


#import "port/macos/3rd/UVCCameraControl.h"



@interface CaptureDelegateLocal : NSObject
{
	int newFrame;
    CVImageBufferRef  mCurrentImageBuffer;
	char* imagedata;
	IplImage* image;
	char* bgr_imagedata;
	IplImage* bgr_image;
	size_t currSize;
    
    UVCCameraControl * cameraControl;
}

- (void)captureOutput:(QTCaptureOutput *)captureOutput
  didOutputVideoFrame:(CVImageBufferRef)videoFrame
	 withSampleBuffer:(QTSampleBuffer *)sampleBuffer
	   fromConnection:(QTCaptureConnection *)connection;

- (void)captureOutput:(QTCaptureOutput *)captureOutput
didDropVideoFrameWithSampleBuffer:(QTSampleBuffer *)sampleBuffer
	   fromConnection:(QTCaptureConnection *)connection;

- (int)updateImage;
- (IplImage*)getOutput;

@end


class QTKitCapture {
public:
    
    static size_t listDevices(std::vector<std::string> &);
    
	QTKitCapture(int cameraNum = -1) ;
	~QTKitCapture();
	bool grabFrame();
	IplImage* retrieveFrame(int);
	double getProperty(int property_id);
	bool setProperty(int property_id, double value);
	int didStart();
	
    bool setExposureLevel(int level);
	
private:
	QTCaptureSession            *mCaptureSession;
	QTCaptureDeviceInput        *mCaptureDeviceInput;
	QTCaptureDecompressedVideoOutput    *mCaptureDecompressedVideoOutput;
    
    UVCCameraControl * cameraControl;
    
	CaptureDelegateLocal* capture;
	
	int startCaptureDevice(int cameraNum);
	void stopCaptureDevice();
	
	void setWidthHeight();
	bool grabFrame(double timeOut);
	
	int camNum;
	int width;
	int height;
	int settingWidth;
	int settingHeight;
	int started;
	int disableAutoRestart;
    
    int uniqueID;
	
};



#endif
