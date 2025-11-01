#include "moves_io.h"
#include "types.h"
#include <iostream>
namespace chess {
namespace uci {
std::string squareToString(Square sq) {
    constexpr std::string_view fileChars[64] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2", "d2", "e2",
        "f2", "g2", "h2", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a4", "b4",
        "c4", "d4", "e4", "f4", "g4", "h4", "a5", "b5", "c5", "d5", "e5", "f5", "g5",
        "h5", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "a7", "b7", "c7", "d7",
        "e7", "f7", "g7", "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
    };
    return std::string{ fileChars[sq] };
}
std::string moveToUci(const Move &mv) {
    if (!mv.is_ok()) {
        // null move
        static const std::string nullMove = "0000";
        return nullMove;
    }
    constexpr char PieceTypeChar[] = " pnbrqk";
    static thread_local std::string move;
    move.clear();
    // Source square
    move += squareToString(mv.from_sq());
    // To square, special: castlings
    switch (mv.type_of()) {
    case CASTLING:
        switch (mv.to_sq()) {
        case SQ_H1:
            move += "g1"; // White kingside castling
            break;
        case SQ_A1:
            move += "c1"; // white queenside castling
            break;
        case SQ_H8:
            move += "g8"; // black kingside castling
            break;
        case SQ_A8:
            move += "c8"; // black queenside castling
            break;
        default:
            UNREACHABLE();
            // assert(false && "weirdly.... i didn't implement Chess960!");
        }
        break;
    case PROMOTION:
        move += squareToString(mv.to_sq());
        move += PieceTypeChar[mv.promotion_type()];
        break;
    default:
        move += squareToString(mv.to_sq());
        break;
    }
    return move;
}
} // namespace uci
std::ostream &operator<<(std::ostream &os, Move mv) {
    os << uci::moveToUci(mv);
    return os;
}
} // namespace chess