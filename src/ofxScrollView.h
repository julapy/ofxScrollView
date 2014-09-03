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
    ofxScrollViewTouchPoint(int x, int y, int id) {
        touchID = id;
        touchPos.x = x;
        touchPos.y = y;
    }
    int touchID;
    ofVec2f touchPos;
};

//--------------------------------------------------------------
class ofxScrollView {
    
public:
    
    ofxScrollView();
    ~ofxScrollView();

    virtual void setup();
    virtual void reset();
    
    void setWindowRect(const ofRectangle & rect);
    void setContentRect(const ofRectangle & rect);
    
    void fitContentToWindow(ofAspectRatioMode aspectRatioMode);
    
    void setZoom(float value);
    void setZoomMin(float value);
    void setZoomMax(float value);
    
    float getZoom();
    float getZoomMin();
    float getZoomMax();
    
    bool isZoomed();
    bool isZoomedMin();
    bool isZoomedMax();

    void zoomTo(const ofVec2f & pos, float zoom, float timeSec=0.0);
    void zoomToMin(const ofVec2f & pos, float timeSec=0.0);
    void zoomToMax(const ofVec2f & pos, float timeSec=0.0);
    
    void positionContentPointAtWindowPoint(const ofVec2f & contentPoint,
                                           const ofVec2f & windowPoint);
    
    void setScrollPositionX(float x, bool bEase=true);
    void setScrollPositionY(float y, bool bEase=true);
    void setScrollPosition(float x, float y, bool bEase=true);
    
    ofVec2f getScrollPosition();
    ofVec2f getScrollPositionNorm();
    
    const ofRectangle & getWindowRect();
    const ofRectangle & getContentRect();
    const ofRectangle & getScrollRect();
    const ofMatrix4x4 & getMatrix();
    
    void setUserInteraction(bool bEnable);
    void setScrollEasing(float value);
    void setBounceBack(float value);
    void setDragVelocityDecay(float value);
    
    virtual void update();

    ofRectangle getRectContainedInWindowRect(const ofRectangle & rect,
                                             float easing=1.0);
    
    ofRectangle getRectZoomedAtScreenPoint(const ofVec2f & screenPoint,
                                           float zoom=0.0);
    
    ofRectangle getRectWithContentPointAtWindowPoint(const ofVec2f & contentPoint,
                                                     const ofVec2f & windowPoint);
    
    ofRectangle getRectLerp(const ofRectangle & rectFrom,
                            const ofRectangle & rectTo,
                            float progress);

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
    
    bool bUserInteractionEnabled;
    bool bPinchZoom;
    
    ofRectangle windowRect;
    ofRectangle contentRect;

    ofRectangle scrollRect;
    ofRectangle scrollRectEased;
    ofRectangle scrollRectAnim0;
    ofRectangle scrollRectAnim1;
    float scrollEasing;
    float bounceBack;
    
    ofVec2f dragDownPos;
    ofVec2f dragMovePos;
    ofVec2f dragMovePosPrev;
    ofVec2f dragVel;
    float dragVelDecay;
    bool bDragging;
    
    ofVec2f zoomDownPos;
    ofVec2f zoomMovePos;
    ofVec2f zoomMovePosPrev;
    float zoomDownDist;
    float zoomMoveDist;
    bool bZooming;
    
    float animTimeStart;
    float animTimeTotal;
    bool bAnimating;
    bool bDoubleTap;
    
    float scale;
    float scaleDown;
    float scaleMin;
    float scaleMax;
    ofMatrix4x4 mat;
    
    vector<ofxScrollViewTouchPoint *> touchPoints;
    
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