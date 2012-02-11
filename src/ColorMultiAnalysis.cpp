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

#include "ColorMultiAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;



void ColorMultiAnalysis::setup(int camWidth, int camHeight)
{
    create_dir();
    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    c = 0;
}

void ColorMultiAnalysis::synthesize()
{

    Timer* save_timer;

    TimerCallback<ColorMultiAnalysis> save_callback(*this, &ColorMultiAnalysis::save_cb);

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

void ColorMultiAnalysis::draw()
{


        if (_frame_cnt < _frame_cnt_max)
        {
            ofColor aColor;
            aColor.setHsb(c, 255, 255);
            ofSetColor(aColor);
            ofRect(0, 0, ofGetWidth(), ofGetHeight());
            //how far are we as a percent of _frame_count_max
            c  = 255.0 * (_frame_cnt_max - _frame_cnt)/(_frame_cnt_max);
        }
        _frame_cnt++;


}


// this runs at save_cb timer rate = DELTA_T_SAVE
void ColorMultiAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;

    // UPDATE THE COLOR ON THE SCREEN
    //float c_last = c;

    cout << "COLORMULTIANALYSIS::saving...\n";
    cout << "c_last... " << c << endl;
    string file_name = ofToString(_save_cnt,2)+"_"+ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";

    cout<<_whole_file_path<<endl;
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);

    if(_save_cnt >= NUM_SAVE_PER_RUN){
        _RUN_DONE = true;
    }

}
