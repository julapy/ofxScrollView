//
//  Grid.h
//  Created by Lukasz Karluk on 15/07/2014.
//  http://julapy.com/
//

#pragma once

#include "ofMain.h"

class Grid {
    
public:
    
    Grid();
    ~Grid();

    void setup(float width, float height);
    float getWidth();
    float getHeight();
    void draw();
    
    ofVec2f size;
};
