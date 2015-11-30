 //
//  ofxOpenCL.cpp
//
//  Created by Chikashi Miyama on 04/10/14.
//
//
#include "ofxOpenCL.h"
#include <OpenGL/OpenGL.h>
#include <algorithm>
#include <sstream>
#include <map>

using namespace std;

bool ofxOpenCL::checkPlatforms(){
    cl::Platform::get(&platforms);
    if(platforms.empty()){
        ofLog(OF_LOG_FATAL_ERROR) << "ofxOpenCL: No platform found... ";
        return true;
    };
    return false;
}

bool ofxOpenCL::checkDevices(const vector<cl::Device> &gpuDevices){
    if(gpuDevices.size() == 0){
        ofLog(OF_LOG_FATAL_ERROR) << "ofxOpenCL: No device found... " ;
        return true;
    }
    return false;
}

bool ofxOpenCL::checkNDRange(const vector<unsigned int> &requestedRange, const Device &targetDevice){
    auto itemSizes = targetDevice.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
    for(int i = 0; i < requestedRange.size(); i++){
        if(requestedRange[i] >= itemSizes[i]){
            ofLog(OF_LOG_FATAL_ERROR) << "ofxOpenCL: NDRange exceeds limit...";
            return true;
        }
    }
    return false;
}
void ofxOpenCL::setNDRange(const vector<unsigned int> &requestedRange){
    switch (requestedRange.size()) {
        case 1:
            ndRange = NDRange(requestedRange[0]); break;
        case 2:
            ndRange = NDRange(requestedRange[0], requestedRange[1]); break;
        case 3:
            ndRange = NDRange(requestedRange[0], requestedRange[1], requestedRange[2]); break;
    }
}

ofxOpenCL::ofxOpenCL(const string &clSource, const string &kernelName, const vector<unsigned int> &requestedRange, bool &error){
    error = false;
    cl_int err;
    if(checkPlatforms()){ error = true; return; }
    
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    cl_context_properties properties[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0};
    
    clContext = cl::Context(CL_DEVICE_TYPE_GPU, properties);
    vector<cl::Device> gpuDevices = clContext.getInfo<CL_CONTEXT_DEVICES>();
    if(checkDevices(gpuDevices)){ error = true; return; } // at least one GPU device?
    
    cl::Device device = gpuDevices[0]; // use first device
    ofLog() << "use " << device.getInfo<CL_DEVICE_NAME>() << " for OpenCL.";

    
    if(checkNDRange(requestedRange, device)){ error = true; return; }
    setNDRange(requestedRange);
    
    
    cl::Program::Sources source(1, make_pair(clSource.c_str(),clSource.length()));
    clProgram = cl::Program(clContext, source);

    if(clProgram.build(gpuDevices) != CL_SUCCESS){return; }
    clCommandQueue = cl::CommandQueue(clContext, device, 0, &err);
    if(error != CL_SUCCESS){return;}
    clKernel = cl::Kernel(clProgram, kernelName.c_str(), &err);
    if(error != CL_SUCCESS){return;}
}

ofxOpenCL::~ofxOpenCL(){
}



void ofxOpenCL::process(const vector<string> &bufferList){
    
    int index = 0;
    for(int i = 0; i < bufferList.size(); i++){
        map<string, shared_ptr<_ofxCLBuffer>>::iterator it = bufferMap.find(bufferList[i]);
        if(it != bufferMap.end()){
            auto ofCLBuffer =  (*it).second;
            clKernel.setArg(index, ofCLBuffer->getRawBuffer());
            index++;
            continue;
        }
        
        map<string, ofxCLBufferGL>::iterator jt = bufferGLMap.find(bufferList[i]);
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

const string ofxOpenCL::getDeviceInfo(){
    stringstream ss;
    ss << "Number of platforms: " << platforms.size();
    for_each(platforms.begin(), platforms.end(), [&ss](const cl::Platform &platform){
        ss << "name: " << platform.getInfo<CL_PLATFORM_NAME>() << endl;
        ss << "version: " << platform.getInfo<CL_PLATFORM_VERSION>()  << endl;
        ss << "profile: " << platform.getInfo<CL_PLATFORM_PROFILE>()  << endl;
        ss << "vendor: " << platform.getInfo<CL_PLATFORM_VENDOR>()  << endl;
        //ofLog() << "extensions: " << platform.getInfo<CL_PLATFORM_EXTENSIONS>();
        
        vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        
        if(devices.empty()){ ofLog(OF_LOG_ERROR) << "no devices found...";}
        else{
            ss << "number of devices: " << devices.size() << endl;
            for_each(devices.begin(), devices.end(), [&ss](const cl::Device &device){
                
                ss << "    name: " << device.getInfo<CL_DEVICE_NAME>() << endl;
                ss << "    version: " << device.getInfo<CL_DEVICE_VERSION>() << endl;
                ss << "    vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << endl;
                ss << "    vendor ID: " << device.getInfo<CL_DEVICE_VENDOR_ID>() << endl;
                ss << "    max compute unit: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << endl;
                ss << "    max work item dimensions: " << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << endl;
                auto itemSizes = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
                string str;
                for_each(itemSizes.begin(), itemSizes.end(), [&str](unsigned long size){
                    str.append(to_string(size));
                    str.append(" ");
                });
                ss << "    max work item sizes: " << str << endl;
                ss << "    max work group size: " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << endl;
                
                ss << "    max clock frequency: " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << endl;
                ss << "    max memory allocation size (MB): " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() / 1024/ 1024 << endl ;
                
                bool imageSupport = device.getInfo<CL_DEVICE_IMAGE_SUPPORT>();
                ss << "    image support: " << (imageSupport ? "yes" : "no") << endl;
                if(imageSupport){
                    ss << "    max read image args: " << device.getInfo<CL_DEVICE_MAX_READ_IMAGE_ARGS>() << endl;
                    ss << "    max write image args: " << device.getInfo<CL_DEVICE_MAX_WRITE_IMAGE_ARGS>() << endl;
                    ss << "    image 2D max width: " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>() << endl;
                    ss << "    image 2D max height: " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>() << endl;
                    ss << "    image 3D max width: " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_WIDTH>() << endl;
                    ss << "    iamge 3D max height: " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_HEIGHT>() << endl;
                    ss << "    max parameter size: " << device.getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>() << endl;
                }
                ss << "    ----------------------------" << endl;
            });
        }
    });
    return move(ss.str());
}
