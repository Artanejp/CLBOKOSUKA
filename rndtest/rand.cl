// Generate Random values from XOR-Shift Algorythm.
// from: http://en.wikipedia.org/wiki/Xorshift
		 

__kernel void CalcRand(int seed, int init, int n, __global uint *ret)
{
  int i;
  int x, y, z, w, t;
  x = 123456789 + abs(seed);
  y = 362436089 + abs(init);
  z = 521288629;
  w = 88675123;

  for(i = 0; i < n; i++) {
     t = x ^ (x << 11);
     x = y; y = z; z = w;
     w = w ^ (w >> 19) ^ (t ^ (t >> 8));
     ret[i] = w;
  }
 }