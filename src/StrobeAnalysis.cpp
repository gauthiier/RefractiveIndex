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

#include "StrobeAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "RefractiveIndex.h"
using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define STATE_STROBE    0
#define STATE_ANALYSIS  1

void StrobeAnalysis::setup(int camWidth, int camHeight)
{
    create_dir();
}

void StrobeAnalysis::synthesize()
{
    Timer strobe_timer(0, 70);

    TimerCallback<StrobeAnalysis> strobe_callback(*this, &StrobeAnalysis::strobe_cb);

    _state = STATE_STROBE;
    _darkness = true;
    _strobe_cnt = 0;

    strobe_timer.start(strobe_callback);

    while(_state != STATE_ANALYSIS)
        Thread::sleep(5);

    strobe_timer.stop();

    // do analysis here
    
    
/*
    while(_state != STATE_STOP)
        Thread::sleep(100);
 */
}

void StrobeAnalysis::draw()
{
    if(_state == STATE_ANALYSIS) {
        ofSetColor(0, 200, 0);
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
        return;
    }

    if(_darkness) {
        ofSetColor(0, 0, 0);
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
    } else {
        ofSetColor(255, 255, 255);
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
    }

}


void StrobeAnalysis::strobe_cb(Timer& timer)
{
    cout << "IResponseAnalysis::saving...\n";
    _strobe_cnt++;

    _darkness = !_darkness;

    if(_strobe_cnt >= 20) {
        _state = STATE_ANALYSIS;
    }


}
