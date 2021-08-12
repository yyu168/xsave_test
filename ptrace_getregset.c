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

char data0[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
char data1[16] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

int main(int argc, char **argv)
{
	pid_t child;
	int r, status;

	int xsave_size;
	struct iovec iov;

	xsave_size = get_xsave_size();

	memset(buf, 0, BUF_SIZE);

	/*
	 * Set the parent's xmm7 to data[0] pattern.
	 */
	set_xmm7(data0);
	child = fork ();

	if (child == 0) {
		sleep(1);
		set_xmm7(data1);
		exit(0);
	}

	r = ptrace(PTRACE_ATTACH, child, NULL, NULL);
	if (r != 0) {
		printf("Cannot attach to child!\n");
		return -1;
	}

	r = waitpid(child, &status, 0);
	if (r != child) {
		printf("Waitpid failed!\n");
		return -1;
	}

	/*
	 * Attached to the child now.
	 */
	r = ptrace(PTRACE_SETOPTIONS, child, NULL, (void*)PTRACE_O_TRACEEXIT);
	if (r) {
		printf("PTRACE_SETOPTIONS failed");
		return -1;
	}

	ptrace(PTRACE_CONT, child, NULL, NULL);
	r = waitpid(child, &status, 0);
	if (r != child) {
		printf("Waitpid failed!\n");
		return -1;
	}

	iov.iov_base = buf;
	iov.iov_len = xsave_size;

	if (ptrace(PTRACE_GETREGSET, child, NT_X86_XSTATE, &iov) == -1)
		printf ("Failed: GETREGSET: %s\n", strerror(errno));

	ptrace(PTRACE_CONT, child, NULL, NULL);

	if ((argc > 1) && (*argv[1] == 'd'))
		dump_buffer(buf, xsave_size);

	/*
	 * Xmm7 is at offset 272, compare it to data1[].
	 */
	if (memcmp(&buf[272], data1, sizeof(data1)) != 0) {
		printf("FAIL: GETREGSET failed!\n");
		return -1;
	}

	/*
	 * Check system states.
	 */
	if ((*((unsigned long long *)(buf + 0x200)) & XSAVES_FEATURES) ||
	    (*((unsigned long long *)(buf + 0x208)) & XSAVES_FEATURES)) {
		printf("FAIL: GETREGSET has system states!\n");
		return -1;
	}

	printf("PASS: GETREGSET.\n");
	return 0;
}

