#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    //----------------------------------------------------------
    grid.setup(2048, 2048);

    screenRect.width = ofGetWidth();
    screenRect.height = ofGetHeight();
    
    windowRect.width = (int)(screenRect.width * 0.9);
    windowRect.height = (int)(screenRect.height * 0.9);
    windowRect.x = (int)((screenRect.width - windowRect.width) * 0.5);
    windowRect.y = (int)((screenRect.height - windowRect.height) * 0.5);
    
    contentRect.width = grid.getWidth();
    contentRect.height = grid.getHeight();
    
    //----------------------------------------------------------
    scrollView.setWindowRect(windowRect); // window size and position of scroll view.
    scrollView.setContentRect(contentRect); // the pixel size of the content being displayed in scroll view.
    scrollView.fitContentToWindow(OF_ASPECT_RATIO_KEEP); // fits content into window, works with ofAspectRatioMode values.
    
    scrollView.setScrollEasing(0.3); // smoothness of scrolling, between 0 and 1.
    scrollView.setBounceBack(0.3); // the speed of bounce back, between 0 and 1.
    scrollView.setDragVelocityDecay(0.9); // the speed of decay of drag velocity after release, between 0 and 1.
    scrollView.setUserInteraction(true); // enable / disable mouse or touch interaction.

    scrollView.setDoubleTapZoom(true); // enable double tap zoom.
    scrollView.setDoubleTapZoomIncrement(1.0); // the increment value of zoom on double tap, between 0 and 1.
    scrollView.setDoubleTapZoomIncrementTimeInSec(0.3); // the time amount of time for zoom increment.
    scrollView.setDoubleTapRegistrationTimeInSec(0.25); // the time threshold between taps for double tap event to register.
    scrollView.setDoubleTapRegistrationDistanceInPixels(20); // the distance threshold between taps for double tap event to register.
    
    scrollView.setup(); // setup must always be called at the end of scroll view config.
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
    
    ofSetColor(0);
    ofBeginShape();
    ofVertex(screenRect.x, screenRect.y);
    ofVertex(screenRect.x + screenRect.width, screenRect.y);
    ofVertex(screenRect.x + screenRect.width, screenRect.y + screenRect.height);
    ofVertex(screenRect.x, screenRect.y + screenRect.height);
    ofNextContour();
    ofVertex(windowRect.x, windowRect.y);
    ofVertex(windowRect.x + windowRect.width, windowRect.y);
    ofVertex(windowRect.x + windowRect.width, windowRect.y + windowRect.height);
    ofVertex(windowRect.x, windowRect.y + windowRect.height);
    ofEndShape(true);
    ofSetColor(255);
    ofNoFill();
    ofRect(windowRect);
    ofFill();
    
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