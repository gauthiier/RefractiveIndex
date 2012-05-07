#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

#include "ofxOpenCv.h"


class ColorMultiAnalysis : public AbstractAnalysis
{
public:
    ColorMultiAnalysis(): AbstractAnalysis("COLOR_MULTI"){;}
    virtual ~ColorMultiAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void acquire();
    void synthesise(); 
    void displayresults();
    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:
    
    bool    _RUN_DONE;
    
    int     _run_cnt, _save_cnt, _fade_cnt, _synth_save_cnt, _anim_cnt;
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
    
    bool _gotFirstImage;
    ofImage _background;


};
