#include "attacks.h"
namespace chess::attacks {
#ifndef GENERATE_AT_RUNTIME
#define _POSSIBLY_CONSTEXPR constexpr
#else
#define _POSSIBLY_CONSTEXPR const
#endif
template <auto AttackFunc, const Bitboard *Magics, size_t TableSize>
_POSSIBLY_CONSTEXPR std::pair<std::array<Magic, 64>, std::array<Bitboard, TableSize>>
generate_magic_table() {
    std::array<Magic, 64> table{};
    std::array<Bitboard, TableSize> attacks{};

    size_t offset = 0;

    for (Square sq = SQ_A1; sq < SQ_NONE; ++sq) {
        Bitboard occ = 0;
        Bitboard edges =
            ((attacks::MASK_RANK[0] | attacks::MASK_RANK[7]) & ~attacks::MASK_RANK[rank_of(sq)]) |
            ((attacks::MASK_FILE[0] | attacks::MASK_FILE[7]) & ~attacks::MASK_FILE[file_of(sq)]);

        Bitboard mask = AttackFunc(static_cast<Square>(sq), 0) & ~edges;
        int bits = popcount(mask);
        int shift = 64 - bits;
        Bitboard magic = Magics[sq];

        auto &entry = table[sq];
        entry.mask = mask;
#ifndef __BMI2__
        entry.magic = magic;
        entry.shift = shift;
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
    generate_magic_table<_chess::_HyperbolaBishopAttacks, attacks::BishopMagics, 0x1480>();
_POSSIBLY_CONSTEXPR std::array<Magic, 64> BishopTable = bishopData.first;
_POSSIBLY_CONSTEXPR std::array<Bitboard, 0x1480> BishopAttacks = bishopData.second;

_POSSIBLY_CONSTEXPR std::pair<std::array<Magic, 64>, std::array<Bitboard, 0x19000>> rookData =
    generate_magic_table<_chess::_HyperbolaRookAttacks, attacks::RookMagics, 0x19000>();
_POSSIBLY_CONSTEXPR std::array<Magic, 64> RookTable = rookData.first;
_POSSIBLY_CONSTEXPR std::array<Bitboard, 0x19000> RookAttacks = rookData.second;
} // namespace chess::attacks

namespace chess::movegen {
inline _POSSIBLY_CONSTEXPR Bitboard att(PieceType pt, Square sq, Bitboard occ) {
    return (pt == BISHOP) ? _chess::_HyperbolaBishopAttacks(sq, occ)
                          : _chess ::_HyperbolaRookAttacks(sq, occ);
}

inline _POSSIBLY_CONSTEXPR std::array<std::array<Bitboard, 64>, 64> generate_between() {
    std::array<std::array<Bitboard, 64>, 64> squares_between_bb{};

    for (int sq1 = 0; sq1 < 64; ++sq1) {
        for (PieceType pt : { BISHOP, ROOK }) {
            for (int sq2 = 0; sq2 < 64; ++sq2) {
                if (att(pt, Square(sq1), 0) & (1ULL << sq2)) {
                    squares_between_bb[sq1][sq2] =
                        att(pt, Square(sq1), 1ULL << (sq2)) & att(pt, Square(sq2), 1ULL << (sq1));
                }
                squares_between_bb[sq1][sq2] |= 1ULL << (sq2);
            }
        }
    }

    return squares_between_bb;
}
_POSSIBLY_CONSTEXPR std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN_BB =
    generate_between();
} // namespace chess::movegen