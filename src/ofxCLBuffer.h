//
//  ofxCLBuffer.h
//
//  Created by Chikashi Miyama on 05/10/14.
//
//

#ifndef ____ofxCLBuffer__
#define ____ofxCLBuffer__

#include "ofMain.h"
#include "cl.hpp"

class ofxCLBuffer{
    
public:
    ofxCLBuffer(const cl::Context &clContext, const cl::CommandQueue &queue, const void* data, unsigned int size, cl_mem_flags mode = CL_MEM_WRITE_ONLY);
    
    void writeToCLBuffer(const void *data, unsigned int size);
    const cl::Buffer &getCLBuffer() const;
    unsigned int getSize() const;
    
protected:
    cl::Buffer clBuffer;
    const cl::CommandQueue &commandQueue;
    const unsigned int size;
    
};

#endif
