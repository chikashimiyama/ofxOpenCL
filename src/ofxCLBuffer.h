//
//  ofxCLBuffer.h
//
//  Created by Chikashi Miyama on 05/10/14.
//
//

#pragma once

#include "cl.hpp"
using namespace std;
using namespace cl;

class _ofxCLBuffer {
    friend class ofxOpenCL;
    
protected:
    _ofxCLBuffer(const Context &clContext, const CommandQueue &queue, cl_mem_flags mode, const unsigned int size):commandQueue(queue), rawBuffer(clContext, mode, size){}
    const Buffer &getRawBuffer() const{
        return rawBuffer;
    }
    
    const cl::CommandQueue &commandQueue;
    Buffer rawBuffer;
};

template <typename T>
class ofxCLBuffer :  public _ofxCLBuffer{
    friend class ofxOpenCL;
    
protected:
   
    ofxCLBuffer(const Context &clContext,
                const CommandQueue &queue,
                const vector<T> &data,
                cl_mem_flags mode = CL_MEM_WRITE_ONLY):
                _ofxCLBuffer(clContext, queue, mode, data.size() * sizeof(T)){
                    writeToCLBuffer(data);
                }

    void writeToCLBuffer(const vector<T> &data){
        commandQueue.enqueueWriteBuffer(rawBuffer, CL_TRUE, 0, sizeof(T) * data.size(), &data[0]);
    }
    
    void readFromCLBuffer(vector<T> &data){
        commandQueue.enqueueReadBuffer(rawBuffer, CL_TRUE,0, sizeof(T) * data.size(), &data[0]);
    }

};

