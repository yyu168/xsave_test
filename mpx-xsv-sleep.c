// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#ifdef NO_GCC_MPX
typedef unsigned long ULONG;

#define NUM_L1_BITS    28
const ULONG MPX_L1_SIZE = (1UL << NUM_L1_BITS) * sizeof (ULONG);
#define MPX_ENABLE_BIT_NO 0
#define BNDPRESERVE_BIT_NO 1

struct xsave_hdr_struct
{
  uint64_t xstate_bv;
  uint64_t reserved1[2];
  uint64_t reserved2[5];
} __attribute__ ((packed));

struct bndregs_struct
{
  uint64_t bndregs[8];
} __attribute__ ((packed));

struct bndcsr_struct {
	uint64_t cfg_reg_u;
	uint64_t status_reg;
} __attribute__((packed));

struct xsave_struct
{
  uint8_t fpu_sse[512];
  struct xsave_hdr_struct xsave_hdr;
  uint8_t ymm[256];
  uint8_t lwp[128];
  struct bndregs_struct bndregs;
  struct bndcsr_struct bndcsr;
} __attribute__ ((packed));

void *l1base = NULL;

uint8_t __attribute__ ((__aligned__ (64))) buffer[4096];
struct xsave_struct *xsave_buf = (struct xsave_struct *)buffer;

#define REX_PREFIX

static inline void
xrstor_state (struct xsave_struct *fx, uint64_t mask)
{
  uint32_t lmask = mask;
  uint32_t hmask = mask >> 32;

 asm volatile (".byte " REX_PREFIX "0x0f,0xae,0x2f\n\t"
 // asm volatile (".byte " REX_PREFIX "0x0f,0xc7,0x1f\n\t"
		: : "D" (fx), "m" (*fx), "a" (lmask), "d" (hmask)
		:   "memory");
}
static inline void
xsave_state (struct xsave_struct *fx, uint64_t mask)
{
  uint32_t lmask = mask;
  uint32_t hmask = mask >> 32;

 asm volatile (".byte " REX_PREFIX "0x0f,0xae,0x27\n\t"
 // asm volatile (".byte " REX_PREFIX "0x0f,0xc7,0x2f\n\t"
		: : "D" (fx), "m" (*fx), "a" (lmask), "d" (hmask)
		:   "memory");
}

void
enable_mpx (void* l1base)
{
  /* enable point lookup */
  memset (buffer, 0, sizeof (buffer));
/*
  xsave_state (xsave_buf, 0x18);

  xsave_buf->xsave_hdr.xstate_bv = 0x1f;
  xsave_buf->bndcsr.cfg_reg_u = (unsigned long)l1base | 1;
  xsave_buf->bndcsr.status_reg = 0;
*/
  xrstor_state (xsave_buf, 0x18);

  xsave_buf->xsave_hdr.xstate_bv = 0x10;
  xsave_buf->bndcsr.cfg_reg_u = (unsigned long)l1base;
  xsave_buf->bndcsr.cfg_reg_u |= 1 << MPX_ENABLE_BIT_NO;
  xsave_buf->bndcsr.cfg_reg_u |= 1 << BNDPRESERVE_BIT_NO;
  xsave_buf->bndcsr.status_reg = 0;

  xrstor_state (xsave_buf, 0x10);
}
#endif

int main(int argc, char **argv) {
 int i;
 uint8_t bndUL0[2] __attribute__((aligned(8))); 
 uint8_t bndUL1[2] __attribute__((aligned(8)));
 uint8_t bndUL2[2] __attribute__((aligned(8)));
 uint8_t bndUL3[2] __attribute__((aligned(8)));
 uint8_t bndULg0[2] __attribute__((aligned(8)));
 uint8_t bndULg1[2] __attribute__((aligned(8)));
 uint8_t bndULg2[2] __attribute__((aligned(8)));
 uint8_t bndULg3[2] __attribute__((aligned(8)));
 uint8_t bndULg0a[2] __attribute__((aligned(8)));
 uint8_t bndULg1a[2] __attribute__((aligned(8)));
 uint8_t bndULg2a[2] __attribute__((aligned(8)));
 uint8_t bndULg3a[2] __attribute__((aligned(8)));

 bndUL0[0]=2;
 bndUL0[1]=23;
 for (i=0;i<2;i++) printf ("bounds 0 %i %u\n",i,bndUL0[i]);
 bndUL1[0]=3;
 bndUL1[1]=24;
 for (i=0;i<2;i++) printf ("bounds 1 %i %u\n",i,bndUL1[i]);
 bndUL2[0]=1;
 bndUL2[1]=25;
 for (i=0;i<2;i++) printf ("bounds 2 %i %u\n",i,bndUL2[i]);
 bndUL3[0]=12;
 bndUL3[1]=32;
 for (i=0;i<2;i++) printf ("bounds 3 %i %u\n",i,bndUL3[i]);

 bndULg0[0]=43;
 bndULg0[1]=54;
 bndULg1[0]=43;
 bndULg1[1]=54;
 bndULg2[0]=43;
 bndULg2[1]=54;
 bndULg3[0]=43;
 bndULg3[1]=54;

#ifdef NO_GCC_MPX
 l1base = mmap (NULL, MPX_L1_SIZE, PROT_READ | PROT_WRITE,
		 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
 enable_mpx (l1base);

 if (prctl (43,0,0,0,0)) printf("No mpx ...\n");
#endif

 asm volatile ("bndmov %0, %%bnd0": :"m" (bndUL0));
 asm volatile ("bndmov %0, %%bnd1": :"m" (bndUL1));
 asm volatile ("bndmov %0, %%bnd2": :"m" (bndUL2));
 asm volatile ("bndmov %0, %%bnd3": :"m" (bndUL3));

 asm volatile ("bndmov %%bnd0, %0": :"m" (bndULg0));
 asm volatile ("bndmov %%bnd1, %0": :"m" (bndULg1));
 asm volatile ("bndmov %%bnd2, %0": :"m" (bndULg2));
 asm volatile ("bndmov %%bnd3, %0": :"m" (bndULg3));
 asm volatile ("bndmov %%bnd0, %0": :"m" (bndULg0));

 sleep(1);

 asm volatile ("bndmov %%bnd0, %0": :"m" (bndULg0a));
 asm volatile ("bndmov %%bnd1, %0": :"m" (bndULg1a));
 asm volatile ("bndmov %%bnd2, %0": :"m" (bndULg2a));
 asm volatile ("bndmov %%bnd3, %0": :"m" (bndULg3a));

 for (i=0;i<2;i++) printf ("bound 0 before cs %i %u\n",i,bndULg0[i]);
 for (i=0;i<2;i++) printf ("bound 1 before cs %i %u\n",i,bndULg1[i]);
 for (i=0;i<2;i++) printf ("bound 2 before cs %i %u\n",i,bndULg2[i]);
 for (i=0;i<2;i++) printf ("bound 3 before cs %i %u\n",i,bndULg3[i]);

 for (i=0;i<2;i++) printf ("bound 0 after cs %i %u\n",i,bndULg0a[i]);
 for (i=0;i<2;i++) printf ("bound 1 after cs %i %u\n",i,bndULg1a[i]);
 for (i=0;i<2;i++) printf ("bound 2 after cs %i %u\n",i,bndULg2a[i]);
 for (i=0;i<2;i++) printf ("bound 3 after cs %i %u\n",i,bndULg3a[i]);
 
#ifdef NO_GCC_MPX
 if (prctl(44,0,0,0,0)) printf("No mpx ...\n");
 munmap(l1base,MPX_L1_SIZE);
#endif

 return 0;
}

