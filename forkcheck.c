// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * forkcheck.c
 * 	check XSAVE area
 *	outside and inside child process
 *	
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <x86intrin.h>
#include <string.h>
#include <unistd.h>

#define MAX_SZ 4096

int main(int argc, char **argv) {
 char mem_ini[MAX_SZ] __attribute__((aligned(64)));
 char mem_sc[MAX_SZ] __attribute__((aligned(64)));
 char mem_sc_[MAX_SZ] __attribute__((aligned(64)));
 int sz;
 uint32_t  ui32;
 uint64_t  ui64;

 sz = 512;
 if (argc > 1) sz = atoi(argv[1]);

 if (sz > MAX_SZ) sz = MAX_SZ;
 if (sz < 0) sz = 0;

 /* XSAVE area initialization for task and child */
 memset(mem_ini, 0, sz);
 memset(mem_sc, 0, sz);

 ui32 = 0xFAFBABAF;
 ui64 = 0xBAB00500FAB7;
 /* FPU */
 asm volatile ("fld %0": :"m" (ui64));
 asm volatile ("fld %0": :"m" (ui32));
 ui64 += 0x93ABE13;
 asm volatile ("fld %0": :"m" (ui64));
 ui64 += 0x93;
 asm volatile ("fld %0": :"m" (ui64));
 asm volatile ("fld %0": :"m" (ui32));
 asm volatile ("fld %0": :"m" (ui64));
 ui64 -= 0x21;
 asm volatile ("fld %0": :"m" (ui64));
 asm volatile ("fld %0": :"m" (ui32));
 asm volatile ("fld %0": :"m" (ui64));

 /* AVX 2 */
 asm volatile ("vbroadcastss %0, %%ymm0": :"m" (ui32));
 ui32 += 0xBA;
 asm volatile ("vbroadcastss %0, %%ymm1": :"m" (ui32));
 ui32 -= 0xA;
 asm volatile ("vbroadcastss %0, %%ymm2": :"m" (ui32));
 ui32 -= 0xB;
 asm volatile ("vbroadcastss %0, %%ymm3": :"m" (ui32));
 ui32 -= 0x3;
 asm volatile ("vbroadcastss %0, %%ymm4": :"m" (ui32));
 ui32 += 0xA;
 asm volatile ("vbroadcastss %0, %%ymm5": :"m" (ui32));
 ui32 -= 0x7;
 asm volatile ("vbroadcastss %0, %%ymm6": :"m" (ui32));
 ui32 -= 0xABABA;
 asm volatile ("vbroadcastss %0, %%ymm7": :"m" (ui32));
#ifndef __i386__
 ui32 += 0xF7;
 asm volatile ("vbroadcastss %0, %%ymm8": :"m" (ui32));
 ui32 -= 0x7;
 asm volatile ("vbroadcastss %0, %%ymm9": :"m" (ui32));
 ui32 += 0x2;
 asm volatile ("vbroadcastss %0, %%ymm10": :"m" (ui32));
 ui32 += 0xD;
 asm volatile ("vbroadcastss %0, %%ymm11": :"m" (ui32));
 ui32 -= 0x4;
 asm volatile ("vbroadcastss %0, %%ymm12": :"m" (ui32));
 ui32 -= 0xDD;
 asm volatile ("vbroadcastss %0, %%ymm13": :"m" (ui32));
 ui32 -= 0xABD;
 asm volatile ("vbroadcastss %0, %%ymm14": :"m" (ui32));
 ui32 += 0xBEBABF456;
 asm volatile ("vbroadcastss %0, %%ymm15": :"m" (ui32));
 ui32 -= 0x7;
#endif
#ifdef __i386__
 _xsave(mem_ini, ((long long) 0xFFFFFFFFFFFFFFFF));
#else
 _xsave64(mem_ini, ((long long) 0xFFFFFFFFFFFFFFFF));
#endif
 /* child process */
 if (fork()) {
#ifdef __i386__
 	 _xsave(mem_sc, ((long long) 0xFFFFFFFFFFFFFFFF));
#else
	 _xsave64(mem_sc, ((long long) 0xFFFFFFFFFFFFFFFF));
#endif	
	if (memcmp(mem_ini, mem_sc, sz) != 0)
		printf("FAIL : child xsave area is not the same \n");

	asm volatile ("vbroadcastss %0, %%xmm2": :"m" (ui32));
#ifdef __i386__
 	 _xsave(mem_sc, ((long long) 0xFFFFFFFFFFFFFFFF));
#else
	_xsave64(mem_sc, ((long long) 0xFFFFFFFFFFFFFFFF));
#endif
	sleep(1);
#ifdef __i386__
 	 _xsave(mem_sc_, ((long long) 0xFFFFFFFFFFFFFFFF));
#else
	_xsave64(mem_sc_, ((long long) 0xFFFFFFFFFFFFFFFF));
#endif
	if (memcmp(mem_ini, mem_sc, sz) == 0)
		printf("FAIL : child xsave area is the same after change\n");
	else if (memcmp(mem_ini, mem_sc_, sz) == 0)
		printf("FAIL : child xsave area is the same after change and context switch\n");
 	else if (memcmp(mem_sc, mem_sc_, sz) != 0)
		printf("FAIL : child xsave area is not the same after context switch\n");	
        else
		printf("PASS \n");

	return 0;
  }
#ifdef __i386__
 	 _xsave(mem_sc, ((long long) 0xFFFFFFFFFFFFFFFF));
#else
	_xsave64(mem_sc, ((long long) 0xFFFFFFFFFFFFFFFF));
#endif
	sleep(1);
#ifdef __i386__
 	 _xsave(mem_sc_, ((long long) 0xFFFFFFFFFFFFFFFF));
#else
 	_xsave64(mem_sc_, ((long long) 0xFFFFFFFFFFFFFFFF));
#endif
	if (memcmp(mem_ini, mem_sc, sz) != 0)
		printf("FAIL : xsave area is not the same \n");

 	else if (memcmp(mem_ini, mem_sc_, sz) != 0)
		printf("FAIL : xsave area is not the same after context switch\n");
 	else
		printf("PASS \n");
 	// DBG 	for (i=0;i<sz;i++) printf("%i %x\t",i,mem_sc[i]);

 return 0;
}

