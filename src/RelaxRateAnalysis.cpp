#include "RelaxRateAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define NUMBER_RUNS     1
#define ACQUIRE_TIME    20
#define TRESHOLD        80

void RelaxRateAnalysis::setup(int camWidth, int camHeight)
{
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis_NUM_RUN:NUM_RUN_relaxrate", NUMBER_RUNS);
    cout << "NUM_RUN RelaxRateAnalysis " << NUM_RUN << endl;
    //NUM_RUN = 5;
    
    int acq_run_time;   // 10 seconds of acquiring per run
    acq_run_time = RefractiveIndex::XML.getValue("config:analysis_time:acquiretime_relaxrate", ACQUIRE_TIME);
    cout << "ACQUIRE_TIME RelaxRateAnalysis " << acq_run_time << endl;
    
    _treshold = RefractiveIndex::XML.getValue("config:relaxrate:treshold", TRESHOLD);
    cout << "TRESHOLD RelaxRateAnalysis " << _treshold << endl;
    
    //int acq_run_time = 20;   // 20 seconds of acquiring per run
    
    DELTA_T_SAVE = 2*(10*acq_run_time/2); // for 20 seconds, we want this to be around 200 files
    // or 10 times per second = every 100 ms
    
    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames
    
    //create_dir_allocate_images();
    
    _run_cnt = 0;
    _level = 0;
    _flip = 1;
    _frame_cnt = 0;
    c = 0;
    _synth_save_cnt = 0;
    
    int anim_time = 5;   // 10 seconds
    _anim_cnt_max = anim_time*ofGetFrameRate();  // e.g.: 30 frames per second = 150 frames
    
    _show_image = false;
    _image_shown = false;
    
    image1.clear();
    image2.clear();
    image3.clear();  
    image4.clear();
    image5.clear();
    
    //  images use for drawing the synthesized files to the screen ///
    image1.setUseTexture(false);  // the non texture image that is needed to first load the image
    image2.setUseTexture(true);   // the image that needs to get written to the screen which takes the content of image1
    
    //  images used for re-loading and saving out the synthesized files ///
    image3.setUseTexture(false);  
    image4.setUseTexture(false);
    image5.setUseTexture(false);
    
    image1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h,  OF_IMAGE_COLOR);
    image2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h,  OF_IMAGE_COLOR);
    image3.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h,  OF_IMAGE_COLOR);  
    image4.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h,  OF_IMAGE_COLOR);
    image5.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h,  OF_IMAGE_COLOR);
    
    //cout << "RefractiveIndex::_vid_w " << RefractiveIndex::_vid_w << endl;
    //cout << "RefractiveIndex::_vid_h " << RefractiveIndex::_vid_h << endl;
    
    // clear() apparently fixes the "OF_WARNING: in allocate, reallocating a ofxCvImage" 
    // that we're getting in OSX/Windows and is maybe crashing Windows
    // http://forum.openframeworks.cc/index.php?topic=1867.0
    cvColorImage1.clear();
	cvGrayImage1.clear();
    cvGrayDiff1.clear();
    
    cvColorImage2.clear();
	cvGrayImage2.clear();
    cvGrayDiff2.clear();
    
    
    cvColorImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
    cvColorImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
}


void RelaxRateAnalysis::acquire()
{
    Timer* save_timer;
    TimerCallback<RelaxRateAnalysis> save_callback(*this, &RelaxRateAnalysis::save_cb);
    
    _run_cnt++;
    _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0, _synth_save_cnt = 0;
    _RUN_DONE = false;

    create_dir_allocate_images();
    
    // RUN ROUTINE
    //for(int i = 0; i < NUM_RUN; i++) {
    //_run_cnt = i;
    //cout << "RUN NUM = " << i;
    
    save_timer = new Timer(0, DELTA_T_SAVE); // timing interval for saving files
  
    save_timer->start(save_callback);
  
        while(!_RUN_DONE && _state != STATE_STOP)
            Thread::sleep(3);
    
    save_timer->stop();
        
}

void RelaxRateAnalysis::synthesise()
{
    //cout << "IResponseAnalysis::saving synthesis...\n";
    if(_state == STATE_STOP) return;
    
    cvContourFinderVect.clear();
    
    for(float i=1;i<_saved_filenames_analysis.size();i++){
                
        if(_state == STATE_STOP) return;
                
        if(image1.loadImage(_saved_filenames_analysis[i])){
                
            ///////////////////////// PROCESS THE SAVED CAMERA IMAGES OF SHIT TO THE IMAGES //////////////////////////
            
            cvColorImage1.setFromPixels(image1.getPixels(), image1.width, image1.height);            
            cvGrayDiff1 = cvColorImage1;
            cvGrayDiff1.threshold(_treshold);
            
            rfiCvContourFinder* cf = new rfiCvContourFinder();
            
            cf->findContours(cvGrayDiff1, 20, (image1.width * image1.height) / 4, 25, true);
            
            cvContourFinderVect.push_back(cf);

        }
    }
    
    // _saved_filenames_synthesis has processed all the files in the analysis images folder
    while(!_RUN_DONE && _state != STATE_STOP)
        Thread::sleep(3);
    
}

void RelaxRateAnalysis::displayresults()
{
        
    cvContourFinderVectDisplay.clear();
    
    for(int i=1;i<cvContourFinderVect.size();i++){
        
        if(_state == STATE_STOP) return;        
        
        
        //cout << "_saved_filenames_analysis[i] - " << _saved_filenames_synthesis[i] << endl;
        
        while(!_image_shown){
            Thread::sleep(2);
            if(_state == STATE_STOP) return;
            //cout << "!_image_shown" << endl;
        }
        
        cvContourFinderVectDisplay.push_back(cvContourFinderVect[i]);    
        _show_image = true;
        _image_shown = false;
        
    }     
    
}


// this runs at frame rate = 33 ms for 30 FPS
void RelaxRateAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
           
            if (_frame_cnt < _frame_cnt_max)
            {
                
                float lightLevel=pow(_level,2);
                
                ofSetColor(c, c, c);
                ofRect(0, 0, ofGetWidth(), ofGetHeight());
                
                c = ofMap(lightLevel, 0, pow(_frame_cnt_max/2,2), 0, 255);
                
                if (_frame_cnt <= (_frame_cnt_max/2)) {
                    _level+=1;
                } else {
                    _level-=1;
                }
                
            } else {
              
                _RUN_DONE = true;
            }
            
            _frame_cnt++;
            //cout << "_frame_cnt:" << _frame_cnt << endl;
            
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...
            
            //cout << "RelaxRateAnalysis = STATE_SYNTHESISING...\n";
            
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
                    //cout << "ShadowScapesAnalysis STATE_SYNTHESIZING = FADING IN ANIMATION...\n";
                    
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
                    
                    //cout << "ShadowScapesAnalysis STATE_SYNTHESIZING = MIDDLE OF ANIMATION...\n";
                    
                    for (int i=0; i <= 15; i++){
                        c_anim = 255;
                        aColour.set(c_anim, c_anim, c_anim, 255);
                        ofSetColor(aColour);
                        
                        ofRect(0, 0, rectSizeW+10*i, rectSizeH+10*i);
                        ofRect(0, 0, rectSizeW-10*i, rectSizeH-10*i);
                    }
                }
                
                //cout << "_anim_cnt = " << _anim_cnt << endl;
                
                if (_anim_cnt > (_anim_cnt_max-_fade_in_frames) && _anim_cnt <= _anim_cnt_max) {

                    //cout << "ShadowScapesAnalysis STATE_SYNTHESIZING = FADE OUT OF ANIMATION...\n";
                    
                    //cout << "_anim_cnt = " << _anim_cnt-(_anim_cnt_max-_fade_in_frames) << endl;
                    fade = ofMap(_anim_cnt-(_anim_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255);
                    //cout << "fade down = " << fade << endl;
                    
                    for (int i=0; i <= 15; i++){
                        
                        c_anim = (17*i);
                        //cout << "c_anim = " << c_anim << endl;
                        aColour.set(c_anim, c_anim, c_anim, 255-fade);
                        ofSetColor(aColour);
                        ofRect(0, 0, rectSizeW+10*i, rectSizeH+10*i);
                        ofRect(0, 0, rectSizeW-10*i, rectSizeH-10*i);
                    }
                    
                }
                
                _anim_cnt++;
                
            } else {
                _RUN_DONE = true;
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
                for(int i=0;i<cvContourFinderVectDisplay.size();i++){
                    cvContourFinderVectDisplay[i]->draw(0,0, ofGetWidth(), ofGetHeight());
                }

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
void RelaxRateAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    
    saveImageAnalysis(file_name);
    
    
}
