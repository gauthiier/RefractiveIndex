/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

class ColorSingleAnalysis : public AbstractAnalysis
{
public:
    ColorSingleAnalysis(): AbstractAnalysis("COLOR_SINGLE"){;}
    virtual ~ColorSingleAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void acquire();
    void synthesise();    
    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:

    bool    _RUN_DONE;
    string  fileNameTag;
    int     _run_cnt, _save_cnt, _fade_cnt;
    float   r,g,b, _frame_cnt, _frame_cnt_max;
};
