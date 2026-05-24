/*
  a chess library (bonus: you can integrate more piece types!) which
  supports Chess960 and is decently fast enough
  Copyright (C) 2025-2026  winapiadmin

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once
#include "types.h"

#if defined(_MSC_VER)
#include <intrin.h>
#endif
#include <immintrin.h>

/// @file bitboard.h
/// @brief Bitboard utility functions (popcount, LSB, MSB, etc.).

namespace chess {

/// @brief constexpr fallback for population count.
/// @param x Input bitboard.
/// @return Number of set bits.
constexpr int popcount_constexpr(Bitboard x) noexcept {
    int count = 0;
    while (x) {
        x &= x - 1;
        ++count;
    }
    return count;
}

/// @brief constexpr fallback for least-significant bit index.
/// @param x Input bitboard.
/// @return Index of the lowest set bit (0-based).
constexpr int lsb_constexpr(Bitboard x) noexcept {
    int pos = 0;
    while ((x & 1) == 0) {
        x >>= 1;
        ++pos;
    }
    return pos;
}

/// @brief constexpr fallback for most-significant bit index.
/// @param x Input bitboard.
/// @return Index of the highest set bit (0-based).
constexpr int msb_constexpr(Bitboard x) noexcept {
    int pos = 63;
    Bitboard mask = 1ULL << 63;
    while ((x & mask) == 0) {
        mask >>= 1;
        --pos;
    }
    return pos;
}

/// @brief Population count (uses hardware POPCNT when available).
/// @param x Input bitboard.
/// @return Number of set bits.
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

/// @brief Least-significant bit index (uses hardware BSF when available).
/// @param x Input bitboard (must be non-zero).
/// @return Index of the lowest set bit.
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

/// @brief Most-significant bit index (uses hardware BSR when available).
/// @param x Input bitboard (must be non-zero).
/// @return Index of the highest set bit.
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

/// @brief Extract and pop the least-significant bit (destructive).
/// @param b Bitboard reference; modified in place.
/// @return Index of the lowest set bit before removal.
inline int pop_lsb(Bitboard &b) noexcept {
    int c = lsb(b);
#ifndef __BMI2__
    b &= b - 1;
#else
    b = _blsr_u64(b);
#endif
    return c;
}

/// @brief Extract and pop the most-significant bit (destructive).
/// @param b Bitboard reference; modified in place.
/// @return Index of the highest set bit before removal.
inline int pop_msb(Bitboard &b) noexcept {
    int c = msb(b);
    b &= ~(1ULL << c);
    return c;
}
} // namespace chess
