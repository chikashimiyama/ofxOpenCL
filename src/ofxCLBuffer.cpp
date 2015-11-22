//
//  ofxCLBuffer.cpp
//
//  Created by Chikashi Miyama on 05/10/14.
//
//

#include "ofxCLBuffer.h"

ofxCLBuffer::ofxCLBuffer(const cl::Context &clContext, const cl::CommandQueue &commandQueue, const void *data, unsigned int size, cl_mem_flags mode):
commandQueue(commandQueue),
clBuffer(clContext, mode, size),
size(size){
    commandQueue.enqueueWriteBuffer(clBuffer, CL_TRUE, 0, size, data);
}

const cl::Buffer &ofxCLBuffer::getCLBuffer() const{
    return clBuffer;
}

unsigned int ofxCLBuffer::getSize() const{
    return size;
};


void ofxCLBuffer::writeToCLBuffer(const void *data, unsigned int size){
    commandQueue.enqueueWriteBuffer(clBuffer, CL_TRUE, 0, size, data);
}
