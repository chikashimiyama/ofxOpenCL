//
//  ofxOpenCL.h
//
//  Created by Chikashi Miyama on 04/10/14.
//
//

#ifndef __ofxOpenCL__
#define __ofxOpenCL__

#include "ofMain.h"
#include "ofxCLBuffer.h"
#include "ofxCLBufferGL.h"
#include "cl.hpp"

class ofxOpenCL{

public:

    // must specify device name and source code.
    ofxOpenCL(const std::string &deviceName,
              const std::string &clSource,
              const std::string &kernelName,
              const std::vector<unsigned> &workItems);
    ~ofxOpenCL();
    
    void createNewBuffer(const std::string &bufferName, const void *data, unsigned int size, cl_mem_flags flag = CL_MEM_READ_WRITE);
    void createNewBufferGL(const std::string &bufferName, std::vector<ofVec3f> defaultVertices, cl_mem_flags flag = CL_MEM_READ_WRITE) ;
    void createNewBufferGL(const std::string &bufferName, std::vector<ofVec3f> defaultVertices, std::vector<ofFloatColor> defaultColors, cl_mem_flags flag = CL_MEM_READ_WRITE) ;


    unsigned int  getNumberOfBuffer();
    unsigned int  getNumberOfGLBuffer();
    ofVbo &getVbo(const std::string &bufferName);

    void updateBuffer(const std::string &bufferName, const void *data, unsigned int size);
    void process(const std::vector<std::string> &bufferList);
    
protected:
    void postDeviceProfile(const cl::Device &device) const;

    cl::NDRange ndRange;
    cl::Program clProgram;
    cl::Context clContext;
    
    cl::CommandQueue clCommandQueue;
    cl::Kernel clKernel;
    std::vector<cl::Device> devVector;
    
    std::map<std::string, ofxCLBuffer> bufferMap;
    std::map<std::string, ofxCLBufferGL> bufferGLMap;
    

};


#endif