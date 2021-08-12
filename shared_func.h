/* Copyright (C) <2021> Intel Corporation */
/* SPDX-License-Identifier: GPL-2.0-or-later */

void dump_buffer(unsigned char *buf, int size);
int get_xsave_size(void);
void set_xmm7(char *data);
#ifndef __i386__
void set_xmm15(char *data);
void set_ymm15(char *data);
#endif
void set_area(void);
