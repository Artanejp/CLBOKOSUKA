// This is OpenCL test.
// Lifegame using OpenCL.
// I tested on Linux box with Phenom2 955 and AMD Radeon HD7700 and proprietary driver.
// (C) 2012 K.Ohta <whatisthis.sowhat@gmail.com>
// History: Oct 19,2012 Initial.
// License: Apache License 2.0

#define ALIVE 1
#define DEAD 0

// Dump board to surface.
void board2surface(__global uchar *src, __global uint *dst, int w, int h, int pitch)
{
  int x, y;
  int ww, wmod;
  __global uchar8 *p;
  __global uchar  *q;
  __global uint8 *dp;
  __global uint *dq;
  uchar8 pattern;
  uint8 data;

  ww = w / 8;
  wmod = w % 8;
  
  p = (__global uchar8 *)src;
  for(y = 0; y < h; y++) {
      dq = (__global uint *)&dst[pitch * y / sizeof(uint)];
      dp = (__global uint8 *)dq;
      dq = &dq[wmod];
      p = (__global uchar8 *)&src[y * w];
      q = (__global uchar *)&src[y * w];
      q = &q[wmod];
      
      for(x = 0; x < w; x += 8) {
          pattern = *p++;
	  pattern = pattern & (uchar8 )(ALIVE, ALIVE, ALIVE, ALIVE, ALIVE, ALIVE, ALIVE, ALIVE);
	  data.s0 = (uint)pattern.s0;
	  data.s1 = (uint)pattern.s1;
	  data.s2 = (uint)pattern.s2;
	  data.s3 = (uint)pattern.s3;
  	  data.s4 = (uint)pattern.s4;
	  data.s5 = (uint)pattern.s5;
	  data.s6 = (uint)pattern.s6;
	  data.s7 = (uint)pattern.s7;

	  data = data * (uint8)(0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff);
	  data = data | (uint8)(0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000);
	  *dp++ = data;
     }
  }

  
}

// Judge dead or alive to one-cell.
uchar DeadOrAlive(__global uchar *src, int addr, int x, int y, int w, int h)
{
   int lives = 0;
//   int addr = x + y * w;
   uchar stat = src[addr];
   
   // Scan (y, x-1) to (y, x + 1) 
   if(x > 0){ // Normal
     if(src[addr - 1] == ALIVE) lives++;
   } 
   if(x < (w - 1)) {
     if(src[addr + 1] == ALIVE) lives++;
   } 
   // Scan (y - 1, x-1) to (y - 1, x + 1) 
   if(y > 0) {
     if(x > 0) {
        if(src[addr - w - 1] == ALIVE) lives++;
     }
     if(x < (w - 1)) {
        if(src[addr - w + 1] == ALIVE) lives++;
     }
     if(src[addr - w] == ALIVE) lives++;
   }
   // Scan (y + 1, x-1) to (y + 1, x + 1) 
   if(y < (h - 1)) {
     if(x > 0) {
        if(src[addr + w - 1] == ALIVE) lives++;
     }
     if(x < (w - 1)) {
        if(src[addr + w + 1] == ALIVE) lives++;
     }
     if(src[addr + w] == ALIVE) lives++;
   }
   // Select DEAD or ALIVE

   if(stat == ALIVE) {
      if(lives < 2) {
           stat = DEAD;  // less
      } else if(lives > 3) {
           stat = DEAD; // Too 
      } else {
           stat = ALIVE; // Continue Alive
      }
   } else {
      if(lives == 3) {
         stat = ALIVE; // Birth
      } else {
         stat = DEAD; // NOP
      }
   }   

   return stat;
}

// Lifegame Core
__kernel void lifegamecore(__global uchar *src, __global uchar *dst, int w, int h, __global uint *pixels, int pitch)
{
  uint addr;
  int x, y;

  addr = 0;

  for(y = 0; y < h; y++) {
     for(x = 0; x < w; x++) {
     dst[addr] = DeadOrAlive(src, addr, x, y, w, h);
     addr++;
     }
  }
  board2surface(src, pixels, w, h, pitch);  
}