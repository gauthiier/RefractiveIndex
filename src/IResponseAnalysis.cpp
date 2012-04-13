#include "IResponseAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define NUMBER_RUNS     1
#define ACQUIRE_TIME    20

void IResponseAnalysis::setup(int camWidth, int camHeight)
{
    AbstractAnalysis::setup(camWidth, camHeight);
        
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis_NUM_RUN:NUM_RUN_iresponse", NUMBER_RUNS);
    cout << "NUM_RUN IResponseAnalysis " << NUM_RUN << endl;
    //NUM_RUN = 5;
    
    int acq_run_time;   // 10 seconds of acquiring per run
    acq_run_time = RefractiveIndex::XML.getValue("config:analysis_time:acquiretime_iresponse", ACQUIRE_TIME);
    cout << "ACQUIRE_TIME IResponseAnalysis " << acq_run_time << endl;

    //flag for main sketch
    meshIsComplete=false;
    _gotFirstImage=false;
    

    //int acq_run_time = 20;   // 20 seconds of acquiring per run
    
    DELTA_T_SAVE = 2*(10*acq_run_time/2); // for 20 seconds, we want this to be around 200 files
    // or 10 times per second = every 100 ms
    
    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames
    
    //create_dir_allocate_images();
    _synth_save_cnt = 0;
    _run_cnt = 0;
    _frame_cnt = 0;
    c = 0;
    
    int anim_time = 10;   // 10 seconds
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
    
    cvConvertorImage.clear();    
    
    cvColorImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff1.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
    cvColorImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
	cvGrayImage2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    cvGrayDiff2.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
    cvConvertorImage.allocate(RefractiveIndex::_vid_w,RefractiveIndex::_vid_h);
    
}


void IResponseAnalysis::acquire()
{

    Timer* save_timer;
    TimerCallback<IResponseAnalysis> save_callback(*this, &IResponseAnalysis::save_cb);

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
   
    //}
}

void IResponseAnalysis::synthesise()
{
    cout<<"SYNTHESISING IRESPONSE";
    
 
    //cout << "IResponseAnalysis::saving synthesis...\n";
    if(_state == STATE_STOP) return;
    
    for(float i=1;i<_saved_filenames_analysis.size()-1;i++){
        
        //cout << "IResponseAnalysis::synthesis FOR LOOP...\n";
    
        //cout << "_saved_filenames_analysis[i]" << _saved_filenames_analysis[i] << endl;
        
        if(_state == STATE_STOP) return;
        
        if(!image1.loadImage(_saved_filenames_analysis[i])){
            //couldn't load image
            cout << "didn't load image" << endl;
        } 
        
        if(image1.loadImage(_saved_filenames_analysis[i])){
             //cout << "LOADED image1!!!" << endl;
            if(image5.loadImage(_saved_filenames_analysis[i+1])){
                
                ///////////////////////// PROCESS THE SAVED CAMERA IMAGES OF SHIT TO THE IMAGES //////////////////////////
                if(!_gotFirstImage){
                    cout<<"background image is"<< _saved_filenames_analysis[i]<<endl;
                    _background=image1;
                    _gotFirstImage=true;
                }
                
                //subtract background begin///////////////

                ofPixels imagePixels1 = image1.getPixelsRef();
                ofPixels imagePixels2 = image5.getPixelsRef();
                ofPixels backgroundPixels = _background.getPixelsRef();

                for(int i=0;i<imagePixels1.size();i++){
                    
                    unsigned char val=imagePixels1[i];
                    // cout<<(int)backgroundPixels[i]<< " thesePixels[i] "<<(int)imagePixels1[i]<<endl;
                    if(imagePixels1[i]-backgroundPixels[i]>0){
                        imagePixels1[i]-=backgroundPixels[i];
                    }
                    else{
                        imagePixels1[i]=0;
                    }
                    if(imagePixels2[i]-backgroundPixels[i]>0){
                        imagePixels2[i]-=backgroundPixels[i];
                    }
                    else{
                        imagePixels2[i]=0;
                    }
                    
                }
                //update the images with their new background subtracted selves
                image1.setFromPixels(imagePixels1);
                image5.setFromPixels(imagePixels2);
                //subtract background end///////////////

                
                //flag the main app that we aren't read yet
                meshIsComplete=false;
                //make a mesh - this mesh will be drawn in the main app
                setMeshFromPixels(make3DZmap(image1, image5, _background), image1,image5, aMesh);
                //with jpgs this was refusing to save out
                meshFileName = _whole_file_path_synthesis+"/"+ofToString(_synth_save_cnt, 2)+"_IResponseSynthesis_"+ofToString(_run_cnt,2)+".png";
                _saved_filenames_synthesis.push_back(meshFileName);

                //flag that we are finished  
                meshIsComplete=true;
                _synth_save_cnt++;
            }
        }
    }

    // _saved_filenames_synthesis has processed all the files in the analysis images folder
    while(!_RUN_DONE && _state != STATE_STOP)
        Thread::sleep(3);
    
    
}


void IResponseAnalysis::displayresults()
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
            // cout << "didn't load image" << endl;
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
void IResponseAnalysis::draw()
{
    
    switch (_state) {
            
        case STATE_ACQUIRING:
        {
            ofEnableAlphaBlending();
            ofColor aColour;
            int _fade_in_frames = _frame_cnt_max/10;
            //cout<< "_fade_in_frames" << _fade_in_frames<< endl;
            
            if (_frame_cnt < _fade_in_frames) {
                
                aColour.set(255, 255, 255, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                ofSetColor(aColour);
                ofRect(0, 0, ofGetWidth(), ofGetHeight());
            
            }
            
            if (_frame_cnt < _frame_cnt_max)
            {
                ofSetColor(c, c, c);
                ofRect(0, 0, ofGetWidth(), ofGetHeight());
                c  = 255.0 * (_frame_cnt_max*_frame_cnt_max - _frame_cnt*_frame_cnt)/(_frame_cnt_max*_frame_cnt_max);
            } else {
                
                _RUN_DONE = true;
            }
            
            if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt < _frame_cnt_max) {
                aColour.set(0, 0, 0, ofMap(_frame_cnt-(_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255));
                ofSetColor(aColour);
                ofRect(0, 0, ofGetWidth(), ofGetHeight());
                // cout <<  "FADE OUT STROBE TIME " << endl;
            }         
            
            ofDisableAlphaBlending();
            _frame_cnt++;

            break;
        }
            
        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...
            
            // cout << "IResponse = STATE_SYNTHESISING...\n";
            
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
                    //cout << "IResponse STATE_SYNTHESIZING = FADING IN ANIMATION...\n";
                    
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
            
        }
        
        default:
            break;
    }

}

// this runs at save_cb timer rate = DELTA_T_SAVE
void IResponseAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    
    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
        
    saveImageAnalysis(file_name);
}

void IResponseAnalysis::setMeshFromPixels(ofPixels somePixels, ofImage currentFirstImage, ofImage currentSecondImage, ofMesh & mesh){
    int x=0;
    int y=0;
    
    //get rid of all previous vectors and colours
    mesh.clear();
    
   //unsigned char * thesePixels =currentSecondImage.getPixels();
    
    for(int i=0;i<somePixels.size();i+=3){
        mesh.addVertex(ofVec3f(x,y,- somePixels.getColor(x, y).getBrightness()   ));
        // add colour from current second image of two - this is a hang over from when i was comparing two images 
        mesh.addColor(  currentSecondImage.getColor(x, y)   );
        x++;
        if(x>=somePixels.getWidth()){
            x=0;
            y++;
        }
        
    }
}
ofPixels IResponseAnalysis::make3DZmap(ofImage &image1, ofImage &image2, ofImage &backgroundImag){
    
    ofPixels imagePixels1 = image1.getPixelsRef();
    ofPixels imagePixels2 = image2.getPixelsRef();
    ofPixels backgroundPixels = backgroundImag.getPixelsRef();
    
    
    ofPixels difference;
    //this unsigned char should be unnecessary - I would have thought - can't you just address the pixel locations in ofPixels directly? 
    unsigned char * thesePixels = new unsigned char[ imagePixels1.getWidth()*imagePixels1.getHeight()*3];
    
    int x=0;
    int y=0;
        
    int chooseComparison=1;
    //previous versiom which compared two images -now deprecated
    if(chooseComparison==0){
    //for each pixel...
        for(int i=0;i<imagePixels1.size();i+=3){
        
            ofColor colourImage1 = imagePixels1.getColor(x, y);
            ofColor colourImage2 = imagePixels2.getColor(x, y);
        
            //the brightness difference at this pixel address for both images
            int thisDiff=abs(colourImage1.getBrightness()-colourImage2.getBrightness());
        
            thesePixels[i]=thisDiff;
            thesePixels[i+1]=thisDiff;
            thesePixels[i+2]=thisDiff;
            x++;
            if(x>=imagePixels1.getWidth()){
                x=0;
                y++;
            
            }
        
        }
    }
    //the current version compares how bright this pixel is with how bright it would be following inverse square fall off from centre
    if(chooseComparison==1){
        //for each pixel...
        float _maxPossibleDistanceToCentre=ofDist(0,0,imagePixels1.getWidth()/2, imagePixels1.getHeight()/2);
        for(int i=0;i<imagePixels1.size();i+=3){
            
            ofColor colourImage1 = imagePixels1.getColor(x, y);
            ofColor colourImage2 = imagePixels2.getColor(x, y);
            
            float _distanceToCentre=ofDist(imagePixels1.getWidth()/2, imagePixels1.getHeight()/2, x, y);
            float _presumedBrightness=ofMap( sqrt(_maxPossibleDistanceToCentre)-sqrt(_distanceToCentre), 0,  sqrt(_maxPossibleDistanceToCentre), 0, 255);
            //float _presumedBrightness=255;
            int thisDiff=abs(colourImage1.getBrightness()-_presumedBrightness);
            //cout<<thisDiff<< " thisDiff "<<endl;

            thesePixels[i]=thisDiff;
            thesePixels[i+1]=thisDiff;
            thesePixels[i+2]=thisDiff;
            x++;
            if(x>=imagePixels1.getWidth()){
                x=0;
                y++;
                
            }
            
        }
    }
    
    difference.setFromPixels(thesePixels,imagePixels1.getWidth(),imagePixels1.getHeight(), 3);
    return difference;
    
}

