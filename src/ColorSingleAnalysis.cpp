#include "ColorSingleAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define NUMBER_RUNS     1
#define ACQUIRE_TIME    20

void ColorSingleAnalysis::setup(int camWidth, int camHeight)
{
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis_NUM_RUN:NUM_RUN_colorsingle", NUMBER_RUNS);
    cout << "NUM_RUN ColorSingleAnalysis " << NUM_RUN << endl;
    //NUM_RUN = 5;
    
    int acq_run_time;   // 10 seconds of acquiring per run
    acq_run_time = RefractiveIndex::XML.getValue("config:analysis_time:acquiretime_colorsingle", ACQUIRE_TIME);
    cout << "ACQUIRE_TIME ColorSingleAnalysis " << acq_run_time << endl;

    //int acq_run_time = 25;   // 20 seconds of acquiring per run
    
    DELTA_T_SAVE = 1*(10*acq_run_time/2); // for 20 seconds, we want this to be around 200 files
                                            // or 10 times per second = every 100 ms
    
    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames
    
    //create_dir_allocate_images();
    
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
    
    cvConvertorImage.clear();    
    
    cvColorImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
    cvColorImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
    cvConvertorImage.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
}


void ColorSingleAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<ColorSingleAnalysis> save_callback(*this, &ColorSingleAnalysis::save_cb);

    _run_cnt++;
    _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0, _synth_save_cnt = 0;
    _RUN_DONE = false;
    create_dir_allocate_images();
    
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
        
        image1.loadImage("Mar_02_19_00_47_2/194_9.72_1.jpg");
        image1.resize(image1.width/20, image1.height/20);
        
        int width = image1.width;
        int height = image1.height;
        
        // get the pixels from the image
        imagePixels = image1.getPixels();
        
        // ------------------- create the vector field ------------------------------------
        
        vectorCount = width * height;
        
        // create a 2d vector field
        vectorField = new ofVec2f[vectorCount];
        
        // set all values in vector field to 0.0
        for(int i=0; i<vectorCount; i++){
            
            vectorField[i].x = 0.0;
            vectorField[i].y = 0.0;
        }
        
        
        // ------------------- calculate the vectors ------------------------------------
        
        // loop through all of the pixels
        for(int x=1; x< width-1; x++){
            for(int y=1; y< height-1; y++){
                
                char areaPixels[9];
                
                // loop through the area pixels
                for(int i=-1; i<=1; i++){
                    for(int j=-1; j<=1; j++){
                        
                        // determine where to read from in the area (not optimized)
                        int readPos = ((y + j) * width + (x + i)) * 3;
                        
                        unsigned char R = imagePixels[readPos];
                        unsigned char G = imagePixels[readPos+1];
                        unsigned char B = imagePixels[readPos+2];
                        
                        unsigned char BR = (0.299 * R) + (.587 * G) + (.114 * B);
                        
                        int writePos = (j+1) * 3 + (i + 1);
                        
                        areaPixels[writePos] = BR;
                    }
                }
                
                
                
                float dX = (areaPixels[0] + areaPixels[3] + areaPixels[6])/3 - (areaPixels[2] + areaPixels[5] + areaPixels[8])/3;
                float dY = (areaPixels[0] + areaPixels[1] + areaPixels[2])/3 - (areaPixels[6] + areaPixels[7] + areaPixels[8])/3;
                
                int vectorPos = y * width + x;
                
                //printf("dx %f\n", dX);
                
                vectorField[vectorPos].x = dX;
                vectorField[vectorPos].y = dY;
                
                //vectorField[vectorPos].x = -dY;
                //vectorField[vectorPos].y = dX;
                
            }
        }
        
        
        
        // ------------------- normalize the vectors ------------------------------------
        
        // variables for the maximum magnitude (absolute) in x and y
        float maxMagX = 1.0;
        float maxMagY = 1.0;
        
        // loop through the vector field to find the maximum x and y values
        for(int i=0; i< vectorCount; i++){
            
            if(fabs(vectorField[i].x) > maxMagX)  maxMagX = fabs(vectorField[i].x);
            if(fabs(vectorField[i].y) > maxMagY)  maxMagY = fabs(vectorField[i].y);
        }
        
        // loop through the vector field to normalize the values
        for(int i=0; i< vectorCount; i++){
            
            vectorField[i].x /= maxMagX;
            vectorField[i].y /= maxMagY;
        }


        
        
        // COMMENTING OUT THE FILLER SYNTH ALGORITHM
        /*  
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
                
                
                //<---- THE OLD WAY OF SAVING - works on OSX but generates BLACK FRAMES on WINDOWS ---->
                // ofSaveImage(cvGrayImage1.getPixelsRef(),_whole_file_path_synthesis+"/"+file_name, OF_IMAGE_QUALITY_BEST);
                
                
                //<---- NEW SAVING - seems to fix WINDOWS saving out BLACK FRAMES PROBLEM ---->
                //ofImage image;
                //image.allocate(cvColorImage1.width, cvColorImage1.height, OF_IMAGE_COLOR);
                
                // This needs to be here for OSX of we get a BAD ACCESS ERROR. DOES IT BREAK WINDOWS? 
                //image.setUseTexture(false);  
                
                //image.setFromPixels(cvColorImage1.getPixels(), cvColorImage1.width, cvColorImage1.height,OF_IMAGE_COLOR);
                //image.saveImage(_whole_file_path_synthesis+"/"+file_name);
            
                //_saved_filenames_synthesis.push_back(_whole_file_path_synthesis+"/"+file_name);
                
                // <--- REALLY NEW SAVING METHOD --- 26 feb 2012 --- consolidated the save function into Abstract Analysis> ///
            
                saveImageSynthesis(file_name, &cvColorImage1, OF_IMAGE_COLOR);
                _synth_save_cnt++;
        
           // }
        
         }
         */
        
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
            
            cout << "case STATE_DISPLAY_RESULTS = true" << endl;

            int width = image1.width;
            int height = image1.height;
            float spacing = 10;
            
            if (_frame_cnt > 2)
            {
                _image_shown = true;
                _frame_cnt=0;
            }
            
            _frame_cnt++;

            if (_show_image)
            {  
                cout << "_show_image = true" << endl;
               
                for(int x=0; x<width; x++){
                    
                    float xPos = (float) x * spacing + 5;
                    
                    for(int y=0; y<height; y++){
                        float yPos = (float)y * spacing + 5;
                        glPushMatrix();
                            ofLine(xPos, yPos, xPos+(vectorField[y*width+x].x*spacing), yPos+(vectorField[y*width+x].y*spacing));
                        glPopMatrix();
                    }
                }
            }
            
            _RUN_DONE = true;
            break;

            /*
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
                    image2.draw(0,0, ofGetWidth(), ofGetHeight());
                
                ofDisableAlphaBlending();
            }
            
            // display results of the synthesis
            _RUN_DONE = true;
            break;
            */
            
        }
        default:
        break;
    }
    
}

// this runs at save_cb timer rate = DELTA_T_SAVE
void ColorSingleAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    string file_name =ofToString(_save_cnt,2)+"_"+fileNameTag+"_"+ofToString(_run_cnt,2)+".jpg";
    
    saveImageAnalysis(file_name);
}
