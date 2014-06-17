//
//  ofxScrollView.cpp
//  Created by Lukasz Karluk on 2/06/2014.
//

#include "ofxScrollView.h"

//--------------------------------------------------------------
ofxScrollView::ofxScrollView() {
    bNeedsToReset = false;
    bUserInteractionEnabled = false;

    scrollEasing = 0.5;
    bounceBack = 1.0;
    
    dragID = -1;
    bDragging = false;
    bDraggingChanged = false;
    
    zoomID = -1;
    bZooming = false;
    bZoomingChanged = false;
    
    scale = 1.0;
    scaleDown = 1.0;
    scaleMin = 1.0;
    scaleMax = 1.0;
    
    setUserInteraction(true);
}

ofxScrollView::~ofxScrollView() {
    setUserInteraction(false);
}

//--------------------------------------------------------------
void ofxScrollView::setup() {
    if(windowRect.isEmpty() == true) {
        windowRect.width = ofGetWidth();
        windowRect.height = ofGetHeight();
    }
    
    if(contentRect.isEmpty() == true) {
        contentRect = windowRect;
    }
    
    scaleMin = windowRect.height / (float)contentRect.height;
    scaleMax = scaleMin * 4;
    scale = scaleMin;
}

void ofxScrollView::reset() {
    scroll.set(0);
    scrollDown.set(0);
    scrollEased.set(0);
    
    dragDownPos.set(0);
    dragMovePos.set(0);
    dragMovePosPrev.set(0);
    dragVel.set(0);
    dragID = -1;
    bDragging = false;
    bDraggingChanged = false;
    
    zoomDownScreenPos.set(0);
    zoomMoveScreenPos.set(0);
    zoomDownContentPos.set(0);
    zoomID = -1;
    bZooming = false;
    bZoomingChanged = false;
    
    scale = scaleMin;
    scaleDown = scaleMin;
    mat.makeIdentityMatrix();
}

//--------------------------------------------------------------
void ofxScrollView::setWindowRect(const ofRectangle & rect) {
    if(windowRect == rect) {
        return;
    }
    windowRect = rect;
    bNeedsToReset = true;
}

void ofxScrollView::setContentRect(const ofRectangle & rect) {
    if(contentRect == rect) {
        return;
    }
    contentRect = rect;
    bNeedsToReset = true;
}

void ofxScrollView::setScrollEasing(float value) {
    scrollEasing = value;
}

void ofxScrollView::setScrollPosition(float x, float y, bool bEase) {
    dragCancel();
    zoomCancel();

    scroll = ofVec2f(x, y);
    if(bEase == false) {
        scrollEased = scroll;
    }
}

void ofxScrollView::setUserInteraction(bool bEnable) {
    if(bUserInteractionEnabled == bEnable) {
        return;
    }
    if(bUserInteractionEnabled == true) {
        bUserInteractionEnabled = false;

#ifdef TARGET_OPENGLES
        ofRemoveListener(ofEvents().touchDown, this, &ofxScrollView::touchDown);
        ofRemoveListener(ofEvents().touchMoved, this, &ofxScrollView::touchMoved);
        ofRemoveListener(ofEvents().touchUp, this, &ofxScrollView::touchUp);
#else
        ofRemoveListener(ofEvents().mousePressed, this, &ofxScrollView::mousePressed);
        ofRemoveListener(ofEvents().mouseDragged, this, &ofxScrollView::mouseDragged);
        ofRemoveListener(ofEvents().mouseReleased, this, &ofxScrollView::mouseReleased);
#endif
        
    } else {
        bUserInteractionEnabled = true;

#ifdef TARGET_OPENGLES
        ofAddListener(ofEvents().touchDown, this, &ofxScrollView::touchDown);
        ofAddListener(ofEvents().touchMoved, this, &ofxScrollView::touchMoved);
        ofAddListener(ofEvents().touchUp, this, &ofxScrollView::touchUp);
#else
        ofAddListener(ofEvents().mousePressed, this, &ofxScrollView::mousePressed);
        ofAddListener(ofEvents().mouseDragged, this, &ofxScrollView::mouseDragged);
        ofAddListener(ofEvents().mouseReleased, this, &ofxScrollView::mouseReleased);
#endif
    }
}

void ofxScrollView::setBounceBack(float value) {
    bounceBack = value;
}

const ofRectangle & ofxScrollView::getWindowRect() {
    return windowRect;
}

const ofRectangle & ofxScrollView::getContentRect() {
    return contentRect;
}

const ofVec2f & ofxScrollView::getScrollPosition() {
    return scrollEased;
}

const ofMatrix4x4 & ofxScrollView::getMatrix() {
    return mat;
}

//--------------------------------------------------------------
ofRectangle ofxScrollView::transformRect(const ofRectangle & rect, const ofMatrix4x4 & mat) {
    ofRectangle r;
    r = rect;
    
    ofVec3f p0(r.x, r.y, 0);
    ofVec3f p1(r.x + r.width, r.y + r.height, 0);
    p0 = mat.preMult(p0);
    p1 = mat.preMult(p1);
    
    r.x = p0.x;
    r.y = p0.y;
    r.width = p1.x - p0.x;
    r.height = p1.y - p0.y;
    
    return r;
}

ofVec2f ofxScrollView::screenPointToContentPoint(const ofVec2f & screenPoint) {
    float px = ofMap(screenPoint.x, scrollRect.x, scrollRect.x + scrollRect.width, 0.0, 1.0, true);
    float py = ofMap(screenPoint.y, scrollRect.y, scrollRect.y + scrollRect.height, 0.0, 1.0, true);
    
    ofVec2f contentPoint;
    contentPoint.x = px * contentRect.width;
    contentPoint.y = py * contentRect.height;
    return contentPoint;
}

//--------------------------------------------------------------
void ofxScrollView::update() {
    
    if(bNeedsToReset == true) {
        bNeedsToReset = false;
        setup();
    }
    
    if(bZooming == true) {
        
        ofVec2f zoomDiff = zoomMoveScreenPos - zoomDownScreenPos;
        float zoomUnitDist = windowRect.width;
        float zoom = ofMap(zoomDiff.x, -zoomUnitDist, zoomUnitDist, -1.0, 1.0, true);
        scale = scaleDown + zoom;
        scale = ofClamp(scale, scaleMin, scaleMax);
        
    } else {
        
        //
        
    }
    
    //----------------------------------------------------------
    scrollSize.x = MAX((contentRect.width * scale) - windowRect.width, 0);
    scrollSize.y = MAX((contentRect.height * scale) - windowRect.height, 0);
    
    //----------------------------------------------------------
    if(bZooming == false) {
        if(bDragging == true) {
            
            ofVec2f dragVelNew = dragMovePos - dragMovePosPrev;
            dragMovePosPrev = dragMovePos;
            dragVel.x += (dragVelNew.x - dragVel.x) * 0.5;
            dragVel.y += (dragVelNew.y - dragVel.y) * 0.5;
            
            ofVec2f dragDiff = dragDownPos - dragMovePos;

            ofVec2f scrollDiff(0, 0);
            if(scrollSize.x > 0) {
                scrollDiff.x = dragDiff.x / scrollSize.x;
            }
            if(scrollSize.y > 0) {
                scrollDiff.y = dragDiff.y / scrollSize.y;
            }
            
            scroll = scrollDown + scrollDiff;
            
        } else {
            
            dragVel *= 0.9;
            
            ofVec2f scrollDiff(0, 0);
            if(scrollSize.x > 0) {
                scrollDiff.x = dragVel.x / scrollSize.x;
            }
            if(scrollSize.y > 0) {
                scrollDiff.y = dragVel.y / scrollSize.y;
            }
            
            scroll -= scrollDiff;
        }
    }
    
    //----------------------------------------------------------
    if(scroll.x < 0.0) {
        scroll.x += (0.0 - scroll.x) * bounceBack;
    } else if(scroll.x > 1.0) {
        scroll.x += (1.0 - scroll.x) * bounceBack;
    }

    if(scroll.y < 0.0) {
        scroll.y += (0.0 - scroll.y) * bounceBack;
    } else if(scroll.y > 1.0) {
        scroll.y += (1.0 - scroll.y) * bounceBack;
    }
    
    scrollEased += (scroll - scrollEased) * scrollEasing;
    
    //----------------------------------------------------------
    
    ofVec2f offset = scrollSize * scrollEased;
    mat.makeIdentityMatrix();
    mat.preMultTranslate(ofVec3f(-offset.x, -offset.y, 0));
    mat.preMultScale(ofVec3f(scale, scale, 1.0));
    
    if(bZooming == true) {
        ofVec3f p0(zoomDownScreenPos.x, zoomDownScreenPos.y, 0);
        ofVec3f p1(mat.preMult(ofVec3f(zoomDownContentPos.x, zoomDownContentPos.y, 0)));
        ofVec3f p2 = p0 - p1;
        
        mat.postMultTranslate(p2);
        
        ofVec3f pos = mat.getTranslation();
        if(scrollSize.x > 0) {
            scroll.x = ofMap(pos.x, 0.0, -scrollSize.x, 0.0, 1.0);
        }
        if(scrollSize.y > 0) {
            scroll.y = ofMap(pos.y, 0.0, -scrollSize.y, 0.0, 1.0);
        }
    }
    
    scrollRect = transformRect(contentRect, mat);
    
    //----------------------------------------------------------
    bDraggingChanged = false;
    bZoomingChanged = false;
}

//--------------------------------------------------------------
void ofxScrollView::begin() {
    ofPushMatrix();
    ofMultMatrix(mat);
}

void ofxScrollView::end() {
    ofPopMatrix();
}

void ofxScrollView::draw() {
    if(bZooming == true) {
        
        ofVec3f p0(zoomDownScreenPos.x, zoomDownScreenPos.y, 0);
        
        ofVec3f p1(zoomDownContentPos.x, zoomDownContentPos.y, 0);
        p1 = mat.preMult(p1);
        
        ofSetColor(ofColor::red);
        ofLine(p0, p1);
        ofCircle(p0, 10);
        ofCircle(p1, 10);
        ofSetColor(255);
    }
}

//--------------------------------------------------------------
void ofxScrollView::exit() {
    //
}

//--------------------------------------------------------------
void ofxScrollView::dragDown(int x, int y, int id) {
    if(dragID == id) {
        return;
    }
    dragID = id;
    
    dragDownPos = dragMovePos = dragMovePosPrev = ofVec2f(x, y);
    dragVel.set(0);
    
    scrollDown = scroll;
    
    bDragging = true;
    bDraggingChanged = true;
}

void ofxScrollView::dragMoved(int x, int y, int id) {
    if(dragID != id) {
        return;
    }
    
    dragMovePos.set(x, y);
}

void ofxScrollView::dragUp(int x, int y, int id) {
    if(dragID != id) {
        return;
    }
    
    dragMovePos.set(x, y);
    
    dragID = -1;
    
    bDragging = false;
    bDraggingChanged = true;
}

void ofxScrollView::dragCancel() {
    dragVel.set(0);
    dragID = -1;
    
    bDragging = false;
    bDraggingChanged = true;
}

//--------------------------------------------------------------
void ofxScrollView::zoomDown(int x, int y, int id) {
    if(zoomID == id) {
        return;
    }
    zoomID = id;
    
    zoomDownScreenPos = zoomMoveScreenPos = ofVec2f(x, y);
    zoomDownContentPos = screenPointToContentPoint(zoomDownScreenPos);
    
    scrollDown = scroll;
    scaleDown = scale;
    
    bZooming = true;
    bZoomingChanged = true;
}

void ofxScrollView::zoomMoved(int x, int y, int id) {
    if(zoomID != id) {
        return;
    }
    
    zoomMoveScreenPos.set(x, y);
}

void ofxScrollView::zoomUp(int x, int y, int id) {
    if(zoomID != id) {
        return;
    }
    
    zoomMoveScreenPos.set(x, y);
    
    zoomID = -1;
    
    bZooming = false;
    bZoomingChanged = true;
}

void ofxScrollView::zoomCancel() {
    zoomID = -1;
    
    bZooming = false;
    bZoomingChanged = true;
}

//--------------------------------------------------------------
void ofxScrollView::mouseMoved(int x, int y) {
    //
}

void ofxScrollView::mousePressed(int x, int y, int button) {
    bool bHit = windowRect.inside(x, y);
    if(bHit == false) {
        return;
    }
    
    if(button == 0) {
        dragDown(x, y, button);
    } else if(button == 2) {
        dragCancel();
        zoomDown(x, y, button);
    }
}

void ofxScrollView::mouseDragged(int x, int y, int button) {
    if(button == 0) {
        dragMoved(x, y, button);
    } else if(button == 2) {
        zoomMoved(x, y, button);
    }
}

void ofxScrollView::mouseReleased(int x, int y, int button) {
    if(button == 0) {
        dragUp(x, y, button);
    } else if(button == 2) {
        zoomUp(x, y, button);
    }
}

//--------------------------------------------------------------
void ofxScrollView::touchDown(int x, int y, int id) {
    //
}

void ofxScrollView::touchMoved(int x, int y, int id) {
    //
}

void ofxScrollView::touchUp(int x, int y, int id) {
    //
}

void ofxScrollView::touchDoubleTap(int x, int y, int id) {
    //
}

void ofxScrollView::touchCancelled(int x, int y, int id) {
    //
}
