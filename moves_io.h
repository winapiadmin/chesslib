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
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

/// @file moves_io.h
/// @brief UCI and SAN move conversion functions.

namespace chess::uci {

/// @brief Convert a Move to UCI coordinate string (e.g. "e2e4", "e7e8q").
/// @param move The move.
/// @param chess960 Whether to use Chess960 castling encoding.
/// @return UCI string.
std::string moveToUci(Move move, bool chess960 = false);

/// @brief Convert a Square to algebraic notation (e.g. "e4").
/// @param sq The square.
/// @return Two-character string.
std::string squareToString(Square sq);

/// @brief Exception thrown when a SAN string represents an illegal move.
class IllegalMoveException : public std::exception {
  public:
    IllegalMoveException(const std::string &message) : message_(message) {}
    const char *what() const noexcept override { return message_.c_str(); }

  private:
    std::string message_;
};

/// @brief Exception thrown when a SAN string is ambiguous.
class AmbiguousMoveException : public std::exception {
  public:
    AmbiguousMoveException(const std::string &message) : message_(message) {}
    const char *what() const noexcept override { return message_.c_str(); }

  private:
    std::string message_;
};

/// @brief Parse a UCI string into a Move for the given position.
/// @tparam T Piece enum type.
/// @tparam P Position tag.
/// @param pos The position.
/// @param uci UCI string (e.g. "e2e4").
/// @return The parsed Move.
template <typename T, typename P = void> Move uciToMove(const _Position<T, P> &pos, std::string_view uci);

/// @brief Parse a SAN string into a Move for the given position.
/// @tparam T Piece enum type.
/// @tparam P Position tag.
/// @param pos The position.
/// @param san SAN string (e.g. "Nf3", "O-O").
/// @param remove_illegals If true, return Move::NO_MOVE instead of throwing.
/// @return The parsed Move.
template <typename T, typename P = void>
Move parseSan(const _Position<T, P> &pos, std::string_view san, bool remove_illegals = false);

/// @brief Alias for parseSan.
template <typename T, typename P = void>
Move parse_san(const _Position<T, P> &pos, std::string_view san, bool remove_illegals = false) {
    return parseSan(pos, san, remove_illegals);
}

/// @brief Convert a Move to SAN string for the given position.
/// @tparam T Piece enum type.
/// @tparam P Position tag.
/// @param pos The position.
/// @param move The move.
/// @param long_ Use long algebraic notation.
/// @param suffix Include check/mate suffix (+/#).
/// @return SAN string.
template <typename T, typename P = void>
std::string moveToSan(const _Position<T, P> &pos, Move move, bool long_ = false, bool suffix = true);

/// @brief Alias for moveToSan.
template <typename T, typename P = void>
std::string move_to_san(const _Position<T, P> &pos, Move move, bool long_ = false, bool suffix = true) {
    return moveToSan(pos, move, long_, suffix);
}
} // namespace chess::uci
