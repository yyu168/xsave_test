// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <x86intrin.h>
#include "xsave.h"

int main(int argc,char **argv) { 

 __m256 ymm0,ymm1,ymm2;

 struct xsave_struct xsv;
 int i;

 uint32_t ui32,eax,edx;

 memset(&xsv, 0, sizeof(struct xsave_struct));
 for (i=0;i<8;i++) ymm0[i]=0.0;
 for (i=0;i<8;i++) ymm1[i]=0.0;
 for (i=0;i<8;i++) ymm2[i]=0.0;

/* AVX 2 */
/*
 ymm0 = _mm256_broadcast_ss((float *)&ui32);
 ymm1 = _mm256_broadcast_ss((float *)&ui32);
 ymm2 = _mm256_broadcast_ss((float *)&ui32);
*/

 ui32 = 0xFDA12F;
 asm volatile ("vbroadcastss %0, %%ymm0": :"m" (ui32)); 
 ui32 = 0xF12DAFFF;
 asm volatile ("vbroadcastss %0, %%ymm1": :"m" (ui32));
 ui32 = 0x4F321F;
 asm volatile ("vbroadcastss %0, %%ymm2": :"m" (ui32));
 ui32 = 0xDFADFA;
 asm volatile ("vbroadcastss %0, %%xmm3": :"m" (ui32));

 asm volatile ("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));

 asm volatile ("mov $0xffffffff, %edx"); 
 asm volatile ("mov $0xffffffffffffffff, %rax"); 
 asm volatile ("xsave64 %0": :"m" (xsv));

// _xsave64(&xsv, ((long long) 0xFFFFFFFFFFFFFFFF));

 asm volatile ("vmovaps %%ymm0, %0": :"m" (ymm0));
 asm volatile ("vmovaps %%ymm1, %0": :"m" (ymm1));
 asm volatile ("vmovaps %%ymm2, %0": :"m" (ymm2));
 for (i=0;i<8;i++) printf("ymm0 %g\n",ymm0[i]);
 for (i=0;i<8;i++) printf("ymm1 %g\n",ymm1[i]);
 for (i=0;i<8;i++) printf("ymm2 %g\n",ymm2[i]);
 for (i=0;i<4;i++) printf("ymm0 %x\n",xsv.i387.xmm_space[i]);
 for (i=0;i<4;i++) printf("ymm1 %x\n",xsv.i387.xmm_space[i+4]);
 for (i=0;i<4;i++) printf("ymm2 %x\n",xsv.i387.xmm_space[i+8]);
 for (i=0;i<4;i++) printf("ymm3 %x\n",xsv.i387.xmm_space[i+12]);
 for (i=0;i<4;i++) printf("ymmh0 %x\n",xsv.ymmh.ymmh_space[i]);
 for (i=0;i<4;i++) printf("ymmh1 %x\n",xsv.ymmh.ymmh_space[i+4]);
 for (i=0;i<4;i++) printf("ymmh2 %x\n",xsv.ymmh.ymmh_space[i+8]);
 for (i=0;i<4;i++) printf("ymmh3 %x\n",xsv.ymmh.ymmh_space[i+12]);
 printf("eax %x\n",eax);
 printf("edx %x\n",edx);

 return 0;
}
