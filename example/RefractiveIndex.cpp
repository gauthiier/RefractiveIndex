/*
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

#define CAMERA_ID           1
#define CAMERA_ACQU_WIDTH   640
#define CAMERA_ACQU_HEIGHT  480

ofPixels         RefractiveIndex::_pixels;
ofVideoGrabber   RefractiveIndex::_vidGrabber;
int              RefractiveIndex::_vid_w, RefractiveIndex::_vid_h, RefractiveIndex::_vid_id;
bool             RefractiveIndex::_vid_stream_open;
bool             RefractiveIndex::_vid_toggle_on;
string           RefractiveIndex::_location;

void RefractiveIndex::setup()
{
    // rate
    ofSetFrameRate(30);
    ofSetVerticalSync(TRUE);

    // camera
    _vid_w  = CAMERA_ACQU_WIDTH;
    _vid_h  = CAMERA_ACQU_HEIGHT;
    _vid_id = CAMERA_ID;
    _vid_stream_open = false;
    _vid_toggle_on = false;

    // gui
    _gui.loadFont("MONACO.TTF", 8);
	_gui.setup("REFRACTIVE INDEX", 0, 0, ofGetWidth(), ofGetHeight());

    // -> PANEL #0
    _gui.addPanel("configuration", 4, false);
    _gui.setWhichPanel(0);
    // --> COLUMN #0
    _gui.setWhichColumn(0);

    //GET THE INPUT NAMES FROM THE QT VIDEO GRABBER

    _gui.addToggle("more cam settings", "SETTINGS", 0);

    _gui.addToggle("turn on camera", "CAM_IS_GO", 0);
    _gui.addButtonSlider("camera width", "CAM_WIDTH", _vid_w, CAMERA_ACQU_WIDTH, 1920, true);
	_gui.addButtonSlider("camera height", "CAM_HEIGHT", _vid_h, CAMERA_ACQU_HEIGHT, 1080, true);

    _gui.setWhichColumn(1);
    _gui.addToggle("run", "RUN", 0);

    _gui.setupEvents();
	_gui.enableEvents();
    //  -- this gives you back an ofEvent for all events in this control panel object
	ofAddListener(_gui.guiEvent, this, &RefractiveIndex::eventsIn);

    _currentAnalysis = NULL;
    _analysisAdapator = NULL;

    //getting a warning from the OFlog that the pixels aren't allocated
    // void ofPixels::allocate(int w, int h, ofImageType type)

    //_pixels.allocate(
    _location="MIDDLESBOROUGH";
    //setup_camera();

}

void RefractiveIndex::update()
{
    _gui.update();
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

    // i would like to pass the pixels we've just got from the camera into the draw function for the current analysis here
    // but the way that 'draw' functino in _currentAnalysis, which is an AbstractAnalysis, is as a "Pure Virtual Function"
    // which i think means it can't be passed any arguments or data???

    //_currentAnalysis->draw(_pixels);

    else
        _gui.draw();

    // if there is a new frame in the camera
   /* if (_vidGrabber.isFrameNew())
    {
       _vidGrabber.draw(0,0); //get ofPixels from the camera
    }*/
}

void RefractiveIndex::setup_camera()
{
    if(_vid_stream_open) {
        _vidGrabber.close();
        _vid_stream_open = false;
    }

    if(!_vidGrabber.initGrabber(_vid_w, _vid_h)) {
        ofLog(OF_LOG_ERROR) << "RefractiveIndex::setup_camera - could not initialise grabber";
        return;
    }
    _vidGrabber.listDevices();
	_vidGrabber.setVerbose(true);
    _vid_stream_open = true;
    _vidGrabber.setDeviceID(_vid_id);

}

void RefractiveIndex::keyPressed  (int key)
{
    if( key =='f')
        ofToggleFullscreen();

    else if( key =='s') {
        if(_currentAnalysis && _analysisAdapator) {
            _analysisAdapator->stop();
            delete _currentAnalysis;
            delete _analysisAdapator;
            _currentAnalysis = NULL;
            _analysisAdapator = NULL;
            cout << "bingo!\n\n";   //bingo means 'stop analysis'?
        }
    }
}

void RefractiveIndex::mouseDragged(int x, int y, int button)
{
    _gui.mouseDragged(x, y, button);
}


void RefractiveIndex::mousePressed(int x, int y, int button)
{
    _gui.mousePressed(x, y, button);
}


void RefractiveIndex::mouseReleased(int x, int y, int button)
{
    _gui.mouseReleased();
}


void RefractiveIndex::eventsIn(guiCallbackData& data)
{
    if(data.getDisplayName() == "run"){

        ofLog(OF_LOG_VERBOSE) << "run...";

        //_currentAnalysis = new ShadowScapesAnalysis();  // create an analysis and give it an adaptor
        //_currentAnalysis = new StrobeAnalysis();  // create an analysis and give it an adaptor
        //_currentAnalysis = new IResponseAnalysis();  // create an analysis and give it an adaptor
        //_currentAnalysis = new ColorMultiAnalysis();
        //_currentAnalysis = new CamFrameRateAnalysis();
        //_currentAnalysis = new CamNoiseAnalysis();
        _currentAnalysis = new ColorSingleAnalysis();
        //_currentAnalysis = new LatencyTestAnalysis();
        //_currentAnalysis = new DiffNoiseAnalysis();

        _analysisAdapator = new AnalysisAdaptor(_currentAnalysis);  //Adaptors start and stop
        _currentAnalysis->setup(_vid_w, _vid_h);
        _analysisAdapator->start();
    }

    if(data.getDisplayName() == "turn on camera" ){

        _vid_toggle_on=!_vid_toggle_on;

        if (_vid_toggle_on)
        {
            setup_camera();
        } else if (!_vid_toggle_on) {
            _vidGrabber.close();
        }
    }

    //more cam settings", "SETTINGS"
    if( data.getDisplayName() == "more cam settings" ){
        _vidGrabber.videoSettings();
    }

}

void RefractiveIndex::grabBackgroundEvent(guiCallbackData & data)
{

}


