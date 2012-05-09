/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

#include "ofxOpenCv.h"


class ShapeFromShadingAnalysis : public AbstractAnalysis
{
public:
    ShapeFromShadingAnalysis(): AbstractAnalysis("SHAPEFROMSHADING"){;}
    virtual ~ShapeFromShadingAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void acquire();
    void synthesise();    
    void displayresults();
    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:
    
    string quad;
    string fileNameQuad;
    
    bool    _RUN_DONE;
    float     _flip, _level;
    int _animation_cnt1;
    int _animation_cnt2;
    int _animation_cnt3;
    int _animation_cnt4;
    int _animation_cnt5;
    int _animation_cnt6;
    int _animation_cnt7;
    int _animation_cnt8;
    int _animation_cnt9;
    int _animation_cnt10;
    int _animation_cnt11;
    int _animation_cnt12;
    int _animation_cnt13;
    int _animation_cnt14;
    int _animation_cnt15;
    int _animation_cnt16;
    int _animation_reset;  // this reset part didn't get working - so using 16 different counters! yay! 
   
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
    
    
    //mesh making function
    void setMeshFromPixels(vector<float> sPixels, ofImage currentFirstImage, ofImage currentSecondImage, ofMesh & mesh);
    
    //depth map function
    vector<float> _returnDepthsAtEachPixel(ofImage &image1, ofImage &image2, ofImage &backgroundImage);
    
    int vertexSubsampling;
    int chooseColour;
    ofPrimitiveMode meshMode;
    ofBlendMode blendMode;
    float multiplier;
    
    bool _gotFirstImage;
    ofImage _background;
};
