#include "ColorSingleAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define NUMBER_RUNS     1

void ColorSingleAnalysis::setup(int camWidth, int camHeight)
{
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis:NUM_RUN", NUMBER_RUNS);
    cout << "NUM_RUN ColorSingleAnalysis " << NUM_RUN << endl;
    
    //NUM_RUN = 5;
    
    int acq_run_time = 25;   // 20 seconds of acquiring per run
    
    DELTA_T_SAVE = 1*(10*acq_run_time/2); // for 20 seconds, we want this to be around 200 files
                                            // or 10 times per second = every 100 ms
    
    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames
    
    //create_dir();
    
    _run_cnt = 0;
    _frame_cnt = 0;
    _synth_save_cnt = 0;
    c = 0;
    
    int anim_time = 10;   // 10 seconds
    _anim_cnt_max = anim_time*ofGetFrameRate();  // e.g.: 30 frames per second = 150 frames
    
    r = 0;
    g = 0;
    b = 0;
    
    fileNameTag = "";

    _show_image = false;
    _image_shown = false;
    
    //  images use for drawing the synthesized files to the screen ///
    image1.setUseTexture(false);  // the non texture image that is needed to first load the image
    image2.setUseTexture(true);   // the image that needs to get written to the screen which takes the content of image1
    
    //  images used for re-loading and saving out the synthesized files ///
    image3.setUseTexture(false);  
    image4.setUseTexture(false);
    image5.setUseTexture(false);
    
    //cout << "RefractiveIndex::_vid_w " << RefractiveIndex::_vid_w << endl;
    //cout << "RefractiveIndex::_vid_h " << RefractiveIndex::_vid_h << endl;
    
    cvColorImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
    cvColorImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
}


void ColorSingleAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<ColorSingleAnalysis> save_callback(*this, &ColorSingleAnalysis::save_cb);
    
    _run_cnt++;
    _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0, _synth_save_cnt = 0;
    _RUN_DONE = false;
    create_dir();
    
    // RUN ROUTINE
    //for(int i = 0; i < NUM_RUN; i++) {
      //  _run_cnt = i;
        //cout << "RUN NUM = " << i;

    save_timer = new Timer(0, DELTA_T_SAVE); // timing interval for saving files
    save_timer->start(save_callback);
        
        while(!_RUN_DONE && _state != STATE_STOP)
            Thread::sleep(3);

    save_timer->stop();

  
    // }
}

void ColorSingleAnalysis::synthesise()
{
    
    //cout << "ColorSingleAnalysis::saving synthesis...\n";
    if(_state == STATE_STOP) return;
    
    for(float i=1;i<_saved_filenames_analysis.size()-1;i++){
        
        //cout << "ColorSingleAnalysis::synthesis FOR LOOP...\n";
        
        //cout << "_saved_filenames_analysis[i]" << _saved_filenames_analysis[i] << endl;
        
        if(_state == STATE_STOP) return;
        
        if(!image1.loadImage(_saved_filenames_analysis[i])){
            //couldn't load image
            cout << "didn't load image" << endl;
        } 
        
        if(image1.loadImage(_saved_filenames_analysis[i])){
            //cout << "LOADED image1!!!" << endl;
            //if(image5.loadImage(_saved_filenames_analysis[i+1])){
                
                ///////////////////////// PROCESS THE SAVED CAMERA IMAGES OF SHIT TO THE IMAGES //////////////////////////
                
                cvColorImage1.setFromPixels(image1.getPixels(), image1.width, image1.height);
                //cvColorImage2.setFromPixels(image5.getPixels(), image5.width, image5.height);
                
                //cvGrayImage1 = cvColorImage1;
                //cvGrayImage2 = cvColorImage2;
                
                //cvGrayDiff1.absDiff(cvGrayImage2, cvGrayImage1);
                //cvGrayDiff1.erode();
                //cvGrayDiff1.contrastStretch();
                //cvGrayDiff1.blur(5);
                //cvGrayDiff1.dilate();
                //cvGrayDiff1.contrastStretch();
            
                cvColorImage1.contrastStretch();
                cvColorImage1.blur(5);
                cvColorImage1.erode();
                cvColorImage1.erode();
                cvColorImage1.dilate();
                cvColorImage1.blur(5);
                cvColorImage1.erode();
                cvColorImage1.erode();
                cvColorImage1.erode();
                cvColorImage1.erode();
            
                /////////////////////////////////// SAVE TO DISK IN THE SYNTHESIS FOLDER ////////////////////////////////
                string file_name;
                
                file_name = ofToString(_synth_save_cnt, 2)+"_ColorSingleAnalysis_"+ofToString(_run_cnt,2)+".jpg";
                
                //image4.setFromPixels(cvColorImage1.getPixelsRef(),image3.width, image3.height, OF_IMAGE_COLOR);
                
                ofSaveImage(cvColorImage1.getPixelsRef(),_whole_file_path_synthesis+"/"+file_name, OF_IMAGE_QUALITY_BEST);
                _saved_filenames_synthesis.push_back(_whole_file_path_synthesis+"/"+file_name);
                _synth_save_cnt++;
                
           // }
        }
    }
    
    // _saved_filenames_synthesis has processed all the files in the analysis images folder
    while(!_RUN_DONE && _state != STATE_STOP)
        Thread::sleep(3);
    
}

void ColorSingleAnalysis::displayresults()
{
    for(float i=1;i<_saved_filenames_synthesis.size();i++){
        
        if(_state == STATE_STOP) return;
        
        //cout << "_saved_filenames_analysis[i] - " << _saved_filenames_synthesis[i] << endl;
        
        while(!_image_shown){
            Thread::sleep(2);
            //cout << "!_image_shown" << endl;
        }
        
        if(!image3.loadImage(_saved_filenames_synthesis[i])){
            //couldn't load image
            cout << "didn't load image" << endl;
        } 
        
        if(image3.loadImage(_saved_filenames_synthesis[i])){
            image3.loadImage(_saved_filenames_synthesis[i]);
            //cout << "_show_image = true;" << endl;
            _show_image = true;
            _image_shown = false;
        }
    }
    
}


void ColorSingleAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            
            
            if (_frame_cnt < _frame_cnt_max)
            {
                    
                float one_third_of_frame_count_max=_frame_cnt_max/3;
                int _fade_in_frames = one_third_of_frame_count_max/10;
                
                if (_frame_cnt < _fade_in_frames){
                    ofSetColor(ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255), 0, 0);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    fileNameTag = "FADING";
                }
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt < one_third_of_frame_count_max){
                    r=255.0;
                    g=0.0;
                    b=0.0;
                    ofSetColor(r,g,b);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    fileNameTag = "RED";
                }
                
                if (_frame_cnt >= one_third_of_frame_count_max && _frame_cnt < 2*one_third_of_frame_count_max){
                    r=0.0;
                    g=255.0;
                    b=0.0;
                    ofSetColor(r,g,b);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    fileNameTag = "GREEN";
                }
                
                if (_frame_cnt >= 2*one_third_of_frame_count_max && _frame_cnt < (_frame_cnt_max-_fade_in_frames) ){
                    r=0.0;
                    g=0.0;
                    b=255.0;
                    ofSetColor(r,g,b);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    fileNameTag = "BLUE";
                }
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt < _frame_cnt_max){
                   
                    int fade = ofMap(_fade_cnt, 0, _fade_in_frames, 0, 255);
                    ofSetColor(0, 0, 255-fade);
        
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    _fade_cnt++;
                    fileNameTag = "FADING";
                }
            
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
            
            //cout << "ColorSingleAnalysis = STATE_SYNTHESISING...\n";
            
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
                   // cout << "ColorSingleAnalysis STATE_SYNTHESIZING = FADING IN ANIMATION...\n";
                    
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
                    
                    //cout << "_anim_cnt = " << _anim_cnt-(_anim_cnt_max-_fade_in_frames) << endl;
                    fade = ofMap(_anim_cnt-(_anim_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255);
                    //cout << "fade down = " << fade << endl;
                    
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
            
            //cout << "STATE_DISPLAY_RESULTS...\n" << endl;
            
            
            if (_frame_cnt > 2)
            {
                _image_shown = true;
                _frame_cnt=0;
            }
            
            _frame_cnt++;
            
            if (_show_image)
            {  
                //cout << "_show_image...\n" << endl;
                
                ofEnableAlphaBlending();
                
                ofSetColor(255, 255, 255);
                image2.setFromPixels(image3.getPixels(),image3.width,image3.height, OF_IMAGE_COLOR);
                //image2.setFromPixels(image3.getPixels(),image3.width,image3.height, OF_IMAGE_GRAYSCALE);
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
void ColorSingleAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    RefractiveIndex::_vidGrabber.grabFrame();  // get a new frame from the camera
    
    if (RefractiveIndex::_vidGrabber.isFrameNew())
    {
        RefractiveIndex::_pixels = RefractiveIndex::_vidGrabber.getPixelsRef(); //get ofPixels from the camera
    } else {
        return;
    }
    
    //cout << "ColorSingleAnalysis::saving...\n";
    
    string file_name =ofToString(_save_cnt,2)+"_"+fileNameTag+"_"+ofToString(_run_cnt,2)+".jpg";
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path_analysis+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames_analysis.push_back(_whole_file_path_analysis+"/"+file_name);


}
