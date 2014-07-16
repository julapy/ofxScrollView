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

    float getWidth();
    float getHeight();
    void draw();
    
    float width;
    float height;
};
