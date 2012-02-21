/*
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#pragma once

#include "AbstractAnalysis.h"

#include "Poco/Thread.h"
#include "Poco/RunnableAdapter.h"

using Poco::Thread;
using Poco::RunnableAdapter;

class AnalysisAdaptor
{
public:
    AnalysisAdaptor(AbstractAnalysis* analysis) : _analysis(analysis) {;}
    virtual ~AnalysisAdaptor(){ delete _runnable; }
    
    void start()
    {
        _stopping = false;
        _runnable = new RunnableAdapter<AbstractAnalysis>(*_analysis, &AbstractAnalysis::do_synthesize);
        _worker.start(*_runnable);
    }
    
    void stop() 
    {
        if(_stopping) return;
        _stopping = true;
        _analysis->_state = STATE_STOP;
        _worker.join();        
    }
    
protected:
    AbstractAnalysis*                   _analysis;
    Thread                              _worker;    //   
    RunnableAdapter<AbstractAnalysis>*  _runnable;  
    bool                                _stopping;
};

