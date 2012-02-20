#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

class ColorMultiAnalysis : public AbstractAnalysis
{
public:
    ColorMultiAnalysis(): AbstractAnalysis("COLOR_MULTI"){;}
    virtual ~ColorMultiAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void acquire();
    void synthesise();    
    void display_results();
    void draw();
    
    void save_cb(Poco::Timer& timer);
    void display_results_cb(Poco::Timer& timer);
    
protected:
    
    bool    _RUN_DONE;
    int     _run_cnt, _save_cnt, _fade_cnt;
    float   c, _frame_cnt, _frame_cnt_max, _results_cnt, _results_cnt_max;
};
