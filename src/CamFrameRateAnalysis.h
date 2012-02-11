/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

#define DELTA_T_SAVE        1000/30  //timer call back needs to be at main app frame rate ie 1000/30
#define NUM_PHASE           1
//#define TIME_PER_RUN        3*1000
#define NUM_RUN             1
//#define SYNTH_TIME          TIME_PER_RUN *NUM_RUN //the number of millis it takes for the whole sequence
#define NUM_SAVE_PER_RUN    100    //this must be equal to the number of frames it takes for the whole analysis to draw - ie  _frame_cnt_max


class CamFrameRateAnalysis : public AbstractAnalysis
{
public:
    CamFrameRateAnalysis(): AbstractAnalysis("CAM_NOISE"){;}
    virtual ~CamFrameRateAnalysis(){;}
    
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
