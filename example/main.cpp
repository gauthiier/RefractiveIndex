#include "ofAppGlutWindow.h"
#include "RefractiveIndex.h"

#define SCREEN_WIDTH        1280
#define SCREEN_HEIGHT       800

int main() {
    
	ofAppGlutWindow window;
   
    bool fullscreen;
    
    //fullscreen = true; 
    fullscreen = false;
    
    cout << "> display configuration" << endl;
    cout << "* fullscreen: " << (fullscreen ? "yes" : "no") << endl;
 
    if(!fullscreen) {
        cout << "* screen width: " << SCREEN_WIDTH << endl;
        cout << "* screen height: " << SCREEN_HEIGHT << endl;
    }
    
	ofSetupOpenGL(&window, SCREEN_WIDTH, SCREEN_HEIGHT, (fullscreen ? OF_FULLSCREEN : OF_WINDOW));
	ofRunApp(new RefractiveIndex());

}
