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

/// @file printers.cpp
/// @brief Stream-operator overloads for board display, move printing, and FEN output.

#include "printers.h"
#include "moves_io.h"
#include "position.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <unordered_map>
namespace chess {
template <typename T> using DescriptiveNameNotation = std::unordered_map<T, std::string>;

/// @brief Print a Position as a text board (ranks 8-1, pieces, side-to-move, castling rights, EP square).
template <typename PieceC, typename> std::ostream &operator<<(std::ostream &os, const _Position<PieceC, void> &pos) {
    // RAII guard to save/restore stream state
    struct ios_guard {
        std::ostream &strm;
        std::ios::fmtflags flags;
        std::streamsize prec;
        std::ostream::char_type fill;
        ios_guard(std::ostream &s) : strm(s), flags(s.flags()), prec(s.precision()), fill(s.fill()) {}
        ~ios_guard() {
            strm.flags(flags);
            strm.precision(prec);
            strm.fill(fill);
        }
    } guard(os);

    os << "\n +---+---+---+---+---+---+---+---+\n";

    for (Rank r = RANK_8; r >= RANK_1; --r) {
        for (File f = FILE_A; f <= FILE_H; ++f)
            os << " | " << pos.piece_on(make_sq(r, f));

        os << " | " << (1 + r) << "\n +---+---+---+---+---+---+---+---+\n";
    }
    os << "   a   b   c   d   e   f   g   h\n";

    // Ensure key is printed in hex, but restores after this function
    os << "\nFen: " << pos.fen() << "\nKey: " << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pos.hash()
       << '\n';

    return os;
}
/// @brief Print a Move as UCI string.
std::ostream &operator<<(std::ostream &os, const Move mv) { return os << mv.uci(); }

/// @brief Print a Color as "w" or "b".
std::ostream &operator<<(std::ostream &os, const Color c) {
    DescriptiveNameNotation<Color> colors = {
        {    WHITE,    "WHITE" },
        {    BLACK,    "BLACK" },
        { COLOR_NB, "COLOR_NB" }
    };
    return os << colors[c];
}
/// @brief Print a PieceType as "p", "n", "b", "r", "q", "k".
std::ostream &operator<<(std::ostream &os, const PieceType c) {
    DescriptiveNameNotation<PieceType> pieces = {
        { NO_PIECE_TYPE, "NO_PIECE_TYPE/ALL_PIECES" },
        {          PAWN,                     "PAWN" },
        {        KNIGHT,                   "KNIGHT" },
        {        BISHOP,                   "BISHOP" },
        {          ROOK,                     "ROOK" },
        {         QUEEN,                    "QUEEN" },
        {          KING,                     "KING" },
        { PIECE_TYPE_NB,            "PIECE_TYPE_NB" },
    };
    return os << pieces[c];
}
/// @brief Print CastlingRights as "KkQq" style string.
std::ostream &operator<<(std::ostream &os, const CastlingRights cr) {
    DescriptiveNameNotation<CastlingRights> castlingFlags = {
        {                      NO_CASTLING,                      "NO_CASTLING" },
        {                         WHITE_OO,                         "WHITE_OO" },
        {                        WHITE_OOO,                        "WHITE_OOO" },
        {                         BLACK_OO,                         "BLACK_OO" },
        {                        BLACK_OOO,                        "BLACK_OOO" },
        {                   WHITE_CASTLING,                   "WHITE_CASTLING" },
        {                   BLACK_CASTLING,                   "BLACK_CASTLING" },
        {                        KING_SIDE,                        "KING_SIDE" },
        {                       QUEEN_SIDE,                       "QUEEN_SIDE" },
        {                     ANY_CASTLING,                     "ANY_CASTLING" },
        {             WHITE_OOO | BLACK_OO,             "WHITE_OOO | BLACK_OO" },
        {             WHITE_OO | BLACK_OOO,             "WHITE_OO | BLACK_OOO" },
        {  WHITE_OO | WHITE_OOO | BLACK_OO,  "WHITE_OO | WHITE_OOO | BLACK_OO" },
        { WHITE_OO | WHITE_OOO | BLACK_OOO, "WHITE_OO | WHITE_OOO | BLACK_OOO" },
        {  WHITE_OO | BLACK_OO | BLACK_OOO,  "WHITE_OO | BLACK_OO | BLACK_OOO" },
        { WHITE_OOO | BLACK_OO | BLACK_OOO, "WHITE_OOO | BLACK_OO | BLACK_OOO" },
    };

    return os << castlingFlags[cr];
}
static std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}
/// @brief Print a Square as algebraic notation (e.g. "e2").
std::ostream &operator<<(std::ostream &os, const Square sq) { return os << uci::squareToString(sq); }

/// @brief Print a piece (color + type), e.g. "wP", "bK".
template <typename PieceC, typename> std::ostream &operator<<(std::ostream &os, PieceC p) {
    char c = '.';
    switch (p) {
    case PieceC::WPAWN:
        c = 'P';
        break;
    case PieceC::BPAWN:
        c = 'p';
        break;
    case PieceC::WKNIGHT:
        c = 'N';
        break;
    case PieceC::BKNIGHT:
        c = 'n';
        break;
    case PieceC::WBISHOP:
        c = 'B';
        break;
    case PieceC::BBISHOP:
        c = 'b';
        break;
    case PieceC::WROOK:
        c = 'R';
        break;
    case PieceC::BROOK:
        c = 'r';
        break;
    case PieceC::WQUEEN:
        c = 'Q';
        break;
    case PieceC::BQUEEN:
        c = 'q';
        break;
    case PieceC::WKING:
        c = 'K';
        break;
    case PieceC::BKING:
        c = 'k';
        break;
    default:
        break;
    }
    return os << c;
}

#define INSTANTITATE(PieceC)                                                                                                   \
    template std::ostream &operator<<(std::ostream &, const _Position<PieceC, void> &);                                        \
    template std::ostream &operator<<(std::ostream &, PieceC);

INSTANTITATE(EnginePiece)
INSTANTITATE(PolyglotPiece)
INSTANTITATE(ContiguousMappingPiece)
} // namespace chess