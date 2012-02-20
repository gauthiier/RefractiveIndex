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
    
    
    DELTA_T_SAVE = 100;
    NUM_PHASE = 1;
    NUM_RUN = 1;
    NUM_SAVE_PER_RUN = 100;  
    
    create_dir();
    _speed = 100.0;  // 900.0 is the correct number
    _scanLineWidth = 100.0;
    _run_cnt = 0;
    _save_cnt = 0;

    int anim_time = 5;   // 5 seconds
    _anim_cnt_max = 5*ofGetFrameRate();  // e.g.: 30 frames per second = 150 frames
}

void ShadowScapesAnalysis::acquire()
{
    int screenSpan;
    if (_dir == V) screenSpan = ofGetHeight();
    if (_dir == H) screenSpan = ofGetWidth();
    if (_dir == D) screenSpan = ofGetHeight();
    
    _step = ((screenSpan/_speed) * 1000.0) / 500.0;
    _line = 0;

    
    // RUN ROUTINE
    for(int i = 0; i < NUM_RUN; i++) {
        
        Timer save_timer(0, DELTA_T_SAVE);  
        TimerCallback<ShadowScapesAnalysis> save_callback(*this, &ShadowScapesAnalysis::save_cb);
        
        _RUN_DONE = false;
        _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0;
        
        save_timer.start(save_callback);
        
        while(!_RUN_DONE)
            Thread::sleep(3);
        
        save_timer.stop();
    }

}

void ShadowScapesAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
}


// the animation draw - and the output draw
void ShadowScapesAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            _line += _step;
            
            //cout << "* _line:" << _line << endl;
            
            if(_dir == V) {
                
                ofEnableAlphaBlending();
                
                ofSetColor(255, 255, 255);
                ofRect(0, (_line-2*_scanLineWidth), ofGetWidth(), _scanLineWidth);            
                
                for (float i=0; i<25; i++)
                { 
                    ofSetColor(255, 255, 255, i*5.0);
                    ofRect(0, (_line-2*_scanLineWidth)+(2*_scanLineWidth/(i+1)), ofGetWidth(), _scanLineWidth);
                    ofRect(0, (_line-2*_scanLineWidth)-(2*_scanLineWidth/(i+1)), ofGetWidth(), _scanLineWidth);
                }        
                
                ofDisableAlphaBlending();
                
            }
            
            if(_dir == H) {
                
                ofEnableAlphaBlending();
                
                ofSetColor(255, 255, 255);
                ofRect( (_line-2*_scanLineWidth), 0, _scanLineWidth, ofGetHeight());
                
                for (float i=0; i<25; i++)
                { 
                    ofSetColor(255, 255, 255, i*5);
                    ofRect( (_line-2*_scanLineWidth)+(2*_scanLineWidth/(i+1)), 0, _scanLineWidth, ofGetHeight());
                    ofRect( (_line-2*_scanLineWidth)-(2*_scanLineWidth/(i+1)), 0, _scanLineWidth, ofGetHeight());
                }        
                ofDisableAlphaBlending();
                
            }
            
            if(_dir == D) {
                
                ofEnableAlphaBlending();
                
                ofPushMatrix();  
                
                ofTranslate(-ofGetWidth(), 0);
                ofRotate(-45);
                
                ofSetColor(255, 255, 255);
                ofRect(0, (_line-_scanLineWidth)+ofGetHeight()-_scanLineWidth, 2*ofGetWidth(), _scanLineWidth);  
                
                for (float i=0; i<25; i++)
                { 
                    ofSetColor(255, 255, 255, i*5);
                    ofRect(0, (_line-_scanLineWidth)+ofGetHeight()-_scanLineWidth+(2*_scanLineWidth/(i+1)), 2*ofGetWidth(), _scanLineWidth);
                    ofRect(0, (_line-_scanLineWidth)+ofGetHeight()-_scanLineWidth-(2*_scanLineWidth/(i+1)), 2*ofGetWidth(), _scanLineWidth);
                }        
                
                ofPopMatrix();  
                ofDisableAlphaBlending();
            }
            
            
            if(_dir == V && int(_line) >= (ofGetHeight()+4*_scanLineWidth)){
                //cout << "VERTICAL IS DONE - _line >= (ofGetHeight()+4*_scanLineWidth) is TRUE" << endl;
                _state = STATE_SYNTHESISING;
                
            }
            
            if(_dir == H && int(_line) >= (ofGetWidth()+4*_scanLineWidth)) {
                
                //cout << "HORIZONTAL IS DONE -  _line >= (ofGetWidth()+4*_scanLineWidth)) is TRUE" << endl;
                _state = STATE_SYNTHESISING;
                
            }
            
            if(_dir == D && int(_line) >= (1.5*ofGetHeight()+4*_scanLineWidth)) {
                //cout << "DIAGONAL IS DONE - _line >= (1.5*ofGetHeight()+4*_scanLineWidth)) is TRUE" << endl;
                _state = STATE_SYNTHESISING;
            }
        
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            cout << "ShadowScapesAnalysis = STATE_SYNTHESISING...\n";

            // display animation of something while the synthesis in on-going...
            ofEnableAlphaBlending();
            ofSetRectMode(OF_RECTMODE_CENTER);
            
            if(_anim_cnt < _anim_cnt_max){
                
                ofColor aColour;
                int rectSizeW = ofGetWidth()/4;
                int rectSizeH = ofGetWidth()/4;
                int _fade_in_frames = _anim_cnt_max/10;
                
                int c_anim = 10;
                int fade;
                if (_anim_cnt < _fade_in_frames) {
                    cout << "ShadowScapesAnalysis STATE_SYNTHESIZING = FADING IN ANIMATION...\n";
                    
                    fade = ofMap(_anim_cnt, 0, _fade_in_frames, 0, 255);
                    cout << "fade up = " << fade << endl;
                    for (int i=0; i < ofGetHeight() ; i=i+rectSizeH)
                    {
                        for (int j=0; j < ofGetWidth(); j=j+rectSizeW)
                        {
                            c_anim = ofRandom(150,255);

                            aColour.set(c_anim, c_anim, c_anim, fade);
                            ofSetColor(aColour);
                            ofRect(ofGetWidth()/2, ofGetHeight()/2, rectSizeW, rectSizeH);
                        }
                    }        
                }
            
                if (_anim_cnt >= _fade_in_frames && _anim_cnt <= (_anim_cnt_max-_fade_in_frames)){
                    
                    for (int i=0; i < ofGetHeight() ; i=i+rectSizeH)
                    {
                        for (int j=0; j < ofGetWidth(); j=j+rectSizeW)
                        { 
                            c_anim = ofRandom(150,255);

                            //c = ofRandom(0,255);
                            aColour.set(c_anim, c_anim, c_anim, 255);
                            ofSetColor(aColour);
                            ofRect(ofGetWidth()/2, ofGetHeight()/2, rectSizeW, rectSizeH);
                            
                        }
                    }        
                }
                
                if (_anim_cnt > (_anim_cnt_max-_fade_in_frames) && _frame_cnt <= _anim_cnt_max) {
                    
                    cout << "_anim_cnt = " << _anim_cnt-(_anim_cnt_max-_fade_in_frames) << endl;
                    fade = ofMap(_anim_cnt-(_anim_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255);
                    cout << "fade down = " << fade << endl;
                   
                    for (int i=0; i < ofGetHeight() ; i=i+rectSizeH)
                    {
                        for (int j=0; j < ofGetWidth(); j=j+rectSizeW)
                        {
                            c_anim = ofRandom(150,255);
                            //c = ofRandom(0,255);
                            
                            aColour.set(c_anim, c_anim, c_anim, 255-fade);
                            ofSetColor(aColour);
                            ofRect(ofGetWidth()/2, ofGetHeight()/2, rectSizeW, rectSizeH);
                        } 
                    }
        
                }
                _anim_cnt++;
                
            } else {
                _state = STATE_DISPLAY_RESULTS;
                _anim_cnt=0;
            }
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofDisableAlphaBlending();
            break;
      
        }
            
        case STATE_DISPLAY_RESULTS:
        {
            // display results of the synthesis
            cout << "ShadowScapesAnalysis = STATE_DISPLAY_RESULTS...\n";
            _RUN_DONE = true;
            break;
        }
            
        default:
            break;
    }        

}


void ShadowScapesAnalysis::save_cb(Timer& timer)
{
    
    _save_cnt++;
    
    cout << "ShadowScapesAnalysis::saving...\n";
    
    string file_name;
    
    if(_dir == H) {
        file_name = ofToString(_save_cnt, 2)+"_H_"+ofToString(_line, 2)+"_"+ofToString(_run_cnt,2)+".jpg";
    }
    
    if(_dir == V) {
        file_name = ofToString(_save_cnt, 2)+"_V_"+ofToString(_line, 2)+"_"+ofToString(_run_cnt,2)+".jpg";
    }
    
    if(_dir == D) {
        file_name = ofToString(_save_cnt, 2)+"_D_"+ofToString(_line, 2)+"_"+ofToString(_run_cnt,2)+".jpg";
    }
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(_whole_file_path+"/"+file_name);
    
}
