
#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

#include "rfiCvContourFinder.h"

#include "ofxOpenCv.h"



class RelaxRateAnalysis : public AbstractAnalysis
{
public:
    RelaxRateAnalysis(): AbstractAnalysis("RELAXRATE"){;}
    virtual ~RelaxRateAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void acquire();
    void synthesise(); 
    void displayresults();
    void cleanup();

    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:
    
    void clearcfindervect();
    void clearcfindervectdisplay();
    
    bool    _RUN_DONE;
    float     _flip, _level;
    int     _run_cnt, _save_cnt, _synth_save_cnt, _anim_cnt;
    float   c, _frame_cnt, _frame_cnt_max, _anim_cnt_max;
    
    int     _threshold;
    int     _maxblobs;
    
    bool _show_image, _image_shown;
    
    ofImage                 image1;    
    ofxCvColorImage         cvColorImage1;    
    ofxCvGrayscaleImage 	cvGrayDiff1;
    
    vector<rfiCvContourFinder*>      cvContourFinderVect;
    vector<rfiCvContourFinder*>      cvContourFinderVectDisplay;

};
