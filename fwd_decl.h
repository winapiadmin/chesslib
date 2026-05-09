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
namespace chess {
enum Color : uint8_t;
enum PieceType : std::int8_t;

template <typename T, typename = void> struct is_piece_enum : std::false_type {};

template <typename T> struct is_piece_enum<T, std::void_t<decltype(T::PIECE_NB)>> : std::true_type {};
enum CastlingRights : int8_t;
enum Square : int8_t;
enum Direction : int8_t;
enum MoveType : uint16_t;
enum File : int8_t;
enum Rank : int8_t;
class Move;
enum class MoveGenType : uint16_t;
template <typename T, typename> class _Position;
using Bitboard = uint64_t;
using Key = uint64_t;
template <typename T, std::size_t MaxSize> class ValueList;
using Movelist = ValueList<Move, 256>;
// bonus: define the piece enums here
enum class PolyglotPiece : uint8_t;
enum class EnginePiece : uint8_t;
enum class ContiguousMappingPiece : uint8_t;
using Position = _Position<EnginePiece, void>;
using Board = Position;

} // namespace chess
