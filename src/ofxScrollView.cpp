//
//  ofxScrollView.cpp
//  Created by Lukasz Karluk on 2/06/2014.
//  http://julapy.com/
//

#include "ofxScrollView.h"

//--------------------------------------------------------------
static float const kEasingStop = 0.001;

//--------------------------------------------------------------
ofxScrollView::ofxScrollView() {
    bUserInteractionEnabled = false;
    bPinchZoom = false;
#ifdef TARGET_OPENGLES
    bPinchZoom = true;
#endif

    scrollEasing = 0.5;
    bounceBack = 1.0;
    
    dragVelDecay = 0.9;
    bDragging = false;
    bDraggingChanged = false;
    
    zoomDownPointDist = 0;
    zoomMovePointDist = 0;
    bZooming = false;
    bZoomingChanged = false;
    
    animTimeStart = 0;
    animTimeTotal = 0;
    animZoomTarget = 0;
    bAnimating = false;
    bAnimationJustStarted = false;
    bAnimationJustFinished = false;
    
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
        setWindowRect(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
    }
    
    if(contentRect.isEmpty() == true) {
        setContentRect(windowRect);
    }
    
    reset();
}

void ofxScrollView::reset() {
    scrollPos = ofVec2f(windowRect.x, windowRect.y);
    scrollPosEased = scrollPos;
    scrollPosDown = scrollPos;
    
    dragDownPos.set(0);
    dragMovePos.set(0);
    dragMovePosPrev.set(0);
    dragVel.set(0);
    bDragging = false;
    bDraggingChanged = false;
    
    zoomDownScreenPos.set(0);
    zoomMoveScreenPos.set(0);
    zoomDownContentPos.set(0);
    animTimeStart = 0;
    animTimeTotal = 0;
    animZoomTarget = 0;
    bZooming = false;
    bZoomingChanged = false;
    bAnimating = false;
    bAnimationJustStarted = false;
    bAnimationJustFinished = false;
    
    scale = scaleMin;
    scaleDown = scaleMin;
    mat.makeIdentityMatrix();
    
    bFirstUpdate = true;
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

void ofxScrollView::fitContentToWindow(ofAspectRatioMode aspectRatioMode, float zoomMultiplier) {
    float sx = windowRect.width / contentRect.width;
    float sy = windowRect.height / contentRect.height;
    
    if(aspectRatioMode == OF_ASPECT_RATIO_KEEP) {
        scaleMin = MIN(sx, sy);
    } else if(aspectRatioMode == OF_ASPECT_RATIO_KEEP_BY_EXPANDING) {
        scaleMin = MAX(sx, sy);
    } else {
        scaleMin = 1.0;
    }
    
    scaleMax = scaleMin * zoomMultiplier;
    scale = scaleMin;
}

void ofxScrollView::setZoom(float value) {
    scale = ofClamp(value, scaleMin, scaleMax);
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

bool ofxScrollView::isZoomedMin() {
    return (scale == scaleMin);
}

bool ofxScrollView::isZoomedMax() {
    return (scale == scaleMax);
}

//--------------------------------------------------------------
void ofxScrollView::zoomTo(const ofVec2f & pos, float zoom, float timeSec) {
    animPos = pos;
    animZoomTarget = ofClamp(zoom, scaleMin, scaleMax);
    animTimeStart = ofGetElapsedTimef();
    animTimeTotal = MAX(timeSec, 0.0);
    bAnimationJustStarted = true;
}

void ofxScrollView::zoomToMin(const ofVec2f & pos, float timeSec) {
    zoomTo(pos, scaleMin, timeSec);
}

void ofxScrollView::zoomToMax(const ofVec2f & pos, float timeSec) {
    zoomTo(pos, scaleMax, timeSec);
}

//--------------------------------------------------------------
void ofxScrollView::setScrollPositionX(float x, bool bEase) {
    dragCancel();
    zoomCancel();
    
    float px = ofClamp(x, 0.0, 1.0);
    scrollPos.x = windowRect.x - (scrollRect.width - windowRect.width) * px;
    if(bEase == false) {
        scrollPosEased.x = scrollPos.x;
    }
}

void ofxScrollView::setScrollPositionY(float y, bool bEase) {
    dragCancel();
    zoomCancel();
    
    float py = ofClamp(y, 0.0, 1.0);
    scrollPos.y = windowRect.y - (scrollRect.height - windowRect.height) * py;
    if(bEase == false) {
        scrollPosEased.y = scrollPos.y;
    }
}

void ofxScrollView::setScrollPosition(float x, float y, bool bEase) {
    setScrollPositionX(x, bEase);
    setScrollPositionY(y, bEase);
}

void ofxScrollView::setScrollEasing(float value) {
    scrollEasing = value;
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

void ofxScrollView::setDragVelocityDecay(float value) {
    dragVelDecay = value;
}

const ofRectangle & ofxScrollView::getWindowRect() {
    return windowRect;
}

const ofRectangle & ofxScrollView::getContentRect() {
    return contentRect;
}

const ofRectangle & ofxScrollView::getScrollRect() {
    return scrollRect;
}

const ofVec2f & ofxScrollView::getScrollPosition() {
    return scrollPosEased;
}

const ofVec2f & ofxScrollView::getScrollPositionNorm() {
    return scrollPosEasedNorm;
}

const ofMatrix4x4 & ofxScrollView::getMatrix() {
    return mat;
}

//--------------------------------------------------------------
void ofxScrollView::update() {

    //----------------------------------------------------------
    if(bAnimationJustStarted == true) {
        bAnimationJustStarted = false;
        
        dragCancel();
        zoomDown(animPos, 0);
        
        bAnimating = true;
    }
    
    //----------------------------------------------------------
    if(bZooming == true) {
        
        float zoom = 0.0;
        
        if(bAnimating == true) {
            
            float zoomDiff = animZoomTarget - scaleDown;
            
            if(animTimeTotal == 0) {
                zoom = animZoomTarget;
            } else {
                float timeNow = ofGetElapsedTimef();
                zoom = ofMap(timeNow,
                             animTimeStart,
                             animTimeStart + animTimeTotal,
                             0,
                             zoomDiff,
                             true);
            }
            
            bAnimating = (zoom != zoomDiff);
            if(bAnimating == false) {
                bAnimationJustFinished = true;
            }
            
        } else {

            float zoomUnitDist = ofVec2f(windowRect.width, windowRect.height).length(); // diagonal.
            float zoomRange = scaleMax - scaleMin;
            float zoomDiff = 0;
            
            if(bPinchZoom == true) {
                
                zoomDiff = zoomMovePointDist - zoomDownPointDist;
                zoomDiff *= 4;
                
            } else {
                
                zoomDiff = zoomMoveScreenPos.x - zoomDownScreenPos.x;
            }
            
            zoom = ofMap(zoomDiff, -zoomUnitDist, zoomUnitDist, -zoomRange, zoomRange, true);
        }
        
        scale = scaleDown + zoom;
        scale = MAX(scale, 0.0);
    }
    
    bool bContainZoom = true;
//    bContainZoom = bContainZoom && (bZooming == false);

    if(bContainZoom == true) {
        if(scale < scaleMin) {
            scale = scaleMin;
        } else if(scale > scaleMax) {
            scale = scaleMax;
        }
    }
    
    //----------------------------------------------------------
    scrollRect.width = contentRect.width * scale;
    scrollRect.height = contentRect.height * scale;
    
    //----------------------------------------------------------
    if(bZooming == false) {
        if(bDragging == true) {
            
            dragVel = dragMovePos - dragMovePosPrev;
            dragMovePosPrev = dragMovePos;
            scrollPos += dragVel;
            
        } else {
            
            dragVel *= dragVelDecay;
            if(ABS(dragVel.x) < kEasingStop) {
                dragVel.x = 0;
            }
            if(ABS(dragVel.y) < kEasingStop) {
                dragVel.y = 0;
            }
            bool bAddVel = true;
            bAddVel = bAddVel && (ABS(dragVel.x) > 0);
            bAddVel = bAddVel && (ABS(dragVel.y) > 0);
            if(bAddVel == true) {
                scrollPos += dragVel;
            }
        }
    }
    
    //----------------------------------------------------------
    bool bContainPosition = true;
//    bContainPosition = bContainPosition && (bDragging == false);
    bContainPosition = bContainPosition && (bZooming == false);
    
    if(bContainPosition == true) {
        
        ofRectangle boundingRect = windowRect;
        ofRectangle contentRectMin = contentRect;
        contentRectMin.width *= scaleMin;
        contentRectMin.height *= scaleMin;
        
        if(scrollRect.width < windowRect.width) {
            boundingRect.x = windowRect.x + (windowRect.width - contentRectMin.width) * 0.5;
            boundingRect.width = contentRectMin.width;
        }
        if(scrollRect.height < windowRect.height) {
            boundingRect.y = windowRect.y + (windowRect.height - contentRectMin.height) * 0.5;
            boundingRect.height = contentRectMin.height;
        }
        
        float x0 = boundingRect.x - MAX(scrollRect.width - boundingRect.width, 0.0);
        float x1 = boundingRect.x;
        float y0 = boundingRect.y - MAX(scrollRect.height - boundingRect.height, 0.0);
        float y1 = boundingRect.y;
        
        float scrollPosEasing = bounceBack;
        if(bFirstUpdate == true) {
            scrollPosEasing = 1.0;
        }
        
        if(scrollPos.x < x0) {
            scrollPos.x += (x0 - scrollPos.x) * scrollPosEasing;
            if(ABS(x0 - scrollPos.x) < kEasingStop) {
                scrollPos.x = x0;
            }
        } else if(scrollPos.x > x1) {
            scrollPos.x += (x1 - scrollPos.x) * scrollPosEasing;
            if(ABS(x1 - scrollPos.x) < kEasingStop) {
                scrollPos.x = x1;
            }
        }
        
        if(scrollPos.y < y0) {
            scrollPos.y += (y0 - scrollPos.y) * scrollPosEasing;
            if(ABS(y0 - scrollPos.y) < kEasingStop) {
                scrollPos.y = y0;
            }
        } else if(scrollPos.y > y1) {
            scrollPos.y += (y1 - scrollPos.y) * scrollPosEasing;
            if(ABS(y1 - scrollPos.y) < kEasingStop) {
                scrollPos.y = y1;
            }
        }
    }
    
    //----------------------------------------------------------
    if(bFirstUpdate == true) {
        scrollPosEased = scrollPos;
    }
    scrollPosEased += (scrollPos - scrollPosEased) * scrollEasing;

    if(ABS(scrollPos.x - scrollPosEased.x) < kEasingStop) {
        scrollPosEased.x = scrollPos.x;
    }
    if(ABS(scrollPos.y - scrollPosEased.y) < kEasingStop) {
        scrollPosEased.y = scrollPos.y;
    }
    
    //----------------------------------------------------------
    
    mat.makeIdentityMatrix();
    mat.preMultTranslate(scrollPosEased);
    mat.preMultScale(ofVec3f(scale, scale, 1.0));

    if(bZooming == true) {
        ofVec3f p0(zoomDownScreenPos.x, zoomDownScreenPos.y, 0);
        ofVec3f p1(mat.preMult(ofVec3f(zoomDownContentPos.x, zoomDownContentPos.y, 0)));
        ofVec3f p2 = p0 - p1;
        
        mat.postMultTranslate(p2);
        scrollPos = scrollPosEased = mat.getTranslation();
        
    } else {
        
        //
    }
    
    //----------------------------------------------------------
    scrollRect.x = scrollPosEased.x;
    scrollRect.y = scrollPosEased.y;
    
    float dx = windowRect.width - scrollRect.width;
    float dy = windowRect.height - scrollRect.height;
    if(dx >= 0) {
        scrollPosEasedNorm.x = 0;
    } else {
        scrollPosEasedNorm.x = ofMap(scrollPosEased.x, dx, 0.0, 1.0, 0.0, true);
    }
    if(dy >= 0) {
        scrollPosEasedNorm.y = 0;
    } else {
        scrollPosEasedNorm.y = ofMap(scrollPosEased.y, dy, 0.0, 1.0, 0.0, true);
    }
    
    //----------------------------------------------------------
    bDraggingChanged = false;
    bZoomingChanged = false;
    
    if(bAnimationJustFinished == true) {
        bAnimationJustFinished = false;
        zoomCancel();
    }
    
    bFirstUpdate = false;
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
void ofxScrollView::dragDown(const ofVec2f & point) {
    dragDownPos = dragMovePos = dragMovePosPrev = point;
    dragVel.set(0);
    
    scrollPosDown = scrollPos;
    
    bDragging = true;
    bDraggingChanged = true;
}

void ofxScrollView::dragMoved(const ofVec2f & point) {
    dragMovePos = point;
}

void ofxScrollView::dragUp(const ofVec2f & point) {
    dragMovePos = point;
    
    bDragging = false;
    bDraggingChanged = true;
}

void ofxScrollView::dragCancel() {
    dragVel.set(0);
    
    bDragging = false;
    bDraggingChanged = true;
}

//--------------------------------------------------------------
void ofxScrollView::zoomDown(const ofVec2f & point, float pointDist) {
    zoomDownScreenPos = zoomMoveScreenPos = point;
    zoomDownContentPos.x = ofMap(zoomDownScreenPos.x, scrollRect.x, scrollRect.x + scrollRect.width, 0, contentRect.width, true);
    zoomDownContentPos.y = ofMap(zoomDownScreenPos.y, scrollRect.y, scrollRect.y + scrollRect.height, 0, contentRect.height, true);
    
    zoomDownPointDist = pointDist;
    
    scaleDown = scale;
    
    bZooming = true;
    bZoomingChanged = true;
    bAnimating = false;
}

void ofxScrollView::zoomMoved(const ofVec2f & point, float pointDist) {
    zoomMoveScreenPos = point;
    zoomMovePointDist = pointDist;
}

void ofxScrollView::zoomUp(const ofVec2f & point, float pointDist) {
    zoomMoveScreenPos = point;
    zoomMovePointDist = pointDist;
    
    bZooming = false;
    bZoomingChanged = true;
}

void ofxScrollView::zoomCancel() {
    bZooming = false;
    bZoomingChanged = true;
    bAnimating = false;
}

//--------------------------------------------------------------
void ofxScrollView::mouseMoved(int x, int y) {
    //
}

void ofxScrollView::mousePressed(int x, int y, int button) {
    if(button == 0) {
        
        touchDown(x, y, 0);
        
    } else if(button == 2) {
        
        touchDown(x, y, 0);
        touchDown(x, y, 2);
    }
}

void ofxScrollView::mouseDragged(int x, int y, int button) {
    touchMoved(x, y, button);
}

void ofxScrollView::mouseReleased(int x, int y, int button) {
    touchUp(x, y, button);
}

//--------------------------------------------------------------
void ofxScrollView::touchDown(int x, int y, int id) {
    bool bHit = windowRect.inside(x, y);
    if(bHit == false) {
        return;
    }
    
    if(touchPoints.size() >= 2) {
        // max 2 touches.
        return;
    }
    
    touchPoints.push_back(new ofxScrollViewTouchPoint(x, y, id));
    
    if(touchPoints.size() == 1) {
        
        zoomCancel();
        dragDown(touchPoints[0]->touchPos);
        
    } else if(touchPoints.size() == 2) {
        
        ofVec2f tp0(touchPoints[0]->touchPos);
        ofVec2f tp1(touchPoints[1]->touchPos);
        ofVec2f tmp = (tp1 - tp0) * 0.5 + tp0;
        float dist = (tp1 - tp0).length();
        
        dragCancel();
        zoomDown(tmp, dist);
    }
}

void ofxScrollView::touchMoved(int x, int y, int id) {
    int touchIndex = -1;
    for(int i=0; i<touchPoints.size(); i++) {
        ofxScrollViewTouchPoint & touchPoint = *touchPoints[i];
        if(touchPoint.touchID == id) {
            touchPoint.touchPos.x = x;
            touchPoint.touchPos.y = y;
            touchIndex = i;
            break;
        }
    }
    
    if(touchIndex == -1) {
        return;
    }
    
    if(touchPoints.size() == 1) {
        
        dragMoved(touchPoints[0]->touchPos);
        
    } else if(touchPoints.size() == 2) {
        
        ofVec2f tp0(touchPoints[0]->touchPos);
        ofVec2f tp1(touchPoints[1]->touchPos);
        ofVec2f tmp = (tp1 - tp0) * 0.5 + tp0;
        float dist = (tp1 - tp0).length();
        
        zoomMoved(tmp, dist);
    }
}

void ofxScrollView::touchUp(int x, int y, int id) {
    int touchIndex = -1;
    for(int i=0; i<touchPoints.size(); i++) {
        ofxScrollViewTouchPoint & touchPoint = *touchPoints[i];
        if(touchPoint.touchID == id) {
            touchPoint.touchPos.x = x;
            touchPoint.touchPos.y = y;
            touchIndex = i;
            break;
        }
    }
    
    if(touchIndex == -1) {
        return;
    }
    
    if(touchPoints.size() == 1) {
        
        dragUp(touchPoints[0]->touchPos);
        
    } else if(touchPoints.size() == 2) {
     
        ofVec2f tp0(touchPoints[0]->touchPos);
        ofVec2f tp1(touchPoints[1]->touchPos);
        ofVec2f tmp = (tp1 - tp0) * 0.5 + tp0;
        float dist = (tp1 - tp0).length();
        
        zoomUp(tmp, dist);
    }
    
    for(int i=0; i<touchPoints.size(); i++) {
        delete touchPoints[i];
        touchPoints[i] = NULL;
    }
    touchPoints.clear();
    
//    delete touchPoints[touchIndex];
//    touchPoints[touchIndex] = NULL;
//    touchPoints.erase(touchPoints.begin()+touchIndex);
}

void ofxScrollView::touchDoubleTap(int x, int y, int id) {
    //
}

void ofxScrollView::touchCancelled(int x, int y, int id) {
    //
}
