/*
 *  CvCapture.mm
 *
 *  Created by Nicholas Butko on 11/3/09.
 *  Copyright 2009. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote products 
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "common.h"
#include "cv_common.h"
#include <iostream>
#include <math.h>

#include "port/macos/3rd/opencv/cap_qtkit.h"



using namespace std; 

/********************** Declaration of class headers ************************/

/*****************************************************************************
 *
 * CaptureDelegateLocal Declaration. 
 *
 * CaptureDelegateLocal is notified on a separate thread by the OS whenever there
 *   is a new frame. When "updateImage" is called from the main thread, it
 *   copies this new frame into an IplImage, but only if this frame has not
 *   been copied before. When "getOutput" is called from the main thread, 
 *   it gives the last copied IplImage. 
 *
 *****************************************************************************/


#if 0
CvCapture* cvCreateCameraCapture_QT(int index ) {
	CvCapture* retval = new QTKitCapture(index);
	if (!((QTKitCapture *)retval)->didStart())
		cvReleaseCapture(&retval); 
	return retval;
}
#endif
/********************** Implementation of Classes ****************************/ 

/*****************************************************************************
 *
 * QTKitCapture Implementation. 
 *
 * QTKitCapture is the instantiation of a capture source for cameras.
 *
 *****************************************************************************/

size_t QTKitCapture::listDevices(std::vector<std::string> & stlist)
{
    NSAutoreleasePool* localpool = [[NSAutoreleasePool alloc] init];
    QTCaptureDevice *device;
	NSArray* devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];
    
    
    stlist.clear();
    for (int pos = 0; pos < [devices count]; ++pos) {
        device = [devices objectAtIndex:pos];
        if (device) {
            stlist.push_back([[device localizedDisplayName] UTF8String]);
        }
    }

    [localpool drain];
    
    return stlist.size();
}


QTKitCapture::QTKitCapture(int cameraNum) {
	mCaptureSession = nil;
	mCaptureDeviceInput = nil;
	mCaptureDecompressedVideoOutput = nil;
	cameraControl = nil;
    
    capture = nil;
	
	width = 0; 
	height = 0; 
	settingWidth = 0; 
	settingHeight = 0; 
	disableAutoRestart = 0; 
	uniqueID = 0;

    
	camNum = cameraNum; 
	
	if (!startCaptureDevice(camNum)) {
		cout << "Warning, camera failed to properly initialize!" << endl; 
		started = 0; 
	} else {
		started = 1; 
	}
	
}

QTKitCapture::~QTKitCapture() {
	stopCaptureDevice(); 
	
	cout << "Cleaned up camera." << endl; 
}

int QTKitCapture::didStart() {
	return started; 
}


bool QTKitCapture::grabFrame() {
	return grabFrame(2);
}

bool QTKitCapture::grabFrame(double timeOut) {
	
	NSAutoreleasePool* localpool = [[NSAutoreleasePool alloc] init];
	double sleepTime = 0.005; 
	double total = 0; 

	NSDate *loopUntil = [NSDate dateWithTimeIntervalSinceNow:sleepTime];
	while (![capture updateImage] && (total += sleepTime)<=timeOut &&
		   [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode 
									beforeDate:loopUntil])
		loopUntil = [NSDate dateWithTimeIntervalSinceNow:sleepTime]; 

	[localpool drain];

	return total <= timeOut; 	
}

IplImage* QTKitCapture::retrieveFrame(int) {
	return [capture getOutput]; 
}

void QTKitCapture::stopCaptureDevice() {
	NSAutoreleasePool* localpool = [[NSAutoreleasePool alloc] init];
	
    if (cameraControl) [cameraControl release];
	[mCaptureSession stopRunning];
	
	QTCaptureDevice *device = [mCaptureDeviceInput device];
    if ([device isOpen])  [device close];
	
	[mCaptureSession release];
    [mCaptureDeviceInput release];
	
	[mCaptureDecompressedVideoOutput setDelegate:mCaptureDecompressedVideoOutput]; 
	[mCaptureDecompressedVideoOutput release]; 
	[capture release]; 
	[localpool drain]; 
	
}

int QTKitCapture::startCaptureDevice(int cameraNum) {
	NSAutoreleasePool* localpool = [[NSAutoreleasePool alloc] init];
	
	capture = [[CaptureDelegateLocal alloc] init]; 
	
	QTCaptureDevice *device; 
	NSArray* devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];
	if ([devices count] == 0) {
		cout << "QTKit didn't find any attached Video Input Devices!" << endl; 
		[localpool drain]; 
		return 0; 
	}
	
	if (cameraNum >= 0) {
		int nCameras = [devices count];
        if( cameraNum < 0 || cameraNum >= nCameras )
            return 0;
		device = [devices objectAtIndex:cameraNum] ;
	} else {
		device = [QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeVideo]  ;
	}
	int success; 
	NSError* error; 
	
    if (device) {
		
		success = [device open:&error];
        if (!success) {
			cout << "QTKit failed to open a Video Capture Device" << endl; 			
			[localpool drain]; 
			return 0; 
        }
		
		mCaptureDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:device] ;
		mCaptureSession = [[QTCaptureSession alloc] init] ;
		
        success = [mCaptureSession addInput:mCaptureDeviceInput error:&error];		
		
		if (!success) {
			cout << "QTKit failed to start capture session with opened Capture Device" << endl;
			[localpool drain]; 
			return 0; 
        }
		
        sscanf( [[device uniqueID] UTF8String], "0x%8x", &uniqueID );

        cameraControl = [[UVCCameraControl alloc] initWithLocationID:uniqueID];
        
        if (cameraControl) {
            [cameraControl setAutoExposure:NO];
        }

		
		mCaptureDecompressedVideoOutput = [[QTCaptureDecompressedVideoOutput alloc] init];
		[mCaptureDecompressedVideoOutput setDelegate:capture];
        
		NSDictionary *pixelBufferOptions ;
		if (width > 0 && height > 0) {
			pixelBufferOptions = [NSDictionary dictionaryWithObjectsAndKeys:
								  [NSNumber numberWithDouble:1.0*width], (id)kCVPixelBufferWidthKey,
								  [NSNumber numberWithDouble:1.0*height], (id)kCVPixelBufferHeightKey,
								  //[NSNumber numberWithUnsignedInt:k32BGRAPixelFormat], (id)kCVPixelBufferPixelFormatTypeKey,
								  [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],
								  (id)kCVPixelBufferPixelFormatTypeKey,
								  nil]; 
		} else {
			pixelBufferOptions = [NSDictionary dictionaryWithObjectsAndKeys:
								  [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA], 
								  (id)kCVPixelBufferPixelFormatTypeKey,
								  nil]; 
		}
		[mCaptureDecompressedVideoOutput setPixelBufferAttributes:pixelBufferOptions]; 
		
#if QTKIT_VERSION_MAX_ALLOWED >= QTKIT_VERSION_7_6_3
		[mCaptureDecompressedVideoOutput setAutomaticallyDropsLateVideoFrames:YES]; 
#endif
		
		
        success = [mCaptureSession addOutput:mCaptureDecompressedVideoOutput error:&error];
        if (!success) {
            cout << "QTKit failed to add Output to Capture Session" << endl; 
			[localpool drain]; 
            return 0;
        }
		
		[mCaptureSession startRunning];
		
        grabFrame(60);
		
		return 1; 
	}
	
	[localpool drain]; 
	return 0; 
}

void QTKitCapture::setWidthHeight() {	
	NSAutoreleasePool* localpool = [[NSAutoreleasePool alloc] init]; 
	NSDictionary* pixelBufferOptions = [NSDictionary dictionaryWithObjectsAndKeys:
						  [NSNumber numberWithDouble:1.0*width], (id)kCVPixelBufferWidthKey,
						  [NSNumber numberWithDouble:1.0*height], (id)kCVPixelBufferHeightKey,
						  [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],
						  (id)kCVPixelBufferPixelFormatTypeKey,
						  nil]; 
	
	[mCaptureDecompressedVideoOutput setPixelBufferAttributes:pixelBufferOptions];
	grabFrame(60); 
	[localpool drain]; 
}


double QTKitCapture::getProperty(int property_id){
	NSAutoreleasePool* localpool = [[NSAutoreleasePool alloc] init]; 
	
	NSArray* connections = [mCaptureDeviceInput	connections]; 
	QTFormatDescription* format = [[connections objectAtIndex:0] formatDescription]; 
	NSSize s1 = [[format attributeForKey:QTFormatDescriptionVideoCleanApertureDisplaySizeAttribute] sizeValue]; 
	
	int width=s1.width, height=s1.height; 
	switch (property_id) {
		case CV_CAP_PROP_FRAME_WIDTH:
			return width;
		case CV_CAP_PROP_FRAME_HEIGHT: 
			return height; 
		default:
			return 0; 
	}
	
	[localpool drain]; 
	
}

bool QTKitCapture::setProperty(int property_id, double value) {
	switch (property_id) {
		case CV_CAP_PROP_FRAME_WIDTH:
			width = value;
			settingWidth = 1; 
			if (settingWidth && settingHeight) {
				setWidthHeight(); 
				settingWidth =0; 
				settingHeight = 0; 
			}
			return true;
		case CV_CAP_PROP_FRAME_HEIGHT:
			height = value;
			settingHeight = 1; 
			if (settingWidth && settingHeight) {
				setWidthHeight();
				settingWidth =0; 
				settingHeight = 0; 
			}
			return true;
		case DISABLE_AUTO_RESTART:
			disableAutoRestart = value;
			return 1;
		default:
			return false;
	} 
}

bool QTKitCapture::setExposureLevel(int level) {
    float exp_time = 1;
    
    exp_time = 10000.0f*pow((double)2.0f, (double)level);
    
    printf("exp_time=%f\n", exp_time);
    return [cameraControl setExposure:exp_time];
}


/*****************************************************************************
 *
 * CaptureDelegateLocal Implementation. 
 *
 * CaptureDelegateLocal is notified on a separate thread by the OS whenever there
 *   is a new frame. When "updateImage" is called from the main thread, it
 *   copies this new frame into an IplImage, but only if this frame has not
 *   been copied before. When "getOutput" is called from the main thread, 
 *   it gives the last copied IplImage. 
 *
 *****************************************************************************/


@implementation CaptureDelegateLocal 

- (id)init {
	[super init]; 
	newFrame = 0; 
	imagedata = NULL; 
	bgr_imagedata = NULL; 
	currSize = 0;
	image = NULL; 
	bgr_image = NULL; 
	return self; 
}


-(void)dealloc {
	if (imagedata != NULL) free(imagedata); 
	if (bgr_imagedata != NULL) free(bgr_imagedata); 
	cvReleaseImage(&image); 
	cvReleaseImage(&bgr_image); 
	[super dealloc]; 
}

- (void)captureOutput:(QTCaptureOutput *)captureOutput 
  didOutputVideoFrame:(CVImageBufferRef)videoFrame 
	 withSampleBuffer:(QTSampleBuffer *)sampleBuffer 
	   fromConnection:(QTCaptureConnection *)connection {
	
    CVBufferRetain(videoFrame);
	CVImageBufferRef imageBufferToRelease  = mCurrentImageBuffer;
    
    @synchronized (self) {
		
        mCurrentImageBuffer = videoFrame;
		newFrame = 1; 
    }
	
	CVBufferRelease(imageBufferToRelease);
    
}
- (void)captureOutput:(QTCaptureOutput *)captureOutput 
didDropVideoFrameWithSampleBuffer:(QTSampleBuffer *)sampleBuffer 
	   fromConnection:(QTCaptureConnection *)connection {
	cout << "Camera dropped frame!" << endl; 
}

-(IplImage*) getOutput {
	return bgr_image; 
}

-(int) updateImage {
	if (newFrame==0) return 0; 
	CVPixelBufferRef pixels; 
	
	@synchronized (self){
		pixels = CVBufferRetain(mCurrentImageBuffer);
		newFrame = 0; 
	}
	
	CVPixelBufferLockBaseAddress(pixels, 0);		
	uint32_t* baseaddress = (uint32_t*)CVPixelBufferGetBaseAddress(pixels);
	
	size_t width = CVPixelBufferGetWidth(pixels);
	size_t height = CVPixelBufferGetHeight(pixels);
	size_t rowBytes = CVPixelBufferGetBytesPerRow(pixels);
	
	if (rowBytes != 0) { 
		
		if (currSize != rowBytes*height*sizeof(char)) {
			currSize = rowBytes*height*sizeof(char); 
			if (imagedata != NULL) free(imagedata); 
			if (bgr_imagedata != NULL) free(bgr_imagedata); 
			imagedata = (char*)malloc(currSize); 
			bgr_imagedata = (char*)malloc(currSize); 
		}
		
		memcpy(imagedata, baseaddress, currSize);
		
		if (image == NULL) {
			image = cvCreateImageHeader(cvSize(width,height), IPL_DEPTH_8U, 4); 
		}
		image->width =width; 
		image->height = height; 
		image->nChannels = 4; 
		image->depth = IPL_DEPTH_8U; 
		image->widthStep = rowBytes; 
		image->imageData = imagedata; 
		image->imageSize = currSize; 
		
		if (bgr_image == NULL) {
			bgr_image = cvCreateImageHeader(cvSize(width,height), IPL_DEPTH_8U, 3); 
		}
		bgr_image->width =width; 
		bgr_image->height = height; 
		bgr_image->nChannels = 3; 
		bgr_image->depth = IPL_DEPTH_8U; 
		bgr_image->widthStep = rowBytes; 
		bgr_image->imageData = bgr_imagedata; 
		bgr_image->imageSize = currSize; 
		
		cvCvtColor(image, bgr_image, CV_BGRA2BGR); 
		
	}
	
	CVPixelBufferUnlockBaseAddress(pixels, 0);
	CVBufferRelease(pixels); 
	
	return 1; 
}

@end


