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

static SDL_Surface *surface = NULL;
static SDL_Surface *display = NULL;

void *SDLDrv_Init(int w, int h)
{
   SDL_Init(SDL_INIT_EVERYTHING);
   surface = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_ASYNCBLIT);
   return (void *)surface;
}

void SDLDrv_End(void)
{
   if(surface != NULL) SDL_FreeSurface(surface);
   surface = NULL;
}


void SDLDrv_result(unsigned char *p, int turn, int w, int h)
{
   int x;
   int y;
   int addr = 0;
   int addr2;
   Uint32 *q;
   
   printf("\nTurn %d:\n", turn);
   if(surface == NULL) return;
   SDL_LockSurface(surface);
   q = (Uint32 *)surface->pixels;
   for(y = 0; y < h ; y++) {
      addr2 = surface->pitch * y / sizeof(Uint32);
	for(x = 0; x < w; x++) { 
	   if(p[addr] == 0) {
	      q[addr2] = 0xff000000;
	   } else {
	      q[addr2] = 0xffffffff;
	   }
	   addr++;
	   addr2++;
	}
   }
   SDL_UnlockSurface(surface);
   SDL_UpdateRect(surface, 0, 0, w, h);
}
