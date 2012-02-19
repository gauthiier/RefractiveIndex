/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "CamNoiseAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;


void CamNoiseAnalysis::setup(int camWidth, int camHeight)
{
    DELTA_T_SAVE = 200;
    NUM_PHASE = 1;
    NUM_RUN = 1;
    NUM_SAVE_PER_RUN = 100;    
    
    create_dir();

    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    c = 0;
}


void CamNoiseAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<CamNoiseAnalysis> save_callback(*this, &CamNoiseAnalysis::save_cb);

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

void CamNoiseAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
}



// this runs at frame rate = 33 ms for 30 FPS
void CamNoiseAnalysis::draw()
{
    switch (_state) {
        case STATE_ACQUIRING:
        {
            /// *** TODO  *** ///
            // still need to deal with latency frames here - i.e.: there are frames
            /// *** TODO  *** ///
              
            if (_frame_cnt < _frame_cnt_max)
            {
                
                ofEnableAlphaBlending();
                ofColor aColour;
                
                int _fade_in_frames = _frame_cnt_max/10;
                float _number_of_grey_levels=10;
                
                float _frames_per_level = _frame_cnt_max / _number_of_grey_levels;
                ofColor someColor;
                
                if (_frame_cnt < _fade_in_frames) {
                    aColour.set(255, 255, 255, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                    ofSetColor(aColour);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    //cout <<  "FADE IN STROBE TIME " << endl;    
                }
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt < (_frame_cnt_max-_fade_in_frames)){
                    
        
                    for(int i=0;i<_number_of_grey_levels;i++){
                        if (_frame_cnt>= _frames_per_level *( i-1) && +_frame_cnt < _frames_per_level * (i) ) {
                            //set colour to current grey level
                            c=255-( 255.0 * ( i /_number_of_grey_levels));
                            someColor.set(c);
                        }
                        ofSetColor(someColor);
                        ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    }
                    
                }
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt < _frame_cnt_max) {
                        aColour.set(0, 0, 0, 255-ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                        ofSetColor(aColour);
                        ofRect(0, 0, ofGetWidth(), ofGetHeight());
                        // cout <<  "FADE OUT STROBE TIME " << endl;
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
void CamNoiseAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;

    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    string thisLocation = RefractiveIndex::_location;
    
    string file = _whole_file_path+"/"+file_name;

    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(file);

    //if(_save_cnt >= NUM_SAVE_PER_RUN)
    //    _RUN_DONE = true;

}
