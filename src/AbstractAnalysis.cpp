/*
 - copyright (c) 2011 Copenhagen Institute of Interaction Design (CIID)
 - all rights reserved.
 
 + redistribution and use in source and binary forms, with or without
 + modification, are permitted provided that the following conditions
 + are met:
 +  > redistributions of source code must retain the above copyright
 +    notice, this list of conditions and the following disclaimer.
 +  > redistributions in binary form must reproduce the above copyright
 +    notice, this list of conditions and the following disclaimer in
 +    the documentation and/or other materials provided with the
 +    distribution.
 
 + THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 + "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 + LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 + FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 + COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 + INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 + BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 + OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 + AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 + OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 + OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 + SUCH DAMAGE.
 
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ author: dviid
 ~ contact: dviid@labs.ciid.dk 
 */

#include "AbstractAnalysis.h"
#include "RefractiveIndex.h"
#include "ofxFileHelper.h"

// this is the main threaded loop for a given analysis
void AbstractAnalysis::do_synthesize() {
    _state = STATE_ACQUIRING;
    acquire();
    _state = STATE_SYNTHESISING;
    synthesise();
    _state = STATE_DISPLAY_RESULTS;
    ofNotifyEvent(_synthesize_cb, _name);
}

void AbstractAnalysis::create_dir()
{
    // HERE IS WHERE WE SETUP THE DIRECTORY FOR ALL THE SAVED IMAGES
    
    //FOR WINDOWS i HAVE HAD TO REPLACE SPACES WITH UNDERSCORES AND REDUCE THE LENGTH OF THE FOLDER NAME
    time_t rawtime;
    struct tm * timeinfo;
    
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    string time = asctime(timeinfo);
    string replaceTime = "";
    
    //DON'T INCLUDE THE DAY IN WORDS EG 'TUE' OR THE YEAR EG 2012 THIS MAKES THE DIRECTORY NAME TOO LONG AND CAUSES DIR CREATION TO FAIL
    for(int i=4;i<time.size()-4;i++){
        if(time.at(i)==' '||time.at(i)==':'){
            replaceTime+="_";
        }
        else{
            replaceTime+=time.at(i);
        }
    }

    ofxFileHelper fileHelper;
    _whole_file_path = ANALYSIS_PATH + RefractiveIndex::_location + "/" + _name + "/"+replaceTime ;
    //cout << "_whole_file_path = " << _whole_file_path << endl;
    
    if(!fileHelper.doesDirectoryExist(_whole_file_path)){
        fileHelper.makeDirectory(ANALYSIS_PATH);
        fileHelper.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location);
        fileHelper.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location+"/"+_name);
        fileHelper.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location+"/"+_name+"/"+replaceTime);
    }
    
    //////////////////////////////END DIRECTORY CREATION //////////////////////////////////////////////////    
}

