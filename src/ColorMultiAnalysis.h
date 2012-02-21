#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

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
    
    int     _run_cnt, _save_cnt, _fade_cnt, _anim_cnt;
    float   c, _frame_cnt, _frame_cnt_max, _anim_cnt_max;

    bool _show_image, _image_shown;
    ofImage image1;
    ofImage image2; 

};
