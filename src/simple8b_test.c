#include "./simple8b.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void testEncodeNoValues() {
    uint64_t* raw = NULL;
    uint64_t encoded;
    int packed = simple8bEncode(raw, 0, &encoded);
    assert(packed == 0);
}

void testEncode(int n, uint64_t val) {
    uint64_t* in = malloc(sizeof(uint64_t) * n);
    assert(in);
    for (int i = 0; i < n; i++) {
        in[i] = val;
    }

    uint64_t* encoded = malloc(sizeof(uint64_t) * 8);
    assert(encoded);
    int encodedCap = 8;
    int encodedLen = 0;

    do {
        int m = simple8bEncode(in, n, encoded + encodedLen);
        n = n - m;
        encodedLen++;
        // enlarge
        if (encodedLen == encodedCap) {
            encoded = realloc(encoded, sizeof(uint64_t) * encodedCap << 1);
            encodedCap = encodedCap << 1;
        }
    } while (n > 0);

    uint64_t decoded[240];
    int k = 0;
    for (int i = 0; i < encodedLen; i++) {
        int unpacked = simple8bDecode(decoded, encoded[i]);
        for (int j = 0; j < unpacked; j++) {
            assert(decoded[j] == in[k]);
            k++;
        }
    }
}

int main() {
    testEncodeNoValues();
    printf("Pass testEncodeNoValues()\n");
    testEncode(20, 2);
    printf("Pass testEncode(20, 2)\n");
    testEncode(250, 0);
    printf("Pass testEncode(250, 0)\n");
    testEncode(250, 1);
    printf("Pass testEncode(250, 1)\n");
    testEncode(250, 134);
    printf("Pass testEncode(250, 134)\n");
    testEncode(240, 1);
    printf("Pass testEncode(240, 1)\n");
    testEncode(120, 1);
    printf("Pass testEncode(120, 1)\n");
    testEncode(60, 1);
    printf("Pass testEncode(60, 1)\n");
    testEncode(30, 3);
    printf("Pass testEncode(30, 3)\n");
    testEncode(20, 7);
    printf("Pass testEncode(20, 7)\n");
    testEncode(15, 15);
    printf("Pass testEncode(15, 15)\n");
    testEncode(12, 31);
    printf("Pass testEncode(12, 31)\n");
    testEncode(10, 63);
    printf("Pass testEncode(10, 63)\n");
    testEncode(8, 127);
    printf("Pass testEncode(8, 127)\n");
    testEncode(7, 255);
    printf("Pass testEncode(7, 255)\n");
    testEncode(6, 1023);
    printf("Pass testEncode(6, 1023)\n");
    testEncode(5, 4095);
    printf("Pass testEncode(5, 4095)\n");
    testEncode(4, 32767);
    printf("Pass testEncode(4, 32767)\n");
    testEncode(3, 1048575);
    printf("Pass testEncode(3, 1048575)\n");
    testEncode(2, 1073741823);
    printf("Pass testEncode(2, 1073741823)\n");
    testEncode(1, 1152921504606846975);
    printf("Pass testEncode(1, 1152921504606846975)\n");
    return 0;
}