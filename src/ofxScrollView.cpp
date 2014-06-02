//
//  ofxScrollView.cpp
//  Created by Lukasz Karluk on 2/06/2014.
//

#include "ofxScrollView.h"

//--------------------------------------------------------------
ofxScrollView::ofxScrollView() {
    setUserInteraction(true);
}

ofxScrollView::~ofxScrollView() {
    setUserInteraction(false);
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

void ofxScrollView::setScrollEasing(float easing) {
    scrollEasing = easing;
}

void ofxScrollView::setUserInteraction(bool bEnable) {
    if(bUserInteractionEnabled == bEnable) {
        return;
    }
    if(bUserInteractionEnabled == true) {
        bUserInteractionEnabled = false;
        
#ifdef TARGET_OPENGLES
        ofAddListener(ofEvents().touchDown, this, &ofxScrollView::touchDown);
        ofAddListener(ofEvents().touchMoved, this, &ofxScrollView::touchMoved);
        ofAddListener(ofEvents().touchUp, this, &ofxScrollView::touchUp);
#else
        ofAddListener(ofEvents().mousePressed, this, &ofxScrollView::mousePressed);
        ofAddListener(ofEvents().mouseDragged, this, &ofxScrollView::mouseDragged);
        ofAddListener(ofEvents().mouseReleased, this, &ofxScrollView::mouseReleased);
#endif
        
    } else {
        bUserInteractionEnabled = true;
        
#ifdef TARGET_OPENGLES
        ofRemoveListener(ofEvents().touchDown, this, &ofxScrollView::touchDown);
        ofRemoveListener(ofEvents().touchMoved, this, &ofxScrollView::touchMoved);
        ofRemoveListener(ofEvents().touchUp, this, &ofxScrollView::touchUp);
#else
        ofRemoveListener(ofEvents().mousePressed, this, &ofxScrollView::mousePressed);
        ofRemoveListener(ofEvents().mouseDragged, this, &ofxScrollView::mouseDragged);
        ofRemoveListener(ofEvents().mouseReleased, this, &ofxScrollView::mouseReleased);
#endif
    }
}

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
    float px = (screenPoint.x - scrollRect.x) / scrollRect.width;
    float py = (screenPoint.y - scrollRect.y) / scrollRect.height;
    px = ofClamp(px, 0.0, 1.0);
    py = ofClamp(py, 0.0, 1.0);
    
    ofVec3f contentPoint;
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
        float zoom = zoomDiff.x / zoomUnitDist;
        zoom = MAX(MIN(zoom, 1.0), -1.0);
        float scaleDiff = zoom;
        scale = scaleDown + scaleDiff;
        
    } else {
        
    }
    
    scale = ofClamp(scale, scaleMin, scaleMax);
    
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
            ofVec2f scrollDiff = dragDiff / scrollSize;
            
            scroll = scrollDown + scrollDiff;
            
        } else {
            
            dragVel *= 0.9;
            scroll += -(dragVel / scrollSize);
        }
    }
    
    //----------------------------------------------------------
    scroll.x = ofClamp(scroll.x, 0.0, 1.0);
    scroll.y = ofClamp(scroll.y, 0.0, 1.0);
    scrollEased += (scroll - scrollEased) * scrollEasing;
    
    //----------------------------------------------------------
    
    ofVec2f offset = scrollSize * scrollEased;
    mat.makeIdentityMatrix();
    mat.preMultTranslate(ofVec3f(-offset.x, -offset.y, 0));
    mat.preMultScale(ofVec3f(scale, scale, 1.0));
    
    scrollRect = transformRect(contentRect, mat);
    
    //----------------------------------------------------------
    bZoomingChanged = false;
    bDraggingChanged = false;
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
        
        ofVec3f point(zoomDownContentPos.x, zoomDownContentPos.y, 0);
        point = mat.preMult(point);
        
        ofSetColor(ofColor::red);
        ofCircle(point, 10);
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
