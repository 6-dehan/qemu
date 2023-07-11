/*
 * Carry-less multiply operations.
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2023 Linaro, Ltd.
 */

#include "qemu/osdep.h"
#include "crypto/clmul.h"

uint64_t clmul_8x8_low(uint64_t n, uint64_t m)
{
    uint64_t r = 0;

    for (int i = 0; i < 8; ++i) {
        uint64_t mask = (n & 0x0101010101010101ull) * 0xff;
        r ^= m & mask;
        m = (m << 1) & 0xfefefefefefefefeull;
        n >>= 1;
    }
    return r;
}

static uint64_t clmul_8x4_even_int(uint64_t n, uint64_t m)
{
    uint64_t r = 0;

    for (int i = 0; i < 8; ++i) {
        uint64_t mask = (n & 0x0001000100010001ull) * 0xffff;
        r ^= m & mask;
        n >>= 1;
        m <<= 1;
    }
    return r;
}

uint64_t clmul_8x4_even(uint64_t n, uint64_t m)
{
    n &= 0x00ff00ff00ff00ffull;
    m &= 0x00ff00ff00ff00ffull;
    return clmul_8x4_even_int(n, m);
}

uint64_t clmul_8x4_odd(uint64_t n, uint64_t m)
{
    return clmul_8x4_even(n >> 8, m >> 8);
}

static uint64_t unpack_8_to_16(uint64_t x)
{
    return  (x & 0x000000ff)
         | ((x & 0x0000ff00) << 8)
         | ((x & 0x00ff0000) << 16)
         | ((x & 0xff000000) << 24);
}

uint64_t clmul_8x4_packed(uint32_t n, uint32_t m)
{
    return clmul_8x4_even_int(unpack_8_to_16(n), unpack_8_to_16(m));
}

uint64_t clmul_16x2_even(uint64_t n, uint64_t m)
{
    uint64_t r = 0;

    n &= 0x0000ffff0000ffffull;
    m &= 0x0000ffff0000ffffull;

    for (int i = 0; i < 16; ++i) {
        uint64_t mask = (n & 0x0000000100000001ull) * 0xffffffffull;
        r ^= m & mask;
        n >>= 1;
        m <<= 1;
    }
    return r;
}

uint64_t clmul_16x2_odd(uint64_t n, uint64_t m)
{
    return clmul_16x2_even(n >> 16, m >> 16);
}
