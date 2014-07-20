#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    windowRect.width = ofGetWidth();
    windowRect.height = ofGetHeight();
    
    contentRect.width = grid.getWidth();
    contentRect.height = grid.getHeight();
    
    scrollView.setWindowRect(windowRect);
    scrollView.setContentRect(contentRect);
    scrollView.setZoomMultiplier(3.0);
    scrollView.setZoomContentToFitContentRect();
    scrollView.setZoomEasing(0.3); // smoothness of zooming, between 0 and 1.
    scrollView.setScrollEasing(0.3); // smoothness of scrolling, between 0 and 1.
    scrollView.setBounceBack(0.3); // the speed of bounce back, between 0 and 1.
    scrollView.setUserInteraction(true); // enable / disable mouse or touch interaction.
    scrollView.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(bDoubleTap == true) {
        bDoubleTap = false;
        
        
    }
    
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
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    ofVec2f touchPointNew(x, y);
    ofVec2f touchPointDiff = touchPointNew - touchPoint;
    touchPoint = touchPointNew;
    
    float touchTimeNow = ofGetElapsedTimef();
    float touchTimeDiff = touchTimeNow - touchTime;
    
    bDoubleTap = true;
    bDoubleTap = bDoubleTap && (touchTimeDiff < 0.2);
    bDoubleTap = bDoubleTap && (touchPointDiff.length() < 10);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}