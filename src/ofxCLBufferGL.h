//
//  ofxOpenCLBufferGL.h
//
//  Created by Chikashi Miyama on 05/10/14.
//
//

#ifndef ____ofxOpenCLBufferGL__
#define ____ofxOpenCLBufferGL__

#include "ofMain.h"
#include "cl.hpp"

class ofxCLBufferGL{
    
public:
    enum bufferMode{
        VERTEX,
        VERTEX_COLOR
    };
    ofxCLBufferGL(const cl::Context &clContext, std::vector<ofVec3f> defaultVertices, cl_mem_flags flag = CL_MEM_WRITE_ONLY);
    ofxCLBufferGL(const cl::Context &clContext, std::vector<ofVec3f> defaultVertices, std::vector<ofFloatColor> defaultColors, cl_mem_flags flag = CL_MEM_WRITE_ONLY);

    ofVbo &getVbo();
    const cl::BufferGL &getCLVertexBuffer() const;
    const cl::BufferGL &getCLColorBuffer() const;
    const bufferMode getBufferMode();
    
protected:
    ofVbo vbo;
    cl::BufferGL clVertexBufferGL;
    cl::BufferGL clColorBufferGL;
    const bufferMode mode;
};

inline const ofxCLBufferGL::bufferMode ofxCLBufferGL::getBufferMode(){
    return mode;
}

#endif
