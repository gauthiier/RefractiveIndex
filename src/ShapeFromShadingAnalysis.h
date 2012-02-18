/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"


class ShapeFromShadingAnalysis : public AbstractAnalysis
{
public:
    ShapeFromShadingAnalysis(): AbstractAnalysis("SHAPEFROMSHADING"){;}
    virtual ~ShapeFromShadingAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void acquire();
    void synthesise();    
    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:
    
    string quad;
    bool    _RUN_DONE;
    float     _flip, _level;
    int     _run_cnt, _save_cnt;
    int _animation_cnt1;
    int _animation_cnt2;
    int _animation_cnt3;
    int _animation_cnt4;
    int _animation_cnt5;
    int _animation_cnt6;
    int _animation_cnt7;
    int _animation_cnt8;
    int _animation_cnt9;
    int _animation_cnt10;
    int _animation_cnt11;
    int _animation_cnt12;
    int _animation_cnt13;
    int _animation_cnt14;
    int _animation_cnt15;
    int _animation_cnt16;
    int _animation_reset;  // this reset part didn't get working - so using 16 different counters! yay! 
    float   c, _frame_cnt, _frame_cnt_max;
    
};
