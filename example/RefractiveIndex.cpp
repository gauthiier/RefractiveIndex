
////also the new shit////

#include "RefractiveIndex.h"

#include "ShadowScapesAnalysis.h"
#include "RelaxRateAnalysis.h"
#include "IResponseAnalysis.h"
#include "ShapeFromShadingAnalysis.h"
#include "StrobeAnalysis.h"
#include "CamNoiseAnalysis.h"
#include "ColorSingleAnalysis.h"
#include "ColorMultiAnalysis.h"
#include "DiffNoiseAnalysis.h"

#include "ofxXmlSettings.h"

#include "ofxArcBall.h"

#include <stdio.h>
#include <stdlib.h>

#define CAMERA_ID           0
#define CAMERA_ACQU_WIDTH   640
#define CAMERA_ACQU_HEIGHT  480

#define LOCATION            "MIDDLESBOROUGH"

#define ISTATE_UNDEF        0xEEEE
#define ISTATE_START        0xAAAA
#define ISTATE_STOP         0xBBBB
#define ISTATE_TRANSITION   0xCCCC
#define ISTATE_END          0xDDDD

static const int subdiv_pixels = 4;

static const int VID_W = 640, VID_H = 480;
static const int VERTICES_X = VID_W / subdiv_pixels, VERTICES_Y = VID_W / subdiv_pixels;
static const int TRI_W = 1000;

int draw_style = FACE;

#define ALGO_1  1
#define ALGO_2  2
#define ALGO_3  3
#define ALGO_4  4

int             algo = ALGO_2;
float           scale = 1.f;


int _state = ISTATE_UNDEF;

int              RefractiveIndex::_mode;
ofPixels         RefractiveIndex::_pixels;
ofVideoGrabber   RefractiveIndex::_vidGrabber;
int              RefractiveIndex::_vid_w, RefractiveIndex::_vid_h, RefractiveIndex::_vid_id;
bool             RefractiveIndex::_vid_stream_open;
bool             RefractiveIndex::_vid_toggle_on;
string           RefractiveIndex::_location;

ofxXmlSettings   RefractiveIndex::XML;

ofShader        RefractiveIndex::_shader;
ofVboMesh       RefractiveIndex::_mesh_vbo;

ofxArcBall      RefractiveIndex::cam;

string          msg;

void RefractiveIndex::setup_shader_vbo()
{
    
    int vertices_per_frame = XML.getValue("config:algorithms:vertices_per_frame", TRI_W);
    int pixel_per_vertex = XML.getValue("config:algorithms:pixel_per_vertex", subdiv_pixels);
    
    int vertices_X = _vid_w / pixel_per_vertex, vertices_Y = _vid_h / pixel_per_vertex;
    
    // VBO        
    for(int i = 0; i < vertices_X; i++){
        for(int j = 0; j < vertices_Y; j++) {
            _verts.push_back(ofVec3f((i / (float)vertices_X) * vertices_per_frame, (j / (float) vertices_Y) * vertices_per_frame, 0.0f));
            _tex.push_back(ofVec2f(i / (float)vertices_X * _vid_w, j / (float) vertices_Y * _vid_h));
			if( ( i + 1 < vertices_X ) && ( j + 1 < vertices_Y ) ) {                
                //triangle #1
                _ind.push_back( (i+0) * vertices_Y + (j+0) );
                _ind.push_back( (i+1) * vertices_Y + (j+0) );
                _ind.push_back( (i+1) * vertices_Y + (j+1) );
                
                //triangle #2
                _ind.push_back( (i+1) * vertices_Y + (j+1) );
                _ind.push_back( (i+0) * vertices_Y + (j+1) );
                _ind.push_back( (i+0) * vertices_Y + (j+0) );
			}
        }
    }
    
    //ofEnableNormalizedTexCoords();
    
    _mesh_vbo.addVertices(_verts);
    _mesh_vbo.addTexCoords(_tex);
    _mesh_vbo.addIndices(_ind);
    
    _mesh_vbo.setMode(OF_PRIMITIVE_TRIANGLES);
        
    // geometry shader
    
	_shader.setGeometryInputType(GL_TRIANGLES);
	_shader.setGeometryOutputType(GL_TRIANGLES);
	_shader.setGeometryOutputCount(3);
    _shader.load("dviid/rfi.vert.glsl", "dviid/rfi.frag.glsl", "dviid/rfi.geom.glsl"); 	
	printf("Maximum number of output vertices support is: %i\n", _shader.getGeometryMaxOutputCount());    
}


void RefractiveIndex::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofHideCursor();
    
    bool save_config = false;
    
    cout << "Loading configuration..." << endl;
    if(XML.loadFile("config.refindx") == false) {
        ofLog(OF_LOG_ERROR) << "error loading config - using default.";
        save_config = true;                
    } else {
        XML.loadFile("config.refindx");
    }
    
    // <mode>
    string m = XML.getValue("config:mode", "analysing");
    _mode = (m == "analysing" ? MODE_ANALYSING : (m == "drawing" ? MODE_DRAWING : MODE_ANALYSING));
        
    // <camera>
    _vid_id = XML.getValue("config:camera:id", CAMERA_ID);
    cout << "_vid_id: " << _vid_id << endl;
    
    _vid_w  = XML.getValue("config:camera:width", CAMERA_ACQU_WIDTH);
    _vid_h  = XML.getValue("config:camera:height", CAMERA_ACQU_HEIGHT);
    
    // <display>
    int fps = XML.getValue("config:display:fps", 30);
            
    // <location>
    _location = XML.getValue("config:locale:name", LOCATION);
        
    cout << "Configuring..." << endl;

    // display
    cout << "> display" << endl;
    ofSetFrameRate(fps);
    
    if(fps > 30) {
        ofSetVerticalSync(FALSE);            
    } else {        
        ofSetVerticalSync(TRUE);
    }
    cout << "* fps = " << fps << endl;
    
    // camera
    cout << "> camera" << endl;
    cout << "* cam id = " << _vid_id << endl;
    cout << "* cam width = " << _vid_w << endl;
    cout << "* cam height = " << _vid_h << endl;
    
    if(_mode == MODE_ANALYSING) {
        _vid_stream_open = false;    
        setup_camera();
    }
    
    cout << "RRRRRREADY!" << endl;        

    _analysisAdapator = NULL;

    _pixels.allocate(_vid_w, _vid_h, OF_IMAGE_COLOR_ALPHA); 
    
    
    setup_shader_vbo();
    
    
    //TODO:  whichever one of these is first - it always runs twice ?    
    
    _analysisVector.push_back(new ShadowScapesAnalysis(V)); //1
    _analysisVector.push_back(new ShadowScapesAnalysis(H)); //2
    _analysisVector.push_back(new ShadowScapesAnalysis(D)); //3
    _analysisVector.push_back(new RelaxRateAnalysis());     //4
    _analysisVector.push_back(new IResponseAnalysis());     //5
    _analysisVector.push_back(new ShapeFromShadingAnalysis()); //6   
    _analysisVector.push_back(new StrobeAnalysis());        //7
    _analysisVector.push_back(new CamNoiseAnalysis());      //8
    _analysisVector.push_back(new ColorSingleAnalysis());   //9
    _analysisVector.push_back(new ColorMultiAnalysis());    //10
    _analysisVector.push_back(new DiffNoiseAnalysis());	    //11
    
    _currentAnalysis = NULL;
    _state = ISTATE_UNDEF;
    
    // disbale <ESC> ?
    //ofSetEscapeQuitsApp(false);
    
}

void RefractiveIndex::analysis_cb(string & analysis)
{
    assert(analysis == _currentAnalysis->_name);
    
    _state = ISTATE_STOP;    
}

void RefractiveIndex::start_analysis()
{
    ofAddListener(_currentAnalysis->_synthesize_cb, this, &RefractiveIndex::analysis_cb);
    _analysisAdapator = new AnalysisAdaptor(_currentAnalysis);
    _currentAnalysis->setup(_vid_w, _vid_h);
    _analysisAdapator->start();    
}

void RefractiveIndex::stop_analysis()
{
    if(_analysisAdapator == NULL) return;
    
    _analysisAdapator->stop(); //blocking
    ofRemoveListener(_currentAnalysis->_synthesize_cb, this, &RefractiveIndex::analysis_cb);
    _currentAnalysis = NULL;
    delete _analysisAdapator;
    _analysisAdapator = NULL;    
}

void RefractiveIndex::state_analysis()
{
    switch (_state) {
        case ISTATE_START:
            start_analysis();    
            _state = ISTATE_UNDEF;
            break;
        case ISTATE_TRANSITION:            
            if(_currentAnalysisIndx >= _analysisVector.size()) {
                _currentAnalysisIndx = 0;
                _currentAnalysis = _analysisVector.at(_currentAnalysisIndx++);
                _state = ISTATE_START;                
            } else {
                _currentAnalysis = _analysisVector.at(_currentAnalysisIndx++);
                _state = ISTATE_START;
            }
            break;
        case ISTATE_STOP:
            stop_analysis(); // blocking
            if(_mode == MODE_DRAWING)
                _state = ISTATE_UNDEF;
            else
                _state = ISTATE_TRANSITION;
            break;
        case ISTATE_END:
            if(_mode == MODE_ANALYSING)
                stop_camera();
            ::exit(1);
            break;
        case ISTATE_UNDEF:
            break;            
        default:
            break;
    }
}

void RefractiveIndex::update()
{    
    state_analysis();
}

void RefractiveIndex::draw()
{    
    // black
    ofBackground(0, 0, 0);
           
    if(_currentAnalysis)
        _currentAnalysis->draw();
}

void RefractiveIndex::setup_camera()
{
    stop_camera();
    
    // THIS IS LOADED IN FROM THE XML FILE SETTINGS 
    _vidGrabber.setDeviceID(_vid_id);
    _vidGrabber.listDevices();
    
    if(!_vidGrabber.initGrabber(_vid_w, _vid_h)) {
        ofLog(OF_LOG_ERROR) << "RefractiveIndex::setup_camera - could not initialise grabber";
        return;
    }
    
    _vidGrabber.setVerbose(true);
    _vidGrabber.setUseTexture(false);
    _vid_stream_open = true;
    cout << "CAMERA SETUP " << endl;
    return;
	
}

void RefractiveIndex::stop_camera()
{
    if(_vid_stream_open) {
        _vidGrabber.close();
        _vid_stream_open = false;
    }    
}

void RefractiveIndex::keyPressed  (int key)
{
    
    if( key =='f')
        ofToggleFullscreen();
    
    if(key == 'x')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysis = NULL;
        _state = ISTATE_UNDEF;
    }    
    
    else if(key == '1')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 0;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }

    else if(key == '2')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 1;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }

    else if(key == '3')
    {        
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 2;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }
    
    else if(key == '4')
    {     
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 3;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }
    
    else if(key == '5')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 4;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }
    
    else if(key == '6')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 5;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }
    
    else if(key == '7')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 6;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }
    
    else if(key == '8')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 7;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }
    
    else if(key == '9')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 8;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }
    
    else if(key == '0')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 9;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }

    else if(key == 'q')
    {
        if(_currentAnalysis)
            _analysisAdapator->stop();
        _currentAnalysisIndx = 10;
        if(!_currentAnalysis)
            _state = ISTATE_TRANSITION;            
    }    
    
}

void RefractiveIndex::exit()
{
    if(_currentAnalysis)
        _analysisAdapator->stop();    
    stop_camera();
}