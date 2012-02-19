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
    // _saved_filenames has all the file names of all the saved images
}


// this runs at frame rate = 33 ms for 30 FPS
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
            break;
        }
            
            
        default:
            break;
    }

}

// this runs at save_cb timer rate = DELTA_T_SAVE
void DiffNoiseAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    cout << "DiffNoiseAnalysis::saving...\n";
    
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(_whole_file_path+"/"+file_name);

    _save_cnt++;
    //if(_save_cnt >= NUM_SAVE_PER_RUN)
    //    _RUN_DONE = true;

}
