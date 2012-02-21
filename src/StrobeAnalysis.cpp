#include "StrobeAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

void StrobeAnalysis::setup(int camWidth, int camHeight)
{
    NUM_RUN = 1;
    
    int acq_run_time = 20;   // 20 seconds of acquiring per run
    
    DELTA_T_SAVE = 2*(10*acq_run_time/2); // for 20 seconds, we want this to be around 200 files
                                          // or 10 times per second = every 100 ms
   
    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames
    
    _strobe_interval = 1500;  //every 1 seconds, or every thirty frames 30 frames

    // The British Health and Safety Executive recommend that a net flash rate for a bank of strobe lights does not exceed 5 flashes per second, at which only 5% of photosensitive epileptics are at risk. It also recommends that no strobing effect continue for more than 30 seconds, due to the potential for discomfort and disorientation.
    
    create_dir();
    
    int anim_time = 5;   // 5 seconds for the animation
    _anim_cnt_max = anim_time*ofGetFrameRate();  // e.g.: 30 frames per second = 150 frames
    
    _show_image = false;
    _image_shown = false;
    
    //for an ofxOpenCv.h image i would like to use..."
    //image3.allocate(RefractiveIndex::_vid_w, RefractiveIndex::_vid_h);
    
    image1.setUseTexture(false);
    image2.setUseTexture(true);

}


void StrobeAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<StrobeAnalysis> save_callback(*this, &StrobeAnalysis::save_cb);

    // RUN ROUTINE
    for(int i = 0; i < NUM_RUN; i++) {

        _run_cnt = i;
        _save_cnt = 0;
        _frame_cnt = 0;

        //cout << "RUN NUM = " << i;

        save_timer = new Timer(0, DELTA_T_SAVE); // timing interval for saving files
        save_timer->start(save_callback);
        _RUN_DONE = false;
        
        _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0;  _strobe_cnt = 0;

        while(!_RUN_DONE)
            Thread::sleep(3);

        save_timer->stop();
        
        _RUN_DONE = false;
    }
}

void StrobeAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
    while(!_RUN_DONE)
        Thread::sleep(3);
}

void StrobeAnalysis::displayresults()
{
    
    for(float i=1;i<_saved_filenames.size();i++){
        
        cout << "_saved_filenames[i]" << _saved_filenames[i] << endl;
        
        while(!_image_shown){
            Thread::sleep(2);
            //cout << "!_image_shown" << endl;
        }
        
        
        if(!image1.loadImage(_saved_filenames[i])){
            //couldn't load image
            cout << "didn't load image" << endl;
        } 
        
        
        if(image1.loadImage(_saved_filenames[i])){
            image1.loadImage(_saved_filenames[i]);
            //cout << "_show_image = true;" << endl;
            _show_image = true;
            _image_shown = false;
        }
    }
}


// this runs at frame rate = 33 ms for 30 FPS
void StrobeAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            
            if (_frame_cnt < _frame_cnt_max)
            {
                ofEnableAlphaBlending();
                ofColor aColour;
                int _fade_in_frames = _frame_cnt_max/10;
                cout<< "_fade_in_frames" << _fade_in_frames<< endl;
                
                if (_frame_cnt < _fade_in_frames) {
                    
                    aColour.set(255, 255, 255, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                    ofSetColor(aColour);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    //cout <<  "FADE IN STROBE TIME " << endl;
                    
                    
                }
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt < (_frame_cnt_max-_fade_in_frames)){
                    
                    //cout <<  "_frame_cnt: " << _frame_cnt << endl;
                    //cout <<  "frame_cnt % 15: " << _frame_cnt%15 << endl;
                    //cout <<  "MAIN STROBE TIME " << endl;
                    
                    if (int(_frame_cnt)%int(ofGetFrameRate()*_strobe_interval/1000) < (ofGetFrameRate()*_strobe_interval/1000)/2)
                    {
                        ofSetColor(255, 255, 255);
                        ofRect(0, 0, ofGetWidth(), ofGetHeight());
                        _strobe_cnt++;
                        _strobe_on = 1;
                    } else if (int(_frame_cnt)%int(ofGetFrameRate()*_strobe_interval/1000) >= (ofGetFrameRate()*_strobe_interval/1000)/2)
                    {
                        ofSetColor(0, 0, 0);
                        ofRect(0, 0, ofGetWidth(), ofGetHeight());
                        _strobe_on = 0;
                    }
                    
                }
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt < _frame_cnt_max) {
                    aColour.set(255, 255, 255, 255-ofMap(_frame_cnt-(_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255));
                    ofSetColor(aColour);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    // cout <<  "FADE OUT STROBE TIME " << endl;
                }         
            
                ofDisableAlphaBlending();
            } else {
                //_state = STATE_SYNTHESISING;
                _RUN_DONE = true;
            }
            
            _frame_cnt++;
            
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
            
            if (_frame_cnt > 2)
            {
                _image_shown = true;
                _frame_cnt=0;
            }
            
            _frame_cnt++;
            
            if (_show_image)
            {  
                ofEnableAlphaBlending();
                
                ofSetColor(255, 255, 255, 255);
                image2.setFromPixels(image1.getPixels(),image1.width,image1.height, OF_IMAGE_COLOR);
                image2.draw(0,0, ofGetWidth(), ofGetHeight());
                
                ofDisableAlphaBlending();
            }
            
            // display results of the synthesis
            _RUN_DONE = true;
            break;

        }
            
            
        default:
            break;
    }

}

// this runs at save_cb timer rate = DELTA_T_SAVE
void StrobeAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    RefractiveIndex::_vidGrabber.grabFrame();  // get a new frame from the camera
    
    if (RefractiveIndex::_vidGrabber.isFrameNew())
    {
        RefractiveIndex::_pixels = RefractiveIndex::_vidGrabber.getPixelsRef(); //get ofPixels from the camera
    } else {
        return;
    }
    
    //cout << "StrobeAnalysis::saving...\n";
    
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(_strobe_on) +"_"+ofToString(_run_cnt,2)+".jpg";
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(_whole_file_path+"/"+file_name);


}
