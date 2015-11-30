# ofxOpenCL
Simplified OpenCL addon for OpenFrameworks

This OpenCL wrapper addon provides oF users a simplified interface for utilizing the power of GPU in their project.

Users of this library should only study and use member fucntions of ofxOpenCL, the constructors of other classes are defined as protected functions. Thus these classes can be instantiated only by their friend class, ofxOpenCL.

The library is exclusively tested on MacOS, and uses only one GPU. For more advanced use of OpenCL, use cl.hpp directly in your project.

## How to use

This section explains a primitive usage of ofxOpenCL with an example.

### Loading OpenCL Program
To use OpenCL. you need to prepare a program to execute in OpenCL. The source file of this code should has a .cl extension. 

In the oF program, you can load .cl file onto a ofBuffer by using ofBufferFromFile function.

For example, if your .cl file is located in the data folder of the project, the following code will do the job.

```
auto buffer = ofBufferFromFile(ofToDataPath("Example.cl")); 
```
now the "buffer" is keeping a OpenCL program as a string. 

### Compilation
Then, you pass the code string as the second argument of the constructor of the ofxOpenCL. Then, the program will be compiled.

```
auto openCL = ofxOpenCL("GeForce GT 750M", buffer.getText(), "hello_kernel", {10}, error);
```

You can check if the result of compilation by passing an error reference variable. if error is 0. The compilation was successful.

As a the first parameter of the function, you can pass the name of GPU device, the default GPU device will be used if not specified.

The third parameter is the name of CL function, defined in .cl file, that you want to execute with the instantiated ofxOpenCL.

The fourth parameter is NDRange. This must be given as std::vector<int> and the size of the vector should be 1, 2 or 3. This parameter determines the dimensionality and the number of workitems in OpenCL.

if the given NDRange exceeds the maximum number of work item of the designated device, the constructor returns an error.

### Arguments

To pass parameters to cl fucntions, we need to use ofxCLBuffer

```
vector<int> a = {0,1,2,3,4,5,6,7,8,9};
openCL.createNewBuffer("a", a, CL_MEM_WRITE_ONLY);
```
ofxOpenCL::createNewBuffer is a template function, that deduces the data type of argument through the data type of the given vector.







