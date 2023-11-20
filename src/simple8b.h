// Reference from simple8b golang package: https://pkg.go.dev/github.com/jwilder/encoding/simple8b
// simple8b.h implements the 64bit integer encoding algorithm as published by 
// Ann and Moffat in "Index compression using 64-bit words", Softw. Pract. Exper. 2010; 40:131–147
// It is capable of encoding multiple integers with values between 0 and to 1^60 -1, in a single word.

#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static inline uint64_t pack240(const uint64_t* restrict src);
static inline uint64_t pack120(const uint64_t* restrict src);
static inline uint64_t pack60(const uint64_t* restrict src);
static inline uint64_t pack30(const uint64_t* restrict src);
static inline uint64_t pack20(const uint64_t* restrict src);
static inline uint64_t pack15(const uint64_t* restrict src);
static inline uint64_t pack12(const uint64_t* restrict src);
static inline uint64_t pack10(const uint64_t* restrict src);
static inline uint64_t pack8(const uint64_t* restrict src);
static inline uint64_t pack7(const uint64_t* restrict src);
static inline uint64_t pack6(const uint64_t* restrict src);
static inline uint64_t pack5(const uint64_t* restrict src);
static inline uint64_t pack4(const uint64_t* restrict src);
static inline uint64_t pack3(const uint64_t* restrict src);
static inline uint64_t pack2(const uint64_t* restrict src);
static inline uint64_t pack1(const uint64_t* restrict src);

static inline void unpack240(uint64_t v, uint64_t* restrict dst);
static inline void unpack120(uint64_t v, uint64_t* restrict dst);
static inline void unpack60(uint64_t v, uint64_t* restrict dst);
static inline void unpack30(uint64_t v, uint64_t* restrict dst);
static inline void unpack20(uint64_t v, uint64_t* restrict dst);
static inline void unpack15(uint64_t v, uint64_t* restrict dst);
static inline void unpack12(uint64_t v, uint64_t* restrict dst);
static inline void unpack10(uint64_t v, uint64_t* restrict dst);
static inline void unpack8(uint64_t v, uint64_t* restrict dst);
static inline void unpack7(uint64_t v, uint64_t* restrict dst);
static inline void unpack6(uint64_t v, uint64_t* restrict dst);
static inline void unpack5(uint64_t v, uint64_t* restrict dst);
static inline void unpack4(uint64_t v, uint64_t* restrict dst);
static inline void unpack3(uint64_t v, uint64_t* restrict dst);
static inline void unpack2(uint64_t v, uint64_t* restrict dst);
static inline void unpack1(uint64_t v, uint64_t* restrict dst);

static inline bool canPack(const uint64_t* restrict src, int srcLen, int n, int bits) {
    if (srcLen < n) {
        return false;
    }

    int end = n < srcLen ? n : srcLen;

    // Selector 0, 1 are special and use 0 bits to encode runs of 1's
    if (bits == 0) {
        for (int i = 0; i < srcLen; i++) {
            if (src[i] != 1) {
                return false;
            }
        }
        return true;
    }

    uint64_t max = (1UL << bits) - 1;

    for (int i = 0; i < end; i++) {
        if (src[i] > max) {
            return false;
        }
    }

    return true;
}

struct packing {
    const int n;
    const int bit;
    void (*const unpack)(uint64_t, uint64_t* restrict);
    uint64_t (*const pack)(const uint64_t* restrict);
};

const static struct packing selector[] = {
    {240, 0, unpack240, pack240},
    {120, 0, unpack120, pack120},
    {60, 1, unpack60, pack60},
    {30, 2, unpack30, pack30},
    {20, 3, unpack20, pack20},
    {15, 4, unpack15, pack15},
    {12, 5, unpack12, pack12},
    {10, 6, unpack10, pack10},
    {8, 7, unpack8, pack8},
    {7, 8, unpack7, pack7},
    {6, 10, unpack6, pack6},
    {5, 12, unpack5, pack5},
    {4, 15, unpack4, pack4},
    {3, 20, unpack3, pack3},
    {2, 30, unpack2, pack2},
    {1, 60, unpack1, pack1},
};

// simple8bEncode() packs as many values into a single uint64.
// simple8bEncode() returns the number of packed values.
extern inline const int simple8bEncode(const uint64_t* restrict src, int srcLen, uint64_t* restrict encoded) {
    if (canPack(src, srcLen, 240, 0)) {
        *encoded = 0UL;
        return 240;
    } else if (canPack(src, srcLen, 120, 0)) {
        *encoded = 1UL << 60;
        return 120;
    } else if (canPack(src, srcLen, 60, 1)) {
        *encoded = pack60(src);
        return 60;
    } else if (canPack(src, srcLen, 30, 2)) {
        *encoded = pack30(src);
        return 30;
    } else if (canPack(src, srcLen, 20, 3)) {
        *encoded = pack20(src);
        return 20;
    } else if (canPack(src, srcLen, 15, 4)) {
        *encoded = pack15(src);
        return 15;
    } else if (canPack(src, srcLen, 12, 5)) {
        *encoded = pack12(src);
        return 12;
    } else if (canPack(src, srcLen, 10, 6)) {
        *encoded = pack10(src);
        return 10;
    } else if (canPack(src, srcLen, 8, 7)) {
        *encoded = pack8(src);
        return 8;
    } else if (canPack(src, srcLen, 7, 8)) {
        *encoded = pack7(src);
        return 7;
    } else if (canPack(src, srcLen, 6, 10)) {
        *encoded = pack6(src);
        return 6;
    } else if (canPack(src, srcLen, 5, 12)) {
        *encoded = pack5(src);
        return 5;
    } else if (canPack(src, srcLen, 4, 15)) {
        *encoded = pack4(src);
        return 4;
    } else if (canPack(src, srcLen, 3, 20)) {
        *encoded = pack3(src);
        return 3;
    } else if (canPack(src, srcLen, 2, 30)) {
        *encoded = pack2(src);
        return 2;
    } else if (canPack(src, srcLen, 1, 60)) {
        *encoded = pack1(src);
        return 1;
    }
    if (srcLen > 0) {
        fprintf(stderr, "value out of bounds\n");
        assert(false);
    }
    *encoded = 0UL;
    return 0;
}

// The length of `dst` should be larger or equal than 240.
// simple8bDecode() returns the number of unpacked values.
extern inline const int simple8bDecode(uint64_t* restrict dst, uint64_t v) {
    uint64_t sel = v >> 60;
    selector[sel].unpack(v, dst);
    return selector[sel].n;
}

static inline uint64_t pack240(const uint64_t* restrict src) {
    return 0;
}

static inline uint64_t pack120(const uint64_t* restrict src) {
    return 0;
}

static inline uint64_t pack60(const uint64_t* restrict src) {
    return 2UL << 60 |
           src[0] |
           src[1] << 1 |
           src[2] << 2 |
           src[3] << 3 |
           src[4] << 4 |
           src[5] << 5 |
           src[6] << 6 |
           src[7] << 7 |
           src[8] << 8 |
           src[9] << 9 |
           src[10] << 10 |
           src[11] << 11 |
           src[12] << 12 |
           src[13] << 13 |
           src[14] << 14 |
           src[15] << 15 |
           src[16] << 16 |
           src[17] << 17 |
           src[18] << 18 |
           src[19] << 19 |
           src[20] << 20 |
           src[21] << 21 |
           src[22] << 22 |
           src[23] << 23 |
           src[24] << 24 |
           src[25] << 25 |
           src[26] << 26 |
           src[27] << 27 |
           src[28] << 28 |
           src[29] << 29 |
           src[30] << 30 |
           src[31] << 31 |
           src[32] << 32 |
           src[33] << 33 |
           src[34] << 34 |
           src[35] << 35 |
           src[36] << 36 |
           src[37] << 37 |
           src[38] << 38 |
           src[39] << 39 |
           src[40] << 40 |
           src[41] << 41 |
           src[42] << 42 |
           src[43] << 43 |
           src[44] << 44 |
           src[45] << 45 |
           src[46] << 46 |
           src[47] << 47 |
           src[48] << 48 |
           src[49] << 49 |
           src[50] << 50 |
           src[51] << 51 |
           src[52] << 52 |
           src[53] << 53 |
           src[54] << 54 |
           src[55] << 55 |
           src[56] << 56 |
           src[57] << 57 |
           src[58] << 58 |
           src[59] << 59;
}

static inline uint64_t pack30(const uint64_t* restrict src) {
    return 3UL << 60 |
           src[0] |
           src[1] << 2 |
           src[2] << 4 |
           src[3] << 6 |
           src[4] << 8 |
           src[5] << 10 |
           src[6] << 12 |
           src[7] << 14 |
           src[8] << 16 |
           src[9] << 18 |
           src[10] << 20 |
           src[11] << 22 |
           src[12] << 24 |
           src[13] << 26 |
           src[14] << 28 |
           src[15] << 30 |
           src[16] << 32 |
           src[17] << 34 |
           src[18] << 36 |
           src[19] << 38 |
           src[20] << 40 |
           src[21] << 42 |
           src[22] << 44 |
           src[23] << 46 |
           src[24] << 48 |
           src[25] << 50 |
           src[26] << 52 |
           src[27] << 54 |
           src[28] << 56 |
           src[29] << 58;
}

static inline uint64_t pack20(const uint64_t* restrict src) {
    return 4UL << 60 |
           src[0] |
           src[1] << 3 |
           src[2] << 6 |
           src[3] << 9 |
           src[4] << 12 |
           src[5] << 15 |
           src[6] << 18 |
           src[7] << 21 |
           src[8] << 24 |
           src[9] << 27 |
           src[10] << 30 |
           src[11] << 33 |
           src[12] << 36 |
           src[13] << 39 |
           src[14] << 42 |
           src[15] << 45 |
           src[16] << 48 |
           src[17] << 51 |
           src[18] << 54 |
           src[19] << 57;
}

static inline uint64_t pack15(const uint64_t* restrict src) {
    return 5UL << 60 |
           src[0] |
           src[1] << 4 |
           src[2] << 8 |
           src[3] << 12 |
           src[4] << 16 |
           src[5] << 20 |
           src[6] << 24 |
           src[7] << 28 |
           src[8] << 32 |
           src[9] << 36 |
           src[10] << 40 |
           src[11] << 44 |
           src[12] << 48 |
           src[13] << 52 |
           src[14] << 56;
}

static inline uint64_t pack12(const uint64_t* restrict src) {
    return 6UL << 60 |
           src[0] |
           src[1] << 5 |
           src[2] << 10 |
           src[3] << 15 |
           src[4] << 20 |
           src[5] << 25 |
           src[6] << 30 |
           src[7] << 35 |
           src[8] << 40 |
           src[9] << 45 |
           src[10] << 50 |
           src[11] << 55;
}

static inline uint64_t pack10(const uint64_t* restrict src) {
    return 7UL << 60 |
           src[0] |
           src[1] << 6 |
           src[2] << 12 |
           src[3] << 18 |
           src[4] << 24 |
           src[5] << 30 |
           src[6] << 36 |
           src[7] << 42 |
           src[8] << 48 |
           src[9] << 54;
}

static inline uint64_t pack8(const uint64_t* restrict src) {
    return 8UL << 60 |
           src[0] |
           src[1] << 7 |
           src[2] << 14 |
           src[3] << 21 |
           src[4] << 28 |
           src[5] << 35 |
           src[6] << 42 |
           src[7] << 49;
}

static inline uint64_t pack7(const uint64_t* restrict src) {
    return 9UL << 60 |
           src[0] |
           src[1] << 8 |
           src[2] << 16 |
           src[3] << 24 |
           src[4] << 32 |
           src[5] << 40 |
           src[6] << 48;
}

static inline uint64_t pack6(const uint64_t* restrict src) {
    return 10UL << 60 |
           src[0] |
           src[1] << 10 |
           src[2] << 20 |
           src[3] << 30 |
           src[4] << 40 |
           src[5] << 50;
}

static inline uint64_t pack5(const uint64_t* restrict src) {
    return 11UL << 60 |
           src[0] |
           src[1] << 12 |
           src[2] << 24 |
           src[3] << 36 |
           src[4] << 48;
}

static inline uint64_t pack4(const uint64_t* restrict src) {
    return 12UL << 60 |
           src[0] |
           src[1] << 15 |
           src[2] << 30 |
           src[3] << 45;
}

static inline uint64_t pack3(const uint64_t* restrict src) {
    return 13UL << 60 |
           src[0] |
           src[1] << 20 |
           src[2] << 40;
}

static inline uint64_t pack2(const uint64_t* restrict src) {
    return 14UL << 60 |
           src[0] |
           src[1] << 30;
}

static inline uint64_t pack1(const uint64_t* restrict src) {
    return 15UL << 60 |
           src[0];
}

static inline void unpack240(uint64_t v, uint64_t* restrict dst) {
    for (int i = 0; i < 240; i++) {
        dst[i] = 1;
    }
}

static inline void unpack120(uint64_t v, uint64_t* restrict dst) {
    for (int i = 0; i < 240; i++) {
        dst[i] = 1;
    }
}

static inline void unpack60(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 1;
    dst[1] = (v >> 1) & 1;
    dst[2] = (v >> 2) & 1;
    dst[3] = (v >> 3) & 1;
    dst[4] = (v >> 4) & 1;
    dst[5] = (v >> 5) & 1;
    dst[6] = (v >> 6) & 1;
    dst[7] = (v >> 7) & 1;
    dst[8] = (v >> 8) & 1;
    dst[9] = (v >> 9) & 1;
    dst[10] = (v >> 10) & 1;
    dst[11] = (v >> 11) & 1;
    dst[12] = (v >> 12) & 1;
    dst[13] = (v >> 13) & 1;
    dst[14] = (v >> 14) & 1;
    dst[15] = (v >> 15) & 1;
    dst[16] = (v >> 16) & 1;
    dst[17] = (v >> 17) & 1;
    dst[18] = (v >> 18) & 1;
    dst[19] = (v >> 19) & 1;
    dst[20] = (v >> 20) & 1;
    dst[21] = (v >> 21) & 1;
    dst[22] = (v >> 22) & 1;
    dst[23] = (v >> 23) & 1;
    dst[24] = (v >> 24) & 1;
    dst[25] = (v >> 25) & 1;
    dst[26] = (v >> 26) & 1;
    dst[27] = (v >> 27) & 1;
    dst[28] = (v >> 28) & 1;
    dst[29] = (v >> 29) & 1;
    dst[30] = (v >> 30) & 1;
    dst[31] = (v >> 31) & 1;
    dst[32] = (v >> 32) & 1;
    dst[33] = (v >> 33) & 1;
    dst[34] = (v >> 34) & 1;
    dst[35] = (v >> 35) & 1;
    dst[36] = (v >> 36) & 1;
    dst[37] = (v >> 37) & 1;
    dst[38] = (v >> 38) & 1;
    dst[39] = (v >> 39) & 1;
    dst[40] = (v >> 40) & 1;
    dst[41] = (v >> 41) & 1;
    dst[42] = (v >> 42) & 1;
    dst[43] = (v >> 43) & 1;
    dst[44] = (v >> 44) & 1;
    dst[45] = (v >> 45) & 1;
    dst[46] = (v >> 46) & 1;
    dst[47] = (v >> 47) & 1;
    dst[48] = (v >> 48) & 1;
    dst[49] = (v >> 49) & 1;
    dst[50] = (v >> 50) & 1;
    dst[51] = (v >> 51) & 1;
    dst[52] = (v >> 52) & 1;
    dst[53] = (v >> 53) & 1;
    dst[54] = (v >> 54) & 1;
    dst[55] = (v >> 55) & 1;
    dst[56] = (v >> 56) & 1;
    dst[57] = (v >> 57) & 1;
    dst[58] = (v >> 58) & 1;
    dst[59] = (v >> 59) & 1;
}

static inline void unpack30(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 3;
    dst[1] = (v >> 2) & 3;
    dst[2] = (v >> 4) & 3;
    dst[3] = (v >> 6) & 3;
    dst[4] = (v >> 8) & 3;
    dst[5] = (v >> 10) & 3;
    dst[6] = (v >> 12) & 3;
    dst[7] = (v >> 14) & 3;
    dst[8] = (v >> 16) & 3;
    dst[9] = (v >> 18) & 3;
    dst[10] = (v >> 20) & 3;
    dst[11] = (v >> 22) & 3;
    dst[12] = (v >> 24) & 3;
    dst[13] = (v >> 26) & 3;
    dst[14] = (v >> 28) & 3;
    dst[15] = (v >> 30) & 3;
    dst[16] = (v >> 32) & 3;
    dst[17] = (v >> 34) & 3;
    dst[18] = (v >> 36) & 3;
    dst[19] = (v >> 38) & 3;
    dst[20] = (v >> 40) & 3;
    dst[21] = (v >> 42) & 3;
    dst[22] = (v >> 44) & 3;
    dst[23] = (v >> 46) & 3;
    dst[24] = (v >> 48) & 3;
    dst[25] = (v >> 50) & 3;
    dst[26] = (v >> 52) & 3;
    dst[27] = (v >> 54) & 3;
    dst[28] = (v >> 56) & 3;
    dst[29] = (v >> 58) & 3;
}

static inline void unpack20(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 7;
    dst[1] = (v >> 3) & 7;
    dst[2] = (v >> 6) & 7;
    dst[3] = (v >> 9) & 7;
    dst[4] = (v >> 12) & 7;
    dst[5] = (v >> 15) & 7;
    dst[6] = (v >> 18) & 7;
    dst[7] = (v >> 21) & 7;
    dst[8] = (v >> 24) & 7;
    dst[9] = (v >> 27) & 7;
    dst[10] = (v >> 30) & 7;
    dst[11] = (v >> 33) & 7;
    dst[12] = (v >> 36) & 7;
    dst[13] = (v >> 39) & 7;
    dst[14] = (v >> 42) & 7;
    dst[15] = (v >> 45) & 7;
    dst[16] = (v >> 48) & 7;
    dst[17] = (v >> 51) & 7;
    dst[18] = (v >> 54) & 7;
    dst[19] = (v >> 57) & 7;
}

static inline void unpack15(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 15;
    dst[1] = (v >> 4) & 15;
    dst[2] = (v >> 8) & 15;
    dst[3] = (v >> 12) & 15;
    dst[4] = (v >> 16) & 15;
    dst[5] = (v >> 20) & 15;
    dst[6] = (v >> 24) & 15;
    dst[7] = (v >> 28) & 15;
    dst[8] = (v >> 32) & 15;
    dst[9] = (v >> 36) & 15;
    dst[10] = (v >> 40) & 15;
    dst[11] = (v >> 44) & 15;
    dst[12] = (v >> 48) & 15;
    dst[13] = (v >> 52) & 15;
    dst[14] = (v >> 56) & 15;
}

static inline void unpack12(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 31;
    dst[1] = (v >> 5) & 31;
    dst[2] = (v >> 10) & 31;
    dst[3] = (v >> 15) & 31;
    dst[4] = (v >> 20) & 31;
    dst[5] = (v >> 25) & 31;
    dst[6] = (v >> 30) & 31;
    dst[7] = (v >> 35) & 31;
    dst[8] = (v >> 40) & 31;
    dst[9] = (v >> 45) & 31;
    dst[10] = (v >> 50) & 31;
    dst[11] = (v >> 55) & 31;
}

static inline void unpack10(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 63;
    dst[1] = (v >> 6) & 63;
    dst[2] = (v >> 12) & 63;
    dst[3] = (v >> 18) & 63;
    dst[4] = (v >> 24) & 63;
    dst[5] = (v >> 30) & 63;
    dst[6] = (v >> 36) & 63;
    dst[7] = (v >> 42) & 63;
    dst[8] = (v >> 48) & 63;
    dst[9] = (v >> 54) & 63;
}

static inline void unpack8(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 127;
    dst[1] = (v >> 7) & 127;
    dst[2] = (v >> 14) & 127;
    dst[3] = (v >> 21) & 127;
    dst[4] = (v >> 28) & 127;
    dst[5] = (v >> 35) & 127;
    dst[6] = (v >> 42) & 127;
    dst[7] = (v >> 49) & 127;
}

static inline void unpack7(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 255;
    dst[1] = (v >> 8) & 255;
    dst[2] = (v >> 16) & 255;
    dst[3] = (v >> 24) & 255;
    dst[4] = (v >> 32) & 255;
    dst[5] = (v >> 40) & 255;
    dst[6] = (v >> 48) & 255;
}

static inline void unpack6(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 1023;
    dst[1] = (v >> 10) & 1023;
    dst[2] = (v >> 20) & 1023;
    dst[3] = (v >> 30) & 1023;
    dst[4] = (v >> 40) & 1023;
    dst[5] = (v >> 50) & 1023;
}

static inline void unpack5(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 4095;
    dst[1] = (v >> 12) & 4095;
    dst[2] = (v >> 24) & 4095;
    dst[3] = (v >> 36) & 4095;
    dst[4] = (v >> 48) & 4095;
}

static inline void unpack4(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 32767;
    dst[1] = (v >> 15) & 32767;
    dst[2] = (v >> 30) & 32767;
    dst[3] = (v >> 45) & 32767;
}

static inline void unpack3(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 1048575;
    dst[1] = (v >> 20) & 1048575;
    dst[2] = (v >> 40) & 1048575;
}

static inline void unpack2(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 1073741823;
    dst[1] = (v >> 30) & 1073741823;
}

static inline void unpack1(uint64_t v, uint64_t* restrict dst) {
    dst[0] = v & 1152921504606846975;
}