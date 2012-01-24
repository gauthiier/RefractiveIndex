/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "ofxControlPanel.h"
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
    
    // the runnable function in the thread 
    virtual void synthesize() = 0;   // this means that this function needs to be overwritten by children that inherit this class       
    
    // throwing the 
    virtual void gui_attach(ofxControlPanel* gui){_gui = gui;}
    virtual void gui_detach(){;}
    
    
    //    virtual void draw(ofPixels _pixels) = 0;
    //    virtual void draw(ofPixels)=0;
    // how to get the pixels into the analysis classes?!?  -j
    
    // ofx
    virtual void draw() = 0;    
    
    // this is what's called a Pure Virtual Function - not sure if you can pass ofPixels through this? 
    
    /*
     When a virtual function is called, the implementation is chosen based not on the static type of the pointer 
     or reference, but on the type of the object being pointed to, which can vary at run time
     So this Pure Virtual Function will be called based on the kind of object or class that instantiated it(?) 
     */
    
    
public:
    string  _name;    

protected:    
    ofxControlPanel*    _gui;
    int                 _cam_w, _cam_h;  
    
    int             _state;
    
    friend class AnalysisAdaptor;
};