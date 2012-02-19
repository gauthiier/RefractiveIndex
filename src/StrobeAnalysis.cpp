/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "StrobeAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

void StrobeAnalysis::setup(int camWidth, int camHeight)
{
    DELTA_T_SAVE = 100;
    NUM_RUN = 1;
    
    _strobe_cnt = 0;
    _strobe_cnt_max = 20;  // 40 x 500 ms = 20000 ms = 20 seconds run time
    _strobe_interval = 1000;  //every 0.5seconds = 15 frames
    _frame_cnt_max = _strobe_cnt_max * _strobe_interval * ofGetFrameRate()/1000;
    
    // The British Health and Safety Executive recommend that a net flash rate for a bank of strobe lights does not exceed 5 flashes per second, at which only 5% of photosensitive epileptics are at risk. It also recommends that no strobing effect continue for more than 30 seconds, due to the potential for discomfort and disorientation.
    
    //or 20 times, every one second... 
    _save_cnt_max = _strobe_cnt_max*_strobe_interval/DELTA_T_SAVE;
    
    create_dir();
}


void StrobeAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<StrobeAnalysis> save_callback(*this, &StrobeAnalysis::save_cb);

    // RUN ROUTINE
    for(int i = 0; i < NUM_RUN; i++) {

        _run_cnt = i;
        _save_cnt = 0;
        _frame_cnt = 0;

        //cout << "RUN NUM = " << i;

        save_timer = new Timer(0, DELTA_T_SAVE); // timing interval for saving files
        save_timer->start(save_callback);
        _RUN_DONE = false;
        _frame_cnt = 0; _save_cnt = 0;

        while(!_RUN_DONE)
            Thread::sleep(3);

        save_timer->stop();
    }
}

void StrobeAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
}

// this runs at frame rate = 33 ms for 30 FPS
void StrobeAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            
            if (_frame_cnt < _frame_cnt_max)
            {
                ofEnableAlphaBlending();
                ofColor aColour;
                int _fade_in_frames = _frame_cnt_max/10;
                cout<< "_fade_in_frames" << _fade_in_frames<< endl;
                
                if (_frame_cnt < _fade_in_frames) {
                    
                    aColour.set(255, 255, 255, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                    ofSetColor(aColour);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    //cout <<  "FADE IN STROBE TIME " << endl;
                    
                    
                }
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt < (_frame_cnt_max-_fade_in_frames)){
                    
                    //cout <<  "_frame_cnt: " << _frame_cnt << endl;
                    //cout <<  "frame_cnt % 15: " << _frame_cnt%15 << endl;
                    //cout <<  "MAIN STROBE TIME " << endl;
                    
                    if (_frame_cnt%int(ofGetFrameRate()*_strobe_interval/1000) < (ofGetFrameRate()*_strobe_interval/1000)/2)
                    {
                        ofSetColor(255, 255, 255);
                        ofRect(0, 0, ofGetWidth(), ofGetHeight());
                        _strobe_cnt++;
                        _strobe_on = 1;
                    } else if (_frame_cnt%int(ofGetFrameRate()*_strobe_interval/1000) >= (ofGetFrameRate()*_strobe_interval/1000)/2)
                    {
                        ofSetColor(0, 0, 0);
                        ofRect(0, 0, ofGetWidth(), ofGetHeight());
                        _strobe_on = 0;
                    }
                    
                }
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt < _frame_cnt_max) {
                    aColour.set(255, 255, 255, 255-ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
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
void StrobeAnalysis::save_cb(Timer& timer)
{

    _save_cnt++;
    
    cout << "StrobeAnalysis::saving...\n";
    
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(_strobe_on) +"_"+ofToString(_run_cnt,2)+".jpg";
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(_whole_file_path+"/"+file_name);


}
