#include "ofAppGlutWindow.h"
#include "RefractiveIndex.h"
#include "ofxXmlSettings.h"

#define SCREEN_WIDTH        800
#define SCREEN_HEIGHT       600

int main() {
	ofAppGlutWindow window;
    
    ofxXmlSettings   XML;
    XML.loadFile("../data/config.refindx");
    bool fullscreen = (XML.getValue("config:display:fullscreen", "false") == "true" ? true : false);
    int screen_w = XML.getValue("config:display:width", SCREEN_WIDTH);
    int screen_h = XML.getValue("config:display:height", SCREEN_HEIGHT);     
    
    cout << "> display configuration" << endl;
    cout << "* fullscreen: " << (fullscreen ? "yes" : "no") << endl;
    if(!fullscreen) {
        cout << "* screen width: " << screen_w << endl;
        cout << "* screen height: " << screen_h << endl;
    }
    
	ofSetupOpenGL(&window, screen_w, screen_h, (fullscreen ? OF_FULLSCREEN : OF_WINDOW));
	ofRunApp(new RefractiveIndex());

}
