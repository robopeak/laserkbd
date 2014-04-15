/*
 *  Laser Keyboard
 *  Camera Selection UI
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2014 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"

#include "model_selector.h"

#include "resource_mgr.h"


static const char * WINDOWNAME_TITLE = "RoboPeak Laser Keyboard Console";
static const int WINDOWS_SIZE_WIDTH = 500;
static const int WINDOWS_SIZE_HEIGHT = 400;

extern ResourceMgr          g_resouce_mgr;

ModelSelector::ModelSelector()
    : _predefined_model(0)
    , _model_specified(false)
{
    windowImg = cvCreateImage(cvSize(WINDOWS_SIZE_WIDTH,WINDOWS_SIZE_HEIGHT), 8, 3);

    _ui_modelv1.setNormalImage(g_resouce_mgr.getImageRes("btn.modelv1.png"));
    _ui_modelv1.setActiveImage(g_resouce_mgr.getImageRes("btn.modelv1.active.png"));

    _ui_modelv2.setNormalImage(g_resouce_mgr.getImageRes("btn.modelv2.png"));
    _ui_modelv2.setActiveImage(g_resouce_mgr.getImageRes("btn.modelv2.active.png"));

    _img_title.setImage(g_resouce_mgr.getImageRes("modelsel.title.png"));

    _img_title.moveTo(0,0);

    _ui_modelv2.moveTo(_ui_modelv2.getWidth()/3, (WINDOWS_SIZE_HEIGHT - _ui_modelv1.getHeight())/2);
    _ui_modelv1.moveTo(WINDOWS_SIZE_WIDTH - _ui_modelv2.getWidth()*4/3,  (WINDOWS_SIZE_HEIGHT - _ui_modelv2.getHeight())/2);

    _ui_modelv1.setListener(&ModelSelector::s_onSelectV1, this);
    _ui_modelv2.setListener(&ModelSelector::s_onSelectV2, this);

    uicontainer.addRenderObject(&_img_title);
    uicontainer.addRenderObject(&_ui_modelv1);
    uicontainer.addRenderObject(&_ui_modelv2);

}

ModelSelector::~ModelSelector()
{
    ::cvReleaseImage(&windowImg);
}


void ModelSelector::setPredefinedModel(int modelID, bool userspecified)
{
    _predefined_model = modelID;
    _model_specified = userspecified;
}


int ModelSelector::doModal()
{
    _working_flg = 1;
    cvNamedWindow(WINDOWNAME_TITLE);

   
    if (_model_specified) {
        goto _final;
    } 

    while(1) {
        uicontainer.renderObjects(windowImg);
        
        cvShowImage(WINDOWNAME_TITLE, windowImg);
        cvSetMouseCallback(WINDOWNAME_TITLE, &ModelSelector::s_onMouse, this);

        int key = cvWaitKey(1);
        
        if (key == VK_ESCAPE) {
            _predefined_model = -1;
            break;
        }
        if (!_working_flg) {
            break;
        }
    }

_final:
    uicontainer.clearAllObjects();
    cvDestroyWindow(WINDOWNAME_TITLE);
    return _predefined_model;
}


void ModelSelector::s_onMouse(int mouse_event, int x, int y,int, void* pThis)
{
    ModelSelector * This = reinterpret_cast<ModelSelector *>(pThis);
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

void ModelSelector::s_onSelectV1(void * data, int button, int x, int y)
{
    ModelSelector * This = reinterpret_cast<ModelSelector *>(data);
    This->_predefined_model = 0;
    This->_working_flg = 0;
}

void ModelSelector::s_onSelectV2(void * data, int button, int x, int y)
{
    ModelSelector * This = reinterpret_cast<ModelSelector *>(data);
    This->_predefined_model = 1;
    This->_working_flg = 0;
}
