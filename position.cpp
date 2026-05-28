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

/// @file position.cpp
/// @brief Position implementation: doMove, undoMove, setFEN, FEN export, and validation.

#include "position.h"
#include "movegen.h"
#include "moves_io.h"
#include "printers.h"
#include "zobrist.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

#if defined(_CHESSLIB_ERROR_MODE_THROW)
#define INVALID_ARG_IF(c, exception)                                                                                           \
    do {                                                                                                                       \
        if (c)                                                                                                                 \
            throw(exception);                                                                                                  \
    } while (0)
#elif defined(_CHESSLIB_ERROR_MODE_ASSERT)
#define INVALID_ARG_IF(c, exception)                                                                                           \
    do {                                                                                                                       \
        assert(!(c) && #exception);                                                                                            \
    } while (0)
#elif defined(_DEBUG) && !defined(NDEBUG)
#define INVALID_ARG_IF(c, exception)                                                                                           \
    do {                                                                                                                       \
        if (c)                                                                                                                 \
            std::cerr << #c << ", message: " << #exception << " (at " << __FILE__ << ":" << __LINE__ << ")\n";                 \
    } while (0)
#else
#define INVALID_ARG_IF(c, exception)                                                                                           \
    do {                                                                                                                       \
        (void)(c);                                                                                                             \
    } while (0)
#endif
namespace chess {

namespace {

/// @brief Precomputed mask for the pawn that would deliver an en-passant capture.
/// @details ep_pawn_mask[sq] has the capturing-pawn square set for the given EP target square.
constexpr Bitboard ep_pawn_mask_for(Square sq) {
    const Rank r = rank_of(sq);
    if (r != RANK_3 && r != RANK_6)
        return 0;

    Bitboard m = 1ULL << static_cast<int>(sq);
    if (r == RANK_6)
        m >>= 8; // WHITE stm → shift down to rank 5
    else
        m <<= 8; // BLACK stm → shift up to rank 4
    return ((m << 1) & ~attacks::MASK_FILE[0]) | ((m >> 1) & ~attacks::MASK_FILE[7]);
}

constexpr std::array<Bitboard, 64> make_ep_pawn_masks() {
    std::array<Bitboard, 64> table{};
    for (int i = 0; i < 64; ++i)
        table[i] = ep_pawn_mask_for(static_cast<Square>(i));
    return table;
}

constexpr auto ep_pawn_masks = make_ep_pawn_masks();

} // namespace

/// @brief Apply a move to the board, updating all internal state.
/// @tparam Strict If true, asserts/checks for invalid moves.
/// @param move The move to execute.
template <typename PieceC, typename T> template <bool Strict> void _Position<PieceC, T>::doMove(const Move &move) {
    INVALID_ARG_IF(!move.is_ok(), uci::IllegalMoveException("doMove called with invalid move"));
    Square from_sq = move.from_sq(), to_sq = move.to_sq();
    Color us = side_to_move(), them = ~us;
    MoveType move_type = move.type_of();
    PieceC moving_piece = piece_on(from_sq);
    PieceC target_piece = piece_on(to_sq);
    PieceType moving_piecetype = piece_of(moving_piece);
    PieceType target_piecetype = piece_of(target_piece);
    Color target_color = color_of(target_piece);
    bool is_capt = move.type_of() == EN_PASSANT || (move.type_of() != CASTLING && target_piece != PieceC::NO_PIECE);
    history.push_back(state());
    state().saved_rook_pin = _rook_pin;
    state().saved_bishop_pin = _bishop_pin;
    state().saved_checkers = _checkers;
    state().saved_check_mask = _check_mask;
    state().incr_sqs[0] = state().incr_sqs[1] = state().incr_sqs[2] = state().incr_sqs[3] = SQ_NONE;
    state().incr_pc[0] = state().incr_pc[1] = state().incr_pc[2] = state().incr_pc[3] = PieceC::NO_PIECE;
    state().mv = move; // Update the move in the current state
#if defined(_DEBUG) || !defined(NDEBUG)
    if (target_piecetype == KING) {
        std::cerr << *this << '\n';
        std::cerr << "Move: " << move << "\n";
        std::cerr << "From: " << from_sq << " To: " << to_sq << "\n";
        std::cerr << "Target piece: " << target_piece << "\n";
        std::cerr << "Side to move: " << us << "\n";
        std::cerr << "movetrace: \n";
        for (int i = 0; i < history.size(); i++)
            std::cerr << uci::moveToUci(history[i].mv, _chess960) << '\n';
    }
#endif
    INVALID_ARG_IF(target_piecetype == KING, uci::IllegalMoveException("Capturing kings is illegal"));
    INVALID_ARG_IF(moving_piecetype == NO_PIECE_TYPE, uci::IllegalMoveException("Expected a piece to move."));
    {
        switch (move_type) {
        case NORMAL:
            removePiece(target_piecetype, to_sq, target_color);
            removePiece(moving_piecetype, from_sq, us);
            placePiece(moving_piecetype, to_sq, us);
            state().incr_sqs[0] = from_sq;
            state().incr_pc[0] = moving_piece;
            state().incr_sqs[1] = to_sq;
            state().incr_pc[1] = target_piece;
            break;
        case PROMOTION:
            removePiece(target_piecetype, to_sq, target_color);
            removePiece(moving_piecetype, from_sq, us);
            placePiece(move.promotion_type(), to_sq, us);
            state().incr_sqs[0] = from_sq;
            state().incr_pc[0] = moving_piece;
            state().incr_sqs[1] = to_sq;
            state().incr_pc[1] = target_piece;
            break;
        case EN_PASSANT: {
            Square ep_capture_sq = to_sq + pawn_push(them);
            removePiece(moving_piecetype, from_sq, us);
            removePiece<PAWN>(ep_capture_sq, them);
            placePiece<PAWN>(to_sq, us);
            state().incr_sqs[0] = from_sq;
            state().incr_pc[0] = moving_piece;
            state().incr_sqs[1] = ep_capture_sq;
            state().incr_pc[1] = make_piece<PieceC>(PAWN, them);
            state().incr_sqs[2] = to_sq;
            state().incr_pc[2] = PieceC::NO_PIECE;
            break;
        }
        case CASTLING: {
            const bool is_king_side = file_of(from_sq) < file_of(to_sq);
            const Square rook_dest = castling_rook_square(us, is_king_side);
            const Square king_dest = castling_king_square(us, is_king_side);

            const PieceC prev_king_dest = piece_on(king_dest);
            const PieceC prev_rook_dest = piece_on(rook_dest);

            state().incr_sqs[0] = from_sq, state().incr_pc[0] = moving_piece;
            state().incr_sqs[1] = to_sq, state().incr_pc[1] = target_piece;
            state().incr_sqs[2] = king_dest, state().incr_pc[2] = prev_king_dest;
            state().incr_sqs[3] = rook_dest, state().incr_pc[3] = prev_rook_dest;

            Square rook_start = is_king_side ? castling_meta_[us].rook_start_ks : castling_meta_[us].rook_start_qs;

            removePiece<KING>(from_sq, us);
            removePiece<ROOK>(rook_start, us);

            placePiece<KING>(king_dest, us);
            placePiece<ROOK>(rook_dest, us);
            break;
        }
        }
    }
    {
        // Detect a two-square pawn push.
        bool isDoublePush = (moving_piecetype == PAWN) && (abs(to_sq - from_sq) == 16);

        // Remove old EP key if it was actually included.
        bool _ir_1 = state().enPassant != SQ_NONE && state().epIncluded;
        state().hash ^= _ir_1 ? zobrist::RandomEP[file_of(state().enPassant)] : 0;

        // Default: no EP square and not included.
        state().epIncluded = false;

        // If the move creates a potential EP target:
        state().enPassant = isDoublePush ? (from_sq + pawn_push(us)) : SQ_NONE;
        if (isDoublePush) {
            // Now side to move is the *opponent*.
            File f = file_of(state().enPassant);
            state().epIncluded = (ep_pawn_masks[state().enPassant] & pieces<PAWN>(~us)) != 0;
            state().hash ^= state().epIncluded ? zobrist::RandomEP[f] : 0;
        }
    }
    {
        CastlingRights clear_mask = NO_CASTLING;
        // Moving piece
        if (moving_piecetype == KING && from_sq == castling_meta_[us].king_start) {
            clear_mask |= (us == WHITE ? WHITE_CASTLING : BLACK_CASTLING);
        } else if (moving_piecetype == ROOK) {
            if (from_sq == castling_meta_[us].rook_start_ks) {
                clear_mask |= (us == WHITE ? WHITE_OO : BLACK_OO);
            } else if (from_sq == castling_meta_[us].rook_start_qs) {
                clear_mask |= (us == WHITE ? WHITE_OOO : BLACK_OOO);
            }
        }
        // Captured piece
        if (target_piecetype == ROOK) {
            if (to_sq == castling_meta_[target_color].rook_start_ks)
                clear_mask |= (target_color == WHITE ? WHITE_OO : BLACK_OO);
            else if (to_sq == castling_meta_[target_color].rook_start_qs)
                clear_mask |= (target_color == WHITE ? WHITE_OOO : BLACK_OOO);
        }
        if (clear_mask) {
            CastlingRights prev = state().castlingRights;
            state().castlingRights &= ~clear_mask;
            state().hash ^= zobrist::RandomCastle[prev] ^ zobrist::RandomCastle[state().castlingRights];
        }
    }
    state().turn = ~state().turn;
    // Update halfmoves, fullmoves and stm
    state().fullMoveNumber += (state().turn == WHITE);
    state().halfMoveClock = (is_capt || moving_piecetype == PAWN) ? 0 : (state().halfMoveClock + 1);
    state().pliesFromNull++;
    state().hash ^= zobrist::RandomTurn;
    rep_hashes_.push_back(state().hash);
    refresh_attacks();
    // DO NOT MIX REPETITIONS
    if constexpr (Strict) {
        state().repetition = 0;
        int end = std::min(rule50_count(), state().pliesFromNull);
        if (end >= 4) {
            Key cur_hash = hash();
            for (int i = 4; i <= end; i += 2) {
                if (rep_hashes_[rep_hashes_.size() - 1 - i] == cur_hash)
                    state().repetition++;
            }
        }
    }
}

/// @brief Set the position from a FEN string.
/// @param str FEN string.
/// @param chess960 Whether to interpret castling notation as Chess960.
/// @param mode FEN parsing strictness mode.
template <typename PieceC, typename T>
bool _Position<PieceC, T>::setFEN(const std::string &str, bool chess960, FENParsingMode mode) {
    history.clear();
    rep_hashes_.clear();
    history.push_back(HistoryEntry<PieceC>());
    _chess960 = chess960;
    std::fill(std::begin(pieces_list), std::end(pieces_list), PieceC::NO_PIECE);
    castling_meta_[WHITE] = {};
    castling_meta_[BLACK] = {};
    std::istringstream ss(str);
    std::string board_fen, active_color, castling, enpassant;
    int halfmove = 0, fullmove = 1;
    if (!(ss >> board_fen >> active_color >> castling >> enpassant >> halfmove >> fullmove)) {
        INVALID_ARG_IF(true, std::runtime_error("Invalid FEN format"));
        return false;
    }
    std::string extra;
    if (ss >> extra) {
        INVALID_ARG_IF(true, std::runtime_error("Trailing FEN data"));
        return false;
    }
    // 1. Parse board
    {
        File f = FILE_A;
        Rank r = RANK_8;
        int file_count = 0;
        int rank_count = 0;
        bool prev_digit = false;
        for (char c : board_fen) {
            if (c == '/') {
                INVALID_ARG_IF(file_count != 8, std::runtime_error("Each rank must contain exactly 8 squares"));
                if (file_count != 8)
                    return false;
                f = FILE_A;
                INVALID_ARG_IF(r == RANK_1, std::runtime_error("Too many ranks"));
                if (r == RANK_1)
                    return false;
                --r;
                file_count = 0;
                ++rank_count;
                prev_digit = false;
                continue;
            }

            if (c >= '1' && c <= '8') {
                INVALID_ARG_IF(prev_digit, std::runtime_error("Compressed streaks of empty squares required"));
                if (prev_digit)
                    return false;
                int empty_squares = c - '0';
                file_count += empty_squares;
                INVALID_ARG_IF(file_count > 8, std::runtime_error("Too many squares on a rank"));
                if (file_count > 8)
                    return false;
                f = static_cast<File>(static_cast<uint8_t>(f) + empty_squares);
                prev_digit = true;
            } else {
                INVALID_ARG_IF(!chess::is_valid(r, f), std::runtime_error("Invalid file/rank position"));
                if (!chess::is_valid(r, f))
                    return false;
                switch (c) {
                case 'p':
                    placePiece<PAWN>(make_sq(r, f), BLACK);
                    break;
                case 'P':
                    placePiece<PAWN>(make_sq(r, f), WHITE);
                    break;
                case 'n':
                    placePiece<KNIGHT>(make_sq(r, f), BLACK);
                    break;
                case 'N':
                    placePiece<KNIGHT>(make_sq(r, f), WHITE);
                    break;
                case 'b':
                    placePiece<BISHOP>(make_sq(r, f), BLACK);
                    break;
                case 'B':
                    placePiece<BISHOP>(make_sq(r, f), WHITE);
                    break;
                case 'r':
                    placePiece<ROOK>(make_sq(r, f), BLACK);
                    break;
                case 'R':
                    placePiece<ROOK>(make_sq(r, f), WHITE);
                    break;
                case 'q':
                    placePiece<QUEEN>(make_sq(r, f), BLACK);
                    break;
                case 'Q':
                    placePiece<QUEEN>(make_sq(r, f), WHITE);
                    break;
                case 'k':
                    placePiece<KING>(make_sq(r, f), BLACK);
                    break;
                case 'K':
                    placePiece<KING>(make_sq(r, f), WHITE);
                    break;
                default:
                    INVALID_ARG_IF(true, std::runtime_error("Invalid FEN character"));
                    return false;
                }

                ++file_count;
                f = static_cast<File>(static_cast<uint8_t>(f) + 1);
                prev_digit = false;
            }
        }

        INVALID_ARG_IF(file_count != 8, std::runtime_error("Last rank must have 8 squares"));
        INVALID_ARG_IF(rank_count != 7, std::runtime_error("FEN must contain exactly 8 ranks"));
        if (file_count != 8 || rank_count != 7)
            return false;
    }

    // 2. Turn
    if (active_color == "w") {
        state().turn = WHITE;
        state().hash ^= zobrist::RandomTurn;
    } else if (active_color == "b") {
        state().turn = BLACK;
    } else {
        INVALID_ARG_IF(active_color != "w" && active_color != "b",
                       std::runtime_error("Expected white or black, got something else."));
        return false;
    }

    // 3. Castling rights
    state().castlingRights = NO_CASTLING;
    if (castling != "-") {
        bool result_castling = true;
        for (Color color : { WHITE, BLACK }) {
            auto findRookQS = [&](Square king_sq, Color color_) -> Square {
                Rank r = rank_of(king_sq);
                for (int f = file_of(king_sq) - 1; f >= FILE_A; --f) {
                    Square sq = make_sq(r, static_cast<File>(f));
                    PieceC p = pieces_list[sq];
                    if (p != PieceC::NO_PIECE && type_of(p) == ROOK && color_of(p) == color_)
                        return sq;
                }
                return SQ_NONE;
            };

            auto findRookKS = [&](Square king_sq, Color color_) -> Square {
                Rank r = rank_of(king_sq);
                for (int f = file_of(king_sq) + 1; f <= FILE_H; ++f) {
                    Square sq = make_sq(r, static_cast<File>(f));
                    PieceC p = pieces_list[sq];
                    if (p != PieceC::NO_PIECE && type_of(p) == ROOK && color_of(p) == color_)
                        return sq;
                }
                return SQ_NONE;
            };
            bool allow_xfen = (mode == MODE_XFEN || mode == MODE_AUTO);
            bool allow_smk = (mode == MODE_SMK || mode == MODE_AUTO);
            auto apply = [&](char c) {
                Square king_sq = kingSq(color);
                INVALID_ARG_IF(king_sq == SQ_NONE, std::runtime_error("Nonexistient king with inappropriate castling field"));
                if (king_sq == SQ_NONE) {
                    result_castling = false;
                    return;
                }

                Square rook_ks = findRookKS(king_sq, color);
                Square rook_qs = findRookQS(king_sq, color);

                auto setKS = [&](Square rook_sq) {
                    INVALID_ARG_IF(rook_sq == SQ_NONE, std::runtime_error("kingside rook not found"));
                    INVALID_ARG_IF(rank_of(king_sq) != rank_of(rook_sq), std::runtime_error("kingside rook not on same rank"));
                    if (rook_sq == SQ_NONE || (rank_of(king_sq) != rank_of(rook_sq))) {
                        result_castling = false;
                        return;
                    }

                    if (color == WHITE) {
                        state().castlingRights |= WHITE_OO;
                        castling_meta_[WHITE].king_start = king_sq;
                        castling_meta_[WHITE].rook_start_ks = rook_sq;
                    } else {
                        state().castlingRights |= BLACK_OO;
                        castling_meta_[BLACK].king_start = king_sq;
                        castling_meta_[BLACK].rook_start_ks = rook_sq;
                    }
                };

                auto setQS = [&](Square rook_sq) {
                    INVALID_ARG_IF(rook_sq == SQ_NONE, std::runtime_error("queenside rook not found"));
                    INVALID_ARG_IF(rank_of(king_sq) != rank_of(rook_sq), std::runtime_error("queenside rook not on same rank"));
                    if (rook_sq == SQ_NONE || (rank_of(king_sq) != rank_of(rook_sq))) {
                        result_castling = false;
                        return;
                    }

                    if (color == WHITE) {
                        state().castlingRights |= WHITE_OOO;
                        castling_meta_[WHITE].king_start = king_sq;
                        castling_meta_[WHITE].rook_start_qs = rook_sq;
                    } else {
                        state().castlingRights |= BLACK_OOO;
                        castling_meta_[BLACK].king_start = king_sq;
                        castling_meta_[BLACK].rook_start_qs = rook_sq;
                    }
                };

                if (c == 'K' && color == WHITE) {
                    INVALID_ARG_IF(chess960 && !allow_xfen, std::runtime_error("xfen into shredder fen parser"));
                    if (chess960 && !allow_xfen) {
                        result_castling = false;
                        return;
                    }
                    if (rook_ks == SQ_NONE) {
                        result_castling = false;
                        return;
                    }
                    if (rank_of(king_sq) != rank_of(rook_ks)) {
                        result_castling = false;
                        return;
                    }
                    setKS(rook_ks);
                } else if (c == 'Q' && color == WHITE) {
                    INVALID_ARG_IF(chess960 && !allow_xfen, std::runtime_error("xfen into shredder fen parser"));
                    if (chess960 && !allow_xfen) {
                        result_castling = false;
                        return;
                    }
                    if (rook_qs == SQ_NONE) {
                        result_castling = false;
                        return;
                    }
                    if (rank_of(king_sq) != rank_of(rook_qs)) {
                        result_castling = false;
                        return;
                    }
                    setQS(rook_qs);
                } else if (c == 'k' && color == BLACK) {
                    INVALID_ARG_IF(chess960 && !allow_xfen, std::runtime_error("xfen into shredder fen parser"));
                    if (chess960 && !allow_xfen) {
                        result_castling = false;
                        return;
                    }
                    if (rook_ks == SQ_NONE) {
                        result_castling = false;
                        return;
                    }
                    if (rank_of(king_sq) != rank_of(rook_ks)) {
                        result_castling = false;
                        return;
                    }
                    setKS(rook_ks);
                } else if (c == 'q' && color == BLACK) {
                    INVALID_ARG_IF(chess960 && !allow_xfen, std::runtime_error("xfen into shredder fen parser"));
                    INVALID_ARG_IF(rook_qs == SQ_NONE, std::runtime_error("Black queenside rook not exist"));
                    INVALID_ARG_IF(rank_of(king_sq) != rank_of(rook_qs),
                                   std::runtime_error("Black queenside rook not in the same rank as black king"));
                    if (chess960 && !allow_xfen) {
                        result_castling = false;
                        return;
                    }
                    if (rook_qs == SQ_NONE) {
                        result_castling = false;
                        return;
                    }
                    if (rank_of(king_sq) != rank_of(rook_qs)) {
                        result_castling = false;
                        return;
                    }
                    setQS(rook_qs);
                }

                else if (c >= 'A' && c <= 'H' && color == WHITE) {
                    INVALID_ARG_IF(chess960 && !allow_smk, std::runtime_error("shredder fen into xfen parser"));
                    if (chess960 && !allow_smk) {
                        result_castling = false;
                        return;
                    }
                    File f = static_cast<File>(c - 'A');
                    Square rook_sq = make_sq(RANK_1, f);

                    PieceC p = pieces_list[rook_sq];
                    INVALID_ARG_IF(p == PieceC::NO_PIECE || type_of(p) != ROOK || color_of(p) != WHITE,
                                   "Invalid white Chess960 rook");
                    if (p == PieceC::NO_PIECE || type_of(p) != ROOK || color_of(p) != WHITE) {
                        result_castling = false;
                        return;
                    }

                    (f > file_of(king_sq)) ? setKS(rook_sq) : setQS(rook_sq);
                } else if (c >= 'a' && c <= 'h' && color == BLACK) {
                    INVALID_ARG_IF(chess960 && !allow_smk, std::runtime_error("shredder fen into xfen parser"));
                    if (chess960 && !allow_smk) {
                        result_castling = false;
                        return;
                    }
                    File f = static_cast<File>(c - 'a');
                    Square rook_sq = make_sq(RANK_8, f);

                    PieceC p = pieces_list[rook_sq];
                    INVALID_ARG_IF(p == PieceC::NO_PIECE || type_of(p) != ROOK || color_of(p) != BLACK,
                                   "Invalid black Chess960 rook");
                    if (p == PieceC::NO_PIECE || type_of(p) != ROOK || color_of(p) != BLACK) {
                        result_castling = false;
                        return;
                    }

                    (f > file_of(king_sq)) ? setKS(rook_sq) : setQS(rook_sq);
                }

                // ignore '-'
            };

            for (char x : castling)
                apply(x);
            if (!result_castling)
                return false;
        }
    }
    state().hash ^= zobrist::RandomCastle[state().castlingRights];

    for (Color c : { WHITE, BLACK }) {
        // king
        if (castlingRights() & (c & KING_SIDE)) {
            const auto king_from = castling_meta_[c].king_start;
            const auto rook_from = make_sq(file_of(castling_meta_[c].rook_start_ks), rank_of(king_from));
            const auto king_to = castling_king_square(c, true);
            const auto rook_to = castling_rook_square(c, true);
            castling_meta_[c].castling_paths[true] =
                (movegen::between(rook_from, rook_to) | movegen::between(king_from, king_to)) &
                ~((1ULL << king_from) | (1ULL << rook_from));
        }
        // queen
        if (castlingRights() & (c & QUEEN_SIDE)) {
            const auto king_from = castling_meta_[c].king_start;
            const auto rook_from = make_sq(file_of(castling_meta_[c].rook_start_qs), rank_of(king_from));
            const auto king_to = castling_king_square(c, false);
            const auto rook_to = castling_rook_square(c, false);
            castling_meta_[c].castling_paths[false] =
                (movegen::between(rook_from, rook_to) | movegen::between(king_from, king_to)) &
                ~((1ULL << king_from) | (1ULL << rook_from));
        }
    }

    if (enpassant != "-" && enpassant.length() == 2 && enpassant[0] >= 'a' && enpassant[0] <= 'h' && enpassant[1] >= '1' &&
        enpassant[1] <= '8') {
        File f = static_cast<File>(enpassant[0] - 'a');
        Rank r = static_cast<Rank>(enpassant[1] - '1');
        Square ep_sq = make_sq(r, f);
        state().enPassant = ep_sq;
        if (ep_pawn_masks[ep_sq] & pieces<PAWN>(side_to_move())) {
            state().hash ^= zobrist::RandomEP[f];
            state().epIncluded = true;
        }
    } else {
        INVALID_ARG_IF(enpassant != "-", std::runtime_error("Invalid en passant FEN field"));
        if (enpassant != "-")
            return false;
    }

    // 5. Halfmove clock
    INVALID_ARG_IF(halfmove < 0 || fullmove < 1, std::runtime_error("Invalid halfmove and/or fullmove counters"));
    if (halfmove < 0 || fullmove < 1)
        return false;
    state().halfMoveClock = static_cast<uint8_t>(halfmove);

    // 6. Fullmove number
    state().fullMoveNumber = fullmove;
    rep_hashes_.push_back(state().hash);
    if (popcount(pieces(WHITE, KING)) == 1 && popcount(pieces(BLACK, KING)) == 1) {
        refresh_attacks();
    } else {
        _rook_pin = _bishop_pin = _checkers = _check_mask = 0;
        _pin_mask = 0;
    }
    return true;
}

/// @brief Export the position as a FEN string.
/// @param xfen If true, use X-FEN castling notation (supports Chess960).
template <typename PieceC, typename T> std::string _Position<PieceC, T>::fen(bool xfen) const {
    std::ostringstream ss;

    // 1) Piece placement
    for (Rank rank = RANK_8; rank >= RANK_1; --rank) {
        int emptyCount = 0;
        for (File file = FILE_A; file <= FILE_H; ++file) {
            Square sq = make_sq(rank, file); // Assuming 0..63 indexing

            PieceC piece = piece_on(sq);
            if (piece == PieceC::NO_PIECE) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    ss << emptyCount;
                    emptyCount = 0;
                }
                ss << piece;
            }
        }
        if (emptyCount > 0)
            ss << emptyCount;
        if (rank > 0)
            ss << '/';
    }

    // 2) Side to move
    ss << ' ' << (side_to_move() == WHITE ? 'w' : 'b');

    // 3) Castling availability
    ss << ' ';
    std::string castlingStr;
    if (chess960()) {
        if (castlingRights() & WHITE_OO)
            castlingStr += (xfen && castling_meta_[WHITE].rook_start_ks == SQ_H1)
                               ? 'K'
                               : static_cast<char>('A' + file_of(castling_meta_[WHITE].rook_start_ks));
        if (castlingRights() & WHITE_OOO)
            castlingStr += (xfen && castling_meta_[WHITE].rook_start_qs == SQ_A1)
                               ? 'Q'
                               : static_cast<char>('A' + file_of(castling_meta_[WHITE].rook_start_qs));
        if (castlingRights() & BLACK_OO)
            castlingStr += (xfen && castling_meta_[BLACK].rook_start_ks == SQ_H8)
                               ? 'k'
                               : static_cast<char>('a' + file_of(castling_meta_[BLACK].rook_start_ks));
        if (castlingRights() & BLACK_OOO)
            castlingStr += (xfen && castling_meta_[BLACK].rook_start_qs == SQ_A8)
                               ? 'q'
                               : static_cast<char>('a' + file_of(castling_meta_[BLACK].rook_start_qs));
    } else {
        if (castlingRights() & WHITE_OO)
            castlingStr += 'K';
        if (castlingRights() & WHITE_OOO)
            castlingStr += 'Q';
        if (castlingRights() & BLACK_OO)
            castlingStr += 'k';
        if (castlingRights() & BLACK_OOO)
            castlingStr += 'q';
    }
    ss << (castlingStr.empty() ? "-" : castlingStr);

    // 4) En passant target square or '-'
    ss << ' ';
    Square ep = ep_square();
    ss << (ep == SQ_NONE ? "-" : uci::squareToString(ep));

    // 5) Halfmove clock
    ss << ' ' << (int)rule50_count();

    // 6) Fullmove number
    ss << ' ' << (int)fullmove_number();

    return ss.str();
}
/// @brief Validate the current position for internal consistency.
/// @tparam Strict If true, also check for non-pawn material and legal castling rights.
template <typename PieceC, typename T> template <bool Strict> bool _Position<PieceC, T>::is_valid() const {
    if (count<KING, WHITE>() != 1)
        return false;
    if (count<KING, BLACK>() != 1)
        return false;
    Color stm = side_to_move();
    // stm checking
    bool whiteInCheck = is_attacked(king_sq(WHITE), BLACK);
    bool blackInCheck = is_attacked(king_sq(BLACK), WHITE);

    // Both kings cannot be in check simultaneously
    if (whiteInCheck && blackInCheck)
        return false;

    // The side to move cannot have its king currently in check from itself (nonsense)
    if (is_attacked(king_sq(~stm), stm))
        return false;
    if (castlingRights() & WHITE_OOO) {
        Square qs_rook = castling_meta_[WHITE].rook_start_qs;
        if (qs_rook == SQ_NONE || piece_on(qs_rook) != make_piece<PieceC>(ROOK, WHITE))
            return false;
    }
    if (castlingRights() & WHITE_OO) {
        Square ks_rook = castling_meta_[WHITE].rook_start_ks;
        if (ks_rook == SQ_NONE || piece_on(ks_rook) != make_piece<PieceC>(ROOK, WHITE))
            return false;
    }
    if (castlingRights() & BLACK_OOO) {
        Square qs_rook = castling_meta_[BLACK].rook_start_qs;
        if (qs_rook == SQ_NONE || piece_on(qs_rook) != make_piece<PieceC>(ROOK, BLACK))
            return false;
    }
    if (castlingRights() & BLACK_OO) {
        Square ks_rook = castling_meta_[BLACK].rook_start_ks;
        if (ks_rook == SQ_NONE || piece_on(ks_rook) != make_piece<PieceC>(ROOK, BLACK))
            return false;
    }
    // pawns not on backrank
    if ((pieces(PAWN) & (attacks::MASK_RANK[RANK_1] | attacks::MASK_RANK[RANK_8])) != 0)
        return false;
    if constexpr (Strict) {
        // pawns not > 8
        {
            int pawns = count<PAWN, WHITE>();
            int queens = count<QUEEN, WHITE>();
            int rooks = count<ROOK, WHITE>();
            int bishops = count<BISHOP, WHITE>();
            int knights = count<KNIGHT, WHITE>();

            // Base set: 1 queen, 2 rooks, 2 bishops, 2 knights
            int promotions =
                std::max(0, (queens - 1)) + std::max(0, (rooks - 2)) + std::max(0, (bishops - 2)) + std::max(0, (knights - 2));

            if (pawns + promotions > 8)
                return false; // impossible
        }
        {
            int pawns = count<PAWN, BLACK>();
            int queens = count<QUEEN, BLACK>();
            int rooks = count<ROOK, BLACK>();
            int bishops = count<BISHOP, BLACK>();
            int knights = count<KNIGHT, BLACK>();

            // Base set: 1 queen, 2 rooks, 2 bishops, 2 knights
            int promotions =
                std::max(0, (queens - 1)) + std::max(0, (rooks - 2)) + std::max(0, (bishops - 2)) + std::max(0, (knights - 2));

            if (pawns + promotions > 8)
                return false; // impossible
        }
    }
    if (Square ep_sq = ep_square(); ep_sq != SQ_NONE) {
        if ((stm == WHITE && rank_of(ep_sq) != RANK_6) || (stm == BLACK && rank_of(ep_sq) != RANK_3))
            return false;
        if (piece_at(ep_sq - pawn_push(stm)) != make_piece<PieceC>(PAWN, ~stm))
            return false;

        if (!(attacks::pawn(~stm, ep_sq) & pieces<PAWN>(stm)))
            return false;
    }
    // Too many checkers
    if (popcount(checkers()) > 2)
        return false;
    if (_valid_ep_square() != ep_square())
        return false;
    if (clean_castling_rights() != castlingRights())
        return false;
    return true;
}
/// @brief Determine if a move results in a check to the opponent.
template <typename PieceC, typename T> CheckType _Position<PieceC, T>::givesCheck(Move move) const {
    const static auto getSniper = [](const _Position<PieceC> *p, const Square ksq, Bitboard oc) {
        const auto us_occ = p->us(p->side_to_move());
        const auto bishop = attacks::bishop(ksq, oc) & p->pieces(PieceType::BISHOP, PieceType::QUEEN) & us_occ;
        const auto rook = attacks::rook(ksq, oc) & p->pieces(PieceType::ROOK, PieceType::QUEEN) & us_occ;
        return (bishop | rook);
    };

    assert(color_of(at(move.from())) == side_to_move());

    const Square from = move.from();
    const Square to = move.to();
    const Square ksq = king_sq(~side_to_move());
    const Bitboard toBB = 1ULL << (to);
    const PieceType pt = piece_of(at(from));

    Bitboard fromKing = 0ull;

    if (pt == PAWN) {
        fromKing = attacks::pawn(~side_to_move(), ksq);
    } else if (pt == KNIGHT) {
        fromKing = attacks::knight(ksq);
    } else if (pt == BISHOP) {
        fromKing = attacks::bishop(ksq, occ());
    } else if (pt == ROOK) {
        fromKing = attacks::rook(ksq, occ());
    } else if (pt == QUEEN) {
        fromKing = attacks::queen(ksq, occ());
    }

    if (fromKing & toBB)
        return CheckType::DIRECT_CHECK;

    // Discovery check
    const Bitboard fromBB = 1ULL << (from);
    const Bitboard oc = occ() ^ fromBB;

    if (Bitboard sniper = getSniper(this, ksq, oc)) {
        const auto sq = static_cast<Square>(pop_lsb(sniper));
        return (!(movegen::between(ksq, sq) & toBB) || move.type_of() == Move::CASTLING) ? CheckType::DISCOVERY_CHECK
                                                                                         : CheckType::NO_CHECK;
    }

    switch (move.type_of()) {
    case Move::NORMAL:
        return CheckType::NO_CHECK;

    case Move::PROMOTION: {
        Bitboard attacks = 0ull;

        switch (move.promotion_type()) {
        case KNIGHT:
            attacks = attacks::knight(to);
            break;
        case BISHOP:
            attacks = attacks::bishop(to, oc);
            break;
        case ROOK:
            attacks = attacks::rook(to, oc);
            break;
        case QUEEN:
            attacks = attacks::queen(to, oc);
            break;
        default:
            break;
        }

        return (attacks & pieces(PieceType::KING, ~side_to_move())) ? CheckType::DIRECT_CHECK : CheckType::NO_CHECK;
    }

    case Move::ENPASSANT: {
        Square capSq = make_sq(file_of(to), rank_of(from));
        return (getSniper(this, ksq, (oc ^ (1ULL << capSq)) | toBB)) ? CheckType::DISCOVERY_CHECK : CheckType::NO_CHECK;
    }

    case Move::CASTLING: {
        const Square rookTo = relative_square(side_to_move(), to > from ? SQ_F1 : SQ_D1);
        return (attacks::rook(ksq, occ()) & (1ULL << rookTo)) ? CheckType::DISCOVERY_CHECK : CheckType::NO_CHECK;
    }
    }

    assert(false);
    return CheckType::NO_CHECK; // Prevent a compiler warning
}
/// @brief Recompute cached attack info (checkers, check mask, pins).
/// @details Called after setFEN and undoMove (when not restoring from history).
template <typename PieceC, typename T> void _Position<PieceC, T>::refresh_attacks() {
    const Color c = side_to_move();

    Square king_square = king_sq(c);
    pinMasks(c, king_square, _rook_pin, _bishop_pin);
    _pin_mask = _rook_pin | _bishop_pin;

    _checkers = attackers(~c, king_square);

    switch (popcount(_checkers)) {
    case 0:
        _check_mask = ~0ULL; // no checks, full mask
        break;

    case 1: {
        auto sq = static_cast<Square>(lsb(_checkers));
        _check_mask = 1ULL << sq | movegen::between(king_square, sq);
        break;
    }

    default:
        _check_mask = 0ULL; // multiple checks, no blocking mask
        break;
    }
}
/// @brief Compute Zobrist hash for the current position.
template <typename PieceC, typename T> uint64_t _Position<PieceC, T>::zobrist() const {
    uint64_t hash = 0;
    for (int sq = 0; sq < 64; ++sq) {
        auto p = piece_on(static_cast<Square>(sq));
        hash ^= (p == PieceC::NO_PIECE) ? 0 : zobrist::RandomPiece[enum_idx<PieceC>()][static_cast<int>(p)][sq];
    }
    hash ^= (state().turn == WHITE) ? zobrist::RandomTurn : 0;
    hash ^= zobrist::RandomCastle[state().castlingRights];
    auto ep_sq = state().enPassant;
    if (ep_sq == SQ_NONE)
        return hash;
    {
        const File f = file_of(ep_sq);
        Bitboard ep_mask = (1ULL << ep_sq);
        const Color stm = side_to_move();
        ep_mask = (stm == WHITE) ? (ep_mask >> 8) : (ep_mask << 8);
        ep_mask = ((ep_mask << 1) & ~attacks::MASK_FILE[0]) | ((ep_mask >> 1) & ~attacks::MASK_FILE[7]);
        if (ep_mask & pieces<PAWN>(stm))
            hash ^= zobrist::RandomEP[f];
    }
    return hash;
}

/// @brief Parse a UCI move string into a Move object.
template <typename PieceC, typename T> Move _Position<PieceC, T>::parse_uci(std::string uci) const {
    return uci::uciToMove(*this, uci);
}

/// @brief Parse and immediately push (execute) a UCI move string.
template <typename PieceC, typename T> Move _Position<PieceC, T>::push_uci(std::string uci) {
    const auto mv = parse_uci(std::move(uci));
    do_move(mv);
    return mv;
}
/// @brief Compute the valid en-passant target square, or SQ_NONE if none.
template <typename PieceC, typename T> Square _Position<PieceC, T>::_valid_ep_square() const {
    if (ep_square() == SQ_NONE)
        return SQ_NONE;
    Rank ep_rank;
    ep_rank = side_to_move() == WHITE ? RANK_6 : RANK_3;
    const Bitboard mask = 1ULL << ep_square();
    Bitboard pawn_mask = mask >> 8;
    Bitboard org_pawn_mask = mask << 8;
    if (side_to_move() == BLACK)
        std::swap(pawn_mask, org_pawn_mask);
    // rank 3 or rank 6, depending on color
    if (rank_of(ep_square()) != ep_rank)
        return SQ_NONE;
    // a pawn in 2 ranks behind
    if (!(pieces(PAWN) & occ(~side_to_move()) & pawn_mask))
        return SQ_NONE;
    // ep_sq must be empty
    if (occ() & mask)
        return SQ_NONE;
    // emptied second rank
    if (occ() & org_pawn_mask)
        return SQ_NONE;
    return ep_square();
}
/// @brief Check if a given color has insufficient mating material.
template <typename PieceC, typename T> bool _Position<PieceC, T>::is_insufficient_material() const {
    const auto count = popcount(occ());

    if (count <= 2)
        return true;

    auto white_bishops = pieces(BISHOP, WHITE);
    auto black_bishops = pieces(BISHOP, BLACK);
    bool has_white_bishop = white_bishops != 0;
    bool has_black_bishop = black_bishops != 0;

    if (count == 3) {
        if (has_white_bishop || has_black_bishop)
            return true;
        if (pieces(KNIGHT, WHITE) || pieces(KNIGHT, BLACK))
            return true;
        return false;
    }

    if (count == 4) {

        Bitboard wb = white_bishops;
        Bitboard bb = black_bishops;

        int wb_cnt = popcount(wb);
        int bb_cnt = popcount(bb);

        Bitboard bishops = wb | bb;
        Bitboard knights = pieces(KNIGHT, WHITE) | pieces(KNIGHT, BLACK);
        Bitboard rooks = pieces(ROOK, WHITE) | pieces(ROOK, BLACK);
        Bitboard queens = pieces(QUEEN, WHITE) | pieces(QUEEN, BLACK);
        Bitboard pawns = pieces(PAWN, WHITE) | pieces(PAWN, BLACK);

        // no heavy pieces allowed for "insufficient material" cases
        if (rooks || queens || pawns)
            return false;

        // K + K + 2 minor pieces total scenario
        if (popcount(knights) + popcount(bishops) == 2) {

            // K + N + N vs K
            if (popcount(knights) == 2)
                return true;

            // K + B + B vs K (same color bishops only)
            if (popcount(bishops) == 2) {
                Square s1 = static_cast<Square>(lsb(bishops));
                Square s2 = static_cast<Square>(msb(bishops));

                if (square_color(s1) == square_color(s2))
                    return true;
            }

            // K + N + B vs K (still insufficient)
            if (popcount(knights) == 1 && popcount(bishops) == 1)
                return true;
        }

        // mixed bishop-only edge case (rare but clean)
        if (popcount(bishops) == 2 && popcount(knights) == 0) {
            Square s1 = static_cast<Square>(lsb(bishops));
            Square s2 = static_cast<Square>(msb(bishops));

            if (square_color(s1) == square_color(s2))
                return true;
        }
    }
    return false;
}
/// @brief Compute the set of castling rights that are physically valid on the board.
template <typename PieceC, typename T> CastlingRights _Position<PieceC, T>::clean_castling_rights() const {
    const Bitboard cr_BOO = 1ULL << static_cast<int>(castling_meta_[BLACK].rook_start_ks);
    const Bitboard cr_BOOO = 1ULL << static_cast<int>(castling_meta_[BLACK].rook_start_qs);
    const Bitboard cr_WOO = 1ULL << static_cast<int>(castling_meta_[WHITE].rook_start_ks);
    const Bitboard cr_WOOO = 1ULL << static_cast<int>(castling_meta_[WHITE].rook_start_qs);
    Bitboard castling = 0;
    // mappings
    castling |= (castlingRights() & WHITE_OO) ? cr_WOO : 0;
    castling |= (castlingRights() & WHITE_OOO) ? cr_WOOO : 0;

    castling |= (castlingRights() & BLACK_OO) ? cr_BOO : 0;
    castling |= (castlingRights() & BLACK_OOO) ? cr_BOOO : 0;
    castling &= pieces(ROOK);
    Bitboard white_castling = castling & attacks::MASK_RANK[RANK_1] & occ(WHITE);
    Bitboard black_castling = castling & attacks::MASK_RANK[RANK_8] & occ(BLACK);
    // rook exists in corresponding square
    white_castling &= (cr_WOO | cr_WOOO);
    black_castling &= (cr_BOO | cr_BOOO);
    // king exists in e1/e8 depending on color
    if (!(occ(WHITE) & pieces(KING) & (1ULL << castling_meta_[WHITE].king_start)))
        white_castling = 0;
    if (!(occ(BLACK) & pieces(KING) & (1ULL << castling_meta_[BLACK].king_start)))
        black_castling = 0;
    castling = white_castling | black_castling;
    // Re-map
    CastlingRights cr = NO_CASTLING;
    cr |= (castling & cr_WOO) ? WHITE_OO : NO_CASTLING;
    cr |= (castling & cr_WOOO) ? WHITE_OOO : NO_CASTLING;
    cr |= (castling & cr_BOO) ? BLACK_OO : NO_CASTLING;
    cr |= (castling & cr_BOOO) ? BLACK_OOO : NO_CASTLING;
    return cr;
}
// clang-format off
#define INSTANTIATE(PieceC) \
template bool _Position<PieceC, void>::setFEN(const std::string &, bool, FENParsingMode); \
template std::string _Position<PieceC, void>::fen(bool) const; \
template void _Position<PieceC, void>::doMove<false>(const Move &move); \
template void _Position<PieceC, void>::doMove<true>(const Move &move); \
template void _Position<PieceC, void>::refresh_attacks(); \
template uint64_t _Position<PieceC, void>::zobrist() const; \
template Move _Position<PieceC, void>::parse_uci(std::string) const; \
template Move _Position<PieceC, void>::push_uci(std::string); \
template bool _Position<PieceC, void>::is_valid<false>() const; \
template bool _Position<PieceC, void>::is_valid<true>() const; \
template CheckType _Position<PieceC, void>::givesCheck(Move) const; \
template bool _Position<PieceC, void>::is_insufficient_material() const;
// clang-format off
INSTANTIATE(PolyglotPiece)
INSTANTIATE(EnginePiece)
INSTANTIATE(ContiguousMappingPiece)
#undef INSTANTIATE
} // namespace chess


