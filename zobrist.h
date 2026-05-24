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
#include <array>
#include <cstdint>

/// @file zobrist.h
/// @brief Zobrist hashing random constants.

namespace chess::zobrist {

/// @brief Random values for piece-square XOR.
/// @details Indexed as RandomPiece[piece_enum_idx][piece_value][square].
extern std::array<uint64_t, 64> *RandomPiece[];

/// @brief Random values for castling rights (indexed by CastlingRights bitmask, 0-15).
extern uint64_t RandomCastle[16];

/// @brief Random values for en-passant file (0-7 = files A-H, 8 = no EP).
extern uint64_t RandomEP[9];

/// @brief Random value XORed when it is black's turn to move.
extern uint64_t RandomTurn;
} // namespace chess::zobrist
