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
#include "types.h"
#include <array>

/// @file movegen.h
/// @brief Move-generation declarations and between-square table.

namespace chess::movegen {

/// @brief Generate en-passant captures for the given colour.
template <typename T, Color c, typename ListT> void genEP(const _Position<T, void> &, ListT &);

/// @brief Generate double-pawn pushes (from the starting rank).
template <typename T, Color c, typename ListT> void genPawnDoubleMoves(const _Position<T, void> &, ListT &, Bitboard, Bitboard);

/// @brief Generate single-pawn moves (pushes and captures).
template <typename T, Color c, bool capturesOnly, typename ListT>
void genPawnSingleMoves(const _Position<T, void> &, ListT &, Bitboard, Bitboard, Bitboard);

/// @brief Generate knight moves.
template <typename T, Color c, bool capturesOnly, typename ListT>
void genKnightMoves(const _Position<T, void> &, ListT &, Bitboard, Bitboard);

/// @brief Generate king moves.
template <typename T, Color c, bool capturesOnly, typename ListT>
void genKingMoves(const _Position<T, void> &, ListT &, Bitboard);

/// @brief Generate sliding-piece moves (bishop, rook, queen).
template <typename T, Color c, PieceType pt, bool capturesOnly, typename ListT>
void genSlidingMoves(const _Position<T, void> &, ListT &, Bitboard, Bitboard, Bitboard);

/// @brief Precomputed between-square bitboards.
/// @details squares_between_bb[sq1][sq2] contains a bitboard of all squares
///          strictly between sq1 and sq2 (excluding both endpoints).
extern std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN_BB;

/// @brief Look up the squares between two squares.
/// @param sq1 First square.
/// @param sq2 Second square.
/// @return Bitboard of squares between sq1 and sq2, excluding sq1 and sq2.
///         Returns 1ULL << sq2 if the squares are not on the same line.
[[nodiscard]] inline Bitboard between(Square sq1, Square sq2) noexcept { return SQUARES_BETWEEN_BB[sq1][sq2]; }
} // namespace chess::movegen
