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
    struct {
        Square king_start = SQ_NONE;
        Square rook_start_ks = SQ_NONE;
        Square rook_start_qs = SQ_NONE;
        std::array<Bitboard, 2> castling_paths;
    } castlingMetadata[2];
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
    return static_cast<MoveGenType>(static_cast<U>(a) | static_cast<U>(b));
}
template <typename PieceC = EnginePiece, typename = std::enable_if_t<is_piece_enum<PieceC>::value>> class _Position {
  private:
    // Move history stack
    std::vector<HistoryEntry<PieceC>> history;
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
        switch (sideToMove()) {
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
    template <bool RetAll = false> inline auto undoMove() -> std::conditional_t<RetAll, HistoryEntry<PieceC> &, void> {
        pieces_list[state().incr_sqs[0]] = state().incr_pc[0];
        pieces_list[state().incr_sqs[1]] = state().incr_pc[1];
        pieces_list[state().incr_sqs[2]] = state().incr_pc[2];
        pieces_list[state().incr_sqs[3]] = state().incr_pc[3];
        if constexpr (RetAll) {
            HistoryEntry<PieceC> state_ = state();
            history.pop_back();
            return state_;
        } else {
            history.pop_back();
            return;
        }
    }

    inline void doNullMove() {
        history.push_back(state());
        state().incr_sqs[0] = state().incr_sqs[1] = state().incr_sqs[2] = state().incr_sqs[3] = SQ_NONE;
        state().incr_pc[0] = state().incr_pc[1] = state().incr_pc[2] = state().incr_pc[3] = PieceC::NO_PIECE;
        state().hash ^= (ep_square() != SQ_NONE && state().epIncluded) ? zobrist::RandomEP[file_of(ep_square())] : 0;
        state().epIncluded = false;
        state().enPassant = SQ_NONE;
        state().turn = ~state().turn;
        state().hash ^= zobrist::RandomCastle[state().castlingRights];
        state().castlingRights =
            static_cast<CastlingRights>(state().castlingRights & (state().turn == WHITE ? BLACK_CASTLING : WHITE_CASTLING));
        state().hash ^= zobrist::RandomCastle[state().castlingRights];
        state().hash ^= zobrist::RandomTurn;
        state().fullMoveNumber += (state().turn == WHITE);
        state().pliesFromNull = state().repetition = 0;
        state().mv = Move::null();
        state().halfMoveClock++;
        refresh_attacks();
    }

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
        PieceC _p2;
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
#ifndef __EXCEPTIONS
        assert(p == _p2 && "Inconsistient piece map");
#else
        if (p != _p2)
            throw std::invalid_argument("Inconsistient piece map");
#endif
        return p;
#endif
    }
    [[nodiscard]] inline Bitboard us(Color c) const { return occ(c); }
    [[nodiscard]] inline Color sideToMove() const { return state().turn; }
    [[nodiscard]] inline uint64_t hash() const { return state().hash; }
    [[nodiscard]] inline uint64_t key() const { return state().hash; }
    [[nodiscard]] inline Color side_to_move() const { return state().turn; }
    [[nodiscard]] inline Square ep_square() const { return state().enPassant; }
    template <PieceType pt> [[nodiscard]] inline Square square(Color c) const {
        return static_cast<Square>(lsb(pieces<pt>(c)));
    }
    [[nodiscard]] inline Square kingSq(Color c) const { return state().kings[c]; }
    [[nodiscard]] inline Bitboard checkers() const { return _checkers; }
    [[nodiscard]] inline Bitboard pin_mask() const { return _pin_mask; }
    inline _Position(std::string fen = START_FEN, bool chess960 = false, FENParsingMode xfen = MODE_AUTO) {
        history.reserve(6144);
        setFEN(fen, chess960, xfen);
    }
    [[nodiscard]] inline bool isCapture(Move mv) const {
        return mv.type_of() == EN_PASSANT || (mv.type_of() != CASTLING && piece_on(mv.to_sq()) != PieceC::NO_PIECE);
    }
    [[nodiscard]] inline bool is_capture(Move mv) const { return isCapture(mv); }
    [[nodiscard]] inline bool is_zeroing(Move mv) const { return isCapture(mv) || at<PieceType>(mv.from_sq()) == PAWN; }
    [[nodiscard]] std::string fen(bool xfen = true) const;
    [[nodiscard]] inline uint8_t halfmoveClock() const { return state().halfMoveClock; }
    [[nodiscard]] inline uint16_t fullmoveNumber() const { return state().fullMoveNumber; }
    [[nodiscard]] inline uint8_t rule50_count() const { return state().halfMoveClock; }
    [[nodiscard]] inline CastlingRights castlingRights(Color c) const {
        return state().castlingRights & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING);
    }
    [[nodiscard]] inline CastlingRights castlingRights() const { return state().castlingRights; }
    [[nodiscard]] inline bool is_castling(Move mv) const { return mv.type_of() == CASTLING; }
    uint64_t zobrist() const;
    inline PieceC piece_at(Square sq) const { return piece_on(sq); }
    template <typename T = PieceC> inline T at(Square sq) const {
        assert(chess::is_valid(sq));
        if constexpr (std::is_same_v<T, PieceType>)
            return piece_of(piece_at(sq));
        else if constexpr (std::is_same_v<T, Color>)
            return color_of(piece_at(sq));
        else
            return piece_at(sq);
    }
    inline Square enpassantSq() const { return ep_square(); }
    CastlingRights clean_castling_rights() const;
    void setFEN(const std::string &str, bool chess960 = false, FENParsingMode xfen = MODE_AUTO);
    inline void set_fen(const std::string &str, bool chess960 = false, FENParsingMode xfen = MODE_AUTO) {
        setFEN(str, chess960, xfen);
    }
    inline void setFen(const std::string &str, bool chess960 = false, FENParsingMode xfen = MODE_AUTO) {
        setFEN(str, chess960, xfen);
    }
    Move parse_uci(std::string) const;
    Move push_uci(std::string);
    Square _valid_ep_square() const;
    template <PieceType pt> inline int count() const { return popcount(pieces(pt)); }
    template <PieceType pt, Color c> inline int count() const { return popcount(pieces<pt, c>()); }
    template <PieceType pt> inline int count(Color c) const { return popcount(pieces<pt>(c)); }
    inline int count(PieceType pt, Color c) const { return popcount(pieces(pt, c)); }
    inline int ply() const { return 2 * (state().fullMoveNumber - 1) + (sideToMove() == BLACK); }
    bool is_insufficient_material(Color c) const;
    inline bool isInsufficientMaterial(Color c) const { return is_insufficient_material(c); }
    inline bool hasInsufficientMaterial(Color c) const { return is_insufficient_material(c); }
    inline bool has_insufficient_material(Color c) const { return is_insufficient_material(c); }
    inline bool is_insufficient_material() const {
        return has_insufficient_material(WHITE) && has_insufficient_material(BLACK);
    }
    inline bool isInsufficientMaterial() const { return is_insufficient_material(); }
    inline bool hasNonPawnMaterial(Color c) const { return bool(us(c) ^ (pieces(PAWN, KING) & us(c))); }
    inline bool inCheck() const { return checkers() != 0LL; }
    inline bool is_check() const { return checkers() != 0LL; }
    inline bool has_castling_rights(Color c) const { return castlingRights(c) != 0; }
    inline bool has_kingside_castling_rights(Color c) const { return (castlingRights(c) & KING_SIDE) != 0; }
    inline bool has_queenside_castling_rights(Color c) const { return (castlingRights(c) & QUEEN_SIDE) != 0; }
    // Return true if a position repeats once earlier but strictly
    // after the root, or repeats twice before or at the root.
    inline bool is_repetition(int ply) const { return state().repetition + 1 >= ply; }
    inline int repetition_count() const { return state().repetition; }
    // Test if it's draw of 75 move rule (that forces everyone to draw). It doesn't consider checkmates.
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
    inline bool is_seventyfive_moves(int n) const { return _is_halfmoves(150); }
    inline bool is_fifty_moves(int n) const { return _is_halfmoves(150); }
    inline bool is_fivefold_repetition() const { return is_repetition(5); }
    inline bool is_attacked_by(Color color, Square sq, Bitboard occupied = 0) const {
        Bitboard occ_bb = occupied ? occupied : this->occ();
        return attackers_mask(color, sq, occ_bb) != 0;
    }

    inline bool was_into_check() const {
        bool atk = false;
        Bitboard bb = pieces<KING>(~sideToMove());
        while (!atk && bb) {
            atk |= isAttacked((Square)pop_lsb(bb), sideToMove());
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
            b |= attackers_mask(~sideToMove(), static_cast<Square>(pop_lsb(path)), occupied);
        }
        return b != 0;
    }
    inline bool is_checkmate() const {
        Movelist moves;
        legals(moves);
        return inCheck() && !moves.size();
    }
    inline bool is_stalemate() const {
        Movelist moves;
        legals(moves);
        return !inCheck() && !moves.size();
    }
    // Material-only key (note: Zobrist=Zpieces^Zep^Zcastling^Zturn, we just XORs the remaining, it's trivial)
    inline Key material_key() const {
        return hash() ^ (zobrist::RandomTurn * ~sideToMove()) ^ (zobrist::RandomCastle[castlingRights()]) ^
               (zobrist::RandomEP[ep_square() == SQ_NONE ? file_of(ep_square()) : FILE_NB]);
    }
    template <bool Strict = false> bool is_valid() const;
    CheckType givesCheck(Move move) const;
    /**
     * @brief Checks if the current position is a draw by 50 move rule.
     * Keep in mind that by the rules of chess, if the position has 50 half
     * moves it's not necessarily a draw, since checkmate has higher priority,
     * <del>call getHalfMoveDrawType,
     * to determine whether the position is a draw or checkmate.</del>
     * @return
     */
    [[nodiscard]] inline bool isHalfMoveDraw() const noexcept { return halfmoveClock() >= 100; }
    [[nodiscard]] inline Bitboard getCastlingPath(Color c, bool isKingSide) const {
        return state().castlingMetadata[c].castling_paths[isKingSide];
    }
    [[nodiscard]] inline auto getCastlingMetadata(Color c) const { return state().castlingMetadata[c]; }

  private:
    template <PieceType pt> [[nodiscard]] inline Bitboard pinMask(Color c, Square sq) const {
        static_assert(pt == BISHOP || pt == ROOK, "Only bishop or rook allowed!");
        Bitboard occ_opp = occ(~c);
        Bitboard occ_us = occ(c);
        Bitboard opp_sliders;
        opp_sliders = (pieces<pt>(~c) | pieces(QUEEN, ~c)) & occ_opp;

        auto pt_attacks = attacks::slider<pt>(sq, occ_opp) & opp_sliders;

        Bitboard pin = 0ull;

        while (pt_attacks) {
            const auto possible_pin = movegen::between(sq, Square(pop_lsb(pt_attacks)));
            Bitboard tmp = possible_pin & occ_us;
            bool v = tmp && (tmp & (tmp - 1)) == 0;
            pin |= v ? possible_pin : 0;
        }

        return pin;
    }
    void refresh_attacks();
    inline const auto &state() const { return history.back(); }
    inline auto &state() { return history.back(); }

  public:
    inline _Position(const _Position &other) : history(other.history), _chess960(other._chess960) {
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
