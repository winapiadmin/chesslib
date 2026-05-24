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
#include <stdexcept>
#include <string>
#include <vector>
namespace chess {

template <typename Piece> struct alignas(64) HistoryEntry {
    // Bitboards for each piece type (white and black)
    Bitboard pieces[7];
    Bitboard occ[COLOR_NB];
    Color turn; // true if white to move
    Move mv;
    Key hash;
    uint8_t halfMoveClock;   // Half-move clock for 50/75-move rule
    uint16_t fullMoveNumber; // Full-move number (starts at 1)
    bool epIncluded;
    int8_t repetition = 0;
    uint8_t pliesFromNull = 0;
    Square enPassant = SQ_NONE; // En passant target square
    Square kings[COLOR_NB] = { SQ_NONE };
    CastlingRights castlingRights; // Castling rights bitmask
    Square incr_sqs[4] = { SQ_NONE, SQ_NONE, SQ_NONE, SQ_NONE };
    Piece incr_pc[4] = { Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE };
    Bitboard saved_rook_pin{};
    Bitboard saved_bishop_pin{};
    Bitboard saved_checkers{};
    Bitboard saved_check_mask{};
    // implementation-specific implementations goes here
};

enum class CheckType { NO_CHECK, DIRECT_CHECK, DISCOVERY_CHECK };

enum FENParsingMode { MODE_XFEN, MODE_SMK, MODE_AUTO };

enum class MoveGenType : uint16_t {
    NONE = 0,

    // piece selectors
    PAWN = 1 << 1,
    KNIGHT = 1 << 2,
    BISHOP = 1 << 3,
    ROOK = 1 << 4,
    QUEEN = 1 << 5,
    KING = 1 << 6,

    PIECE_MASK = PAWN | KNIGHT | BISHOP | ROOK | QUEEN | KING,

    // move-type selectors
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
    return static_cast<MoveGenType>(static_cast<U>(a) |
                                    static_cast<U>(b)); // NOLINT(clang-analyzer-optin.core.EnumCastOutOfRange)
}
template <typename PieceC = EnginePiece, typename = std::enable_if_t<is_piece_enum<PieceC>::value>> class _Position {
  private:
    // Move history stack + parallel compact hash array
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
        PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE
    };
    bool _chess960{};
    struct CastlingMeta {
        Square king_start = SQ_NONE;
        Square rook_start_ks = SQ_NONE;
        Square rook_start_qs = SQ_NONE;
        std::array<Bitboard, 2> castling_paths{};
    } castling_meta_[2]{};

  public:
    static inline constexpr auto START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    static inline constexpr auto START_CHESS960_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w HAha - 0 1";
    // Legal move generation functions
    template <MoveGenType type = MoveGenType::ALL, Color c> void legals(Movelist &out) const {

        constexpr auto raw = static_cast<uint16_t>(type);
        constexpr uint16_t pieceBits = raw & static_cast<uint16_t>(MoveGenType::PIECE_MASK);
        constexpr uint16_t modeBits =
            raw & (static_cast<uint16_t>(MoveGenType::CAPTURE) | static_cast<uint16_t>(MoveGenType::QUIET));

        // ----------------------------------------
        // Resolve default piece selection
        // ----------------------------------------
        constexpr uint16_t effectivePieces =
            pieceBits ? pieceBits
                      : (raw == static_cast<uint16_t>(MoveGenType::NONE) ? 0 : static_cast<uint16_t>(MoveGenType::PIECE_MASK));

        // ----------------------------------------
        // Resolve default mode selection
        // ----------------------------------------
        constexpr bool includeCaps = modeBits == 0 || (modeBits & static_cast<uint16_t>(MoveGenType::CAPTURE));
        constexpr bool includeQuiet = modeBits == 0 || (modeBits & static_cast<uint16_t>(MoveGenType::QUIET));
        constexpr bool captureOnly = includeCaps && !includeQuiet;

        // Early-out for NONE
        if constexpr (effectivePieces == 0 && modeBits != 0)
            return;

        // Now your existing piece dispatch logic stays the same:
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

    // Legal move generation functions
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
    template <bool Strict = true> void doMove(const Move &move);
    template <bool Strict = true> void do_move(const Move &move) { doMove<Strict>(move); }
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
    template <bool RetAll = false> inline auto undo_move() -> std::conditional_t<RetAll, HistoryEntry<PieceC>, void> { return undoMove<RetAll>(); }

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

    [[nodiscard]] inline Bitboard pieces() const { return occ(); }
    template <PieceType pt> [[nodiscard]] inline Bitboard pieces(Color c) const {
        assert(c != COLOR_NB);
        if constexpr (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return state().pieces[pt] & state().occ[c];
    }
    template <Color c> [[nodiscard]] inline Bitboard pieces(PieceType pt) const {
        static_assert(c != COLOR_NB);
        if (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return state().pieces[pt] & state().occ[c];
    }
    template <PieceType pt, Color c> [[nodiscard]] inline Bitboard pieces() const {
        static_assert(c != COLOR_NB);
        if constexpr (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return state().pieces[pt] & state().occ[c];
    }
    [[nodiscard]] inline Bitboard pieces(PieceType pt, Color c) const {
        assert(c != COLOR_NB);
        // still branchless
        switch (pt) {
        case PIECE_TYPE_NB:
        case ALL_PIECES:
            return occ(c);
        default:
            return state().pieces[pt] & state().occ[c];
        }
    }
    [[nodiscard]] inline Bitboard pieces(PieceType pt) const {
        switch (static_cast<int>(pt)) {
        case PIECE_TYPE_NB:
        case ALL_PIECES:
            return occ();
        default:
            return state().pieces[pt];
        }
    }
    template <typename... PTypes, typename = std::enable_if_t<(std::is_same_v<PTypes, PieceType> && ...)>>
    [[nodiscard]] inline Bitboard pieces(PTypes... ptypes) const {
        return (state().pieces[static_cast<int>(ptypes)] | ...);
    }

    template <typename... PTypes, typename = std::enable_if_t<(std::is_same_v<PTypes, PieceType> && ...)>>
    [[nodiscard]] inline Bitboard pieces(Color c, PTypes... ptypes) const {
        return (pieces(ptypes, c) | ...);
    }

    /**
     * @brief Returns the origin squares of pieces of a given color attacking a target square
     *
     * Sample code to not make myself (and others) confused:
    ```c++
    if (attackers(BLACK, E4)) {
        std::cout << "E4 is under attack by black!";
    }
    ```
     *
     * @param color Attacker Color
     * @param square Attacked Square
     * @param occupied Board occupation
     * @return Attackers to the bitboard
     */
    [[nodiscard]] inline Bitboard attackers(Color color, Square square, Bitboard occupied) const {
        auto queens = pieces<QUEEN>(color);

        // using the fact that if we can attack PieceType from square, they can attack us back
        auto atks = (attacks::pawn(~color, square) & pieces<PAWN>(color));
        atks |= (attacks::knight(square) & pieces<KNIGHT>(color));
        atks |= (attacks::bishop(square, occupied) & (pieces<BISHOP>(color) | queens));
        atks |= (attacks::rook(square, occupied) & (pieces<ROOK>(color) | queens));
        atks |= (attacks::king(square) & pieces<KING>(color));

        return atks & occupied;
    }

    /**
     * @brief Checks if a square is attacked by the given color.
     * @param square
     * @param color
     * @return
     */
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
    /**
     * @brief Checks if a square is attacked by the given color.
     * @param square
     * @param color
     * @param occupied
     * @return
     */
    [[nodiscard]] inline bool isAttacked(Square sq, Color by, Bitboard occupied) const noexcept {
        const Bitboard diag_attackers = pieces(PieceType::BISHOP, by) | pieces(PieceType::QUEEN, by);
        const Bitboard ortho_attackers = pieces(PieceType::ROOK, by) | pieces(PieceType::QUEEN, by);

        return (attacks::pawn(~by, sq) & pieces(PieceType::PAWN, by)) ||
               (attacks::knight(sq) & pieces(PieceType::KNIGHT, by)) || (attacks::king(sq) & pieces(PieceType::KING, by)) ||
               (attacks::bishop(sq, occupied) & diag_attackers) || (attacks::rook(sq, occupied) & ortho_attackers);
    }

    [[nodiscard]] inline bool is_attacked(Square sq, Color by, Bitboard occupied) const noexcept { return isAttacked(sq, by, occupied); }

    [[nodiscard]] inline Bitboard attackers(Color color, Square square) const { return attackers(color, square, occ()); }

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

    inline void placePiece(PieceType pt, Square sq, Color c) {
        bool a = pt == KING;
        // if (pt == NO_PIECE_TYPE)
        //     return;
        Bitboard v = 1ULL << sq;
        state().pieces[pt] |= v;
        state().occ[c] |= v;
        pieces_list[sq] = make_piece<PieceC>(pt, c);
        state().hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][(int)pieces_list[sq]][sq];
        state().kings[c] = a ? sq : state().kings[c];
    }

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

    [[nodiscard]] inline Bitboard occ(Color c) const {
        ASSUME(c != COLOR_NB);
        return state().occ[c];
    }
    [[nodiscard]] inline Bitboard occ() const { return state().occ[0] | state().occ[1]; }
    inline PieceC piece_on(Square s) const {
        assert(chess::is_valid(s));
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
#if defined(_DEBUG) && !defined(NDEBUG)
        assert(p == _p2 && "Inconsistient piece map");
#endif
        return p;
#endif
    }
    [[nodiscard]] inline Bitboard us(Color c) const { return occ(c); }
    [[nodiscard]] inline uint64_t hash() const { return state().hash; }
    [[nodiscard]] inline Color side_to_move() const { return state().turn; }
    [[nodiscard]] inline Square ep_square() const { return state().enPassant; }
    template <PieceType pt> [[nodiscard]] inline Square square(Color c) const {
        return static_cast<Square>(lsb(pieces<pt>(c)));
    }
    [[nodiscard]] inline Square kingSq(Color c) const { return state().kings[c]; }
    [[nodiscard]] inline Square king_sq(Color c) const { return kingSq(c); }
    [[nodiscard]] inline Bitboard checkers() const { return _checkers; }
    [[nodiscard]] inline Bitboard pin_mask() const { return _pin_mask; }
    inline _Position(std::string fen = START_FEN, bool chess960 = false, FENParsingMode xfen = MODE_AUTO) {
        history.reserve(6144);
        rep_hashes_.reserve(6144);
        setFEN(fen, chess960, xfen);
    }
    [[nodiscard]] inline bool is_capture(Move mv) const {
        return mv.type_of() == EN_PASSANT || (mv.type_of() != CASTLING && piece_on(mv.to_sq()) != PieceC::NO_PIECE);
    }
    [[nodiscard]] inline bool isCapture(Move mv) const { return is_capture(mv); }
    [[nodiscard]] inline bool is_zeroing(Move mv) const { return is_capture(mv) || at<PieceType>(mv.from_sq()) == PAWN; }
    [[nodiscard]] inline PieceC piece_at(Square sq) const { return piece_on(sq); }
    [[nodiscard]] std::string fen(bool xfen = true) const;
    [[nodiscard]] inline uint16_t fullmoveNumber() const { return state().fullMoveNumber; }
    [[nodiscard]] inline uint16_t fullmove_number() const { return state().fullMoveNumber; }
    [[nodiscard]] inline uint8_t rule50_count() const { return state().halfMoveClock; }
    [[nodiscard]] inline CastlingRights castlingRights(Color c) const {
        return state().castlingRights & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING);
    }
    [[nodiscard]] inline CastlingRights castlingRights() const { return state().castlingRights; }
    [[nodiscard]] inline bool is_castling(Move mv) const { return mv.type_of() == CASTLING; }
    uint64_t zobrist() const;
    template <typename T = PieceC> inline T at(Square sq) const {
        assert(chess::is_valid(sq));
        if constexpr (std::is_same_v<T, PieceType>)
            return piece_of(piece_on(sq));
        else if constexpr (std::is_same_v<T, Color>)
            return color_of(piece_on(sq));
        else
            return piece_on(sq);
    }
    CastlingRights clean_castling_rights() const;
    void setFEN(const std::string &str, bool chess960 = false, FENParsingMode xfen = MODE_AUTO);
    inline void set_fen(const std::string &str, bool chess960 = false, FENParsingMode xfen = MODE_AUTO) {
        setFEN(str, chess960, xfen);
    }
    Move parse_uci(std::string) const;
    Move push_uci(std::string);
    Square _valid_ep_square() const;
    template <PieceType pt> inline int count() const { return popcount(pieces(pt)); }
    template <PieceType pt, Color c> inline int count() const { return popcount(pieces<pt, c>()); }
    template <PieceType pt> inline int count(Color c) const { return popcount(pieces<pt>(c)); }
    inline int count(PieceType pt, Color c) const { return popcount(pieces(pt, c)); }
    inline int ply() const { return 2 * (state().fullMoveNumber - 1) + (side_to_move() == BLACK); }
    bool is_insufficient_material(Color c) const;
    inline bool is_insufficient_material() const {
        return is_insufficient_material(WHITE) && is_insufficient_material(BLACK);
    }

    inline bool hasNonPawnMaterial(Color c) const { return bool(us(c) & ~(pieces(PAWN) | pieces(KING)) & occ(c)); }
    inline bool is_check() const { return checkers() != 0LL; }
    inline bool has_castling_rights(Color c) const { return castlingRights(c) != 0; }
    inline bool has_kingside_castling_rights(Color c) const { return (castlingRights(c) & KING_SIDE) != 0; }
    inline bool has_queenside_castling_rights(Color c) const { return (castlingRights(c) & QUEEN_SIDE) != 0; }
    // Return true if a position repeats once earlier but strictly
    // after the root, or repeats twice before or at the root.
    inline bool is_repetition(int ply) const { return state().repetition + 1 >= ply; }
    inline int repetition_count() const { return state().repetition; }
    // Test if it's draw of 50 move rule (that forces everyone to draw). It doesn't consider checkmates.
    inline bool is_draw(int ply) const { return rule50_count() > 99 || is_repetition(ply); }
    // Tests whether there has been at least one repetition
    // of positions since the last capture or pawn move.
    inline bool has_repeated() const {
        auto idx = history.size() - 1;
        int end = std::min(rule50_count(), state().pliesFromNull);
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
    inline bool is_attacked_by(Color color, Square sq, Bitboard occupied = 0) const {
        Bitboard occ_bb = occupied ? occupied : this->occ();
        return attackers_mask(color, sq, occ_bb) != 0;
    }

    inline bool was_into_check() const {
        bool atk = false;
        Bitboard bb = pieces<KING>(~side_to_move());
        while (!atk && bb) {
            atk |= is_attacked((Square)pop_lsb(bb), side_to_move());
        }
        return atk != 0;
    }
    inline Bitboard attackers_mask(Color color, Square square, Bitboard occupied) const {
        auto queens = pieces<QUEEN>(color);

        // using the fact that if we can attack PieceType from square, they can attack us back
        auto atks = (attacks::pawn(~color, square) & pieces<PAWN>(color));
        atks |= (attacks::knight(square) & pieces<KNIGHT>(color));
        atks |= (attacks::bishop(square, occupied) & (pieces<BISHOP>(color) | queens));
        atks |= (attacks::rook(square, occupied) & (pieces<ROOK>(color) | queens));
        atks |= (attacks::king(square) & pieces<KING>(color));

        return atks & occ(color);
    }
    inline bool _attacked_for_king(Bitboard path, Bitboard occupied) const {
        Bitboard b = 0;
        while (!b && path) {
            b |= attackers_mask(~side_to_move(), static_cast<Square>(pop_lsb(path)), occupied);
        }
        return b != 0;
    }
    inline bool is_checkmate() const {
        Movelist moves;
        legals(moves);
        return is_check() && !moves.size();
    }
    inline bool is_stalemate() const {
        Movelist moves;
        legals(moves);
        return !is_check() && !moves.size();
    }
    // Material-only key (note: Zobrist=Zpieces^Zep^Zcastling^Zturn, we just XORs the remaining, it's trivial)
    inline Key material_key() const {
        return hash() ^ (zobrist::RandomTurn * ~side_to_move()) ^ (zobrist::RandomCastle[castlingRights()]) ^
               (zobrist::RandomEP[ep_square() == SQ_NONE ? file_of(ep_square()) : FILE_NB]);
    }
    template <bool Strict = false> bool is_valid() const;
    CheckType givesCheck(Move move) const;
    [[nodiscard]] inline CheckType gives_check(Move move) const { return givesCheck(move); }
    /**
     * @brief Checks if the current position is a draw by 50 move rule.
     * Keep in mind that by the rules of chess, if the position has 50 half
     * moves it's not necessarily a draw, since checkmate has higher priority,
     * <del>call getHalfMoveDrawType,
     * to determine whether the position is a draw or checkmate.</del>
     * @return
     */
    [[nodiscard]] inline bool isHalfMoveDraw() const noexcept { return rule50_count() >= 100; }
    [[nodiscard]] inline bool is_half_move_draw() const noexcept { return isHalfMoveDraw(); }
    [[nodiscard]] inline Bitboard getCastlingPath(Color c, bool isKingSide) const {
        return castling_meta_[c].castling_paths[isKingSide];
    }
    [[nodiscard]] inline Bitboard get_castling_path(Color c, bool isKingSide) const { return getCastlingPath(c, isKingSide); }
    [[nodiscard]] inline auto getCastlingMetadata(Color c) const { return castling_meta_[c]; }
    [[nodiscard]] inline auto get_castling_metadata(Color c) const { return getCastlingMetadata(c); }

  private:
    void pinMasks(Color c, Square sq, Bitboard &rook_pin, Bitboard &bishop_pin) const {
        Bitboard occ_opp = occ(~c);
        Bitboard occ_us = occ(c);
        Bitboard opp_queens = pieces(QUEEN, ~c) & occ_opp;

        Bitboard opp_bishops = (pieces<BISHOP>(~c) | opp_queens);
        Bitboard bishop_atks = attacks::slider<BISHOP>(sq, occ_opp) & opp_bishops;

        Bitboard opp_rooks = (pieces<ROOK>(~c) | opp_queens);
        Bitboard rook_atks = attacks::slider<ROOK>(sq, occ_opp) & opp_rooks;

        rook_pin = 0; bishop_pin = 0;
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
    void refresh_attacks();
    inline const auto &state() const { return history.back(); }
    inline auto &state() { return history.back(); }

  public:
    inline _Position(const _Position &other) : history(other.history), rep_hashes_(other.rep_hashes_), _chess960(other._chess960), castling_meta_{other.castling_meta_[0], other.castling_meta_[1]} {
        std::copy(std::begin(other.pieces_list), std::end(other.pieces_list), std::begin(pieces_list));
        refresh_attacks();
    }
};
namespace attacks {
/**
 * @brief Returns the attacks for a given piece on a given square
 * @param board
 * @param color
 * @param square
 * @return
 */
template <typename T, typename = std::enable_if_t<is_piece_enum<T>::value>>
[[nodiscard]] inline Bitboard attackers(const _Position<T> &board, Color color, Square square) noexcept {
    return board.attackers(color, square);
}
} // namespace attacks
// Aliases
using Position = _Position<EnginePiece>;
using Board = _Position<EnginePiece>;
}; // namespace chess
