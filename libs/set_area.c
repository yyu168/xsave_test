// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

#include <sys/types.h>

void set_xmm7(char *data)
{
	asm volatile ("movdqu %0, %%xmm7" : : "m" (*data));
}

#ifndef __i386__
void set_xmm15(char *data)
{
	asm volatile ("movdqu %0, %%xmm15" : : "m" (*data));
}

void set_ymm15(char *data)
{
	asm volatile("vmovdqu %0, %%ymm15" : : "m" (*data));
}
#endif

void set_area(void)
{
	unsigned long ui32;
	unsigned long long ui64;

	ui32 = 0x123456789UL;
	ui64 = 0x0123456789abcdefULL;

	/* FPU */
	asm volatile ("finit");
	asm volatile ("fld %0": :"m" (ui64));
	asm volatile ("fld %0": :"m" (ui32));
	asm volatile ("fld %0": :"m" (ui64));
	asm volatile ("fld %0": :"m" (ui64));
	asm volatile ("fld %0": :"m" (ui32));
	asm volatile ("fld %0": :"m" (ui64));
	asm volatile ("fld %0": :"m" (ui64));
	asm volatile ("fld %0": :"m" (ui32));
	asm volatile ("fld %0": :"m" (ui64));

	/* AVX 2 */
	asm volatile ("vbroadcastss %0, %%ymm0": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm1": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm2": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm3": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm4": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm5": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm6": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm7": :"m" (ui32));

	#ifndef __i386__
	asm volatile ("vbroadcastss %0, %%ymm8": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm9": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm10": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm11": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm12": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm13": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm14": :"m" (ui32));
	asm volatile ("vbroadcastss %0, %%ymm15": :"m" (ui32));
	#endif
}

