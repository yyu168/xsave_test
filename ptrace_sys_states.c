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

int main(int argc, char **argv)
{
	pid_t child;
	int r, status;

	int xsave_size;
	struct iovec iov;

	xsave_size = get_xsave_size();
	memset(buf, 0, BUF_SIZE);

	child = fork ();

	if (child == 0) {
		sleep(1);
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
	iov.iov_base = buf;
	iov.iov_len = xsave_size;

	/*
	 * Try to set xcomp_bv of some system states.
	 */
	*((unsigned long long *)(buf + 0x208)) |= XSAVES_FEATURES;

	if (ptrace(PTRACE_SETREGSET, child, NT_X86_XSTATE, &iov) == -1) {
		printf("PASS: Set sys states prevented!\n");
	} else {
		printf("FAIL: Can set sys states!\n");
	}

	ptrace(PTRACE_CONT, child, NULL, NULL);
	return 0;
}

