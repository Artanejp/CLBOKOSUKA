// This is OpenCL test.
// Lifegame using OpenCL.
// I tested on Linux box with Phenom2 955 and AMD Radeon HD7700 and proprietary driver.
// (C) 2012 K.Ohta <whatisthis.sowhat@gmail.com>
// History: Oct 19,2012 Initial.
//          Oct 21,2012 Split to this file.
// License: Apache License 2.0

// Printout board to console.
#include <stdio.h>

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
