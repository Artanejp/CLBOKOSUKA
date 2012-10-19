// This is OpenCL test.
// Lifegame using OpenCL.
// I tested on Linux box with Phenom2 955 and AMD Radeon HD7700 and proprietary driver.
// (C) 2012 K.Ohta <whatisthis.sowhat@gmail.com>
// History: Oct 19,2012 Initial.
// Licence: BSD

#define ALIVE 1
#define DEAD 0

uchar DeadOrAlive(__global uchar *src, uchar stat, int x, int y, int w, int h)
{
   int lives = 0;
   int addr = x + y * w;

   
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

__kernel void lifegamecore(__global uchar *src, int w, int h)
{
  uint addr;
  int x, y;
  
  // y = 0
  addr = 0;
  for(y = 0; y < h; y++) {
     for(x = 0; x < w; x++) {
     src[addr] = DeadOrAlive(src, src[addr], x, y, w, h);
     addr++;
     }
  }
  
}