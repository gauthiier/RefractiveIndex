#include "DiffNoiseAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define NUMBER_RUNS     1
#define ACQUIRE_TIME    20

const int algo_default = 1;
const int scale_default = 500;
const int draw_style_default = 3;
const int line_width_default = 0.5f;
const float point_size_default = 0.5f;

void DiffNoiseAnalysis::setup(int camWidth, int camHeight)
{
    AbstractAnalysis::setup(camWidth, camHeight);
    
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis_NUM_RUN:NUM_RUN_diffnoise", NUMBER_RUNS);
    cout << "NUM_RUN DiffNoiseAnalysis " << NUM_RUN << endl;
    //NUM_RUN = 5;
    
    int acq_run_time;   // 10 seconds of acquiring per run
    acq_run_time = RefractiveIndex::XML.getValue("config:analysis_time:acquiretime_diffnoise", ACQUIRE_TIME);
    cout << "ACQUIRE_TIME DiffNoiseAnalysis " << acq_run_time << endl;
    
    //int acq_run_time = 20;   // 20 seconds of acquiring per run
    
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
    
    _show_image = false;
    _image_shown = false;
    
    image1.clear();
    image2.clear();
    
    //  images use for drawing the synthesized files to the screen ///
    image1.setUseTexture(false);  // the non texture image that is needed to first load the image
    image2.setUseTexture(true);   // the image that needs to get written to the screen which takes the content of image1
    
    image1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h,  OF_IMAGE_COLOR);
    image2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h,  OF_IMAGE_COLOR);
    
    
    ////---------
    
    algo = RefractiveIndex::XML.getValue("config:algorithms:diffnoise:algo", algo_default);
    scale = RefractiveIndex::XML.getValue("config:algorithms:diffnoise:scale", scale_default);
    draw_style = RefractiveIndex::XML.getValue("config:algorithms:diffnoise:draw_style", draw_style_default);
    line_width = RefractiveIndex::XML.getValue("config:algorithms:diffnoise:line_width", line_width_default);        
    point_size = RefractiveIndex::XML.getValue("config:algorithms:diffnoise:point_size", point_size_default);
    
}


void DiffNoiseAnalysis::acquire()
{
    Timer* save_timer;
    TimerCallback<DiffNoiseAnalysis> save_callback(*this, &DiffNoiseAnalysis::save_cb);

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

    //}
}

void DiffNoiseAnalysis::synthesise()
{   
    
    // we don't need to synthesise
    return;
    
    /*
     
     //cout << "IResponseAnalysis::saving synthesis...\n";
     if(_state == STATE_STOP) return;
     
     _RUN_DONE = false;
     
     // _saved_filenames_synthesis has processed all the files in the analysis images folder
     while(!_RUN_DONE && _state != STATE_STOP)
     Thread::sleep(3);
     */
    
}

void DiffNoiseAnalysis::displayresults()
{
    
    for(float i=1;i<_saved_filenames_analysis.size();i++){
        
        if(_state == STATE_STOP) return;
        
        //cout << "_saved_filenames_analysis[i] - " << _saved_filenames_synthesis[i] << endl;
        
        while(!_image_shown){
            Thread::sleep(2);
            //cout << "!_image_shown" << endl;
        }
        
        _show_image = false;
        
        
        if(!image1.loadImage(_saved_filenames_analysis[i])){
            //couldn't load image
            cout << "didn't load image" << endl;
        } 
        
        if(image1.loadImage(_saved_filenames_analysis[i])){
            //cout << "_show_image = true;" << endl;
            _show_image = true;
            _image_shown = false;
        }
    }         
    
}


// this runs at frame rate = 33 ms for 30 FPS
void DiffNoiseAnalysis::draw()
{
    switch (_state) {
        case STATE_ACQUIRING:
        {
            
            if (_frame_cnt < _frame_cnt_max)
            {
      
                ofColor aColour;
                int rectSize = 5;
                ofEnableAlphaBlending();
                
                int _fade_in_frames = _frame_cnt_max/10;
                
                if (_frame_cnt < _fade_in_frames) {
                    
                    for (int i=1; i < ofGetHeight() ; i=i+rectSize)
                    {
                        for (int j=1; j < ofGetWidth(); j=j+rectSize)
                        {
                            c = ofRandom(0,255);
                            aColour.set(c, c, c, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                            ofSetColor(aColour);
                            ofRect(j, i, rectSize, rectSize);
                        }
                    }        
                    
                    //cout << "FADING IN..." << endl;
                }
                
                
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt <= (_frame_cnt_max-_fade_in_frames)){
                    
                    for (int i=1; i < ofGetHeight() ; i=i+rectSize)
                    {
                        for (int j=1; j < ofGetWidth(); j=j+rectSize)
                        {
                            c = ofRandom(0,255);
                            aColour.set(c, c, c, 255);
                            ofSetColor(aColour);
                            ofRect(j, i, rectSize, rectSize);
                            
                        }
                    }        
                }
                
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt <= _frame_cnt_max) {
                    
                    for (int i=1; i < ofGetHeight() ; i=i+rectSize)
                    {
                        for (int j=1; j < ofGetWidth(); j=j+rectSize)
                        {
                            c = ofRandom(0,255);
                            aColour.set(c, c, c, 255-ofMap(_frame_cnt-(_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255));
                            ofSetColor(aColour);
                            ofRect(j, i, rectSize, rectSize);
                        } 
                    }
                    
                    //_fade_cnt++;
                    //cout << "FADING OUT..." << endl;
                }
            
                ofDisableAlphaBlending();
           
            } else {
               // _state  = STATE_SYNTHESISING;
               _RUN_DONE = true;
            
            }
            
            _frame_cnt++;
            
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...
            
            //cout << "DiffNoiseAnalysis = STATE_SYNTHESISING...\n";
            
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
                    //cout << "DiffNoiseAnalysis STATE_SYNTHESIZING = FADING IN ANIMATION...\n";
                    
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
            if (_frame_cnt > 2)
            {
                _image_shown = true;
                _frame_cnt=0;
            }
            
            _frame_cnt++;
            
            ofEnableAlphaBlending();
            glShadeModel(GL_SMOOTH);
            glLineWidth(line_width);
            glPointSize(point_size);
            glEnable(GL_POINT_SMOOTH);
            
            //RefractiveIndex::cam.begin();
            
            ofTranslate(tx, ty, tz);
            ofRotateX(rx); ofRotateY(ry); ofRotateZ(rz);
            glScalef(1.5, 1, 1);
            
            if (_show_image)
                image2.setFromPixels(image1.getPixels(), image1.width, image1.height, OF_IMAGE_COLOR);
            
            image2.bind();        
            
            RefractiveIndex::_shader.begin();
            
            RefractiveIndex::_shader.setUniform1i("algo", algo);
            RefractiveIndex::_shader.setUniform1f("scale", scale);
            RefractiveIndex::_shader.setUniform1i("tex0", 0);
            
            switch (draw_style) {
                case VERTS:
                    RefractiveIndex::_mesh_vbo.drawVertices();
                    break;
                case WIRE:
                    RefractiveIndex::_mesh_vbo.drawWireframe();
                    break;
                case FACE:
                    RefractiveIndex::_mesh_vbo.drawFaces();
                    break;            
            }
            
            RefractiveIndex::_shader.end();                
            
            image2.unbind();
            
            //RefractiveIndex::cam.end();    
            
            break;
            
        }
        default:
            break;
    }
}

// this runs at save_cb timer rate = DELTA_T_SAVE
void DiffNoiseAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
            
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";

    saveImageAnalysis(file_name);
}
