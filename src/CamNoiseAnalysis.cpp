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
      // HERE IS WHERE WE SETUP THE DIRECTORY FOR ALL THE SAVED IMAGES

    //FOR WINDOWS i HAVE HAD TO REPLACE SPACES WITH UNDERSCORES AND REDUCE THE LENGTH OF THE FOLDER NAME
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    string time = asctime(timeinfo);
    string replaceTime = "";

    //DON'T INCLUDE THE DAY IN WORDS EG 'TUE' OR THE YEAR EG 2012 THIS MAKES THE DIRECTORY NAME TOO LONG AND CAUSES DIR CREATION TO FAIL
    for(int i=4;i<time.size()-4;i++){
        if(time.at(i)==' '||time.at(i)==':'){
            replaceTime+="_";
        }
        else{
            replaceTime+=time.at(i);
        }
    }

    ofDirectory dir;

    _whole_file_path= string(ANALYSIS_PATH)+RefractiveIndex::_location+"/"+ _name+"/"+replaceTime ;
    //directories have to be created one level at a time hence repeated calls
    if(!dir.doesDirectoryExist(_whole_file_path)){
        dir.createDirectory(string(ANALYSIS_PATH)+RefractiveIndex::_location+"/", true,false);
        dir.createDirectory(string(ANALYSIS_PATH)+RefractiveIndex::_location+"/"+ _name+"/", true,false);
        dir.createDirectory(string(ANALYSIS_PATH)+RefractiveIndex::_location+"/"+ _name+"/"+replaceTime+"/", true,false);
    }

    //////////////////////////////END DIRECTORY CREATION //////////////////////////////////////////////////

    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    c = 0;
}


void CamNoiseAnalysis::synthesize()
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

void CamNoiseAnalysis::gui_attach(ofxControlPanel* gui)
{

}

void CamNoiseAnalysis::gui_detach()
{

}

//void CamNoiseAnalysis::draw(ofPixels _pixels)   //trying to figure out how to get pixels from the RefractiveIndex.cpp


// this runs at frame rate = 33 ms for 30 FPS
void CamNoiseAnalysis::draw()
{
    /// *** TODO  *** ///
    // still need to deal with latency frames here - i.e.: there are frames
    /// *** TODO  *** ///

    float _number_of_grey_levels=5;

    float _frames_per_level = _frame_cnt_max / _number_of_grey_levels;
    ofColor someColor;

    for(int i=0;i<=_number_of_grey_levels;i++){
        if (_frame_cnt>= _frames_per_level *( i-1) && +_frame_cnt < _frames_per_level * (i) ) {
            //set colour to current grey level
            c=255-( 255.0 * ( i /_number_of_grey_levels));
            someColor.set(c);

        }
        ofSetColor(someColor);
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
    }



    _frame_cnt++;


}

// this runs at save_cb timer rate = DELTA_T_SAVE
void CamNoiseAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;

    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    string thisLocation = RefractiveIndex::_location;


    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);

    if(_save_cnt >= NUM_SAVE_PER_RUN)
        _RUN_DONE = true;

}
