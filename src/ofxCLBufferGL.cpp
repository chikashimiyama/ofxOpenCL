//
//  ofxOpenCLBufferGL.cpp
//
//  Created by Chikashi Miyama on 05/10/14.
//
//

#include "ofxCLBufferGL.h"

ofxCLBufferGL::ofxCLBufferGL(const cl::Context &clContext, std::vector<ofVec3f> defaultVertices, cl_mem_flags flag):mode(VERTEX){
    cl_int err;
    vbo.setVertexData(&defaultVertices.front(), defaultVertices.size(), GL_DYNAMIC_DRAW); // OpenGL VBO
    vbo.disableColors();
    vbo.disableNormals();
    vbo.disableTexCoords();
    clVertexBufferGL = cl::BufferGL(clContext, flag, vbo.getVertId(), &err); // link vbo and openCL
    
    if(err != CL_SUCCESS){
        ofLog(OF_LOG_ERROR) << "ofxBufferGLCombo::ofxBufferGLCombo\nunable to allocate memory";
    }
}

ofxCLBufferGL::ofxCLBufferGL(const cl::Context &clContext, std::vector<ofVec3f> defaultVertices, std::vector<ofFloatColor> defaultColor, cl_mem_flags flag):mode(VERTEX_COLOR){
    cl_int err;
    
    if(err != CL_SUCCESS){
        ofLog(OF_LOG_ERROR) << "ofxBufferGLCombo::ofxBufferGLCombo\nunable to allocate memory";
    }
};


ofVbo &ofxCLBufferGL::getVbo(){
    return vbo;
}

const cl::BufferGL &ofxCLBufferGL::getCLVertexBuffer() const{
    return clVertexBufferGL;
}

const cl::BufferGL &ofxCLBufferGL::getCLColorBuffer() const{
    return clColorBufferGL;
}
