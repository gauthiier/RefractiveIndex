/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "DiffNoiseAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;


void DiffNoiseAnalysis::setup(int camWidth, int camHeight)
{
    DELTA_T_SAVE = 600;  // right number is about 450
    NUM_PHASE = 1;
    NUM_RUN = 1;
    NUM_SAVE_PER_RUN = 50;    
    
    create_dir();
    _fade_cnt=0;
    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    c = 0;
}


void DiffNoiseAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<DiffNoiseAnalysis> save_callback(*this, &DiffNoiseAnalysis::save_cb);

    // RUN ROUTINE
    for(int i = 0; i < NUM_RUN; i++) {

        _run_cnt = i;

        cout << "RUN NUM = " << i;

        save_timer = new Timer(0, DELTA_T_SAVE); // timing interval for saving files
        save_timer->start(save_callback);
        _RUN_DONE = false;
        _frame_cnt = 0; _save_cnt = 0;

        while(!_RUN_DONE)
            Thread::sleep(3);

        save_timer->stop();
    }
}

void DiffNoiseAnalysis::synthesise()
{
    //incrementer to whichMesh
    speed=0.2;
    //whichMesh is the index in the vector of meshes
    whichMesh=0;
    
    int index=0;
    float iterator=1;
    bool debug=false;
    if(debug){
        _saved_filenames.clear();
        _saved_filenames=getListOfImageFilePaths("MIDDLESBOROUGH", _name);
        
        //hack to limit number of meshes.
        if(_saved_filenames.size()>100){
            iterator= _saved_filenames.size() /100;
        }
        
    }
    //clear vector so we don't add to it on successive runs
    meshes.clear();
    
    for(float i=0;i<_saved_filenames.size()-1;i+=iterator){
        
        
        ofImage image1;
        ofImage image2;
        
        //there is a known issue with using loadImage inside classes in other directories. the fix is to call setUseTExture(false)
        image1.setUseTexture(false);
        image2.setUseTexture(false);
        //some of the textures are not loading correctly so only make mesh if both the images load
        if(image1.loadImage(_saved_filenames[i]) && image2.loadImage(_saved_filenames[i+1])){
            meshes.push_back(ofMesh());
            cout<<"setting mesh"<<endl;
            int _recorded_brightness_value=getRecordedValueFromFileName(_saved_filenames[i]);
            setMeshFromPixels( calculateListOfZValues(image1,image2, COMPARE_HUE,_recorded_brightness_value), image2, &meshes[index]);            
            index++;
        }
    }
    
}

void DiffNoiseAnalysis::display_results(){
    
    Timer* display_results_timer;
    
    TimerCallback<DiffNoiseAnalysis> display_results_callback(*this, &DiffNoiseAnalysis::display_results_cb);
    // display results of the synthesis
    
    display_results_timer = new Timer(0, 20); // timing interval for saving files
    display_results_timer->start(display_results_callback);
    _RUN_DONE = false;
    _results_cnt=0;
    _results_cnt_max=300;
    
    while(!_RUN_DONE)
        Thread::sleep(3);
    
    display_results_timer->stop();
    
}// this runs at frame rate = 33 ms for 30 FPS

void DiffNoiseAnalysis::draw()
{
    switch (_state) {
        case STATE_ACQUIRING:
        {
            
            if (_frame_cnt < _frame_cnt_max)
            {
      
                ofColor aColour;
                int rectSize = 5;
                ofEnableAlphaBlending();
                
                int _fade_in_frames = _frame_cnt_max/10;
                
                if (_frame_cnt < _fade_in_frames) {
                    
                    for (int i=1; i < ofGetHeight() ; i=i+rectSize)
                    {
                        for (int j=1; j < ofGetWidth(); j=j+rectSize)
                        {
                            c = ofRandom(0,255);
                            aColour.set(c, c, c, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                            ofSetColor(aColour);
                            ofRect(j, i, rectSize, rectSize);
                        }
                    }        
                    
                    cout << "FADING IN..." << endl;
                }
                
                
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt < (_frame_cnt_max-_fade_in_frames)){
                    
                    for (int i=1; i < ofGetHeight() ; i=i+rectSize)
                    {
                        for (int j=1; j < ofGetWidth(); j=j+rectSize)
                        {
                            c = ofRandom(0,255);
                            aColour.set(c, c, c, 255);
                            ofSetColor(aColour);
                            ofRect(j, i, rectSize, rectSize);
                            
                        }
                    }        
                }
                
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt < _frame_cnt_max) {
                    
                    for (int i=1; i < ofGetHeight() ; i=i+rectSize)
                    {
                        for (int j=1; j < ofGetWidth(); j=j+rectSize)
                        {
                            c = ofRandom(0,255);
                            aColour.set(c, c, c, 255-ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                            ofSetColor(aColour);
                            ofRect(j, i, rectSize, rectSize);
                        } 
                    }
                    
                    _fade_cnt++;
                    cout << "FADING OUT..." << endl;
                    
                }
                
                
                ofDisableAlphaBlending();
           
            } else {
            
               _RUN_DONE = true;
            
            }
            
            _frame_cnt++;
            
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...
            break;
        }
            
        case STATE_DISPLAY_RESULTS:
        {
            // display results of the synthesis
            // display results of the synthesis
            int imageWidth=640;
            int imageHeight =480;
            ofPushMatrix();
            ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
            ofRotateY(_results_cnt*0.3);
            //ofRotateX(90);
            //ofRotateZ(whichMesh);
            ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2),-400;
            ofTranslate((ofGetWidth()/2)-(imageWidth/2),0,0 );
            
            meshes[whichMesh].drawVertices();
            ofPopMatrix();
            whichMesh+=speed;
            cout<<whichMesh<<" size of meshes "<<meshes.size()<<endl;
            if(whichMesh>meshes.size() -1 || whichMesh<0){
                speed*=-1;
                whichMesh+=speed;
                
            }
            

            break;
        }
            
            
        default:
            break;
    }

}

// this runs at save_cb timer rate = DELTA_T_SAVE
void DiffNoiseAnalysis::save_cb(Timer& timer)
{


    float rand10 = ofRandom(0,10);

    if (rand10 > 5.0) {

    cout << "DiffNoiseAnalysis::saving...\n";
    cout << "c_last... " << c << endl;
    cout<<"rand10... " <<rand10<<endl;

    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    string thisLocation = RefractiveIndex::_location;

    //RefractiveIndex::_pixels = RefractiveIndex::_vidGrabber.getPixelsRef(); //get ofPixels from the camera
    //    fileName = imageSaveFolderPath+whichAnalysis+"_"+ofToString(100.0*i*scanLineSpeed/ofGetHeight(),2)+"%_"+ofToString(i)+".jpg";
    //ofSaveImage(vectorOfPixels[i], fileName, OF_IMAGE_QUALITY_BEST);
        
    string file = _whole_file_path+"/"+file_name;

    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
        
    _saved_filenames.push_back(ofToDataPath("")+file);

    }
    _save_cnt++;
    //if(_save_cnt >= NUM_SAVE_PER_RUN)
    //    _RUN_DONE = true;

}
void DiffNoiseAnalysis::display_results_cb(Timer& timer){
    _results_cnt++;
    if (_results_cnt>_results_cnt_max) {
        _RUN_DONE=true;
    }
}


