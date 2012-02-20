/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"


class StrobeAnalysis : public AbstractAnalysis
{
public:
    StrobeAnalysis(): AbstractAnalysis("STROBE"){;}
    virtual ~StrobeAnalysis(){;}
    
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
    int     _strobe_cnt, _run_cnt, _strobe_cnt_max;
    int     _save_cnt;
    
    int     _frame_cnt, _frame_cnt_max, _save_cnt_max ;
    float   _results_cnt, _results_cnt_max;
    
    int     _strobe_interval;
    bool    _strobe_on;
};
