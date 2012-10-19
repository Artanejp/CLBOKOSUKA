// This is OpenCL test.
// Lifegame using OpenCL.
// I tested on Linux box with Phenom2 955 and AMD Radeon HD7700 and proprietary driver.
// (C) 2012 K.Ohta <whatisthis.sowhat@gmail.com>
// History: Oct 19,2012 Initial.
// Licence: BSD

#include <signal.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


#define SRCFILE "./lifegame.cl"
#define LOGSIZE 1024*1024
#define BOARD_WIDTH 80
#define BOARD_HEIGHT 25
#define SRCSIZE 65536*4

static cl_platform_id platform_id = NULL;
static cl_uint ret_num_platforms;
static cl_device_id device_id = NULL;
static cl_uint ret_num_devices;
static cl_context context = NULL;
static cl_command_queue command_queue = NULL;
static cl_mem src = NULL;
static cl_mem dst = NULL;
static cl_program program = NULL;
static cl_kernel kernel = NULL;
static unsigned char *board_s = NULL;
static char *srcStr;

int destroy_rss(int n)
{
    cl_int ret;
    // Free
    if(kernel != NULL) ret = clReleaseKernel(kernel);
    if(program != NULL) ret = clReleaseProgram(program);
    if(src != NULL) ret = clReleaseMemObject(src);
    if(dst != NULL) ret = clReleaseMemObject(dst);
    if(command_queue != NULL) ret = clReleaseCommandQueue(command_queue);
    if(context != NULL) ret = clReleaseContext(context);
    if(src != NULL) free(src);
    if(dst != NULL) free(dst);
    if(board_s != NULL) free(board_s);
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
	    if(rand_r(&seed) < (RAND_MAX / 8)) {
		board[x + y * w] = 1; // Alive
	    } else {
		board[x + y * w] = 0; // Dead
	    }
	   
	}
       
    }
   
}


void DoTurn(void)
{
    int w, h;
    cl_int ret;
    cl_event event_buf;
    cl_event event_exec;

    w = BOARD_WIDTH;
    h = BOARD_HEIGHT;


    ret = clEnqueueWriteBuffer(command_queue, src, CL_TRUE, 0,
                              w * h * sizeof(unsigned char), (void *)board_s
                              , 0, NULL, &event_buf);
    // Wait for complete

    // DO
    ret = clEnqueueTask(command_queue, kernel, 0, NULL, &event_exec);
    // Wait for complete
//    ret = clFinish(command_queue);
    // Copy Devidce to HOST
    ret = clEnqueueReadBuffer(command_queue, src, CL_TRUE, 0,
                              w * h * sizeof(unsigned char), (void *)board_s
                              , 0, NULL, &event_buf);
}

void printresult(unsigned char *p, int turn, int w, int h)
{
   int x;
   int y;
   int addr = 0;
   printf("\nTurn %d:\n", turn);
   for(y = 0; y < h ; y++) {
	for(x = 0; x < w; x++) { 
	   if(p[addr] == 0) {
		printf(" ");
	   } else {
		printf("O");
	   }
	   addr++;
	}
       printf("\n");
   }
   printf("\n");
}


int main(void)
{
    size_t codeSize;
    cl_int ret;
    FILE *fp;
    int seed;
    int init;
    int i, j;
    int w, h;
    char *logBuf = NULL;
    int turn;
    w = BOARD_WIDTH;
    h = BOARD_HEIGHT;

   
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id,
                         &ret_num_devices);
    context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
    command_queue = clCreateCommandQueue(context, device_id,
                                         CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ret);
   
    
   
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
       logBuf = (char *)malloc(LOGSIZE * sizeof(char));
       memset(logBuf, 0x00, LOGSIZE * sizeof(char));
       
       ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 
				   LOGSIZE - 1, (void *)logBuf, NULL);
       if(logBuf != NULL) printf("Build Log:\n%s\n", logBuf);
       free(logBuf);
       destroy_rss(0);
    }
    board_s = (unsigned char *)malloc(BOARD_WIDTH * BOARD_HEIGHT * sizeof(unsigned char));
    if(board_s == NULL) destroy_rss(0);
    BuildBoardRandom(board_s, BOARD_WIDTH, BOARD_HEIGHT);    
    // Create Kernel
    kernel = clCreateKernel(program, "lifegamecore", &ret);

    
    src  = clCreateBuffer(context, CL_MEM_READ_WRITE,
                               BOARD_WIDTH * BOARD_HEIGHT * sizeof(unsigned char), NULL, &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&src);
//    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&dst);
    ret = clSetKernelArg(kernel, 1, sizeof(int),    (void *)&w);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&h);

    // Set Arg
    printresult(board_s, 0, BOARD_WIDTH, BOARD_HEIGHT);
    signal(15, (sighandler_t)destroy_rss);
    signal(9, (sighandler_t)destroy_rss);

    // Printout results
    turn = 1;
    while(1) {
	sleep(1);
        DoTurn();
        printresult(board_s, turn, BOARD_WIDTH, BOARD_HEIGHT);
        turn++;
    }
   
    // Free buffer
    destroy_rss(0);

}

