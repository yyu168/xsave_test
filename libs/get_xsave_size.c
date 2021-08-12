// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

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

