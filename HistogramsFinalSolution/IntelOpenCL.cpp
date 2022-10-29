#define _CRT_SECURE_NO_DEPRECATE
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/opencl.hpp>


#endif
#define MAX_SOURCE_SIZE (0x100000)
#define BUFFER_DEF_SIZE (64)

struct Timer {

    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::chrono::duration<float> duration;

    Timer() {
        start = std::chrono::high_resolution_clock::now();
    }

    void Stop() {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        float ms = duration.count() * 1000.0f;
        std::cout << "Timer took: " << ms << "ms" << std::endl;
        std::cout << "_________________________________________________________________________\n";

    }

};

int main() {

   
   // int a = 3 ^ 2;
  /*  for (int i = 0; i < LIST_SIZE; i++)
    {
        A[i] = i;
       B[i] = 0;
    }*/
    std::vector<cl::Platform> platform;
    cl::Platform::get(&platform);


    FILE* open_yuv = fopen("DOTA2_I420.yuv", "rb");
    const int width = 1920;
    const int height = 1080;
    double  UandV = ((double)width / 2) * ((double)height / 2);
     int Yint = width * height; 
    //std::cout << UandV << "\n";
    int  nFrameSize = width * height * 1.5;
    int frameNo = 0;
    unsigned char* frameBufferY = new unsigned char[width * height];
    unsigned char* newframeBufferU = new unsigned char[UandV];
    unsigned char* newframeBufferV = new unsigned char[UandV];
    int LIST_SIZE = Yint;
    int* A = new int[LIST_SIZE];
    int* B = new int[LIST_SIZE];
    double* status = new double[LIST_SIZE];

    std::vector<int> Y_values;
    std::vector<int> U_values;
    std::vector<int> V_values;
    std::vector<int> full_frame_values;


    fseek(open_yuv, nFrameSize * frameNo, SEEK_SET);

    fread(frameBufferY, 1, width * height, open_yuv);
    fread(newframeBufferU ,1, UandV, open_yuv);
    fread(newframeBufferV, 1, UandV, open_yuv);

    //Y = width * height 
    // u = width/2 * (height/2)
    // v =width/2 * (height/2)
    //int full_frame = (width * height) + int(UandV);
   for (int count = 0; count < UandV; count++) {
        //y channel
        //std::cout << (int)frameBuffer[count] << "," ;
        // u channel
     //   std::cout << (int)newframeBufferU[count] << ",";
        // v channel
        //
       // std::cout << (int)newframeBufferV[count] << "," ;
       U_values.push_back((int)newframeBufferU[count]);
       V_values.push_back((int)newframeBufferV[count]);
    }
  for (int count = 0; count < Yint; count++) {
       Y_values.push_back((int)frameBufferY[count]);
      // std::cout << Y_values[count] << "   ";
   }
  fclose(open_yuv);

  FILE* full_frame = fopen("DOTA2_I420.yuv", "rb");
  unsigned char* full_frame_buffer = new unsigned char[nFrameSize];

  fseek(full_frame, nFrameSize * frameNo, SEEK_SET);
  fread(full_frame_buffer, 1, nFrameSize, full_frame);

  for (int count = 0; count < nFrameSize; count++) {

      full_frame_values.push_back((int)full_frame_buffer[count]);

  }
  /*
  for (int count = 0; count < 10; count++) {
      std::cout << full_frame_values[count] << "\n";
  }
  */

  fclose(full_frame);

  
   std::cout << "_________________________________________________________________________\n";
   std::cout << Y_values.size() << "\n";
   std::cout << U_values.size() << "\n";
   std::cout << V_values.size() << "\n";

   std::cout << "_________________________________________________________________________\n";

    // std::cout << "Using platform:" << platform.getInfo.<CL_PLATFORM_NAME>();

     // Check to set see what devices are avavible on the platform

    cl::Platform default_platform = platform[0];
    cl_command_queue_properties properties[3] = { CL_QUEUE_PROFILING_ENABLE,1,0};
    cl::Event event;
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
   // std::cout << "Using Version:  " << default_platform.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << "\n"

    std::vector<cl::Device> all_devices;
    //check if ther are devices on the platform
    default_platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);
    if (all_devices.size() == 0) {
        std::cout << "NO devices found. \n";
    }

    cl::Device default_device = all_devices[0];
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

    //setting up context to managage devices
    int err;
    cl::Context context(CL_DEVICE_TYPE_GPU);
  //  cl_context context_2 = clGetPlatform;
   // cl_context context_2 = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);

    cl::Program::Sources sources;


    std::ifstream sourceFIle("vector_average_kernel.cl");
    std::string sourceCode(std::istreambuf_iterator<char>(sourceFIle), (std::istreambuf_iterator<char>()));

    std::cout << "Program Kernel: " << sourceCode;

    //sources.push_back(sourceCode);
    cl::Program program(context, sourceCode);

    //err = program.build(default_device);
    if (program.build({ default_device }) != CL_SUCCESS) {
        std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
        exit(1);
    }



// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15     //Allocating Memory on the the Device
    // vectors the size of 16
   // std::vector<int> A_host;
  //  std::vector<int> B_host;
   // std::vector<int> C_host;
   // std::vector<int> D_host;

  /*  for (int count = 0; count < ; count++) {
        A_host.push_back(count);
        B_host.push_back(count);
     //   C_host.push_back(0);

    }*/
    //int LIST_SIZE = (int)UandV;

    cl::Kernel kernel(program, "vector_average");
    cl::Kernel kernel_2(program, "vector_call");
   // int LIST_SIZE = A_host.size();

    cl::Buffer A_Buffer(context, CL_MEM_READ_WRITE, sizeof(double) * LIST_SIZE);
    cl::Buffer B_Buffer(context, CL_MEM_READ_WRITE, sizeof(double) * LIST_SIZE);
    cl::Buffer C_Buffer(context, CL_MEM_READ_ONLY, sizeof(double) * LIST_SIZE);
    cl::Buffer D_Buffer(context, CL_MEM_READ_WRITE, sizeof(double) * LIST_SIZE);

    cl::Buffer U_Buffer(context, CL_MEM_READ_WRITE, sizeof(double) * LIST_SIZE);
    cl::Buffer V_Buffer(context, CL_MEM_READ_WRITE, sizeof(double) * LIST_SIZE);
    cl::Buffer full_Buffer(context, CL_MEM_READ_WRITE, sizeof(double)* nFrameSize);




    cl::Buffer Vvalues(context, CL_MEM_READ_WRITE, sizeof(int) * Yint);
    cl::Buffer Uvalues(context, CL_MEM_READ_WRITE, sizeof(double) * UandV);

    // Creating a Queue
   // Timer timer;
    //int err;
    //cl::QueueProperties::Profiling;
    cl::CommandQueue queue(context, default_device, cl::QueueProperties::Profiling);
    queue.enqueueWriteBuffer(Vvalues, CL_TRUE, 0, sizeof(int) * LIST_SIZE, &Y_values[0], NULL, NULL);
    queue.enqueueWriteBuffer(B_Buffer, CL_TRUE, 0, sizeof(int) * LIST_SIZE, B, NULL, NULL);
    queue.enqueueWriteBuffer(full_Buffer, CL_TRUE, 0, sizeof(int) * nFrameSize, &full_frame_values[0], NULL, NULL);

    kernel.setArg(0, full_Buffer);
    kernel.setArg(1, B_Buffer);
   // kernel.setArg(1, C_Buffer);
   // kernel.setArg(3, D_Buffer);
   // kernel.setArg(1, A_Buffer);
    
    kernel_2.setArg(0, full_Buffer);
    kernel_2.setArg(1, D_Buffer);
    kernel_2.setArg(2, Uvalues);
    kernel_2.setArg(3, Vvalues);


    int length_Y = 1080 ;
    int width_Y = 1920 ;
    //cl::NDRange global(LIST_SIZE);
    cl::NDRange local(8,8);
    cl::NDRange local_2(4, 4);

    cl::NDRange global(1);
    cl::NDRange global_2(1080,1920);
    cl::NDRange global_3(length_Y / 2, width_Y / 2);

    //cl::NullRangeNullRange
    double* c = new double[LIST_SIZE];
    double* a = new double[LIST_SIZE];

    double* d = new double[UandV];
   // cl::CommandQueue commandQueue(context, devices[0], cl::QueueProperties::Profiling, &err);
    /*
    
    cl::CommandQueue commandQueue(context, devices[0], cl::QueueProperties::Profiling, &err);


y_ElapsedTimeAverageGPU = (1e-6) * (event.getProfilingInfo<CL_PROFILING_COMMAND_END>() - event.getProfilingInfo<CL_PROFILING_COMMAND_START>());
    
    */

   // y_ElapsedTimeAverageGPU = (1e-6) * (event.getProfilingInfo<CL_PROFILING_COMMAND_END>() - event.getProfilingInfo<CL_PROFILING_COMMAND_START>());
  //  queue.enqueueReadBuffer(C_Buffer, CL_TRUE, 0, sizeof(double) * LIST_SIZE, c,NULL, NULL);
  // 
//_______________________________________________________________________________________________________________
    std::cout << "_________________________________________________________________________\n";
    std::cout << "Running a Single With Dyanmic Paralisam \n";

    cl_ulong start_time, end_time;

    
    // time;
    queue.enqueueNDRangeKernel(kernel_2, cl::NullRange, global, cl::NullRange, NULL, &event);
    event.wait();

    double time_now= (1e-6) * (event.getProfilingInfo <CL_PROFILING_COMMAND_END>() - event.getProfilingInfo < CL_PROFILING_COMMAND_START>());

   // time.Stop();

    queue.enqueueReadBuffer(D_Buffer, CL_TRUE, 0, sizeof(double) * LIST_SIZE, c, NULL, NULL);
    queue.enqueueReadBuffer(Uvalues, CL_TRUE, 0, sizeof(double) * UandV, d, NULL, NULL);
//______________________________________________________________________________________________________________

//_______________________________________________________________________________________________________________
    std::cout << "_________________________________________________________________________\n";
    std::cout << "Running a two kernels Without Dyanmic Paralisam \n";

   // float time_now;
    //Timer time_2;
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_2, local, NULL, NULL);

    event.wait();
    //clFinish(queue);

   // start_time = (1e-6) * (event.getProfilingInfo <CL_PROFILING_COMMAND_END>() - event.getProfilingInfo < CL_PROFILING_COMMAND_START>());

  //  clGetEventProfilingInfo
    //start_time = (1e-6) * (event.getProfilingInfo <CL_PROFILING_COMMAND_END>() - event.getProfilingInfo < CL_PROFILING_COMMAND_START>());
  //  end_time = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
   // double blue = event.getProfilingInfo < CL_PROFILING_COMMAND_START>();
    //time_2.Stop();
   // std::cout <<"Time start: " << start_time << std::endl;
  //  std::cout << "Time end_time" << end_time << std::endl;
   // std::cout << "Time blue " << blue << std::endl;
    std::cout << "Time blue " << time_now << std::endl;


   // Timer time_3;
    queue.enqueueNDRangeKernel(kernel, global_2, global_3, local_2, NULL, NULL);
    //time_now = (1e-6) * (event.getProfilingInfo <CL_PROFILING_COMMAND_END>() - event.getProfilingInfo < CL_PROFILING_COMMAND_START>());

    //time_3.Stop();
    event.wait();

    queue.enqueueReadBuffer(B_Buffer, CL_TRUE, 0, sizeof(double) * LIST_SIZE, a, NULL, NULL);
    event.wait();

    
  //_______________________________________________________________________________________________________________


   /* for (int count = 0; count < 10; count++) {
      // if(c[count] > 0)
        std::cout << " c  Luma Values Average   "  << c[count] <<  "\n";
    }*/
    for (int count = 0; count < 10; count++) {
        // if(c[count] > 0)
       std::cout << " D  Luma Values Average   " << d[count] << "\n";

        std::cout << " c  Luma Values Average   " << c[count] << "\n";
       // std::cout << " a  Luma Values Average   " << a[count] << "\n";

    }


    return 0;







}