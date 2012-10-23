// This is OpenCL test.
// Lifegame using OpenCL.
// I tested on Linux box with Phenom2 955 and AMD Radeon HD7700 and proprietary driver.
// (C) 2012 K.Ohta <whatisthis.sowhat@gmail.com>
// History: Oct 19,2012 Initial.
// License: Apache License 2.0

#include <signal.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <SDL/SDL.h>

#define SRCFILE "./lifegame.cl"
#define LOGSIZE 1024*1024
#define BOARD_WIDTH 512
#define BOARD_HEIGHT 512
#define SRCSIZE 65536*4

static unsigned char *board_s = NULL;
cl_platform_id platform_id = NULL;
cl_uint ret_num_platforms;
cl_device_id device_id = NULL;
cl_uint ret_num_devices;
cl_context context = NULL;
cl_command_queue command_queue = NULL;
static cl_mem src = NULL;
static cl_mem dst = NULL;
static cl_mem smem;
static cl_program program = NULL;
static cl_kernel kernel = NULL;
static cl_event event_exec;
static cl_event event_buf1;
static cl_event event_buf2;
static cl_event event_copy;

static char *srcStr;
static char *pixmem = NULL;

extern SDL_Surface *surface;
extern SDL_Surface *ssource;
extern void printresult(unsigned char *p, int turn, int w, int h);

extern void *SDLDrv_Init(int w, int h);
extern void SDLDrv_End(void);
extern void SDLDrv_result(cl_mem smem, cl_event *waitevent, int turn, int w, int h);
extern cl_mem SDLDrv_CLInit(int w, int h);

// destroy resources, you must call at all ending.
int destroy_rss(int n)
{
    cl_int ret;
   
    SDLDrv_End();
    // Free
    if(kernel != NULL) ret = clReleaseKernel(kernel);
    if(program != NULL) ret = clReleaseProgram(program);
    if(src != NULL) ret = clReleaseMemObject(src);
    if(dst != NULL) ret = clReleaseMemObject(dst);
    if(command_queue != NULL) ret = clReleaseCommandQueue(command_queue);
    if(context != NULL) ret = clReleaseContext(context);
    if(board_s != NULL) free(board_s);
    if(pixmem != NULL)  free(pixmem);
    exit(0);
}

static void BuildBoardRandom(unsigned char *board, int w, int h)
{
    struct timeval tv;
    struct timezone tz;
    unsigned int seed;
    int x, y;
   
    gettimeofday(&tv, &tz);
    seed = (int)tv.tv_usec + (int)tv.tv_sec;
    srand(seed);
//    seed = rand();
    for(y = 0; y < h; y++) {
	for(x = 0; x < w; x++) {
	    if(rand_r(&seed) < (RAND_MAX / 6)) {
		board[x + y * w] = 1; // Alive
	    } else {
		board[x + y * w] = 0; // Dead
	    }
	   
	}
       
    }
   
}

// Running one turn.
static void DoTurn(int parallel)
{
    int w, h;
    cl_int ret;
    int pitch;
    size_t *goff = NULL;
    size_t gws[] = {parallel};
    size_t lws[] = {4};
   

    w = BOARD_WIDTH;
    h = BOARD_HEIGHT;
    if(ssource != NULL) {
       pitch = ssource->pitch;
    } else {
       pitch = w * sizeof(Uint32);
    }
   
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&src);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&dst);
    ret = clSetKernelArg(kernel, 2, sizeof(int),    (void *)&w);
    ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&h);
    ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&smem);
    ret = clSetKernelArg(kernel, 5, sizeof(int), (void *)&pitch);

    // Execute kernel (lifegamecore(), in lifegame.cl)
//    ret = clEnqueueTask(command_queue, kernel, 0, NULL, &event_exec);
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, 
				 goff, gws, lws, 
				 0, NULL, &event_exec);
    if(ret != CL_SUCCESS) {
       printf("Error on running program\n");
       destroy_rss(0);
    }



    ret = clEnqueueCopyBuffer(command_queue, dst, src, 0, 0,
                              w * h * sizeof(unsigned char), 
			      1, &event_exec, &event_buf2);
    if(ret != CL_SUCCESS) {
       printf("Error on Copy buffer\n");
       destroy_rss(0);
    }


   
}



int main(void)
{
    size_t codeSize;
    cl_int ret;
    cl_event event;
    FILE *fp;
    int seed;
    int init;
    int i, j;
    int w, h;
    char *logBuf = NULL;
    int turn;
    int pitch;
    w = BOARD_WIDTH;
    h = BOARD_HEIGHT;

    // Init environment
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
                         &ret_num_devices);
    context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueue(context, device_id,
                                         CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ret);
   
    
    // Read CL sourcecode from external file.
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
    if(ret != CL_SUCCESS) {  // Printout error log.
       logBuf = (char *)malloc(LOGSIZE * sizeof(char));
       memset(logBuf, 0x00, LOGSIZE * sizeof(char));
       
       ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 
				   LOGSIZE - 1, (void *)logBuf, NULL);
       if(logBuf != NULL) printf("Build Log:\n%s\n", logBuf);
       free(logBuf);
       destroy_rss(0);
    }
   
    // Initialize board.
    board_s = (unsigned char *)malloc(BOARD_WIDTH * BOARD_HEIGHT * sizeof(unsigned char));
    if(board_s == NULL) destroy_rss(0);
    BuildBoardRandom(board_s, BOARD_WIDTH, BOARD_HEIGHT);
   
    // Prepare to execution.
    // Create Kernel
//    kernel = clCreateKernel(program, "lifegamecore", &ret);
    kernel = clCreateKernel(program, "lifegamecore_parallel", &ret);

    // Make two buffer for OpenCL, in (src) and out (dst).
    src  = clCreateBuffer(context, CL_MEM_READ_WRITE,
                               BOARD_WIDTH * BOARD_HEIGHT * sizeof(unsigned char), NULL, &ret);
    dst  = clCreateBuffer(context, CL_MEM_READ_WRITE,
                               BOARD_WIDTH * BOARD_HEIGHT * sizeof(unsigned char), NULL, &ret);
    if(SDLDrv_Init(BOARD_WIDTH, BOARD_HEIGHT) == NULL) destroy_rss(0);
    smem = SDLDrv_CLInit(BOARD_WIDTH, BOARD_HEIGHT);
    if(smem == NULL) destroy_rss(0);
   
    // Set arguments for calling CL code.


    // Wait for complete
    if(ret != CL_SUCCESS) {
       printf("Error on Send buffer\n");
       destroy_rss(0);
    }

    // Printout initial condition of bode.   
   // Resist signal handler ... You *should* resist.
    signal(15, (sighandler_t)destroy_rss); // SIGTERM (^C)
    signal(9, (sighandler_t)destroy_rss);  // SIGKILL

    // First, put source data to buffer ("src")  from board.
    ret = clEnqueueWriteBuffer(command_queue, src, CL_TRUE, 0,
                              w * h * sizeof(unsigned char), (void *)board_s
                              , 0, NULL, &event_buf1);
    // Wait for complete
    if(ret != CL_SUCCESS) {
       printf("Error on Send buffer\n");
       destroy_rss(0);
    }
    // Printout results
    turn = 1;
    while(1) {
        SDL_Delay(100);// Wait 100ms.
        DoTurn(128);
        SDLDrv_result(smem, &event_buf2, turn, BOARD_WIDTH, BOARD_HEIGHT);
        turn++;
    }
   
    // Free buffer
    destroy_rss(0);

}

