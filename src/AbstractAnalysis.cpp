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

void AbstractAnalysis::do_synthesize() {
    // acquire images + pattern on display
    pattern_acquire();
    // acquire notify obeservers
    ofNotifyEvent(_acquire_cb, _name);
    // analysis and synthesis of acquired images
    synthesize();
    // synthesize notify obeservers
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
    
    ofDirectory dir;
    
    _whole_file_path= string(ANALYSIS_PATH)+ RefractiveIndex::_location+"/"+ _name+"/"+replaceTime ;
    //directories have to be created one level at a time hence repeated calls
    if(!dir.doesDirectoryExist(_whole_file_path)){
        dir.createDirectory(string(ANALYSIS_PATH)+RefractiveIndex::_location+"/", true,false);
        dir.createDirectory(string(ANALYSIS_PATH)+RefractiveIndex::_location+"/"+ _name+"/", true,false);
        dir.createDirectory(string(ANALYSIS_PATH)+RefractiveIndex::_location+"/"+ _name+"/"+replaceTime+"/", true,false);
    }
    
    //////////////////////////////END DIRECTORY CREATION //////////////////////////////////////////////////    
}

