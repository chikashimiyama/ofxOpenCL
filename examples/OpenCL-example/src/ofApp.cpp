#include "ofApp.h"
#include "ofxOpenCL.h"
#include <chrono>
//--------------------------------------------------------------
void ofApp::setup(){

    
    auto buffer = ofBufferFromFile(ofToDataPath("Example.cl"));
    bool error;
    auto openCL = ofxOpenCL(buffer.getText(), "direct_convolution", {512}, {}, error);
    if(error){
        abort();
    }
    
    // count 3D from 0 to 999
    vector<int> a(512), b(512), c(1024); // access all work items
    for(int i = 0; i < 50; i++){ a[i] = i;  b[i] = 0;}
    
    // impulses for test
    b[0] = 1;
    b[60] = 1;
    b[150] = 1;
    
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    openCL.createNewBuffer("a", a, CL_MEM_WRITE_ONLY);
    openCL.createNewBuffer("b", b, CL_MEM_WRITE_ONLY);
    openCL.createNewBuffer("c", c, CL_MEM_READ_ONLY);
    openCL.process({"a","b","c"});
    openCL.retrieveBuffer("c", c);
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    ofLog() << "elapsed time: " << elapsed_seconds.count() << "s\n";

    
    for (int x : c){
        cout << x << " ";
    }
    
    //

    
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
