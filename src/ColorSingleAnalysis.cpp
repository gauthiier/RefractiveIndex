/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk
 */

#include "ColorSingleAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;


void ColorSingleAnalysis::setup(int camWidth, int camHeight)
{
    DELTA_T_SAVE = 100;//300;
    NUM_PHASE = 1;
    NUM_RUN = 1;
    NUM_SAVE_PER_RUN = 100;    
    
    create_dir();
    _frame_cnt = 0;
    _frame_cnt_max = ofGetFrameRate() * ((DELTA_T_SAVE * NUM_SAVE_PER_RUN) / 1000);
    r = 0;
    g = 0;
    b = 0;
    
    _fade_cnt=0;
    fileNameTag = "";
}


void ColorSingleAnalysis::acquire()
{

    Timer* save_timer;

    TimerCallback<ColorSingleAnalysis> save_callback(*this, &ColorSingleAnalysis::save_cb);

    // RUN ROUTINE
    for(int i = 0; i < NUM_RUN; i++) {

        _run_cnt = i;

        cout << "RUN NUM = " << i;

        save_timer = new Timer(0, DELTA_T_SAVE); // timing interval for saving files
        save_timer->start(save_callback);
        _RUN_DONE = false;
        _frame_cnt = 0; _save_cnt = 0;

        while(!_RUN_DONE)
            Thread::sleep(3);

        save_timer->stop();
    }
}

void ColorSingleAnalysis::synthesise()
{
    // _saved_filenames has all the file names of all the saved images
    //incrementer to whichMesh
    speed=0.2;
    //whichMesh is the index in the vector of meshes
    whichMesh=0;
    
    int index=0;
    float iterator=1;
    bool debug=false;
    if(debug){
        _saved_filenames.clear();
        _saved_filenames=getListOfImageFilePaths("MIDDLESBOROUGH", _name);
        
        //hack to limit number of meshes.
        if(_saved_filenames.size()>100){
            iterator= _saved_filenames.size() /100;
        }
        
    }
    //clear vector so we don't add to it on successive runs
    meshes.clear();
    
    for(float i=0;i<_saved_filenames.size()-1;i+=iterator){
        
        ofImage image1;
        ofImage image2;
        
        //there is a known issue with using loadImage inside classes in other directories. the fix is to call setUseTExture(false)
        image1.setUseTexture(false);
        image2.setUseTexture(false);
        //some of the textures are not loading correctly so only make mesh if both the images load
        if(image1.loadImage(_saved_filenames[i]) && image2.loadImage(_saved_filenames[i+1])){
            meshes.push_back(ofMesh());

            if(i<_saved_filenames.size()/3){
                setMeshFromPixels( calculateListOfZValues(image1,image2, COMPARE_RED), image2, &meshes[index]);
            }
            if(i>=_saved_filenames.size()/3 && i<2* _saved_filenames.size()/3){
                setMeshFromPixels( calculateListOfZValues(image1,image2, COMPARE_GREEN), image2, &meshes[index]);   
            }
            if(i>= 2* _saved_filenames.size()/3 && i<_saved_filenames.size()){
                setMeshFromPixels( calculateListOfZValues(image1,image2, COMPARE_BLUE), image2, &meshes[index]);            
            }
            index++;
        }
    }
}

void ColorSingleAnalysis::display_results(){
    
    Timer* display_results_timer;
    
    TimerCallback<ColorSingleAnalysis> display_results_callback(*this, &ColorSingleAnalysis::display_results_cb);
    // display results of the synthesis
    
    display_results_timer = new Timer(0, 20); // timing interval for saving files
    display_results_timer->start(display_results_callback);
    _RUN_DONE = false;
    _results_cnt=0;
    _results_cnt_max=500;
    
    while(!_RUN_DONE)
        Thread::sleep(3);
    
    display_results_timer->stop();
    
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
                    
                    ofSetColor(0, 0, 255-ofMap(_fade_cnt, 0, _fade_in_frames, 0, 255));
                    cout << "255-ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255)"<< 255-ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255) << endl;
                    
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    _fade_cnt++;
                    fileNameTag = "FADING";
                }
            
            } else {
                _RUN_DONE = true;
            }
            
            _frame_cnt++;
            
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
            int imageWidth=640;
            int imageHeight =480;
            ofPushMatrix();
            ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
            ofRotateY(_results_cnt*0.3);
            //ofRotateX(90);
            //ofRotateZ(whichMesh);
            ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2),-400;
            ofTranslate((ofGetWidth()/2)-(imageWidth/2),0,0 );
            
            meshes[whichMesh].drawVertices();
            ofPopMatrix();
            whichMesh+=speed;
            
            if(whichMesh>meshes.size() -1 || whichMesh<0){
                speed*=-1;
                whichMesh+=speed;
            }

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
    
    //    cout << "ColorSingleAnalysis::saving...\n";
    
    string file_name =ofToString(_save_cnt,2)+"_"+fileNameTag+"_"+ofToString(_run_cnt,2)+".jpg";
    
    //cout<<ofToString(_save_cnt,2)+"_"+fileNameTag+"_"+ofToString(_run_cnt,2)+".jpg";
    cout<<file_name<<endl;
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path+"/"+file_name, OF_IMAGE_QUALITY_BEST);
    _saved_filenames.push_back(ofToDataPath("")+_whole_file_path+"/"+file_name);

    //cout<<_whole_file_path+"/"+file_name<<endl;
    
    //if(_save_cnt >= NUM_SAVE_PER_RUN)
    //    _RUN_DONE = true;

}
void ColorSingleAnalysis::display_results_cb(Timer& timer){
    _results_cnt++;
    if (_results_cnt>_results_cnt_max) {
        _RUN_DONE=true;
    }
}

