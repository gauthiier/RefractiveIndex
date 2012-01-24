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
        _runnable = new RunnableAdapter<AbstractAnalysis>(*_analysis, &AbstractAnalysis::synthesize);
        _worker.start(*_runnable);
    }
    
    void stop() 
    {
        _analysis->_state = STATE_STOP;
        _worker.join();
    }
    
protected:
    AbstractAnalysis*                   _analysis;
    Thread                              _worker;    //   
    RunnableAdapter<AbstractAnalysis>*  _runnable;    
};

