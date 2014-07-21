#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	

    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetOrientation(OF_ORIENTATION_90_LEFT);
    
    //----------------------------------------------------------
    windowRect.width = ofGetWidth();
    windowRect.height = ofGetHeight();
    
    contentRect.width = grid.getWidth();
    contentRect.height = grid.getHeight();
    
    scrollView.setWindowRect(windowRect);
    scrollView.setContentRect(contentRect);
    scrollView.setZoomMultiplier(3.0);
    scrollView.setZoomContentToFitContentRect();
    scrollView.setScrollEasing(0.3); // smoothness of scrolling, between 0 and 1.
    scrollView.setBounceBack(0.3); // the speed of bounce back, between 0 and 1.
    scrollView.setUserInteraction(true); // enable / disable mouse or touch interaction.
    scrollView.setup();
}

//--------------------------------------------------------------
void ofApp::update(){

    scrollView.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	
    // ofxScrollView returns a matrix to do any transformations manually,
    // otherwise drawing things between begin() and end() methods will also do the trick.
    
    ofMatrix4x4 mat;
    mat = scrollView.getMatrix();
    
    scrollView.begin();
    
    grid.draw();
    
    scrollView.end();
    
    scrollView.draw();
    
    ofSetColor(0);
    ofDrawBitmapString(ofToString((int)ofGetFrameRate()) + " fps", 20, 20);
    ofSetColor(255);
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    
    touchPoint.x = touch.x;
    touchPoint.y = touch.y;
    
    float zoomCurrent = scrollView.getZoom();
    float zoomMax = scrollView.getZoomMax();
    float zoomMin = scrollView.getZoomMin();
    float zoomRange = zoomMax - zoomMin;
    
    float zoomTarget = 1.0;
    bool bZoomed = scrollView.isZoomedMax();
    if(bZoomed == true) {
        zoomTarget = zoomMin;
    } else {
        zoomTarget = zoomMax;
    }
    
    float zoomTimeSec = ABS(zoomTarget - zoomCurrent) / zoomRange;
    zoomTimeSec *= 0.2;
    
    scrollView.zoomTo(touchPoint, zoomTarget, zoomTimeSec);
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}

