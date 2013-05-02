/*
 *  Laser Keyboard
 *  Camera Selection UI
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#pragma once

#include "cvui/cvui.h"

class CameraSelector
{
public:
    CameraSelector();
    ~CameraSelector();

    void setPredefinedCamera(int camid, bool specified);

    int doModal();

protected:
    
    IplImage * windowImg;
    rp::cvui::RenderContainer uicontainer;


    rp::cvui::ImageFrame _ui_backimg;
    rp::cvui::Button     _ui_leftbtn;
    rp::cvui::Button     _ui_rightbtn;
    rp::cvui::Button     _ui_okbtn;
    rp::cvui::Button     _ui_cancelbtn;
  //  static void s_onLeftButton(

    std::vector<std::string>  _cameralist;

    int    _prefined_camid;
    int    _working_flg; // 1-working 0-exit
    bool   _camid_specified;

    void   renderTxtContent();
    static void s_onMouse(int mouse_event, int x, int y,int, void* pThis);

    static void s_onleftbtn(void * data, int button, int x, int y);
    static void s_onrightbtn(void * data, int button, int x, int y);

    static void s_on_okbtn(void * data, int button, int x, int y);
    static void s_on_cancelbtn(void * data, int button, int x, int y);



};