#include "ColorMultiAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define NUMBER_RUNS     1
#define ACQUIRE_TIME    20

void ColorMultiAnalysis::setup(int camWidth, int camHeight)
{
    AbstractAnalysis::setup(camWidth, camHeight);
    
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis_NUM_RUN:NUM_RUN_colormulti", NUMBER_RUNS);
    cout << "NUM_RUN ColorMultiAnalysis " << NUM_RUN << endl;
    //NUM_RUN = 5;
    
    //flag for main sketch
    meshIsComplete=false;
    _gotFirstImage=false;
    
    _mesh_size_multiplier=5;
    vertexSubsampling = 1;
    chooseColour=1;
    
    int acq_run_time;   // 10 seconds of acquiring per run
    acq_run_time = RefractiveIndex::XML.getValue("config:analysis_time:acquiretime_colormulti", ACQUIRE_TIME);
    cout << "ACQUIRE_TIME ColorMultiAnalysis " << acq_run_time << endl;

    //int acq_run_time = 35;  
    
    DELTA_T_SAVE = 1*(10*acq_run_time/2);   // for 20 seconds, we want this to be around 200 files
                                            // or 10 times per second = every 100 ms
    
    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames
    
    //create_dir_allocate_images();
    
    _frame_cnt = 0;
    _run_cnt = 0;
    _synth_save_cnt = 0;
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

void ColorMultiAnalysis::acquire()
{

    Timer* save_timer;
    TimerCallback<ColorMultiAnalysis> save_callback(*this, &ColorMultiAnalysis::save_cb);

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

void ColorMultiAnalysis::synthesise()
{
    
    //cout << "ColorMultiAnalysis::saving synthesis...\n";
    if(_state == STATE_STOP) return;
    
    for(float i=1;i<_saved_filenames_analysis.size()-1;i++){
        
        //cout << "ColorMultiAnalysis::synthesis FOR LOOP...\n";
        
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
            
            //cvColorImage1.blur(1);
            //cvColorImage1.erode();
            
            //cvColorImage1.dilate();
            //cvColorImage1.dilate();
            //cvColorImage1.dilate();
            //cvColorImage1.dilate();
            
            //cvFloatImage1 = cvColorImage1;
            //cvGrayImage1 = cvColorImage1;

            cvSmooth( cvColorImage1.getCvImage(), cvColorImage1.getCvImage(), CV_GAUSSIAN, 9, 9);
            cvXorS( cvColorImage1.getCvImage(), cvScalarAll(1), cvColorImage1.getCvImage(), 0 );
            
            //cvCanny(cvGrayImage1.getCvImage(), cvGrayImage1.getCvImage(), 100, 100, 3);
            //cvLaplace(cvGrayImage1.getCvImage(), cvGrayImage1.getCvImage(), 0);
            
            //cvGrayImage1 = cvCreateImage(cvSize(image1.width, image1.height),IPL_DEPTH_16S,1);
            //cvSobel(cvGrayImage1.getCvImage(), cvGrayImage1.getCvImage(), 0, 1, 3);
            
            // convert the CV image 
            image1.setFromPixels(cvColorImage1.getPixelsRef()); 
            
            ///////////////////////// PROCESS THE SAVED CAMERA IMAGES OF SHIT TO THE IMAGES //////////////////////////
            if(!_gotFirstImage){
                cout<<"background image is"<< _saved_filenames_analysis[i]<<endl;
                _background=image1;
                _gotFirstImage=true;
            }
            
            //subtract background begin///////////////
            
            ofPixels imagePixels1       = image1.getPixelsRef();
            //ofPixels imagePixels2       = image5.getPixelsRef();
            ofPixels backgroundPixels   = _background.getPixelsRef();
            
            for(int i=0;i<imagePixels1.size();i++){
                //unsigned char val=imagePixels1[i];
                // cout<<(int)backgroundPixels[i]<< " thesePixels[i] "<<(int)imagePixels1[i]<<endl;
                if(imagePixels1[i]-backgroundPixels[i]>0){
                    imagePixels1[i]-=backgroundPixels[i];
                }
                else{
                    imagePixels1[i]=0;
                }
            }
            
            //update the images with their new background subtracted selves
            image1.setFromPixels(imagePixels1);
            
            //flag the main app that we aren't read yet
            meshIsComplete=false;
            
            //make a mesh - this mesh will be drawn in the main app
            setMeshFromPixels(_returnDepthsAtEachPixel(image1, image1, _background), image1, image1, aMesh);
            
            //setMeshFromPixels(_returnDepthsAtEachPixel(image1, image1, _background), image1, image1, aMesh);
            
            /////////////////////////////////// SAVE TO DISK IN THE SYNTHESIS FOLDER ////////////////////////////////
            //string file_name;
            
            //with jpgs this was refusing to save out
            meshFileName = _whole_file_path_synthesis+"/"+ofToString(_synth_save_cnt, 2)+"_ColorMultiAnalysis_"+ofToString(_run_cnt,2)+".png";
            _saved_filenames_synthesis.push_back(meshFileName);
            
            //file_name = ofToString(_synth_save_cnt, 2)+"_ColorMultiAnalysis_"+ofToString(_run_cnt,2)+".jpg";
            
            //flag that we are finished  
            meshIsComplete=true;
            _synth_save_cnt++;
            
            // }
        }
    }
    
    // _saved_filenames_synthesis has processed all the files in the analysis images folder
    while(!_RUN_DONE && _state != STATE_STOP)
        Thread::sleep(3);
}

void ColorMultiAnalysis::displayresults()
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


void ColorMultiAnalysis::draw()
{
    
    ofEnableSmoothing();
    
    ofEnableLighting(); 
    ofEnableSeparateSpecularLight();     
    light.enable();
	
    light.setPosition(200,200,-150);
    lightStatic.enable();
    
    glEnable(GL_DEPTH_TEST);
    
    ofSetLineWidth(2.0f);
    //glPointSize(4.0f);
    
    ofEnableBlendMode ( OF_BLENDMODE_ADD );
    //ofEnableBlendMode ( OF_BLENDMODE_MULTIPLY );
    //ofEnableBlendMode ( OF_BLENDMODE_SUBTRACT );
    //ofEnableBlendMode ( OF_BLENDMODE_ALPHA );
    //ofEnableBlendMode ( OF_BLENDMODE_SCREEN );

    
    switch (_state) {
        case STATE_ACQUIRING:
        {
            ofEnableAlphaBlending();
            
            if (_frame_cnt < _frame_cnt_max)
            {

                int _fade_in_frames = _frame_cnt_max/50;
                ofColor aColor;
                
                if (_frame_cnt < _fade_in_frames) {
                   
                    
                    aColor.setHsb(c, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255), ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                    
                    ofSetColor(aColor);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    
                    //cout << "FADING IN..." << endl;
                }
                
                
                
                if (_frame_cnt >= _fade_in_frames && _frame_cnt < _frame_cnt_max-_fade_in_frames){
                    
                    aColor.setHsb(c, 255, 255);
                    ofSetColor(aColor);
                    
                    //how far are we as a percent of _frame_count_max * 360 HUE VALUES
                    c  = 255.0 * (_frame_cnt_max - _frame_cnt)/(_frame_cnt_max);
                    
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                }
                
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt <= _frame_cnt_max) {
                    
                    aColor.set(c, c, c, 255-int(ofMap(_frame_cnt-(_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255)));
                    
                    //aColor.setHsb(c, 255-ofMap(_fade_cnt- (_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255), 255-(ofMap(_fade_cnt-(_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255)));
                    
                    ofSetColor(aColor);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    //cout << "FADING OUT..." << endl;
                    
                }
                
                
            } else {
                //_state = STATE_SYNTHESISING;
                _RUN_DONE = true;
            }
            
            _frame_cnt++;            
            ofDisableAlphaBlending();
            break;
        }
            
        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...
            
            //cout << "ColorMultiAnalysis = STATE_SYNTHESISING...\n";
            
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
                    //cout << "ColorMultiAnalysis STATE_SYNTHESIZING = FADING IN ANIMATION...\n";
                    
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
                _state = STATE_DISPLAY_RESULTS;
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
void ColorMultiAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;
    string file_name = ofToString(_save_cnt,2)+"_"+ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";
    saveImageAnalysis(file_name);    
}


void ColorMultiAnalysis::setMeshFromPixels(vector<float> sPixels, ofImage currentFirstImage, ofImage currentSecondImage, ofMesh & mesh){
    int x=0;
    int y=0;
    
    //get rid of all previous vectors and colours
    mesh.clear();
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    //mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    //mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    //mesh.setMode(OF_PRIMITIVE_LINES);
    //mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    //mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
    //mesh.setMode(OF_PRIMITIVE_POINTS);
    
    /*
     OF_PRIMITIVE_TRIANGLES,
     OF_PRIMITIVE_TRIANGLE_STRIP,
     OF_PRIMITIVE_TRIANGLE_FAN,
     OF_PRIMITIVE_LINES,
     OF_PRIMITIVE_LINE_STRIP,
     OF_PRIMITIVE_LINE_LOOP,
     OF_PRIMITIVE_POINTS
     
     */
    
    ofColor meshColour=ofColor(255,255,255);
    
   
    
    //the average z position of the matrix - used later to centre the mesh on the z axis when drawing
    float zPlaneAverage=0;
    
    for(int i=0;i<sPixels.size();i++){
        zPlaneAverage+=sPixels[i];
    }
    if (sPixels.size()!=0) {
        zPlaneAverage/=sPixels.size();
        //cout<<zPlaneAverage<<" zPlaneAverage "<<endl;
    }
    
    else{
        cout<<"DEPTH FLOAT ARRAY IS EMPTY";
    }
    
    if(chooseColour==1){
        
        for(int i=0;i<sPixels.size();i++){
            mesh.addColor(  currentSecondImage.getColor(x, y+1));
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*(y+1),- sPixels[ (currentSecondImage.getWidth()*(y+1))+x    ]   ));
            
            mesh.addColor(  currentSecondImage.getColor(x, y));
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*y,- sPixels[(currentSecondImage.getWidth()*(y))+x ]  ));
            
            mesh.addColor(  currentSecondImage.getColor(x+1, y+1));
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+1),_mesh_size_multiplier*(y+1),- sPixels[(currentSecondImage.getWidth()*(y+1))+x+1 ]  ));
            
            mesh.addColor(  currentSecondImage.getColor(x+1, y+1));
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+1),_mesh_size_multiplier*(y+1),- sPixels[(currentSecondImage.getWidth()*(y+1))+x+1]   ));
            
            mesh.addColor(  currentSecondImage.getColor(x, y));
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*y,- sPixels[(currentSecondImage.getWidth()*(y))+x ]  ));
            
            mesh.addColor(  currentSecondImage.getColor(x+1, y)   );
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+1),_mesh_size_multiplier*y,- sPixels[(currentSecondImage.getWidth()*(y))+x +1 ]));
            
            x=x+vertexSubsampling;
            if(x>=currentSecondImage.getWidth()-1){
                x=0;
                y=y+vertexSubsampling;
                //something is going badly wrong with my maths for me to need this HELP TODO fix this - why am I running over the end of the vector?
                if(y>=currentSecondImage.getHeight()-1){
                    break;
                }
            }
        }
    }
}



vector<float> ColorMultiAnalysis::_returnDepthsAtEachPixel(ofImage &image1, ofImage &image2, ofImage &backgroundImag){
    
    ofPixels imagePixels1 = image1.getPixelsRef();
    //ofPixels imagePixels2 = image2.getPixelsRef();
    ofPixels backgroundPixels = backgroundImag.getPixelsRef();
    vector<float> differences;
    
    ofPixels difference;
    
    //this unsigned char should be unnecessary - I would have thought - can't you just address the pixel locations in ofPixels directly? 
    unsigned char * thesePixels = new unsigned char[imagePixels1.getWidth()*imagePixels1.getHeight()*3];
    
    for(int i=0;i<imagePixels1.size();i++){
        thesePixels[i]=0;
    }
    
    int x=0;
    int y=0;
    
    int chooseComparison=1;
    
    //comparison here to find out how close each color is to pure RED / GREEN / BLUE
    
    if(chooseComparison==1){
        //for each pixel...
        float _maxPossibleDistanceToCentre=ofDist(0,0,imagePixels1.getWidth()/2, imagePixels1.getHeight()/2);
        
        for(int i=0;i<imagePixels1.size();i+=3){
            
            ofColor imageColor1 = imagePixels1.getColor(x, y);
            //ofColor colourImage2 = imagePixels2.getColor(x, y);
            
            float _distanceToCentre=ofDist(imagePixels1.getWidth()/2, imagePixels1.getHeight()/2, x, y);
            float _presumedBrightness=ofMap(sqrt(_maxPossibleDistanceToCentre)-sqrt(_distanceToCentre), 0,  sqrt(_maxPossibleDistanceToCentre), 0, 255);
            
            //int thisDiff=abs(imageColor1.getHue());
            //int thisDiff=abs(imageColor1.getBrightness());
            //int thisDiff=abs(imageColor1.getBrightness()-_presumedBrightness);
            
            int thisDiff=255-abs(imageColor1.getHue());
            //int thisDiff=abs(imageColor1.getLightness());
            
            //cout<<thisDiff<< " thisDiff "<<endl;
            
            //red hue: 0
            //green hue: 120 
            //blue hue: 240
            
            float multiplier=5.0;
            
            differences.push_back(multiplier* thisDiff);
            
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
    
    //difference.setFromPixels(thesePixels,imagePixels1.getWidth(),imagePixels1.getHeight(), 3);
    return differences;
}
