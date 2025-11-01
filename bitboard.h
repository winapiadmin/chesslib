#pragma once
#include "types.h"

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace chess {
// -------------------------------
// constexpr fallbacks
// -------------------------------
constexpr int popcount_constexpr(Bitboard x) noexcept {
    int count = 0;
    while (x) {
        x &= x - 1;
        ++count;
    }
    return count;
}

constexpr int lsb_constexpr(Bitboard x) noexcept {
    int pos = 0;
    while ((x & 1) == 0) {
        x >>= 1;
        ++pos;
    }
    return pos;
}

constexpr int msb_constexpr(Bitboard x) noexcept {
    int pos = 63;
    Bitboard mask = 1ULL << 63;
    while ((x & mask) == 0) {
        mask >>= 1;
        --pos;
    }
    return pos;
}

// -------------------------------
// runtime + constexpr aware
// -------------------------------
#if defined(__GNUG__) || defined(__clang__)
[[gnu::const]]
#endif
inline constexpr int popcount(Bitboard x) noexcept {
#if defined(__GNUG__) || defined(__clang__)
    if (!is_constant_evaluated())
        return __builtin_popcountll(x);
#elif defined(_MSC_VER)
    if (!is_constant_evaluated())
        return static_cast<int>(_mm_popcnt_u64(x));
#endif
    return popcount_constexpr(x);
}

#if defined(__GNUG__) || defined(__clang__)
[[gnu::const]]
#endif
inline constexpr int lsb(Bitboard x) noexcept {
#if defined(__GNUG__) || defined(__clang__)
    if (!is_constant_evaluated())
        return __builtin_ctzll(x);
#elif defined(_MSC_VER)
    if (!is_constant_evaluated()) {
        unsigned long index = 0;
        _BitScanForward64(&index, x);
        return static_cast<int>(index);
    }
#endif
    return lsb_constexpr(x);
}

#if defined(__GNUG__) || defined(__clang__)
[[gnu::const]]
#endif
inline constexpr int msb(Bitboard x) noexcept {
#if defined(__GNUG__) || defined(__clang__)
    if (!is_constant_evaluated())
        return 63 - __builtin_clzll(x);
#elif defined(_MSC_VER)
    if (!is_constant_evaluated()) {
        unsigned long index = 0;
        _BitScanReverse64(&index, x);
        return static_cast<int>(index);
    }
#endif
    return msb_constexpr(x);
}

// -------------------------------
// destructive variants
// -------------------------------
inline int pop_lsb(Bitboard &b) noexcept {
    int c = lsb(b);
    b &= b - 1;
    return c;
}

inline int pop_msb(Bitboard &b) noexcept {
    int c = msb(b);
    b &= ~(1ULL << c);
    return c;
}
} // namespace chess
