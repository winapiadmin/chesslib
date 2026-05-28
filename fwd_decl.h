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
#include <cstdint>
#include <type_traits>

/// @file fwd_decl.h
/// @brief Forward declarations for all major chess types.

namespace chess {

/// @enum Color
/// @brief Side to move or piece color.
enum Color : uint8_t;

/// @enum PieceType
/// @brief Basic piece type without color information.
enum PieceType : std::int8_t;

/// @brief Trait to detect piece-enum types (PolyglotPiece, EnginePiece, ContiguousMappingPiece).
template <typename T, typename = void> struct is_piece_enum : std::false_type {};

template <typename T> struct is_piece_enum<T, std::void_t<decltype(T::PIECE_NB)>> : std::true_type {};

/// @enum CastlingRights
/// @brief Bitmask of available castling rights.
enum CastlingRights : int8_t;
/// @enum Square
/// @brief Board square index (0-63, A1-H8).
enum Square : int8_t;
/// @enum Direction
/// @brief Compass direction offsets for board traversal.
enum Direction : int8_t;
/// @enum MoveType
/// @brief Move-type flags (normal, promotion, en-passant, castling).
enum MoveType : uint16_t;
/// @enum File
/// @brief File index (0-7, A-H).
enum File : int8_t;
/// @enum Rank
/// @brief Rank index (0-7, 1-8).
enum Rank : int8_t;

/// @class Move
/// @brief Compact 16-bit move representation.
class Move;

/// @enum MoveGenType
/// @brief Move-generation filter flags for piece type and move kind.
enum class MoveGenType : uint16_t;

/// @class _Position
/// @brief Templated chess position class parameterised by the piece enum.
template <typename T, typename> class _Position;

/// @typedef Bitboard
/// @brief 64-bit bitboard representing a set of squares.
using Bitboard = uint64_t;

/// @typedef Key
/// @brief 64-bit Zobrist hash key.
using Key = uint64_t;

/// @class ValueList
/// @brief Stack-allocated fixed-capacity container.
template <typename T, std::size_t MaxSize> class ValueList;

/// @typedef Movelist
/// @brief Fixed-capacity list of up to 300 moves.
using Movelist = ValueList<Move, 300>;

/// @enum PolyglotPiece
/// @brief Piece encoding used by Polyglot opening books.
enum class PolyglotPiece : uint8_t;

/// @enum EnginePiece
/// @brief Default engine piece encoding (8 values per colour).
enum class EnginePiece : uint8_t;

/// @enum ContiguousMappingPiece
/// @brief Compact piece encoding (0-5 white, 6-11 black).
enum class ContiguousMappingPiece : uint8_t;

/// @typedef Position
/// @brief Default chess position type (uses EnginePiece).
using Position = _Position<EnginePiece, void>;

/// @typedef Board
/// @brief Alias for Position.
using Board [[deprecated("Use Position instead")]] = Position;

} // namespace chess
