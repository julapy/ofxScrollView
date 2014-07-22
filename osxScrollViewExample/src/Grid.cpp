//
//  Grid.cpp
//  Created by Lukasz Karluk on 15/07/2014.
//  http://julapy.com/
//

#include "Grid.h"

Grid::Grid() {
    size.x = 4096;
    size.y = 4096;
}

Grid::~Grid() {
    //
}

void Grid::setup(float w, float h) {
    size.x = w;
    size.y = h;
}

float Grid::getWidth() {
    return size.x;
}

float Grid::getHeight() {
    return size.y;
}

void Grid::draw() {
    ofSetColor(255);
    ofRect(0, 0, size.x, size.y);

    int countX = 20;
    int countY = 20;
    float sizeX = size.x / countX;
    float sizeY = size.y / countY;
    
    ofSetColor(0);
    
    for(int yi=0; yi<countY; yi++) {
        for(int xi=0; xi<countX; xi++) {
            int i = (yi * countX) + xi;
            int x = xi * sizeX;
            int y = yi * sizeY;
            
            ofDrawBitmapString(ofToString(i), x + 10, y + 20);
        }
    }
    
    ofNoFill();
    ofSetColor(ofColor::red);
    ofRect(0, 0, size.x, size.y);
    
    for(int yi=1; yi<countY; yi++) {
        int x0 = 0;
        int x1 = size.x;
        int y0 = yi * sizeY;
        int y1 = y0;
        ofLine(x0, y0, x1, y1);
    }
    
    for(int xi=1; xi<countX; xi++) {
        int x0 = xi * sizeX;
        int x1 = x0;
        int y0 = 0;
        int y1 = size.y;
        ofLine(x0, y0, x1, y1);
    }
    
    ofFill();
    ofSetColor(255);
}
