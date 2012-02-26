/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include <string>

#define ANALYSIS_PATH "analysis/"
#define SYNTHESIS_PATH "synthesis/"

#define STATE_ACQUIRING         0x1111
#define STATE_SYNTHESISING      0x2222
#define STATE_DISPLAY_RESULTS   0x3333
#define STATE_STOP              0xDEADBEEF

class AbstractAnalysis {
    
public:
    AbstractAnalysis(string name) : _name(name) {;}
    virtual ~AbstractAnalysis(){;}
    
    // generic function to set up the camera
    virtual void setup(int camWidth, int camHeight){_cam_w = camWidth; _cam_h = camHeight;}  

    // this is the main threaded loop for a given analysis
    void do_synthesize();
                        
    // show the results to the screen
    virtual void draw() = 0;    
        
protected:
    
    virtual void create_dir();
    
    virtual void saveimage(string filename);
    
    // acquire images - all the children (see - do_synthesize)
    virtual void acquire() = 0;   
    
    // analysis + synthesize images - all the children (see - do_synthesize)
    virtual void synthesise() = 0;   
    
    // display the results from disk
    virtual void displayresults() = 0;   
        
public:
    string  _name;    
    
    // event
    ofEvent<string> _synthesize_cb;    

protected:    
    int             _cam_w, _cam_h;          
    string          _whole_file_path_analysis, _whole_file_path_synthesis;
    vector<string>  _saved_filenames_analysis;
    vector<string>  _saved_filenames_synthesis;
    
    int             _state;
    
    //int             _run_cnt;
    
    float           DELTA_T_SAVE;
    int             NUM_PHASE;
    int             NUM_RUN;
    int             NUM_SAVE_PER_RUN;    
    
    friend class AnalysisAdaptor;
};