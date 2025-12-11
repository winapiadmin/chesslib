#include "moves_io.h"
#include "position.h"
#include "types.h"
#include <algorithm>
#include <iostream>
#include <regex>
#include <string_view>
#if defined(__EXCEPTIONS) && defined(_DEBUG)
#define THROW_IF_EXCEPTIONS_ON(stuff) throw stuff
#else
#define THROW_IF_EXCEPTIONS_ON(stuff) ((void)0)
#endif
namespace chess {
namespace uci {
std::string squareToString(Square sq) {
    constexpr std::string_view fileChars[65] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2", "d2", "e2", "f2",  "g2", "h2", "a3",
        "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4",  "h4", "a5", "b5",
        "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",  "a7", "b7", "c7",
        "d7", "e7", "f7", "g7", "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "none"
    };
    return std::string{ fileChars[sq] };
}
std::string moveToUci(Move mv) {
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
#else
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("this isn't chess960"));
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
template <typename T, typename V> Move uciToMove(const _Position<T, V> &pos, std::string_view uci) {
    if (uci.length() < 4) {
        THROW_IF_EXCEPTIONS_ON(IllegalMoveException("example: a2a4 or d7d8q"));
        return Move::NO_MOVE;
    }

    Square source = parse_square(uci.substr(0, 2));
    Square target = parse_square(uci.substr(2, 2));

    if (!is_valid(source) || !is_valid(target)) {
        THROW_IF_EXCEPTIONS_ON(IllegalMoveException("source !in [a1, h8], target !in [a1, h8]"));
        return Move::NO_MOVE;
    }
    auto pt = piece_of(pos.at(source));
    if (pt == NO_PIECE_TYPE) {
        THROW_IF_EXCEPTIONS_ON(IllegalMoveException("source need to be a existing piece, got nothing"));
        return Move::NO_MOVE;
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
#else
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("promotions: NRBQ"));
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
#else
    if (it == moves.end())
        THROW_IF_EXCEPTIONS_ON(IllegalMoveException("Move is illegal"));
#endif
    return move;
}
template <typename T, typename P> Move parseSan(const _Position<T, P> &pos, std::string_view raw_san, bool remove_illegals) {
    auto do_parse = [&](std::string_view input_san) -> Move {
        if (input_san.empty())
            return Move::none();
        Movelist moves;
        pos.legals(moves);

        // Make a local mutable copy we can trim safely.
        std::string san(input_san), _san(raw_san);

        // 1) Castling shortcuts
        if (san == "O-O" || san == "0-0" || san == "O-O+" || san == "0-0+" || san == "O-O#" || san == "0-0#") {
            Move km = chess::Move::make<CASTLING>(relative_square(pos.side_to_move(), SQ_E1),
                                                  relative_square(pos.side_to_move(), SQ_H1));
            if (std::find(moves.begin(), moves.end(), km) != moves.end())
                return km;
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+san+"' in "+pos.fen()));
            return Move::none();
        }
        if (san == "O-O-O" || san == "0-0-0" || san == "O-O-O+" || san == "0-0-0+" || san == "O-O-O#" || san == "0-0-0#") {
            Move qm = chess::Move::make<CASTLING>(relative_square(pos.side_to_move(), SQ_E1),
                                                  relative_square(pos.side_to_move(), SQ_A1));
            if (std::find(moves.begin(), moves.end(), qm) != moves.end())
                return qm;
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+san+"' in "+pos.fen()));
            return Move::none();
        }
        // 2) Strip trailing annotations (+, #) that aren't required in the standard (except "e.p. "). Repeated occurrences too.
        while (!san.empty()) {
            char c = san.back();
            if (c == '+' || c == '#')
                san.pop_back();
            else
                break;
        }
        if (san.empty()) {
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+san+"' in "+pos.fen()));
            return Move::none();
        }

        // 3) Extract promotion if present (e.g. c8=Q or c8Q)
        PieceType promotion = NO_PIECE_TYPE;
        if (san.size() >= 3) {
            // look for "=Q" or similar at the very end, or single letter promotion (historical)
            char penult = san[san.size() - 2];
            char last = san.back();
            if (penult == '=') {
                promotion = parse_pt(last);
                if (promotion == NO_PIECE_TYPE) {
                    THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+_san+"' in "+pos.fen()));
                    return Move::none();
                }
                san.pop_back(); // remove piece letter
                san.pop_back(); // remove '='
            } else if ((last == 'Q' || last == 'R' || last == 'B' || last == 'N' || last == 'q' || last == 'r' || last == 'b' ||
                        last == 'n')) {
                // allow c8Q or c8q as shorthand (optional)
                promotion = parse_pt(last);
                san.pop_back();
            }
        }

        // 4) Destination square: always the last [file][rank]
        if (san.size() < 2)
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+_san+"' in "+pos.fen()));
        char dfile = san[san.size() - 2];
        char drank = san[san.size() - 1];
        if (!(dfile >= 'a' && dfile <= 'h' && drank >= '1' && drank <= '8')) {
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+_san+"' in "+pos.fen()));
            return Move::none();
        }
        std::string dest_sq_str = san.substr(san.size() - 2, 2);
        Square to_square = parse_square(dest_sq_str);
        if (to_square == SQ_NONE) {
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+_san+"' in "+pos.fen()));
            return Move::none();
        }
        san.resize(san.size() - 2); // chop off destination

        // 5) Now san contains everything before the dest:
        //    possible piece letter, possible source square, possible disambiguation,
        //    optional 'x' capture markers (we will ignore 'x').
        // Remove all 'x' characters (capture indicators) from the remainder
        std::string prefix;
        prefix.reserve(san.size());
        for (char c : san)
            if (c != 'x' && c != 'X')
                prefix.push_back(c);
        // prefix now holds the pre-destination token (e.g. "Nbd" from "Nbd2" or "Pe2" from "Pe2e4")

        // 6) Detect a fully specified source square at the end of prefix (LAN)
        bool has_src_square = false;
        Square src_square = SQ_NONE;
        if (prefix.size() >= 2) {
            char sfile = prefix[prefix.size() - 2];
            char srank = prefix[prefix.size() - 1];
            if (sfile >= 'a' && sfile <= 'h' && srank >= '1' && srank <= '8') {
                // consume it
                std::string src_sq_str = prefix.substr(prefix.size() - 2, 2);
                src_square = parse_square(src_sq_str);
                if (src_square == SQ_NONE)
                    THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+_san+"' in "+pos.fen()));
                prefix.resize(prefix.size() - 2);
                has_src_square = true;
            }
        }

        // 7) Detect piece letter at front if present
        PieceType piece_type = NO_PIECE_TYPE;
        if (!prefix.empty()) {
            char front = prefix.front();
            PieceType pt = parse_pt(front);
            if (pt != NO_PIECE_TYPE) {
                piece_type = pt;
                // remove leading piece letter
                prefix.erase(prefix.begin());
            }
        }
        // If no explicit piece letter, it's a pawn move
        if (piece_type == NO_PIECE_TYPE)
            piece_type = PAWN;

        // 8) The remaining prefix is disambiguation: can be file, rank, or file+rank (rare)
        int dis_file = -1; // 0..7 or -1
        int dis_rank = -1; // 0..7 or -1
        for (char c : prefix) {
            if (c >= 'a' && c <= 'h')
                dis_file = c - 'a';
            else if (c >= '1' && c <= '8')
                dis_rank = c - '1';
            else {
                // unexpected char in prefix
                THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '"+_san+"' in "+pos.fen()));
                return Move::none();
            }
        }

        // 9) Build candidate filter and scan legal moves
        Move matched = Move::null();
        bool found = false;
        Bitboard to_mask = (1ULL << to_square) & ~pos.occ(pos.side_to_move()); // mask excluding own pieces on destination

        // If pawn and no disambiguation file, restrict pawns to dest file (avoid ambiguous pawn non-file forms)
        // This matches python-chess behavior described earlier.
        Bitboard from_mask = ~0ULL;
        if (piece_type == PAWN) {
            from_mask &= pos.pieces(PAWN, pos.side_to_move());
            if (dis_file == -1 && !has_src_square) {
                // restrict to same file as destination (non-capture pawns must be on same file)
                int dest_file = file_of(to_square);
                from_mask &= attacks::MASK_FILE[dest_file];
            }
        } else {
            from_mask &= pos.pieces(piece_type, pos.side_to_move());
        }

        // Additional disambiguation masks:
        if (dis_file != -1)
            from_mask &= attacks::MASK_FILE[dis_file];
        if (dis_rank != -1)
            from_mask &= attacks::MASK_RANK[dis_rank];
        if (has_src_square) {
            // If fully specified source given, narrow to that square only.
            from_mask &= (1ULL << src_square);
        }

        for (Move m : moves) {
            // match destination
            if (m.to_sq() != to_square)
                continue;

            // match promotion
            if (promotion != NO_PIECE_TYPE) {
                if (m.type_of() != PROMOTION || m.promotion_type() != promotion)
                    continue;
            } else {
                // if move is promotion but SAN lacked piece, reject (require explicit promotion)
                if (m.type_of() == PROMOTION)
                    continue;
            }

            // match piece type: check the piece that is on m.from_sq() in pos
            PieceType src_pt = piece_of(pos.piece_on(m.from_sq()));
            if (src_pt != piece_type)
                continue;

            // match from_mask (disambiguation and pawn filtering)
            if (((1ULL << m.from_sq()) & from_mask) == 0)
                continue;

            // Everything matches -> accept candidate
            if (found) {
                THROW_IF_EXCEPTIONS_ON(
                    AmbiguousMoveException("ambiguous san: '" + _san + "' in " + pos.fen()));
                return Move::none();
            }
            matched = m;
            found = true;
        }

        if (!found) {
            THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '" + _san + "' in " + pos.fen()));
            return Move::none();
        }

        return matched;
    };

    if (remove_illegals) {
        std::string trimmed_san(raw_san);
        while (!trimmed_san.empty()) {
            try{
                Move attempt = do_parse(trimmed_san);
                if (attempt.is_ok())
                    return attempt;
            }
            catch(...){}
            trimmed_san.pop_back();
        }
        THROW_IF_EXCEPTIONS_ON(IllegalMoveException("illegal san: '" + std::string(raw_san) + "' in " + pos.fen()));
        return Move::none();
    } else {
        return do_parse(raw_san);
    }
}
template <typename T, typename P> std::string moveToSan(const _Position<T, P> &pos, Move move, bool long_, bool suffix) {
    const char FILE_NAMES[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

    const char RANK_NAMES[] = { '1', '2', '3', '4', '5', '6', '7', '8' };
    constexpr char PieceTypeChar[] = " pnbrqk";
    // Null move. (or none)
    if (!move.is_ok()) {
        return "--";
    }

    std::string san;
    PieceType piece_type = piece_of(pos.at(move.from_sq()));
    bool capture = pos.is_capture(move);
    // Castling.
    if (pos.is_castling(move)) {
        if (file_of(move.to_sq()) < file_of(move.from_sq())) {
            san = "O-O-O";
            goto appendCheck;
        } else {
            san = "O-O";
            goto appendCheck;
        }
    }
    if (piece_type == NO_PIECE_TYPE) {
        THROW_IF_EXCEPTIONS_ON(IllegalMoveException("san() and lan() expect move to be pseudo-legal or null, but got " +
                                                    moveToUci(move) + " in " + pos.fen()));
        return "";
    }

    if (piece_type != PAWN) {
        san = std::toupper(PieceTypeChar[piece_type]);
    }
    if (long_) {
        san += squareToString(move.from_sq());
    } else if (piece_type != PAWN) {
        // Get ambiguous move candidates.
        // Relevant candidates: not exactly the current move,
        // but to the same square.
        Movelist moves;
        pos.legals(moves);
        Bitboard others = 0;
        Bitboard from_mask = pos.pieces(piece_type, pos.side_to_move());
        from_mask &= ~(1ULL << move.from_sq());
        Bitboard to_mask = 1ULL << move.to_sq();
        for (const Move &candidate : moves) {
            Bitboard cand_from_bb = 1ULL << candidate.from_sq();
            // Only consider other pieces of same type that can move to the same destination.
            if ((cand_from_bb & from_mask) && ((1ULL << candidate.to_sq()) & to_mask))
                others |= cand_from_bb;
        }

        // Disambiguate only if there are other candidates that can move to the same square.
        if (others) {
            bool need_file = false, need_rank = false;
            for (Square sq = SQ_A1; sq < SQ_NONE; ++sq) {
                if (others & (1ULL << sq)) {
                    if (file_of(sq) == file_of(move.from_sq()))
                        need_rank = true;
                    if (rank_of(sq) == rank_of(move.from_sq()))
                        need_file = true;
                }
            }
            // If neither shares file nor rank, include file by default.
            if (!need_file && !need_rank)
                need_file = true;
            if (need_file)
                san += FILE_NAMES[file_of(move.from_sq())];
            if (need_rank)
                san += RANK_NAMES[rank_of(move.from_sq())];
        }
    } else if (capture) {
        san += FILE_NAMES[file_of(move.from_sq())];
    }

    // Captures.
    if (capture) {
        san += "x";
    } else if (long_) {
        san += "-";
    }

    // Destination square.
    san += squareToString(move.to_sq());

    // Promotion.
    if (move.type_of() == PROMOTION) {
        san += "=" + std::string(1, std::toupper(PieceTypeChar[move.promotion_type()]));
    }
appendCheck:
    if (!suffix)
        return san;
    _Position<T> p = pos;
    p.doMove(move);
    bool _check = p.is_check();
    Movelist moves;
    p.legals(moves);
    // Checkmate: no legal moves and in check; Stalemate: no legal moves and not in check
    if (moves.size() == 0 && _check)
        san += "#";
    else if (_check)
        san += "+";
    return san;
}
#define INSTANTITATE(PieceC)                                                                                                   \
    template Move uciToMove<PieceC, void>(const _Position<PieceC, void> &, std::string_view);                                  \
    template Move parseSan<PieceC, void>(const _Position<PieceC, void> &, std::string_view, bool);                             \
    template std::string moveToSan<PieceC, void>(const _Position<PieceC, void> &, Move, bool, bool);
INSTANTITATE(PolyglotPiece)
INSTANTITATE(EnginePiece)
INSTANTITATE(ContiguousMappingPiece)
#undef INSTANTITATE
} // namespace uci
std::string Move::uci() const { return uci::moveToUci(*this); }
} // namespace chess
