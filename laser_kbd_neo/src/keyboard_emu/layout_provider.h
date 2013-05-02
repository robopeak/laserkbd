/*
 *  Keyboard layout manager and provider
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

typedef unsigned short KEY_VAL_T;

#define KEY_FN 0x92

struct KeyDesc_t {
    float x;
    float y;
    float width;
    float height;
    KEY_VAL_T lcase_val;
    KEY_VAL_T ucase_val;
    const char * desc;
};


class KeyLayoutProvider {
public:
    KeyLayoutProvider();
    ~KeyLayoutProvider();

    const KeyDesc_t * getKeyAt(int pos);
    size_t getCount();

    int findKeyIdxByPoint(float x, float y);
    
    static bool IsButtonHit(float x, float y, const KeyDesc_t & key);

    void renderLayout(IplImage* img, const std::vector<int> & key_pressed);

    const cv::Size2f & getLayoutSize() const { return _size; }
    int  getTopLeftKeyIdx() {return _id_tl; }
    int  getTopRightKeyIdx() { return _id_tr; }
    int  getBottomLeftKeyIdx() { return _id_bl; }
    int  getBottomRightKeyIdx() { return _id_br; }
    
    int  getKeyIdxByKeyVal(int ch);

    CvPoint2D32f keyboardPosToWindowPos(const CvPoint2D32f & );

protected:
    void * _kd_tree;

    int    _id_tl, _id_tr, _id_bl, _id_br;
    cv::Size2f  _size;
};