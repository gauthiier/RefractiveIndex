/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "RelaxRateAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;


void RelaxRateAnalysis::setup(int camWidth, int camHeight)
{
    DELTA_T_SAVE = 300;
    NUM_PHASE = 1;
    NUM_RUN = 3;
    NUM_SAVE_PER_RUN = 100;    
    
    create_dir();
    
    _level = 0;
    _flip = 1;
    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    c = 0;
}


void RelaxRateAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<RelaxRateAnalysis> save_callback(*this, &RelaxRateAnalysis::save_cb);

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

void RelaxRateAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
}

// this runs at frame rate = 33 ms for 30 FPS
void RelaxRateAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            /// *** TODO  *** ///
            // still need to deal with latency frames here - i.e.: there are frames
            /// *** TODO  *** ///
           
            if (_frame_cnt < _frame_cnt_max)
            {
                
                float lightLevel=pow(_level,2);
                
                ofSetColor(c, c, c);
                ofRect(0, 0, ofGetWidth(), ofGetHeight());
                
                c = ofMap(lightLevel, 0, pow(_frame_cnt_max/2,2), 0, 255);
                
                if (_frame_cnt <= (_frame_cnt_max/2)) {
                    _level+=1;
                } else {
                    _level-=1;
                }
                
            } else {
                cout << "RELAXRATE RUN COMPLETED" << endl;
                //_state = STATE_SYNTHESISING;
                _RUN_DONE = true;
            }
            
            _frame_cnt++;
            cout << "_frame_cnt:" << _frame_cnt << endl;
            
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
void RelaxRateAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    cout << "RelaxRateAnalysis::saving...\n";
    
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(_whole_file_path+"/"+file_name);
}
