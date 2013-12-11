/*
 *  Laser Keyboard
 *  Camera Selection UI
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"

#include "camera_selector.h"

#include "resource_mgr.h"

#include "port/common/PowerVideoCapture.h"

static const char * WINDOWNAME_TITLE = "RoboPeak Laser Keyboard Console";
static const int WINDOWS_SIZE_WIDTH = 600;
static const int WINDOWS_SIZE_HEIGHT = 400;

extern ResourceMgr          g_resouce_mgr;

CameraSelector::CameraSelector()
    : _prefined_camid(-1)
    , _camid_specified(false)
{
    windowImg = cvCreateImage(cvSize(WINDOWS_SIZE_WIDTH,WINDOWS_SIZE_HEIGHT), 8, 3);

    _ui_backimg.setImage(g_resouce_mgr.getImageRes("coverpage.png"));
    _ui_leftbtn.setNormalImage(g_resouce_mgr.getImageRes("btn.prev.png"));
    _ui_leftbtn.setActiveImage(g_resouce_mgr.getImageRes("btn.prev.active.png"));

    _ui_rightbtn.setNormalImage(g_resouce_mgr.getImageRes("btn.next.png"));
    _ui_rightbtn.setActiveImage(g_resouce_mgr.getImageRes("btn.next.active.png"));

    _ui_leftbtn.moveTo(0, (WINDOWS_SIZE_HEIGHT - _ui_leftbtn.getHeight())/2);
    _ui_rightbtn.moveTo(WINDOWS_SIZE_WIDTH - _ui_rightbtn.getWidth(),  (WINDOWS_SIZE_HEIGHT - _ui_rightbtn.getHeight())/2);


    _ui_okbtn.setNormalImage(g_resouce_mgr.getImageRes("btn.ok.png"));
    _ui_okbtn.setActiveImage(g_resouce_mgr.getImageRes("btn.ok.active.png"));

    _ui_cancelbtn.setNormalImage(g_resouce_mgr.getImageRes("btn.cancel.png"));
    _ui_cancelbtn.setActiveImage(g_resouce_mgr.getImageRes("btn.cancel.active.png"));

    _ui_cancelbtn.moveTo((WINDOWS_SIZE_WIDTH-_ui_okbtn.getWidth()*3)/2, _ui_leftbtn.getBottom() + 60);
    _ui_okbtn.moveTo((WINDOWS_SIZE_WIDTH+_ui_cancelbtn.getWidth())/2, _ui_leftbtn.getBottom() + 60);

    _ui_leftbtn.setListener(&CameraSelector::s_onleftbtn, this);
    _ui_rightbtn.setListener(&CameraSelector::s_onrightbtn, this);
    _ui_okbtn.setListener(&CameraSelector::s_on_okbtn, this);
    _ui_cancelbtn.setListener(&CameraSelector::s_on_cancelbtn, this);
}

CameraSelector::~CameraSelector()
{
    ::cvReleaseImage(&windowImg);
}


void CameraSelector::setPredefinedCamera(int camid, bool userspecified)
{
    _prefined_camid = camid;
    _camid_specified = userspecified;
}


int CameraSelector::doModal()
{
    _working_flg = 1;
    cvNamedWindow(WINDOWNAME_TITLE);

    //phase #1 : show log

    unsigned long startms = getms();

    uicontainer.addRenderObject(&_ui_backimg);


    uicontainer.renderObjects(windowImg);
    cvShowImage(WINDOWNAME_TITLE, windowImg);

    while(getms() - startms < 2000) {   
        int key = cvWaitKey(1);
        if (key == VK_ESCAPE) {
            break;
        }
    }

    

    if (_camid_specified) {
        goto _final;
    } 

    PowerVideoCapture::EnumCaptureDevices(_cameralist);

    if (_cameralist.size()) {
        if (_prefined_camid<0) _camid_specified = 0;
        else if (_prefined_camid>=_cameralist.size()) {
            _prefined_camid = _cameralist.size()-1;
        }
    }

    // clear bkg
    ::cvRectangle(windowImg, cvPoint(0,0), cvPoint(WINDOWS_SIZE_WIDTH,WINDOWS_SIZE_HEIGHT),
        cvScalar(0), -1);


    //phase #2
    _ui_backimg.setImage(g_resouce_mgr.getImageRes("camsel.title.png"));


    
    uicontainer.addRenderObject(&_ui_leftbtn);
    uicontainer.addRenderObject(&_ui_rightbtn);

    uicontainer.addRenderObject(&_ui_okbtn);
    uicontainer.addRenderObject(&_ui_cancelbtn);

    while(1) {
        renderTxtContent();
        uicontainer.renderObjects(windowImg);
        
        cvShowImage(WINDOWNAME_TITLE, windowImg);
        cvSetMouseCallback(WINDOWNAME_TITLE, &CameraSelector::s_onMouse, this);

        int key = cvWaitKey(1);
        
        if (key == VK_ESCAPE) {
            _prefined_camid = -1;
            break;
        }
        if (!_working_flg) {
            break;
        }
    }

_final:
    uicontainer.clearAllObjects();
    cvDestroyWindow(WINDOWNAME_TITLE);
    return _prefined_camid;
}

void CameraSelector::renderTxtContent()
{
    char msgbuffer[100];
    if (_cameralist.size()) {
        sprintf(msgbuffer, "(%d/%d)", _prefined_camid+1, _cameralist.size());
    } else {
        sprintf(msgbuffer, "(null)");

    }
    cvRectangle(windowImg, cvPoint(0,_ui_backimg.getBottom()), cvPoint(WINDOWS_SIZE_WIDTH,_ui_leftbtn.getY()),
        cvScalar(0), -1);
    cv_textOut(windowImg, WINDOWS_SIZE_WIDTH/2, _ui_backimg.getBottom() + 20
        , msgbuffer, cvScalar(255,255,255), 2, true);

    cvRectangle(windowImg, cvPoint(_ui_leftbtn.getRight() + 5,_ui_leftbtn.getY()), cvPoint(_ui_rightbtn.getX()-5,_ui_rightbtn.getBottom()),
        cvScalar(0), -1);
    cvRectangle(windowImg, cvPoint(_ui_leftbtn.getRight() + 5,_ui_leftbtn.getY()), cvPoint(_ui_rightbtn.getX()-5,_ui_rightbtn.getBottom()),
        cvScalar(100,100,100), 1);

    if (_cameralist.size()) 
    {
        cv_textOut(windowImg, 
                   WINDOWS_SIZE_WIDTH / 2, 
                   (_ui_leftbtn.getY() + _ui_leftbtn.getBottom()) / 2, 
                   _cameralist[_prefined_camid].c_str(), 
                   cvScalar(255,255,255), 2, true);
    }
}

void CameraSelector::s_onMouse(int mouse_event, int x, int y,int, void* pThis)
{
    CameraSelector * This = reinterpret_cast<CameraSelector *>(pThis);
    switch (mouse_event) {
        case CV_EVENT_LBUTTONDOWN:
            This->uicontainer.handleMousePress(1, x, y);
            break;
        case CV_EVENT_MOUSEMOVE:
            This->uicontainer.handleMouseMove(1, x, y);
            break;

        case CV_EVENT_LBUTTONUP:
            This->uicontainer.handleMouseRelease(1, x, y);
            break;
    }
}

void CameraSelector::s_onleftbtn(void * data, int button, int x, int y)
{
    CameraSelector * This = reinterpret_cast<CameraSelector *>(data);
    if (--This->_prefined_camid <0) This->_prefined_camid = This->_cameralist.size()-1;
}

void CameraSelector::s_onrightbtn(void * data, int button, int x, int y)
{
    CameraSelector * This = reinterpret_cast<CameraSelector *>(data);
    if (++This->_prefined_camid >= This->_cameralist.size()) This->_prefined_camid = 0;

}


void CameraSelector::s_on_okbtn(void * data, int button, int x, int y)
{
    CameraSelector * This = reinterpret_cast<CameraSelector *>(data);
    This->_working_flg = 0;
}

void CameraSelector::s_on_cancelbtn(void * data, int button, int x, int y)
{
    CameraSelector * This = reinterpret_cast<CameraSelector *>(data);
    This->_prefined_camid = -1;

    This->_working_flg = 0;
}
