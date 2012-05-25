/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

#include "ofxOpenCv.h"


class ColorSingleAnalysis : public AbstractAnalysis
{
public:
    ColorSingleAnalysis(): AbstractAnalysis("COLOR_SINGLE"){;}
    virtual ~ColorSingleAnalysis(){;}
    
public:
    
    void setup(int camWidth, int camHeight);
    void acquire();
    void synthesise();   
    void displayresults();
    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:

    bool    _RUN_DONE;
    string  fileNameTag;
    float   r,g,b;
    
    int     _run_cnt, _save_cnt, _fade_cnt, _synth_save_cnt, _anim_cnt;
    float   c, _frame_cnt, _frame_cnt_max, _anim_cnt_max;
    
    bool _show_image, _image_shown;

    ofImage         image1;
    ofImage         image2; 
    
    int algo;
    int scale;
    int draw_style;
    float line_width;    

};
