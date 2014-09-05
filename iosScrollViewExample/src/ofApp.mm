#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	

    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetOrientation(OF_ORIENTATION_90_LEFT);
    
    //----------------------------------------------------------
    ofLoadImage(image, "sample_image.jpg");
    
    //----------------------------------------------------------
    windowRect.width = ofGetWidth();
    windowRect.height = ofGetHeight();
    
    contentRect.width = grid.getWidth();
    contentRect.height = grid.getHeight();
    
    //----------------------------------------------------------
    scrollView.setWindowRect(windowRect); // window size and position of scroll view.
    scrollView.setContentRect(contentRect); // the pixel size of the content being displayed in scroll view.
    scrollView.fitContentToWindow(OF_ASPECT_RATIO_KEEP_BY_EXPANDING); // fits content into window, works with ofAspectRatioMode values.
    
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
    
    image.draw(0, 0, grid.getWidth(), grid.getHeight());
    
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

