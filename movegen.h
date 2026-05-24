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
extern std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN_BB;
/*
 * [(file(sq1), rank(sq1)), (file(sq2), rank(sq2))] -> bitboard of squares between sq1 and sq2, excluding sq1 and sq2
 */
[[nodiscard]] inline Bitboard between(Square sq1, Square sq2) noexcept { return SQUARES_BETWEEN_BB[sq1][sq2]; }
} // namespace chess::movegen
