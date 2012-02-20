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

#define COMPARE_RED 1
#define COMPARE_BLUE 2
#define COMPARE_GREEN 3
#define  COMPARE_HUE 4
#define COMPARE_BRIGHTNESS 5

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
    virtual void saveimage(string filename);
    
    // acquire images - all the children (see - do_synthesize)
    virtual void acquire() = 0;   
    
    // analysis + synthesize images - all the children (see - do_synthesize)
    virtual void synthesise() = 0;   
   
    //added tom s 19/2 function runs a call back exactly like acquire.
    virtual void display_results() = 0;   
    
    //returns ofPixels which contain the color differences between the two images. Is overloaded to include comparison with values written in to file names for some analyses
    virtual ofPixels calculateListOfZValues(ofImage image1, ofImage image2, int whichComparison);
    virtual ofPixels calculateListOfZValues(ofImage image1, ofImage image2, int whichComparison, int colourValue);
    
    //uses the returned pixels from make3DZmap to make a mesh of points whose Z positions are set by the brightness values in ofPixels - ofPixels is being used as a convenient container for a bunch of z coordinates
    virtual void setMeshFromPixels(ofPixels somePixels, ofImage currentSecondImage, ofMesh * someMesh);
    //HELPER FUNCTIONS
    
    //this is purely for debug/viewing purposes and loads old images from middlesborough test
    virtual vector<string> getListOfImageFilePaths(string location, string whichAnalysis);
    
    //splits up the filename and returns the recorded value eg brightness
    //EG FILENAME : DIFF_NOISE_7_85.7322.jpg RETURNS : 85.7322
    virtual int getRecordedValueFromFileName(string str);
    
    static vector<ofMesh>meshes;

    
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
    //make this vector were static
    //how fast to move from one mesh to the next
    float speed;
    //the index (inside the vector of meshes) of the current mesh being displayed
    float whichMesh;
    friend class AnalysisAdaptor;
};