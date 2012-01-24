/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#include "CamFrameRateAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;


void CamFrameRateAnalysis::setup(int camWidth, int camHeight)
{    
    // HERE IS WHERE WE SETUP THE DIRECTORY FOR ALL THE SAVED IMAGES
    time_t rawtime;
    struct tm * timeinfo;
    
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    string time = asctime(timeinfo);
    
    cout<<"time"<<time<<endl;
    
    ofDirectory dir;
    _whole_file_path= string(ANALYSIS_PATH)+RefractiveIndex::_location+"/"+ _name+"/"+time;
    
    //directories have to be created one level at a time hence repeated calls

    
    if(!dir.doesDirectoryExist(_whole_file_path)){
        
        dir.createDirectory(string(ANALYSIS_PATH)+RefractiveIndex::_location+"/", true,false); 
        dir.createDirectory(string(ANALYSIS_PATH)+RefractiveIndex::_location+"/"+ _name+"/", true,false); 
        dir.createDirectory(_whole_file_path, true,false);
        
    }
    
    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
     c = 0;
}


void CamFrameRateAnalysis::synthesize()
{
    
    Timer* save_timer;
    
    TimerCallback<CamFrameRateAnalysis> save_callback(*this, &CamFrameRateAnalysis::save_cb);
    
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

void CamFrameRateAnalysis::gui_attach(ofxControlPanel* gui)
{
    
}

void CamFrameRateAnalysis::gui_detach()
{
    
}

//void CamFrameRateAnalysis::draw(ofPixels _pixels)   //trying to figure out how to get pixels from the RefractiveIndex.cpp 


// this runs at frame rate = 33 ms for 30 FPS
void CamFrameRateAnalysis::draw()
{
    /// *** TODO  *** ///
    // still need to deal with latency frames here - i.e.: there are frames 
    /// *** TODO  *** ///
    float totalTime=_frame_cnt_max/2;
    
    float numSteps=10;
    
    vector<float>stepLengths;
    
    
    //c must increase until frame_cnt_max * 0.5 and then decrease afterwards
    
    
    if (_frame_cnt < _frame_cnt_max)
    {
        ofSetColor(c, c, c);
        ofRect(0, 0, ofGetWidth(), ofGetHeight()); 
        c  = 255.0 * (_frame_cnt_max - _frame_cnt)/(_frame_cnt_max);
        cout<<_frame_cnt<<endl;
    }
    _frame_cnt++;
    
    
}

// this runs at save_cb timer rate = DELTA_T_SAVE
void CamFrameRateAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    //cout << "c_last... " << c << endl;    
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    string thisLocation = RefractiveIndex::_location;
    
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);    
    
    if(_save_cnt >= NUM_SAVE_PER_RUN)
        _RUN_DONE = true;
    
}