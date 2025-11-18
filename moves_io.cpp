#include "moves_io.h"
#include "position.h"
#include "types.h"
#include <algorithm>
#include <iostream>
#include <string_view>
namespace chess {
namespace uci {
std::string squareToString(Square sq) {
    constexpr std::string_view fileChars[65] = { "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "none" };
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
#if defined(_DEBUG) || !defined(NDEBUG)
            assert(false && "this isn't chess960");
#elif defined(__EXCEPTIONS)
            throw std::invalid_argument("this isn't chess960");
#else
            break;
#endif
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
template <typename T, typename V> Move uciToMove(const _Position<T, V> &pos, std::string uci) { return uciToMove(pos, std::string_view(uci)); }
template <typename T, typename V> Move uciToMove(const _Position<T, V> &pos, std::string_view uci) {
    if (uci.length() < 4) {
#ifdef __EXCEPTIONS
        throw std::invalid_argument("example: a2a4");
#endif
        return Move::NO_MOVE;
    }

    Square source = parse_square(uci.substr(0, 2));
    Square target = parse_square(uci.substr(2, 2));

    if (!is_valid(source) || !is_valid(target)) {
#ifdef __EXCEPTIONS
        throw std::invalid_argument("source: [a1, h8], target: [a1, h8]");
#endif
        return Move::NO_MOVE;
    }
    auto pt = piece_of(pos.at(source));
    if (pt == NO_PIECE_TYPE) {
#ifdef __EXCEPTIONS
        throw std::invalid_argument("source need to be a existing piece, got nothing");
#endif
    }
    if (pt == KING && square_distance(target, source) == 2) {
        target = make_sq(target > source ? File::FILE_H : File::FILE_A, rank_of(source));
        return Move::make<CASTLING>(source, target);
    }
    // en passant
    if (pt == PAWN && target == pos.enpassantSq()) {
        return Move::make<EN_PASSANT>(source, target);
    }

    // promotion
    if (pt == PAWN && uci.length() == 5 && (rank_of(target) == (pos.sideToMove() == WHITE ? RANK_8 : RANK_1))) {
        auto promotion = parse_pt(uci[4]);

        if (promotion == NO_PIECE_TYPE || promotion == KING || promotion == PAWN) {
#if defined(_DEBUG) || !defined(NDEBUG)
            assert(false && "promotions: NRBQ");
#elif defined(__EXCEPTIONS) && (defined(_DEBUG) || !defined(NDEBUG))
            throw std::invalid_argument("promotions: NRBQ");
#endif
            return Move::NO_MOVE;
        }

        return Move::make<PROMOTION>(source, target, promotion);
    }
    auto move = (uci.length() == 4) ? Move::make(source, target) : Move::NO_MOVE;
    Movelist moves;
    pos.legals(moves);
    auto it = std::find(moves.begin(), moves.end(), move);
#if defined(_DEBUG) || !defined(NDEBUG)
    assert(it != moves.end() && "Move is illegal");
#elif defined(__EXCEPTIONS)
    if (it == moves.end())
        throw std::invalid_argument("Move is illegal");
#endif
    return move;
}
#define INSTANTITATE(PieceC)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    template Move uciToMove<PieceC, void>(const _Position<PieceC, void> &, std::string_view);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    template Move uciToMove<PieceC, void>(const _Position<PieceC, void> &, std::string);
INSTANTITATE(PolyglotPiece)
INSTANTITATE(EnginePiece)
INSTANTITATE(ContiguousMappingPiece)
#undef INSTANTITATE
} // namespace uci
std::string Move::uci() const { return uci::moveToUci(*this); }
} // namespace chess
