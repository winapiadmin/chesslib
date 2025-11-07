#include "attacks.h"
namespace chess::attacks {
#ifndef GENERATE_AT_RUNTIME
#define _POSSIBLY_CONSTEXPR constexpr
#else
#define _POSSIBLY_CONSTEXPR const
#endif
const uint64_t RookMagics[64] = {
    0x0080004001201580,     0x0240200010004000,     0x0080100080982004,     0x0080100104880080,
    0x0100040300080010,     0x4200081002000401,     0x0400102088020104,     0x00800c4021000080,
    0x0040800440112084,     0x0000400040201000,     0x4042802000100088,     0x4002001200204008,
    0x8420800803804400,     0x0402004810120024,     0x0002000402002811,     0x1881000100028052,
    0x8000e18000400090,     0x01020200410080a2,     0x0a04420020820014,     0x4001010020181000,
    0x4441010010060800,     0x0024008002008024,     0x0080040012082110,     0x00004a0002448421,
    0x0080004040002002,     0x8080200040005000,     0x0002200280100081,     0x0400100080800800,
    0xa020040080080080,     0x0882010200100c08,     0x8000105400021881,     0x4200004200108423,
    0x0304400090800028,     0x4010002000c00651,     0x0400500088802000,     0x0410010011002028,
    0x0000800400802800,     0x4223801200800400,     0x000a002102000408,     0x220012884a000124,
    0x00002080c0008000,     0xa102200050004000,     0x0008314082020024,     0x0328904022020008,
    0xc008008084008008,     0x0004000200048080,     0x1007010002008080,     0x0190014484020009,
    0x3808204000800080,     0x0001400420008080,     0x020cc01020820200,     0x0000080090008280,
    0x0100080004110100,     0x00092200a4008080,     0x0040104102082400,     0x0203050888440200,
    0x0180084101108021,     0x4241012a00104082,     0x0202200041000911,     0x40000508a0100101,
    0x000200081004a102,     0x4003004812a40001,     0x00001002480902a4,     0x0010840084410022
};
const uint64_t BishopMagics[64] = {
    0x0020200b11090050,     0x0005210802098008,     0x80100400802000c2,     0x0009040300201010,
    0x0005104000008400,     0x0001101230802800,     0x00a40c0404840480,     0x0202060041243000,
    0x8802300471041400,     0x0500882101021214,     0x2100184094008a04,     0x00080c4040810000,
    0x0001108860101004,     0x0001220190181040,     0x8000442092101000,     0x0408210080900800,
    0x30201090200204b8,     0x0002001002222400,     0x100800404a104010,     0x0004040805612120,
    0xc0040002020a1100,     0x80208004100c8812,     0xa002020d01052000,     0x000a00690101010a,
    0x0010428108980110,     0x8041204010020250,     0x4208040898004050,     0x0005040108440080,
    0x8001080401004000,     0x8a09004008080805,     0x0429140008440420,     0x0002004802004200,
    0x0008021000400412,     0x0000901020080628,     0x4010822081040800,     0x0088202020880180,
    0x0800410040040040,     0x0122044200010080,     0x0964808200608800,     0x4014005081060288,
    0x1010921821014000,     0x0000849808002000,     0x4181008041001000,     0x180018a035000806,
    0x0000400852000440,     0x0820018a00901200,     0x02ea02422a00a405,     0x10212a0a00c00200,
    0x0001280802080010,     0x0a01090086600220,     0x20000a8248480410,     0x0280010441109064,
    0x0020006810ac0004,     0x0010420204030200,     0x001c901026008004,     0x0110621801022010,
    0x8240404208a02002,     0x8c1060404410a80a,     0x5000000021080840,     0x00d080010120a808,
    0x1412000a200c2419,     0x1140b4e082020204,     0x0020400208412100,     0x0222081001004100
};
template <auto AttackFunc, const Bitboard *Magics, size_t TableSize> _POSSIBLY_CONSTEXPR std::pair<std::array<Magic, 64>, std::array<Bitboard, TableSize>> generate_magic_table() {
    std::array<Magic, 64> table{};
    std::array<Bitboard, TableSize> attacks{};

    size_t offset = 0;

    for (Square sq = SQ_A1; sq < SQ_NONE; ++sq) {
        Bitboard occ = 0;
        Bitboard edges = ((attacks::MASK_RANK[0] | attacks::MASK_RANK[7]) & ~attacks::MASK_RANK[rank_of(sq)]) | ((attacks::MASK_FILE[0] | attacks::MASK_FILE[7]) & ~attacks::MASK_FILE[file_of(sq)]);

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
_POSSIBLY_CONSTEXPR std::pair<std::array<Magic, 64>, std::array<Bitboard, 0x1480>> bishopData = generate_magic_table<_chess::_HyperbolaBishopAttacks, attacks::BishopMagics, 0x1480>();
_POSSIBLY_CONSTEXPR std::array<Magic, 64> BishopTable = bishopData.first;
_POSSIBLY_CONSTEXPR std::array<Bitboard, 0x1480> BishopAttacks = bishopData.second;

_POSSIBLY_CONSTEXPR std::pair<std::array<Magic, 64>, std::array<Bitboard, 0x19000>> rookData = generate_magic_table<_chess::_HyperbolaRookAttacks, attacks::RookMagics, 0x19000>();
_POSSIBLY_CONSTEXPR std::array<Magic, 64> RookTable = rookData.first;
_POSSIBLY_CONSTEXPR std::array<Bitboard, 0x19000> RookAttacks = rookData.second;
} // namespace chess::attacks

namespace chess::movegen {
inline _POSSIBLY_CONSTEXPR Bitboard att(PieceType pt, Square sq, Bitboard occ) { return (pt == BISHOP) ? _chess::_HyperbolaBishopAttacks(sq, occ) : _chess ::_HyperbolaRookAttacks(sq, occ); }

inline _POSSIBLY_CONSTEXPR std::array<std::array<Bitboard, 64>, 64> generate_between() {
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
_POSSIBLY_CONSTEXPR std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN_BB = generate_between();
} // namespace chess::movegen