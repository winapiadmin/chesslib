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