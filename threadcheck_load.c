// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * threadcheck_load.c
 * 	check XSAVE area
 *	XSAVE area while threading
 *	
 */
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <string.h>
#include <pthread.h>
#include <immintrin.h>
#include <unistd.h>
#include "xsave.h"

#define MAX_SZ 4096

/* thread */
static void * _thread(void * arg) {

	sched_yield();

	sleep(1);

        pthread_exit(0);
}

int main(int argc, char **argv) {
 int i,FAIL;
 pthread_t thread;
 float *f0, *f1, *f2, *f3;
 FAIL = 0;

 /* XSAVE area initialization with intrinsic */
 /* FPU */
 /* AVX 2 */
__m256 sub[14],suby[14];
uint32_t zeFloat0T[8] __attribute__((aligned(32)));
uint32_t zeFloat1T[8] __attribute__((aligned(32)));
uint32_t zeFloat0Tc[8] __attribute__((aligned(32)));
uint32_t zeFloat1Tc[8] __attribute__((aligned(32)));
f0 = (float *)zeFloat0T;
f1 = (float *)zeFloat1T;
f2 = (float *)zeFloat0Tc;
f3 = (float *)zeFloat1Tc;
zeFloat0T[0] = 0xABA;
zeFloat0T[1] = 0x666;
zeFloat0T[2] = 0xBEA;
zeFloat0T[3] = 0x432;
zeFloat0T[4] = 0x1234;
zeFloat0T[5] = 0xCCD;
zeFloat0T[6] = 0xCEC;
zeFloat0T[7] = 0x888;
zeFloat1T[0] = 0xABA;
zeFloat1T[1] = 0x666;
zeFloat1T[2] = 0xBEA;
zeFloat1T[3] = 0x432;
zeFloat1T[4] = 0x1234;
zeFloat1T[5] = 0xCCD;
zeFloat1T[6] = 0xCEC;
zeFloat1T[7] = 0x7777;
__m256 xmm0 = _mm256_load_ps(f0);
__m256 xmm1 = _mm256_load_ps(f1);
__m256 xmm2 = _mm256_load_ps(f0);
__m256 xmm3 = _mm256_load_ps(f1);
__m256 xmm4 = _mm256_load_ps(f0);
__m256 xmm5 = _mm256_load_ps(f1);
__m256 xmm6 = _mm256_load_ps(f0);
__m256 xmm7 = _mm256_load_ps(f1);
__m256 xmm8 = _mm256_load_ps(f0);
__m256 xmm9 = _mm256_load_ps(f1);
__m256 xmm10 = _mm256_load_ps(f0);
__m256 xmm11 = _mm256_load_ps(f1);
__m256 xmm12 = _mm256_load_ps(f0);
__m256 xmm13 = _mm256_load_ps(f1);
__m256 xmm14 = _mm256_load_ps(f0);
__m256 xmm15 = _mm256_load_ps(f1);
__m256 ymm0 = _mm256_load_ps(f0);
__m256 ymm1 = _mm256_load_ps(f1);
__m256 ymm2 = _mm256_load_ps(f0);
__m256 ymm3 = _mm256_load_ps(f1);
__m256 ymm4 = _mm256_load_ps(f0);
__m256 ymm5 = _mm256_load_ps(f1);
__m256 ymm6 = _mm256_load_ps(f0);
__m256 ymm7 = _mm256_load_ps(f1);
__m256 ymm8 = _mm256_load_ps(f0);
__m256 ymm9 = _mm256_load_ps(f1);
__m256 ymm10 = _mm256_load_ps(f0);
__m256 ymm11 = _mm256_load_ps(f1);
__m256 ymm12 = _mm256_load_ps(f0);
__m256 ymm13 = _mm256_load_ps(f1);
__m256 ymm14 = _mm256_load_ps(f0);
__m256 ymm15 = _mm256_load_ps(f1);

/* thread creation */
pthread_create( &thread, NULL, _thread, NULL);

_mm256_store_ps(f2, xmm0);
_mm256_store_ps(f3, xmm1);

/* waiting for thread */
pthread_join(thread,NULL);

/* XSAVE area values with intrinsic */
sub[0] = _mm256_sub_ps(xmm2,xmm0);
sub[1] = _mm256_sub_ps(xmm3,xmm1);
sub[2] = _mm256_sub_ps(xmm4,xmm0);
sub[3] = _mm256_sub_ps(xmm5,xmm1);
sub[4] = _mm256_sub_ps(xmm6,xmm0);
sub[5] = _mm256_sub_ps(xmm7,xmm1);
sub[6] = _mm256_sub_ps(xmm8,xmm0);
sub[7] = _mm256_sub_ps(xmm9,xmm1);
sub[8] = _mm256_sub_ps(xmm10,xmm0);
sub[9] = _mm256_sub_ps(xmm11,xmm1);
sub[10] = _mm256_sub_ps(xmm12,xmm0);
sub[11] = _mm256_sub_ps(xmm13,xmm1);
sub[12] = _mm256_sub_ps(xmm14,xmm0);
sub[13] = _mm256_sub_ps(xmm15,xmm1);
suby[0] = _mm256_sub_ps(ymm2,ymm0);
suby[1] = _mm256_sub_ps(ymm3,ymm1);
suby[2] = _mm256_sub_ps(ymm4,ymm0);
suby[3] = _mm256_sub_ps(ymm5,ymm1);
suby[4] = _mm256_sub_ps(ymm6,ymm0);
suby[5] = _mm256_sub_ps(ymm7,ymm1);
suby[6] = _mm256_sub_ps(ymm8,ymm0);
suby[7] = _mm256_sub_ps(ymm9,ymm1);
suby[8] = _mm256_sub_ps(ymm10,ymm0);
suby[9] = _mm256_sub_ps(ymm11,ymm1);
suby[10] = _mm256_sub_ps(ymm12,ymm0);
suby[11] = _mm256_sub_ps(ymm13,ymm1);
suby[12] = _mm256_sub_ps(ymm14,ymm0);
suby[13] = _mm256_sub_ps(ymm15,ymm1);

for (i=0;i<14*8;i++)
{
//DBG	printf("sub %i %i : %g\n",i/8,i%8,sub[i/8][i%8]);
	if ((sub[i/8][i%8]!=0) || (suby[i/8][i%8]!=0)) {
		printf("substract %i is not zero\n",i);
		FAIL = 1;
	}
}
for (i=0;i<8;i++)
{
	if ((zeFloat0Tc[i]-zeFloat0T[i]!=0) || (zeFloat1Tc[i]-zeFloat1T[i]!=0))
	{
		printf("store and load %i is different\n",i);
		FAIL = 1;
	}
}

/* DBG
for (i=0;i<8;i++)
{
	printf("xmm0 %i : %g\n",i,xmm0[i]);
	printf("xmm0 %i : %x\n",i,zeFloat0Tc[i]);
	printf("xmm1 %i : %g\n",i,xmm1[i]);
	printf("xmm1 %i : %x\n",i,zeFloat1Tc[i]);
}

*/

printf("%s\n",FAIL?"FAIL":"PASS");
return FAIL;
}

