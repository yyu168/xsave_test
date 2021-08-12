// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <string.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <elf.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <errno.h>
#include "xsave.h"
#include "shared_func.h"

#define BUF_SIZE 4096
unsigned char buf[BUF_SIZE] __attribute__((aligned(64)));

char data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

int main(int argc, char **argv)
{
	pid_t child;
	int r;

	if ((argc > 1) && (*argv[1] == 'c')) {
		do_xsave(buf);
//		dump_buffer(buf, get_xsave_size());

		/*
		 * All states should be cleard, except xstate[1]
		 */
		if ((*((unsigned long long *)(buf + 0x200)) & 0xfffffffffffffffd) ||
		    (*((unsigned long long *)(buf + 0x208)))) {
			printf("FAIL: after exec() states not clean!\n");
			return -1;
		}

		/*
		 * Xmm7 is at offset 272 ~ 295.
		 * In the exec'ed child, it should be cleared.
		 */
		if (*((unsigned long long *)(buf + 272)) ||
		    *((unsigned long long *)(buf + 280))) {
			printf("FAIL: after exec(), xmm7 is not cleared!\n");
			return -1;
		}
		printf("PASS: after exec(), xmm7 is clared!\n");
		return 0;
	}

	set_area();
	set_xmm7(data);
	child = fork();

	if (child == 0) 
	{
		char *args[] = {argv[0], "c", 0};

		r = execve(argv[0], args, 0);
		if (r != 0) {
			printf("execve() failed!\n");
			return -1;
		}
	}
	printf("\n");
	return 0;
}

