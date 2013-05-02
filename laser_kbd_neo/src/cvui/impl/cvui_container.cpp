/*
 *  Simple UI System for OpenCV
 *  Render Container Implementation
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */

#include "common.h"
#include "cv_common.h"


#include "cvui_base.h"


namespace rp{ namespace cvui {

RenderContainer::RenderContainer()
    : _lasthoverobj(NULL)
{

}

RenderContainer::~RenderContainer()
{
    clearAllObjects();
}

bool RenderContainer::addRenderObject(RenderObject * obj)
{
    if (!obj) return false;

    if (obj->getContainer()) return false;

    _objlist.push_back(obj);

    obj->setDirtyFlag();
    obj->_container = this;

    obj->onAddToContainer();

    return true;
}

//virtual bool addRenderObjectAt(RenderObject * obj, RenderObject * refobj);

// returns -1 if the specified obj doesn't belong to this container
int  RenderContainer::getObjectZOrder(RenderObject * obj)
{
    if (!obj) return -1;
    if (obj->getContainer() != this) return -1;


    int count = 0;
    std::list<RenderObject *>::const_iterator itr = _objlist.begin();

    while (itr != _objlist.end() ) {
        if ((*itr) == obj) {
            return count;
        }

        ++count;
        ++itr;
    }

    assert(!"should not reach here");
    return -1;
}

void RenderContainer::clearAllObjects()
{
    std::list<RenderObject *>::const_iterator itr = _objlist.begin();

    while (itr != _objlist.end() ) {
        (*itr)->onRemoveFromContainer();
        (*itr)->_container = NULL;

        ++itr;
    }
    _lasthoverobj = NULL;
    _objlist.clear();
}

void RenderContainer::removeObject(RenderObject * obj)
{
    if (!obj || obj->getContainer()!=this) return;

    obj->onRemoveFromContainer();
    obj->_container=NULL;
    _objlist.remove(obj);

    if (_lasthoverobj == obj) _lasthoverobj = NULL; 
}

void RenderContainer::renderObjects(IplImage* frame)
{
    CvRect dirtyRT;
    dirtyRT.x = -1;
    dirtyRT.y = -1;

    std::list<RenderObject *>::const_iterator itr = _objlist.begin();

    for (;itr != _objlist.end() ; ++itr) {
        RenderObject * current = *itr;

        if (!current->getVisible()) continue;

        CvRect ObjRect;
        ObjRect.x = current->getX();
        ObjRect.y = current->getY();
        ObjRect.width = current->getWidth();
        ObjRect.height = current->getHeight();

        if (!current->getDirtyFlag()) {
            CvRect intersect = cvRectIntersection(dirtyRT, ObjRect);

            if (intersect.width<=0 || intersect.height<=0) {
                continue;
            }
        }

        current->renderSelf(frame);


        dirtyRT.x = std::min(dirtyRT.x, ObjRect.x);
        dirtyRT.y = std::min(dirtyRT.y, ObjRect.y);
        
        int mergedRight = std::max(dirtyRT.x + dirtyRT.width, ObjRect.x + ObjRect.width);
        int mergedBottom = std::max(dirtyRT.y + dirtyRT.height, ObjRect.y + ObjRect.height);
        
        dirtyRT.width = mergedRight - dirtyRT.x;
        dirtyRT.height = mergedBottom - dirtyRT.y;

    }
}


void RenderContainer::handleMouseMove(int button, int x, int y)
{
    bool isabovesomething = false;
    std::list<RenderObject *>::const_iterator itr = _objlist.begin();
    for (;itr != _objlist.end() ; ++itr) {
        RenderObject * current = *itr;

        if (!current->getVisible()) continue;

        if (!current->hitTest(x, y)) {
            continue;
        }
        if (_lasthoverobj!=current) {
            if (_lasthoverobj) _lasthoverobj->handleMouseLeave(button);
            current->handleMouseEnter(button);
            _lasthoverobj = current;
        }


        isabovesomething = true;
        if (current->handleMouseMove(button, x, y)) break;   
    }
    if (!isabovesomething) {
        if (_lasthoverobj) _lasthoverobj->handleMouseLeave(button);
        _lasthoverobj = NULL;
    }
}



void RenderContainer::handleMousePress(int button, int x, int y)
{
    std::list<RenderObject *>::const_iterator itr = _objlist.begin();
    for (;itr != _objlist.end() ; ++itr) {
        RenderObject * current = *itr;

        if (!current->getVisible()) continue;

        if (!current->hitTest(x, y)) continue;

        if (current->handleMousePress(button, x, y)) break;

    }

    
}


void RenderContainer::handleMouseRelease(int button, int x, int y)
{
    std::list<RenderObject *>::const_iterator itr = _objlist.begin();
   for (;itr != _objlist.end() ; ++itr) {
        RenderObject * current = *itr;

        if (!current->getVisible()) continue;

        if (!current->hitTest(x, y)) continue;

        if (current->handleMouseRelease(button, x, y)) break;
    }

}

}}