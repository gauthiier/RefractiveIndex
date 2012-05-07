
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

#define CAMERA_ID           0
#define CAMERA_ACQU_WIDTH   640
#define CAMERA_ACQU_HEIGHT  480

#define LOCATION            "MANCHESTER"

#define ISTATE_UNDEF        0xEEEE
#define ISTATE_START        0xAAAA
#define ISTATE_STOP         0xBBBB
#define ISTATE_TRANSITION   0xCCCC
#define ISTATE_END          0xDDDD


int _state = ISTATE_UNDEF;

int              RefractiveIndex::_mode;
ofPixels         RefractiveIndex::_pixels;
ofVideoGrabber   RefractiveIndex::_vidGrabber;
int              RefractiveIndex::_vid_w, RefractiveIndex::_vid_h, RefractiveIndex::_vid_id;
bool             RefractiveIndex::_vid_stream_open;
bool             RefractiveIndex::_vid_toggle_on;
string           RefractiveIndex::_location;

ofxXmlSettings   RefractiveIndex::XML;

void RefractiveIndex::setup()
{
    camDist=1000;
    ofBackground(0, 0, 0);
    //camera.setOrientation(ofVec3f(1,-1,1));
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

    //getting a warning from the OFlog that the pixels aren't allocated
    //void ofPixels::allocate(int w, int h, ofImageType type)    
    _pixels.allocate(_vid_w, _vid_h, OF_IMAGE_COLOR); 
    
 
    //TODO:  whichever one of these is first - it always runs twice ?
    
    _analysisVector.push_back(new ShadowScapesAnalysis(V));  //1
    _analysisVector.push_back(new ShadowScapesAnalysis(H));  //2
    _analysisVector.push_back(new ShadowScapesAnalysis(D));  //3
	    
    _analysisVector.push_back(new RelaxRateAnalysis());   //4
	
    _analysisVector.push_back(new IResponseAnalysis());   //5
    
    _analysisVector.push_back(new ShapeFromShadingAnalysis());   //6
    
    _analysisVector.push_back(new StrobeAnalysis());   //7
    
    _analysisVector.push_back(new CamNoiseAnalysis());    //8
    
    _analysisVector.push_back(new ColorSingleAnalysis());    //9
    
    _analysisVector.push_back(new ColorMultiAnalysis());    //0
    
    _analysisVector.push_back(new DiffNoiseAnalysis());	  //Q
    
    //_currentAnalysisIndx = 0;
    //_currentAnalysis = _analysisVector.at(_currentAnalysisIndx++); 
    //_state = ISTATE_START;
    
    _currentAnalysis = NULL;
    _state = ISTATE_UNDEF;
  
    
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
    
    
    //allocate fbo for HD
    fbo.allocate(1920,1080);

//    fbo.allocate( _currentAnalysis->_mesh_size_multiplier *_vid_w,_currentAnalysis->_mesh_size_multiplier * _vid_h);
    camera.setPosition(fbo.getWidth()/2, fbo.getHeight()/2,_currentAnalysis->_mesh_size_multiplier *500);
    _meshRotation=0;

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
                //_state = ISTATE_END;
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
    // refractive mauve - this doesn't work... looks weird in various places.
    //ofBackground(113, 110, 136);
    
    // black
    ofBackground(0, 0, 0);
   
    if(_currentAnalysis){
        
        _currentAnalysis->draw();
       
        if(_currentAnalysis->meshIsComplete){
            
            fbo.begin();
            glShadeModel(GL_SMOOTH);  
            ofClear(0,0,0);
            
            camera.begin();
            
                //this is a hack, I don't know how to colour the fbo with black pixels so I'm drawing a massive black rectangle in the  background
                ofPushMatrix();
                    ofTranslate(0, 0,-500);
                    ofSetColor(0, 0, 0);
                    ofRect(-fbo.getWidth(), -fbo.getHeight(), fbo.getWidth()*3, fbo.getHeight()*3);                
                    ofPopMatrix();
                ofSetColor(255);
                
                float xDiff= (fbo.getWidth()- 1.33333*(_currentAnalysis->_mesh_size_multiplier * _vid_w))/2;
                float yDiff= (fbo.getHeight()- 1.0*(_currentAnalysis->_mesh_size_multiplier * _vid_h))/2;
                
                ofTranslate(xDiff,yDiff,-_currentAnalysis->zPlaneAverage );
                    ofScale(1.33333,1.0,1.0);
                    //_currentAnalysis->aMesh.drawVertices();     // TODO: tom - why do you have the vertices drawing here? 
                    _currentAnalysis->aMesh.drawFaces();
                    _currentAnalysis->aMesh.draw();
                
            camera.end();
            fbo.end();

            ofPixels pixels;
            fbo.readToPixels(pixels);
          
            ofSaveImage(pixels,_currentAnalysis->meshFileName, OF_IMAGE_QUALITY_BEST);
            //saving jpgs doesn't work - pngs only!
            // PNG is fine - better for Final Cut anyway! 

            ofDisableBlendMode() ;  
        }
    }
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
    
    /*  TODO:  complete the below... would be good to trigger the Analysis from keypresses if needed... */
    // currently this doesn't work... the save_cb's in the individual 
    // tried to add a  stop_analysis(); call but it blocks the whole programme
    
    // i.e.: ask david how to shut off the prior Analysis if it's not finished running from here? 
    
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
    
    /*
     TO DO:  add a file dialog so we can save images to another hard drive...
     e.g.: http://dev.openframeworks.cc/pipermail/of-dev-openframeworks.cc/2011-April/003125.html
     
>> 		case 's':
>> 			doSave ^= true;
>> 			doLoad = false;
>> 			if(doSave) {
>> 				ofFileDialogResult r = ofSystemLoadDialog("Select path to save to", true);
>> 				if(r.bSuccess) {
>> 					saveCounter = 0;
>> 					savePath = r.getPath();
>> 					ofDirectory::createDirectory(savePath + "/color/");
>> 					ofDirectory::createDirectory(savePath + "/depth/");
>> 					printf("SAVE %s %s\n", r.getPath().c_str(), r.getName().c_str());
>> 				} else {
>> 					doSave = false;
>> 				}
>> 				
>> 			}
>> 			break;
>> 			
>> 		case 'l':
>> 			doLoad ^= true;
>> 			doSave = false;
>> 			if(doLoad) {
>> 				ofFileDialogResult r = ofSystemLoadDialog("Select path to load from", true);
>> 				if(r.bSuccess) {
>> 					loadCounter = 0;
>> 					loadPath = r.getPath();
>> 					ofDirectory dir;
>> 					loadMaxFiles = MAX(dir.listDir(loadPath + "/color"), dir.listDir(loadPath + "/depth"));
>> 					printf("LOAD %i %s %s\n", loadMaxFiles, r.getPath().c_str(), r.getName().c_str());
>> 				} else {
>> 					doLoad = false;
>> 				}
>> 				
>> 			}
>> 			break;
    */
    
    
}

void RefractiveIndex::exit()
{
    if(_currentAnalysis)
        _analysisAdapator->stop();    
    stop_camera();
}