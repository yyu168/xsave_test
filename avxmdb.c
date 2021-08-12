// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * avxmdb.c
 * 	check XSAVE area
 *	use XSAVE area for fractal calculus while threading
 *	
 */

#include <immintrin.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

struct _threadArg {
	float x1;
	float y1;
	float x2;
	float y2;
	int wdt;
	int hgt;
	int mi;
	unsigned * img;
};

void MandelbrotCPU(float x1, float y1, float x2, float y2,
	           int width, int height, int maxIters, unsigned * image);

/* thread */
static void * _thread(void * arg) {
   struct _threadArg * zeArg = (struct _threadArg *)arg;
   MandelbrotCPU(zeArg->x1, zeArg->y1, zeArg->x2, zeArg->y2, zeArg->wdt,zeArg->hgt, zeArg->mi, zeArg->img);
   pthread_exit(0);
}

/* fractal calculus */
void MandelbrotCPU(float x1, float y1, float x2, float y2,
	           int width, int height, int maxIters, unsigned * image)
{
int i,j,k;
float dx = (x2-x1)/width;
float dy = (y2-y1)/height;
// round up width to next multiple of 8
int roundedWidth = (width+7) & ~7UL; 
 
float constants[] = {dx, dy, x1, y1, 1.0f, 4.0f};
__m256 ymm0 = _mm256_broadcast_ss(constants);   // all dx
__m256 ymm1 = _mm256_broadcast_ss(constants+1); // all dy
__m256 ymm2 = _mm256_broadcast_ss(constants+2); // all x1
__m256 ymm3 = _mm256_broadcast_ss(constants+3); // all y1
__m256 ymm4 = _mm256_broadcast_ss(constants+4); // all 1's (iter increments)
__m256 ymm5 = _mm256_broadcast_ss(constants+5); // all 4's (comparisons)
 
float incr[8] __attribute__((aligned(32))) = {0.0f,1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f}; // used to reset the i position when j increases
__m256 ymm6 = _mm256_xor_ps(ymm0,ymm0); // zero out j counter (ymm0 is just a dummy)
 
for (j = 0; j < height; j+=1)
{
	__m256 ymm7  = _mm256_load_ps(incr);  // i counter set to 0,1,2,..,7
	for (i = 0; i < roundedWidth; i+=8)
	{
		__m256 ymm8 = _mm256_mul_ps(ymm7, ymm0);  // x0 = (i+k)*dx 
		ymm8 = _mm256_add_ps(ymm8, ymm2);         // x0 = x1+(i+k)*dx
		__m256 ymm9 = _mm256_mul_ps(ymm6, ymm1);  // y0 = j*dy
		ymm9 = _mm256_add_ps(ymm9, ymm3);         // y0 = y1+j*dy
		__m256 ymm10 = _mm256_xor_ps(ymm0,ymm0);  // zero out iteration counter
		__m256 ymm11 = ymm10, ymm12 = ymm10;        // set initial xi=0, yi=0
 
		unsigned int test = 0;
		int iter = 0;
		do
		{
			__m256 ymm13 = _mm256_mul_ps(ymm11,ymm11); // xi*xi
			__m256 ymm14 = _mm256_mul_ps(ymm12,ymm12); // yi*yi
			__m256 ymm15 = _mm256_add_ps(ymm13,ymm14); // xi*xi+yi*yi
 			
			// xi*xi+yi*yi < 4 in each slot
			ymm15 = _mm256_cmp_ps(ymm15,ymm5, _CMP_LT_OQ);        
			// now ymm15 has all 1s in the non overflowed locations
			test = _mm256_movemask_ps(ymm15)&255;      // lower 8 bits are comparisons
			ymm15 = _mm256_and_ps(ymm15,ymm4);
			// get 1.0f or 0.0f in each field as counters
			// counters for each pixel iteration
			ymm10 = _mm256_add_ps(ymm10,ymm15);  
      
			/* add some schedule */
			sched_yield(); 

			ymm15 = _mm256_mul_ps(ymm11,ymm12);        // xi*yi 
			ymm11 = _mm256_sub_ps(ymm13,ymm14);        // xi*xi-yi*yi
			ymm11 = _mm256_add_ps(ymm11,ymm8);         // xi <- xi*xi-yi*yi+x0 done!
			ymm12 = _mm256_add_ps(ymm15,ymm15);        // 2*xi*yi
			ymm12 = _mm256_add_ps(ymm12,ymm9);         // yi <- 2*xi*yi+y0	
 
			++iter;
		} while ((test != 0) && (iter < maxIters));
 
		// convert iterations to output values
		__m256i ymm10i = _mm256_cvtps_epi32(ymm10);
 
		// write only where needed
		int top = (i+7) < width? 8: width&7;
		for (k = 0; k < top; ++k)
		{
			switch (2*k) 
			{
				case 0 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,0);
					break;
				//case 1 :
				//	image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,1);
				//	break;
				case 2 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,2);
					break;
				//case 3 :
				//	image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,3);
				//	break;
				case 4 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,4);
					break;
				case 5 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,5);
					break;
				case 6 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,6);
					break;
				case 7 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,7);
					break;
				case 8 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,8);
					break;
				case 9 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,9);
					break;
				case 10 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,10);
					break;
				case 11 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,11);
					break;
				case 12 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,12);
					break;
				case 13 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,13);
					break;
				case 14 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,14);
					break;
				case 15 :
					image[i+k+j*width] =  _mm256_extract_epi16(ymm10i,15);
					break;
			}
 		}

		// next i position - increment each slot by 8
		ymm7 = _mm256_add_ps(ymm7, ymm5);
		ymm7 = _mm256_add_ps(ymm7, ymm5);
	}
	ymm6 = _mm256_add_ps(ymm6,ymm4); // increment j counter
}
}
#define SiZ 512 
static unsigned img[SiZ*SiZ];
static unsigned img1[SiZ*SiZ];

int main(int argc,char **argv)
{
  float x1,y1,x2,y2;
  int wdt,hgt,mi;
  struct _threadArg zeArg1,zeArg2;
  pthread_t thrd1,thrd2;
 
  unsigned PASS = 0;  

  memset(img,0,sizeof(img));
  memset(img1,0,sizeof(img1));

  x1 = 0.29768; y1 = 0.48364; x2 = 0.29778; y2 = 0.48354;
  wdt = SiZ; hgt = SiZ; mi = 4096;

  zeArg1.x1 = x1;zeArg1.y1 = y1;zeArg1.x2 = x2;zeArg1.y2 = y2;
  zeArg1.wdt = wdt;zeArg1.hgt = hgt;zeArg1.mi = mi;zeArg1.img = img1;

  pthread_create(&thrd1,NULL,_thread,&zeArg1);
  MandelbrotCPU(x1, y1, x2, y2, wdt,hgt, mi, img);
 
  pthread_join(thrd1,NULL);

  if (memcmp(img,img1,SiZ*SiZ)==0) PASS++;

  memset(img,0,sizeof(img));
  memset(img1,0,sizeof(img1));

  zeArg1.x1 = x1;zeArg1.y1 = y1;zeArg1.x2 = x2;zeArg1.y2 = y2;
  zeArg1.wdt = wdt;zeArg1.hgt = hgt;zeArg1.mi = mi;zeArg1.img = img;
  pthread_create(&thrd1,NULL,_thread,&zeArg1);
  zeArg2.x1 = x1;zeArg2.y1 = y1;zeArg2.x2 = x2;zeArg2.y2 = y2;
  zeArg2.wdt = wdt;zeArg2.hgt = hgt;zeArg2.mi = mi;zeArg2.img = img1;
  pthread_create(&thrd2,NULL,_thread,&zeArg2);
 
  pthread_join(thrd1,NULL);
  pthread_join(thrd2,NULL);
 
  if (memcmp(img,img1,SiZ*SiZ)==0) PASS++;

  printf("%s\n",PASS==2?"PASS":"FAIL"); 

  return 0; 
}
