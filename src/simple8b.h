// Reference from simple8b golang package: https://pkg.go.dev/github.com/jwilder/encoding/simple8b
// simple8b.h implements the 64bit integer encoding algorithm as published by
// Anh and Moffat in "Index compression using 64-bit words", Softw. Pract. Exper. 2010; 40:131–147
// It is capable of encoding multiple integers with values between 0 and to 1^60 -1, in a single word.

#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// `simple8bEncode()` packs as many values from the `src` into a single uint64 number `encoded` 
// and returns the number of packed values.
const int simple8bEncode(const uint64_t* restrict src, int srcLen, uint64_t* restrict encoded);

// simple8bDecode() decodes a 64-bit number v, writes the result into a large enough list `dst`
// and returns the number of unpacked values. The length of `dst` should be greater or equal to `240`
const int simple8bDecode(uint64_t* restrict dst, uint64_t v);