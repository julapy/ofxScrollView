//
//  ofxScrollView.h
//  Created by Lukasz Karluk on 2/06/2014.
//  http://julapy.com/
//

#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
class ofxScrollViewTouchPoint {
public:
    ofxScrollViewTouchPoint() {
        //
    }
    int touchID = 0;
    ofVec2f touchPos;
    float touchDownTimeInSec = 0;
};

//--------------------------------------------------------------
class ofxScrollView {
    
public:
    
    ofxScrollView();
    ~ofxScrollView();
    
    void setUserInteraction(bool bEnable);
    void setPinchZoom(bool bEnable);
    void setScrollEasing(float value);
    void setBounceBack(float value);
    void setDragVelocityDecay(float value);
    void setDoubleTapZoom(bool bEnable);
    void setDoubleTapZoomIncrement(float value);
    void setDoubleTapZoomIncrementTimeInSec(float value);
    void setDoubleTapRegistrationTimeInSec(float value);
    void setDoubleTapRegistrationDistanceInPixels(float value);

    virtual void setup();
    virtual void reset();
    
    void setWindowRect(const ofRectangle & rect);
    void setContentRect(const ofRectangle & rect);
    
    void fitContentToWindow(ofAspectRatioMode aspectRatioMode);

    void setScale(float value);
    void setScaleMin(float value);
    void setScaleMax(float value);
    
    float getScale();
    float getScaleMin();
    float getScaleMax();
    
    void setZoom(float value);
    float getZoom();
    bool isZoomed();
    bool isZoomedInMax();
    bool isZoomedOutMax();
    
    float zoomToScale(float value);
    float scaleToZoom(float value);

    void zoomToMin(const ofVec2f & screenPoint, float timeSec=0.0);
    void zoomToMax(const ofVec2f & screenPoint, float timeSec=0.0);
    void zoomTo(const ofVec2f & screenPoint, float zoom, float timeSec=0.0);
    void zoomToContentPointAndPositionAtScreenPoint(const ofVec2f & contentPoint,
                                                    const ofVec2f & screenPoint,
                                                    float zoom,
                                                    float timeSec=0.0);
    void moveContentPointToScreenPoint(const ofVec2f & contentPoint,
                                       const ofVec2f & screenPoint,
                                       float timeSec=0.0);
    bool animStart(float animTimeInSec);
    
    void setScrollPositionX(float x, bool bEase=true);
    void setScrollPositionY(float y, bool bEase=true);
    void setScrollPosition(float x, float y, bool bEase=true);
    
    ofVec2f getScrollPosition();
    ofVec2f getScrollPositionNorm();
    
    const ofRectangle & getWindowRect();
    const ofRectangle & getContentRect();
    const ofRectangle & getScrollRect();
    const ofMatrix4x4 & getMatrix();
    
    virtual void update();

    //-------------------------------------------------------------- the brains!
    ofRectangle getRectContainedInWindowRect(const ofRectangle & rect,
                                             float easing=1.0);
    
    ofRectangle getRectZoomedAtScreenPoint(const ofRectangle & rect,
                                           const ofVec2f & screenPoint,
                                           float zoom=0.0);
    
    ofRectangle getRectWithContentPointAtScreenPoint(const ofRectangle & rect,
                                                     const ofVec2f & contentPoint,
                                                     const ofVec2f & screenPoint);
    
    ofRectangle getRectLerp(const ofRectangle & rectFrom,
                            const ofRectangle & rectTo,
                            float progress);
    
    ofMatrix4x4 getMatrixForRect(const ofRectangle & rect);
    
    ofVec2f getContentPointAtScreenPoint(const ofRectangle & rect,
                                         const ofVec2f & screenPoint);
    
    ofVec2f getScreenPointAtContentPoint(const ofRectangle & rect,
                                         const ofVec2f & contentPoint);

    //--------------------------------------------------------------
    virtual void begin();
    virtual void end();
    virtual void draw();
    
    virtual void exit();
    
    virtual void dragDown(const ofVec2f & point);
    virtual void dragMoved(const ofVec2f & point);
    virtual void dragUp(const ofVec2f & point);
    virtual void dragCancel();
    
    virtual void zoomDown(const ofVec2f & point, float pointDist);
    virtual void zoomMoved(const ofVec2f & point, float pointDist);
    virtual void zoomUp(const ofVec2f & point, float pointDist);
    virtual void zoomCancel();
    
    ofRectangle windowRect;
    ofRectangle contentRect;
    
    bool bUserInteractionEnabled = false;
    bool bPinchZoomEnabled = false;

    ofRectangle scrollRect;
    ofRectangle scrollRectEased;
    ofRectangle scrollRectAnim0;
    ofRectangle scrollRectAnim1;
    float scrollEasing = 0.5;
    float bounceBack = 1.0;
    
    ofVec2f dragDownPos;
    ofVec2f dragMovePos;
    ofVec2f dragMovePosPrev;
    ofVec2f dragVel;
    float dragVelDecay = 0.9;
    bool bDragging = false;
    
    ofVec2f zoomDownPos;
    ofVec2f zoomMovePos;
    ofVec2f zoomMovePosPrev;
    float zoomDownDist = 0;
    float zoomMoveDist = 0;
    bool bZooming = false;
    
    float animTimeStart = 0.0;
    float animTimeTotal = 0.0;
    bool bAnimating = false;

    bool bDoubleTapZoomEnabled = false;
    float doubleTapZoomIncrement = 1.0;
    float doubleTapZoomIncrementTimeInSec = 0.2;
    float doubleTapRegistrationTimeInSec = 0.25;
    float doubleTapRegistrationDistanceInPixels = 22;
    
    float scale = 1.0;
    float scaleDown = 1.0;
    float scaleMin = 1.0;
    float scaleMax = 1.0;
    ofMatrix4x4 mat;
    
    vector<ofxScrollViewTouchPoint> touchPoints;
    ofxScrollViewTouchPoint touchDownPointLast;
    
    //----------------------------------------------------------
    virtual void mouseMoved(ofMouseEventArgs & mouse){
        mouseMoved(mouse.x,mouse.y);
    }
    virtual void mouseDragged(ofMouseEventArgs & mouse){
        mouseDragged(mouse.x,mouse.y,mouse.button);
    }
    virtual void mousePressed(ofMouseEventArgs & mouse){
        mousePressed(mouse.x,mouse.y,mouse.button);
    }
    virtual void mouseReleased(ofMouseEventArgs & mouse){
        mouseReleased(mouse.x,mouse.y,mouse.button);
    }
    
    virtual void mouseMoved(int x, int y);
    virtual void mouseDragged(int x, int y, int button);
    virtual void mousePressed(int x, int y, int button);
    virtual void mouseReleased(int x, int y, int button);
    
    //----------------------------------------------------------
	virtual void touchDown(ofTouchEventArgs &touch) {
        touchDown(touch.x, touch.y, touch.id);
    }
	virtual void touchMoved(ofTouchEventArgs &touch) {
        touchMoved(touch.x, touch.y, touch.id);
    }
	virtual void touchUp(ofTouchEventArgs &touch) {
        touchUp(touch.x, touch.y, touch.id);
    }
    virtual void touchDoubleTap(ofTouchEventArgs &touch) {
        touchDoubleTap(touch.x, touch.y, touch.id);
    }
    virtual void touchCancelled(ofTouchEventArgs &touch) {
        touchCancelled(touch.x, touch.y, touch.id);
    }
    
    virtual void touchDown(int x, int y, int id);
    virtual void touchMoved(int x, int y, int id);
    virtual void touchUp(int x, int y, int id);
    virtual void touchDoubleTap(int x, int y, int id);
    virtual void touchCancelled(int x, int y, int id);
};
