// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * signcheck.c
 * 	check XSAVE area
 *	xsave behaviour related to signal handling	
 *	
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <x86intrin.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "xsave.h"

#ifdef __i386__
#define XSAVE _xsave
#else
#define XSAVE _xsave64
#endif

/*
 * Copied from Linux kernel
 */

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
				unsigned int *ecx, unsigned int *edx)
{
	/* ecx is often an input as well as an output. */
	asm volatile("cpuid"
	    : "=a" (*eax),
	      "=b" (*ebx),
	      "=c" (*ecx),
	      "=d" (*edx)
	    : "0" (*eax), "2" (*ecx)
	    : "memory");
}

int get_xsave_size(void)
{
	unsigned int eax, ebx, ecx, edx;

	eax = 0x0d;
	ebx = 0;
	ecx = 0;
	edx = 0;
	native_cpuid(&eax, &ebx, &ecx, &edx);

	return (int)ecx;
}

void set_area()
{
	uint32_t  ui32;
	uint64_t  ui64;

	ui32 = 0xFAFBABAF;
	ui64 = 0xBAB00500FAB7;

	/* FPU */
	asm volatile ("finit");
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
	/*
	asm volatile ("vbroadcastss %0, %%ymm0": :"m" (ui32));
	ui32 += 0xBA;
	*/
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
}

void signal_handler(int signum)
{
	if (signum == SIGUSR1)
	{
	}
	return;
}

static unsigned char xsave_buffer0[4096] __attribute__((aligned(64)));
static unsigned char xsave_buffer1[4096] __attribute__((aligned(64)));

void dump_buffer(unsigned char *buf, int size)
{
	int i, j;

	printf("-----------------------------------------------------\n");
	printf("xsave size = %d (%03xh)\n", size, size);

	for (i = 0; i < size; i += 16) {
		printf("%04x: ", i);

		for (j = i; ((j < i + 16) && (j < size)); j++) {
			printf("%02x ", buf[j]);
		}

		printf("\n");
	}
}

int main(int argc, char **argv)
{
	struct sigaction sigact;
	int xsave_size;

	sigact.sa_handler = signal_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGUSR1,&sigact,NULL);
	sigaction(SIGUSR2,&sigact,NULL);

	xsave_size = get_xsave_size();

	memset(xsave_buffer0, 0, sizeof(xsave_buffer0));
	memset(xsave_buffer1, 0, sizeof(xsave_buffer1));
	set_area();

	XSAVE(xsave_buffer0, (long long)0xffffffffffffffff);
	raise(SIGUSR1);
	XSAVE(xsave_buffer1, (long long)0xffffffffffffffff);

	if ((argc > 1) && (*argv[1] == 'd')) {
		dump_buffer(xsave_buffer0, xsave_size);
		dump_buffer(xsave_buffer1, xsave_size);
	}

	if (memcmp(xsave_buffer0, xsave_buffer1, xsave_size)) {
		printf("FAIL\n");
		return 1;
	}
	printf("PASS\n");
	return 0;
}
