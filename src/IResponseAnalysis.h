/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

#define DELTA_T_SAVE        100  //the milisecond timing 
#define NUM_PHASE           1
#define NUM_RUN             1
#define NUM_SAVE_PER_RUN    100


class IResponseAnalysis : public AbstractAnalysis
{
public:
    IResponseAnalysis(): AbstractAnalysis("I_RESPONSE"){;}
    virtual ~IResponseAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void synthesize();
    void gui_attach(ofxControlPanel* gui);
    void gui_detach();
    
    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:
    
    bool    _RUN_DONE;
    int     _run_cnt, _save_cnt;
    float   c, _frame_cnt, _frame_cnt_max;
    string _whole_file_path;
    
};
