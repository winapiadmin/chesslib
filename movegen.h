#pragma once
#include "fwd_decl.h"
#include <array>
namespace chess::movegen {

template <typename T, Color c> void genEP(const _Position<T, void> &, Movelist &);
template <typename T, Color c> void genPawnDoubleMoves(const _Position<T, void> &, Movelist &, Bitboard, Bitboard);
template <typename T, Color c, bool capturesOnly = false>
void genPawnSingleMoves(const _Position<T, void> &, Movelist &, Bitboard, Bitboard, Bitboard);
template <typename T, Color c, bool capturesOnly = false>
void genKnightMoves(const _Position<T, void> &, Movelist &, Bitboard, Bitboard);
template <typename T, Color c, bool capturesOnly = false> void genKingMoves(const _Position<T, void> &, Movelist &, Bitboard);
template <typename T, Color c, PieceType pt, bool capturesOnly = false>
void genSlidingMoves(const _Position<T, void> &, Movelist &, Bitboard, Bitboard, Bitboard);
extern std::array<std::array<Bitboard, 65>, 65> SQUARES_BETWEEN_BB;
/*
 * [(file(sq1), rank(sq1)), (file(sq2), rank(sq2))] -> bitboard of squares between sq1 and sq2, excluding sq1 and sq2
 */
[[nodiscard]] inline Bitboard between(Square sq1, Square sq2) noexcept { return SQUARES_BETWEEN_BB[sq1][sq2]; }
} // namespace chess::movegen
