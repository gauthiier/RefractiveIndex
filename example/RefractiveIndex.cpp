/*
 
 todo:
 (1) Look at warinings about the #define which get over written
 
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

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

#define CAMERA_ID           1
#define CAMERA_ACQU_WIDTH   640
#define CAMERA_ACQU_HEIGHT  480

#define LOCATION            "MIDDLESBOROUGH"

#define ISTATE_UNDEF        0xEEEE
#define ISTATE_START        0xAAAA
#define ISTATE_STOP         0xBBBB
#define ISTATE_TRANSITION   0xCCCC
#define ISTATE_END          0xDDDD

int _state = ISTATE_UNDEF;

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

    _analysisAdapator = NULL;

    //getting a warning from the OFlog that the pixels aren't allocated
    //void ofPixels::allocate(int w, int h, ofImageType type)    
    
    
    //TODO:  whichever one of these is first - it always runs twice ?
    
    _analysisVector.push_back(new ShadowScapesAnalysis(V));
    _analysisVector.push_back(new ShadowScapesAnalysis(H));
    _analysisVector.push_back(new ShadowScapesAnalysis(D));
    
    _analysisVector.push_back(new RelaxRateAnalysis());
    
    _analysisVector.push_back(new IResponseAnalysis());
    
    _analysisVector.push_back(new ShapeFromShadingAnalysis());
    
    _analysisVector.push_back(new StrobeAnalysis());
    
    _analysisVector.push_back(new CamNoiseAnalysis());
    
    _analysisVector.push_back(new ColorSingleAnalysis());
    
    _analysisVector.push_back(new ColorMultiAnalysis());
    
    _analysisVector.push_back(new DiffNoiseAnalysis());
        

    _currentAnalysisIndx = 0;
    _currentAnalysis = _analysisVector.at(_currentAnalysisIndx); 
    
    _state = ISTATE_START;
    
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
                _state = ISTATE_END;
            } else {
                _currentAnalysis = _analysisVector.at(_currentAnalysisIndx++);
                _state = ISTATE_START;
            }
            break;
        case ISTATE_STOP:
            stop_analysis(); // blocking
            _state = ISTATE_TRANSITION;
            break;
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
    state_analysis();
    
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
    
    /*  TODO:  complete the below... would be good to trigger the Analysis from keypresses if needed... */
    // currently this doesn't work... the save_cb's in the individual 
    // tried to add a  stop_analysis(); call but it blocks the whole programme
    
    // i.e.: ask david how to shut off the prior Analysis if it's not finished running from here? 
    
    /*
    if(key == '1')
    {
        // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(0);
        _state = ISTATE_START;
    }

    if(key == '2')
    {
     
     // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(1);
        _state = ISTATE_START;
    }

    if(key == '3')
    {
     
     // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(2);
        _state = ISTATE_START;
    }
    
    if(key == '4')
    {
     
     // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(3);
        _state = ISTATE_START;
    }
    
    if(key == '5')
    {
     
     // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(4);
        _state = ISTATE_START;
    }
    
    if(key == '6')
    {
     
     // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(5);
        _state = ISTATE_START;
    }
    
    if(key == '7')
    {
     
     // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(6);
        _state = ISTATE_START;
    }
    
    if(key == '8')
    {
     
     // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(7);
        _state = ISTATE_START;
    }
    
    if(key == '9')
    {
     
     // something needs to go here to cancel the prior analysis
        _currentAnalysis = _analysisVector.at(8);
        _state = ISTATE_START;
    }
     */
}

void RefractiveIndex::exit()
{
    stop_camera();
}