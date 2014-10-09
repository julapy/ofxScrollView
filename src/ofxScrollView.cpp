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
    bPinchZoomEnabled = false;
    bPinchZoomSupported = false;
    
    scrollEasing = 0.5;
    bounceBack = 1.0;
    
    dragVelDecay = 0.9;
    bDragging = false;
    
    zoomDownDist = 0;
    zoomMoveDist = 0;
    bZooming = false;
    
    animTimeStart = 0.0;
    animTimeTotal = 0.0;
    bAnimating = false;
    
    bDoubleTapZoomEnabled = false;
    doubleTapZoomRangeMin = 0.0;
    doubleTapZoomRangeMax = 1.0;
    doubleTapZoomIncrement = 1.0;
    doubleTapZoomIncrementTimeInSec = 0.2;
    doubleTapRegistrationTimeInSec = 0.25;
    doubleTapRegistrationDistanceInPixels = 22;
    
    scale = 1.0;
    scaleDown = 1.0;
    scaleMin = 1.0;
    scaleMax = 1.0;
    
    setUserInteraction(true);
    setPinchZoom(true);
    setDoubleTapZoom(true);
    
#ifdef TARGET_OPENGLES
    bPinchZoomSupported = true;
#endif
}

ofxScrollView::~ofxScrollView() {
    setUserInteraction(false);
}

//--------------------------------------------------------------
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

void ofxScrollView::setPinchZoom(bool value) {
    bPinchZoomEnabled = value;
}

void ofxScrollView::setScrollEasing(float value) {
    scrollEasing = value;
}

void ofxScrollView::setBounceBack(float value) {
    bounceBack = value;
}

void ofxScrollView::setDragVelocityDecay(float value) {
    dragVelDecay = value;
}

void ofxScrollView::setDoubleTapZoom(bool value) {
    bDoubleTapZoomEnabled = value;
}

void ofxScrollView::setDoubleTapZoomRangeMin(float value) {
    doubleTapZoomRangeMin = value;
}

void ofxScrollView::setDoubleTapZoomRangeMax(float value) {
    doubleTapZoomRangeMax = value;
}

void ofxScrollView::setDoubleTapZoomIncrement(float value) {
    doubleTapZoomIncrement = value;
}

void ofxScrollView::setDoubleTapZoomIncrementTimeInSec(float value) {
    doubleTapZoomIncrementTimeInSec = value;
}

void ofxScrollView::setDoubleTapRegistrationTimeInSec(float value) {
    doubleTapRegistrationTimeInSec = value;
}

void ofxScrollView::setDoubleTapRegistrationDistanceInPixels(float value) {
    doubleTapRegistrationDistanceInPixels = value;
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
    touchPoints.clear();
    
    dragDownPos.set(0);
    dragMovePos.set(0);
    dragMovePosPrev.set(0);
    dragVel.set(0);
    bDragging = false;
    
    zoomDownPos.set(0);
    zoomMovePos.set(0);
    zoomMovePosPrev.set(0);
    bZooming = false;
    
    animTimeStart = 0.0;
    animTimeTotal = 0.0;
    bAnimating = false;
    
    scale = scaleMin;
    scaleDown = scaleMin;
    
    scrollRect.width = scrollRectEased.width = contentRect.width * scale;
    scrollRect.height = scrollRectEased.height = contentRect.height * scale;
    scrollRect = scrollRectEased = getRectContainedInWindowRect(scrollRect);
    
    mat = getMatrixForRect(scrollRect);
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
void ofxScrollView::fitContentToWindow(ofAspectRatioMode aspectRatioMode) {
    float sx = windowRect.width / contentRect.width;
    float sy = windowRect.height / contentRect.height;
    
    if(aspectRatioMode == OF_ASPECT_RATIO_KEEP) {
        scaleMin = MIN(sx, sy);
    } else if(aspectRatioMode == OF_ASPECT_RATIO_KEEP_BY_EXPANDING) {
        scaleMin = MAX(sx, sy);
    } else {
        scaleMin = 1.0;
    }
    
    scaleMin = MIN(scaleMin, 1.0);
    scaleMax = 1.0;
    scale = scaleMin;
}

//--------------------------------------------------------------
void ofxScrollView::setScale(float value) {
    scale = value;
    scale = ofClamp(scale, scaleMin, scaleMax);
}

void ofxScrollView::setScaleMin(float value) {
    scaleMin = value;
    scale = ofClamp(scale, scaleMin, scaleMax);
}

void ofxScrollView::setScaleMax(float value) {
    scaleMax = value;
    scale = ofClamp(scale, scaleMin, scaleMax);
}

//--------------------------------------------------------------
float ofxScrollView::getScale() {
    return scale;
}

float ofxScrollView::getScaleMin() {
    return scaleMin;
}

float ofxScrollView::getScaleMax() {
    return scaleMax;
}

//--------------------------------------------------------------
void ofxScrollView::setZoom(float value) {
    float zoom = ofClamp(value, 0.0, 1.0);
    scale = zoomToScale(zoom);
}

float ofxScrollView::getZoom() {
    float zoom = scaleToZoom(scale);
    return zoom;
}

bool ofxScrollView::isZoomed() {
    float zoom = getZoom();
    return (zoom > 0.0);
}

bool ofxScrollView::isZoomedInMax() {
    float zoom = getZoom();
    return (zoom == 1.0);
}

bool ofxScrollView::isZoomedOutMax() {
    float zoom = getZoom();
    return (zoom == 0.0);
}

//--------------------------------------------------------------
float ofxScrollView::zoomToScale(float value) {
    if(scaleMin == scaleMax) {
        return scaleMin;
    }
    return ofMap(value, 0.0, 1.0, scaleMin, scaleMax, true);
}

float ofxScrollView::scaleToZoom(float value) {
    if(scaleMin == scaleMax) {
        return 0.0;
    }
    return ofMap(value, scaleMin, scaleMax, 0.0, 1.0, true);
}

//--------------------------------------------------------------
void ofxScrollView::zoomToMin(const ofVec2f & screenPoint, float timeSec) {
    zoomTo(screenPoint, scaleMin, timeSec);
}

void ofxScrollView::zoomToMax(const ofVec2f & screenPoint, float timeSec) {
    zoomTo(screenPoint, scaleMax, timeSec);
}

void ofxScrollView::zoomTo(const ofVec2f & screenPoint, float zoom, float timeSec) {
    bool bAnimate = animStart(timeSec);
    
    scrollRectAnim0 = scrollRect;
    scrollRectAnim1 = scrollRect;
    scrollRectAnim1 = getRectZoomedAtScreenPoint(scrollRectAnim1, screenPoint, zoom);
    scrollRectAnim1 = getRectContainedInWindowRect(scrollRectAnim1);
    
    if(bAnimate == false) {
        scrollRect = scrollRectEased = scrollRectAnim1;
    }
}

void ofxScrollView::zoomToContentPointAndPositionAtScreenPoint(const ofVec2f & contentPoint,
                                                               const ofVec2f & screenPoint,
                                                               float zoom,
                                                               float timeSec) {
    bool bAnimate = animStart(timeSec);

    scrollRectAnim0 = scrollRect;
    scrollRectAnim1 = scrollRect;
    scrollRectAnim1 = getRectWithContentPointAtScreenPoint(scrollRectAnim1, contentPoint, screenPoint);
    scrollRectAnim1 = getRectZoomedAtScreenPoint(scrollRectAnim1, screenPoint, zoom);
    scrollRectAnim1 = getRectContainedInWindowRect(scrollRectAnim1);
    
    if(bAnimate == false) {
        scrollRect = scrollRectEased = scrollRectAnim1;
    }
}

void ofxScrollView::moveContentPointToScreenPoint(const ofVec2f & contentPoint,
                                                  const ofVec2f & screenPoint,
                                                  float timeSec) {
    bool bAnimate = animStart(timeSec);

    scrollRectAnim0 = scrollRect;
    scrollRectAnim1 = scrollRect;
    scrollRectAnim1 = getRectWithContentPointAtScreenPoint(scrollRectAnim1, contentPoint, screenPoint);
    
    if(bAnimate == false) {
        scrollRect = scrollRectEased = scrollRectAnim1;
    }
}


bool ofxScrollView::animStart(float animTimeInSec) {
    bAnimating = true;

    animTimeStart = ofGetElapsedTimef();
    animTimeTotal = MAX(animTimeInSec, 0.0);
    
    if(animTimeTotal < 0.001) {
        animTimeTotal = 0;
        bAnimating = false;
    }
    
    return bAnimating;
}

//--------------------------------------------------------------
void ofxScrollView::setScrollPositionX(float x, bool bEase) {
    dragCancel();
    zoomCancel();
    
    float px = ofClamp(x, 0.0, 1.0);
    scrollRect.x = windowRect.x - (scrollRect.width - windowRect.width) * px;
    if(bEase == false) {
        scrollRectEased.x = scrollRect.x;
    }
}

void ofxScrollView::setScrollPositionY(float y, bool bEase) {
    dragCancel();
    zoomCancel();
    
    float py = ofClamp(y, 0.0, 1.0);
    scrollRect.y = windowRect.y - (scrollRect.height - windowRect.height) * py;
    if(bEase == false) {
        scrollRectEased.y = scrollRect.y;
    }
}

void ofxScrollView::setScrollPosition(float x, float y, bool bEase) {
    setScrollPositionX(x, bEase);
    setScrollPositionY(y, bEase);
}

ofVec2f ofxScrollView::getScrollPosition() {
    return ofVec2f(scrollRectEased.x, scrollRectEased.y);
}

ofVec2f ofxScrollView::getScrollPositionNorm() {
    ofVec2f scrollPosEasedNorm;
    
    float dx = windowRect.width - scrollRect.width;
    float dy = windowRect.height - scrollRect.height;
    if(dx >= 0) {
        scrollPosEasedNorm.x = 0;
    } else {
        scrollPosEasedNorm.x = ofMap(scrollRectEased.x, dx, 0.0, 1.0, 0.0, true);
    }
    if(dy >= 0) {
        scrollPosEasedNorm.y = 0;
    } else {
        scrollPosEasedNorm.y = ofMap(scrollRectEased.y, dy, 0.0, 1.0, 0.0, true);
    }
    
    return scrollPosEasedNorm;
}

//--------------------------------------------------------------
const ofRectangle & ofxScrollView::getWindowRect() {
    return windowRect;
}

const ofRectangle & ofxScrollView::getContentRect() {
    return contentRect;
}

const ofRectangle & ofxScrollView::getScrollRect() {
    return scrollRect;
}

const ofMatrix4x4 & ofxScrollView::getMatrix() {
    return mat;
}

//--------------------------------------------------------------
void ofxScrollView::update() {
    
    if(bAnimating == true) {
        
        float timeNow = ofGetElapsedTimef();
        float progress = ofMap(timeNow, animTimeStart, animTimeStart + animTimeTotal, 0.0, 1.0, true);
        bAnimating = (progress < 1.0);
        
        ofRectangle rect = getRectLerp(scrollRectAnim0, scrollRectAnim1, progress);
        scrollRect = rect;
        
        scale = scrollRect.width / contentRect.width;
        
    } else {
        
        //==========================================================
        // dragging.
        //==========================================================
        
        if(bDragging == true || bZooming == true) {
            
            if(bDragging == true) {
                
                dragVel = dragMovePos - dragMovePosPrev;
                dragMovePosPrev = dragMovePos;
                
            } else if(bZooming == true) {
                
                dragVel = zoomMovePos - zoomMovePosPrev;
                zoomMovePosPrev = zoomMovePos;
            }
            
            scrollRect.x += dragVel.x;
            scrollRect.y += dragVel.y;
            
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
                scrollRect.x += dragVel.x;
                scrollRect.y += dragVel.y;
            }
        }
        
        //==========================================================
        // zooming.
        //==========================================================
        
        if(bZooming == true) {
            
            float zoomUnitDist = ofVec2f(windowRect.width, windowRect.height).length(); // diagonal.
            float zoomRange = scaleMax - scaleMin;
            float zoomDiff = 0;
            
            if(bPinchZoomSupported == true) {
                
                zoomDiff = zoomMoveDist - zoomDownDist;
                zoomDiff *= 4;
                
            } else {
                
                zoomDiff = zoomMovePos.x - zoomDownPos.x;
            }
            
            float zoom = ofMap(zoomDiff, -zoomUnitDist, zoomUnitDist, -zoomRange, zoomRange, true);
            
            scale = scaleDown + zoom;
            scale = MAX(scale, 0.0);
            
            if(scale < scaleMin) {
                scale = scaleMin;
            } else if(scale > scaleMax) {
                scale = scaleMax;
            }
            
            float zoomScale = scaleToZoom(scale);
            ofRectangle rect = getRectZoomedAtScreenPoint(scrollRect, zoomMovePos, zoomScale);
            scrollRect = rect;
        }
    }
    
    scrollRect = getRectContainedInWindowRect(scrollRect, bounceBack);
    
    //==========================================================
    // apply easing to scrollRect.
    //==========================================================
    
    scrollRectEased.x += (scrollRect.x - scrollRectEased.x) * scrollEasing;
    scrollRectEased.y += (scrollRect.y - scrollRectEased.y) * scrollEasing;
    scrollRectEased.width += (scrollRect.width - scrollRectEased.width) * scrollEasing;
    scrollRectEased.height += (scrollRect.height - scrollRectEased.height) * scrollEasing;
    
    if(ABS(scrollRect.x - scrollRectEased.x) < kEasingStop) {
        scrollRectEased.x = scrollRect.x;
    }
    if(ABS(scrollRect.y - scrollRectEased.y) < kEasingStop) {
        scrollRectEased.y = scrollRect.y;
    }
    if(ABS(scrollRect.width - scrollRectEased.width) < kEasingStop) {
        scrollRectEased.width = scrollRect.width;
    }
    if(ABS(scrollRect.height - scrollRectEased.height) < kEasingStop) {
        scrollRectEased.height = scrollRect.height;
    }
    
    mat = getMatrixForRect(scrollRectEased);
}

//-------------------------------------------------------------- the brains!
ofRectangle ofxScrollView::getRectContainedInWindowRect(const ofRectangle & rectToContain,
                                                        float easing) {

    ofRectangle rect = rectToContain;
    ofRectangle boundingRect = windowRect;
    ofRectangle contentRectMin = contentRect;
    contentRectMin.width *= scaleMin;
    contentRectMin.height *= scaleMin;
    
    if(rect.width < windowRect.width) {
        boundingRect.x = windowRect.x + (windowRect.width - contentRectMin.width) * 0.5;
        boundingRect.width = contentRectMin.width;
    }
    if(rect.height < windowRect.height) {
        boundingRect.y = windowRect.y + (windowRect.height - contentRectMin.height) * 0.5;
        boundingRect.height = contentRectMin.height;
    }
    
    float x0 = boundingRect.x - MAX(rect.width - boundingRect.width, 0.0);
    float x1 = boundingRect.x;
    float y0 = boundingRect.y - MAX(rect.height - boundingRect.height, 0.0);
    float y1 = boundingRect.y;
    
    if(rect.x < x0) {
        rect.x += (x0 - rect.x) * easing;
        if(ABS(x0 - rect.x) < kEasingStop) {
            rect.x = x0;
        }
    } else if(rect.x > x1) {
        rect.x += (x1 - rect.x) * easing;
        if(ABS(x1 - rect.x) < kEasingStop) {
            rect.x = x1;
        }
    }
    
    if(rect.y < y0) {
        rect.y += (y0 - rect.y) * easing;
        if(ABS(y0 - rect.y) < kEasingStop) {
            rect.y = y0;
        }
    } else if(rect.y > y1) {
        rect.y += (y1 - rect.y) * easing;
        if(ABS(y1 - rect.y) < kEasingStop) {
            rect.y = y1;
        }
    }
    
    return rect;
}

ofRectangle ofxScrollView::getRectZoomedAtScreenPoint(const ofRectangle & rect,
                                                      const ofVec2f & screenPoint,
                                                      float zoom) {
    
    float zoomScale = zoomToScale(zoom);
    
    ofVec2f contentPoint = getContentPointAtScreenPoint(rect, screenPoint);
    
    ofVec2f p0(0, 0);
    ofVec2f p1(contentRect.width, contentRect.height);
    p0 -= contentPoint;
    p1 -= contentPoint;
    p0 *= zoomScale;
    p1 *= zoomScale;
    p0 += screenPoint;
    p1 += screenPoint;
    
    ofRectangle rectNew;
    rectNew.x = p0.x;
    rectNew.y = p0.y;
    rectNew.width = p1.x - p0.x;
    rectNew.height = p1.y - p0.y;
    
    return rectNew;
}

ofRectangle ofxScrollView::getRectWithContentPointAtScreenPoint(const ofRectangle & rect,
                                                                const ofVec2f & contentPoint,
                                                                const ofVec2f & screenPoint) {
    
    ofVec2f contentScreenPoint = getScreenPointAtContentPoint(rect, contentPoint);
    ofVec2f contentPointToScreenPointDifference = screenPoint - contentScreenPoint;
    
    ofRectangle rectNew;
    rectNew = scrollRect;
    rectNew.x += contentPointToScreenPointDifference.x;
    rectNew.y += contentPointToScreenPointDifference.y;
    
    return rectNew;
}

ofRectangle ofxScrollView::getRectLerp(const ofRectangle & rectFrom,
                                       const ofRectangle & rectTo,
                                       float progress) {
    
    ofVec3f r00 = rectFrom.getTopLeft();
    ofVec3f r01 = rectFrom.getBottomRight();
    
    ofVec3f r10 = rectTo.getTopLeft();
    ofVec3f r11 = rectTo.getBottomRight();
    
    ofVec3f r20 = r00.interpolate(r10, progress);
    ofVec3f r21 = r01.interpolate(r11, progress);
    
    ofRectangle rect;
    rect.set(r20, r21);
    
    return rect;
}

ofMatrix4x4 ofxScrollView::getMatrixForRect(const ofRectangle & rect) {
    
    float rectScale = rect.width / contentRect.width;
    
    ofMatrix4x4 rectMat;
    rectMat.preMultTranslate(ofVec3f(rect.x, rect.y, 0.0));
    rectMat.preMultScale(ofVec3f(rectScale, rectScale, 1.0));
    
    return rectMat;
}

ofVec2f ofxScrollView::getContentPointAtScreenPoint(const ofRectangle & rect,
                                                    const ofVec2f & screenPoint) {
    
    ofVec2f contentPoint;
    contentPoint.x = ofMap(screenPoint.x, rect.x, rect.x + rect.width, 0, contentRect.width, true);
    contentPoint.y = ofMap(screenPoint.y, rect.y, rect.y + rect.height, 0, contentRect.height, true);
    return contentPoint;
}

ofVec2f ofxScrollView::getScreenPointAtContentPoint(const ofRectangle & rect,
                                                    const ofVec2f & contentPoint) {
    
    ofVec2f screenPoint;
    screenPoint.x = ofMap(contentPoint.x, 0, contentRect.width, rect.x, rect.x + rect.width, true);
    screenPoint.y = ofMap(contentPoint.y, 0, contentRect.height, rect.y, rect.y + rect.height, true);
    return screenPoint;
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
    //
}

//--------------------------------------------------------------
void ofxScrollView::exit() {
    //
}

//--------------------------------------------------------------
void ofxScrollView::dragDown(const ofVec2f & point) {
    dragDownPos = dragMovePos = dragMovePosPrev = point;
    dragVel.set(0);
    
    bDragging = true;
    bAnimating = false;
}

void ofxScrollView::dragMoved(const ofVec2f & point) {
    dragMovePos = point;
}

void ofxScrollView::dragUp(const ofVec2f & point) {
    dragMovePos = point;
    
    bDragging = false;
}

void ofxScrollView::dragCancel() {
    dragVel.set(0);
    
    bDragging = false;
}

//--------------------------------------------------------------
void ofxScrollView::zoomDown(const ofVec2f & point, float pointDist) {
    if(bPinchZoomEnabled == false) {
        return;
    }
    
    zoomDownPos = zoomMovePos = zoomMovePosPrev = point;
    zoomDownDist = zoomMoveDist = pointDist;
    
    scaleDown = scale;
    
    bZooming = true;
    bAnimating = false;
}

void ofxScrollView::zoomMoved(const ofVec2f & point, float pointDist) {
    if(bPinchZoomEnabled == false) {
        return;
    }
    
    zoomMovePos = point;
    zoomMoveDist = pointDist;
}

void ofxScrollView::zoomUp(const ofVec2f & point, float pointDist) {
    if(bPinchZoomEnabled == false) {
        return;
    }
    
    zoomMovePos = point;
    zoomMoveDist = pointDist;
    
    bZooming = false;
}

void ofxScrollView::zoomCancel() {
    bZooming = false;
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

    ofxScrollViewTouchPoint touchPointNew;
    touchPointNew.touchPos.set(x, y);
    touchPointNew.touchID = id;
    touchPointNew.touchDownTimeInSec = ofGetElapsedTimef();
    
    //---------------------------------------------------------- double tap.
    ofVec2f touchPointDiff = touchPointNew.touchPos - touchDownPointLast.touchPos;
    float touchTimeDiff = touchPointNew.touchDownTimeInSec - touchDownPointLast.touchDownTimeInSec;
    
    touchDownPointLast = touchPointNew;
    
    bool bDoubleTap = true;
    bDoubleTap = bDoubleTap && (touchTimeDiff < doubleTapRegistrationTimeInSec);
    bDoubleTap = bDoubleTap && (touchPointDiff.length() < doubleTapRegistrationDistanceInPixels);
    
    if(bDoubleTapZoomEnabled == true &&
       bDoubleTap == true) {
        
        dragCancel();
        zoomCancel();

        touchPoints.clear();
        touchDownPointLast.touchPos.set(0, 0);
        touchDownPointLast.touchDownTimeInSec = 0.0;
        
        touchDoubleTap(x, y, id);
        return;
    }
    
    //----------------------------------------------------------
    if(touchPoints.size() >= 2) {
        // max 2 touches.
        return;
    }
    
    touchPoints.push_back(touchPointNew);
    
    if(touchPoints.size() == 1) {
        
        zoomCancel();
        dragDown(touchPoints[0].touchPos);
        
    } else if(touchPoints.size() == 2) {
        
        ofVec2f tp0(touchPoints[0].touchPos);
        ofVec2f tp1(touchPoints[1].touchPos);
        ofVec2f tmp = (tp1 - tp0) * 0.5 + tp0;
        float dist = (tp1 - tp0).length();
        
        dragCancel();
        zoomDown(tmp, dist);
    }
}

void ofxScrollView::touchMoved(int x, int y, int id) {
    int touchIndex = -1;
    for(int i=0; i<touchPoints.size(); i++) {
        ofxScrollViewTouchPoint & touchPoint = touchPoints[i];
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
        
        dragMoved(touchPoints[0].touchPos);
        
    } else if(touchPoints.size() == 2) {
        
        ofVec2f tp0(touchPoints[0].touchPos);
        ofVec2f tp1(touchPoints[1].touchPos);
        ofVec2f tmp = (tp1 - tp0) * 0.5 + tp0;
        float dist = (tp1 - tp0).length();
        
        zoomMoved(tmp, dist);
    }
}

void ofxScrollView::touchUp(int x, int y, int id) {
    int touchIndex = -1;
    for(int i=0; i<touchPoints.size(); i++) {
        ofxScrollViewTouchPoint & touchPoint = touchPoints[i];
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
        
        dragUp(touchPoints[0].touchPos);
        
    } else if(touchPoints.size() == 2) {
     
        ofVec2f tp0(touchPoints[0].touchPos);
        ofVec2f tp1(touchPoints[1].touchPos);
        ofVec2f tmp = (tp1 - tp0) * 0.5 + tp0;
        float dist = (tp1 - tp0).length();
        
        zoomUp(tmp, dist);
    }
    
    touchPoints.clear();
}

void ofxScrollView::touchDoubleTap(int x, int y, int id) {
    if(bDoubleTapZoomEnabled == false) {
        return;
    }
    
    bool bHit = windowRect.inside(x, y);
    if(bHit == false) {
        return;
    }
    
    ofVec2f touchPoint(x, y);
    
    float zoomCurrent = getZoom();
    float zoomTarget = 0.0;
    
    bool bZoomedInMax = (zoomCurrent == doubleTapZoomRangeMax);
    if(bZoomedInMax == true) {
        zoomTarget = doubleTapZoomRangeMin; // zoom all the way out.
    } else {
        zoomTarget = zoomCurrent + doubleTapZoomIncrement;
    }
    zoomTarget = ofClamp(zoomTarget, doubleTapZoomRangeMin, doubleTapZoomRangeMax);
    
    float zoomTimeSec = ABS(zoomTarget - zoomCurrent);
    zoomTimeSec *= doubleTapZoomIncrementTimeInSec;
    
    zoomTo(touchPoint, zoomTarget, zoomTimeSec);
}

void ofxScrollView::touchCancelled(int x, int y, int id) {
    //
}
