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
#include <cstdint>
#include <iosfwd>
#include <type_traits>

/// @file printers.h
/// @brief Stream-output operators for chess types.

namespace chess {

/// @brief Print a Move as UCI string.
/// @param os Output stream.
/// @param mv The move.
/// @return Reference to the stream.
std::ostream &operator<<(std::ostream &os, const Move mv);

/// @brief Print a Color ("white" or "black").
std::ostream &operator<<(std::ostream &os, const Color c);

/// @brief Print CastlingRights (e.g. "KQkq").
std::ostream &operator<<(std::ostream &os, const CastlingRights cr);

/// @brief Print a piece (e.g. "P", "N", "K").
template <typename PieceC, typename = std::enable_if_t<is_piece_enum<PieceC>::value>>
std::ostream &operator<<(std::ostream &os, PieceC p);

/// @brief Print a square (e.g. "e4").
std::ostream &operator<<(std::ostream &os, const Square sq);

/// @brief Print a PieceType (e.g. "Pawn", "Knight").
std::ostream &operator<<(std::ostream &os, const PieceType pt);

template <typename T, typename> class _Position;

/// @brief Print a board position in human-readable ASCII.
template <typename PieceC = EnginePiece, typename = std::enable_if_t<is_piece_enum<PieceC>::value>>
std::ostream &operator<<(std::ostream &os, const _Position<PieceC, void> &pos);
} // namespace chess
