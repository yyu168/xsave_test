/* Copyright (C) <2021> Intel Corporation */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <x86intrin.h>

struct i387_fxsave_struct {
	uint16_t			cwd; /* Control Word			*/
	uint16_t			swd; /* Status Word			*/
	uint16_t			twd; /* Tag Word			*/
	uint16_t			fop; /* Last Instruction Opcode		*/
	union {
		struct {
			uint64_t	rip; /* Instruction Pointer		*/
			uint64_t	rdp; /* Data Pointer			*/
		};
		struct {
			uint32_t	fip; /* FPU IP Offset			*/
			uint32_t	fcs; /* FPU IP Selector			*/
			uint32_t	foo; /* FPU Operand Offset		*/
			uint32_t	fos; /* FPU Operand Selector		*/
		};
	};
	uint32_t			mxcsr;		/* MXCSR Register State */
	uint32_t			mxcsr_mask;	/* MXCSR Mask		*/

	/* 8*16 bytes for each FP-reg = 128 bytes:			*/
	uint32_t			st_space[32];

	/* 16*16 bytes for each XMM-reg = 256 bytes:			*/
	uint32_t			xmm_space[64];

	uint32_t			padding[12];

	union {
		uint32_t		padding1[12];
		uint32_t		sw_reserved[12];
	};

} __attribute__((aligned(16)));

struct ymmh_struct {
	/* 16 * 16 bytes for each YMMH-reg = 256 bytes */
	uint32_t ymmh_space[64];
};

/* We don't support LWP yet: */
struct lwp_struct {
	uint8_t reserved[128];
};

struct bndregs_struct {
	uint64_t bndregs[8];
} __attribute__((packed));

struct bndcsr_struct {
	uint64_t cfg_reg_u;
	uint64_t status_reg;
} __attribute__((packed));

struct xsave_hdr_struct {
	uint64_t xstate_bv;
	uint64_t xcomp_bv;
	uint64_t reserved[6];
} __attribute__((packed));

struct xsave_struct {
	struct i387_fxsave_struct i387;
	struct xsave_hdr_struct xsave_hdr;
	struct ymmh_struct ymmh;
	struct lwp_struct lwp;
	struct bndregs_struct bndregs;
	struct bndcsr_struct bndcsr;
	/* new processor state extensions will go here */
} __attribute__ ((packed, aligned (64)));




static inline void do_xsave(void *ptr) {
	#ifdef __i386__
		_xsave(ptr, ((long long) 0xFFFFFFFFFFFFFFFF));
	#else
		_xsave64(ptr, ((long long) 0xFFFFFFFFFFFFFFFF));
	#endif
}

/*
 * List of XSAVE features Linux knows about:
 */
enum xfeature {
	XFEATURE_FP,
	XFEATURE_SSE,
	/*
	 * Values above here are "legacy states".
	 * Those below are "extended states".
	 */
	XFEATURE_YMM,
	XFEATURE_BNDREGS,
	XFEATURE_BNDCSR,
	XFEATURE_OPMASK,
	XFEATURE_ZMM_Hi256,
	XFEATURE_Hi16_ZMM,
	XFEATURE_PT_UNIMPLEMENTED_SO_FAR,
	XFEATURE_PKRU,
	XFEATURE_RESERVED,
	XFEATURE_SHSTK_USER,
	XFEATURE_SHSTK_KERNEL,

	XFEATURE_MAX,
};

/*
 * Copied from linux/arch/x86/include/asm/fpu/types.h.
 */
#define XFEATURE_MASK_FP		(1 << XFEATURE_FP)
#define XFEATURE_MASK_SSE		(1 << XFEATURE_SSE)
#define XFEATURE_MASK_YMM		(1 << XFEATURE_YMM)
#define XFEATURE_MASK_BNDREGS		(1 << XFEATURE_BNDREGS)
#define XFEATURE_MASK_BNDCSR		(1 << XFEATURE_BNDCSR)
#define XFEATURE_MASK_OPMASK		(1 << XFEATURE_OPMASK)
#define XFEATURE_MASK_ZMM_Hi256		(1 << XFEATURE_ZMM_Hi256)
#define XFEATURE_MASK_Hi16_ZMM		(1 << XFEATURE_Hi16_ZMM)
#define XFEATURE_MASK_PT		(1 << XFEATURE_PT_UNIMPLEMENTED_SO_FAR)
#define XFEATURE_MASK_PKRU		(1 << XFEATURE_PKRU)
#define XFEATURE_MASK_SHSTK_USER	(1 << XFEATURE_SHSTK_USER)
#define XFEATURE_MASK_SHSTK_KERNEL	(1 << XFEATURE_SHSTK_KERNEL)

#define XSAVES_FEATURES ( \
	XFEATURE_MASK_PT | \
	XFEATURE_MASK_SHSTK_USER | \
	XFEATURE_MASK_SHSTK_KERNEL | \
	0x8000000000000000 \
	)


