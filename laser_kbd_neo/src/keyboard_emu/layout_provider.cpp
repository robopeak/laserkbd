/*
 *  Keyboard layout manager and provider
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#if defined(WIN32) || defined(_MACOS)
#define FEATURE_PATTERN_TYPE_2
#else
#include "../config.h"
#endif
#include "../common.h"
#include "../cv_common.h"
#include <algorithm>
#include "layout_provider.h"
#include <functional>
#include "kdtree++/kdtree.hpp"

static const float MAX_KEY_BUTTON_SIZE = 60.0f;

const static KeyDesc_t _key_mapper[] =
{ 
    //    X       Y      W      H      Val   Val(capped)  Disp
    {     0 + 1,     0,   16,    20,   '1',    '!',       "1 !"} ,
    {    17 + 1,     0,   18,    20,   '2',    '@',       "2 Q"} ,
    {    35 + 1,     0,   18,    20,   '3',    '#',       "3 #"} ,
    {    53 + 1,     0,   18,    20,   '4',    '$' ,      "4 $"} ,
    {    71 + 1,     0,   18,    20,   '5',    '%' ,      "5 %"} ,
    {    89 + 1,     0,   18,    20,   '6',    '^' ,      "6 ^"} ,
    {   107 + 1,     0,   18,    20,   '7',    '&' ,      "7 &"} ,
    {   125 + 1,     0,   18,    20,   '8',  VK_MULTIPLY ,"8 *"} ,
    {   143 + 1,     0,   18,    20,   '9',    '(',       "9 ("} ,
    {   161 + 1,     0,   18,    20,   '0',    ')' ,      "0 )"} ,
    {   179 + 1,     0,   18,    20,VK_OEM_MINUS,'_' ,    "- _"} ,
    {   196 + 1,     0,   16,    20,VK_OEM_PLUS,VK_ADD ,  "= +"} ,
    {    -7,  19.5,   23,    19,VK_ESCAPE,VK_ESCAPE,  "ESC" } ,
    {    14,  19.5,   19,    19,   'Q',    'Q' , "Q"} ,
    {    33,  19.5,   19,    19,   'W',    'W', "W" } ,
    {    52,  19.5,   19,    19,   'E',    'E', "E" } ,
    {    71,  19.5,   19,    19,   'R',    'R' , "R"} ,
    {    90,  19.5,   19,    19,   'T',    'T', "T" } ,
    {   109,  19.5,   19,    19,   'Y',    'Y' , "Y"} ,
    {   128,  19.5,   18,    19,   'U',    'U' , "U"} ,
    {   147,  19.5,   19,    19,   'I',    'I' , "I"} ,
    {   166,  19.5,   19,    19,   'O',    'O' , "O"} ,
    {   185,  19.5,   19,    19,   'P',    'P' , "P"} ,
    { 206.5,  19.5,   24,    19,   VK_BACK,VK_BACK, "Back" } ,
    { -19.5,  38.5,   23,    19,   VK_CAPITAL, VK_CAPITAL, "CAP"} ,
    {   1.5,  38.5,   19,    19,   VK_TAB,VK_TAB , "TAB"} ,
    {  20.5,  38.5,   19,    19,   'A',    'A', "A"} ,
    {  39.5,  38.5,   19,    19,   'S',    'S' , "S"} ,
    {  58.5,  38.5,   19,    19,   'D',    'D' , "D"} ,
    {  77.5,  38.5,   19,    19,   'F',    'F' , "F"} ,
    {  96.5,  38.5,   19,    19,   'G',    'G' , "G"} ,
    { 115.5,  38.5,   19,    19,   'H',    'H' , "H"} ,
    { 134.5,  38.5,   19,    19,   'J',    'J', "J" } ,
    { 153.5,  38.5,   19,    19,   'K',    'K' , "K"} ,
    { 172.5,  38.5,   19,    19,   'L',    'L' , "L"} ,
    { 191.5,  38.5,   19,    19,   VK_OEM_1,VK_OEM_1, "; :" } ,
    {   218,  38.5,   34,    19,   VK_RETURN,VK_RETURN , "RET"} ,
    {   -32,  57.5,   27,    19,   VK_LSHIFT, VK_LSHIFT, "SHIFT"} ,
    {    -9,  57.5,   19,    19,   VK_OEM_3,VK_OEM_3 , "` ~"} ,
    {    10,  57.5,   19,    19,   VK_OEM_5,VK_OEM_5, "\\ |" } ,
    {    29,  57.5,   19,    19,   'Z',    'Z' , "Z"} ,
    {    48,  57.5,   19,    19,   'X',    'X' , "X"} ,
    {    67,  57.5,   19,    19,   'C',    'C' , "C"} ,
    {    86,  57.5,   19,    19,   'V',    'V' , "V"} ,
    {   105,  57.5,   19,    19,   'B',    'B' , "B"} ,
    {   124,  57.5,   19,    19,   'N',    'N' , "N"} ,
    {   143,  57.5,   19,    19,   'M',    'M' , "M"} ,
    {   162,  57.5,   19,    19,   VK_OEM_COMMA,    '<' , ", <"} ,
    {   181,  57.5,   19,    19,   VK_OEM_PERIOD,'>' , ". >"} ,
#ifdef FEATURE_PATTERN_TYPE_2
    {   200,  57.5,   19,    19,   VK_UP,VK_UP , "UP" } ,
    {   219,  57.5,   19,    19,   VK_OEM_2,VK_OEM_2 , "/ ?"} ,
#else
    {   200,  57.5,   19,    19,   VK_OEM_2,VK_OEM_2 , "/ ?"} ,
    {   219,  57.5,   19,    19,   VK_UP,VK_UP , "UP"} ,
#endif

    {   238,  57.5,   19,    19,   VK_RSHIFT,VK_RSHIFT , "SHIFT"} ,
    {  -34.5,  76.5,   22,    19,   VK_CONTROL,VK_CONTROL , "CTRL"} ,
#ifdef FEATURE_PATTERN_TYPE_2
    { -13,  76.5,   21,    19,   VK_MENU,VK_MENU , "ALT"} ,
    {   8,  76.5,   21,    19,   KEY_FN,KEY_FN , "FN"} ,

#else
    { -13,  76.5,   21,    19,   KEY_FN,KEY_FN , "FN"} ,
    {   8,  76.5,   21,    19,   VK_MENU,VK_MENU , "ALT"} ,
#endif
    {  28,  76.5,   19,    19,   VK_OEM_4,VK_OEM_4 , "[ {"} ,
    {  47,  76.5,   19,    19,   VK_OEM_6,VK_OEM_6 , "] }"} ,
    {   104.5,  76.5,  96,    19,   VK_SPACE,VK_SPACE , "SPACE"} ,
    { 162,  76.5,   19,    19,   VK_OEM_7,VK_OEM_7 , "' \""} ,
#ifdef FEATURE_PATTERN_TYPE_2
    { 181,  76.5,   19,    19,   VK_LEFT,VK_LEFT , "LEFT"} ,
    { 200,  76.5,   19,    19,   VK_DOWN, VK_DOWN, "DOWN"} ,
    { 219,  76.5,   19,    19,   VK_RIGHT,VK_RIGHT, "RIGHT"} ,
    { 238,  76.5,   19,    19,   VK_DELETE, VK_DELETE, "DEL"} 
#else
    { 181,  76.5,   19,    19,   VK_DELETE, VK_DELETE, "DEL"} ,
    { 200,  76.5,   19,    19,   VK_LEFT,VK_LEFT , "LEFT"} ,
    { 219,  76.5,   19,    19,   VK_DOWN, VK_DOWN, "DOWN"} ,
    { 238,  76.5,   19,    19,   VK_RIGHT,VK_RIGHT, "RIGHT" } 
#endif
};


struct KDBlob {
   float _x;
   float _y;
   int   _id;

   KDBlob(int id, float x, float y){
        _id = id;
        _x  = x;
        _y = y;
   }

   float operator[](size_t pos) const{
        if (pos == 0) return _x;
        else return _y;
   }
};

struct kdblob_evaluator
{
    bool operator() ( const KDBlob & blob) const {
        return KeyLayoutProvider::IsButtonHit(_x,
            _y, _key_mapper[blob._id]);
    }
    kdblob_evaluator(float x, float y) {
        _x = x;
        _y = y;
    }


    float _x, _y;
};

static float kdblob_accessor(const KDBlob  & val, size_t pos) {
    return val[pos];
}

typedef KDTree::KDTree<2, KDBlob, std::pointer_to_binary_function<const KDBlob &,size_t,float> > kdtree_2d_t;

#define GET_KD_TREE() (reinterpret_cast<kdtree_2d_t *>(_kd_tree))

KeyLayoutProvider::KeyLayoutProvider()
{
    _id_tl = _id_tr = _id_bl = _id_br = 0;
    
    _kd_tree = new kdtree_2d_t(std::ptr_fun(kdblob_accessor));
    

    for (int pos = 0; pos<_countof(_key_mapper); ++pos)
    {
        KDBlob blob(pos, _key_mapper[pos].x, _key_mapper[pos].y);
        GET_KD_TREE()->insert(blob);

        
        // find top-left key
        if (_key_mapper[_id_tl].x >= _key_mapper[pos].x
            && _key_mapper[_id_tl].y >= _key_mapper[pos].y) {
            _id_tl = pos;
        }

        // find top-right key
        if (_key_mapper[_id_tr].x <= _key_mapper[pos].x
            && _key_mapper[_id_tr].y >= _key_mapper[pos].y) {
            _id_tr = pos;
        }

        // find bottom-left key
        if (_key_mapper[_id_bl].x >= _key_mapper[pos].x
            && _key_mapper[_id_bl].y <= _key_mapper[pos].y) {
            _id_bl = pos;
        }

        // find bottom-right key
        if (_key_mapper[_id_br].x <= _key_mapper[pos].x
            && _key_mapper[_id_br].y <= _key_mapper[pos].y) {
            _id_br = pos;
        }
    }
 
    // buildsize
    size_t width1 = _key_mapper[_id_tl].width/2 + 
                    _key_mapper[_id_tr].width/2 +
                    _key_mapper[_id_tr].x - _key_mapper[_id_tl].x;

    size_t width2 = _key_mapper[_id_bl].width/2 + 
                    _key_mapper[_id_br].width/2 +
                    _key_mapper[_id_br].x - _key_mapper[_id_bl].x;

    size_t height1 = _key_mapper[_id_tl].height/2 + 
                    _key_mapper[_id_bl].height/2 +
                    _key_mapper[_id_bl].y - _key_mapper[_id_tl].y;

    size_t height2 = _key_mapper[_id_tr].height/2 + 
                    _key_mapper[_id_br].height/2 +
                    _key_mapper[_id_br].y - _key_mapper[_id_tr].y;

    _size.width = std::max(width1, width2);
    _size.height = std::max(height1, height2);
}

KeyLayoutProvider::~KeyLayoutProvider()
{
    delete GET_KD_TREE();
}

const KeyDesc_t * KeyLayoutProvider::getKeyAt(int pos)
{
    if (pos >= getCount()) return NULL;
    return &_key_mapper[pos];
}

size_t KeyLayoutProvider::getCount()
{
    return _countof(_key_mapper);
}

int KeyLayoutProvider::findKeyIdxByPoint(float x, float y)
{

    KDBlob searchBlob(-1, x, y);
    std::pair<kdtree_2d_t::const_iterator,float> nif =  GET_KD_TREE()->find_nearest_if(searchBlob, MAX_KEY_BUTTON_SIZE,
        kdblob_evaluator(x,y));

    if (nif.first == GET_KD_TREE()->end()) {
        return -1;
    }


    return nif.first->_id;
}
int  KeyLayoutProvider::getKeyIdxByKeyVal(int ch)
{
    
    for (int pos = 0; pos < _countof(_key_mapper); ++pos) {
        if ( _key_mapper[pos].lcase_val == ch) {
            return pos;
        }
    }
    return -1;
}

bool KeyLayoutProvider::IsButtonHit(float x, float y, const KeyDesc_t & key)
{
    float fixed_width;
    fixed_width = key.width + 3;
    //y += 5;// make some post-fix
    if ( x>=key.x-fixed_width*5/12 && x < key.x+fixed_width*5/12) {
        if (y>=key.y-key.height*5/12 && y< key.y + key.height*5/12) {
            return true;
        }
    }
    return false;
}

CvPoint2D32f KeyLayoutProvider::keyboardPosToWindowPos(const CvPoint2D32f & src)
{
    return cvPoint2D32f(src.x - std::min(_key_mapper[_id_tl].x - _key_mapper[_id_tl].width/2, _key_mapper[_id_bl].x - _key_mapper[_id_bl].width/2)
        , src.y);
}

void KeyLayoutProvider::renderLayout(IplImage* img, const std::vector<int> & key_pressed)
{
    const int imgWidth = img->roi->width;
    const int imgHeight = img->roi->height;

    const float MARGIN_VAL = 2;

    float offsetX = std::min(_key_mapper[_id_tl].x - _key_mapper[_id_tl].width/2, _key_mapper[_id_bl].x - _key_mapper[_id_bl].width/2);
    float offsetY = std::min(_key_mapper[_id_tl].y - _key_mapper[_id_tl].height/2, _key_mapper[_id_tr].y - _key_mapper[_id_tr].height/2);

    float scale = 1;

    scale = (imgHeight - MARGIN_VAL*2) / _size.height;

    if (_size.width * scale > imgWidth - MARGIN_VAL*2) {
        scale = (imgWidth - MARGIN_VAL*2) / _size.width;
    }

    offsetX -= (imgWidth/scale - _size.width)/2;
    offsetY -= (imgHeight/scale - _size.height)/2;

    cvFillImage(img, 0x0);

     // draw Layout
    for (int pos = 0; pos < _countof(_key_mapper); ++pos)
    {
        float posX, posY;
        float width, height;
        
        posX = _key_mapper[pos].x -  offsetX;
        posY = _key_mapper[pos].y -  offsetY;
        width = _key_mapper[pos].width -2;
        height = _key_mapper[pos].height -2;

        posX*=scale;
        posY*=scale;
        width*=scale;
        height*=scale;

        cvDrawRect( img, cvPoint(posX-width/2, posY-height/2) ,
                cvPoint(posX+width/2, posY+height/2), cvScalar(120,120,120));
        cv_textOut(img, posX-width/3, posY, _key_mapper[pos].desc, cvScalar(100,100,100));

    }


    // draw pressed
    for (int pos = 0; pos < key_pressed.size(); ++pos)
    {
        float posX, posY;
        float width, height;
        
        posX = _key_mapper[key_pressed[pos]].x -  offsetX;
        posY = _key_mapper[key_pressed[pos]].y -  offsetY;
        width = _key_mapper[key_pressed[pos]].width -2;
        height = _key_mapper[key_pressed[pos]].height -2;

        posX*=scale;
        posY*=scale;
        width*=scale;
        height*=scale;

        cvDrawRect( img, cvPoint(posX-width/2, posY-height/2) ,
                cvPoint(posX+width/2, posY+height/2), cvScalar(0,255,255));
        cv_textOut(img, posX-width/3, posY, _key_mapper[key_pressed[pos]].desc, cvScalar(0,255,255));

    }
}
   
