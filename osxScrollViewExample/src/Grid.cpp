//
//  Grid.cpp
//  Created by Lukasz Karluk on 15/07/2014.
//  http://julapy.com/
//

#include "Grid.h"

Grid::Grid() {
    width = 4096;
    height = 4096;
}

Grid::~Grid() {
    //
}

float Grid::getWidth() {
    return width;
}

float Grid::getHeight() {
    return height;
}

void Grid::draw() {
    ofSetColor(255);
    ofRect(0, 0, width, height);

    int numOfRows = 20;
    int numOfCols = 20;
    float rowSize = height / numOfRows;
    float colSize = width / numOfCols;
    
    ofSetColor(0);
    
    for(int yi=0; yi<numOfRows; yi++) {
        for(int xi=0; xi<numOfCols; xi++) {
            int i = (yi * numOfCols) + xi;
            int x = xi * colSize;
            int y = yi * rowSize;
            
            ofDrawBitmapString(ofToString(i), x + 60, y + 60);
        }
    }
    
    ofNoFill();
    ofSetColor(ofColor::red);
    ofRect(0, 0, width, height);
    
    for(int yi=1; yi<numOfRows; yi++) {
        int x0 = 0;
        int x1 = width;
        int y0 = yi * rowSize;
        int y1 = y0;
        ofLine(x0, y0, x1, y1);
    }
    
    for(int xi=1; xi<numOfCols; xi++) {
        int x0 = xi * colSize;
        int x1 = x0;
        int y0 = 0;
        int y1 = height;
        ofLine(x0, y0, x1, y1);
    }
    
    ofFill();
    ofSetColor(255);
}
