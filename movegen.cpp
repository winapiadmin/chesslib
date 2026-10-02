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
// AVX512 and scalar bb-movegens

// License: https://github.com/official-stockfish/Stockfish/blob/master/COPYING.txt
// movegen references

// License: https://github.com/Disservin/chess-library/blob/master/LICENSE

/// @file movegen.cpp
/// @brief Move generator: AVX-512 accelerated splatting and per-piece-type move generation.

#include "movegen.h"
#include "position.h"

namespace chess {

#if defined(_MSC_VER)
#define RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
#define RESTRICT __restrict__
#else
#define RESTRICT
#endif

namespace _chess {

#if defined(__AVX512F__) && defined(__AVX512VNNI__) && defined(__AVX512VBMI2__)

// clang-format off
const __m512i AllSquares = _mm512_set_epi8(
  63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41,
  40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18,
  17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
// clang-format on

template <Direction offset> /**
                             * @brief Packs pawn destination squares into move objects.
                             *
                             * @tparam offset Direction offset from destination to origin squares.
                             * @param moveList Output buffer where move objects are written.
                             * @param to_bb Bitboard of destination squares (at most 8 bits set).
                             * @return Pointer advanced by popcount(to_bb).
                             */
inline Move *splat_pawn_moves(Move *RESTRICT moveList, Bitboard to_bb) {
    assert(popcount(to_bb) <= 8); // <= 8 pawns per side

    const __m128i toSquares = _mm_cvtepi8_epi16(_mm512_castsi512_si128(_mm512_maskz_compress_epi8(to_bb, AllSquares)));
    const __m128i fromSquares = _mm_subs_epi16(toSquares, _mm_set1_epi16(offset));
    const __m128i moves = _mm_or_si128(_mm_slli_epi16(fromSquares, 6), _mm_slli_epi16(toSquares, 0));

    _mm_storeu_si128(reinterpret_cast<__m128i *>(moveList), moves);
    return moveList + popcount(to_bb);
}

/**
 * @brief Convert a destination bitboard into move objects from a fixed source square.
 *
 * @param moveList Output array where moves are stored.
 * @param from Source square for all moves.
 * @param to_bb Bitboard of destination squares (popcount must not exceed 32).
 * @return Pointer advanced by the number of moves written.
 */
inline Move *splat_moves(Move *moveList, Square from, Bitboard to_bb) {
    assert(popcount(to_bb) <= 32); // Q can attack up to 27 squares

    const __m512i fromVec = _mm512_set1_epi16(Move(from, SQUARE_ZERO).raw());
    const __m512i toSquares = _mm512_cvtepi8_epi16(_mm512_castsi512_si256(_mm512_maskz_compress_epi8(to_bb, AllSquares)));
    const __m512i moves = _mm512_or_si512(fromVec, _mm512_slli_epi16(toSquares, 0));

    _mm512_storeu_si512(moveList, moves);
    return moveList + popcount(to_bb);
}

#elif defined(__AVX512BW__) && defined(__AVX512VBMI2__)
// NOTE: this branch's write_moves() uses _mm512_maskz_compress_epi16, which is an
// AVX512VBMI2 instruction (VPCOMPRESSW) despite operating on the "BW" (byte/word) data
// type — AVX512BW alone does not provide it. Guarding on __AVX512BW__ alone used to let
// this branch get selected (and fail to compile) on real Skylake-X/Cascade Lake/Cooper
// Lake Xeons, which have AVX512BW+AVX512VNNI but lack AVX512VBMI2. Those CPUs are common
// in cloud/CI fleets, so a bare `-march=native` build would break there. Requiring
// AVX512VBMI2 here means such CPUs now correctly fall through to the portable scalar path
// below instead of failing the build.
template <int Offset = 0> struct alignas(64) SplatTable {
    std::array<uint16_t, 64> data;
    constexpr int clamp64(int x) { return (x < 0) ? 0 : (x > 63 ? 63 : x); }

    constexpr SplatTable() : data{} {
        for (int i = 0; i < 64; ++i) {
            int from = clamp64(i - Offset);
            data[i] = Move((Square)from, (Square)i).raw();
        }
    }
};

constexpr SplatTable<> SPLAT_TABLE{};
template <int Offset> constexpr SplatTable<Offset> SPLAT_PAWN_TABLE{};
// AVX-512 (32 lanes of uint16_t)
/**
 * @brief Compresses and stores selected moves from a vectorized batch.
 *
 * Stores only the moves from the vector at positions indicated by the mask,
 * compressing them into the output buffer and advancing the output pointer.
 *
 * @param moveList Output buffer for move storage.
 * @param mask Bitmask indicating which vector lanes contain valid moves.
 * @param vector 512-bit vector of move data in 16-bit lanes.
 * @return Pointer to the next available position in the output buffer.
 */
static inline Move *write_moves(Move *RESTRICT moveList, uint32_t mask, __m512i vector) {
    // Avoid _mm512_mask_compressstoreu_epi16() as it's 256 uOps on Zen4
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(moveList), _mm512_maskz_compress_epi16(mask, vector));
    return moveList + popcount(mask);
}

inline Move *splat_moves(Move *RESTRICT moveList, Square from, Bitboard to_bb) {
    const auto *table = reinterpret_cast<const __m512i *>(SPLAT_TABLE.data.data());
    __m512i fromVec = _mm512_set1_epi16(Move(from, SQUARE_ZERO).raw());
    // two 32-lane blocks (0..31, 32..63)
    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 0), _mm512_or_si512(_mm512_load_si512(table + 0), fromVec));
    moveList =
        write_moves(moveList, static_cast<uint32_t>(to_bb >> 32), _mm512_or_si512(_mm512_load_si512(table + 1), fromVec));

    return moveList;
}

template <Direction offset> inline Move *splat_pawn_moves(Move *RESTRICT moveList, Bitboard to_bb) {
    const auto *table = reinterpret_cast<const __m512i *>(SPLAT_PAWN_TABLE<offset>.data.data());
    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 0), _mm512_load_si512(table + 0));
    moveList = write_moves(moveList, static_cast<uint32_t>(to_bb >> 32), _mm512_load_si512(table + 1));

    return moveList;
}
#else
template <Direction offset> inline Move *splat_pawn_moves(Move *RESTRICT moveList, Bitboard to_bb) {
    while (to_bb) {
        auto to = static_cast<Square>(pop_lsb(to_bb));
#if defined(_DEBUG) || !defined(NDEBUG)
        Square from = to - offset;
        assert(from >= 0 && from < 64); // sanity check
        *moveList++ = Move(from, to);
#else
        *moveList++ = Move(to - offset, to);
#endif
    }
    return moveList;
}

inline Move *splat_moves(Move *RESTRICT moveList, Square from, Bitboard to_bb) {
    while (to_bb)
        *moveList++ = Move(from, static_cast<Square>(pop_lsb(to_bb)));
    return moveList;
}
#endif
} // namespace _chess
/**
 * @brief Appends moves from a source square to destination squares, or counts them.
 *
 * For Movelist, generates and stores all moves efficiently. For CountOnlyList,
 * only increments the count. For other list types, appends placeholder moves.
 *
 * @param list The list of moves
 * @param from Source square for all moves.
 * @param targets Bitboard of destination squares.
 */
template <typename ListT> inline void record_moves(ListT &list, Square from, Bitboard targets) {
    if constexpr (std::is_same_v<ListT, Movelist>) {
        _chess::splat_moves(list.data() + list.size_, from, targets);
        list.size_ += popcount(targets);
    } else if constexpr (std::is_same_v<ListT, CountOnlyList>) {
        list.size_ += popcount(targets);
    } else {
        while (targets) {
            list.push_back(Move::none());
            pop_lsb(targets);
        }
    }
}

template <Direction offset,
          typename ListT> /**
                           * @brief Records pawn promotion moves for each destination square.
                           *
                           * For each destination in `dests`, records four promotion moves: knight, bishop, rook, and queen.
                           * The source square is computed by subtracting the `offset` template parameter from the destination.
                           *
                           * @param list Move list to accumulate promotions, or a count-only list.
                           * @param dests Bitboard of destination squares where pawns promote.
                           */
inline void record_promotions(ListT &list, Bitboard dests) {
    if constexpr (std::is_same_v<ListT, Movelist>) {
        while (dests) {
            Square to = static_cast<Square>(pop_lsb(dests));
            Square from = static_cast<Square>(to - offset);
            list[list.size_] = Move::make<PROMOTION>(from, to, KNIGHT);
            list[list.size_ + 1] = Move::make<PROMOTION>(from, to, BISHOP);
            list[list.size_ + 2] = Move::make<PROMOTION>(from, to, ROOK);
            list[list.size_ + 3] = Move::make<PROMOTION>(from, to, QUEEN);
            list.size_ += 4;
        }
    } else if constexpr (std::is_same_v<ListT, CountOnlyList>) {
        list.size_ += 4 * popcount(dests);
    } else {
        UNREACHABLE();
    }
}
template <Direction offset, typename ListT> /**
                                             * @brief Records or counts pawn moves from destination squares.
                                             *
                                             * For `Movelist`, stores pawn moves with origin squares derived from the
                                             * destinations via the compile-time `offset` parameter. For `CountOnlyList`,
                                             * increments the move counter without storing moves.
                                             */
inline void record_pawn_moves(ListT &list, Bitboard targets) {
    if constexpr (std::is_same_v<ListT, Movelist>) {
        _chess::splat_pawn_moves<offset>(list.data() + list.size_, targets);
        list.size_ += popcount(targets);
    } else if constexpr (std::is_same_v<ListT, CountOnlyList>) {
        // CountOnlyList doesn't store moves; just increase the counter.
        list.size_ += popcount(targets);
    } else {
        UNREACHABLE();
    }
}
} // namespace chess
namespace chess {
template <typename T, Color c, typename ListT> /**
                                                 * @brief Generates all legal en passant captures for the moving side.
                                                 */
void movegen::genEP(const _Position<T, void> &pos, ListT &mv, Bitboard occ_all) {

    const Square king_sq = pos.king_sq(c);
    const Square ep_sq = pos.ep_square();
    if (ep_sq == SQ_NONE)
        return;

    Bitboard candidates = attacks::pawn(~c, ep_sq) & pos.template pieces<PAWN, c>();
    if (!candidates)
        return;

    const Square ep_pawn_sq = ep_sq - pawn_push(c);
    const Bitboard ep_mask = (1ULL << ep_pawn_sq) | (1ULL << ep_sq);

    const Bitboard enemy_bq =
    	pos.template pieces<BISHOP, ~c>() | pos.template pieces<QUEEN, ~c>();
	
	const Bitboard enemy_rq =
    	pos.template pieces<ROOK, ~c>() | pos.template pieces<QUEEN, ~c>();
    ASSUME(popcount(candidates)<=2);
    while (candidates) {
        Square from = static_cast<Square>(pop_lsb(candidates));

        // Remove the EP pawn and this attacker from occupancy
        Bitboard occ_temp = occ_all ^ ((1ULL << from) | ep_mask);

        // attackers check
        Bitboard atks = 0;
		if (attacks::bishop(king_sq, occ_temp) & enemy_bq)
    		continue;
		
		if (attacks::rook(king_sq, occ_temp) & enemy_rq)
    		continue;
		
        mv.push_back(Move::make<EN_PASSANT>(from, ep_sq));
    }
}
template <typename T, Color c, typename ListT>
/**
 * @brief Generates pawn double-step pushes from the starting rank.
 *
 * Respects pin constraints and check evasion requirements.
 */
void movegen::genPawnDoubleMoves(const _Position<T, void> &pos, ListT &moves, Bitboard pin_mask, Bitboard check_mask, Bitboard all_occ) {
    constexpr Bitboard RANK_2 = (c == WHITE) ? attacks::MASK_RANK[1] : attacks::MASK_RANK[6];
    constexpr Direction UP = pawn_push(c);

    Bitboard pawns = pos.template pieces<PAWN, c>() & RANK_2;

    // Split pin types
    Bitboard pin_file = pin_mask & attacks::MASK_FILE[file_of(pos.king_sq(c))];

    Bitboard unpinned = pawns & ~pin_mask;
    Bitboard file_pinned = pawns & pin_file;

    // First step must be empty
    Bitboard step1_unpinned = attacks::shift<UP>(unpinned) & ~all_occ;
    Bitboard step1_pinned = attacks::shift<UP>(file_pinned) & pin_file & ~all_occ;

    // Second step must also be empty
    Bitboard step2_unpinned = attacks::shift<UP>(step1_unpinned) & ~all_occ;
    Bitboard step2_pinned = attacks::shift<UP>(step1_pinned) & pin_file & ~all_occ;

    Bitboard destinations = (step2_unpinned | step2_pinned) & check_mask;

    record_pawn_moves<2 * UP>(moves, destinations);
}
template <typename T, Color c, bool capturesOnly, typename ListT>
/**
 * @brief Generates pawn single-step pushes and captures, including promotions, while respecting pin and check constraints.
 *
 * Generates all legal single-square pawn moves in the forward direction and diagonal captures.
 * Handles promotions when pawns reach the promotion rank. Respects piece pinning constraints
 * (rook and bishop pins) and check evasion mask filtering. For `capturesOnly` mode, omits
 * non-capturing forward moves.
 *
 * @param pos The position to generate moves from.
 * @param moves The move list to append generated moves to.
 * @param _rook_pin Bitmask of pawns pinned along rook lines (vertical/horizontal).
 * @param _bishop_pin Bitmask of pawns pinned along bishop lines (diagonal).
 * @param _check_mask Bitmask of squares that moves must target to be legal (check evasion).
 */
void movegen::genPawnSingleMoves(
    const _Position<T, void> &pos, ListT &moves, Bitboard _rook_pin, Bitboard _bishop_pin, Bitboard _check_mask, Bitboard occ_all) {
    constexpr auto UP = relative_direction(c, NORTH);
    constexpr auto UP_LEFT = relative_direction(c, NORTH_WEST);
    constexpr auto UP_RIGHT = relative_direction(c, NORTH_EAST);
    constexpr auto RANK_PROMO = attacks::MASK_RANK[relative_rank(c, RANK_8)];

    const auto pawns = pos.template pieces<PAWN, c>();
    const Bitboard enemy_king = 1ULL << pos.kingSq(~c);
    const auto occ_opp = pos.occ(~c) & ~enemy_king;
    // These pawns can maybe take Left or Right
    const Bitboard pawns_lr = pawns & ~_rook_pin;
    const Bitboard unpinned_pawns_lr = pawns_lr & ~_bishop_pin;
    const Bitboard pinned_pawns_lr = pawns_lr & _bishop_pin;

    auto l_pawns = attacks::shift<UP_LEFT>(unpinned_pawns_lr) | (attacks::shift<UP_LEFT>(pinned_pawns_lr) & _bishop_pin);
    auto r_pawns = attacks::shift<UP_RIGHT>(unpinned_pawns_lr) | (attacks::shift<UP_RIGHT>(pinned_pawns_lr) & _bishop_pin);
    // Prune moves that don't capture a piece and are not on the checkmask.
    l_pawns &= occ_opp & _check_mask;
    r_pawns &= occ_opp & _check_mask;

    // These pawns can walk Forward
    const auto pawns_hv = pawns & ~_bishop_pin;

    const auto pawns_pinned_hv = pawns_hv & _rook_pin;
    const auto pawns_unpinned_hv = pawns_hv & ~_rook_pin;

    // Prune moves that are blocked by a piece
    const auto single_push_unpinned = attacks::shift<UP>(pawns_unpinned_hv) & ~occ_all;
    const auto single_push_pinned = attacks::shift<UP>(pawns_pinned_hv) & _rook_pin & ~occ_all;

    // Prune moves that are not on the checkmask.
    Bitboard single_push = (single_push_unpinned | single_push_pinned) & _check_mask;
    {
        Bitboard promo_left = l_pawns & RANK_PROMO;
        Bitboard promo_right = r_pawns & RANK_PROMO;
        Bitboard promo_push = single_push & RANK_PROMO;

        if constexpr (!capturesOnly) {
            record_promotions<UP>(moves, promo_push);
        }

        record_promotions<UP_LEFT>(moves, promo_left);
        record_promotions<UP_RIGHT>(moves, promo_right);
    }

    single_push &= ~RANK_PROMO;
    l_pawns &= ~RANK_PROMO;
    r_pawns &= ~RANK_PROMO;
    if constexpr (!capturesOnly) {
        record_pawn_moves<UP>(moves, single_push);
    }
    record_pawn_moves<UP_LEFT>(moves, l_pawns);
    record_pawn_moves<UP_RIGHT>(moves, r_pawns);
}
template <typename T, Color c, bool capturesOnly, typename ListT>
/**
 * @brief Generates legal knight moves for the given color.
 *
 * Generates all knight moves subject to pin and check constraints.
 * If `capturesOnly` is true, restricts to capture moves only.
 *
 * @param pos The position.
 * @param list The move list to record moves into.
 * @param _pin_mask Bitboard of pinned pieces; pinned knights are excluded.
 * @param _check_mask Bitboard indicating squares that resolve checks.
 */
void movegen::genKnightMoves(const _Position<T, void> &pos, ListT &list, Bitboard _pin_mask, Bitboard _check_mask, Bitboard occ_all) {
    Bitboard knights = pos.template pieces<KNIGHT, c>() & ~_pin_mask;
    Bitboard enemy_king = 1ULL << pos.kingSq(~c);
    Bitboard mask1 = ~(pos.occ(c) | enemy_king) & _check_mask;
    if constexpr (capturesOnly)
        mask1 &= pos.occ(~c);
    while (knights) {
        Square x = static_cast<Square>(pop_lsb(knights));
        Bitboard moves = attacks::knight(x) & mask1;
        record_moves(list, x, moves);
    }
}
template <typename T, Color c, bool capturesOnly, typename ListT>
/**
 * @brief Generates legal king moves and castling.
 *
 * Computes all legal king destination squares by excluding occupied friendly squares and squares attacked by enemy pieces.
 * When `capturesOnly` is true, only captures are generated. Otherwise, also generates castling moves if the king is not in
 * check, the castling path is unobstructed, and all squares the king passes through are not under attack.
 *
 * @param pos The position.
 * @param out The move list to record moves into.
 * @param _pin_mask Bitboard of pinned pieces; used to filter illegal castling moves.
 */
void movegen::genKingMoves(const _Position<T, void> &pos, ListT &out, Bitboard _pin_mask, Bitboard occ_all) {
    constexpr Color them = ~c;
    const Square kingSq = pos.king_sq(c);
    ASSUME(kingSq >= SQ_A1 && kingSq <= SQ_H8);
    const Bitboard myOcc = pos.occ(c);
    const Bitboard enemy_king_bb = pos.template pieces<KING,them>();
    const Bitboard occ_opp = pos.occ(~c) & ~enemy_king_bb;
    const Bitboard kingAttacks = attacks::king(kingSq);
    if constexpr (capturesOnly) {
        if (UNLIKELY(!(kingAttacks & occ_opp))) return;
    }

    Bitboard enemyAttacks = pos.enemy_attacks();

    Bitboard moves = kingAttacks & ~myOcc & ~enemyAttacks;
    if constexpr (capturesOnly)
        moves &= occ_opp;
    record_moves(out, kingSq, moves);
    if constexpr (!capturesOnly) {
        if (UNLIKELY(pos.checkers()))
            return;

        Bitboard enemy_attacks = enemyAttacks;
        constexpr CastlingRights kingRights = KING_SIDE & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING),
                                 queenRights = QUEEN_SIDE & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING);
        Bitboard OO_EMPTY = pos.get_castling_path(c, true);
        Bitboard OO_SAFE = between(kingSq, castling_king_square(c, true));
        Bitboard OOO_EMPTY = pos.get_castling_path(c, false);
        Bitboard OOO_SAFE = between(kingSq, castling_king_square(c, false));
        Square rookKing = pos.get_castling_metadata(c).rook_start_ks, rookQueen = pos.get_castling_metadata(c).rook_start_qs;

        if (pos.castlingRights() & kingRights &&
            !(occ_all & OO_EMPTY || enemy_attacks & OO_SAFE || _pin_mask & 1ULL << rookKing)) {
            out.push_back(Move::make<CASTLING>(kingSq, rookKing));
        }
        if (pos.castlingRights() & queenRights &&
            !(occ_all & OOO_EMPTY || enemy_attacks & OOO_SAFE || _pin_mask & 1ULL << rookQueen)) {
            out.push_back(Move::make<CASTLING>(kingSq, rookQueen));
        }
    }
}
template <typename T, Color c, PieceType pt, bool capturesOnly, typename ListT>
/**
 * @brief Generates legal moves for sliding pieces (bishop, rook, or queen).
 *
 * Computes all valid destination squares for each sliding piece on the board, applying pin
 * constraints and check restrictions. Pieces pinned along rook lines are confined to those lines;
 * pieces pinned along bishop lines are confined to those diagonals.
 *
 * @param pos The position.
 * @param moves The move list to record moves into.
 * @param _rook_pin Bitboard of squares pinned along rook lines (vertical/horizontal).
 * @param _bishop_pin Bitboard of squares pinned along bishop lines (diagonals).
 * @param _check_mask Bitboard of legal destination squares when in check.
 */
void movegen::genSlidingMoves(
    const _Position<T, void> &pos, ListT &moves,
    Bitboard _rook_pin, Bitboard _bishop_pin, Bitboard _check_mask, Bitboard occ_all) {

    static_assert(pt == BISHOP || pt == ROOK || pt == QUEEN, "Sliding pieces only.");

    Bitboard sliders = pos.template pieces<pt, c>();
    if constexpr (pt == BISHOP) sliders &= ~_rook_pin;
    if constexpr (pt == ROOK)   sliders &= ~_bishop_pin;

    const Bitboard occ_opp    = pos.occ(~c);
    const Bitboard enemy_king = pos.template pieces<KING, ~c>();
    const Bitboard filter     = ~(pos.occ(c) | enemy_king) & _check_mask;

    while (sliders) {
        const Square   from    = static_cast<Square>(pop_lsb(sliders));
        const Bitboard from_bb = 1ULL << from;

        Bitboard pin_mask;
        if constexpr (pt == BISHOP) {
            pin_mask = (_bishop_pin & from_bb) ? _bishop_pin : ~0ULL;
        } else if constexpr (pt == ROOK) {
            pin_mask = (_rook_pin & from_bb) ? _rook_pin : ~0ULL;
        } else {
            pin_mask = (_rook_pin   & from_bb) ? _rook_pin
                     : (_bishop_pin & from_bb) ? _bishop_pin
                                               : ~0ULL;
        }
        const Bitboard filtered = pin_mask & filter;

        Bitboard targets;
        if constexpr (pt == BISHOP) {
            targets = attacks::bishop(from, occ_all) & filtered;
        } else if constexpr (pt == ROOK) {
            targets = attacks::rook(from, occ_all) & filtered;
        } else {
            const bool r = (_rook_pin   & from_bb) != 0;
            const bool b = !r && (_bishop_pin & from_bb) != 0;
            targets = r ? attacks::rook(from, occ_all)
                    : b ? attacks::bishop(from, occ_all)
                        : attacks::queen(from, occ_all);
            targets &= filtered;
        }

        if constexpr (capturesOnly) targets &= occ_opp;
        record_moves(moves, from, targets);
    }
}
#define INSTANTIATE(PieceC, ListT)                                                                                             \
    template void chess::movegen::genEP<PieceC, Color::WHITE, ListT>(const _Position<PieceC, void> &, ListT &, Bitboard);      \
    template void chess::movegen::genEP<PieceC, Color::BLACK, ListT>(const _Position<PieceC, void> &, ListT &, Bitboard);      \
    template void chess::movegen::genPawnDoubleMoves<PieceC, Color::WHITE, ListT>(const _Position<PieceC, void> &,             \
                                                                                   ListT &,                                     \
                                                                                   Bitboard,                                    \
                                                                                   Bitboard,                                    \
                                                                                   Bitboard);                                   \
    template void chess::movegen::genPawnDoubleMoves<PieceC, Color::BLACK, ListT>(const _Position<PieceC, void> &,             \
                                                                                   ListT &,                                     \
                                                                                   Bitboard,                                    \
                                                                                   Bitboard,                                    \
                                                                                   Bitboard);                                   \
    template void chess::movegen::genPawnSingleMoves<PieceC, Color::WHITE, true, ListT>(const _Position<PieceC, void> &,       \
                                                                                         ListT &,                               \
                                                                                         Bitboard,                              \
                                                                                         Bitboard,                              \
                                                                                         Bitboard,                              \
                                                                                         Bitboard);                             \
    template void chess::movegen::genPawnSingleMoves<PieceC, Color::WHITE, false, ListT>(const _Position<PieceC, void> &,      \
                                                                                          ListT &,                              \
                                                                                          Bitboard,                             \
                                                                                          Bitboard,                             \
                                                                                          Bitboard,                             \
                                                                                          Bitboard);                            \
    template void chess::movegen::genPawnSingleMoves<PieceC, Color::BLACK, true, ListT>(const _Position<PieceC, void> &,       \
                                                                                         ListT &,                               \
                                                                                         Bitboard,                              \
                                                                                         Bitboard,                              \
                                                                                         Bitboard,                              \
                                                                                         Bitboard);                             \
    template void chess::movegen::genPawnSingleMoves<PieceC, Color::BLACK, false, ListT>(const _Position<PieceC, void> &,      \
                                                                                          ListT &,                              \
                                                                                          Bitboard,                             \
                                                                                          Bitboard,                             \
                                                                                          Bitboard,                             \
                                                                                          Bitboard);                            \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, BISHOP, true, ListT>(const _Position<PieceC, void> &,  \
                                                                                              ListT &,                          \
                                                                                              Bitboard,                         \
                                                                                              Bitboard,                         \
                                                                                              Bitboard,                         \
                                                                                              Bitboard);                        \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, ROOK, true, ListT>(const _Position<PieceC, void> &,    \
                                                                                            ListT &,                            \
                                                                                            Bitboard,                           \
                                                                                            Bitboard,                           \
                                                                                            Bitboard,                           \
                                                                                            Bitboard);                          \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, QUEEN, true, ListT>(const _Position<PieceC, void> &,   \
                                                                                             ListT &,                           \
                                                                                             Bitboard,                          \
                                                                                             Bitboard,                          \
                                                                                             Bitboard,                          \
                                                                                             Bitboard);                         \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, BISHOP, false, ListT>(const _Position<PieceC, void> &, \
                                                                                               ListT &,                         \
                                                                                               Bitboard,                        \
                                                                                               Bitboard,                        \
                                                                                               Bitboard,                        \
                                                                                               Bitboard);                       \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, ROOK, false, ListT>(const _Position<PieceC, void> &,   \
                                                                                             ListT &,                           \
                                                                                             Bitboard,                          \
                                                                                             Bitboard,                          \
                                                                                             Bitboard,                          \
                                                                                             Bitboard);                         \
    template void chess::movegen::genSlidingMoves<PieceC, Color::WHITE, QUEEN, false, ListT>(const _Position<PieceC, void> &,  \
                                                                                              ListT &,                          \
                                                                                              Bitboard,                         \
                                                                                              Bitboard,                         \
                                                                                              Bitboard,                         \
                                                                                              Bitboard);                        \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, BISHOP, true, ListT>(const _Position<PieceC, void> &,  \
                                                                                              ListT &,                          \
                                                                                              Bitboard,                         \
                                                                                              Bitboard,                         \
                                                                                              Bitboard,                         \
                                                                                              Bitboard);                        \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, ROOK, true, ListT>(const _Position<PieceC, void> &,    \
                                                                                            ListT &,                            \
                                                                                            Bitboard,                           \
                                                                                            Bitboard,                           \
                                                                                            Bitboard,                           \
                                                                                            Bitboard);                          \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, QUEEN, true, ListT>(const _Position<PieceC, void> &,   \
                                                                                             ListT &,                           \
                                                                                             Bitboard,                          \
                                                                                             Bitboard,                          \
                                                                                             Bitboard,                          \
                                                                                             Bitboard);                         \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, BISHOP, false, ListT>(const _Position<PieceC, void> &, \
                                                                                               ListT &,                         \
                                                                                               Bitboard,                        \
                                                                                               Bitboard,                        \
                                                                                               Bitboard,                        \
                                                                                               Bitboard);                       \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, ROOK, false, ListT>(const _Position<PieceC, void> &,   \
                                                                                             ListT &,                           \
                                                                                             Bitboard,                          \
                                                                                             Bitboard,                          \
                                                                                             Bitboard,                          \
                                                                                             Bitboard);                         \
    template void chess::movegen::genSlidingMoves<PieceC, Color::BLACK, QUEEN, false, ListT>(const _Position<PieceC, void> &,  \
                                                                                              ListT &,                          \
                                                                                              Bitboard,                         \
                                                                                              Bitboard,                         \
                                                                                              Bitboard,                         \
                                                                                              Bitboard);                        \
    template void chess::movegen::genKnightMoves<PieceC, Color::WHITE, true, ListT>(const _Position<PieceC, void> &,           \
                                                                                     ListT &,                                   \
                                                                                     Bitboard,                                  \
                                                                                     Bitboard,                                  \
                                                                                     Bitboard);                                 \
    template void chess::movegen::genKnightMoves<PieceC, Color::BLACK, true, ListT>(const _Position<PieceC, void> &,           \
                                                                                     ListT &,                                   \
                                                                                     Bitboard,                                  \
                                                                                     Bitboard,                                  \
                                                                                     Bitboard);                                 \
    template void chess::movegen::genKnightMoves<PieceC, Color::WHITE, false, ListT>(const _Position<PieceC, void> &,          \
                                                                                      ListT &,                                  \
                                                                                      Bitboard,                                 \
                                                                                      Bitboard,                                 \
                                                                                      Bitboard);                                \
    template void chess::movegen::genKnightMoves<PieceC, Color::BLACK, false, ListT>(const _Position<PieceC, void> &,          \
                                                                                      ListT &,                                  \
                                                                                      Bitboard,                                 \
                                                                                      Bitboard,                                 \
                                                                                      Bitboard);                                \
    template void chess::movegen::genKingMoves<PieceC, Color::WHITE, true, ListT>(const _Position<PieceC, void> &,             \
                                                                                   ListT &,                                     \
                                                                                   Bitboard,                                    \
                                                                                   Bitboard);                                   \
    template void chess::movegen::genKingMoves<PieceC, Color::BLACK, true, ListT>(const _Position<PieceC, void> &,             \
                                                                                   ListT &,                                     \
                                                                                   Bitboard,                                    \
                                                                                   Bitboard);                                   \
    template void chess::movegen::genKingMoves<PieceC, Color::WHITE, false, ListT>(const _Position<PieceC, void> &,            \
                                                                                    ListT &,                                    \
                                                                                    Bitboard,                                   \
                                                                                    Bitboard);                                  \
    template void chess::movegen::genKingMoves<PieceC, Color::BLACK, false, ListT>(const _Position<PieceC, void> &,            \
                                                                                    ListT &,                                    \
                                                                                    Bitboard,                                   \
                                                                                    Bitboard);
INSTANTIATE(EnginePiece, Movelist)
INSTANTIATE(PolyglotPiece, Movelist)
INSTANTIATE(ContiguousMappingPiece, Movelist)
INSTANTIATE(EnginePiece, CountOnlyList)
INSTANTIATE(PolyglotPiece, CountOnlyList)
INSTANTIATE(ContiguousMappingPiece, CountOnlyList)
} // namespace chess
