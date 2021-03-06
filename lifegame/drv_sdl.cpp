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
#include <GL/gl.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>

SDL_Surface *surface = NULL;
SDL_Surface *ssource = NULL;

#define LOGSIZE 1024*1024
#define BOARD_WIDTH 512
#define BOARD_HEIGHT 512
#define SRCSIZE 65536*4

static cl_mem mtexture = NULL;
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
// 20121022 Use OpenGL
   SDL_Init(SDL_INIT_EVERYTHING);

   surface = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
//   ssource = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 32,
//			    0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
       
   return (void *)surface;
}

cl_mem SDLDrv_CLInit(int w, int h)
{
   cl_int ret;
   cl_mem dst;
   dst  = clCreateBuffer(context, CL_MEM_READ_WRITE,
                         w * h * sizeof(Uint32), NULL, &ret);
   
   return dst;
}



void SDLDrv_End(void)
{
   cl_int ret;
   if(surface != NULL) SDL_FreeSurface(surface);
       // Free
    if(kernel != NULL) ret = clReleaseKernel(kernel);
    if(program != NULL) ret = clReleaseProgram(program);
    if(ssource != NULL) SDL_FreeSurface(ssource);
   surface = NULL;
}


void SDLDrv_result(cl_mem smem, cl_event *waitevent, int turn, int w, int h)
{
   int x;
   int y;
   int addr = 0;
   int addr2;
   cl_int ret;
   Uint32 *q;
   cl_event event_disp;
   
   printf("\nTurn %d: Tick %d\n", turn, SDL_GetTicks());
   if(surface == NULL) return;
//   if(ssource == NULL) return;
    SDL_LockSurface(surface);
    ret = clEnqueueReadBuffer(command_queue, smem, CL_TRUE, 0,
                              h * surface->pitch, (void *)(surface->pixels)
                              , 0, NULL, &event_disp);
    SDL_UnlockSurface(surface);
    if(ret != CL_SUCCESS) {
       printf("Error on Drawing buffer\n");
       destroy_rss(0);
    }
//   SDL_UpdateRect(ssource, 0, 0, w, h);
//   SDL_BlitSurface(ssource, NULL, surface, NULL);
   SDL_UpdateRect(surface, 0, 0, w, h);
   SDL_Flip(surface);
}

   