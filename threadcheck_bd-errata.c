// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <string.h>
#include <pthread.h>
/* gcc -mxsave -pthread -O2 */

#define VBROAD7 "vbroadcastss %0, %%ymm7"
#define RXMM7 "movaps %%xmm7, %0"
#define VBROAD8 "vbroadcastss %0, %%ymm8"
#define RXMM8 "movaps %%xmm8, %0"

/* empty thread */
static void * _thread(void * arg) {
        pthread_exit(0);
  }

/* main : argument is 7 or 8 */
int main(int argc, char **argv) {
 int i;
 unsigned nr;
 uint32_t  ui32=0xFAFBABAF;
 pthread_t thread;
 uint16_t xmmi[8] __attribute__((aligned(16)));
 uint16_t xmm[8] __attribute__((aligned(16)));
 
 /* register xmm7 or xmm8 */
 if (argc>1)   
	nr=atoi(argv[1]);
 else 
	nr=8;
 if (nr!=7 && nr !=8) {
	printf("Bad argument : Register number is 7 or 8 ...\n");
	return -1;
 }

 /* broadcast the register - read it  - thread creation  - read it */
 switch (nr) {
 	case 7 :
 		asm volatile (VBROAD7: :"m" (ui32));
 		asm volatile (RXMM7: :"m" (xmmi));
 		pthread_create( &thread, NULL, _thread, NULL);
 		pthread_join(thread,NULL);
 		asm volatile (RXMM7: :"m" (xmm));
                break;
	case 8 :
 		asm volatile (VBROAD8: :"m" (ui32));
 		asm volatile (RXMM8: :"m" (xmmi));
 		pthread_create( &thread, NULL, _thread, NULL);
 		pthread_join(thread,NULL);
 		asm volatile (RXMM8: :"m" (xmm));
                break;
 }
 
 /* value before thread creation and after */
 for (i=0;i<8;i++) printf ("xmm%i ini %i %x\n",nr,i,xmmi[i]);
 for (i=0;i<8;i++) printf ("xmm%i %i %x\n",nr,i,xmm[i]);

 return 0;
}

