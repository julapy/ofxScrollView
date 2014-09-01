#pragma once

#include "ofMain.h"
#include "ofxiOS.h"
#include "ofxiOSExtras.h"
#include "ofxScrollView.h"
#include "Grid.h"

class ofApp : public ofxiOSApp{
	
public:

    void setup();
    void update();
    void draw();
    void exit();
	
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);
    
    void lostFocus();
    void gotFocus();
    void gotMemoryWarning();
    void deviceOrientationChanged(int newOrientation);
    
    ofRectangle windowRect;
    ofRectangle contentRect;
    ofxScrollView scrollView;
    
    Grid grid;
    ofTexture image;
    
    ofVec2f touchPoint;
    float touchTime = 0.0;

};


