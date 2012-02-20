/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "ColorSingleAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;


void ColorSingleAnalysis::setup(int camWidth, int camHeight)
{
    DELTA_T_SAVE = 100;  //300 is the right number here
    NUM_PHASE = 1;
    NUM_RUN = 1;
    NUM_SAVE_PER_RUN = 100;    
    
    create_dir();
    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    r = 0;
    g = 0;
    b = 0;
    
    _fade_cnt=0;
    fileNameTag = "";
}


void ColorSingleAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<ColorSingleAnalysis> save_callback(*this, &ColorSingleAnalysis::save_cb);

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

void ColorSingleAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
}


void ColorSingleAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            
            
            if (_frame_cnt < _frame_cnt_max)
            {
                    
                float one_third_of_frame_count_max=_frame_cnt_max/3;
                int _fade_in_frames = one_third_of_frame_count_max/10;
                
                if (_frame_cnt < _fade_in_frames){
                    ofSetColor(ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255), 0, 0);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    fileNameTag = "FADING";
                }
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt < one_third_of_frame_count_max){
                    r=255.0;
                    g=0.0;
                    b=0.0;
                    ofSetColor(r,g,b);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    fileNameTag = "RED";
                }
                
                if (_frame_cnt >= one_third_of_frame_count_max && _frame_cnt < 2*one_third_of_frame_count_max){
                    r=0.0;
                    g=255.0;
                    b=0.0;
                    ofSetColor(r,g,b);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    fileNameTag = "GREEN";
                }
                
                if (_frame_cnt >= 2*one_third_of_frame_count_max && _frame_cnt < (_frame_cnt_max-_fade_in_frames) ){
                    r=0.0;
                    g=0.0;
                    b=255.0;
                    ofSetColor(r,g,b);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    fileNameTag = "BLUE";
                }
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt < _frame_cnt_max){
                   
                    int fade = ofMap(_fade_cnt, 0, _fade_in_frames, 0, 255);
                    ofSetColor(0, 0, 255-fade);
        
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    _fade_cnt++;
                    fileNameTag = "FADING";
                }
            
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
void ColorSingleAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    cout << "ColorSingleAnalysis::saving...\n";
    
    string file_name =ofToString(_save_cnt,2)+"_"+fileNameTag+"_"+ofToString(_run_cnt,2)+".jpg";
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(_whole_file_path+"/"+file_name);


}
