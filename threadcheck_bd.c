// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * threadcheck_bd.c
 * 	check XSAVE area
 *	XSAVE area while threading
 *	
 */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <x86intrin.h>
#include <string.h>
#include <pthread.h>
#include "xsave.h"

#define MAX_SZ 4096

struct xsave_struct xsv_ini,xsv_sc,xsv_sc_;
static int sz;

/* XSAVE area initialization */
void setArea() {
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
 asm volatile ("vbroadcastss %0, %%xmm0": :"m" (ui32));
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
 ui32 += 0xF7;
 /*
 asm volatile ("vbroadcastss %0, %%ymm8": :"m" (ui32));
 ui32 -= 0x7;
 */
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
}

static int FAIL;

/* thread */
static void * _thread(void * arg) {
	uint32_t  ui32=0xBABAF;
	
	sched_yield();

	if (*(int*)arg) {
		asm volatile ("vbroadcastss %0, %%ymm5": :"m" (ui32));
	}

	sleep(1);
  	
        pthread_exit(0);
  }

int main(int argc, char **argv) {
 int glob;
 pthread_t thread;

 sz = 512;
 if (argc > 1) sz = atoi(argv[1]);

 if (sz > MAX_SZ) sz = MAX_SZ;

 memset(&xsv_ini.xsave_hdr, 0, sizeof(struct xsave_hdr_struct));
 memset(&xsv_sc.xsave_hdr, 0, sizeof(struct xsave_hdr_struct));
 memset(&xsv_sc_.xsave_hdr, 0, sizeof(struct xsave_hdr_struct));
 memset(&xsv_ini, 0, sz);
 memset(&xsv_sc, 0, sz);
 memset(&xsv_sc_, 0, sz);

 /* XSAVE area initialization */
 setArea();

 _xsave64(&xsv_ini, ((long long) 0xFFFFFFFFFFFFFFFF));
 
 glob = 0;

 /* thread creation */
 pthread_create( &thread, NULL, _thread, &glob);

 _xsave64(&xsv_sc, ((long long) 0xFFFFFFFFFFFFFFFF));

 /* wating for thread */
 pthread_join(thread,NULL);

 _xsave64(&xsv_sc_, ((long long) 0xFFFFFFFFFFFFFFFF));

 if (memcmp(&xsv_ini, &xsv_sc, sz)) {
	printf("FAIL : th0 xsave area is not the same \n");
	FAIL = 1;
 }
 else if (memcmp(&xsv_ini, &xsv_sc_, sz)) {
	printf("FAIL : th0 xsave area is not the same after join\n");
	FAIL = 1;
 }

 memset(&xsv_ini.xsave_hdr, 0, sizeof(struct xsave_hdr_struct));
 memset(&xsv_sc.xsave_hdr, 0, sizeof(struct xsave_hdr_struct));
 memset(&xsv_sc_.xsave_hdr, 0, sizeof(struct xsave_hdr_struct));

 _xsave64(&xsv_ini, ((long long) 0xFFFFFFFFFFFFFFFF));

 glob = 1;

 /* thread creation */
 pthread_create( &thread, NULL, _thread, &glob);

 _xsave64(&xsv_sc, ((long long) 0xFFFFFFFFFFFFFFFF));

 /* wating for thread */ 
 pthread_join(thread,NULL);

 _xsave64(&xsv_sc_, ((long long) 0xFFFFFFFFFFFFFFFF));

 if (memcmp(&xsv_ini, &xsv_sc, sz)) {
	printf("FAIL : th1 xsave area is not the same \n");
	FAIL = 1;
 }
 else if (memcmp(&xsv_ini, &xsv_sc_, sz)) {
	printf("FAIL : th1 xsave area is not the same after join\n");
	FAIL = 1;
 }

 printf("%s\n",FAIL?"FAIL":"PASS");

 return 0;
}

