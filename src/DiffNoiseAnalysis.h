/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

#define DELTA_T_SAVE        1000/30  //timer call back needs to be at main app frame rate ie 1000/30
#define NUM_PHASE           1
#define NUM_RUN             1
#define NUM_SAVE_PER_RUN    100 //this analysis actually necessarily saves a random quantity of frames = about half this number


class DiffNoiseAnalysis : public AbstractAnalysis
{
public:
    DiffNoiseAnalysis(): AbstractAnalysis("DIFF_NOISE"){;}
    virtual ~DiffNoiseAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void synthesize();    
    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:
    
    bool    _RUN_DONE;
    int     _run_cnt, _save_cnt;
    float   c, _frame_cnt, _frame_cnt_max;
    
};
