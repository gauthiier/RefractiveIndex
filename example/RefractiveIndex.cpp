/*
 
 todo:
 (1) Look at warinings about the #define which get over written
 
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "RefractiveIndex.h"

#include "IResponseAnalysis.h"
#include "StrobeAnalysis.h"
#include "ShadowScapesAnalysis.h"
#include "ColorMultiAnalysis.h"
#include "ColorSingleAnalysis.h"
#include "CamFrameRateAnalysis.h"
#include "CamNoiseAnalysis.h"
#include "LatencyTestAnalysis.h"
#include "DiffNoiseAnalysis.h"

#include "ofxXmlSettings.h"

#define CAMERA_ID           1
#define CAMERA_ACQU_WIDTH   640
#define CAMERA_ACQU_HEIGHT  480

#define LOCATION            "????-not-config-???"

#define ISTATE_ACQU_START       0xAAAA
#define ISTATE_ACQU_STOP        0xBBBB
#define ISTATE_SYNTH_START      0xCCCC
#define ISTATE_SYNTH_STOP       0xDDDD

#define ISTATE_TRANSITION       0xFFFF
#define ISTATE_UNDEF            0xEEEE
#define ISTATE_END              0x1111

int _state = ISTATE_UNDEF;

bool _in_acquisition = true;

ofPixels         RefractiveIndex::_pixels;
ofVideoGrabber   RefractiveIndex::_vidGrabber;
int              RefractiveIndex::_vid_w, RefractiveIndex::_vid_h, RefractiveIndex::_vid_id;
bool             RefractiveIndex::_vid_stream_open;
bool             RefractiveIndex::_vid_toggle_on;
string           RefractiveIndex::_location;

ofxXmlSettings   XML;

void RefractiveIndex::setup()
{
    bool save_config = false;
    
    cout << "Loading configuration..." << endl;
    if(!XML.loadFile("../data/config.refindx")) {
        ofLog(OF_LOG_ERROR) << "error loading config - using default.";
        save_config = true;                
    }
        
    // <camera>
    _vid_id = XML.getValue("config:camera:id", CAMERA_ID);
    _vid_w = XML.getValue("config:camera:width", CAMERA_ACQU_WIDTH);
    _vid_h = XML.getValue("config:camera:height", CAMERA_ACQU_HEIGHT);
    
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
    
    _vid_stream_open = false;    
    setup_camera();
    
    cout << "RRRRRREADY!" << endl;        

    //getting a warning from the OFlog that the pixels aren't allocated
    //void ofPixels::allocate(int w, int h, ofImageType type)    
    
    
    // setup analysis
    AnalysisAdaptor* adpt;
    
    adpt = new AnalysisAdaptor(new ShadowScapesAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    adpt = new AnalysisAdaptor(new StrobeAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    adpt = new AnalysisAdaptor(new IResponseAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    adpt = new AnalysisAdaptor(new ColorMultiAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    adpt = new AnalysisAdaptor(new CamFrameRateAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    adpt = new AnalysisAdaptor(new CamNoiseAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    adpt = new AnalysisAdaptor(new ColorSingleAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    adpt = new AnalysisAdaptor(new LatencyTestAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    adpt = new AnalysisAdaptor(new DiffNoiseAnalysis());
    _acquisitionMap[adpt->_analysis->_name];
    
    _currentAnalysisIndx = _acquisitionMap.begin();
    _currentAnalysisAdaptor = _currentAnalysisIndx->second;
    _currentAnalysis = _currentAnalysisAdaptor->_analysis;
    
    _state = ISTATE_ACQU_START;
    
}

void RefractiveIndex::acquire_cb(string & analysis)
{
    assert(analysis == _currentAnalysis->_name);    
    _state = ISTATE_ACQU_STOP;    
}

void RefractiveIndex::synthesize_cb(string & analysis)
{
    TAnalysisMap::iterator it = _synthesisMap.find(analysis);
    if(it != _synthesisMap.end()) {
        AnalysisAdaptor* adpt = it->second;
        AbstractAnalysis* a = adpt->_analysis;        
        adpt->stop();
        ofRemoveListener(a->_acquire_cb, this, &RefractiveIndex::acquire_cb);
        ofRemoveListener(a->_synthesize_cb, this, &RefractiveIndex::synthesize_cb);                
    }
    
    //_state = ISTATE_SYNTH_STOP;    
}


void RefractiveIndex::start_analysis()
{
    ofAddListener(_currentAnalysis->_acquire_cb, this, &RefractiveIndex::acquire_cb);
    ofAddListener(_currentAnalysis->_synthesize_cb, this, &RefractiveIndex::synthesize_cb);
    _currentAnalysis->setup(_vid_w, _vid_h);
    _currentAnalysisAdaptor->start();    
}

void RefractiveIndex::stop_analysis()
{
    if(_currentAnalysisAdaptor == NULL) return;
    
    _currentAnalysisAdaptor->stop(); //blocking
    ofRemoveListener(_currentAnalysis->_acquire_cb, this, &RefractiveIndex::acquire_cb);
    ofRemoveListener(_currentAnalysis->_synthesize_cb, this, &RefractiveIndex::synthesize_cb);
    _currentAnalysis = NULL;
    _currentAnalysisAdaptor = NULL;    
}

void RefractiveIndex::state_machine_analysis()
{
    static int synth_cnt = 0;
    
    switch (_state) {
        case ISTATE_ACQU_START:
            start_analysis();    
            _state = ISTATE_UNDEF;
            break;
        case ISTATE_ACQU_STOP:
            // continue to synthesis        
            _acquisitionMap.erase(_currentAnalysisIndx);
            _synthesisMap[_currentAnalysis->_name] = _currentAnalysisAdaptor;            
            _state = ISTATE_TRANSITION;
            break;                        
        case ISTATE_TRANSITION:          
            if(_currentAnalysisIndx != _acquisitionMap.end()) {
                _currentAnalysisIndx++;
                _currentAnalysisAdaptor = _currentAnalysisIndx->second;
                _currentAnalysis = _currentAnalysisAdaptor->_analysis;                
            } else {
                if(_synthesisMap.size() > 0) {
                    if(_in_acquisition) {
                        _currentAnalysisIndx = _synthesisMap.begin();
                        _in_acquisition = false;
                    }
                    if(!_acquisitionMap.empty() && _currentAnalysisIndx != _synthesisMap.end()) {
                        _currentAnalysisIndx++;
                        _currentAnalysisAdaptor = _currentAnalysisIndx->second;
                        _currentAnalysis = _currentAnalysisAdaptor->_analysis; 
                        _state = ISTATE_UNDEF;
                    } else if(_acquisitionMap.empty()) {
                        _state = ISTATE_END;
                    }
                }
            }
            break;  
        case ISTATE_SYNTH_STOP:
            
        case ISTATE_END:
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
    state_machine_analysis();
    
    RefractiveIndex::_vidGrabber.grabFrame();  // get a new frame from the camera

    if (_vidGrabber.isFrameNew())
    {
        _pixels = _vidGrabber.getPixelsRef(); //get ofPixels from the camera
    }
}

void RefractiveIndex::draw()
{
    ofBackground(0, 0, 0);
    if(_currentAnalysis)
        _currentAnalysis->draw();
}

void RefractiveIndex::setup_camera()
{
    stop_camera();

    if(!_vidGrabber.initGrabber(_vid_w, _vid_h)) {
        ofLog(OF_LOG_ERROR) << "RefractiveIndex::setup_camera - could not initialise grabber";
        return;
    }
    _vidGrabber.listDevices();
	_vidGrabber.setVerbose(true);
    _vid_stream_open = true;
    _vidGrabber.setDeviceID(_vid_id);

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
}

void RefractiveIndex::exit()    
{
    stop_camera();
}