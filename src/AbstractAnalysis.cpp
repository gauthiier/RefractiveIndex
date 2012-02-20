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


vector<ofMesh>  AbstractAnalysis::meshes;

// this is the main threaded loop for a given analysis
void AbstractAnalysis::do_synthesize() {
    _state = STATE_ACQUIRING;
    acquire();
    _state = STATE_SYNTHESISING;
    synthesise();
    _state = STATE_DISPLAY_RESULTS;
    display_results();
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
ofPixels AbstractAnalysis::calculateListOfZValues(ofImage image1, ofImage image2, int whichComparison){
    //zScale is the mapping factor from pixel difference to shift on the zPlane
    int zScale=200;
    
    ofPixels imagePixels1 = image1.getPixelsRef();
    ofPixels imagePixels2 = image2.getPixelsRef();
    
    ofPixels difference;
    //this unsigned char should be unnecessary - I would have thought - can't you just address the pixel locations in ofPixels directly? 
    unsigned char * thesePixels = new unsigned char[ imagePixels1.getWidth()*imagePixels1.getHeight()*3];
    
    //where are we in the image pixel array
    int x=0;
    int y=0;
    
    //for each pixel...
    for(int i=0;i<imagePixels1.size();i+=3){
        
        //get the colour of each image at this x y location - we will use these colours for comparison according to the below criteria
        ofColor colourImage1 = imagePixels1.getColor(x, y);
        ofColor colourImage2 = imagePixels2.getColor(x, y);
        
        //COMPARE THIS PIXEL'S VALUES with the first image in the sequence
        int thisDiff;
        //compare Red
        if (whichComparison==1) {
            thisDiff=ofMap((colourImage1.r-colourImage2.r),-255,255,0,zScale);
        }
        //compare blue
        if (whichComparison==2) {
            thisDiff=ofMap((colourImage1.g-colourImage2.g),-255,255,0,zScale);
        }
        //compare green
        if (whichComparison==3) {
            thisDiff=ofMap((colourImage1.b-colourImage2.b),-255,255,0,zScale);
        }
        //compare hue
        if (whichComparison==4) {
            thisDiff=ofMap((colourImage1.getHue()-colourImage2.getHue()),-255,255,0,zScale);
        }
        //compare brightness
        if (whichComparison==5) {
            thisDiff=ofMap((colourImage1.getBrightness()-colourImage2.getBrightness()),-255,255,0,zScale);
        }
        thesePixels[i]=thisDiff;
        thesePixels[i+1]=thisDiff;
        thesePixels[i+2]=thisDiff;
        
        x++;
        //new line
        if(x>imagePixels1.getWidth()){
            x=0;
            y++;
            
        }
    }
    
    difference.setFromPixels(thesePixels,imagePixels1.getWidth(),imagePixels1.getHeight(), 3);
    
    return difference;
    
}

ofPixels AbstractAnalysis::calculateListOfZValues(ofImage image1, ofImage image2, int whichComparison, int colourValue){
    //zScale is the mapping factor from pixel difference to shift on the zPlane
    int zScale=200;
    
    ofPixels imagePixels1 = image1.getPixelsRef();
    ofPixels imagePixels2 = image2.getPixelsRef();
    
    ofPixels difference;
    //this unsigned char should be unnecessary - I would have thought - can't you just address the pixel locations in ofPixels directly? 
    unsigned char * thesePixels = new unsigned char[ imagePixels1.getWidth()*imagePixels1.getHeight()*3];
    
    //where are we in the image pixel array
    int x=0;
    int y=0;
    
    //for each pixel...
    for(int i=0;i<imagePixels1.size();i+=3){
        
        //get the colour of each image at this x y location - we will use these colours for comparison according to the below criteria
        ofColor colourImage1 = imagePixels1.getColor(x, y);
        ofColor colourImage2 = imagePixels2.getColor(x, y);
        
        //COMPARE THIS PIXEL'S VALUES with the first image in the sequence
        int thisDiff;
        //compare Red
        if (whichComparison==1) {
            thisDiff=ofMap((colourImage1.r-colourImage2.r),-255,255,0,zScale);
        }
        //compare blue
        if (whichComparison==2) {
            thisDiff=ofMap((colourImage1.g-colourImage2.g),-255,255,0,zScale);
        }
        //compare green
        if (whichComparison==3) {
            thisDiff=ofMap((colourImage1.b-colourImage2.b),-255,255,0,zScale);
        }
        //compare hue
        if (whichComparison==4) {
            thisDiff=ofMap((colourImage1.getHue()-colourImage2.getHue()),-255,255,0,zScale);
        }
        //compare brightness
        if (whichComparison==5) {
            thisDiff=ofMap((colourImage1.getBrightness()-colourImage2.getBrightness()),-255,255,0,zScale);
        }
        thesePixels[i]=thisDiff;
        thesePixels[i+1]=thisDiff;
        thesePixels[i+2]=thisDiff;
        
        x++;
        //new line
        if(x>imagePixels1.getWidth()){
            x=0;
            y++;
            
        }
    }
    
    difference.setFromPixels(thesePixels,imagePixels1.getWidth(),imagePixels1.getHeight(), 3);
    
    return difference;
    
}

void AbstractAnalysis::setMeshFromPixels(ofPixels somePixels, ofImage currentSecondImage, ofMesh * someMesh){
    int x=0;
    int y=0;
    
    //get rid of all previous vectors and colours - uncomment if re-setting the mesh on the fly - ie live rather than saving it first
    //someMesh->clear();
    
    unsigned char * thesePixels =currentSecondImage.getPixels();
    
    for(int i=0;i<somePixels.size();i+=3){
        someMesh->addVertex(ofVec3f(x,y,- somePixels.getColor(x, y).getBrightness()   ));
        // add colour from current second image of two 
        someMesh->addColor(  currentSecondImage.getColor(x, y)   );
        x++;
        if(x>somePixels.getWidth()){
            x=0;
            y++;
        }
        
    }
    
}

vector<string>AbstractAnalysis:: getListOfImageFilePaths(string location, string whichAnalysis){
    
    string path = ofToDataPath("")+"debug_analysis/"+location+"/"+whichAnalysis;
    //ofxDirList dirList;
    ofDirectory dirList;
    int numDirs = dirList.listDir(path);
    
    vector<string>directoryNames;
    
    //get the last folder alphabetically - this should probably change to do something fancy with date to find most recent but don't want to code that until we are sure
    string dirName=dirList.getName(numDirs-1);
    
    
    const char *results=dirName.c_str();
    
    ofFile file=ofFile(path+"/"+dirName);
    vector<string>fileNamesToReturn;
    
    
    // get 
    if(file.isDirectory()){
        dirList.listDir(path+"/"+dirName);
        //if there are no files, exit here
        if(dirList.size()==0){
            //if it's empty return an error warning
            fileNamesToReturn.push_back("NO FILE HERE!");
            cout<<"NO FILE HERE!";
            return fileNamesToReturn;
        }
        for (int i=0; i<dirList.size(); i++) {
            
            string fname=dirList.getName(i);
            const char *results=fname.c_str();
            
            //full path is what actually gets written into the vector
            string fullPath=path+"/"+dirName+"/"+fname;
            
            
            fileNamesToReturn.push_back(fullPath);
        }
    }
    else{
        cout<<"WARNING, DIRECTORY NOT FOUND";
        fileNamesToReturn.push_back("NO FILE HERE!");
    }
    
    
    
    
    return fileNamesToReturn;
}
int AbstractAnalysis::getRecordedValueFromFileName(string str){
    //split filename by underscore - there HAS to be a quicker way of doing things - its ONE LINE in java :(
    char * cstr, *p;
    vector<char *>tokens;
    //string str ("Please split this phrase into tokens");
    
    //make char pointer array
    cstr = new char [str.size()+1];
    //copy string to char pointer array
    strcpy (cstr, str.c_str());
    
    //tokenise char p array and put first results into pointer?
    p=strtok (cstr,"_");
    
    while (p!=NULL)
    {
        p=strtok(NULL,"_");
        //push tokenised char into vector
        tokens.push_back(p);
        
    }
    delete[] cstr; 
    char *p1;
    //cstr = new char [str.size()+1];
    //strcpy (cstr, str.c_str());
    
    p1=strtok (tokens[tokens.size()-2],".");
    
    return ofToInt(p1);
}


void AbstractAnalysis::saveimage(string filename) 
{
    if(RefractiveIndex::_pixels.isAllocated()) {
        ofSaveImage(RefractiveIndex::_pixels, filename, OF_IMAGE_QUALITY_BEST);
        _saved_filenames.push_back(filename);
    } else {
        ofLog(OF_LOG_ERROR) << "RefractiveIndex::_pixels NOT allocated...";
    }
}

