# Copyright (C) <2021> Intel Corporation
# SPDX-License-Identifier: GPL-2.0-or-later

#!/bin/sh
##
# BAT-xsave.sh
#       enumeration cpu and os
#       schedule, fork, signal, thread and multi-thread test
#

echo BAT XSAVE
EXE_BAT=`pwd`
LOG_BAT="BAT-xsave.log"

echo os 
echo uname ---------------------------------------- > $LOG_BAT

uname -a >> $LOG_BAT
echo dmesg ---------------------------------------- >> $LOG_BAT

dmesg | grep -i fpu >> $LOG_BAT

echo enumeration
echo procfs ---------------------------------------- >> $LOG_BAT
cat /proc/cpuinfo >> $LOG_BAT

echo schedule check
echo schedule check ---------------------------------------- >> $LOG_BAT
$EXE_BAT/schedcheck 1088 >> $LOG_BAT
sudo $EXE_BAT/schedcheck 1088 >> $LOG_BAT
echo fork check
echo fork check ---------------------------------------- >> $LOG_BAT
$EXE_BAT/forkcheck 1088 >> $LOG_BAT
sudo $EXE_BAT/forkcheck 1088 >> $LOG_BAT
echo signal check
echo signal check ---------------------------------------- >> $LOG_BAT
$EXE_BAT/signcheck >> $LOG_BAT
sudo $EXE_BAT/signcheck >> $LOG_BAT
echo thread check
echo thread check ---------------------------------------- >> $LOG_BAT
$EXE_BAT/threadcheck_load >> $LOG_BAT
sudo $EXE_BAT/threadcheck_load >> $LOG_BAT
echo multi thread check
echo multi thread check ---------------------------------------- >> $LOG_BAT
$EXE_BAT/avxmdb >> $LOG_BAT
sudo $EXE_BAT/avxmdb >> $LOG_BAT

echo ptrace check -------------------------------------- >> $LOG_BAT
$EXE_BAT/ptrace_getregset >> $LOG_BAT
$EXE_BAT/ptrace_sys_states >> $LOG_BAT
$EXE_BAT/ptrace_exec >> $LOG_BAT
