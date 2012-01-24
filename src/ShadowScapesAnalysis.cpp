/*
 - copyright (c) 2011 Copenhagen Institute of Interaction Design (CIID)
 - all rights reserved.

 + redistribution and use in source and binary forms, with or without
 + modification, are permitted provided that the following conditions
 + are met:
 +  > redistributions of source code must retain the above copyright
 +    notice, this list of conditions and the following disclaimer.
 +  > redistributions in binary form must reproduce the above copyright
 +    notice, this list of conditions and the following disclaimer in
 +    the documentation and/or other materials provided with the
 +    distribution.

 + THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 + "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 + LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 + FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 + COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 + INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 + BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 + OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 + AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 + OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 + OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 + SUCH DAMAGE.

 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "ShadowScapesAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define STATE_SCAN      0
#define STATE_ANALYSIS  1

void ShadowScapesAnalysis::setup(int camWidth, int camHeight)
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
    _speed = 300;
}

void ShadowScapesAnalysis::synthesize()
{
    int w;
    if(_dir == H) w = ofGetWidth();
    else if(_dir == V) w = ofGetHeight();
    _step = ((w / _speed) * 1000) / 50;
    _line = 0;
    Timer scan_timer(0, 50);

    TimerCallback<ShadowScapesAnalysis> strobe_callback(*this, &ShadowScapesAnalysis::scan_cb);

    _state = STATE_SCAN;

    scan_timer.start(strobe_callback);

    while(_state != STATE_ANALYSIS)
        Thread::sleep(5);

    scan_timer.stop();
    // do analysis here
    // go back to the files i've saved and do the math here -

    while(_state != STATE_STOP)
        Thread::sleep(100);

}

void ShadowScapesAnalysis::gui_attach(ofxControlPanel* gui)
{
    gui->addToggle("GO", "GO", 0);
    gui->addButtonSlider("animation time limit", "ANIMATION_TIME_LIMIT", 10, 1, 3000, TRUE);

}

void ShadowScapesAnalysis::gui_detach()
{


}


// the animation draw - and the output draw
void ShadowScapesAnalysis::draw()
{

    static int _pos;

    if(_state == STATE_ANALYSIS) {
        ofSetColor(0, 200, 0);
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
        return;
    }

    if(_state == STATE_SCAN) {
        if(_pos != _line) {
            //take snap??
            _pos = _line;
        }
        ofSetColor(255, 255,  255);

        if(_dir == H) ofRect(_pos, 0, 50, ofGetHeight());
        else if(_dir == V) ofRect(0, _pos, ofGetWidth(), 50);

    }


}


void ShadowScapesAnalysis::scan_cb(Timer& timer)
{
    cout << "ShadowScapesAnalysis::scan_cb\n";

    _line += _step;

    if((_dir == H && _line >= ofGetWidth()) ||
       (_dir == V && _line >= ofGetHeight())) {
        _state = STATE_ANALYSIS;
    }


}
