/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "ofMain.h"
#include "ofEvents.h"

#include "AbstractAnalysis.h"
#include "AnalysisAdaptor.h"


typedef map<string, AnalysisAdaptor*>   TAnalysisMap;
typedef vector<AnalysisAdaptor*>        TAnalysisVec;

class RefractiveIndex : public ofBaseApp
{
public:
    
    // ofx
    void setup();
    void update();
    void draw();
    void exit();
    
    // camera
    void setup_camera();
    void stop_camera();
    
    // starts the whole process
    void start_analysis();
    
    // stops the whole process
    void stop_analysis();
        
    // callbacks
    void acquire_cb(string & analysis);
    void synthesize_cb(string & analysis);
    
    // state machine
    void state_machine_analysis();
    
    // ofx
    void keyPressed  (int key);
    void keyReleased(int key){;}
    void mouseMoved(int x, int y ){;}
    void mouseDragged(int x, int y, int button){;}
    void mousePressed(int x, int y, int button){;}
    void mouseReleased(int x, int y, int button){;}
    void windowResized(int w, int h){;}  
    
protected:
        
    AbstractAnalysis*           _currentAnalysis;
    AnalysisAdaptor*            _currentAnalysisAdaptor;
    AnalysisAdaptor*            _currentSynthesisAdaptor;
    TAnalysisMap::iterator      _currentAnalysisIndx;
    
    TAnalysisMap                _acquisitionMap;
    TAnalysisMap                _synthesisMap;
    
    TAnalysisVec                _display_results_vector;
        
    
public:    
    // acquisition
    static ofPixels         _pixels;    
    static ofVideoGrabber   _vidGrabber;
    vector<string>          videoSourceList;    
    static int              _vid_w, _vid_h, _vid_id;
    static bool             _vid_stream_open;
    static bool             _vid_toggle_on;
    
    // this should be in xml
    static string           _location;            
    
};