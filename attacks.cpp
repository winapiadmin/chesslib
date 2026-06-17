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

/// @file attacks.cpp
/// @brief Magic-bitboard generation, hyperbola-quintessence helpers, and between-square table.

#include "attacks.h"

namespace chess::_chess {
// [INTERNAL]

/// @brief Reverse bits horizontally in a 64-bit integer.
/// @details Used by the hyperbola quintessence algorithm.
static constexpr Bitboard reverse(Bitboard b) {
    b = (b & 0x5555555555555555ULL) << 1 | ((b >> 1) & 0x5555555555555555ULL);
    b = (b & 0x3333333333333333ULL) << 2 | ((b >> 2) & 0x3333333333333333ULL);
    b = (b & 0x0f0f0f0f0f0f0f0fULL) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0fULL);
    b = (b & 0x00ff00ff00ff00ffULL) << 8 | ((b >> 8) & 0x00ff00ff00ff00ffULL);
    b = (b & 0x0000ffff0000ffffULL) << 16 | ((b >> 16) & 0x0000ffff0000ffffULL);
    b = (b & 0x00000000ffffffffULL) << 32 | ((b >> 32) & 0x00000000ffffffffULL);
    return b;
}

/// @brief Hyperbola quintessence attack computation for a single line.
/// @param sliderBB Bitboard with the slider's square set.
/// @param occ Occupancy bitboard.
/// @param mask Line mask (rank, file, or diagonal).
/// @return Attacks along the masked line.
static constexpr Bitboard hyp_quint(Bitboard sliderBB, Bitboard occ, Bitboard mask) {
    Bitboard occ_masked = occ & mask;
    Bitboard left = occ_masked - 2 * sliderBB;
    Bitboard right = reverse(occ_masked) - 2 * reverse(sliderBB);
    return (left ^ reverse(right)) & mask;
}

/// @brief Compute the diagonal mask through a square.
static constexpr Bitboard diag_mask(Square sq) {
    int r = rank_of(sq);
    int f = file_of(sq);
    Bitboard mask = 0ULL;
    int start_r = (r > f) ? r - f : 0;
    int start_f = (f > r) ? f - r : 0;
    while (start_r < 8 && start_f < 8) {
        mask |= 1ULL << (start_r * 8 + start_f);
        ++start_r;
        ++start_f;
    }
    return mask;
}
/// @brief Compute the anti-diagonal mask through a square.
static constexpr Bitboard antidiag_mask(Square sq) {
    int r = rank_of(sq);
    int f = file_of(sq);
    Bitboard mask = 0ULL;
    int sum = r + f;
    int start_r = (sum < 7) ? 0 : sum - 7;
    int start_f = (sum < 7) ? sum : 7;
    while (start_r < 8 && start_f >= 0) {
        mask |= 1ULL << (start_r * 8 + start_f);
        ++start_r;
        --start_f;
    }
    return mask;
}

/// @brief Bishop attacks via hyperbola quintessence.
static constexpr Bitboard _HyperbolaBishopAttacks(Square sq, Bitboard occ) {
    Bitboard slider = 1ULL << sq;
    Bitboard d_mask = diag_mask(sq);
    Bitboard ad_mask = antidiag_mask(sq);
    return hyp_quint(slider, occ, d_mask) | hyp_quint(slider, occ, ad_mask);
}

/// @brief Rank mask for a square.
static constexpr Bitboard rank_mask(Square sq) { return attacks::MASK_RANK[rank_of(sq)]; }

/// @brief File mask for a square.
static constexpr Bitboard file_mask(Square sq) { return attacks::MASK_FILE[file_of(sq)]; }

/**
 * @brief Computes all squares a rook can attack from a given position.
 *
 * @param sq The rook's square.
 * @param occ Board occupancy.
 * @return Bitboard of attacked squares.
 */
static constexpr Bitboard _HyperbolaRookAttacks(Square sq, Bitboard occ) {
    Bitboard slider = 1ULL << sq;
    Bitboard r_mask = rank_mask(sq);
    Bitboard f_mask = file_mask(sq);
    return hyp_quint(slider, occ, r_mask) | hyp_quint(slider, occ, f_mask);
}
} // namespace chess::_chess
namespace chess::attacks {

// Precompute rays for each square and each of 8 directions.
const std::array<std::array<Bitboard, 64>, 8> RAYS = []() {
    std::array<std::array<Bitboard, 64>, 8> r{};
    for (int dir = 0; dir < 8; ++dir) {
        for (Square sq = SQ_A1; sq < SQ_NONE; ++sq) {
            Bitboard cur = 1ULL << sq;
            Bitboard accum = 0ULL;
            while (true) {
                switch (dir) {
                case RD_NORTH:
                    cur = cur << 8;
                    break;
                case RD_SOUTH:
                    cur = cur >> 8;
                    break;
                case RD_EAST:
                    cur = (cur & ~MASK_FILE[FILE_H]) << 1;
                    break;
                case RD_WEST:
                    cur = (cur & ~MASK_FILE[FILE_A]) >> 1;
                    break;
                case RD_NE:
                    cur = (cur & ~MASK_FILE[FILE_H]) << 9;
                    break;
                case RD_NW:
                    cur = (cur & ~MASK_FILE[FILE_A]) << 7;
                    break;
                case RD_SE:
                    cur = (cur & ~MASK_FILE[FILE_H]) >> 7;
                    break;
                case RD_SW:
                    cur = (cur & ~MASK_FILE[FILE_A]) >> 9;
                    break;
                }
                if (!cur)
                    break;
                accum |= cur;
            }
            r[dir][sq] = accum;
        }
    }
    return r;
}();

#ifdef __BMI2__
/// @brief Software fallback for the PEXT instruction.
/// @details Used during constant evaluation when BMI2 is unavailable.
/// @param val The value to compress.
/// @param mask The bit mask.
/**
 * @brief Extracts bits from a value according to a mask and compacts them.
 *
 * For each set bit position in `mask`, extracts the corresponding bit from `val`
 * and places it into the result at consecutive positions, starting from bit 0.
 *
 * @param val The value to extract bits from.
 * @param mask A mask indicating which bit positions in `val` to extract.
 * @return A compacted bitboard containing only the extracted bits.
 */
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
    /**
     * @brief Extracts relevant occupancy bits for magic bitboard indexing.
     *
     * @param b Occupancy bitboard.
     * @return Compressed index into the magic attack table.
     */
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
    /**
     * @brief Converts an occupancy pattern to an attack table index.
     *
     * @return Index for accessing the precomputed attack bitboard.
     */
    constexpr Bitboard operator()(Bitboard b) const { return (((b & mask)) * magic) >> shift; }
};
#endif

#ifndef GENERATE_AT_RUNTIME
#define _POSSIBLY_CONSTEXPR constexpr
#else
#define _POSSIBLY_CONSTEXPR
#endif
// clang-format off
_POSSIBLY_CONSTEXPR std::array<uint64_t, 64> RookMagics = {
    0x8a80104000800020ULL, 0x0140002000100040ULL, 0x02801880a0017001ULL, 0x0100081001000420ULL,
    0x0200020010080420ULL, 0x03001c0002010008ULL, 0x8480008002000100ULL, 0x2080088004402900ULL,
    0x0000800098204000ULL, 0x2024401000200040ULL, 0x0100802000801000ULL, 0x0120800800801000ULL,
    0x0208808088000400ULL, 0x0002802200800400ULL, 0x2200800100020080ULL, 0x0801000060821100ULL,
    0x0080044006422000ULL, 0x0100808020004000ULL, 0x12108a0010204200ULL, 0x0140848010000802ULL,
    0x0481828014002800ULL, 0x8094004002004100ULL, 0x4010040010010802ULL, 0x0000020008806104ULL,
    0x0100400080208000ULL, 0x2040002120081000ULL, 0x0021200680100081ULL, 0x0020100080080080ULL,
    0x0002000a00200410ULL, 0x0000020080800400ULL, 0x0080088400100102ULL, 0x0080004600042881ULL,
    0x4040008040800020ULL, 0x0440003000200801ULL, 0x0004200011004500ULL, 0x0188020010100100ULL,
    0x0014800401802800ULL, 0x2080040080800200ULL, 0x0124080204001001ULL, 0x0200046502000484ULL,
    0x0480400080088020ULL, 0x1000422010034000ULL, 0x0030200100110040ULL, 0x0000100021010009ULL,
    0x2002080100110004ULL, 0x0202008004008002ULL, 0x0020020004010100ULL, 0x2048440040820001ULL,
    0x0101002200408200ULL, 0x0040802000401080ULL, 0x4008142004410100ULL, 0x02060820c0120200ULL,
    0x0001001004080100ULL, 0x020c020080040080ULL, 0x2935610830022400ULL, 0x0044440041009200ULL,
    0x0280001040802101ULL, 0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL,
    0x00020030a0244872ULL, 0x0012001008414402ULL, 0x02006104900a0804ULL, 0x0001004081002402ULL
};
_POSSIBLY_CONSTEXPR std::array<uint64_t, 64> BishopMagics = {
    0x00262011140108c0ULL, 0x000404040400a100ULL, 0x0908a08409808000ULL, 0x0204404080800004ULL,
    0x018a021105200000ULL, 0x0000900420010001ULL, 0x181400841422c000ULL, 0x000a014900882000ULL,
    0x2818401001120980ULL, 0x4000081104008200ULL, 0x0208081081021004ULL, 0x082108208060a000ULL,
    0x8002031040000000ULL, 0x2000011008043000ULL, 0x4002810588200801ULL, 0x2082248a00a22080ULL,
    0x0011020490022800ULL, 0x088d802005040108ULL, 0x1158010108410600ULL, 0x20040102021200c0ULL,
    0x4492000422010000ULL, 0x0200201100a01021ULL, 0x4005802400c80813ULL, 0x010200044a108c0cULL,
    0x020b208150041000ULL, 0x0004200002181100ULL, 0x00080200140c00d2ULL, 0x010a008020088080ULL,
    0x00808c000080a000ULL, 0x301800c016010080ULL, 0x0084010300880900ULL, 0x00004c8060440440ULL,
    0x000314a000106010ULL, 0x2000900804100640ULL, 0x0011105000480a80ULL, 0x3140120081080080ULL,
    0x4000484040340100ULL, 0x2101010200040a00ULL, 0x0808091054240238ULL, 0x0002048204210440ULL,
    0x4404040218004000ULL, 0x00034a0210042012ULL, 0x0002010048080300ULL, 0xc408c02204200800ULL,
    0x0600050212001400ULL, 0x4002141802000022ULL, 0x00200422c1804200ULL, 0x9008422c00612240ULL,
    0x0100e11018200450ULL, 0x0000248c04200205ULL, 0x040000820806000aULL, 0xa500004205042200ULL,
    0x0400483002020000ULL, 0x1002109001284000ULL, 0x40c8200802004180ULL, 0x1002100241810000ULL,
    0x0080842402024000ULL, 0x0000108880901000ULL, 0x1240480242049044ULL, 0x0440410010608820ULL,
    0x00c0050824050c01ULL, 0x0850400420140100ULL, 0x4e42401016020040ULL, 0x0810a00812488021ULL
};

// clang-format on

/// @brief Generate magic-bitboard lookup tables.
/// @tparam AttackFunc The hyperbola attack function to use.
/// @tparam TableSize Total number of attack entries.
/// @tparam IsBishop true for bishop, false for rook.
/// @return Pair of (magic table, attack table).
template <auto AttackFunc, size_t TableSize, bool IsBishop>
/**
 * @brief Generates magic bitboard tables for fast attack computation.
 *
 * @return A pair containing the Magic entry table (64 entries, one per square) and the corresponding precomputed attack
 * bitboards table.
 */
_POSSIBLY_CONSTEXPR std::pair<std::array<Magic, 64>, std::array<Bitboard, TableSize>> generate_magic_table() {
    std::array<Magic, 64> table{};
    std::array<Bitboard, TableSize> attacks{};

    size_t offset = 0;
    for (Square sq = SQ_A1; sq < SQ_NONE; ++sq) {
        Bitboard occ = 0;
        Bitboard edges = ((attacks::MASK_RANK[0] | attacks::MASK_RANK[7]) & ~attacks::MASK_RANK[rank_of(sq)]) |
                         ((attacks::MASK_FILE[0] | attacks::MASK_FILE[7]) & ~attacks::MASK_FILE[file_of(sq)]);

        Bitboard mask = AttackFunc(static_cast<Square>(sq), 0) & ~edges;
        int bits = popcount(mask);
        Bitboard magic = 0;
        if constexpr (IsBishop)
            magic = BishopMagics[sq];
        else
            magic = RookMagics[sq];

        auto &entry = table[sq];
        entry.mask = mask;
#ifndef __BMI2__
        entry.magic = magic;
        entry.shift = 64 - bits;
#endif
        entry.index = offset;

        // Carry-rippler loop over all blocker subsets
        occ = 0;

        do {
            size_t idx = entry(occ);
            attacks[offset + idx] = AttackFunc(static_cast<Square>(sq), occ);
            occ = (occ - mask) & mask;
        } while (occ);

        offset += (1ULL << bits);
    }
    return std::pair{ table, attacks };
}
_POSSIBLY_CONSTEXPR std::pair<std::array<Magic, 64>, std::array<Bitboard, 0x1480>> bishopData =
    generate_magic_table<_chess::_HyperbolaBishopAttacks, 0x1480, true>();
_POSSIBLY_CONSTEXPR std::array<Magic, 64> BishopTable = bishopData.first;
_POSSIBLY_CONSTEXPR std::array<Bitboard, 0x1480> BishopAttacks = bishopData.second;

_POSSIBLY_CONSTEXPR std::pair<std::array<Magic, 64>, std::array<Bitboard, 0x19000>> rookData =
    generate_magic_table<_chess::_HyperbolaRookAttacks, 0x19000, false>();
_POSSIBLY_CONSTEXPR std::array<Magic, 64> RookTable = rookData.first;
_POSSIBLY_CONSTEXPR std::array<Bitboard, 0x19000> RookAttacks = rookData.second;

/**
 * @brief Returns the attack bitboard for a bishop on the given square.
 *
 * @param sq Bishop square.
 * @param occupied Occupancy bitboard.
 * @return Bitboard of squares attacked.
 */
[[nodiscard]] Bitboard bishop(Square sq, Bitboard occupied) {
    const auto &entry = BishopTable[(int)sq];
    return BishopAttacks[entry.index + entry(occupied)];
}

/**
 * @brief Look up rook attacks from the precomputed magic table.
 * @param sq Rook square.
 * @param occupied Occupancy bitboard.
 * @return Bitboard of squares attacked.
 */
[[nodiscard]] Bitboard rook(Square sq, Bitboard occupied) {
    const auto &entry = RookTable[(int)sq];
    return RookAttacks[entry.index + entry(occupied)];
}
} // namespace chess::attacks
namespace chess::movegen {

/// @brief Hyperbola attack for bishop or rook (used for between-table generation).
inline static Bitboard att(PieceType pt, Square sq, Bitboard occ) {
    return (pt == BISHOP) ? chess::_chess::_HyperbolaBishopAttacks(sq, occ) : chess::_chess::_HyperbolaRookAttacks(sq, occ);
}

/// @brief Generate the between-square table at program startup.
inline static std::array<std::array<Bitboard, 64>, 64> generate_between() {
    std::array<std::array<Bitboard, 64>, 64> squares_between_bb{};

    for (int sq1 = 0; sq1 < 64; ++sq1) {
        for (PieceType pt : { BISHOP, ROOK }) {
            for (int sq2 = 0; sq2 < 64; ++sq2) {
                if (att(pt, Square(sq1), 0) & (1ULL << sq2)) {
                    squares_between_bb[sq1][sq2] = att(pt, Square(sq1), 1ULL << (sq2)) & att(pt, Square(sq2), 1ULL << (sq1));
                }
                squares_between_bb[sq1][sq2] |= 1ULL << (sq2);
            }
        }
    }

    return squares_between_bb;
}
std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN_BB = generate_between();
} // namespace chess::movegen
