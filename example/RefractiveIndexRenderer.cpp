/* 
 RefractiveIndexRenderer.cpp - RefractiveIndexRenderer library
 Copyright (c) 2013 Copenhagen Institute of Interaction Design. 
 All right reserved.
 
 This library is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser Public License for more details.
 
 You should have received a copy of the GNU Lesser Public License
 along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 + author: dviid
 + contact: dviid@labs.ciid.dk 
 */

#include "RefractiveIndexRenderer.h"

// of
#include "ofSystemUtils.h"

// of addons
#include "ofxFileHelper.h"

const int VPF = 1000;
const int PPV = 4;

int W = 1280, INPUT_W = 0, H = 800, INPUT_H = 0, SIZE = 0;

bool killnow = false;
bool go = false;
bool edit = false;

ofTrueTypeFont font;
ofImage image;

const int algo_default = 1;
const float scale_default = 1;
const int draw_style_default = 3;
const int line_width_default = 0.5f;
const float point_size_default = 0.5f;

//viewport
float tx, ty, tz, rx, ry, rz;    


void RefractiveIndexRenderer::setup(){
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    cout << "Loading configuration..." << endl;
    if(!XML.loadFile("config.renderer.refindx")) {
        ofLog(OF_LOG_ERROR) << "error loading config - using defaults.";
    }    
    
    string e = XML.getValue("config:renderer:edit", "false");
    edit = (e == "true");
    
    if(!setup_dirs())
        exit();
    
    if(_images_directory_filenames.size() == 0)
        exit();
    
    SIZE = _images_directory_filenames.size();
    go = true;
    
    // we assume all files from the images dir are the same size
    
    string sample = _images_directory_filenames[(int)ofRandom(0, _images_directory_filenames.size() -1)];    
    if(!image.loadImage(sample))
        exit();    
    INPUT_W = image.getWidth();
    INPUT_H = image.getHeight();
    
    // allocate pixels data
    _pixels.allocate(W, H, OF_IMAGE_COLOR); 
    
    // allocate FBO
    _fbo.allocate(W, H, GL_RGB); 
    
    // allocate VBO
    setup_vbo(W, H);
    
    // create shader
    setup_shader();
    
    // set index count (progess in rendering + saving images from FBO to drawing/ files)
    _indx = 0;
    
    // set load font 
    font.loadFont("/System/Library/Fonts/Geneva.dfont", 16);
    
    algo = XML.getValue("config:renderer:algorithm:algo", algo_default);
    scale = XML.getValue("config:renderer:algorithm:scale", scale_default);
    draw_style = XML.getValue("config:renderer:algorithm:draw_style", draw_style_default);
    line_width = XML.getValue("config:renderer:algorithm:line_width", line_width_default);        
    point_size = XML.getValue("config:renderer:algorithm:point_size", point_size_default);
    

}
void RefractiveIndexRenderer::update(){    
    if(killnow)
        exit();         
}
void RefractiveIndexRenderer::draw(){
    
    static bool done = false;
    
    if(!go) return;
    
    if(!edit) {
    char str[255];
        sprintf(str, "%s \n input: %s\n output: %s \n at: %i \n total: %i", (done ? "Done!" : "Rendering"),_images_directory.c_str(), _draw_directory.c_str(), _indx, SIZE);
        font.drawString(str, 10, 50);  
    }
    
    if(done) return;
        
    if(_indx >= _images_directory_filenames.size()) {
        if(!edit) {
            done = true;
            image.clear();
            return;
        } else {
            _indx = 0;
        }
    }
    
    // clear previous image
    image.clear();
    
    // load new image to render
    image.loadImage(_images_directory_filenames[_indx++]);
    image.resize(W, H);
    
    // bind FBO    
    _fbo.begin();
    
    // camera
    _cam.begin();    

    // GL
    //ofEnableAlphaBlending();
    glShadeModel(GL_SMOOTH);
    glLineWidth(line_width);
    glPointSize(point_size);
    glEnable(GL_POINT_SMOOTH);
    
    
    ofTranslate(tx, ty, tz);
    ofRotateX(rx+1.5*cos(ofGetElapsedTimef())); ofRotateY(ry+1.5*sin(ofGetElapsedTimef())); ofRotateZ(rz);
    //ofRotateX(rx); ofRotateY(ry); ofRotateZ(rz);
    glScalef(1.5, 1, 1);
        
    glClear(GL_COLOR_BUFFER_BIT);
    
    // bind texture
    image.bind();        
    
    // bind shader
    _shader.begin();    
    _shader.setUniform1i("algo", algo);
    _shader.setUniform1f("scale", scale);
    _shader.setUniform1i("tex0", 0);
    
    // VBO draw 
    switch (draw_style) {
        case VERTS:
            _mesh_vbo.drawVertices();
            break;
        case WIRE:
            _mesh_vbo.drawWireframe();
            break;
        case FACE:
            _mesh_vbo.drawFaces();
            break;            
    }
    
    // unbind shader
    _shader.end();                
    
    // unbind texture
    image.unbind();
    
    _cam.end();
    
    // unbind FBO
    _fbo.end();
    
    if(edit) {
        _fbo.draw(0, 0);
    } else {
        std::stringstream s;
        s << _indx;    
        save(_draw_directory + "/" + s.str());    
    }
    
}
void RefractiveIndexRenderer::exit(){
    ofLog(OF_LOG_ERROR) << "exit...";  
}
void RefractiveIndexRenderer::keyPressed  (int key){
    if( key =='e'){
        edit = !edit;    
        _indx = 0;
    }
}
bool file_cmp(string f0, string f1)
{
    int v0 = atoi(f0.substr(0, f0.find("_")).c_str());
    int v1 = atoi(f1.substr(0, f1.find("_")).c_str());    
    return v0 < v1;
}
bool RefractiveIndexRenderer::setup_dirs(){
    
    // ask for a directory
    ofFileDialogResult r = ofSystemLoadDialog("choooose da folda", true);
    if(!r.bSuccess) {
        ofSystemAlertDialog("OOOOPS.... ERROR...");
        return;
    }
    
    // copy names
    _images_directory = r.filePath;
    _draw_directory = r.filePath + "/darwings";   
        
    // list all images in the directory and save into vector
    File dir(_images_directory);
    
    if(dir.exists() && dir.isDirectory()) {
        vector<string> list;
        dir.list(list);   
        
        std::sort(list.begin(), list.end(), file_cmp);
        
        for(int i = 0; i < list.size(); i++) {
            string filepath = _images_directory + "/" + list[i]; 
            _images_directory_filenames.push_back(filepath);
        }                
    }
    
    // create drawing directory
    if(!ofxFileHelper::doesDirectoryExist(_draw_directory))
        ofxFileHelper::makeDirectory(_draw_directory);        
    
}
void RefractiveIndexRenderer::setup_vbo(int img_width, int img_height){
    
    int vertices_per_frame = XML.getValue("config:renderer:vertices_per_frame", VPF);
    int pixel_per_vertex = XML.getValue("config:renderer:pixel_per_vertex", PPV);
    
    int vertices_X = img_width / pixel_per_vertex, vertices_Y = img_height / pixel_per_vertex;
    
    // VBO        
    for(int i = 0; i < vertices_X; i++){
        for(int j = 0; j < vertices_Y; j++) {
            _verts.push_back(ofVec3f((i / (float)vertices_X) * vertices_per_frame, (j / (float) vertices_Y) * vertices_per_frame, 0.0f));
            _tex.push_back(ofVec2f(i / (float)vertices_X * img_width, j / (float) vertices_Y * img_height));
            //_tex.push_back(ofVec2f(i * pixel_per_vertex, j * pixel_per_vertex));
			if( ( i + 1 < vertices_X ) && ( j + 1 < vertices_Y ) ) {                
                //triangle #1
                _ind.push_back( (i+0) * vertices_Y + (j+0) );
                _ind.push_back( (i+1) * vertices_Y + (j+0) );
                _ind.push_back( (i+1) * vertices_Y + (j+1) );
                
                //triangle #2
                _ind.push_back( (i+1) * vertices_Y + (j+1) );
                _ind.push_back( (i+0) * vertices_Y + (j+1) );
                _ind.push_back( (i+0) * vertices_Y + (j+0) );
			}
        }
    }
    
    //ofEnableNormalizedTexCoords();
    
    _mesh_vbo.addVertices(_verts);
    _mesh_vbo.addTexCoords(_tex);
    _mesh_vbo.addIndices(_ind);
    
    _mesh_vbo.setMode(OF_PRIMITIVE_TRIANGLES);    
    
}
void RefractiveIndexRenderer::setup_shader(){
    
    // TODO: INTEGRATE JAMIE CODE
/*    
    _shader.setGeometryOutputCount(3);
    _shader.load("rfi.vert", "rfi.frag", "rfi.geom"); 	
    _shader.setGeometryInputType(GL_TRIANGLES);
    _shader.setGeometryOutputType(GL_TRIANGLES);    
*/    
	_shader.setGeometryOutputCount(3);
    _shader.load("dviid/rfi.vert.glsl", "dviid/rfi.frag.glsl", "dviid/rfi.geom.glsl"); 	
	_shader.setGeometryInputType(GL_TRIANGLES);
	_shader.setGeometryOutputType(GL_TRIANGLES);
    
	printf("Maximum number of output vertices support is: %i\n", _shader.getGeometryMaxOutputCount());        
        
}
void RefractiveIndexRenderer::save(string filename){   
    
    // clear pixels
    _pixels.clear();
    
    // copy fbo pixels to of pixels
    _fbo.readToPixels(_pixels);
    
    // save pixels
    ofSaveImage(_pixels, filename+".jpg", OF_IMAGE_QUALITY_BEST);    
    
}

