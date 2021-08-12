# Copyright (C) <2021> Intel Corporation
# SPDX-License-Identifier: GPL-2.0-or-later

#
# Makefile 
#
#
ifndef INSTALL_PATH
	INSTALL_PATH= ./
endif
ifdef ARCH
	str__ARCH=ARCH=$(ARCH)
endif

CC       = gcc -g -Wall
ifeq ($(ARCH),i386)
 cCFLAGS   = -m32
endif
cCFLAGS += -mxsave -O2

# Turn on all warnings
CFLAGS += -Wformat -Wformat-security -Werror=format-security
CFLAGS += -D_FORTIFY_SOURCE=2
CFLAGS += -fstack-protector-strong
CFLAGS += -fPIE
CFLAGS += -Wall
CFLAGS += -pthread

BIN	= schedcheck forkcheck signcheck threadcheck_load avxmdb \
  ptrace_getregset ptrace_sys_states ptrace_exec

BIN	+= mpx-xsv mpx-xsv-fork mpx-xsv-sleep
BIN	+= testcomp threadcheck_bd threadcheck_bd-errata
BIN	+= vz_novz_performance

all: $(BIN) install

avxmdb : avxmdb.c
	$(CC) $(CFLAGS) $(cCFLAGS) $(LDFLAGS) -mavx2 -pthread -o $@ $< 

threadcheck_load : threadcheck_load.c
	$(CC) $(CFLAGS) $(cCFLAGS) $(LDFLAGS) -mavx2 -pthread -o $@ $< 

SHARED_FILES = ./libs/dump_buffer.c ./libs/get_xsave_size.c ./libs/set_area.c

ptrace_getregset: $(SHARED_FILES) ptrace_getregset.c
	$(CC) $(CFLAGS) $(cCFLAGS) $(LDFLAGS) $^ -o $@

ptrace_sys_states: $(SHARED_FILES) ptrace_sys_states.c
	$(CC) $(CFLAGS) $(cCFLAGS) $(LDFLAGS) $^ -o $@

ptrace_exec: $(SHARED_FILES) ptrace_exec.c
	$(CC) $(CFLAGS) $(cCFLAGS) $(LDFLAGS) $^ -o $@

module:
	$(MAKE) $(str__ARCH) W=1 -C $(KERNEL_SRC) M=`pwd`
	@rm -f *.mod.* *.o

clean:
	rm -r -f *.o *.ko .*.cmd .*.d .*.tmp *.mod.c modules.order \
	modules.builtin .tmp_* *.class *.so Module.symvers $(BIN) \
	$(INSTALL_PATH)/test-xsave

install: $(BIN)
	@mkdir -p $(INSTALL_PATH)/test-xsave
	@mv $(BIN) $(INSTALL_PATH)/test-xsave
#	@-mv *.ko $(INSTALL_PATH)/test-xsave
	@cp BAT-xsave.sh $(INSTALL_PATH)/test-xsave

.c : 
	$(CC) $(CFLAGS) $(cCFLAGS) $(LDFLAGS) -o $@ $< 

