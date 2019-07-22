#pragma once

#define __SSE4_2__

#if defined(_MSC_VER) && !defined(__clang__)
#include "stdint.h"
#include <intrin.h>
// and/or keywords etc.
#include <iso646.h>

inline uint64_t __builtin_ctzll(const uint64_t x) {
	unsigned long val;
	_BitScanForward64(&val, x);
	return val;
}

inline uint64_t __builtin_popcountll(const uint64_t num) {
	return __popcnt64(num);
}

inline uint64_t __builtin_clzll(const uint64_t num) {
	return __lzcnt64(num);
}
#endif