/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "ShapeFromShadingAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;


void ShapeFromShadingAnalysis::setup(int camWidth, int camHeight)
{
    DELTA_T_SAVE = 300;  //300 is about the right number
    NUM_PHASE = 1;
    NUM_RUN = 1;
    NUM_SAVE_PER_RUN = 100;    
    
    create_dir();
    
    _level = 0;
    _flip = 1;
    _frame_cnt = 0;
    
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    c = 0;
}


void ShapeFromShadingAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<ShapeFromShadingAnalysis> save_callback(*this, &ShapeFromShadingAnalysis::save_cb);

    // RUN ROUTINE
    for(int i = 0; i < NUM_RUN; i++) {

        _run_cnt = i;

        cout << "RUN NUM = " << i;

        save_timer = new Timer(0, DELTA_T_SAVE); // timing interval for saving files
        save_timer->start(save_callback);
        _RUN_DONE = false;
        _frame_cnt = 0; _save_cnt = 0;
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
        
        while(!_RUN_DONE)
            Thread::sleep(3);

        save_timer->stop();
    }
}

void ShapeFromShadingAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
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
                
                //TODO:  put in CROSS FADES, ETC§E 
                
                /*
                 if (_animation_reset == true)
                 {
                 _animation_cnt1 = 0;
                 _animation_cnt2 = 0;
                 _animation_cnt3 = 0;
                 _animation_cnt4 = 0;
                 }
                 */
                
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
        
                _RUN_DONE = true;
            }
            
            _frame_cnt++;
            //cout << "_frame_cnt:" << _frame_cnt << endl;            
            
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...
            break;
        }
            
        case STATE_DISPLAY_RESULTS:
        {
            // display results of the synthesis
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
    
    cout << "ShapeFromShadingAnalysis::saving...\n";
    
    string file_name = ofToString(_save_cnt,2)+"_"+ quad +"_"+ofToString(_run_cnt,2)+".jpg";
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    
    _saved_filenames.push_back(_whole_file_path+"/"+file_name);

}
