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
    testEncode(20, 2);
    testEncode(250, 0);
    testEncode(250, 1);
    testEncode(250, 134);
    testEncode(240, 1);
    testEncode(120, 1);
    testEncode(60, 1);
    testEncode(30, 3);
    testEncode(20, 7);
    testEncode(15, 15);
    testEncode(12, 31);
    testEncode(10, 63);
    testEncode(8, 127);
    testEncode(7, 255);
    testEncode(6, 1023);
    testEncode(5, 4095);
    testEncode(4, 32767);
    testEncode(3, 1048575);
    testEncode(2, 1073741823);
    testEncode(1, 1152921504606846975);
}