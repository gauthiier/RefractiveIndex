#include "ShapeFromShadingAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define NUMBER_RUNS     1
#define ACQUIRE_TIME    20

void ShapeFromShadingAnalysis::setup(int camWidth, int camHeight)
{       
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis_NUM_RUN:NUM_RUN_shapefromshading", NUMBER_RUNS);
    cout << "NUM_RUN ShapeFromShadingAnalysis " << NUM_RUN << endl;
    //NUM_RUN = 5;
    
    int acq_run_time;   // 10 seconds of acquiring per run
    acq_run_time = RefractiveIndex::XML.getValue("config:analysis_time:acquiretime_shapefromshading", ACQUIRE_TIME);
    cout << "ACQUIRE_TIME ShapeFromShadingAnalysis " << acq_run_time << endl;


    //int acq_run_time = 20;   // 20 seconds of acquiring per run
    
    DELTA_T_SAVE = 2*(10*acq_run_time/2); // for 20 seconds, we want this to be around 200 files
    // or 10 times per second = every 100 ms
    
    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames
    
    //create_dir_allocate_images();
    _synth_save_cnt = 0;
    _run_cnt = 0;
    _frame_cnt = 0;
    c = 0;
    
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
    
   // cout << "RefractiveIndex::_vid_w " << RefractiveIndex::_vid_w << endl;
   // cout << "RefractiveIndex::_vid_h " << RefractiveIndex::_vid_h << endl;
    
    // clear() apparently fixes the "OF_WARNING: in allocate, reallocating a ofxCvImage" 
    // that we're getting in OSX/Windows and is maybe crashing Windows
    // http://forum.openframeworks.cc/index.php?topic=1867.0
    cvColorImage1.clear();
	cvGrayImage1.clear();
    cvGrayDiff1.clear();
    
    cvColorImage2.clear();
	cvGrayImage2.clear();
    cvGrayDiff2.clear();
    
    cvConvertorImage.clear();    
    
    cvColorImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
    cvColorImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
    cvConvertorImage.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);

}



void ShapeFromShadingAnalysis::acquire()
{

    Timer* save_timer;
    TimerCallback<ShapeFromShadingAnalysis> save_callback(*this, &ShapeFromShadingAnalysis::save_cb);

    _run_cnt++;
    _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0, _synth_save_cnt = 0;    
    _RUN_DONE = false;
    create_dir_allocate_images();
    
    _animation_cnt1 = 0;
    _animation_cnt2 = 0;
    _animation_cnt3 = 0;
    _animation_cnt4 = 0;
    _animation_cnt5 = 0;
    _animation_cnt6 = 0;
    _animation_cnt7 = 0;
    _animation_cnt8 = 0;
    _animation_cnt9 = 0;
    _animation_cnt10 = 0;
    _animation_cnt11 = 0;
    _animation_cnt12 = 0;
    _animation_cnt13 = 0;
    _animation_cnt14 = 0;
    _animation_cnt15 = 0;
    _animation_cnt16 = 0;
    _animation_reset = false;  // coundn't get this to work - so using seperate counters - shitty!
    
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

void ShapeFromShadingAnalysis::synthesise()
{
   // cout << "ShapeFromShadingAnalysis::saving synthesis...\n";
    if(_state == STATE_STOP) return;
    
    for(float i=1;i<_saved_filenames_analysis.size()-1;i++){
        
    //    cout << "ShapeFromShadingAnalysis::synthesis FOR LOOP...\n";
        
     //   cout << "_saved_filenames_analysis[i]" << _saved_filenames_analysis[i] << endl;
        
        if(_state == STATE_STOP) return;
        
        if(!image1.loadImage(_saved_filenames_analysis[i])){
            //couldn't load image
            cout << "didn't load image" << endl;
        } 
        
        if(image1.loadImage(_saved_filenames_analysis[i])){
        //    cout << "LOADED image1!!!" << endl;
            if(image5.loadImage(_saved_filenames_analysis[i+1])){
                
                ///////////////////////// PROCESS THE SAVED CAMERA IMAGES OF SHIT TO THE IMAGES //////////////////////////
                
                cvColorImage1.setFromPixels(image1.getPixels(), image1.width, image1.height);
                cvColorImage2.setFromPixels(image5.getPixels(), image5.width, image5.height);
                
                cvColorImage1.convertToGrayscalePlanarImage(cvGrayImage1, 1);
                cvColorImage2.convertToGrayscalePlanarImage(cvGrayImage2, 1);
                
                cvGrayDiff1.absDiff(cvGrayImage2, cvGrayImage1);
                cvGrayDiff1.erode();
                cvGrayDiff1.contrastStretch();
                //cvGrayDiff1.blur(5);
                //cvGrayDiff1.dilate();
                //cvGrayDiff1.contrastStretch();
                
                /////////////////////////////////// SAVE TO DISK IN THE SYNTHESIS FOLDER ////////////////////////////////
                string file_name;
                
                file_name = ofToString(_synth_save_cnt, 2)+"_ShapeFromShadingSynthesis_"+ofToString(_run_cnt,2)+".jpg";
                
                
                //<---- THE OLD WAY OF SAVING - works on OSX but generates BLACK FRAMES on WINDOWS ---->
                // ofSaveImage(cvGrayImage1.getPixelsRef(),_whole_file_path_synthesis+"/"+file_name, OF_IMAGE_QUALITY_BEST);
            
                //<---- NEW SAVING - seems to fix WINDOWS saving out BLACK FRAMES PROBLEM ---->
                //ofImage image;
                //image.allocate(cvGrayDiff1.width, cvGrayDiff1.height, OF_IMAGE_GRAYSCALE);
                
                //*** This needs to be here for OSX of we get a BAD ACCESS ERROR. DOES IT BREAK WINDOWS? ***//
                //image.setUseTexture(false);  
               
                //image.setFromPixels(cvGrayDiff1.getPixels(), cvGrayDiff1.width, cvGrayDiff1.height, OF_IMAGE_GRAYSCALE);
                //image.saveImage(_whole_file_path_synthesis+"/"+file_name);
                
                //_saved_filenames_synthesis.push_back(_whole_file_path_synthesis+"/"+file_name);
                
                
                // <--- REALLY NEW SAVING METHOD --- 26 feb 2012 --- consolidated the save function into Abstract Analysis> ///
                cvConvertorImage.setFromGrayscalePlanarImages(cvGrayDiff1,cvGrayDiff1,cvGrayDiff1);
                
                saveImageSynthesis(file_name, &cvConvertorImage, OF_IMAGE_GRAYSCALE);
                _synth_save_cnt++;
                
            }
        }
    }
    
    // _saved_filenames_synthesis has processed all the files in the analysis images folder
    while(!_RUN_DONE && _state != STATE_STOP)
        Thread::sleep(3);    
    
}

void ShapeFromShadingAnalysis::displayresults()
{
    for(float i=1;i<_saved_filenames_synthesis.size();i++){
        
        if(_state == STATE_STOP) return;
        
       // cout << "_saved_filenames_analysis[i] - " << _saved_filenames_synthesis[i] << endl;
        
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

// this runs at frame rate = 33 ms for 30 FPS
void ShapeFromShadingAnalysis::draw()
{
    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            if (_frame_cnt < _frame_cnt_max)
            {
                
                ofEnableAlphaBlending();
                
                int _quarter_frame_cnt_max = _frame_cnt_max/4;
                int _half_frame_cnt_max = _frame_cnt_max/2;
                int _threequarters_frame_cnt_max = 3*_frame_cnt_max/4;
                
                if(_frame_cnt < _quarter_frame_cnt_max) {
                    
                    quad =  "QUAD1";
                    //cout << quad << endl;
                    
                    if(_frame_cnt <= _quarter_frame_cnt_max/4)
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt1, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(3*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());
                        _animation_cnt1++;
                    }
                    
                    if(_frame_cnt <= _quarter_frame_cnt_max/2)
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt2, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(2.5*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());   
                        _animation_cnt2++;
                    }
                    
                    if(_frame_cnt <= _quarter_frame_cnt_max) 
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt3, 0, _quarter_frame_cnt_max/4, 0, 255));
                        
                        ofRect(2.25*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());      
                        _animation_cnt3++;
                    }
                    
                    if((7*_quarter_frame_cnt_max/8 < _frame_cnt) && (_frame_cnt < _quarter_frame_cnt_max))
                    {
                        
                        ofSetColor(0, 0, 0, ofMap(_animation_cnt4, 0, _quarter_frame_cnt_max/8, 0, 255));                    
                        //cout << "4 counter: " << ofMap(_animation_cnt4, 0, _quarter_frame_cnt_max/8, 0, 255) << endl;
                        
                        ofRect(2.25*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());
                        //ofRect(2.25*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());   
                        //ofRect(2.5*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight()); 
                        //ofRect(2.75*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());      
                        _animation_cnt4++;
                    }  
                    
                    
                } 
                
                
                if((_frame_cnt >= _frame_cnt_max/4) && (_frame_cnt < _frame_cnt_max/2)){
                    
                    quad =  "QUAD2";
                    //cout << quad << endl;
                    
                    int _local_frame_count = _frame_cnt - _quarter_frame_cnt_max;
                    //cout << "_local_frame_count" << _local_frame_count << endl;
                    
                    if(_local_frame_count < _quarter_frame_cnt_max/4)
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt5, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(0, 3*ofGetHeight()/4, ofGetWidth(), ofGetHeight()/2);
                        //cout << "_animation_cnt1: " << _animation_cnt1 << endl;
                        
                        _animation_cnt5++;
                    }
                    
                    if(_local_frame_count < _quarter_frame_cnt_max/2)
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt6, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(0, 2.5*ofGetHeight()/4, ofGetWidth(), ofGetHeight()/2);
                        _animation_cnt6++;
                    }
                    
                    if(_local_frame_count < _quarter_frame_cnt_max) 
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt7, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(0, 2.25*ofGetHeight()/4, ofGetWidth(), ofGetHeight()/2);
                        _animation_cnt7++;
                    }
                    
                    if((7*_quarter_frame_cnt_max/8 < _local_frame_count) && (_local_frame_count < _quarter_frame_cnt_max))
                    {
                        
                        ofSetColor(0, 0, 0, ofMap(_animation_cnt8, 0, _quarter_frame_cnt_max/8, 0, 255));                    
                        //cout << "4 counter: " << ofMap(_animation_cnt4, 0, _quarter_frame_cnt_max/8, 0, 255) << endl;
                        
                        ofRect(0, 2.25*ofGetHeight()/4, ofGetWidth(), ofGetHeight()/2);
                        //ofRect(2.25*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());   
                        //ofRect(2.5*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight()); 
                        //ofRect(2.75*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());      
                        _animation_cnt8++;
                    }  
                    
                    
            
                }
                
                
                //draw bottom middle
                if ((_frame_cnt >= _frame_cnt_max/2)  && (_frame_cnt < 3*_frame_cnt_max/4)) {
                    
                    quad =  "QUAD3";
                    //cout << quad << endl;
                    
                    int _local_frame_count = _frame_cnt - _half_frame_cnt_max;
                    //cout << "_local_frame_count" << _local_frame_count << endl;
                    
                    
                    
                    if(_local_frame_count <= _quarter_frame_cnt_max/4)
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt9, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(ofGetWidth()/4, 0, -ofGetWidth()/2, ofGetHeight());
                        //cout << "_animation_cnt1: " << _animation_cnt1 << endl;
                        
                        _animation_cnt9++;
                    }
                    
                    if(_local_frame_count < _quarter_frame_cnt_max/2)
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt10, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(1.25*ofGetWidth()/4, 0, -ofGetWidth()/2, ofGetHeight());
                        _animation_cnt10++;
                    }
                    
                    if(_local_frame_count < _quarter_frame_cnt_max) 
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt11, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(1.5*ofGetWidth()/4, 0, -ofGetWidth()/2, ofGetHeight());
                        _animation_cnt11++;
                    }
                    
                    if((7*_quarter_frame_cnt_max/8 < _local_frame_count) && (_local_frame_count < _quarter_frame_cnt_max))
                    {
                        
                        ofSetColor(0, 0, 0, ofMap(_animation_cnt12, 0, _quarter_frame_cnt_max/8, 0, 255));                    
                        //cout << "4 counter: " << ofMap(_animation_cnt4, 0, _quarter_frame_cnt_max/8, 0, 255) << endl;
                        
                        ofRect(1.5*ofGetWidth()/4, 0, -ofGetWidth()/2, ofGetHeight());
                        //ofRect(2.25*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());   
                        //ofRect(2.5*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight()); 
                        //ofRect(2.75*ofGetWidth()/4, 0, ofGetWidth()/2, ofGetHeight());      
                        _animation_cnt12++;
                    }  
                
                }
                
                
                //draw left
                if((_frame_cnt >= 3*_frame_cnt_max/4) && (_frame_cnt <= _frame_cnt_max) ){
                    
                    quad =  "QUAD4";
                    //cout << quad << endl;
                    
                    int _local_frame_count = _frame_cnt - _threequarters_frame_cnt_max;
                    //cout << "_local_frame_count" << _local_frame_count << endl;
                    
                    
                    if(_local_frame_count <= _quarter_frame_cnt_max/4)
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt13, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(0, ofGetHeight()/4, ofGetWidth(), -ofGetHeight()/2);
                        //cout << "_animation_cnt13: " << _animation_cnt1 << endl;
                        
                        _animation_cnt13++;
                    }
                    
                    if(_local_frame_count < _quarter_frame_cnt_max/2)
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt14, 0, _quarter_frame_cnt_max/4, 0, 255));
                        
                        ofRect(0, 1.25*ofGetHeight()/4, ofGetWidth(), -ofGetHeight()/2);
                        _animation_cnt14++;
                    }
                    
                    if(_local_frame_count < _quarter_frame_cnt_max) 
                    {
                        ofSetColor(255, 255, 255, ofMap(_animation_cnt15, 0, _quarter_frame_cnt_max/4, 0, 255));
                        ofRect(0, 1.5*ofGetHeight()/4, ofGetWidth(), -ofGetHeight()/2);
                        _animation_cnt15++;
                    }
                    
                    if((7*_quarter_frame_cnt_max/8 < _local_frame_count) && (_local_frame_count < _quarter_frame_cnt_max))
                    {
                        
                        ofSetColor(0, 0, 0, ofMap(_animation_cnt16, 0, _quarter_frame_cnt_max/8, 0, 255));                    
                        //cout << "4 counter: " << ofMap(_animation_cnt4, 0, _quarter_frame_cnt_max/8, 0, 255) << endl;
                        
                        ofRect(0, 1.5*ofGetHeight()/4, ofGetWidth(), -ofGetHeight()/2);
                        
                        _animation_cnt16++;
                    }  
                    
                }
                
                ofDisableAlphaBlending();
            } else {
                //_state = STATE_SYNTHESISING;
                _frame_cnt = 0;
                _RUN_DONE = true;
            }
            
            _frame_cnt++;
            //cout << "_frame_cnt:" << _frame_cnt << endl;            
            
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...
            
           // cout << "RelaxRateAnalysis = STATE_SYNTHESISING...\n";
            
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
         //   cout << "STATE_DISPLAY_RESULTS...\n" << endl;
            
            if (_frame_cnt > 2)
            {
                _image_shown = true;
                _frame_cnt=0;
            }
            
            _frame_cnt++;
            
            if (_show_image)
            {  
            //    cout << "_show_image...\n" << endl;
                
                ofEnableAlphaBlending();
                
                ofSetColor(255, 255, 255);
                image2.setFromPixels(image3.getPixels(),image3.width,image3.height, OF_IMAGE_GRAYSCALE);
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
void ShapeFromShadingAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
            
    string file_name = ofToString(_save_cnt,2)+"_"+ quad +"_"+ofToString(_run_cnt,2)+".jpg";
    
    saveImageAnalysis(file_name);
}
