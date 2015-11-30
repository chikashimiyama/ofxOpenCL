#include "ofApp.h"
#include "ofxOpenCL.h"
//--------------------------------------------------------------
void ofApp::setup(){

    
    auto buffer = ofBufferFromFile(ofToDataPath("Example.cl"));
    bool error;
    vector<unsigned int>ndRange = {10};
    auto openCL = ofxOpenCL(buffer.getText(), "hello_kernel", ndRange, error);
    if(error != CL_SUCCESS){
        abort();
    }
    vector<int> b = {0,1,2,3,4,5,6,7,8,9};
    vector<int> a = {0,1,2,3,4,5,6,7,8,9};
    vector<int> result  = {0,0,0,0,0,0,0,0,0,0};

    openCL.createNewBuffer("a", a, CL_MEM_WRITE_ONLY);
    openCL.createNewBuffer("b", b, CL_MEM_WRITE_ONLY);
    openCL.createNewBuffer("result", result, CL_MEM_READ_ONLY);
    openCL.process({"a","b","result"});
    openCL.retrieveBuffer("result", result);
    
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
