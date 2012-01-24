#include "ofAppGlutWindow.h"
#include "testApp.h"
#include "RefractiveIndex.h"

int main() {
	ofAppGlutWindow window;
    //window.setGlutDisplayString("rgba double samples >= 8");
	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
	ofRunApp(new RefractiveIndex());
    //ofRunApp(new testApp());

}
