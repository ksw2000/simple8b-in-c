# Simple8b-in-C

simple8b.h implements the 64-bit integer encoding algorithm as published by Anh and Moffat in "Index compression using 64-bit words", Softw. Pract. Exper. 2010; 40:131–147

Simple8b is 64-bit word-sized encoder that packs multiple integers into a single word using a 4 bit selector values and up to 60 bits for the remaining values.  Integers are encoded using the following table:

|  selector   |  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7   |  8   |  9   |  10  |  11  |  12  |  13  |  14  |  15  |
| :---------: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: |
|    bits     |  0   |  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7   |  8   |  10  |  12  |  15  |  20  |  30  |  60  |
|      N      | 240  | 120  |  60  |  30  |  20  |  15  |  12  |  10  |  8   |  7   |  6   |  5   |  4   |  3   |  2   |  1   |
| Wasted Bits |  60  |  60  |  0   |  0   |  0   |  0   |  12  |  0   |  4   |  4   |  0   |  0   |  0   |  0   |  0   |  0   |

For example, when the number of values can be encoded using 4 bits, selected 5 is encoded in the 4 most significant bits followed by 15 values encoded used 4 bits each in the remaining 60 bits.

The code is rewritten from a Go package [github.com/jwilder/encoding](https://pkg.go.dev/github.com/jwilder/encoding)

## Functions

```c
const int simple8bEncode(const uint64_t *__restrict__ src, int srcLen, uint64_t *__restrict__ encoded);
```

`simple8bEncode()` packs as many values from the `src` into a single uint64 number `encoded` and returns the number of packed values.

```c
const int simple8bDecode(uint64_t *__restrict__ dst, uint64_t v);
```

`simple8bDecode()` decodes a 64-bit number `v`, writes the result into a large enough list `dst`, and returns the number of unpacked values. The length of `dst` should be greater or equal to `240`

## Example

```c
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
```

**Result**

```
[0] = 1
[1] = 2
[2] = 3
[3] = 4
[4] = 5
[5] = 6
[6] = 7
[7] = 8
[8] = 9
[9] = 10
```

## Usage

**Compile Directly**

```
.
├── src/
│   ├── simple8b.h
│   └── simple8b.c
└── example.c
```

```sh
gcc example.c src/simple8b.c -o example
./example
```

**Static Library**

```sh
mkdir lib
gcc -c src/simple8b.c -o lib/simple8b.o
ar rcs lib/simple8b.a lib/simple8b.o
gcc example.c -o example -L. lib/simple8b.a
```

```
.
├── src/
│   ├── simple8b.h
│   └── simple8b.c
├── lib/
│   ├── simple8b.a
│   └── simple8b.o
├── example.c
└── example
```
