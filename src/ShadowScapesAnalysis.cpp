#include "ShadowScapesAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "RefractiveIndex.h"

//#include "ofxXmlSettings.h"
#include "ofxOpenCv.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define STATE_SCAN      0
#define STATE_ANALYSIS  1
#define NUMBER_RUNS     1
#define ACQUIRE_TIME    20

const int algo_default = 1;
const int scale_default = 500;
const int draw_style_default = 3;
const double line_width_default = 0.5;

void ShadowScapesAnalysis::setup(int camWidth, int camHeight)
{
    AbstractAnalysis::setup(camWidth, camHeight);
    
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis_NUM_RUN:NUM_RUN_shadowscapes", NUMBER_RUNS);
    cout << "NUM_RUN ShadowScapesAnalysis " << NUM_RUN << endl;
    //NUM_RUN = 5;
    
    int acq_run_time;   // 10 seconds of acquiring per run
    acq_run_time = RefractiveIndex::XML.getValue("config:analysis_time:acquiretime_shadowscapes", ACQUIRE_TIME);
    cout << "ACQUIRE_TIME ShadowScapesAnalysis " << acq_run_time << endl;
    
    int screenSpan;
    if (_dir == V) screenSpan = ofGetHeight();
    if (_dir == H) screenSpan = ofGetWidth();
    if (_dir == D) screenSpan = ofGetHeight();
    
    _step = (screenSpan/acq_run_time)/(ofGetFrameRate());
            // pixel per frame = (pixels / sec) / (frame / sec)
    
    // 40 pixels per second should give us a 20 second scan at 800 pixels wide

    DELTA_T_SAVE = 3*(10*acq_run_time/2);   // for 20 seconds, we want this to be around 100 files
                                            // or 5 times per second = every 200 ms
    
    _scanLineWidth = 300.0;
    _run_cnt = 0;
    _save_cnt = 0;
    _synth_save_cnt = 0;

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
    
    algo = RefractiveIndex::XML.getValue("config:algorithms:shadowscapes:algo", algo_default);
    scale = RefractiveIndex::XML.getValue("config:algorithms:shadowscapes:scale", scale_default);
    draw_style = RefractiveIndex::XML.getValue("config:algorithms:shadowscapes:draw_style", draw_style_default);
    line_width = RefractiveIndex::XML.getValue("config:algorithms:shadowscapes:line_width", line_width_default);
    
}

void ShadowScapesAnalysis::acquire()
{
        
    Timer save_timer(0, DELTA_T_SAVE);  
    TimerCallback<ShadowScapesAnalysis> save_callback(*this, &ShadowScapesAnalysis::save_cb);
    
    _run_cnt++;
    _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0, _synth_save_cnt = 0;
    _line = 0;
    _RUN_DONE = false;

    create_dir_allocate_images();
   
    save_timer.start(save_callback);
      
    while(!_RUN_DONE && _state != STATE_STOP)
        Thread::sleep(3);
    
    save_timer.stop();

}

void ShadowScapesAnalysis::synthesise()
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


void ShadowScapesAnalysis::displayresults()
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
                _RUN_DONE = true;
                
            }
            
            if(_dir == H && int(_line) >= (ofGetWidth()+4*_scanLineWidth)) {
                
                //cout << "HORIZONTAL IS DONE -  _line >= (ofGetWidth()+4*_scanLineWidth)) is TRUE" << endl;
                _RUN_DONE = true;
                
            }
            
            if(_dir == D && int(_line) >= (1.5*ofGetHeight()+4*_scanLineWidth)) {
                //cout << "DIAGONAL IS DONE - _line >= (1.5*ofGetHeight()+4*_scanLineWidth)) is TRUE" << endl;
            
                _RUN_DONE = true;
            }
        
            break;
        }
            
        case STATE_SYNTHESISING:
        {
           // cout << "ShadowScapesAnalysis = STATE_SYNTHESISING...\n";

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
                  //  cout << "ShadowScapesAnalysis STATE_SYNTHESIZING = FADING IN ANIMATION...\n";
                
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
            
            
            ofEnableAlphaBlending();
            glShadeModel(GL_SMOOTH);
            glLineWidth(line_width);
            
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
        
    saveImageAnalysis(file_name);
        
}
