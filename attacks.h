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
#include "bitboard.h"
#include "fwd_decl.h"
#include "types.h"
#include <array>
#include <immintrin.h>
#include <vector>

/// @file attacks.h
/// @brief Precomputed attack tables and magic-bitboard lookup functions.

namespace chess::attacks {

/// @brief Precomputed pawn-attack bitboards.
/// @details Indexed as PawnAttacks[color][square].
constexpr Bitboard PawnAttacks[2][64] = {
    // clang-format off
    // white pawn attacks
    { 0x200, 0x500, 0xa00, 0x1400,
      0x2800, 0x5000, 0xa000, 0x4000,
      0x20000, 0x50000, 0xa0000, 0x140000,
      0x280000, 0x500000, 0xa00000, 0x400000,
      0x2000000, 0x5000000, 0xa000000, 0x14000000,
      0x28000000, 0x50000000, 0xa0000000, 0x40000000,
      0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
      0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
      0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
      0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
      0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
      0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
      0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000,
      0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
      0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0 },

    // black pawn attacks
    { 0x0, 0x0, 0x0, 0x0,
      0x0, 0x0, 0x0, 0x0,
      0x2, 0x5, 0xa, 0x14,
      0x28, 0x50, 0xa0, 0x40,
      0x200, 0x500, 0xa00, 0x1400,
      0x2800, 0x5000, 0xa000, 0x4000,
      0x20000, 0x50000, 0xa0000, 0x140000,
      0x280000, 0x500000, 0xa00000, 0x400000,
      0x2000000, 0x5000000, 0xa000000, 0x14000000,
      0x28000000, 0x50000000, 0xa0000000, 0x40000000,
      0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
      0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
      0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
      0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
      0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
      0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000
    }
    // clang-format on
};

/// @brief Precomputed knight-attack bitboards.
/// @details Indexed by square.
constexpr Bitboard KnightAttacks[64] = {
    0x0000000000020400, 0x0000000000050800, 0x00000000000A1100, 0x0000000000142200, 0x0000000000284400, 0x0000000000508800,
    0x0000000000A01000, 0x0000000000402000, 0x0000000002040004, 0x0000000005080008, 0x000000000A110011, 0x0000000014220022,
    0x0000000028440044, 0x0000000050880088, 0x00000000A0100010, 0x0000000040200020, 0x0000000204000402, 0x0000000508000805,
    0x0000000A1100110A, 0x0000001422002214, 0x0000002844004428, 0x0000005088008850, 0x000000A0100010A0, 0x0000004020002040,
    0x0000020400040200, 0x0000050800080500, 0x00000A1100110A00, 0x0000142200221400, 0x0000284400442800, 0x0000508800885000,
    0x0000A0100010A000, 0x0000402000204000, 0x0002040004020000, 0x0005080008050000, 0x000A1100110A0000, 0x0014220022140000,
    0x0028440044280000, 0x0050880088500000, 0x00A0100010A00000, 0x0040200020400000, 0x0204000402000000, 0x0508000805000000,
    0x0A1100110A000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xA0100010A0000000, 0x4020002040000000,
    0x0400040200000000, 0x0800080500000000, 0x1100110A00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000,
    0x100010A000000000, 0x2000204000000000, 0x0004020000000000, 0x0008050000000000, 0x00110A0000000000, 0x0022140000000000,
    0x0044280000000000, 0x0088500000000000, 0x0010A00000000000, 0x0020400000000000
};

/// @brief Precomputed king-attack bitboards.
/// @details Indexed by square.
constexpr Bitboard KingAttacks[64] = {
    0x0000000000000302, 0x0000000000000705, 0x0000000000000E0A, 0x0000000000001C14, 0x0000000000003828, 0x0000000000007050,
    0x000000000000E0A0, 0x000000000000C040, 0x0000000000030203, 0x0000000000070507, 0x00000000000E0A0E, 0x00000000001C141C,
    0x0000000000382838, 0x0000000000705070, 0x0000000000E0A0E0, 0x0000000000C040C0, 0x0000000003020300, 0x0000000007050700,
    0x000000000E0A0E00, 0x000000001C141C00, 0x0000000038283800, 0x0000000070507000, 0x00000000E0A0E000, 0x00000000C040C000,
    0x0000000302030000, 0x0000000705070000, 0x0000000E0A0E0000, 0x0000001C141C0000, 0x0000003828380000, 0x0000007050700000,
    0x000000E0A0E00000, 0x000000C040C00000, 0x0000030203000000, 0x0000070507000000, 0x00000E0A0E000000, 0x00001C141C000000,
    0x0000382838000000, 0x0000705070000000, 0x0000E0A0E0000000, 0x0000C040C0000000, 0x0003020300000000, 0x0007050700000000,
    0x000E0A0E00000000, 0x001C141C00000000, 0x0038283800000000, 0x0070507000000000, 0x00E0A0E000000000, 0x00C040C000000000,
    0x0302030000000000, 0x0705070000000000, 0x0E0A0E0000000000, 0x1C141C0000000000, 0x3828380000000000, 0x7050700000000000,
    0xE0A0E00000000000, 0xC040C00000000000, 0x0203000000000000, 0x0507000000000000, 0x0A0E000000000000, 0x141C000000000000,
    0x2838000000000000, 0x5070000000000000, 0xA0E0000000000000, 0x40C0000000000000
};

/// @brief Per-rank mask (rank 0-7).
constexpr Bitboard MASK_RANK[8] = { 0xff,         0xff00,         0xff0000,         0xff000000,
                                    0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000 };

/// @brief Per-file mask (file 0-7).
constexpr Bitboard MASK_FILE[8] = {
    0x101010101010101,  0x202020202020202,  0x404040404040404,  0x808080808080808,
    0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
};

#ifdef __BMI2__
/// @brief Software fallback for the PEXT instruction.
/// @details Used during constant evaluation when BMI2 is unavailable.
/// @param val The value to compress.
/// @param mask The bit mask.
/// @return Compressed bits.
constexpr uint64_t software_pext_u64(uint64_t val, uint64_t mask) {
    uint64_t result = 0;
    uint64_t bit_position = 0;

    for (uint64_t bit = 1; bit != 0; bit <<= 1) {
        if (mask & bit) {
            if (val & bit) {
                result |= 1ULL << bit_position;
            }
            ++bit_position;
        }
    }
    return result;
}

/// @brief Magic structure for PEXT-based magic bitboards (BMI2 path).
struct Magic {
    Bitboard mask; ///< Relevant occupancy mask.
    int index;     ///< Starting index into the attack table.
    constexpr Bitboard operator()(Bitboard b) const {
        if (is_constant_evaluated()) {
            return software_pext_u64(b, mask);
        } else {
            return _pext_u64(b, mask);
        }
    }
};
#else
/// @brief Magic structure for classical (multiply-and-shift) magic bitboards.
struct Magic {
    Bitboard mask;  ///< Relevant occupancy mask.
    Bitboard magic; ///< Magic multiplier.
    size_t index;   ///< Starting index into the attack table.
    Bitboard shift; ///< Right-shift amount.
    constexpr Bitboard operator()(Bitboard b) const { return (((b & mask)) * magic) >> shift; }
};
#endif

} // namespace chess::attacks
namespace chess::attacks {

/// @brief Shift a bitboard in the given direction.
/// @param b The bitboard.
/// @param direction Direction to shift.
/// @return Shifted bitboard.
[[nodiscard]] static constexpr Bitboard shift(const Bitboard b, Direction direction) {
    switch (direction) {
    case Direction::NORTH:
        return b << 8;
    case Direction::SOUTH:
        return b >> 8;
    case Direction::NORTH_WEST:
        return (b & ~MASK_FILE[0]) << 7;
    case Direction::WEST:
        return (b & ~MASK_FILE[0]) >> 1;
    case Direction::SOUTH_WEST:
        return (b & ~MASK_FILE[0]) >> 9;
    case Direction::NORTH_EAST:
        return (b & ~MASK_FILE[7]) << 9;
    case Direction::EAST:
        return (b & ~MASK_FILE[7]) << 1;
    case Direction::SOUTH_EAST:
        return (b & ~MASK_FILE[7]) >> 7;
    case DOUBLE_NORTH:
        return b << 16;
    case DOUBLE_SOUTH:
        return b >> 16;
    case DOUBLE_EAST:
        return (b & ~MASK_FILE[7] & ~(MASK_FILE[7] >> 1)) << 2;
    case DOUBLE_WEST:
        return (b & ~MASK_FILE[0] & ~(MASK_FILE[0] << 1)) >> 2;
    case DOUBLE_NORTH_EAST: {
        Bitboard t = (b & ~MASK_FILE[7]) << 9;
        return (t & ~MASK_FILE[7]) << 9;
    }
    case DOUBLE_NORTH_WEST: {
        Bitboard t = (b & ~MASK_FILE[0]) << 7;
        return (t & ~MASK_FILE[0]) << 7;
    }
    case DOUBLE_SOUTH_EAST: {
        Bitboard t = (b & ~MASK_FILE[7]) >> 7;
        return (t & ~MASK_FILE[7]) >> 7;
    }
    case DOUBLE_SOUTH_WEST: {
        Bitboard t = (b & ~MASK_FILE[0]) >> 9;
        return (t & ~MASK_FILE[0]) >> 9;
    }
    case DIR_NONE:
        return b;
    default:
        UNREACHABLE();
        return 0;
    }
}

/// @brief Template wrapper for shift in a compile-time-known direction.
template <Direction direction> [[nodiscard]] static constexpr Bitboard shift(const Bitboard b) { return shift(b, direction); }

/// @brief Generate left-side pawn attacks for the given colour.
/// @tparam c Colour.
/// @param pawns Bitboard of pawns.
/// @return Bitboard of left-capture target squares.
template <Color c> [[nodiscard]] constexpr Bitboard pawnLeftAttacks(const Bitboard pawns) {
    ASSUME(c == WHITE || c == BLACK);
    return c == WHITE ? (pawns << 7) & ~MASK_FILE[7] : (pawns >> 7) & ~MASK_FILE[0];
}

/// @brief Generate right-side pawn attacks for the given colour.
/// @tparam c Colour.
/// @param pawns Bitboard of pawns.
/// @return Bitboard of right-capture target squares.
template <Color c> [[nodiscard]] constexpr Bitboard pawnRightAttacks(const Bitboard pawns) {
    ASSUME(c == WHITE || c == BLACK);
    return c == WHITE ? (pawns << 9) & ~MASK_FILE[0] : (pawns >> 9) & ~MASK_FILE[7];
}

/// @brief Generate all pawn attacks from a bitboard of pawns.
/// @tparam c Colour.
/// @param pawns Bitboard of pawns.
/// @return Bitboard of all squares attacked by the pawns.
template <Color c> [[nodiscard]] constexpr Bitboard pawn(const Bitboard pawns) {
    ASSUME(c == WHITE || c == BLACK);
    if constexpr (c == WHITE) {
        return ((pawns & ~MASK_FILE[FILE_A]) << 7) | ((pawns & ~MASK_FILE[FILE_H]) << 9);
    } else {
        return ((pawns & ~MASK_FILE[FILE_H]) >> 7) | ((pawns & ~MASK_FILE[FILE_A]) >> 9);
    }
}

/// @brief Look up pawn attacks for a single square.
/// @param c Colour.
/// @param sq Square.
/// @return Bitboard of squares attacked.
[[nodiscard]] constexpr Bitboard pawn(Color c, Square sq) { return PawnAttacks[(int)c][(int)sq]; }

/// @brief Look up knight attacks for a single square.
/// @param sq Square.
/// @return Bitboard of squares attacked.
[[nodiscard]] constexpr Bitboard knight(Square sq) { return KnightAttacks[(int)sq]; }

/// @brief Compute knight attacks for a bitboard of knights.
/// @param knights Bitboard of knights.
/// @return Bitboard of all squares attacked.
[[nodiscard]] constexpr Bitboard knight(Bitboard knights) {
    Bitboard l1 = (knights >> 1) & 0x7f7f7f7f7f7f7f7fULL;
    Bitboard l2 = (knights >> 2) & 0x3f3f3f3f3f3f3f3fULL;
    Bitboard r1 = (knights << 1) & 0xfefefefefefefefeULL;
    Bitboard r2 = (knights << 2) & 0xfcfcfcfcfcfcfcfcULL;
    Bitboard h1 = l1 | r1;
    Bitboard h2 = l2 | r2;
    return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}

/// @brief Look up bishop attacks via magic bitboard (defined in attacks.cpp).
/// @param sq Bishop square.
/// @param occupied Occupancy bitboard.
/// @return Bitboard of squares attacked.
[[nodiscard]] Bitboard bishop(Square sq, Bitboard occupied);

/// @brief Look up rook attacks via magic bitboard (defined in attacks.cpp).
/// @param sq Rook square.
/// @param occupied Occupancy bitboard.
/// @return Bitboard of squares attacked.
[[nodiscard]] Bitboard rook(Square sq, Bitboard occupied);

/// @brief Compute queen attacks (bishop | rook).
/// @param sq Queen square.
/// @param occupied Occupancy bitboard.
/// @return Bitboard of squares attacked.
[[nodiscard]] inline Bitboard queen(Square sq, Bitboard occupied) { return bishop(sq, occupied) | rook(sq, occupied); }

/// @brief Look up king attacks for a single square.
/// @param sq Square.
/// @return Bitboard of squares attacked.
[[nodiscard]] constexpr Bitboard king(Square sq) { return KingAttacks[(int)sq]; }

/// @brief Template dispatcher for slider attacks (bishop / rook / queen).
/// @tparam pt Piece type (must be a slider).
/// @param sq Square.
/// @param occupied Occupancy bitboard.
/// @return Bitboard of squares attacked.
template <PieceType pt> [[nodiscard]] inline Bitboard slider(Square sq, Bitboard occupied) {
    static_assert(pt == PieceType::BISHOP || pt == PieceType::ROOK || pt == PieceType::QUEEN, "PieceType must be a slider!");

    if constexpr (pt == PieceType::BISHOP)
        return bishop(sq, occupied);
    if constexpr (pt == PieceType::ROOK)
        return rook(sq, occupied);
    if constexpr (pt == PieceType::QUEEN)
        return queen(sq, occupied);
}

} // namespace chess::attacks
