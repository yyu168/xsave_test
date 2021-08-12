// Copyright (C) <2021> Intel Corporation
// SPDX-License-Identifier: GPL-2.0-or-later

int main() {

    unsigned long long i;
#ifdef VZ    
    asm("vzeroupper");
#endif
    for (i = 0; i < 0xc0000000; i++) {
        asm("movups (%rsp), %xmm2");
        asm("addps %xmm1, %xmm2");
    }
    return 0;
}
