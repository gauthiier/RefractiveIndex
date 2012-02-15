/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "LatencyTestAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;


void LatencyTestAnalysis::setup(int camWidth, int camHeight)
{
    DELTA_T_SAVE = 100;
    NUM_PHASE = 1;
    NUM_RUN = 1;
    NUM_SAVE_PER_RUN = 100;    
    
    create_dir();
    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    c = 0;
}


void LatencyTestAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<LatencyTestAnalysis> save_callback(*this, &LatencyTestAnalysis::save_cb);

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

void LatencyTestAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
}


// this runs at frame rate = 33 ms for 30 FPS
void LatencyTestAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            /// *** TODO  *** ///
            // still need to deal with latency frames here - i.e.: there are frames
            /// *** TODO  *** ///
            
            if (_frame_cnt < _frame_cnt_max/3)
            {
                c  = 0;
                
                ofSetColor(c, c, c);
                cout<<"1st third"<<endl;
                ofRect(0, 0, ofGetWidth(), ofGetHeight());
            }
            if (_frame_cnt >= _frame_cnt_max/3 && _frame_cnt < 2*( _frame_cnt_max/3))
            {
                c  = 255;
                cout<<"2nd third"<<endl;
                
                ofSetColor(c, c, c);
                ofRect(0, 0, ofGetWidth(), ofGetHeight());
            }
            if (_frame_cnt >= 2*( _frame_cnt_max/3) && _frame_cnt < _frame_cnt_max)
            {
                c  = 0;
                cout<<"3rd third"<<endl;
                
                ofSetColor(c, c, c);
                ofRect(0, 0, ofGetWidth(), ofGetHeight());
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
void LatencyTestAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;

    // UPDATE THE COLOR ON THE SCREEN
    //float c_last = c;

    cout << "LatencyTestAnalysis::saving...\n";
    cout << "c_last... " << c << endl;
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    string thisLocation = RefractiveIndex::_location;

    //RefractiveIndex::_pixels = RefractiveIndex::_vidGrabber.getPixelsRef(); //get ofPixels from the camera
    //    fileName = imageSaveFolderPath+whichAnalysis+"_"+ofToString(100.0*i*scanLineSpeed/ofGetHeight(),2)+"%_"+ofToString(i)+".jpg";
    //ofSaveImage(vectorOfPixels[i], fileName, OF_IMAGE_QUALITY_BEST);
    
    string file = _whole_file_path+"/"+file_name;

    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(file);

    if(_save_cnt >= NUM_SAVE_PER_RUN)
        _RUN_DONE = true;

}
