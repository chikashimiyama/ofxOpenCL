//
//  ofxOpenCL.cpp
//
//  Created by Chikashi Miyama on 04/10/14.
//
//

#include "ofxOpenCL.h"
#include <iostream>
#include <string>


ofxOpenCL::ofxOpenCL(const std::string &deviceName, const std::string &clSource, const std::string &kernelName, const std::vector<unsigned int> &workItems){
    if(workItems.size() == 1){
        ndRange = cl::NDRange(workItems[0]);
    }else if(workItems.size() == 2){
        ndRange = cl::NDRange(workItems[0], workItems[1]);
    }else if(workItems.size() >= 3){
        ndRange = cl::NDRange(workItems[0], workItems[1], workItems[2]);
    }else{
        ofLog(OF_LOG_ERROR) << "ofxOpenCL::ofxOpenCL unable to define NDRange. set to 512";
        ndRange = cl::NDRange(512);
    }
    
    // look for platform
    cl_int err = CL_SUCCESS;
    std::vector<cl::Platform> plats;
    cl::Platform::get(&plats);
    ofLog() << "Number of platforms found " << plats.size() ;

    if(plats.size() == 0){
        ofLog() << "no platform found... aborting" ;
        abort();
    }

    cl::Platform default_platform=plats[0];
    ofLog() << "Using platform: "<<default_platform.getInfo<CL_PLATFORM_NAME>() ;;

    
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    cl_context_properties properties[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0};
    
    clContext = cl::Context(CL_DEVICE_TYPE_GPU, properties);
    std::vector<cl::Device> all_devices = clContext.getInfo<CL_CONTEXT_DEVICES>();
    
    if(all_devices.size() == 0){
        ofLog() << "No device found... aborting" ;
        abort();
    }else{
        ofLog() << "Number of devices found:" << all_devices.size() ;
    }

    cl::Device device;
    cl_int targetDevice;
    for(int i = 0; i < all_devices.size(); i++ ){
        ofLog() << "Device No." << i << ": " << all_devices[i].getInfo<CL_DEVICE_NAME>() ;
        if(all_devices[i].getInfo<CL_DEVICE_NAME>() == deviceName){
            device = all_devices[i];
        }
    }

    if(device.getInfo<CL_DEVICE_NAME>().empty()){
        ofLog() << "ofxOpenCL::ofxOpenCL\nNo such device found: " << deviceName ;
        abort();
    }else{
        ofLog() << "Using device: " << device.getInfo<CL_DEVICE_NAME>() ;;
        postDeviceProfile(device);
        devVector.push_back(device);
    }
    
    
    cl::Program::Sources source(1, std::make_pair(clSource.c_str(),clSource.length()));
    clProgram = cl::Program(clContext, source);
    if(clProgram.build(devVector) != CL_SUCCESS){
        ofLog() << "ofxOpenCL::ofxOpenCL\nopen cl build error." ;
        abort();
    }
    
    clCommandQueue = cl::CommandQueue(clContext, devVector[0], 0, &err);
    if(err != CL_SUCCESS){
        ofLog(OF_LOG_ERROR) << "ofxOpenCL::ofxOpenCL\nunable to create command queue.";
        abort();
    }
    
    clKernel = cl::Kernel(clProgram, kernelName.c_str(), &err);
    if(err != CL_SUCCESS){
        ofLog(OF_LOG_ERROR) << "ofxOpenCL::ofxOpenCL\nunable to create kernel.";
        abort();
    }
}

ofxOpenCL::~ofxOpenCL(){
}

void ofxOpenCL::postDeviceProfile(const cl::Device &device) const{
    std::string deviceVersion;
    std::string deviceVendor;
    std::string deviceProfile;
    size_t max_clock_frequency;
    size_t maxWorkGroupSize;
    size_t globalMemSize;
    size_t maxWorkItemDimensions;
    size_t maxWorkItemSizes[3];
    
    device.getInfo(CL_DEVICE_VERSION, &deviceVersion);
    device.getInfo(CL_DEVICE_VENDOR, &deviceVendor);
    device.getInfo(CL_DEVICE_PROFILE, &deviceProfile);
    
    device.getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &max_clock_frequency);
    device.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxWorkGroupSize);
    device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &globalMemSize);
    device.getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &maxWorkItemDimensions);
    device.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &maxWorkItemSizes);

    
    ofLog() << "Device Version: " << deviceVersion ;
    ofLog() << "Device Vendor: " << deviceVendor ;
    ofLog() << "Device Profile: " << deviceProfile ;
    ofLog() << "Max Frequency: " << max_clock_frequency ;
    ofLog() << "Max work group size:" << maxWorkGroupSize ;
    ofLog() << "Global memory size:" << globalMemSize ;
    ofLog() << "Max Work Item Dimensions:" << maxWorkItemDimensions ;
    ofLog() << "Max Work Item Sizes:" ;
    for(int i = 0; i < maxWorkItemDimensions; i ++){
        ofLog() << " dim" << i << ":" << maxWorkItemSizes[i] ;
    }
}

void ofxOpenCL::createNewBuffer(const std::string &bufferName, const void *data, unsigned int size, cl_mem_flags flag){
    bufferMap.insert(std::pair<std::string, ofxCLBuffer>(bufferName, ofxCLBuffer(clContext, clCommandQueue, data, size, flag)));
}

void ofxOpenCL::createNewBufferGL(const std::string &bufferName, std::vector<ofVec3f> defaultVertices, cl_mem_flags flag){
    bufferGLMap.insert(std::pair<std::string, ofxCLBufferGL>(bufferName, ofxCLBufferGL(clContext, defaultVertices, flag)));
}

void ofxOpenCL::createNewBufferGL(const std::string &bufferName, std::vector<ofVec3f> defaultVertices, std::vector<ofFloatColor> defaultColors, cl_mem_flags flag){
    bufferGLMap.insert(std::pair<std::string, ofxCLBufferGL>(bufferName, ofxCLBufferGL(clContext, defaultVertices, defaultColors, flag)));
}

unsigned int  ofxOpenCL::getNumberOfBuffer(){
    return bufferMap.size();
}

unsigned int  ofxOpenCL::getNumberOfGLBuffer(){
    return bufferGLMap.size();
}

ofVbo &ofxOpenCL::getVbo(const std::string &bufferName){
    std::map<std::string, ofxCLBufferGL>::iterator it = bufferGLMap.find(bufferName);
    if(it != bufferGLMap.end()){
        return (*it).second.getVbo();
    }else{
        // need to be fixed
    }
}


void ofxOpenCL::updateBuffer(const std::string &bufferName, const void *data, unsigned int size){
    std::map<std::string, ofxCLBuffer>::iterator it = bufferMap.find(bufferName);
    if(it != bufferMap.end()){
        (*it).second.writeToCLBuffer(data, size);
    }
}

void ofxOpenCL::process(const std::vector<std::string> &bufferList){
    
    int index = 0;
    for(int i = 0; i < bufferList.size(); i++){
        std::map<std::string, ofxCLBuffer>::iterator it = bufferMap.find(bufferList[i]);
        if(it != bufferMap.end()){
            unsigned int size = (*it).second.getSize();
            clKernel.setArg(index, (*it).second.getCLBuffer());
            index++;
            continue;
        }
        
        std::map<std::string, ofxCLBufferGL>::iterator jt = bufferGLMap.find(bufferList[i]);
        if(jt != bufferGLMap.end()){
            ofxCLBufferGL::bufferMode bm = (*jt).second.getBufferMode();
            clKernel.setArg(index, (*jt).second.getCLVertexBuffer());
            index++;
            if(bm == ofxCLBufferGL::VERTEX_COLOR){
                clKernel.setArg(index, (*jt).second.getCLColorBuffer());
                index++;
            }
            continue;
        }
        
        ofLog(OF_LOG_ERROR) << "ofxOpenCL::process\n"<< bufferList[i] << " argument not found.";
    }
    
    clCommandQueue.enqueueNDRangeKernel(clKernel, cl::NullRange, ndRange, cl::NullRange);
    clCommandQueue.finish();
}
