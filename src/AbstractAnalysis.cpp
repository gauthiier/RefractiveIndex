/* */

#include "AbstractAnalysis.h"
#include "RefractiveIndex.h"
#include "ofxFileHelper.h"

// this is the main threaded loop for a given analysis
void AbstractAnalysis::do_synthesize() {
    _saved_filenames_analysis.clear();
    _state = STATE_ACQUIRING;
    acquire();
    if(_state == STATE_STOP) goto exit;
    _state = STATE_SYNTHESISING;
    synthesise();
    if(_state == STATE_STOP) goto exit;
    _state = STATE_DISPLAY_RESULTS;
    displayresults();
    exit:    
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

    ofxFileHelper fileHelperAnalysis;
    ofxFileHelper fileHelperSynthesis;
    
    _whole_file_path_analysis = ANALYSIS_PATH + RefractiveIndex::_location + "/" + _name + "/"+replaceTime ;
  
    //cout << "_whole_file_path_analysis = " << _whole_file_path_analysis << endl;
    
    if(!fileHelperAnalysis.doesDirectoryExist(_whole_file_path_analysis)){
        fileHelperAnalysis.makeDirectory(ANALYSIS_PATH);
        fileHelperAnalysis.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location);
        fileHelperAnalysis.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location+"/"+_name);
        fileHelperAnalysis.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location+"/"+_name+"/"+replaceTime);
    }
    
    _whole_file_path_synthesis = SYNTHESIS_PATH + RefractiveIndex::_location + "/" + _name + "/"+replaceTime;
    
    if(!fileHelperSynthesis.doesDirectoryExist(_whole_file_path_synthesis)){
        fileHelperSynthesis.makeDirectory(SYNTHESIS_PATH);
        fileHelperSynthesis.makeDirectory(SYNTHESIS_PATH+RefractiveIndex::_location);
        fileHelperSynthesis.makeDirectory(SYNTHESIS_PATH+RefractiveIndex::_location+"/"+_name);
        fileHelperSynthesis.makeDirectory(SYNTHESIS_PATH+RefractiveIndex::_location+"/"+_name+"/"+replaceTime);
    }
    
    //////////////////////////////END DIRECTORY CREATION //////////////////////////////////////////////////    
}

