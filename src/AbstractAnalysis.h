/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include "ofxOpenCv.h"
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
    virtual void setup(int camWidth, int camHeight);  

    // this is the main threaded loop for a given analysis
    void do_synthesize();
                        
    // show the results to the screen
    virtual void draw() = 0;    
        
protected:
    
    virtual void create_dir_allocate_images();
    
    virtual void read_dir_create_list(string folder_path);
    
    virtual void saveImageAnalysis(string filename);
    virtual void saveImageSynthesis(string filename, ofxCvImage* newPixels, ofImageType newType);
    
    // acquire images - all the children (see - do_synthesize)
    virtual void acquire() = 0;   
    
    // analysis + synthesize images - all the children (see - do_synthesize)
    virtual void synthesise() = 0;   
    
    // display the results from disk
    virtual void displayresults() = 0;   
    
    // display the results from disk
    virtual void cleanup() {;}   
    
        
public:
    string  _name;       
    string  _draw_directory;
    
    // event
    ofEvent<string> _synthesize_cb;    
    bool    meshIsComplete;
    bool    imageForContourAvailable;
    ofMesh  aMesh;
    ofLight light;
    ofLight lightStatic; 
    
    string  meshFileName;
    //difference between our image size and the size of the fbo
    float   widthScaleFactor;
    float   heightScaleFactor;
    ofImage  contourImage;
    ofPixels meshPix;
    ofPixels publicColorImage;
    float    zPlaneAverage;
    
    //added Tom 1/5/12 defines the stretch we make to the mesh to make it fit HD proportions
    float           _mesh_size_multiplier;
    
protected:    
    int             _cam_w, _cam_h;          
    string          _whole_file_path_analysis, _whole_file_path_synthesis;
    vector<string>  _saved_filenames_analysis;
    vector<string>  _saved_filenames_synthesis;
    
    int             _state;
    
    ofImage         myColorImage1;
    ofImage         myColorImage2;
    ofImage         myGrayImage1;
    
    //int             _run_cnt;
    
    float           DELTA_T_SAVE;
    int             NUM_PHASE;
    int             NUM_RUN;
    int             NUM_SAVE_PER_RUN;    
    
    friend class AnalysisAdaptor;
};