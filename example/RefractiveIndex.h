
#pragma once

#include "ofMain.h"
#include "ofEvents.h"
//#include "ofxControlPanel.h"

#include "AbstractAnalysis.h"
#include "AnalysisAdaptor.h"

#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

#include "ofxArcBall.h"

#define MODE_DRAWING        0xEEFF
#define MODE_ANALYSING      0xFFEE

#define VERTS   1
#define WIRE    2
#define FACE    3

class RefractiveIndex : public ofBaseApp
{
public:
    
    // ofx
    void setup();
    void update();
    void draw();
    void exit();
    
    // refindx
    void setup_camera();
    void stop_camera();
    void analysis_cb(string & analysis);
    void start_analysis();
    void stop_analysis();
    void state_analysis();
    
    void setup_shader_vbo();
    
    // ofx
    void keyPressed  (int key);
    void keyReleased(int key){;}
    void mouseMoved(int x, int y ){;}
    void mouseDragged(int x, int y, int button){;}
    void mousePressed(int x, int y, int button){;}
    void mouseReleased(int x, int y, int button){;}
    void windowResized(int w, int h){;}  
    
protected:
    
    //void eventsIn(guiCallbackData & data);
    //void grabBackgroundEvent(guiCallbackData & data);
    
    // gui
    //ofxControlPanel     _gui;   
    
    AbstractAnalysis*           _currentAnalysis;
    int                         _currentAnalysisIndx;
    AnalysisAdaptor*            _analysisAdapator;
    vector<AbstractAnalysis*>   _analysisVector;    
    
public:    
    // acquisition
    static ofPixels         _pixels;    
    static ofVideoGrabber   _vidGrabber;
    static int              _mode;
    
    vector<string>          videoSourceList;    
    static int              _vid_w, _vid_h, _vid_id;
    static bool             _vid_stream_open;
    static bool             _vid_toggle_on;
    
    // this should be in xml
    static string           _location;
    static ofxXmlSettings   XML;  // made this static so we can access RUN_NUM in the analyses 
    
    
    static ofVboMesh       _mesh_vbo;
    
    vector<ofVec3f> _verts;
    vector<ofVec2f> _tex;
    vector<unsigned int> _ind;
    
    static ofxArcBall cam;
    
    static ofShader        _shader;
    
    
};