// This is OpenCL test.
// Lifegame using OpenCL.
// I tested on Linux box with Phenom2 955 and AMD Radeon HD7700 and proprietary driver.
// (C) 2012 K.Ohta <whatisthis.sowhat@gmail.com>
// History: Oct 19,2012 Initial.
//          Oct 21,2012 Split to this file.
// License: Apache License 2.0

// Printout board to console.
#include <stdio.h>
#include <SDL/SDL.h>
#include <CL/cl.h>

static SDL_Surface *surface = NULL;
static char *ssource;

#define SRCFILE "./board2surface.cl"
#define LOGSIZE 1024*1024
#define BOARD_WIDTH 512
#define BOARD_HEIGHT 512
#define SRCSIZE 65536*4

static unsigned char *board_s = NULL;
static cl_mem src = NULL;
static cl_mem dst = NULL;
static cl_program program = NULL;
static cl_kernel kernel = NULL;

extern cl_platform_id platform_id;
extern cl_uint ret_num_platforms;
extern cl_device_id device_id;
extern cl_uint ret_num_devices;
extern cl_context context;
extern cl_command_queue command_queue;


extern int destroy_rss(int n);

void *SDLDrv_Init(int w, int h)
{
   SDL_Init(SDL_INIT_EVERYTHING);
   surface = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_ASYNCBLIT);
   return (void *)surface;
}

cl_mem SDLDrv_CLInit(SDL_Surface *s)
{
   cl_int ret;
   size_t codeSize;
   int pitch;
   if(s == NULL) return NULL;
    dst  = clCreateBuffer(context, CL_MEM_READ_WRITE,
                               s->h * s->pitch, NULL, &ret);
    pitch = s->pitch;
   
   return dst;
}



void SDLDrv_End(void)
{
   cl_int ret;
   if(surface != NULL) SDL_FreeSurface(surface);
       // Free
    if(kernel != NULL) ret = clReleaseKernel(kernel);
    if(program != NULL) ret = clReleaseProgram(program);
    if(src != NULL) ret = clReleaseMemObject(src);
    if(ssource != NULL) free(ssource);
   surface = NULL;
}


void SDLDrv_result(unsigned char *p, int turn, int w, int h)
{
   int x;
   int y;
   int addr = 0;
   int addr2;
   Uint32 *q;
   
   printf("\nTurn %d: Tick %d\n", turn, SDL_GetTicks());
   if(surface == NULL) return;
//   SDL_LockSurface(surface);
//   q = (Uint32 *)surface->pixels;
//   for(y = 0; y < h ; y++) {
//      addr2 = surface->pitch * y / sizeof(Uint32);
//	for(x = 0; x < w; x++) { 
//	   if(p[addr] == 0) {
//	      q[addr2] = 0xff000000;
//	   } else {
//	      q[addr2] = 0xffffffff;
//	   }
//	   addr++;
//	   addr2++;
//	}
//   }
//   SDL_UnlockSurface(surface);
//   SDL_UpdateRect(surface, 0, 0, w, h);
}

   