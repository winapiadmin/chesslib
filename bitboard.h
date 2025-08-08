#pragma once
#include "types.h"
#include <immintrin.h>
#include <nmmintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>
#include <xmmintrin.h>
#ifdef _MSC_VER
    #include <intrin.h>
    #include <softintrin.h>
#endif
#include <ammintrin.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <wmmintrin.h>
// #include <zmmintrin.h> (immintrin.h)
#include <bit>
// That's.... enough.
namespace chess {
    // Use __builtin_popcountll for GCC/Clang, _mm_popcnt_u64 for MSVC, fallback otherwise
    constexpr int      popcount(Bitboard x) { return std::popcount(static_cast<uint64_t>(x)); }
    constexpr int      lsb(Bitboard x) { return std::countr_zero(static_cast<uint64_t>(x)); }
    constexpr int      msb(Bitboard x) { return 63 - std::countl_zero(static_cast<uint64_t>(x)); }
    constexpr uint64_t reverseBits64(uint64_t b)
    {
        b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
        b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
        b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
        b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

        return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) | (b >> 48);
    }
    inline int pop_lsb(Bitboard& b) {
        auto c= lsb(b);
        b &= b - 1;  // clear the least significant bit
        return c;
    }
    inline int pop_msb(Bitboard& b) {
        auto c = msb(b);
        b &= ~(1<<b);
        return c;
    }
    static_assert(reverseBits64(0x8000000000000000ULL) == 0x1ULL,
                  "reverseBits64(0x8000000000000000ULL) ? 0x1ULL");
    static_assert(reverseBits64(0x0000000000000001ULL) == 0x8000000000000000ULL,
                  "reverseBits64(0x0000000000000001ULL) ? 0x8000000000000000ULLL");
    static_assert(reverseBits64(0xF0F0F0F0F0F0F0F0ULL) == 0x0F0F0F0F0F0F0F0FULL,
                  "reverseBits64(0xF0F0F0F0F0F0F0F0ULL) ? 0x0F0F0F0F0F0F0F0FULLL");
    constexpr Bitboard byteswap(Bitboard x) { return std::byteswap(static_cast<uint64_t>(x)); }
};
