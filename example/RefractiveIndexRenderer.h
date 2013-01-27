/* 
 RefractiveIndexRenderer.h - RefractiveIndexRenderer library
 Copyright (c) 2012 Copenhagen Institute of Interaction Design. 
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

#pragma once

// of
#include "ofMain.h"
#include "ofEvents.h"

// of addons
#include "ofxXmlSettings.h"
#include "ofxArcBall.h"

#define VERTS   1
#define WIRE    2
#define FACE    3

class RefractiveIndexRenderer : public ofBaseApp
{
public:
    
    // of
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed  (int key);
    
    bool setup_dirs();
    void setup_vbo(int img_width, int img_height);
    void setup_shader();
    
    void save(string filename);
    
public:
    
    ofVboMesh   _mesh_vbo;
    vector<ofVec3f> _verts;
    vector<ofVec2f> _tex;
    vector<unsigned int> _ind;
    
    ofShader    _shader;    
    ofPixels    _pixels;   
    ofFbo       _fbo;    
    int         _indx;    
    
    ofxXmlSettings   XML;
    
    string          _images_directory;       
    string          _draw_directory;
    vector<string>  _images_directory_filenames;
    
    int algo;
    float scale;
    int draw_style;   
    float line_width;
    float point_size;    
    
    ofxArcBall _cam;
    
};
