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
    
    
    NUM_RUN = 1;
    
    int acq_run_time = 35;  
    
    DELTA_T_SAVE = 10*acq_run_time/2; // for 20 seconds, we want this to be around 200 files
    // or 10 times per second = every 100 ms
    
    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames
    
    create_dir();
    
    _frame_cnt = 0;
    c = 0;
    
    int anim_time = 5;   // 10 seconds
    _anim_cnt_max = anim_time*ofGetFrameRate();  // e.g.: 30 frames per second = 150 frames
    
    create_dir();

}

void ColorMultiAnalysis::acquire()
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
     
         _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0;

        while(!_RUN_DONE)
            Thread::sleep(3);

        save_timer->stop();
        _RUN_DONE = false;
        
    }
}

void ColorMultiAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
}


void ColorMultiAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            ofEnableAlphaBlending();
            
            if (_frame_cnt < _frame_cnt_max)
            {

                int _fade_in_frames = _frame_cnt_max/50;
                ofColor aColor;
                
                if (_frame_cnt < _fade_in_frames) {
                   
                    
                    aColor.setHsb(c, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255), ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                    
                    ofSetColor(aColor);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    
                    cout << "FADING IN..." << endl;
                }
                
                
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt < _frame_cnt_max-_fade_in_frames){
                    
                    aColor.setHsb(c, 255, 255);
                    ofSetColor(aColor);
                    
                    //how far are we as a percent of _frame_count_max * 360 HUE VALUES
                    c  = 255.0 * (_frame_cnt_max - _frame_cnt)/(_frame_cnt_max);
                    
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                }
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt <= _frame_cnt_max) {
                    
                    aColor.set(c, c, c, 255-int(ofMap(_frame_cnt-(_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255)));
                    
                    //aColor.setHsb(c, 255-ofMap(_fade_cnt- (_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255), 255-(ofMap(_fade_cnt-(_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255)));
                    
                    ofSetColor(aColor);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    cout << "FADING OUT..." << endl;
                    
                }
                
                
            } else {
                //_state = STATE_SYNTHESISING;
                _RUN_DONE = true;
            }
            
            _frame_cnt++;            
            ofDisableAlphaBlending();
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...
            
            cout << "RelaxRateAnalysis = STATE_SYNTHESISING...\n";
            
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
                _state = STATE_DISPLAY_RESULTS;
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
            _RUN_DONE = true;
            break;
        }
            
            
        default:
            break;
    }
    
}


// this runs at save_cb timer rate = DELTA_T_SAVE
void ColorMultiAnalysis::save_cb(Timer& timer)
{
  
    _save_cnt++;
    
    RefractiveIndex::_vidGrabber.grabFrame();  // get a new frame from the camera
    
    if (RefractiveIndex::_vidGrabber.isFrameNew())
    {
        RefractiveIndex::_pixels = RefractiveIndex::_vidGrabber.getPixelsRef(); //get ofPixels from the camera
    }
    
    cout << "ColorMultiAnalysis::saving...\n";
    
    string file_name = ofToString(_save_cnt,2)+"_"+ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(_whole_file_path+"/"+file_name);


}
