#include "CamNoiseAnalysis.h"
#include "ofMain.h"

#include "Poco/Timer.h"
#include "Poco/Thread.h"

#include "RefractiveIndex.h"

using Poco::Timer;
using Poco::TimerCallback;
using Poco::Thread;

#define NUMBER_RUNS     1
#define ACQUIRE_TIME    20

void CamNoiseAnalysis::setup(int camWidth, int camHeight)
{
    AbstractAnalysis::setup(camWidth, camHeight);
    
    NUM_RUN = RefractiveIndex::XML.getValue("config:analysis_NUM_RUN:NUM_RUN_camnoise", NUMBER_RUNS);
    cout << "NUM_RUN CamNoiseAnalysis " << NUM_RUN << endl;
    //NUM_RUN = 5;

    
    //flag for main sketch
    meshIsComplete=false;
    _gotFirstImage=false;
    _mesh_size_multiplier   = 8;
    vertexSubsampling       = 1;
    chooseColour            = 6;
    multiplier              = 4.0;
    
    ofSetLineWidth(5.0f);
    glPointSize(5.0f);
    
    //blendMode = OF_BLENDMODE_ADD;
    //blendMode = OF_BLENDMODE_MULTIPLY;
    //blendMode = OF_BLENDMODE_SUBTRACT;
    //blendMode = OF_BLENDMODE_ALPHA;
    blendMode = OF_BLENDMODE_SCREEN;
    
    //meshMode = OF_PRIMITIVE_TRIANGLES;
    //meshMode = OF_PRIMITIVE_TRIANGLE_STRIP;
    //meshMode = OF_PRIMITIVE_TRIANGLE_FAN;
   // meshMode = OF_PRIMITIVE_LINES;
    //meshMode = OF_PRIMITIVE_LINE_STRIP;
    meshMode = OF_PRIMITIVE_POINTS;
    
    ///setup light    
    ofEnableLighting();    
    GLfloat light_ambient[] = {0.5, 0.5, 0.5, 0.5 };  
    GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 0.5 };  
    GLfloat light_specular[] = { 0.5, 0.5, 0.5, 0.5 };  
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };  
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);  
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);  
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  
    
    glEnable(GL_LIGHT0);  
    
    GLfloat light_ambient1[] = { 0.5, 0.5, 0.5, 0.5 };  
    GLfloat light_diffuse1[] = { 0.5, 0.5, 0.5, 0.5 };  
    GLfloat light_specular1[] = { 0.5, 0.5, 0.5, 0.5 };  
    GLfloat light_position1[] = { -1.0, 1.0, 1.0, 0.0 };  
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);  
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);  
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);  
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);  
    
    glEnable(GL_LIGHT1);     
    glPointSize(9.0f);
    glEnable(GL_POINT_SMOOTH);
    glCullFace(GL_BACK);
    
    int acq_run_time;   // 10 seconds of acquiring per run
    acq_run_time = RefractiveIndex::XML.getValue("config:analysis_time:acquiretime_camnoise", ACQUIRE_TIME);
    cout << "ACQUIRE_TIME CamNoiseAnalysis " << acq_run_time << endl;


    //int acq_run_time = 20;   // 20 seconds of acquiring per run

    DELTA_T_SAVE = 2*(10*acq_run_time/2);       // for 20 seconds, we want this to be around 200 files
                                                // or 5 times per second = every 200 ms

    _frame_cnt_max = acq_run_time*ofGetFrameRate();  // e.g.: 30 frames per second * 20 seconds = 600 frames

    _frame_cnt = 0;
    _run_cnt = 0;
    _synth_save_cnt = 0;

    c = 0;

    int anim_time = 10;   // 10 seconds
    _anim_cnt_max = anim_time*ofGetFrameRate();  // e.g.: 30 frames per second = 150 frames

    //create_dir_allocate_images();

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


void CamNoiseAnalysis::acquire()
{

    Timer* save_timer;
    TimerCallback<CamNoiseAnalysis> save_callback(*this, &CamNoiseAnalysis::save_cb);

    _frame_cnt = 0; _save_cnt = 0; _anim_cnt = 0, _synth_save_cnt = 0;
    _run_cnt++;
    _RUN_DONE = false;
    create_dir_allocate_images();

    // RUN ROUTINE
    // for(int i = 0; i < NUM_RUN; i++) {
    // _run_cnt = i;
    //cout << "RUN NUM = " << i;

    save_timer = new Timer(0, DELTA_T_SAVE); // timing interval for saving files
    save_timer->start(save_callback);

        while(!_RUN_DONE && _state != STATE_STOP)
            Thread::sleep(3);

    save_timer->stop();


   // }
}

void CamNoiseAnalysis::synthesise()
{
    //cout << "CamNoiseAnalysis::saving synthesis...\n";
    if(_state == STATE_STOP) return;
    
    _returnAveragePixelValues();
    vector<vector<float> > averagePixelBrightnessesForEachLevel = averagePixelValuesForAllLevels;
    float lightLevel=0;
    float pLightLevel=0;
    cout<<_listOfLightLevels.size()<<" "<<averagePixelBrightnessesForEachLevel.size()<<" these should be the same";
    
    for(float i=1;i<_saved_filenames_analysis.size()-1;i++){
        vector<string> fileNameParts= ofSplitString(_saved_filenames_analysis[i], "_");
        
        lightLevel = ofToFloat(fileNameParts[fileNameParts.size()-2]);
        //cout << "CamNoiseAnalysis::synthesis FOR LOOP...\n";
        
        //find which list of average pixel values is for THIS light level
        int _averagePixelVectorIndex;
        for (int j=0; j<_listOfLightLevels.size(); j+=3) {
            if(lightLevel==_listOfLightLevels[j]){
                _averagePixelVectorIndex=j;
            }
        }
        //cout << "_saved_filenames_analysis[i]" << _saved_filenames_analysis[i] << endl;

        if(_state == STATE_STOP) return;

        if(!image1.loadImage(_saved_filenames_analysis[i])){
            //couldn't load image
            cout << "didn't load image" << endl;
        }

        if(image1.loadImage(_saved_filenames_analysis[i])){
            //    cout << "LOADED image1!!!" << endl;
            if(image5.loadImage(_saved_filenames_analysis[i+1])){
                
                ///////////////////////// PROCESS THE SAVED CAMERA IMAGES OF SHIT TO THE IMAGES //////////////////////////
                
             /*   cvColorImage1.setFromPixels(image1.getPixels(), image1.width, image1.height);
                cvColorImage2.setFromPixels(image5.getPixels(), image5.width, image5.height);
                
                cvColorImage1.convertToGrayscalePlanarImage(cvGrayImage1, 1);
                cvColorImage2.convertToGrayscalePlanarImage(cvGrayImage2, 1);
                
                //cvGrayDiff1.absDiff(cvGrayImage2, cvGrayImage1);
                //cvGrayDiff1.erode();
                
                cvGrayImage1.dilate();
                cvGrayImage1.blur(5);
                cvGrayImage1.contrastStretch();
                
                /////////////////////////////////// SAVE TO DISK IN THE SYNTHESIS FOLDER ////////////////////////////////
                cvColorImage1.setFromGrayscalePlanarImages(cvGrayImage1, cvGrayImage1, cvGrayImage1);
                //cvColorImage2.setFromGrayscalePlanarImages(cvGrayImage2, cvGrayImage2, cvGrayImage2);
                
                //cvPyrMeanShiftFiltering(cvColorImage1.getCvImage(), cvColorImage1.getCvImage(), 1, 1);
                //cvPyrMeanShiftFiltering(cvColorImage2.getCvImage(), cvColorImage2.getCvImage(), 1, 1);
                
                //cvFloatImage1 = cvColorImage1;
                //cvGrayImage1 = cvColorImage1;
                
                
                //cvSmooth( cvColorImage1.getCvImage(), cvColorImage1.getCvImage(), CV_GAUSSIAN, 5, 5);
                //cvSmooth( cvColorImage2.getCvImage(), cvColorImage2.getCvImage(), CV_GAUSSIAN, 5, 5);
                
                //cvCanny(cvGrayImage1.getCvImage(), cvGrayImage1.getCvImage(), 100, 100, 3);
                //cvLaplace(cvGrayImage1.getCvImage(), cvGrayImage1.getCvImage(), 0);
                
                //cvGrayImage1 = cvCreateImage(cvSize(image1.width, image1.height),IPL_DEPTH_16S,1);
                //cvSobel(cvGrayImage1.getCvImage(), cvGrayImage1.getCvImage(), 0, 1, 3);
                
                // convert the CV image 
                image1.setFromPixels(cvColorImage1.getPixelsRef()); 
                image5.setFromPixels(cvColorImage2.getPixelsRef());  
                */
                ///////////////////////// PROCESS THE SAVED CAMERA IMAGES OF SHIT TO THE IMAGES //////////////////////////
                if(!_gotFirstImage){
                    cout<<"background image is"<< _saved_filenames_analysis[i]<<endl;
                    _background=image1;
                    _gotFirstImage=true;
                }
                
                //subtract background begin///////////////
                
                ofPixels imagePixels1       = image1.getPixelsRef();
                ofPixels imagePixels2       = image5.getPixelsRef();
                ofPixels backgroundPixels   = _background.getPixelsRef();
                
                //DIFFERENCING SUBSEQUENT IMAGES
                /*
                 for(int i=0;i<imagePixels1.size();i++){
                 //unsigned char val=imagePixels1[i];
                 // cout<<(int)backgroundPixels[i]<< " thesePixels[i] "<<(int)imagePixels1[i]<<endl;
                 if(imagePixels1[i]-imagePixels2[i]>0){
                 imagePixels1[i]-=imagePixels2[i];
                 }
                 else{
                 imagePixels1[i]=0;
                 }
                 }
                 */ 
                
                //DIFFERENCING THE BACKGROUND
                /*
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
                 */
                
                //update the images with their new background subtracted selves
                image1.setFromPixels(imagePixels1);
                
                //flag the main app that we aren't read yet
                meshIsComplete=false;
                
                //make a mesh - this mesh will be drawn in the main app
                setMeshFromPixels(_returnDepthsAtEachPixel(image1, averagePixelBrightnessesForEachLevel[_averagePixelVectorIndex], _background), image1, image1, aMesh);
                
                //setMeshFromPixels(_returnDepthsAtEachPixel(image1, image1, _background), image1, image1, aMesh);
                
                /////////////////////////////////// SAVE TO DISK IN THE SYNTHESIS FOLDER ////////////////////////////////
                //string file_name;
                
                //with jpgs this was refusing to save out
                meshFileName = _whole_file_path_synthesis+"/"+ofToString(_synth_save_cnt, 2)+"_CamNoiseAnalysis_"+ofToString(_run_cnt,2)+".png";
                _saved_filenames_synthesis.push_back(meshFileName);
                
                //file_name = ofToString(_synth_save_cnt, 2)+"_ColorMultiAnalysis_"+ofToString(_run_cnt,2)+".jpg";
                
                //flag that we are finished  
                meshIsComplete=true;
                _synth_save_cnt++;
                
            }
        } else {
            cout<<"couldn't load image from "<<_saved_filenames_analysis[i]<<endl;
        }

    }

    // _saved_filenames_synthesis has processed all the files in the analysis images folder
    while(!_RUN_DONE && _state != STATE_STOP)
        Thread::sleep(3);
}

void CamNoiseAnalysis::displayresults()
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


// this runs at frame rate = 33 ms for 30 FPS
void CamNoiseAnalysis::draw()
{
    switch (_state) {
        case STATE_ACQUIRING:
        {
            /// *** TODO  *** ///
            // still need to deal with latency frames here - i.e.: there are frames
            /// *** TODO  *** ///

            if (_frame_cnt < _frame_cnt_max)
            {

                ofEnableAlphaBlending();
                ofColor aColour;

                int _fade_in_frames = _frame_cnt_max/10;
                float _number_of_grey_levels=5;

                float _frames_per_level = _frame_cnt_max / _number_of_grey_levels;
                ofColor someColor;

                 /*
                if (_frame_cnt < _fade_in_frames) {
                    aColour.set(255, 255, 255, ofMap(_frame_cnt, 0, _fade_in_frames, 0, 255));
                    ofSetColor(aColour);
                    ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    //cout <<  "FADE IN STROBE TIME " << endl;
                }
                */
                //if (_frame_cnt >= _fade_in_frames && _frame_cnt < (_frame_cnt_max)){


                    for(int i=1;i<_number_of_grey_levels;i++){
                        if ( _frame_cnt >= _frames_per_level*(i-1) && +_frame_cnt < _frames_per_level * (i+1) ) {
                            //set colour to current grey level
                            c=255-( 255.0 * ( i /_number_of_grey_levels));
                            //cout << "c: " << c << endl;
                            someColor.set(c);
                        }

                        ofSetColor(someColor);
                        ofRect(0, 0, ofGetWidth(), ofGetHeight());
                    }

                //}
                /*
                if (_frame_cnt >= (_frame_cnt_max-_fade_in_frames) && _frame_cnt < _frame_cnt_max) {
                        aColour.set(0, 0, 0, ofMap(_frame_cnt-(_frame_cnt_max-_fade_in_frames), 0, _fade_in_frames, 0, 255));
                        ofSetColor(aColour);
                        ofRect(0, 0, ofGetWidth(), ofGetHeight());
                        // cout <<  "FADE OUT STROBE TIME " << endl;
                }
                */
                ofDisableAlphaBlending();


            } else {
               // _state = STATE_SYNTHESISING;
                _RUN_DONE = true;
            }

            _frame_cnt++;
            break;

        }

        case STATE_SYNTHESISING:
        {
            // display animation of something while the synthesis in on-going...

            //cout << "CamNoiseAnalysis = STATE_SYNTHESISING...\n";

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
                    //cout << "CamNoiseAnalysis STATE_SYNTHESIZING = FADING IN ANIMATION...\n";

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
void CamNoiseAnalysis::save_cb(Timer& timer)
{
    _save_cnt++;

    string file_name = ofToString(_save_cnt,2)+"_"+ ofToString(c,2)+"_"+ofToString(_run_cnt,2)+".jpg";

    saveImageAnalysis(file_name);
}



void CamNoiseAnalysis::setMeshFromPixels(vector<float> sPixels, ofImage currentFirstImage, ofImage currentSecondImage, ofMesh & mesh){
    int x=0;
    int y=0;
    
    //to do 
    
    
    //get rid of all previous vectors and colours
    mesh.clear();
    mesh.setMode(meshMode);
    
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
    
    if(chooseColour==2){
        
        for(int i=0;i<sPixels.size();i++){
            ofColor currentSecondImageColor = currentSecondImage.getColor(x, y+1);
            
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*(y+1),- sPixels[ (currentSecondImage.getWidth()*(y+1))+x    ]   ));
            
            currentSecondImageColor = currentSecondImage.getColor(x, y);
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*y,- sPixels[(currentSecondImage.getWidth()*(y))+x ]  ));
            
            currentSecondImageColor = currentSecondImage.getColor(x+1, y+1);
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+1),_mesh_size_multiplier*(y+1),- sPixels[(currentSecondImage.getWidth()*(y+1))+x+1 ]  ));
            
            currentSecondImageColor =  currentSecondImage.getColor(x+1, y+1);
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+1),_mesh_size_multiplier*(y+1),- sPixels[(currentSecondImage.getWidth()*(y+1))+x+1]   ));
            
            currentSecondImageColor =  currentSecondImage.getColor(x, y);
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*y,- sPixels[(currentSecondImage.getWidth()*(y))+x ]  ));
            
            currentSecondImageColor =  currentSecondImage.getColor(x+1, y);
            mesh.addColor(  currentSecondImageColor.getBrightness());
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
    
    if(chooseColour==3){
        
        for(int i=0;i<sPixels.size();i++){
            ofColor currentSecondImageColor;
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            mesh.addColor(  currentSecondImageColor);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*(y+1),- sPixels[ (currentSecondImage.getWidth()*(y+1))+x    ]   ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            
            mesh.addColor(  currentSecondImageColor);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*y,- sPixels[(currentSecondImage.getWidth()*(y))+x ]  ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            mesh.addColor(  currentSecondImageColor);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+1),_mesh_size_multiplier*(y+1),- sPixels[(currentSecondImage.getWidth()*(y+1))+x+1 ]  ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            mesh.addColor(  currentSecondImageColor);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+1),_mesh_size_multiplier*(y+1),- sPixels[(currentSecondImage.getWidth()*(y+1))+x+1]   ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            mesh.addColor(  currentSecondImageColor);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*x,_mesh_size_multiplier*y,- sPixels[(currentSecondImage.getWidth()*(y))+x ]  ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            mesh.addColor(  currentSecondImageColor);
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
    
    if(chooseColour==4){
        
        for(int i=0;i<sPixels.size();i++){
            ofColor currentSecondImageColor;
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            int randomJitter = ofRandom(0,5);
            
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+randomJitter),_mesh_size_multiplier*(y+randomJitter+1),- sPixels[ (currentSecondImage.getWidth()*(y+randomJitter+1))+(x+randomJitter)    ]   ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            randomJitter = ofRandom(0,5);
            
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+randomJitter),_mesh_size_multiplier*(y+randomJitter),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)))+(x+randomJitter) ]  ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            randomJitter = ofRandom(0,5);
            
            
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*((x+randomJitter)+1),_mesh_size_multiplier*((y+randomJitter)+1),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)+1))+(x+randomJitter)+1 ]  ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            randomJitter = ofRandom(0,5);
            
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*((x+randomJitter)+1),_mesh_size_multiplier*((y+randomJitter)+1),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)+1))+(x+randomJitter)+1]   ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            randomJitter = ofRandom(0,5);
            
            
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x+randomJitter),_mesh_size_multiplier*(y+randomJitter),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)))+(x+randomJitter) ]  ));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            randomJitter = ofRandom(0,5);
            
            mesh.addColor(  currentSecondImageColor.getBrightness());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*((x+randomJitter)+1),_mesh_size_multiplier*(y+randomJitter),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)))+(x+randomJitter) +1 ]));
            
            ofSeedRandom();
            
            currentSecondImageColor.r = ofRandom(0,255);
            currentSecondImageColor.g = ofRandom(0,255);
            currentSecondImageColor.b = ofRandom(0,255);
            
            randomJitter = ofRandom(0,5);
            
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
    
    if(chooseColour==5){
        
        for(int i=0;i<sPixels.size();i++){
            
            int randomJitter2 = 0;
            int randomJitter = 0;
            
            ofColor currentSecondImageColor;
            ofColor currentSecondImageBW;
            
            currentSecondImageColor = currentSecondImage.getColor(x, y+1);
            currentSecondImageBW.r = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.g = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.b = currentSecondImageColor.getBrightness()+randomJitter2;
            
            mesh.addColor( currentSecondImageBW);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x),_mesh_size_multiplier*((y)+1),- sPixels[ (currentSecondImage.getWidth()*(y+randomJitter+1))+(x+randomJitter) ] ));
            
            currentSecondImageColor = currentSecondImage.getColor(x, y);
            currentSecondImageBW.r = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.g = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.b = currentSecondImageColor.getBrightness()+randomJitter2;
            
            mesh.addColor(currentSecondImageBW);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x),_mesh_size_multiplier*(y),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)))+(x+randomJitter) ] ));
            
            currentSecondImageColor = currentSecondImage.getColor(x+1, y+1);
            currentSecondImageBW.r = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.g = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.b = currentSecondImageColor.getBrightness()+randomJitter2;
            
            mesh.addColor(currentSecondImageBW);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*((x)+1),_mesh_size_multiplier*((y)+1),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)+1))+(x+randomJitter)+1 ]  ));
            
            currentSecondImageColor = currentSecondImage.getColor(x+1, y+1);            
            currentSecondImageBW.r = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.g = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.b = currentSecondImageColor.getBrightness()+randomJitter2;
            
            mesh.addColor( currentSecondImageBW);
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*((x)+1),_mesh_size_multiplier*((y)+1),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)+1))+(x+randomJitter)+1]   ));
            
            currentSecondImageColor =  currentSecondImage.getColor(x, y);            
            currentSecondImageBW.r = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.g = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.b = currentSecondImageColor.getBrightness()+randomJitter2;
            
            mesh.addColor(  currentSecondImageBW.clamp());
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*(x),_mesh_size_multiplier*(y),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)))+(x+randomJitter) ]  ));
            
            currentSecondImageColor = currentSecondImage.getColor(x+1, y);
            currentSecondImageBW.r = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.g = currentSecondImageColor.getBrightness()+randomJitter2;
            currentSecondImageBW.b = currentSecondImageColor.getBrightness()+randomJitter2;
            
            mesh.addColor(  currentSecondImageBW.clamp() ); 
            mesh.addVertex(ofVec3f(_mesh_size_multiplier*((x)+1),_mesh_size_multiplier*(y),- sPixels[(currentSecondImage.getWidth()*((y+randomJitter)))+(x+randomJitter)+1 ]));
            
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
    if(chooseColour==6){
        
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
            
            x++;
            if(x>=currentSecondImage.getWidth()-1){
                x=0;
                y++;
                //something is going badly wrong with my maths for me to need this HELP TODO fix this - why am I running over the end of the vector?
                if(y>=currentSecondImage.getHeight()-1){
                    break;
                }
            }
        }
    }

}


vector<float> CamNoiseAnalysis::_returnDepthsAtEachPixel(ofImage &image1, vector<float> averageDepths, ofImage &backgroundImag){
    
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
    int inc=0;
    if(chooseComparison==1){
        //for each pixel...
        float _maxPossibleDistanceToCentre=ofDist(0,0,imagePixels1.getWidth()/2, imagePixels1.getHeight()/2);
        
        for(int i=0;i<imagePixels1.size();i+=3){
            
            ofColor imageColor1 = imagePixels1.getColor(x, y);
            
            ofColor imageColor2 = imagePixels1.getColor(x+1, y+1);
            
            ofColor imageColor3 = imagePixels1.getColor(x+2, y+2);
            
            //ofColor colourImage2 = imagePixels2.getColor(x, y);
            
            //int thisDiff=abs(imageColor1.getHue());
            //int thisDiff=abs(imageColor1.getBrightness());
            //int thisDiff=abs(imageColor1.getBrightness()-_presumedBrightness);
            if(i<100){
                
              //  cout<<" average: "<<averageDepths[inc]<<" actual: "<<imageColor1.getBrightness()<<" ";
            }
           // int thisDiff=imageColor1.getBrightness();
            int thisDiff=-(imageColor1.getBrightness() - averageDepths[inc] );
            //int thisDiff=abs(imageColor1.getLightness());
            //int thisDiff=-abs(imageColor1.r);
            
            //cout<<thisDiff<< " thisDiff "<<endl;
            
            //red hue: 0
            //green hue: 120 
            //blue hue: 240
            
            differences.push_back(multiplier * thisDiff);
            
            thesePixels[i]=thisDiff;
            thesePixels[i+1]=thisDiff;
            thesePixels[i+2]=thisDiff;
            x++;
            
            if(x>=imagePixels1.getWidth()){
                x=0;
                y++;
            }
            inc++;
        }
    }
    
    //difference.setFromPixels(thesePixels,imagePixels1.getWidth(),imagePixels1.getHeight(), 3);
    return differences;
}


void CamNoiseAnalysis:: _returnAveragePixelValues(){
    
    
    //second number in 15_204.00_2.jpg is the light level
    
    float lightLevel=0;
    float pLightLevel=0;
    float numberOfImagesActuallyLoaded=0;
    vector<float> averagePixelValuesForOneLevel;

    for (int j=0; j<RefractiveIndex::_vid_w*RefractiveIndex::_vid_h; j++) {
        averagePixelValuesForOneLevel.push_back(0.0);
    }
    for(float i=1;i<_saved_filenames_analysis.size()-1;i++){
        
        
        if(_state == STATE_STOP) return;
        
        if(!image1.loadImage(_saved_filenames_analysis[i])){
            //couldn't load image
            cout << "didn't load image" << endl;
        }
        
        if(image1.loadImage(_saved_filenames_analysis[i])){

            //split file name by under score
            vector<string> fileNameParts= ofSplitString(_saved_filenames_analysis[i], "_");
            
            lightLevel = ofToFloat(fileNameParts[fileNameParts.size()-2]);
           // cout<<" lightLevel "<<lightLevel;

            ofPixels image1Pixels=image1.getPixelsRef();
            
            int x=0;
            int y=0;
            int inc=0;
            for (int j=0; j<image1Pixels.size(); j+=3) {
                ofColor color=image1Pixels.getColor(x, y);
                averagePixelValuesForOneLevel[inc]+= color.getBrightness();
                x++;
                inc++;
                if(x>=image1Pixels.getWidth()){ 
                    x=0;
                    y++;
                }
            }
           // _listOfLightLevels}
            numberOfImagesActuallyLoaded++;
            
            //if this is a new light level advance
            if (lightLevel!=pLightLevel) {
                
                ///check that this light level is not already in our list
                
                
             _listOfLightLevels.push_back(lightLevel);
                
                for (int j=0; j<averagePixelValuesForOneLevel.size(); j++) {
                    averagePixelValuesForOneLevel[j]/=numberOfImagesActuallyLoaded;
                    //cout<<" "<<averagePixelValuesForOneLevel[j]<<" ";
                }
                averagePixelValuesForAllLevels.push_back(averagePixelValuesForOneLevel);
                for (int j=0; j<averagePixelValuesForOneLevel.size(); j++) {
                    averagePixelValuesForOneLevel[j]=0;
                }

                pLightLevel=lightLevel;
                numberOfImagesActuallyLoaded=0;

            }

        }
    }
    
    for(int j=0;j<_listOfLightLevels.size();j++){
        //cout<<_listOfLightLevels[j]<<" lightlevel ";
    }
    
}
