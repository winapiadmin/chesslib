#include "attacks.h"
namespace chess::attacks {
    constexpr auto bishopData =
        generate_magic_table<_chess::_HyperbolaBishopAttacks, attacks::BishopMagics, 0x1480>();
    constexpr auto BishopTable = bishopData.first;
    constexpr auto BishopAttacks = bishopData.second;

    constexpr auto rookData =
        generate_magic_table<_chess::_HyperbolaRookAttacks, attacks::RookMagics, 0x19000>();
    constexpr auto RookTable = rookData.first;
    constexpr auto RookAttacks = rookData.second;
}