#include <stdint.h>
#include <stdio.h>

#include "./src/simple8b.h"

int main() {
    uint64_t src[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint64_t encoded;
    int n = simple8bEncode(src, sizeof(src) / sizeof(src[0]), &encoded);

    uint64_t decoded[240];
    int m = simple8bDecode(decoded, encoded);
    for (int i = 0; i < m; i++) {
        printf("[%d] = %lu\n", i, decoded[i]);
    }

    return 0;
}