/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include <string>

#define ANALYSIS_PATH "analysis/"

#define STATE_STOP  0xDEADBEEF

class AbstractAnalysis {
    
public:
    AbstractAnalysis(string name) : _name(name) {;}
    virtual ~AbstractAnalysis(){;}
    
    // generic function to set up the camera
    virtual void setup(int camWidth, int camHeight){_cam_w = camWidth; _cam_h = camHeight;}   
    void do_synthesize();
                        
    // ofx
    virtual void draw() = 0;    
        
protected:
    
    // creates working directory 
    virtual void create_dir();    
 
    // acquire images from camera and display patters on screen
    virtual void pattern_acquire() = 0;
    
    // analyses and sythesizes images acquired
    virtual void synthesize() = 0;       
            
public:
    string  _name;    
    
    // event notification / callbacks
    ofEvent<string> _acquire_cb;
    ofEvent<string> _synthesize_cb;    

protected:    
    int                 _cam_w, _cam_h;      
    int                 _state;    
    string              _whole_file_path;
    
    float   DELTA_T_SAVE;
    int     NUM_PHASE;
    int     NUM_RUN;
    int     NUM_SAVE_PER_RUN;    
    
    friend class AnalysisAdaptor;
};