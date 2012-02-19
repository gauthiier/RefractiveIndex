/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include <string>

#define ANALYSIS_PATH "analysis/"

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
                        
    // ofx
    virtual void draw() = 0;    
        
protected:
    
    virtual void create_dir();
    
    // acquire images - all the children (see - do_synthesize)
    virtual void acquire() = 0;   
    
    // analysis + synthesize images - all the children (see - do_synthesize)
    virtual void synthesise() = 0;   
    
    virtual ofPixels calculateListOfZValues(ofImage image1, ofImage image2, int whichComparison);
    
    virtual void setMeshFromPixels(ofPixels somePixels, ofImage currentSecondImage, ofMesh * someMesh);
        
public:
    string  _name;    
    
    // event
    ofEvent<string> _synthesize_cb;    

protected:    
    int             _cam_w, _cam_h;          
    string          _whole_file_path;
    vector<string>  _saved_filenames;
    
    int             _state;
    
    float           DELTA_T_SAVE;
    int             NUM_PHASE;
    int             NUM_RUN;
    int             NUM_SAVE_PER_RUN;    
    
    //added Tom S 19/2/12
    //each mesh in the vector is a seperate 3D point cloud which is coloured with pixel data and shifted in the z plane according to the specified type of colour difference eg red value or hue
    vector<ofMesh>meshes;
    //how fast to move from one mesh to the next
    float speed;
    //the index (inside the vector of meshes) of the current mesh being displayed
    float whichMesh;
    friend class AnalysisAdaptor;
};