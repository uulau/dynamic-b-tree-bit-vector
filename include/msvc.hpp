#pragma once

#define __SSE4_2__

#if defined(_MSC_VER) && !defined(__clang__)
#include "stdint.h"
#include <intrin.h>
// and/or keywords etc.
#include <iso646.h>

uint64_t __builtin_ctzll(uint64_t x) {
	unsigned long val;
	_BitScanForward64(&val, x);
	return val;
}

uint64_t __builtin_popcountll(uint64_t num) {
	return __popcnt64(num);
}

uint64_t __builtin_clzll(uint64_t num) {
	return __lzcnt64(num);
}
#endif