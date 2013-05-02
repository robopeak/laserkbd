/*
 *  Simple UI System for OpenCV
 *  Header for Base Infrastructure 
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#pragma once

#include <list>


namespace rp{ namespace cvui {

class RenderContainer;
class RenderObject;

class RenderContainer {
public:
    RenderContainer() ;
    virtual ~RenderContainer() ;

    virtual bool addRenderObject(RenderObject * obj);
    //virtual bool addRenderObjectAt(RenderObject * obj, RenderObject * refobj);

    // returns -1 if the specified obj doesn't belong to this container
    virtual int  getObjectZOrder(RenderObject * obj);

    virtual void clearAllObjects();
    virtual void removeObject(RenderObject * obj);

    virtual void renderObjects(IplImage* frame);

    virtual void handleMouseMove(int button, int x, int y);
    virtual void handleMousePress(int button, int x, int y);
    virtual void handleMouseRelease(int button, int x, int y);

protected:
    std::list<RenderObject *> _objlist;
    RenderObject *            _lasthoverobj;

};


class RenderObject {
    friend class RenderContainer;
public:
    RenderObject() 
        : _container(NULL)
        , _x(0), _y(0), _width(0), _height(0)
        , _alpha(0)
        , _visible(true)
        , _dirtyflag(false)
    {};

    virtual ~RenderObject() {
        if (_container) {
            assert(!"render object should be removed from the container before destroy it.");
        }
    };

    virtual int getX() {
        return _x;
    }

    virtual int getY() {
        return _y;
    }

    virtual int getWidth() {
        return _width;
    }

    virtual int getHeight() {
        return _height;
    }

    int getBottom() {
        return getY() + getHeight();
    }

    int getRight() {
        return getX() + getWidth();
    }

    virtual void moveTo(int x, int y) {
        _x = x;
        _y = y;
        _dirtyflag = true;
    }

    virtual void setSize(int cx, int cy) {
        _width = cx;
        _height = cy;
        _dirtyflag = true;
    }

    bool getVisible() { return _visible; }
    virtual void setVisible(bool visible) { _visible = visible; }

    bool getDirtyFlag() { return _dirtyflag; }
    virtual void setDirtyFlag() { _dirtyflag = true; }

    virtual void renderSelf(IplImage* frame) {
        _dirtyflag = false;
    }

    virtual bool handleMouseMove(int button, int x, int y) { return false; }
    virtual bool handleMouseEnter(int button) { return false; }
    virtual bool handleMouseLeave(int button) { return false; }
    virtual bool handleMousePress(int button, int x, int y) { return false; }
    virtual bool handleMouseRelease(int button, int x, int y) { return false; }

    RenderContainer * getContainer() { return _container; }


    virtual bool hitTest(int x, int y) {
        return (x >= _x && x < _x + _width && y >= _y && y < _y + _height);
    }

protected:

    virtual void onAddToContainer() {}
    virtual void onRemoveFromContainer() {}

    RenderContainer * _container;
    int _x, _y, _width, _height;
    int _alpha;
    bool _visible;
    bool _dirtyflag;
};


typedef void (*ui_evt_listener) (void * data, int button, int x, int y);


}}