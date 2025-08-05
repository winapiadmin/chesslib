#include "attacks.h"
#include <functional>
namespace chess {
    template <Square sq>
    constexpr inline static void initSliders(attacks::Magic                                   table[],
                                            Bitboard                                         magic,
                                            const std::function<Bitboard(Square, Bitboard)>& attacks)
    {
        // The edges of the board are not considered for the attacks
        // i.e. for the sq h7 edges will be a1-h1, a1-a8, a8-h8, ignoring the edge of the current square
        const Bitboard edges =
          ((attacks::MASK_RANK[0] | attacks::MASK_RANK[7]) & ~attacks::MASK_RANK[rank_of(sq)])
          | ((attacks::MASK_FILE[0] | attacks::MASK_FILE[7]) & ~attacks::MASK_FILE[file_of(sq)]);

        Bitboard occ = 0ULL;

        auto& table_sq = table[static_cast<int>(sq)];

#ifndef USE_PEXT
        table_sq.magic = magic;
#endif
        table_sq.mask = (attacks(sq, occ) & ~edges);
#ifndef USE_PEXT
        table_sq.shift = 64 - popcount(table_sq.mask);
#endif

        if constexpr (static_cast<int>(sq) < 63)
        {
            table[static_cast<int>(sq) + 1].attacks =
              table_sq.attacks + (1ull << popcount(table_sq.mask));
        }

        do
        {
            table_sq.attacks[table_sq(occ)] = attacks(sq, occ);
            occ                             = (occ - table_sq.mask) & table_sq.mask;
        } while (occ);
    }
    attacks::Magic attacks::RookTable[64]   = {};
    attacks::Magic attacks::BishopTable[64] = {};

    static inline Bitboard RookAttacks[0x19000]  = {};
    static inline Bitboard BishopAttacks[0x1480] = {};

    template <Square sq>
    void initBothSliders() {
        initSliders<sq>(attacks::BishopTable, attacks::BishopMagics[static_cast<int>(sq)], _chess::_HyperbolaBishopAttacks);
        initSliders<sq>(attacks::RookTable,   attacks::RookMagics[static_cast<int>(sq)],   _chess::_HyperbolaRookAttacks);
    }

    template <int... sqs>
    void initAllSliders(std::integer_sequence<int, sqs...>) {
        (initBothSliders<static_cast<Square>(sqs)>(), ...); // fold expression calls all
    }

    constexpr int                   attacks::init_attacks()
    {
        BishopTable[0].attacks = BishopAttacks;
        RookTable[0].attacks   = RookAttacks;
        for (size_t i = 0; i < 0x19000; i += 8)
            _mm_prefetch(reinterpret_cast<char*>(RookAttacks + i), _MM_HINT_T0);
        for (size_t i = 0; i < 0x1480; i += 8)
            _mm_prefetch(reinterpret_cast<char*>(BishopAttacks + i), _MM_HINT_T0);
        initAllSliders(std::make_integer_sequence<int, 64>{});
        return 0;
    }
}  // namespace chess
