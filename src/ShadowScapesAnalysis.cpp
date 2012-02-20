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
    NUM_RUN = 1;
    
    int acq_run_time = 15;   // 10 seconds of acquiring per run
    
    int screenSpan;
    if (_dir == V) screenSpan = ofGetHeight();
    if (_dir == H) screenSpan = ofGetWidth();
    if (_dir == D) screenSpan = ofGetHeight();
    
    _step = (screenSpan/acq_run_time)/(ofGetFrameRate());
            // pixel per frame = (pixels / sec) / (frame / sec)
    
            
    // 40 pixels per second should give us a 20 second scan at 800 pixels wide

    DELTA_T_SAVE = 10*acq_run_time/2; // for 20 seconds, we want this to be around 200 files
    // or 10 times per second = every 100 ms
        
    create_dir();

    _scanLineWidth = 100.0;
    _run_cnt = 0;
    _save_cnt = 0;

    int anim_time = 5;   // 10 seconds
    _anim_cnt_max = anim_time*ofGetFrameRate();  // e.g.: 30 frames per second = 150 frames
}

void ShadowScapesAnalysis::acquire()
{

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
        
        _RUN_DONE = false;
    }

}

void ShadowScapesAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
    while(!_RUN_DONE)
        Thread::sleep(3);
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
                //_state = STATE_SYNTHESISING;
                _RUN_DONE = true;
                
            }
            
            if(_dir == H && int(_line) >= (ofGetWidth()+4*_scanLineWidth)) {
                
                //cout << "HORIZONTAL IS DONE -  _line >= (ofGetWidth()+4*_scanLineWidth)) is TRUE" << endl;
                //_state = STATE_SYNTHESISING;
                _RUN_DONE = true;
                
            }
            
            if(_dir == D && int(_line) >= (1.5*ofGetHeight()+4*_scanLineWidth)) {
                //cout << "DIAGONAL IS DONE - _line >= (1.5*ofGetHeight()+4*_scanLineWidth)) is TRUE" << endl;
                //_state = STATE_SYNTHESISING;
                _RUN_DONE = true;
            }
        
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            cout << "ShadowScapesAnalysis = STATE_SYNTHESISING...\n";

            // display animation of something while the synthesis in on-going...
            ofEnableAlphaBlending();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofPushMatrix();
            ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
            
            if(_anim_cnt < _anim_cnt_max){
                
                ofColor aColour;
                int rectSizeW = ofGetWidth()/4;
                int rectSizeH = ofGetHeight()/4;
                int _fade_in_frames = _anim_cnt_max/2;
                
                int c_anim = 10;
                int fade;
                
                //ofRotate(ofMap(_anim_cnt/2.0, 0, _anim_cnt_max, 0, 360));
                         
                if (_anim_cnt < _fade_in_frames) {
                    cout << "ShadowScapesAnalysis STATE_SYNTHESIZING = FADING IN ANIMATION...\n";
                
                    fade = ofMap(_anim_cnt, 0, _fade_in_frames, 0, 255);
                
                    for (int i=0; i <= 15; i++){
                        c_anim = 0+17*i;
                        
                        aColour.set(c_anim, c_anim, c_anim, fade);
                        ofSetColor(aColour);
                        
                        ofRect(0, 0, rectSizeW+10*i, rectSizeH+10*i);
                        ofRect(0, 0, rectSizeW-10*i, rectSizeH-10*i);
                    }
                }
            
                if (_anim_cnt >= _fade_in_frames && _anim_cnt <= (_anim_cnt_max-_fade_in_frames)){
                    
                    for (int i=0; i <= 15; i++){
                        c_anim = 255;
                        aColour.set(c_anim, c_anim, c_anim, 255);
                        ofSetColor(aColour);
                        
                        ofRect(0, 0, rectSizeW+10*i, rectSizeH+10*i);
                        ofRect(0, 0, rectSizeW-10*i, rectSizeH-10*i);
                    }
                }
                
                if (_anim_cnt > (_anim_cnt_max-_fade_in_frames) && _anim_cnt <= _anim_cnt_max) {
                    
                    cout << "_anim_cnt = " << _anim_cnt-(_anim_cnt_max-_fade_in_frames) << endl;
                    fade = ofMap(_anim_cnt-(_anim_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255);
                    cout << "fade down = " << fade << endl;
                   
                    for (int i=0; i <= 15; i++){
                        
                        c_anim = (17*i);
                        
                        aColour.set(c_anim, c_anim, c_anim, 255-fade);
                        ofSetColor(aColour);
                        ofRect(0, 0, rectSizeW+10*i, rectSizeH+10*i);
                        ofRect(0, 0, rectSizeW-10*i, rectSizeH-10*i);
                    }
        
                }
                _anim_cnt++;
                
            } else {
                
                _RUN_DONE = true;
                //_state = STATE_DISPLAY_RESULTS;
                _anim_cnt=0;
            }
            ofPopMatrix();
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
