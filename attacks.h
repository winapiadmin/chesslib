#pragma once
#include "bitboard.h"
#include "types.h"
#include <array>
#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__BMI2__) || defined(__M_INTRINSIC_PROMOTE__)
#include <bmi2intrin.h>
#else
#include <immintrin.h>
#endif
namespace chess::attacks {
    // clang-format off
    // pre-calculated lookup table for pawn attacks
    static constexpr Bitboard PawnAttacks[2][64] = {
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
    static constexpr Bitboard KnightAttacks[64] = {
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
    static constexpr Bitboard KingAttacks[64] = {
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
    static constexpr Bitboard MASK_RANK[8] = {
        0xff,         0xff00,         0xff0000,         0xff000000,
        0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000};

    static constexpr Bitboard MASK_FILE[8] = {
        0x101010101010101,  0x202020202020202,  0x404040404040404,  0x808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
    };

    static constexpr Bitboard RookMagics[64] = {
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
        0x00020030a0244872ULL, 0x0012001008414402ULL, 0x02006104900a0804ULL, 0x0001004081002402ULL};

    static constexpr Bitboard BishopMagics[64] = {
        0x0040040844404084ULL, 0x002004208a004208ULL, 0x0010190041080202ULL, 0x0108060845042010ULL,
        0x0581104180800210ULL, 0x2112080446200010ULL, 0x1080820820060210ULL, 0x03c0808410220200ULL,
        0x0004050404440404ULL, 0x0000021001420088ULL, 0x24d0080801082102ULL, 0x0001020a0a020400ULL,
        0x0000040308200402ULL, 0x0004011002100800ULL, 0x0401484104104005ULL, 0x0801010402020200ULL,
        0x00400210c3880100ULL, 0x0404022024108200ULL, 0x0810018200204102ULL, 0x0004002801a02003ULL,
        0x0085040820080400ULL, 0x810102c808880400ULL, 0x000e900410884800ULL, 0x8002020480840102ULL,
        0x0220200865090201ULL, 0x2010100a02021202ULL, 0x0152048408022401ULL, 0x0020080002081110ULL,
        0x4001001021004000ULL, 0x800040400a011002ULL, 0x00e4004081011002ULL, 0x001c004001012080ULL,
        0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL,
        0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL,
        0x0209188240001000ULL, 0x400408a884001800ULL, 0x00110400a6080400ULL, 0x1840060a44020800ULL,
        0x0090080104000041ULL, 0x0201011000808101ULL, 0x1a2208080504f080ULL, 0x8012020600211212ULL,
        0x0500861011240000ULL, 0x0180806108200800ULL, 0x4000020e01040044ULL, 0x300000261044000aULL,
        0x0802241102020002ULL, 0x0020906061210001ULL, 0x5a84841004010310ULL, 0x0004010801011c04ULL,
        0x000a010109502200ULL, 0x0000004a02012000ULL, 0x500201010098b028ULL, 0x8040002811040900ULL,
        0x0028000010020204ULL, 0x06000020202d0240ULL, 0x8918844842082200ULL, 0x4010011029020020ULL};

    //clang-format on
    #ifdef __BMI2__
        constexpr uint64_t software_pext_u64(uint64_t val, uint64_t mask) {
            uint64_t result = 0;
            uint64_t bit_position = 0;

            for (uint64_t bit = 1; bit!= 0; bit <<= 1) {
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
            Bitboard       mask;
            int            index;
            constexpr Bitboard       operator()(Bitboard b) const {
                if (is_constant_evaluated()) { return software_pext_u64(b, mask); }
                else { return _pext_u64(b, mask); }
            }
        };
    #else
        struct Magic {
            Bitboard       mask;
            Bitboard       magic;
            int            index;
            Bitboard       shift;
            constexpr Bitboard       operator()(Bitboard b) const {
                return (((b & mask)) * magic) >> shift;
            }
        };
    #endif
    
}

namespace chess::_chess {
    // [INTERNAL]

    // Reverse bits horizontally in 64-bit integer
    constexpr Bitboard reverse(Bitboard b) {
        b = (b & 0x5555555555555555ULL) << 1  | ((b >> 1)  & 0x5555555555555555ULL);
        b = (b & 0x3333333333333333ULL) << 2  | ((b >> 2)  & 0x3333333333333333ULL);
        b = (b & 0x0f0f0f0f0f0f0f0fULL) << 4  | ((b >> 4)  & 0x0f0f0f0f0f0f0f0fULL);
        b = (b & 0x00ff00ff00ff00ffULL) << 8  | ((b >> 8)  & 0x00ff00ff00ff00ffULL);
        b = (b & 0x0000ffff0000ffffULL) << 16 | ((b >> 16) & 0x0000ffff0000ffffULL);
        b = (b & 0x00000000ffffffffULL) << 32 | ((b >> 32) & 0x00000000ffffffffULL);
        return b;
    }

    constexpr Bitboard hyp_quint(Bitboard sliderBB, Bitboard occ, Bitboard mask) {
        Bitboard occ_masked = occ & mask;
        Bitboard left = occ_masked - 2 * sliderBB;
        Bitboard right = reverse(occ_masked) - 2 * reverse(sliderBB);
        return (left ^ reverse(right)) & mask;
    }

    // For Bishop: Mask for diagonal and anti-diagonal
    constexpr Bitboard diag_mask(Square sq) {
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
    constexpr Bitboard antidiag_mask(Square sq) {
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

    // Hyperbola Quintessence for Bishop
    constexpr Bitboard _HyperbolaBishopAttacks(Square sq, Bitboard occ) {
        Bitboard slider = 1ULL << sq;
        Bitboard d_mask = diag_mask(sq);
        Bitboard ad_mask = antidiag_mask(sq);
        return hyp_quint(slider, occ, d_mask) | hyp_quint(slider, occ, ad_mask);
    }

    // For Rook: Rank and File Masks
    constexpr Bitboard rank_mask(Square sq) {
        return attacks::MASK_RANK[rank_of(sq)];
    }

    constexpr Bitboard file_mask(Square sq) {
        return attacks::MASK_FILE[file_of(sq)];
    }

    // Hyperbola Quintessence for Rook
    constexpr Bitboard _HyperbolaRookAttacks(Square sq, Bitboard occ) {
        Bitboard slider = 1ULL << sq;
        Bitboard r_mask = rank_mask(sq);
        Bitboard f_mask = file_mask(sq);
        return hyp_quint(slider, occ, r_mask) | hyp_quint(slider, occ, f_mask);
    }
}  // namespace chess::_chess [INTERNAL]
namespace chess::attacks{
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
    template<Direction direction>
    [[nodiscard]] static constexpr Bitboard shift(const Bitboard b){
        ASSUME(direction==NORTH||direction==EAST||direction==SOUTH||direction==WEST||
                 direction==NORTH_EAST||direction==SOUTH_EAST||direction==SOUTH_WEST||direction==NORTH_WEST);
		switch (direction)
		{
		case Direction::NORTH :
			return b << 8;
		case Direction::SOUTH :
			return b >> 8;
		case Direction::NORTH_WEST :
			return (b & ~MASK_FILE[0]) << 7;
		case Direction::WEST :
			return (b & ~MASK_FILE[0]) >> 1;
		case Direction::SOUTH_WEST :
			return (b & ~MASK_FILE[0]) >> 9;
		case Direction::NORTH_EAST :
			return (b & ~MASK_FILE[7]) << 9;
		case Direction::EAST :
			return (b & ~MASK_FILE[7]) << 1;
		case Direction::SOUTH_EAST :
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
    template<Color c>
    [[nodiscard]] constexpr Bitboard pawnLeftAttacks(const Bitboard pawns){
        ASSUME(c==WHITE||c==BLACK);
		return c == WHITE ? (pawns << 7) & ~MASK_FILE[7]
                                        : (pawns >> 7) & ~MASK_FILE[0];
	}

    /**
     * @brief Generate the right side pawn attacks.
     * @tparam c
     * @param pawns
     * @return
     */
    template<Color c>
    [[nodiscard]] constexpr Bitboard pawnRightAttacks(const Bitboard pawns){
        ASSUME(c==WHITE||c==BLACK);
		return c == WHITE ? (pawns << 9) & ~MASK_FILE[0]
                                        : (pawns >> 9) & ~MASK_FILE[7];
	}

    /**
     * @brief Generate the right side pawn attacks.
     * @tparam c
     * @param pawns
     * @return
     */
    template<Color c>
    [[nodiscard]] constexpr Bitboard pawn(const Bitboard pawns) {
        ASSUME(c==WHITE||c==BLACK);
        Bitboard attacks = 0ULL;
        if constexpr (c == WHITE) {
            attacks |= ((pawns & ~MASK_FILE[FILE_A]) << 7); // left captures
            attacks |= ((pawns & ~MASK_FILE[FILE_H]) << 9); // right captures
        }
        else {
            attacks |= ((pawns & ~MASK_FILE[FILE_H]) >> 7);
            attacks |= ((pawns & ~MASK_FILE[FILE_A]) >> 9);
        }
        return attacks;
	}

    /**
     * @brief Returns the pawn attacks for a given color and square
     * @param c
     * @param sq
     * @return
     */
    [[nodiscard]] constexpr Bitboard pawn(Color c, Square sq) {
		return PawnAttacks[(int)c][(int)sq];
	}

    /**
     * @brief Returns the knight attacks for a given square
     * @param sq
     * @return
     */
    [[nodiscard]] constexpr Bitboard knight(Square sq) {
		return KnightAttacks[(int)sq];
	}
    /**
     * @brief Returns the knight attacks for a given knights
     * @param sq
     * @return
     */
    [[nodiscard]]constexpr Bitboard knight(Bitboard knights) {
        Bitboard l1 = (knights >> 1) & 0x7f7f7f7f7f7f7f7fULL;  // shift left by 1, mask out file A
        Bitboard l2 = (knights >> 2) & 0x3f3f3f3f3f3f3f3fULL;  // shift left by 2, mask out files A+B
        Bitboard r1 = (knights << 1) & 0xfefefefefefefefeULL;  // shift right by 1, mask out file H
        Bitboard r2 = (knights << 2) & 0xfcfcfcfcfcfcfcfcULL;  // shift right by 2, mask out files G+H
        Bitboard h1 = l1 | r1; // 1-square horizontal shifts
        Bitboard h2 = l2 | r2; // 2-square horizontal shifts
        return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);  // vertical shifts: +2,+1,-2,-1
    }

    /**
     * @brief Returns the bishop attacks for a given square
     * @param sq
     * @param occupied
     * @return
     */
    [[nodiscard]] constexpr Bitboard bishop(Square sq, Bitboard occupied) {
        if (is_constant_evaluated()) { return _chess::_HyperbolaBishopAttacks(sq, occupied); }
		else { return BishopAttacks[BishopTable[(int)sq].index+BishopTable[(int)sq](occupied)]; }
	}

    /**
     * @brief Returns the rook attacks for a given square
     * @param sq
     * @param occupied
     * @return
     */
    [[nodiscard]] constexpr Bitboard rook(Square sq, Bitboard occupied) {
        if (is_constant_evaluated()) { return _chess::_HyperbolaRookAttacks(sq, occupied); }
		else { return RookAttacks[RookTable[(int)sq].index+RookTable[(int)sq](occupied)]; }
	}

    /**
     * @brief Returns the queen attacks for a given square
     * @param sq
     * @param occupied
     * @return
     */
    [[nodiscard]] constexpr Bitboard queen(Square sq, Bitboard occupied) {
		return bishop(sq, occupied) | rook(sq, occupied);
	}

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
	template<PieceType pt>
	[[nodiscard]] constexpr Bitboard slider(Square sq, Bitboard occupied) {
		static_assert(pt == PieceType::BISHOP || pt == PieceType::ROOK || pt == PieceType::QUEEN,
					  "PieceType must be a slider!");

		if constexpr (pt == PieceType::BISHOP)
			return bishop(sq, occupied);
		if constexpr (pt == PieceType::ROOK)
			return rook(sq, occupied);
		if constexpr (pt == PieceType::QUEEN)
			return queen(sq, occupied);
	}

} // namespace chess::attacks

namespace chess::movegen{
    inline constexpr Bitboard att(PieceType pt, Square sq, Bitboard occ) {
        return (pt == BISHOP) ? attacks::bishop(sq, occ)
                                : attacks::rook(sq, occ);
    }

    inline constexpr std::array<std::array<Bitboard, 64>, 64> generate_between()
    {
        std::array<std::array<Bitboard, 64>, 64> squares_between_bb{};

        for (int sq1 = 0; sq1 < 64; ++sq1)
        {
            for (PieceType pt : {BISHOP, ROOK})
            {
                for (int sq2 = 0; sq2 < 64; ++sq2)
                {
                    if (att(pt, Square(sq1), 0)&(1ULL<<sq2))
                    {
                        squares_between_bb[sq1][sq2] = att(pt, Square(sq1), 1ULL<<(sq2))
                                                        & att(pt, Square(sq2), 1ULL<<(sq1));
                    }
                    squares_between_bb[sq1][sq2]|=1ULL<<(sq2);
                }
            }
        }

        return squares_between_bb;
    }
    constexpr std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN_BB = generate_between();
    [[nodiscard]] inline constexpr Bitboard between(Square sq1, Square sq2) {
        return SQUARES_BETWEEN_BB[sq1][sq2];
    }
}
