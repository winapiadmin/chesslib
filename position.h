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
#include "attacks.h"
#include "bitboard.h"
#include "movegen.h"
#include "types.h"
#include "zobrist.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

/// @file position.h
/// @brief Chess position representation, move execution, and game-state queries.

namespace chess {

/// @struct HistoryEntry
/// @brief Saved position state for undo operations.
/// @tparam Piece Piece-enum type.
template <typename Piece> struct alignas(64) HistoryEntry {
    Bitboard pieces[7];      ///< Bitboards per piece type.
    Bitboard occ[COLOR_NB];  ///< Occupancy per colour.
    Color turn;              ///< Side to move.
    Move mv;                 ///< The move that led to this position.
    Key hash;                ///< Zobrist hash.
    uint8_t halfMoveClock;   ///< Half-move clock for 50/75-move rule.
    uint16_t fullMoveNumber; ///< Full-move number (starts at 1).
    bool epIncluded;
    int8_t repetition; ///< Repetition counter from this position.
    uint8_t pliesFromNull = 0;
    Square enPassant = SQ_NONE; ///< En-passant target square.
    Square kings[COLOR_NB] = { SQ_NONE };
    CastlingRights castlingRights; ///< Castling rights bitmask.
    Square incr_sqs[4] = { SQ_NONE, SQ_NONE, SQ_NONE, SQ_NONE };
    Piece incr_pc[4] = { Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE };
    /// @name Cached attack data (saved to avoid recomputation on undo)
    /// @{
    Bitboard saved_rook_pin{};
    Bitboard saved_bishop_pin{};
    Bitboard saved_checkers{};
    Bitboard saved_check_mask{};
    /// @}
};

/// @enum CheckType
/// @brief Classification of check on a move.
enum class CheckType { NO_CHECK, DIRECT_CHECK, DISCOVERY_CHECK };

/// @enum FENParsingMode
/// @brief FEN parsing mode for castling rights.
enum FENParsingMode { MODE_XFEN, MODE_SMK, MODE_AUTO };

/// @enum MoveGenType
/// @brief Flags controlling which pieces and move types are generated.
enum class MoveGenType : uint16_t {
    NONE = 0,

    PAWN = 1 << 1,
    KNIGHT = 1 << 2,
    BISHOP = 1 << 3,
    ROOK = 1 << 4,
    QUEEN = 1 << 5,
    KING = 1 << 6,

    PIECE_MASK = PAWN | KNIGHT | BISHOP | ROOK | QUEEN | KING,

    CAPTURE = 1 << 7,
    QUIET = 1 << 8,

    ALL = PIECE_MASK | CAPTURE | QUIET
};

template <typename MoveGenType> constexpr MoveGenType operator&(MoveGenType a, MoveGenType b) {
    using U = std::underlying_type_t<MoveGenType>;
    return static_cast<MoveGenType>(static_cast<U>(a) & static_cast<U>(b));
}

template <typename MoveGenType> constexpr MoveGenType operator|(MoveGenType a, MoveGenType b) {
    using U = std::underlying_type_t<MoveGenType>;
    return static_cast<MoveGenType>(static_cast<U>(a) | static_cast<U>(b));
}

/// @class _Position
/// @brief Templated chess position.
/// @tparam PieceC Piece-enum type (EnginePiece, PolyglotPiece, or ContiguousMappingPiece).
/// @tparam (unused) Position tag parameter.
template <typename PieceC = EnginePiece, typename = std::enable_if_t<is_piece_enum<PieceC>::value>> class _Position {
  private:
    std::vector<HistoryEntry<PieceC>> history;
    std::vector<Key> rep_hashes_;
    Bitboard _rook_pin{};
    Bitboard _bishop_pin{};
    Bitboard _checkers{};
    Bitboard _check_mask{};
    Bitboard _pin_mask{};
    PieceC pieces_list[SQUARE_NB + 1] = {
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE
    };
    bool _chess960{};

    /// @struct CastlingMeta
    /// @brief Per-colour castling metadata for Chess960.
    struct CastlingMeta {
        Square king_start = SQ_NONE;
        Square rook_start_ks = SQ_NONE;
        Square rook_start_qs = SQ_NONE;
        std::array<Bitboard, 2> castling_paths{};
    } castling_meta_[2]{};

  public:
    static inline constexpr auto START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    static inline constexpr auto START_CHESS960_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w HAha - 0 1";

    /// @brief Generate legal moves filtered by type.
    /// @tparam type Bitmask of MoveGenType flags.
    /// @tparam c Colour to move.
    /// @param out Output move list.
    template <MoveGenType type = MoveGenType::ALL, Color c> void legals(Movelist &out) const {
        constexpr auto raw = static_cast<uint16_t>(type);
        constexpr uint16_t pieceBits = raw & static_cast<uint16_t>(MoveGenType::PIECE_MASK);
        constexpr uint16_t modeBits =
            raw & (static_cast<uint16_t>(MoveGenType::CAPTURE) | static_cast<uint16_t>(MoveGenType::QUIET));

        constexpr uint16_t effectivePieces =
            pieceBits ? pieceBits
                      : (raw == static_cast<uint16_t>(MoveGenType::NONE) ? 0 : static_cast<uint16_t>(MoveGenType::PIECE_MASK));

        constexpr bool includeCaps = modeBits == 0 || (modeBits & static_cast<uint16_t>(MoveGenType::CAPTURE));
        constexpr bool includeQuiet = modeBits == 0 || (modeBits & static_cast<uint16_t>(MoveGenType::QUIET));
        constexpr bool captureOnly = includeCaps && !includeQuiet;

        if constexpr (effectivePieces == 0 && modeBits != 0)
            return;

        if constexpr (effectivePieces & static_cast<uint16_t>(MoveGenType::PAWN)) {
            movegen::genPawnSingleMoves<PieceC, c, captureOnly>(*this, out, _rook_pin, _bishop_pin, _check_mask);
            if constexpr (includeQuiet)
                movegen::genPawnDoubleMoves<PieceC, c>(*this, out, _pin_mask, _check_mask);
            if constexpr (includeCaps)
                movegen::genEP<PieceC, c>(*this, out);
        }
        if constexpr (effectivePieces & static_cast<uint16_t>(MoveGenType::KNIGHT)) {
            movegen::genKnightMoves<PieceC, c, captureOnly>(*this, out, _pin_mask, _check_mask);
        }
        if constexpr (effectivePieces & static_cast<uint16_t>(MoveGenType::KING)) {
            movegen::genKingMoves<PieceC, c, captureOnly>(*this, out, _pin_mask);
        }
        if constexpr (effectivePieces & static_cast<uint16_t>(MoveGenType::BISHOP)) {
            movegen::genSlidingMoves<PieceC, c, BISHOP, captureOnly>(*this, out, _rook_pin, _bishop_pin, _check_mask);
        }
        if constexpr (effectivePieces & static_cast<uint16_t>(MoveGenType::ROOK)) {
            movegen::genSlidingMoves<PieceC, c, ROOK, captureOnly>(*this, out, _rook_pin, _bishop_pin, _check_mask);
        }
        if constexpr (effectivePieces & static_cast<uint16_t>(MoveGenType::QUEEN)) {
            movegen::genSlidingMoves<PieceC, c, QUEEN, captureOnly>(*this, out, _rook_pin, _bishop_pin, _check_mask);
        }
    }

    /// @brief Generate legal moves (runtime colour dispatch).
    template <MoveGenType type = MoveGenType::ALL> inline void legals(Movelist &out) const {
        switch (side_to_move()) {
        case WHITE:
            legals<type, WHITE>(out);
            return;
        case BLACK:
            legals<type, BLACK>(out);
            return;
        default:
            UNREACHABLE();
            return;
        }
    }

    /// @brief Execute a move on the board.
    /// @tparam Strict If true, validates that the move is legal.
    template <bool Strict = true> void doMove(const Move &move);
    template <bool Strict = true> void do_move(const Move &move) { doMove<Strict>(move); }

    /// @brief Undo the last move.
    /// @tparam RetAll If true, return the popped HistoryEntry.
    /// @return The saved state if RetAll, otherwise void.
    template <bool RetAll = false> inline auto undoMove() -> std::conditional_t<RetAll, HistoryEntry<PieceC>, void> {
        pieces_list[state().incr_sqs[0]] = state().incr_pc[0];
        pieces_list[state().incr_sqs[1]] = state().incr_pc[1];
        pieces_list[state().incr_sqs[2]] = state().incr_pc[2];
        pieces_list[state().incr_sqs[3]] = state().incr_pc[3];
        rep_hashes_.pop_back();
        _rook_pin = state().saved_rook_pin;
        _bishop_pin = state().saved_bishop_pin;
        _checkers = state().saved_checkers;
        _check_mask = state().saved_check_mask;
        _pin_mask = _rook_pin | _bishop_pin;
        if constexpr (RetAll) {
            HistoryEntry<PieceC> state_ = state();
            history.pop_back();
            return state_;
        } else {
            history.pop_back();
            return;
        }
    }
    template <bool RetAll = false> inline auto undo_move() -> std::conditional_t<RetAll, HistoryEntry<PieceC>, void> {
        return undoMove<RetAll>();
    }

    /// @brief Execute a null move (switch sides without moving).
    inline void doNullMove() {
        history.push_back(state());
        state().saved_rook_pin = _rook_pin;
        state().saved_bishop_pin = _bishop_pin;
        state().saved_checkers = _checkers;
        state().saved_check_mask = _check_mask;
        state().incr_sqs[0] = state().incr_sqs[1] = state().incr_sqs[2] = state().incr_sqs[3] = SQ_NONE;
        state().incr_pc[0] = state().incr_pc[1] = state().incr_pc[2] = state().incr_pc[3] = PieceC::NO_PIECE;
        state().hash ^= (ep_square() != SQ_NONE && state().epIncluded) ? zobrist::RandomEP[file_of(ep_square())] : 0;
        state().epIncluded = false;
        state().enPassant = SQ_NONE;
        state().turn = ~state().turn;
        state().hash ^= zobrist::RandomTurn;
        rep_hashes_.push_back(state().hash);
        state().fullMoveNumber += (state().turn == WHITE);
        state().pliesFromNull = state().repetition = 0;
        state().mv = Move::null();
        state().halfMoveClock++;
        refresh_attacks();
    }
    inline void do_null_move() { doNullMove(); }

    /// @name Occupancy queries
    /// @{

    /// @brief Combined occupancy (both colours).
    [[nodiscard]] inline Bitboard pieces() const { return occ(); }

    /// @brief Bitboard of a piece type for a colour (compile-time colour).
    template <PieceType pt> [[nodiscard]] inline Bitboard pieces(Color c) const {
#if defined(_CHESSLIB_ERROR_MODE_ASSERTS)
        assert(c != COLOR_NB && "color is COLOR_NB");
#elif defined(_CHESSLIB_ERROR_MODE_THROWS)
        if (c == COLOR_NB)
            throw std::runtime_error("color is COLOR_NB");
#endif
        if constexpr (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return state().pieces[pt] & state().occ[c];
    }

    /// @brief Bitboard of a piece type for a colour (runtime colour).
    template <Color c> [[nodiscard]] inline Bitboard pieces(PieceType pt) const {
        static_assert(c != COLOR_NB);
        if (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return state().pieces[pt] & state().occ[c];
    }

    /// @brief Bitboard of a piece type for a colour (compile-time both).
    template <PieceType pt, Color c> [[nodiscard]] inline Bitboard pieces() const {
        static_assert(c != COLOR_NB);
        if constexpr (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return state().pieces[pt] & state().occ[c];
    }

    /// @brief Bitboard of a piece type for a colour (runtime both).
    [[nodiscard]] inline Bitboard pieces(PieceType pt, Color c) const {
#if defined(_CHESSLIB_ERROR_MODE_ASSERTS)
        assert(c != COLOR_NB && "color is COLOR_NB");
#elif defined(_CHESSLIB_ERROR_MODE_THROWS)
        if (c == COLOR_NB)
            throw std::runtime_error("color is COLOR_NB");
#endif
        switch (pt) {
        case PIECE_TYPE_NB:
        case ALL_PIECES:
            return occ(c);
        default:
            return state().pieces[pt] & state().occ[c];
        }
    }

    /// @brief Bitboard of a piece type (both colours).
    [[nodiscard]] inline Bitboard pieces(PieceType pt) const {
        switch (static_cast<int>(pt)) {
        case PIECE_TYPE_NB:
        case ALL_PIECES:
            return occ();
        default:
            return state().pieces[pt];
        }
    }

    /// @brief Union bitboard of multiple piece types.
    template <typename... PTypes, typename = std::enable_if_t<(std::is_same_v<PTypes, PieceType> && ...)>>
    [[nodiscard]] inline Bitboard pieces(PTypes... ptypes) const {
        return (state().pieces[static_cast<int>(ptypes)] | ...);
    }

    /// @brief Union bitboard of multiple piece types for a colour.
    template <typename... PTypes, typename = std::enable_if_t<(std::is_same_v<PTypes, PieceType> && ...)>>
    [[nodiscard]] inline Bitboard pieces(Color c, PTypes... ptypes) const {
        return (pieces(ptypes, c) | ...);
    }

    /// @}

    /// @brief Get all pieces of a given colour attacking a target square.
    /// @param colour Attacker colour.
    /// @param square Attacked square.
    /// @param occupied Occupancy bitboard.
    /// @return Bitboard of attackers.
    [[nodiscard]] inline Bitboard attackers(Color colour, Square square, Bitboard occupied) const {
        auto queens = pieces<QUEEN>(colour);

        auto atks = (attacks::pawn(~colour, square) & pieces<PAWN>(colour));
        atks |= (attacks::knight(square) & pieces<KNIGHT>(colour));
        atks |= (attacks::bishop(square, occupied) & (pieces<BISHOP>(colour) | queens));
        atks |= (attacks::rook(square, occupied) & (pieces<ROOK>(colour) | queens));
        atks |= (attacks::king(square) & pieces<KING>(colour));

        return atks & occupied;
    }

    /// @brief Test whether a square is attacked by the given colour.
    [[nodiscard]] inline bool isAttacked(Square sq, Color by) const noexcept {
        const Bitboard occ_bb = occ();
        const Bitboard us_bb = occ(by);

        Bitboard diag_attackers = pieces(PieceType::BISHOP, by) | pieces(PieceType::QUEEN, by);
        Bitboard ortho_attackers = pieces(PieceType::ROOK, by) | pieces(PieceType::QUEEN, by);

        return (attacks::pawn(~by, sq) & pieces(PieceType::PAWN, by)) ||
               (attacks::knight(sq) & pieces(PieceType::KNIGHT, by)) || (attacks::king(sq) & pieces(PieceType::KING, by)) ||
               (attacks::bishop(sq, occ_bb) & diag_attackers & us_bb) || (attacks::rook(sq, occ_bb) & ortho_attackers & us_bb);
    }
    [[nodiscard]] inline bool is_attacked(Square sq, Color by) const noexcept { return isAttacked(sq, by); }

    /// @brief Test whether a square is attacked (custom occupancy).
    [[nodiscard]] inline bool isAttacked(Square sq, Color by, Bitboard occupied) const noexcept {
        const Bitboard diag_attackers = pieces(PieceType::BISHOP, by) | pieces(PieceType::QUEEN, by);
        const Bitboard ortho_attackers = pieces(PieceType::ROOK, by) | pieces(PieceType::QUEEN, by);

        return (attacks::pawn(~by, sq) & pieces(PieceType::PAWN, by)) ||
               (attacks::knight(sq) & pieces(PieceType::KNIGHT, by)) || (attacks::king(sq) & pieces(PieceType::KING, by)) ||
               (attacks::bishop(sq, occupied) & diag_attackers) || (attacks::rook(sq, occupied) & ortho_attackers);
    }
    [[nodiscard]] inline bool is_attacked(Square sq, Color by, Bitboard occupied) const noexcept {
        return isAttacked(sq, by, occupied);
    }

    /// @brief Get attackers for a colour using the current occupancy.
    [[nodiscard]] inline Bitboard attackers(Color colour, Square square) const { return attackers(colour, square, occ()); }

    /// @brief Place a piece on the board (compile-time piece type).
    template <PieceType pt> inline void placePiece(Square sq, Color c) {
        if constexpr (pt != NO_PIECE_TYPE) {
            Bitboard v = 1ULL << sq;
            state().pieces[pt] |= v;
            state().occ[c] |= v;
            pieces_list[sq] = make_piece<PieceC>(pt, c);
            state().hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][(int)pieces_list[sq]][sq];
            if constexpr (pt == KING)
                state().kings[c] = sq;
        }
    }

    /// @brief Remove a piece from the board (compile-time piece type).
    template <PieceType pt> inline void removePiece(Square sq, Color c) {
        if constexpr (pt != NO_PIECE_TYPE) {
            Bitboard v = ~(1ULL << sq);
            state().pieces[pt] &= v;
            state().occ[c] &= v;
            pieces_list[sq] = PieceC::NO_PIECE;
            state().hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][static_cast<int>(make_piece<PieceC>(pt, c))][sq];
            if constexpr (pt == KING)
                state().kings[c] = SQ_NONE;
        }
    }

    /// @brief Place a piece (runtime piece type).
    inline void placePiece(PieceType pt, Square sq, Color c) {
        bool a = pt == KING;
        Bitboard v = 1ULL << sq;
        state().pieces[pt] |= v;
        state().occ[c] |= v;
        pieces_list[sq] = make_piece<PieceC>(pt, c);
        state().hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][(int)pieces_list[sq]][sq];
        state().kings[c] = a ? sq : state().kings[c];
    }

    /// @brief Remove a piece (runtime piece type).
    inline void removePiece(PieceType pt, Square sq, Color c) {
        bool a = pt == KING;
        if (pt != NO_PIECE_TYPE) {
            Bitboard v = ~(1ULL << sq);
            state().pieces[pt] &= v;
            state().occ[c] &= v;
            pieces_list[sq] = PieceC::NO_PIECE;
            state().hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][static_cast<int>(make_piece<PieceC>(pt, c))][sq];
            state().kings[c] = a ? SQ_NONE : state().kings[c];
        }
    }

    /// @brief Occupancy of a single colour.
    [[nodiscard]] inline Bitboard occ(Color c) const {
        ASSUME(c != COLOR_NB);
        return state().occ[c];
    }

    /// @brief Combined occupancy.
    [[nodiscard]] inline Bitboard occ() const { return state().occ[0] | state().occ[1]; }

    /// @brief Piece on a square.
    inline PieceC piece_on(Square s) const {
#if defined(_CHESSLIB_ERROR_MODE_ASSERTS)
        assert(chess::is_valid(sq) && "sq is out-of-bounds");
#elif defined(_CHESSLIB_ERROR_MODE_THROWS)
        if (!chess::is_valid(sq))
            throw std::runtime_error("sq is out-of-bounds");
#endif
#if !defined(_DEBUG) || defined(NDEBUG)
        return pieces_list[s];
#else
        PieceC _p2 = PieceC::NO_PIECE;
        Bitboard mask = (1ULL << s);
        if (((state().occ[WHITE] | state().occ[BLACK]) & mask) == 0) {
            _p2 = PieceC::NO_PIECE;
        } else {
            bool c = (state().occ[WHITE] & mask) != 0;
            for (PieceType pt : { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING }) {
                bool is_p = (state().pieces[(int)pt] & mask) != 0;
                if (is_p) {
                    _p2 = make_piece<PieceC>(pt, c ? WHITE : BLACK);
                    break;
                }
            }
        }
        auto p = pieces_list[s];
#if defined(_CHESSLIB_ERROR_MODE_ASSERTS)
        assert(p == _p2 && "Inconsistient piece map");
#elif defined(_CHESSLIB_ERROR_MODE_THROWS)
        if (p != _p2)
            throw std::runtime_error("Inconsistient piece map");
#endif
        return p;
#endif
    }
    [[nodiscard]] inline Bitboard us(Color c) const { return occ(c); }
    [[nodiscard]] inline uint64_t hash() const { return state().hash; }
    [[nodiscard]] inline Color side_to_move() const { return state().turn; }
    [[nodiscard]] inline Square ep_square() const { return state().enPassant; }

    /// @brief Get the square of a king.
    template <PieceType pt> [[nodiscard]] inline Square square(Color c) const {
        return static_cast<Square>(lsb(pieces<pt>(c)));
    }
    [[nodiscard]] inline Square kingSq(Color c) const { return state().kings[c]; }
    [[nodiscard]] inline Square king_sq(Color c) const { return kingSq(c); }

    /// @brief Current checkers.
    [[nodiscard]] inline Bitboard checkers() const { return _checkers; }

    /// @brief Combined pin mask.
    [[nodiscard]] inline Bitboard pin_mask() const { return _pin_mask; }

    /// @brief Construct from a FEN string.
    inline _Position(std::string fen = START_FEN, bool chess960 = false, FENParsingMode xfen = MODE_AUTO) {
        history.reserve(6144);
        rep_hashes_.reserve(6144);
        setFEN(fen, chess960, xfen);
    }

    /// @brief Check whether a move is a capture.
    [[nodiscard]] inline bool is_capture(Move mv) const {
        return mv.type_of() == EN_PASSANT || (mv.type_of() != CASTLING && piece_on(mv.to_sq()) != PieceC::NO_PIECE);
    }
    [[nodiscard]] inline bool isCapture(Move mv) const { return is_capture(mv); }

    /// @brief Whether the move resets the 50-move clock (capture or pawn move).
    [[nodiscard]] inline bool is_zeroing(Move mv) const { return is_capture(mv) || at<PieceType>(mv.from_sq()) == PAWN; }
    [[nodiscard]] inline PieceC piece_at(Square sq) const { return piece_on(sq); }

    /// @brief Export position to FEN.
    [[nodiscard]] std::string fen(bool xfen = true) const;

    [[nodiscard]] inline uint16_t fullmoveNumber() const { return state().fullMoveNumber; }
    [[nodiscard]] inline uint16_t fullmove_number() const { return state().fullMoveNumber; }
    [[nodiscard]] inline uint8_t rule50_count() const { return state().halfMoveClock; }

    /// @brief Castling rights for a specific colour.
    [[nodiscard]] inline CastlingRights castlingRights(Color c) const {
        return state().castlingRights & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING);
    }
    [[nodiscard]] inline CastlingRights castlingRights() const { return state().castlingRights; }

    /// @brief Whether a move is a castling move.
    [[nodiscard]] inline bool is_castling(Move mv) const { return mv.type_of() == CASTLING; }

    /// @brief Raw Zobrist hash.
    uint64_t zobrist() const;

    /// @brief Extract a property from a square.
    template <typename T = PieceC> inline T at(Square sq) const {
        if constexpr (std::is_same_v<T, PieceType>)
            return piece_of(piece_on(sq));
        else if constexpr (std::is_same_v<T, Color>)
            return color_of(piece_on(sq));
        else
            return piece_on(sq);
    }

    /// @brief Get the castling rights with only the active rook squares set.
    CastlingRights clean_castling_rights() const;

    /// @brief Set position from FEN.
    void setFEN(const std::string &str, bool chess960 = false, FENParsingMode xfen = MODE_AUTO);
    inline void set_fen(const std::string &str, bool chess960 = false, FENParsingMode xfen = MODE_AUTO) {
        setFEN(str, chess960, xfen);
    }

    /// @brief Parse a UCI move string for this position.
    Move parse_uci(std::string) const;

    /// @brief Parse and execute a UCI move.
    Move push_uci(std::string);

    /// @brief Compute the valid en-passant square (if any).
    Square _valid_ep_square() const;

    /// @name Piece counts
    /// @{
    template <PieceType pt> inline int count() const { return popcount(pieces(pt)); }
    template <PieceType pt, Color c> inline int count() const { return popcount(pieces<pt, c>()); }
    template <PieceType pt> inline int count(Color c) const { return popcount(pieces<pt>(c)); }
    inline int count(PieceType pt, Color c) const { return popcount(pieces(pt, c)); }
    /// @}

    /// @brief Ply count from the start of the game.
    inline int ply() const { return 2 * (state().fullMoveNumber - 1) + (side_to_move() == BLACK); }

    /// @brief Test for draw by insufficient material.
    bool is_insufficient_material(Color c) const;
    inline bool is_insufficient_material() const { return is_insufficient_material(WHITE) && is_insufficient_material(BLACK); }

    /// @brief Whether a colour has any non-pawn, non-king material.
    inline bool hasNonPawnMaterial(Color c) const { return bool(us(c) & ~(pieces(PAWN) | pieces(KING)) & occ(c)); }

    /// @brief Whether the side to move is in check.
    inline bool is_check() const { return checkers() != 0LL; }

    /// @name Castling-right queries
    /// @{
    inline bool has_castling_rights(Color c) const { return castlingRights(c) != 0; }
    inline bool has_kingside_castling_rights(Color c) const { return (castlingRights(c) & KING_SIDE) != 0; }
    inline bool has_queenside_castling_rights(Color c) const { return (castlingRights(c) & QUEEN_SIDE) != 0; }
    /// @}

    /// @brief Whether the position has repeated the given number of times.
    inline bool is_repetition(int ply) const { return state().repetition + 1 >= ply; }
    inline int repetition_count() const { return state().repetition; }

    /// @brief Whether the position is a draw (50-move or repetition).
    inline bool is_draw(int ply) const { return rule50_count() > 99 || is_repetition(ply); }

    /// @brief Whether there has been at least one repetition since the last capture or pawn move.
    inline bool has_repeated() const {
        auto idx = history.size() - 1;
        int end = std::min({ static_cast<int>(rule50_count()),
                             static_cast<int>(state().pliesFromNull),
                             static_cast<int>(history.size()) - 1 });
        while (end-- >= 4) {
            if (history[idx].repetition)
                return true;
            idx--;
        }
        return false;
    }

    inline bool _is_halfmoves(int n) const { return rule50_count() >= n; }
    inline bool chess960() const { return _chess960; }
    inline bool is_seventyfive_moves() const { return _is_halfmoves(150); }
    inline bool is_fifty_moves() const { return _is_halfmoves(100); }
    inline bool is_fivefold_repetition() const { return is_repetition(5); }

    /// @brief Whether a square is attacked by a colour (with optional custom occupancy).
    inline bool is_attacked_by(Color color, Square sq, Bitboard occupied = 0) const {
        Bitboard occ_bb = occupied ? occupied : this->occ();
        return attackers_mask(color, sq, occ_bb) != 0;
    }

    /// @brief Whether the previous move left the opponent in check.
    inline bool was_into_check() const {
        bool atk = false;
        Bitboard bb = pieces<KING>(~side_to_move());
        while (!atk && bb) {
            atk |= is_attacked((Square)pop_lsb(bb), side_to_move());
        }
        return atk != 0;
    }

    /// @brief Get attackers mask for a colour to a square.
    inline Bitboard attackers_mask(Color color, Square square, Bitboard occupied) const {
        auto queens = pieces<QUEEN>(color);

        auto atks = (attacks::pawn(~color, square) & pieces<PAWN>(color));
        atks |= (attacks::knight(square) & pieces<KNIGHT>(color));
        atks |= (attacks::bishop(square, occupied) & (pieces<BISHOP>(color) | queens));
        atks |= (attacks::rook(square, occupied) & (pieces<ROOK>(color) | queens));
        atks |= (attacks::king(square) & pieces<KING>(color));

        return atks & occ(color);
    }

    /// @brief Check whether any square on path is attacked (for castling through check).
    inline bool _attacked_for_king(Bitboard path, Bitboard occupied) const {
        Bitboard b = 0;
        while (!b && path) {
            b |= attackers_mask(~side_to_move(), static_cast<Square>(pop_lsb(path)), occupied);
        }
        return b != 0;
    }

    /// @brief Whether the current side is checkmated.
    inline bool is_checkmate() const {
        Movelist moves;
        legals(moves);
        return is_check() && !moves.size();
    }

    /// @brief Whether the current side is stalemated.
    inline bool is_stalemate() const {
        Movelist moves;
        legals(moves);
        return !is_check() && !moves.size();
    }

    /// @brief Compute the material-only key (excludes turn, EP, castling).
    inline Key material_key() const {
        return hash() ^ (zobrist::RandomTurn * ~side_to_move()) ^ (zobrist::RandomCastle[castlingRights()]) ^
               (state().epIncluded ? zobrist::RandomEP[file_of(ep_square())] : zobrist::RandomEP[FILE_NB]);
    }

    /// @brief Validate position consistency.
    template <bool Strict = false> bool is_valid() const;

    /// @brief Classify check type for a move.
    CheckType givesCheck(Move move) const;
    [[nodiscard]] inline CheckType gives_check(Move move) const { return givesCheck(move); }

    /// @brief Whether the 50-move rule applies (>= 100 half-moves).
    [[nodiscard]] inline bool isHalfMoveDraw() const noexcept { return rule50_count() >= 100; }
    [[nodiscard]] inline bool is_half_move_draw() const noexcept { return isHalfMoveDraw(); }

    /// @brief Get the castling path bitboard for a colour and side.
    [[nodiscard]] inline Bitboard getCastlingPath(Color c, bool isKingSide) const {
        return castling_meta_[c].castling_paths[isKingSide];
    }
    [[nodiscard]] inline Bitboard get_castling_path(Color c, bool isKingSide) const { return getCastlingPath(c, isKingSide); }
    [[nodiscard]] inline auto getCastlingMetadata(Color c) const { return castling_meta_[c]; }
    [[nodiscard]] inline auto get_castling_metadata(Color c) const { return getCastlingMetadata(c); }

  private:
    /// @brief Compute pin masks for the king at sq.
    void pinMasks(Color c, Square sq, Bitboard &rook_pin, Bitboard &bishop_pin) const {
        Bitboard occ_opp = occ(~c);
        Bitboard occ_us = occ(c);
        Bitboard opp_queens = pieces(QUEEN, ~c) & occ_opp;

        Bitboard opp_bishops = (pieces<BISHOP>(~c) | opp_queens);
        Bitboard bishop_atks = attacks::slider<BISHOP>(sq, occ_opp) & opp_bishops;

        Bitboard opp_rooks = (pieces<ROOK>(~c) | opp_queens);
        Bitboard rook_atks = attacks::slider<ROOK>(sq, occ_opp) & opp_rooks;

        rook_pin = 0;
        bishop_pin = 0;
        while (bishop_atks) {
            auto possible = movegen::between(sq, Square(pop_lsb(bishop_atks)));
            Bitboard tmp = possible & occ_us;
            if (tmp && (tmp & (tmp - 1)) == 0)
                bishop_pin |= possible;
        }
        while (rook_atks) {
            auto possible = movegen::between(sq, Square(pop_lsb(rook_atks)));
            Bitboard tmp = possible & occ_us;
            if (tmp && (tmp & (tmp - 1)) == 0)
                rook_pin |= possible;
        }
    }

    /// @brief Recompute cached attack data (pins, checkers, check mask).
    void refresh_attacks();

    inline const auto &state() const { return history.back(); }
    inline auto &state() { return history.back(); }

  public:
    /// @brief Copy constructor (deep copy of position state).
    inline _Position(const _Position &other)
        : history(other.history), rep_hashes_(other.rep_hashes_), _chess960(other._chess960),
          castling_meta_{ other.castling_meta_[0], other.castling_meta_[1] } {
        std::copy(std::begin(other.pieces_list), std::end(other.pieces_list), std::begin(pieces_list));
        refresh_attacks();
    }
};

namespace attacks {
/// @brief Convenience wrapper: get attackers from a position.
/// @tparam T Piece-enum type.
/// @param board The position.
/// @param colour Attacker colour.
/// @param square Attacked square.
/// @return Bitboard of attackers.
template <typename T, typename = std::enable_if_t<is_piece_enum<T>::value>>
[[nodiscard]] inline Bitboard attackers(const _Position<T> &board, Color colour, Square square) noexcept {
    return board.attackers(colour, square);
}
} // namespace attacks

using Position = _Position<EnginePiece>;
using Board [[deprecated]] = _Position<EnginePiece>;
}; // namespace chess
