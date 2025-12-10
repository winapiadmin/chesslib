#pragma once
#include "bitboard.h"
#include "types.h"
#include <array>
#include <immintrin.h>
namespace chess::attacks {
// clang-format off
    // pre-calculated lookup table for pawn attacks
    constexpr Bitboard PawnAttacks[2][64] = {
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
    };

    // pre-calculated lookup table for knight attacks
    constexpr Bitboard KnightAttacks[64] = {
        0x0000000000020400, 0x0000000000050800, 0x00000000000A1100, 0x0000000000142200,
        0x0000000000284400, 0x0000000000508800, 0x0000000000A01000, 0x0000000000402000,
        0x0000000002040004, 0x0000000005080008, 0x000000000A110011, 0x0000000014220022,
        0x0000000028440044, 0x0000000050880088, 0x00000000A0100010, 0x0000000040200020,
        0x0000000204000402, 0x0000000508000805, 0x0000000A1100110A, 0x0000001422002214,
        0x0000002844004428, 0x0000005088008850, 0x000000A0100010A0, 0x0000004020002040,
        0x0000020400040200, 0x0000050800080500, 0x00000A1100110A00, 0x0000142200221400,
        0x0000284400442800, 0x0000508800885000, 0x0000A0100010A000, 0x0000402000204000,
        0x0002040004020000, 0x0005080008050000, 0x000A1100110A0000, 0x0014220022140000,
        0x0028440044280000, 0x0050880088500000, 0x00A0100010A00000, 0x0040200020400000,
        0x0204000402000000, 0x0508000805000000, 0x0A1100110A000000, 0x1422002214000000,
        0x2844004428000000, 0x5088008850000000, 0xA0100010A0000000, 0x4020002040000000,
        0x0400040200000000, 0x0800080500000000, 0x1100110A00000000, 0x2200221400000000,
        0x4400442800000000, 0x8800885000000000, 0x100010A000000000, 0x2000204000000000,
        0x0004020000000000, 0x0008050000000000, 0x00110A0000000000, 0x0022140000000000,
        0x0044280000000000, 0x0088500000000000, 0x0010A00000000000, 0x0020400000000000};

    // pre-calculated lookup table for king attacks
    constexpr Bitboard KingAttacks[64] = {
        0x0000000000000302, 0x0000000000000705, 0x0000000000000E0A, 0x0000000000001C14,
        0x0000000000003828, 0x0000000000007050, 0x000000000000E0A0, 0x000000000000C040,
        0x0000000000030203, 0x0000000000070507, 0x00000000000E0A0E, 0x00000000001C141C,
        0x0000000000382838, 0x0000000000705070, 0x0000000000E0A0E0, 0x0000000000C040C0,
        0x0000000003020300, 0x0000000007050700, 0x000000000E0A0E00, 0x000000001C141C00,
        0x0000000038283800, 0x0000000070507000, 0x00000000E0A0E000, 0x00000000C040C000,
        0x0000000302030000, 0x0000000705070000, 0x0000000E0A0E0000, 0x0000001C141C0000,
        0x0000003828380000, 0x0000007050700000, 0x000000E0A0E00000, 0x000000C040C00000,
        0x0000030203000000, 0x0000070507000000, 0x00000E0A0E000000, 0x00001C141C000000,
        0x0000382838000000, 0x0000705070000000, 0x0000E0A0E0000000, 0x0000C040C0000000,
        0x0003020300000000, 0x0007050700000000, 0x000E0A0E00000000, 0x001C141C00000000,
        0x0038283800000000, 0x0070507000000000, 0x00E0A0E000000000, 0x00C040C000000000,
        0x0302030000000000, 0x0705070000000000, 0x0E0A0E0000000000, 0x1C141C0000000000,
        0x3828380000000000, 0x7050700000000000, 0xE0A0E00000000000, 0xC040C00000000000,
        0x0203000000000000, 0x0507000000000000, 0x0A0E000000000000, 0x141C000000000000,
        0x2838000000000000, 0x5070000000000000, 0xA0E0000000000000, 0x40C0000000000000};
    constexpr Bitboard MASK_RANK[8] = {
        0xff,         0xff00,         0xff0000,         0xff000000,
        0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000};

    constexpr Bitboard MASK_FILE[8] = {
        0x101010101010101,  0x202020202020202,  0x404040404040404,  0x808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
    };
    extern const std::array<uint64_t, 64> BishopMagics;
    extern const std::array<uint64_t, 64> RookMagics;
// clang-format on
#ifdef __BMI2__
constexpr uint64_t software_pext_u64(uint64_t val, uint64_t mask) {
    if (!is_constant_evaluated())
        return ~0ULL;
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
struct Magic {
    Bitboard mask;
    int index;
    constexpr Bitboard operator()(Bitboard b) const {
        if (is_constant_evaluated()) {
            return software_pext_u64(b, mask);
        } else {
            return _pext_u64(b, mask);
        }
    }
};
#else
struct Magic {
    Bitboard mask;
    Bitboard magic;
    int index;
    Bitboard shift;
    constexpr Bitboard operator()(Bitboard b) const { return (((b & mask)) * magic) >> shift; }
};
#endif

} // namespace chess::attacks
namespace chess::attacks {
extern const std::array<Magic, 64> RookTable;
extern const std::array<Bitboard, 0x19000> RookAttacks;
extern const std::array<Magic, 64> BishopTable;
extern const std::array<Bitboard, 0x1480> BishopAttacks;
/**
 * @brief  Shifts a bitboard in a given direction
 * @tparam direction
 * @param b
 * @return
 */
template <Direction direction> [[nodiscard]] static constexpr Bitboard shift(const Bitboard b) {
    ASSUME(direction == NORTH || direction == EAST || direction == SOUTH || direction == WEST || direction == NORTH_EAST ||
           direction == SOUTH_EAST || direction == SOUTH_WEST || direction == NORTH_WEST);
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
    default:
        UNREACHABLE();
    }
}

/**
 * @brief
 * @tparam c
 * @param pawns
 * @return
 */
template <Color c> [[nodiscard]] constexpr Bitboard pawnLeftAttacks(const Bitboard pawns) {
    ASSUME(c == WHITE || c == BLACK);
    return c == WHITE ? (pawns << 7) & ~MASK_FILE[7] : (pawns >> 7) & ~MASK_FILE[0];
}

/**
 * @brief Generate the right side pawn attacks.
 * @tparam c
 * @param pawns
 * @return
 */
template <Color c> [[nodiscard]] constexpr Bitboard pawnRightAttacks(const Bitboard pawns) {
    ASSUME(c == WHITE || c == BLACK);
    return c == WHITE ? (pawns << 9) & ~MASK_FILE[0] : (pawns >> 9) & ~MASK_FILE[7];
}

/**
 * @brief Generate the right side pawn attacks.
 * @tparam c
 * @param pawns
 * @return
 */
template <Color c> [[nodiscard]] constexpr Bitboard pawn(const Bitboard pawns) {
    ASSUME(c == WHITE || c == BLACK);
    if constexpr (c == WHITE) {
        return ((pawns & ~MASK_FILE[FILE_A]) << 7) | // left captures
               ((pawns & ~MASK_FILE[FILE_H]) << 9);  // right captures
    } else {
        return ((pawns & ~MASK_FILE[FILE_H]) >> 7) | ((pawns & ~MASK_FILE[FILE_A]) >> 9);
    }
}

/**
 * @brief Returns the pawn attacks for a given color and square
 * @param c
 * @param sq
 * @return
 */
[[nodiscard]] constexpr Bitboard pawn(Color c, Square sq) { return PawnAttacks[(int)c][(int)sq]; }

/**
 * @brief Returns the knight attacks for a given square
 * @param sq
 * @return
 */
[[nodiscard]] constexpr Bitboard knight(Square sq) { return KnightAttacks[(int)sq]; }
/**
 * @brief Returns the knight attacks for given knights
 * @param sq
 * @return
 */
[[nodiscard]] constexpr Bitboard knight(Bitboard knights) {
    Bitboard l1 = (knights >> 1) & 0x7f7f7f7f7f7f7f7fULL;   // shift left by 1, mask out file A
    Bitboard l2 = (knights >> 2) & 0x3f3f3f3f3f3f3f3fULL;   // shift left by 2, mask out files A+B
    Bitboard r1 = (knights << 1) & 0xfefefefefefefefeULL;   // shift right by 1, mask out file H
    Bitboard r2 = (knights << 2) & 0xfcfcfcfcfcfcfcfcULL;   // shift right by 2, mask out files G+H
    Bitboard h1 = l1 | r1;                                  // 1-square horizontal shifts
    Bitboard h2 = l2 | r2;                                  // 2-square horizontal shifts
    return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8); // vertical shifts: +2,+1,-2,-1
}

/**
 * @brief Returns the bishop attacks for a given square
 * @param sq
 * @param occupied
 * @return
 */
[[nodiscard]] constexpr Bitboard bishop(Square sq, Bitboard occupied) {
    return BishopAttacks[BishopTable[(int)sq].index + BishopTable[(int)sq](occupied)];
}

/**
 * @brief Returns the rook attacks for a given square
 * @param sq
 * @param occupied
 * @return
 */
[[nodiscard]] constexpr Bitboard rook(Square sq, Bitboard occupied) {
    return RookAttacks[RookTable[(int)sq].index + RookTable[(int)sq](occupied)];
}

/**
 * @brief Returns the queen attacks for a given square
 * @param sq
 * @param occupied
 * @return
 */
[[nodiscard]] constexpr Bitboard queen(Square sq, Bitboard occupied) { return bishop(sq, occupied) | rook(sq, occupied); }

/**
 * @brief Returns the king attacks for a given square
 * @param sq
 * @return
 */
[[nodiscard]] constexpr Bitboard king(Square sq) { return KingAttacks[(int)sq]; }

/**
 * @brief Returns the slider attacks for a given square
 * @param sq
 * @param occupied
 * @tparam pt
 * @return
 */
template <PieceType pt> [[nodiscard]] constexpr Bitboard slider(Square sq, Bitboard occupied) {
    static_assert(pt == PieceType::BISHOP || pt == PieceType::ROOK || pt == PieceType::QUEEN, "PieceType must be a slider!");

    if constexpr (pt == PieceType::BISHOP)
        return bishop(sq, occupied);
    if constexpr (pt == PieceType::ROOK)
        return rook(sq, occupied);
    if constexpr (pt == PieceType::QUEEN)
        return queen(sq, occupied);
}

} // namespace chess::attacks