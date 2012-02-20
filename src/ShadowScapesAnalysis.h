/* */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Timer.h"

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
    void draw();
    
    void save_cb(Poco::Timer& timer);
    
protected:
    
    bool    _RUN_DONE;
    float     _line;  
    float    _speed; // pix per second
    float    _scanLineWidth; // pix per second
    float     _step;
    shadow_type _dir;
    int     _run_cnt, _save_cnt, _anim_cnt;
    float   c, _frame_cnt, _frame_cnt_max, _anim_cnt_max;
    
};