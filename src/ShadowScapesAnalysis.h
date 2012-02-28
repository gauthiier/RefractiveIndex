/* */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

#include "ofxOpenCv.h"

enum shadow_type {
    H, V, D,
};

class ShadowScapesAnalysis : public AbstractAnalysis
{
public:
    ShadowScapesAnalysis(shadow_type dir): AbstractAnalysis("SHADOWSCAPE"), _dir(dir){;}
    ShadowScapesAnalysis(): AbstractAnalysis("SHADOWSCAPE"), _dir(H){;}
    virtual ~ShadowScapesAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void acquire();
    void synthesise();   
    void displayresults();

    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:
    
    bool    _RUN_DONE;
    float     _line;  
    float    _speed; // pix per second
    float    _scanLineWidth; // pix per second
    float     _step;
    shadow_type _dir;
    int     _run_cnt, _save_cnt, _synth_save_cnt, _anim_cnt;
    float   c, _frame_cnt, _frame_cnt_max, _anim_cnt_max;
    
    bool _show_image, _image_shown;
    ofImage         image1;
    ofImage         image2; 
    ofImage         image3;
    ofImage         image4;
    ofImage         image5; 
    ofImage         image6; 

    ofxCvColorImage         cvColorImage1;
    ofxCvColorImage         cvColorImage2;
    ofxCvColorImage         cvColorImage3;
    ofxCvColorImage         cvColorImage4;
    ofxCvColorImage         cvColorImage5;
    ofxCvColorImage         cvColorImage6;
    
    ofxCvGrayscaleImage 	cvGrayDiff1;
    ofxCvGrayscaleImage 	cvGrayDiff2;
    
    ofxCvGrayscaleImage 	cvGrayImage1;
    ofxCvGrayscaleImage 	cvGrayImage2;
    ofxCvGrayscaleImage 	cvGrayImage3;
    ofxCvGrayscaleImage 	cvGrayImage4;
    
    ofxCvContourFinder      cvContourFinder1;
    
    //this is the temporary container to allow us to convert and save out greyscale images
    ofxCvColorImage         cvConvertorImage;    

};