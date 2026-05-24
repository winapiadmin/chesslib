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
namespace chess::uci {
std::string moveToUci(Move move, bool chess960 = false);

std::string squareToString(Square sq);
class IllegalMoveException : public std::exception {
  public:
    IllegalMoveException(const std::string &message) : message_(message) {}
    const char *what() const noexcept override { return message_.c_str(); }

  private:
    std::string message_;
};
class AmbiguousMoveException : public std::exception {
  public:
    AmbiguousMoveException(const std::string &message) : message_(message) {}
    const char *what() const noexcept override { return message_.c_str(); }

  private:
    std::string message_;
};
template <typename T, typename P = void> Move uciToMove(const _Position<T, P> &pos, std::string_view uci);
template <typename T, typename P = void>
Move parseSan(const _Position<T, P> &pos, std::string_view uci, bool remove_illegals = false);
template <typename T, typename P = void>
Move parse_san(const _Position<T, P> &pos, std::string_view uci, bool remove_illegals = false) {
    return parseSan(pos, uci, remove_illegals);
}

template <typename T, typename P = void>
std::string moveToSan(const _Position<T, P> &pos, Move move, bool long_ = false, bool suffix = true);
template <typename T, typename P = void>
std::string move_to_san(const _Position<T, P> &pos, Move move, bool long_ = false, bool suffix = true) {
    return moveToSan(pos, move, long_, suffix);
}
} // namespace chess::uci
