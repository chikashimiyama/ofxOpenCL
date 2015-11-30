//
//  ofxOpenCL.h
//
//  Created by Chikashi Miyama on 04/10/14.
//
//

#pragma once
#include <memory>

#include "ofxCLBufferGL.h"
#include "ofxCLBuffer.h"
using namespace std;
using namespace cl;

// facade class that talks with OpenCL library
class ofxOpenCL{

protected:


    vector<cl::Platform> platforms;
    
    NDRange ndRange;
    Program clProgram;
    Context clContext;
    CommandQueue clCommandQueue;
    Kernel clKernel;
    
    map<string, shared_ptr<_ofxCLBuffer>> bufferMap;
    map<string, ofxCLBufferGL> bufferGLMap;
    
    bool checkPlatforms();
    bool checkDevices(const vector<cl::Device> &gpuDevices);
    bool checkNDRange(const vector<unsigned int> &requestedRange, const Device &targetDevice);
    void setNDRange(const vector<unsigned int> &requestedRange);
    
public:
    /*!
        @brief
    */
    ofxOpenCL(const string &clSource,
              const string &kernelName,
              const vector<unsigned int> &requestedNDRange,
              bool &error);
    
    ~ofxOpenCL();
    
    template <typename T>
    inline void createNewBuffer(const string &bufferName,
                                const vector<T> &data,
                                const cl_mem_flags flag = CL_MEM_READ_WRITE);

    std::size_t getNumberOfBuffer();
    
    ofVbo &getVbo(const string &bufferName);

    // copy buffer RAM->VRAM
    template <typename T>
    bool updateBuffer(const string &bufferName, const vector<T> &data);

    // execute cl
    void process(const vector<string> &bufferList);

    // copy buffer VRAM->RAM
    template <typename T>
    bool retrieveBuffer(const string &bufferName, vector<T> &data);
    
    const string getDeviceInfo();


};

template <typename T>
inline void ofxOpenCL::createNewBuffer(const string &bufferName, const vector<T> &data, const cl_mem_flags flag){
    
    bufferMap.insert(pair<string, shared_ptr<_ofxCLBuffer>>(bufferName, shared_ptr<_ofxCLBuffer>(new ofxCLBuffer<T>(clContext, clCommandQueue, data, flag))));
    
}

std::size_t ofxOpenCL::getNumberOfBuffer(){
    return bufferMap.size();
}



ofVbo &ofxOpenCL::getVbo(const string &bufferName){
    map<string, ofxCLBufferGL>::iterator it = bufferGLMap.find(bufferName);
    if(it != bufferGLMap.end()){
        return (*it).second.getVbo();
    }else{
        // need to be fixed
    }
}


template <typename T>
inline bool ofxOpenCL::updateBuffer(const string &bufferName, const vector<T> &data){
    map<string, shared_ptr<_ofxCLBuffer>>::iterator it = bufferMap.find(bufferName);
    if(it == bufferMap.end()){return false;}
    shared_ptr<_ofxCLBuffer> buffer = (*it).second;
    auto rawBuffer = static_cast<ofxCLBuffer<T>>(buffer)->get();
    rawBuffer.writeToCLBuffer(data, sizeof(T) * data.size());
    return true;
}

template <typename T>
inline bool ofxOpenCL::retrieveBuffer(const string &bufferName, vector<T> &data){
    map<string, shared_ptr<_ofxCLBuffer>>::iterator it = bufferMap.find(bufferName);
    if(it == bufferMap.end()){return false;}
    shared_ptr<_ofxCLBuffer> buffer = (*it).second;
    auto rawBuffer = static_cast<ofxCLBuffer<T>*>(buffer.get());
    rawBuffer->readFromCLBuffer(data);
}