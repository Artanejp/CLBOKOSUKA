// It is OpenCL test.
#include <stdlib.h>
#include <CL/cl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define SRCSIZE 1024*1024
#define SRCFILE "./rand.cl"
#define NRANDS 65536
#define LOGSIZE 1024*1024
int main(void)
{
    cl_platform_id platform_id = NULL;
    cl_uint ret_num_platforms;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    cl_mem result = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_event event;
    size_t codeSize;
    char *srcStr;
    cl_int ret;
    FILE *fp;
    int seed;
    int init;
    int nRands = NRANDS * 16;
    int *nBuf = NULL;
    int i, j;
    struct timeval tv;
    struct timezone tz;
    char *logBuf = NULL;


    gettimeofday(&tv, &tz);
    seed = (int)tv.tv_sec;
    init = (int)tv.tv_usec;
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
                         &ret_num_devices);
    context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueue(context, device_id,
                                         CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ret);
   
    nBuf = (int *)malloc((nRands + 1)* sizeof(int));
    logBuf = (char *)malloc(LOGSIZE * sizeof(char));
    memset(logBuf, 0x00, LOGSIZE * sizeof(char));
   
    srcStr = (char *)malloc(SRCSIZE);
    fp = fopen(SRCFILE, "r");
    codeSize = fread(srcStr, 1, SRCSIZE - 1, fp);
    fclose(fp);

    
    // Build prepare program ID
    program = clCreateProgramWithSource(context, 1, (const char **)&srcStr,
                                        (const size_t *)&codeSize, &ret);
    printf("Build Result=%d\n", ret);

    // Compile from source
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    printf("Compile Result=%d\n", ret);
    if(ret != CL_SUCCESS) {
       ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 
				   LOGSIZE - 1, (void *)logBuf, NULL);
       if(logBuf != NULL) printf("Build Log:\n%s\n", logBuf);
    }
   
    // Create Kernel
    kernel = clCreateKernel(program, "CalcRand", &ret);

    
    result  = clCreateBuffer(context, CL_MEM_READ_WRITE,
                               (nRands + 1) * sizeof(int), NULL, &ret);


    // Set Arg
    ret = clSetKernelArg(kernel, 0, sizeof(int), (void *)&seed);
    ret = clSetKernelArg(kernel, 1, sizeof(int), (void *)&init);
    ret = clSetKernelArg(kernel, 2, sizeof(int),    (void *)&nRands);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&result);

    // DO RAND
    ret = clEnqueueTask(command_queue, kernel, 0, NULL, &event);

    // Copy Devidce to HOST
    ret = clEnqueueReadBuffer(command_queue, result, CL_TRUE, 0,
                              nRands * sizeof(int),
                              nBuf, 1, &event, NULL);


    // Free
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(result);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    // Printout results
    printf("Answer:\n");
    for(i=0; i < NRANDS; i++)  {
	for(j = 0; j < 16 ; j++) {
	     printf( "%08x ", nBuf[i * 16 + j] );
	  }
     printf( "\n" );
    }

    // Free buffer
    free(logBuf);
    free(nBuf);
    free(srcStr);

    return 0;
}

