/*
 - copyright (c) 2011 Copenhagen Institute of Interaction Design (CIID)
 - all rights reserved.
 
 + redistribution and use in source and binary forms, with or without
 + modification, are permitted provided that the following conditions
 + are met:
 +  > redistributions of source code must retain the above copyright
 +    notice, this list of conditions and the following disclaimer.
 +  > redistributions in binary form must reproduce the above copyright
 +    notice, this list of conditions and the following disclaimer in
 +    the documentation and/or other materials provided with the
 +    distribution.
 
 + THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 + "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 + LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 + FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 + COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 + INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 + BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 + OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 + AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 + OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 + OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 + SUCH DAMAGE.
 
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#include "RefractiveIndex.h"

#include "IResponseAnalysis.h"
#include "StrobeAnalysis.h"
#include "ShadowScapesAnalysis.h"

#define CAMERA_ID           1
#define CAMERA_ACQU_WIDTH   640
#define CAMERA_ACQU_HEIGHT  480

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
    //setup_camera();
    
    // gui
    _gui.loadFont("MONACO.TTF", 8);		
	_gui.setup("REFRACTIVE INDEX", 0, 0, ofGetWidth(), ofGetHeight());

    // -> PANEL #0
    _gui.addPanel("configuration", 4, false);
    _gui.setWhichPanel(0);
    // --> COLUMN #0
    _gui.setWhichColumn(0);
    _gui.addToggle("open camera", "CAM_IS_GO", 0);
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
    
    
}

void RefractiveIndex::update()
{
    _gui.update();
}

void RefractiveIndex::draw()
{
    ofBackground(0, 0, 0);    
    
    if(_currentAnalysis)
        _currentAnalysis->draw();
    else 
        _gui.draw();
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
    _vidGrabber.setDeviceID(_vid_id);
	_vidGrabber.setVerbose(true); 
    _vid_stream_open = true;
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
            cout << "bingo!\n\n";
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
        _currentAnalysis = new IResponseAnalysis();
        _analysisAdapator = new AnalysisAdaptor(_currentAnalysis);
        _currentAnalysis->setup(_vid_w, _vid_h);
        _analysisAdapator->start();
    }
}

void RefractiveIndex::grabBackgroundEvent(guiCallbackData & data)
{
    
}


