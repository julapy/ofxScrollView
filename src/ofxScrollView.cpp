//
//  ofxScrollView.cpp
//  Created by Lukasz Karluk on 2/06/2014.
//  http://julapy.com/
//

#include "ofxScrollView.h"

//--------------------------------------------------------------
ofxScrollView::ofxScrollView() {
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
    scaleEased = 1.0;
    scaleEasing = 0.5;
    scaleDown = 1.0;
    scaleMin = 1.0;
    scaleMax = 1.0;
    scaleMultiplier = 1.0;
    
    setUserInteraction(true);
}

ofxScrollView::~ofxScrollView() {
    setUserInteraction(false);
}

//--------------------------------------------------------------
void ofxScrollView::setup() {
    if(windowRect.isEmpty() == true) {
        setWindowRect(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
    }
    
    if(contentRect.isEmpty() == true) {
        setContentRect(windowRect);
    }
}

void ofxScrollView::reset() {
    scrollPos.set(0);
    scrollPosEased.set(0);
    scrollPosDown.set(0);
    
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
}

void ofxScrollView::setContentRect(const ofRectangle & rect) {
    if(contentRect == rect) {
        return;
    }
    contentRect = rect;
}

//--------------------------------------------------------------
void ofxScrollView::setZoomMinMax(float min, float max) {
    scaleMin = min;
    scaleMax = min;
    scaleMultiplier = scaleMax / scaleMin;
    scale = scaleEased = ofClamp(scale, scaleMin, scaleMax);
}

void ofxScrollView::setZoomMultiplier(float value) {
    scaleMultiplier = value;
    scaleMax = scaleMin * scaleMultiplier;
    scale = scaleEased = ofClamp(scale, scaleMin, scaleMax);
}

void ofxScrollView::setZoomContentToFitContentRect() {
    float sx = windowRect.width / contentRect.width;
    float sy = windowRect.height / contentRect.height;
    scaleMin = MAX(sx, sy);
    scaleMax = scaleMin * scaleMultiplier;
    scale = scaleEased = ofClamp(scale, scaleMin, scaleMax);
}

void ofxScrollView::setZoom(float value) {
    scale = scaleEased = ofClamp(value, scaleMin, scaleMax);
}

void ofxScrollView::setZoomEasing(float value) {
    scaleEasing = value;
}

float ofxScrollView::getZoom() {
    return scale;
}

float ofxScrollView::getZoomMin() {
    return scaleMin;
}

float ofxScrollView::getZoomMax() {
    return scaleMax;
}

bool ofxScrollView::isZoomed() {
    return (scale > scaleMin);
}

void ofxScrollView::zoomTo(const ofVec2f & pos, float zoom) {
    //
}

void ofxScrollView::zoomMinTo(const ofVec2f & pos) {
    //
}

void ofxScrollView::zoomMaxTo(const ofVec2f & pos) {
    //
}

//--------------------------------------------------------------
void ofxScrollView::setScrollEasing(float value) {
    scrollEasing = value;
}

void ofxScrollView::setScrollPosition(float x, float y, bool bEase) {
    dragCancel();
    zoomCancel();

    scrollPos = ofVec2f(x, y);
    if(bEase == false) {
        scrollPosEased = scrollPos;
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
    return scrollPosEased;
}

const ofMatrix4x4 & ofxScrollView::getMatrix() {
    return mat;
}

//--------------------------------------------------------------
ofVec2f ofxScrollView::screenPointToContentPoint(const ofVec2f & screenPoint) {
    ofVec2f contentPoint;
    contentPoint.x = ofMap(screenPoint.x,
                           scrollRect.x,
                           scrollRect.x + scrollRect.width,
                           0,
                           contentRect.width,
                           true);
    contentPoint.y = ofMap(screenPoint.y,
                           scrollRect.y,
                           scrollRect.y + scrollRect.height,
                           0,
                           contentRect.height,
                           true);
    return contentPoint;
}

//--------------------------------------------------------------
void ofxScrollView::update() {
    
    if(bZooming == true) {
        
        ofVec2f zoomDiff = zoomMoveScreenPos - zoomDownScreenPos;
        float zoomUnitDist = ofVec2f(windowRect.width, windowRect.height).length(); // diagonal.
        float zoomRange = scaleMax - scaleMin;
        float zoom = ofMap(zoomDiff.x, -zoomUnitDist, zoomUnitDist, -zoomRange, zoomRange, true);
        scale = scaleDown + zoom;
        scale = MAX(scale, 0.0);
    }
    
    bool bContainZoom = true;
    bContainZoom = bContainZoom && (bZooming == false);

    if(bContainZoom == true) {
        scale = ofClamp(scale, scaleMin, scaleMax);
    }
    
    scaleEased += (scale - scaleEased) * scaleEasing;
    
    //----------------------------------------------------------
    scrollRect.width = contentRect.width * scaleEased;
    scrollRect.height = contentRect.height * scaleEased;
    
    //----------------------------------------------------------
    if(bZooming == false) {
        if(bDragging == true) {
            
            dragVel = dragMovePos - dragMovePosPrev;
            dragMovePosPrev = dragMovePos;
            scrollPos += dragVel;
            
        } else {
            
            dragVel *= 0.9;
            scrollPos += dragVel;
        }
    }
    
    //----------------------------------------------------------
    bool bContainPosition = true;
    bContainPosition = bContainPosition && (bDragging == false);
    bContainPosition = bContainPosition && (bZooming == false);
    
    if(bContainPosition == true) {
        
        float x0 = windowRect.x - MAX(scrollRect.width - windowRect.width, 0.0);
        float x1 = windowRect.x;
        float y0 = windowRect.y - MAX(scrollRect.height - windowRect.height, 0.0);
        float y1 = windowRect.y;
        
        if(scrollPos.x < x0) {
            scrollPos.x += (x0 - scrollPos.x) * bounceBack;
        } else if(scrollPos.x > x1) {
            scrollPos.x += (x1 - scrollPos.x) * bounceBack;
        }
        
        if(scrollPos.y < y0) {
            scrollPos.y += (y0 - scrollPos.y) * bounceBack;
        } else if(scrollPos.y > y1) {
            scrollPos.y += (y1 - scrollPos.y) * bounceBack;
        }
    }
    
    scrollPosEased += (scrollPos - scrollPosEased) * scrollEasing;
    
    scrollRect.x = scrollPosEased.x;
    scrollRect.y = scrollPosEased.y;
    
    //----------------------------------------------------------
    
    mat.makeIdentityMatrix();
    mat.preMultTranslate(scrollPosEased);
    mat.preMultScale(ofVec3f(scaleEased, scaleEased, 1.0));

    if(bZooming == true) {
        ofVec3f p0(zoomDownScreenPos.x, zoomDownScreenPos.y, 0);
        ofVec3f p1(mat.preMult(ofVec3f(zoomDownContentPos.x, zoomDownContentPos.y, 0)));
        ofVec3f p2 = p0 - p1;
        
        mat.postMultTranslate(p2);
        
        ofVec3f pos = mat.getTranslation();
        scrollPos = pos;
    }
    
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
    
    scrollPosDown = scrollPos;
    
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
    
    scrollPosDown = scrollPos;
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
    bool bHit = windowRect.inside(x, y);
    if(bHit == false) {
        return;
    }
    
    if(id == 0) {
        dragDown(x, y, id);
    }
}

void ofxScrollView::touchMoved(int x, int y, int id) {
    if(id == 0) {
        dragMoved(x, y, id);
    }
}

void ofxScrollView::touchUp(int x, int y, int id) {
    if(id == 0) {
        dragUp(x, y, id);
    }
}

void ofxScrollView::touchDoubleTap(int x, int y, int id) {
    //
}

void ofxScrollView::touchCancelled(int x, int y, int id) {
    //
}
