/*
 *  Laser Keyboard
 *  Camera Selection UI
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2014 RoboPeak Team
 *  http://www.robopeak.net
 */


#pragma once

#include "cvui/cvui.h"

class ModelSelector
{
public:
    ModelSelector();
    ~ModelSelector();

    void setPredefinedModel(int modelID, bool specified);

    int doModal();

protected:
    
    IplImage * windowImg;
    rp::cvui::RenderContainer uicontainer;


    rp::cvui::Button     _ui_modelv1;
    rp::cvui::Button     _ui_modelv2;
    rp::cvui::ImageFrame _img_title;
    int    _working_flg; // 1-working 0-exit
    int    _predefined_model;
    bool   _model_specified;

    void   renderTxtContent();

    static void s_onSelectV1(void * data, int button, int x, int y);
    static void s_onSelectV2(void * data, int button, int x, int y);
    static void s_onMouse(int mouse_event, int x, int y,int, void* pThis);


};