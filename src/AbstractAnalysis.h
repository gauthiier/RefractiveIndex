/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "ofMain.h"
#include "ofEvents.h"

#include <string>

//#define ANALYSIS_PATH "data/analysis/"
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
    
    virtual void create_dir();
    
    // the runnable function in the thread 
    virtual void synthesize() = 0;   
    // this means that this function needs to be overwritten by children that inherit this class   
    
    
public:
    string  _name;    
    
    // event
    ofEvent<string> _synthesize_cb;    

protected:    
    int                 _cam_w, _cam_h;      
    int                 _state;    
    string              _whole_file_path;
    
    friend class AnalysisAdaptor;
};