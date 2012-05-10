/* */

#include "AbstractAnalysis.h"
#include "RefractiveIndex.h"
#include "ofxFileHelper.h"
#include "ofSystemUtils.h"

void AbstractAnalysis::setup(int camWidth, int camHeight) {
    
    _cam_w = camWidth; _cam_h = camHeight;
    
    if(RefractiveIndex::_mode == MODE_DRAWING) {
        ofFileDialogResult r = ofSystemLoadDialog("choooose da folda", true);
        if(!r.bSuccess) {
            ofSystemAlertDialog("OOOOPS.... ERROR...");
            return;
        }
                
        _whole_file_path_analysis = r.filePath;
        _whole_file_path_synthesis = r.filePath + "/darwings";        
    }
    
}

// this is the main threaded loop for a given analysis
void AbstractAnalysis::do_synthesize() {
    
    switch(RefractiveIndex::_mode)
    {
        case MODE_ANALYSING:
        {
            for(int i = 0; i < NUM_RUN; i++) {
                
                cout << "NUM_RUN: " << i << endl;
                
                _saved_filenames_analysis.clear();  
                _saved_filenames_synthesis.clear(); 
                
                _state = STATE_ACQUIRING;
                acquire();
                if(_state == STATE_STOP) goto exit;
                _state = STATE_SYNTHESISING;
                synthesise();
                if(_state == STATE_STOP) goto exit;
                _state = STATE_DISPLAY_RESULTS;
                displayresults();
                cleanup();
            }                    
        }
            
        case MODE_DRAWING:
        {            
            ofxFileHelper fileHelperDrawing;
            if(!fileHelperDrawing.doesDirectoryExist(_whole_file_path_synthesis)){
                fileHelperDrawing.makeDirectory(_whole_file_path_synthesis);
            }            
            
            read_dir_create_list(_whole_file_path_analysis);
            _state = STATE_SYNTHESISING;
            synthesise();
            if(_state == STATE_STOP) goto exit;
            _state = STATE_DISPLAY_RESULTS;
            displayresults();
            cleanup();            
        }
    }
    
exit:    
    cleanup();
    ofNotifyEvent(_synthesize_cb, _name);    
    
}

void AbstractAnalysis::create_dir_allocate_images()
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
        
        if(!fileHelperAnalysis.doesDirectoryExist(ANALYSIS_PATH))
        fileHelperAnalysis.makeDirectory(ANALYSIS_PATH);
        
        if(!fileHelperAnalysis.doesDirectoryExist(ANALYSIS_PATH+RefractiveIndex::_location))
        fileHelperAnalysis.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location);

        if(!fileHelperAnalysis.doesDirectoryExist(ANALYSIS_PATH+RefractiveIndex::_location+"/"+_name))
        fileHelperAnalysis.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location+"/"+_name);
        
        if(!fileHelperAnalysis.doesDirectoryExist(ANALYSIS_PATH+RefractiveIndex::_location+"/"+_name+"/"+replaceTime))
        fileHelperAnalysis.makeDirectory(ANALYSIS_PATH+RefractiveIndex::_location+"/"+_name+"/"+replaceTime);
        
        
    }
    
    _whole_file_path_synthesis = SYNTHESIS_PATH + RefractiveIndex::_location + "/" + _name + "/"+replaceTime;
    
    if(!fileHelperSynthesis.doesDirectoryExist(_whole_file_path_synthesis)){
        
        if(!fileHelperAnalysis.doesDirectoryExist(SYNTHESIS_PATH))
        fileHelperSynthesis.makeDirectory(SYNTHESIS_PATH);
        
        if(!fileHelperAnalysis.doesDirectoryExist(SYNTHESIS_PATH+RefractiveIndex::_location))
        fileHelperSynthesis.makeDirectory(SYNTHESIS_PATH+RefractiveIndex::_location);

        if(!fileHelperAnalysis.doesDirectoryExist(SYNTHESIS_PATH+RefractiveIndex::_location+"/"+_name))
        fileHelperSynthesis.makeDirectory(SYNTHESIS_PATH+RefractiveIndex::_location+"/"+_name);
        
        if(!fileHelperAnalysis.doesDirectoryExist(SYNTHESIS_PATH+RefractiveIndex::_location+"/"+_name+"/"+replaceTime))
        fileHelperSynthesis.makeDirectory(SYNTHESIS_PATH+RefractiveIndex::_location+"/"+_name+"/"+replaceTime);
        
    }
    //////////////////////////////END DIRECTORY CREATION //////////////////////////////////////////////////  
    
    
    //////////////////////////////ALLOCATE IMAGES //////////////////////////////////////////////////  
    
    myColorImage1.clear();
    myColorImage1.setUseTexture(false);
    myColorImage1.allocate(RefractiveIndex::_vid_w, RefractiveIndex::_vid_h, OF_IMAGE_COLOR);

    myColorImage2.clear();
    myColorImage2.setUseTexture(false);
    myColorImage2.allocate(RefractiveIndex::_vid_w, RefractiveIndex::_vid_h, OF_IMAGE_COLOR);
    
    
    myGrayImage1.clear();
    myGrayImage1.setUseTexture(false);
    myGrayImage1.allocate(RefractiveIndex::_vid_w, RefractiveIndex::_vid_h, OF_IMAGE_GRAYSCALE);
    
    //////////////////////////////END ALLOCATE IMAGES //////////////////////////////////////////////////  

}

bool cmp_file(string f0, string f1)
{
    int v0 = atoi(f0.substr(0, f0.find("_")).c_str());
    int v1 = atoi(f1.substr(0, f1.find("_")).c_str());    
    return v0 < v1;
}

void AbstractAnalysis::read_dir_create_list(string folder_path)
{
    File dir(folder_path);
    
    if(dir.exists() && dir.isDirectory()) {
        vector<string> list;
        dir.list(list);   
        
        std::sort(list.begin(), list.end(), cmp_file);
        
        for(int i = 0; i < list.size(); i++) {
            string filepath = folder_path + "/" + list[i]; 
            _saved_filenames_analysis.push_back(filepath);
        }                
    }
}

void AbstractAnalysis::saveImageAnalysis(string filename)
{
    
    RefractiveIndex::_vidGrabber.grabFrame();  // get a new frame from the camera
    
    if (RefractiveIndex::_vidGrabber.isFrameNew())
    {
        RefractiveIndex::_pixels = RefractiveIndex::_vidGrabber.getPixelsRef(); //get ofPixels from the camera
    } else {
        return;
    }
        
#ifdef TARGET_OSX   
    
    ofSaveImage(RefractiveIndex::_pixels, _whole_file_path_analysis+"/"+filename, OF_IMAGE_QUALITY_BEST);
    
#elif defined(TARGET_WIN32)    
    
    //<---- NEW SAVING - seems to fix WINDOWS saving out BLACK FRAMES PROBLEM ---->
    unsigned char * somePixels;
    ofPixels appPix = RefractiveIndex::_pixels;
    somePixels = appPix.getPixels();
    myColorImage1.setUseTexture(false);
    myColorImage1.setFromPixels(somePixels,appPix.getWidth(),appPix.getHeight(), OF_IMAGE_COLOR);
    myColorImage1.saveImage(ofToDataPath("")+ _whole_file_path_analysis+"/"+filename);
    myColorImage1.clear();
    
#endif
    
    _saved_filenames_analysis.push_back(_whole_file_path_analysis+"/"+filename);
    
}


void AbstractAnalysis::saveImageSynthesis(string filename, ofxCvImage* newPixels, ofImageType newType)
{
    
#ifdef TARGET_OSX   
    

    ofSaveImage(newPixels->getPixelsRef(), _whole_file_path_synthesis+"/"+filename, OF_IMAGE_QUALITY_BEST);
    
#elif defined(TARGET_WIN32)    
    
    if (newType == OF_IMAGE_COLOR){
       myColorImage2.setUseTexture(false);
       myColorImage2.setFromPixels(newPixels->getPixels(), newPixels->getWidth(), newPixels->getHeight(), OF_IMAGE_COLOR);
       myColorImage2.saveImage(_whole_file_path_synthesis+"/"+filename);
    }

    if (newType == OF_IMAGE_GRAYSCALE){
        myGrayImage1.setUseTexture(false);
        
        // THIS IS HOW YOU HAVE TO SAVE OUT THE GREYSCALE IMAGES on WINDOWS FOR SOME REASON --> i.e.: as an OF_IMAGE_COLOR
        // But they don't save properly - they're spatially translated and generally f'd up
        myGrayImage1.setFromPixels(newPixels->getPixels(), newPixels->getWidth(), newPixels->getHeight(), OF_IMAGE_COLOR);
        myGrayImage1.setImageType(OF_IMAGE_COLOR);
        myGrayImage1.saveImage(_whole_file_path_synthesis+"/"+filename);
        //myGrayImage1.clear();
    }
        
#endif
    
    _saved_filenames_synthesis.push_back(_whole_file_path_synthesis+"/"+filename);
            
    
}

