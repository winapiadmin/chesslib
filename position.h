#pragma once
#include "attacks.h"
#include "bitboard.h"
#include "printers.h"
#include "types.h"
#include "zobrist.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#if defined(WIN32)
#include <malloc.h>
#endif
namespace chess {

template <typename Piece> struct alignas(64) HistoryEntry {
    // Bitboards for each piece type (white and black)
    Bitboard pieces[7];
    Bitboard occ[COLOR_NB];
    Color turn; // true if white to move
    Move mv;
    Key hash;
    struct {
        uint8_t halfMoveClock;   // Half-move clock for 50/75-move rule
        uint16_t fullMoveNumber; // Full-move number (starts at 1)
        bool epIncluded;
        int8_t repetition = 0;
        uint8_t pliesFromNull = 0;
    };
    Square enPassant = SQ_NONE; // En passant target square
    Square kings[COLOR_NB] = { SQ_NONE };
    CastlingRights castlingRights; // Castling rights bitmask
    Square incr_sqs[4] = { SQ_NONE, SQ_NONE, SQ_NONE, SQ_NONE };
    Piece incr_pc[4] = { Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE };
    // implementation-specific implementations goes here
};

template <typename T, std::size_t MaxSize> class HeapAllocatedValueList {
  private:
    constexpr static int ALIGNMENT = 64;

  public:
    using size_type = std::size_t;
    inline HeapAllocatedValueList() {
        values_ = reinterpret_cast<T *>(::operator new(MaxSize*sizeof(T), std::align_val_t{ALIGNMENT}));
        assert(values_);
    }
    inline ~HeapAllocatedValueList() { ::operator delete(values_, std::align_val_t{ALIGNMENT}); }

    HeapAllocatedValueList(const HeapAllocatedValueList &other) : size_(other.size_) {
        ::operator delete(values_, std::align_val_t{ALIGNMENT});
        values_ = reinterpret_cast<T *>(::operator new(MaxSize*sizeof(T), std::align_val_t{ALIGNMENT}));
        assert(values_);
        std::copy(other.values_, other.values_ + size_, values_);
    }

    HeapAllocatedValueList &operator=(const HeapAllocatedValueList &other) {
        if (this != &other) {
            ::operator delete(values_, std::align_val_t{ALIGNMENT});
            // Allocate new memory and copy
            T *new_values = reinterpret_cast<T *>(::operator new(MaxSize*sizeof(T), std::align_val_t{ALIGNMENT}));
            assert(new_values);
            std::copy(other.values_, other.values_ + other.size_, new_values);

            // Free old memory
            free(values_);

            // Assign new data
            values_ = new_values;
            size_ = other.size_;
        }
        return *this;
    }

    HeapAllocatedValueList(HeapAllocatedValueList &&other) noexcept : size_(other.size_) {
        ::operator delete(values_, std::align_val_t{ALIGNMENT});
        values_ = reinterpret_cast<T *>(::operator new(MaxSize*sizeof(T), std::align_val_t{ALIGNMENT}));
        assert(values_);
        std::copy(other.values_, other.values_ + size_, values_);
    }

    HeapAllocatedValueList &operator=(HeapAllocatedValueList &&other) noexcept {
        if (this != &other) {
            ::operator delete(values_, std::align_val_t{ALIGNMENT});
            values_ = reinterpret_cast<T *>(::operator new(MaxSize*sizeof(T), std::align_val_t{ALIGNMENT}));
            assert(values_);
            std::copy(other.values_, other.values_ + size_, values_);
        }
        return *this;
    }
    inline size_type size() const { return size_; }

    inline void push_back(const T &value) {
        assert(size_ < MaxSize);

        values_[size_++] = value;
    }
    inline void clear() { size_ = 0; }
    inline const T &pop() {
        assert(size_ > 0);
        return values_[--size_]; // always safe due to mask
    }

    inline void pop_back() { size_ -= (size_ > 0) ? 1 : 0; }

    inline const T &front() const { return (size_ > 0) ? values_[0] : T{}; }

    inline T &operator[](int index) {
        assert(index < MaxSize); // relax the conditions, it's BRANCHLESS so forgive
        size_type i = static_cast<size_type>(index);
        return values_[i];
    }

    inline T &operator[](int index) const {
        assert(index < MaxSize); // relax the conditions, it's BRANCHLESS so forgive
        size_type i = static_cast<size_type>(index);
        return values_[i];
    }
    inline const T *begin() const { return values_; }
    inline T *data() { return values_; }
    inline const T *end() const { return values_ + size_; }
    size_type size_ = 0;

  private:
    T *values_;
};

enum class MoveGenType : uint8_t { ALL, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, CAPTURE };
template <typename PieceC = EnginePiece, typename = std::enable_if_t<is_piece_enum<PieceC>::value>> class _Position {
  private:
    HistoryEntry<PieceC> current_state;

    // Move history stack
    HeapAllocatedValueList<HistoryEntry<PieceC>, 6144> history; // ahh, but i hope it fulfils before I manages to find the absolute limit of a game
    // Move generation functions, but INTERNAL. (they're kind of long so i put them into a source
    // file) Pawns (fully extensively tested)
    template <Color c> void genEP(Movelist &mv) const;
    template <Color c> void genPawnDoubleMoves(Movelist &mv) const;
    template <Color c, bool capturesOnly = false> void genPawnSingleMoves(Movelist &mv) const;
    template <Color c, bool capturesOnly = false> void genKnightMoves(Movelist &list) const;
    template <Color c, bool capturesOnly = false> void genKingMoves(Movelist &mv) const;
    template <Color c, PieceType pt, bool capturesOnly = false> void genSlidingMoves(Movelist &mv) const;

    Bitboard _rook_pin;
    Bitboard _bishop_pin;
    Bitboard _checkers;
    Bitboard _check_mask;
    Bitboard _pin_mask;
    alignas(64) PieceC pieces_list[SQUARE_NB + 1] = { PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
                                                      PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE };

  public:
    // Legal move generation functions
    template <MoveGenType type = MoveGenType::ALL, Color c> __FORCEINLINE void legals(Movelist &out) const {
        if constexpr (type == MoveGenType::ALL) {
            // Simple cases
            genEP<c>(out);
            genPawnSingleMoves<c>(out);
            genPawnDoubleMoves<c>(out);
            genKnightMoves<c>(out);
            genKingMoves<c>(out);
            genSlidingMoves<c, BISHOP>(out);
            genSlidingMoves<c, ROOK>(out);
            genSlidingMoves<c, QUEEN>(out);
        } else if constexpr (type == MoveGenType::PAWN) {
            genEP<c>(out);
            genPawnDoubleMoves<c>(out);
            genPawnSingleMoves<c>(out);
        } else if constexpr (type == MoveGenType::KNIGHT) {
            genKnightMoves<c>(out);
        } else if constexpr (type == MoveGenType::KING) {
            genKingMoves<c>(out);
        } else if constexpr (type == MoveGenType::CAPTURE) {
            genEP<c>(out);
            genSlidingMoves<c, BISHOP, true>(out);
            genSlidingMoves<c, ROOK, true>(out);
            genSlidingMoves<c, QUEEN, true>(out);
            genKnightMoves<c, true>(out);
            genKingMoves<c, true>(out);
            genPawnSingleMoves<c, true>(out);
        }
    }
    // Legal move generation functions
    template <MoveGenType type = MoveGenType::ALL> __FORCEINLINE void legals(Movelist &out) const {
        const Color stm = sideToMove();       // Cache it
        ASSUME(stm == WHITE || stm == BLACK); // Now clearly no side effects

        switch (stm) {
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
        assert(history.size_ > 0 && "undoMove called with empty history");

        // Restore previous state from history
        assert(current_state.mv.is_ok() && "Corrupted history entry");
        assert(current_state.incr_pc[0] != at(current_state.mv.from()) && "Corrupted history entry");
        pieces_list[current_state.incr_sqs[0]] = current_state.incr_pc[0];
        pieces_list[current_state.incr_sqs[1]] = current_state.incr_pc[1];
        pieces_list[current_state.incr_sqs[2]] = current_state.incr_pc[2];
        pieces_list[current_state.incr_sqs[3]] = current_state.incr_pc[3];
        current_state = history.pop();
        if constexpr (RetAll) {
            return current_state;
        }
    }


    __FORCEINLINE void doNullMove() {
        history.push_back(current_state);
        current_state.turn = ~current_state.turn;
        current_state.hash ^= zobrist::RandomTurn;
        current_state.fullMoveNumber += (current_state.turn == WHITE);
        current_state.pliesFromNull = current_state.repetition = 0;
        current_state.mv = Move::null();
    }

    __FORCEINLINE Bitboard pieces() const { return occ(); }
    template <PieceType pt> __FORCEINLINE Bitboard pieces(Color c) const {
        ASSUME(c == WHITE || c == BLACK);
        if constexpr (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return current_state.pieces[pt] & current_state.occ[c];
    }
    template <Color c> __FORCEINLINE Bitboard pieces(PieceType pt) const {
        ASSUME(c == WHITE || c == BLACK);
        if (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return current_state.pieces[pt] & current_state.occ[c];
    }
    template <PieceType pt, Color c> inline Bitboard pieces() const {
        ASSUME(c == WHITE || c == BLACK);
        if constexpr (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return current_state.pieces[pt] & current_state.occ[c];
    }
    __FORCEINLINE Bitboard pieces(PieceType pt, Color c) const {
        ASSUME(c == WHITE || c == BLACK);
        // still branchless
        switch (pt) {
        case PIECE_TYPE_NB:
        case ALL_PIECES:
            return occ(c);
        default:
            return current_state.pieces[pt] & current_state.occ[c];
        }
    }
    __FORCEINLINE Bitboard pieces(PieceType pt) const {
        switch (int(pt)) {
        case PIECE_TYPE_NB:
        case ALL_PIECES:
            return occ();
        default:
            return current_state.pieces[pt];
        }
    }
    template <typename... PTypes, typename = std::enable_if_t<(std::is_integral_v<PTypes> && ...)>> [[nodiscard]] __FORCEINLINE Bitboard pieces(PTypes... ptypes) const { return (current_state.pieces[ptypes] | ...); }

    template <typename... PTypes, typename = std::enable_if_t<(std::is_integral_v<PTypes> && ...)>> [[nodiscard]] __FORCEINLINE Bitboard pieces(Color c, PTypes... ptypes) const { return (pieces(ptypes, c) | ...); }

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
    [[nodiscard]] __FORCEINLINE Bitboard attackers(Color color, Square square, Bitboard occupied) const {
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
    [[nodiscard]] __FORCEINLINE bool isAttacked(Square sq, Color by) const noexcept {
        const Bitboard occ_bb = occ();
        const Bitboard us_bb = occ(by);

        Bitboard diag_attackers = pieces(PieceType::BISHOP, by) | pieces(PieceType::QUEEN, by);
        Bitboard ortho_attackers = pieces(PieceType::ROOK, by) | pieces(PieceType::QUEEN, by);

        return (attacks::pawn(~by, sq) & pieces(PieceType::PAWN, by)) || (attacks::knight(sq) & pieces(PieceType::KNIGHT, by)) || (attacks::king(sq) & pieces(PieceType::KING, by)) || (attacks::bishop(sq, occ_bb) & diag_attackers & us_bb) || (attacks::rook(sq, occ_bb) & ortho_attackers & us_bb);
    }
    /**
     * @brief Checks if a square is attacked by the given color.
     * @param square
     * @param color
     * @param occupied
     * @return
     */
    [[nodiscard]] __FORCEINLINE bool isAttacked(Square sq, Color by, Bitboard occupied) const noexcept {
        const Bitboard diag_attackers = pieces(PieceType::BISHOP, by) | pieces(PieceType::QUEEN, by);
        const Bitboard ortho_attackers = pieces(PieceType::ROOK, by) | pieces(PieceType::QUEEN, by);

        return (attacks::pawn(~by, sq) & pieces(PieceType::PAWN, by)) || (attacks::knight(sq) & pieces(PieceType::KNIGHT, by)) || (attacks::king(sq) & pieces(PieceType::KING, by)) || (attacks::bishop(sq, occupied) & diag_attackers) || (attacks::rook(sq, occupied) & ortho_attackers);
    }

    [[nodiscard]] __FORCEINLINE Bitboard attackers(Color color, Square square) const { return attackers(color, square, occ()); }
    // Compile-time piece type and color, runtime square
    template <PieceType pt> __FORCEINLINE void placePiece(Square sq, Color c) {
        if constexpr (pt != NO_PIECE_TYPE) {
            Bitboard v = 1ULL << sq;
            current_state.pieces[pt] |= v;
            current_state.occ[c] |= v;
            pieces_list[sq] = make_piece<PieceC>(pt, c);
            current_state.hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][(int)pieces_list[sq]][sq];
            if constexpr (pt == KING)
                current_state.kings[c] = sq;
        }
    }

    template <PieceType pt> __FORCEINLINE void removePiece(Square sq, Color c) {
        if constexpr (pt != NO_PIECE_TYPE) {
            Bitboard v = ~(1ULL << sq);
            current_state.pieces[pt] &= v;
            current_state.occ[c] &= v;
            pieces_list[sq] = PieceC::NO_PIECE;
            current_state.hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][static_cast<int>(make_piece<PieceC>(pt, c))][sq];
            if constexpr (pt == KING)
                current_state.kings[c] = SQ_NONE;
        }
    }

    __FORCEINLINE void placePiece(PieceType pt, Square sq, Color c) {
        bool a = pt == KING;
        // if (pt == NO_PIECE_TYPE)
        //     return;
        Bitboard v = 1ULL << sq;
        current_state.pieces[pt] |= v;
        current_state.occ[c] |= v;
        pieces_list[sq] = make_piece<PieceC>(pt, c);
        current_state.hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][(int)pieces_list[sq]][sq];
        current_state.kings[c] = a ? sq : current_state.kings[c];
    }

    __FORCEINLINE void removePiece(PieceType pt, Square sq, Color c) {
        bool a = pt == KING;
        if (pt != NO_PIECE_TYPE) {
            Bitboard v = ~(1ULL << sq);
            current_state.pieces[pt] &= v;
            current_state.occ[c] &= v;
            pieces_list[sq] = PieceC::NO_PIECE;
            current_state.hash ^= zobrist::RandomPiece[enum_idx<PieceC>()][static_cast<int>(make_piece<PieceC>(pt, c))][sq];
            current_state.kings[c] = a ? SQ_NONE : current_state.kings[c];
        }
    }

    __FORCEINLINE Bitboard occ(Color c) const {
        ASSUME(c != COLOR_NB);
        return current_state.occ[c];
    }
    __FORCEINLINE Bitboard occ() const { return current_state.occ[0] | current_state.occ[1]; }
    PieceC piece_on(Square s) const {
#if !defined(_DEBUG) || defined(NDEBUG)
        return pieces_list[s];
#else
        auto p = pieces_list[s];
        auto p2 = piece_of(p);
        if (p2 == ALL_PIECES)
            return p;
        else {
            Bitboard bb = 1ULL << s;
            Bitboard _ = pieces(p2, color_of(p));
            Bitboard b = _ & bb;
#ifndef __EXCEPTIONS
            assert(b && "Inconsistient piece map");
#else
            if (!b)
                throw std::invalid_argument("Inconsistient piece map");
#endif
        }
        return p;
#endif
    }
    __FORCEINLINE Color sideToMove() const { return current_state.turn; }
    __FORCEINLINE uint64_t hash() const { return current_state.hash; }
    __FORCEINLINE uint64_t key() const { return current_state.hash; }
    __FORCEINLINE Color side_to_move() const { return current_state.turn; }
    __FORCEINLINE Square ep_square() const { return current_state.enPassant; }
    template <PieceType pt> __FORCEINLINE Square square(Color c) const { return Square(lsb(pieces<pt>(c))); }
    __FORCEINLINE Square kingSq(Color c) const { return current_state.kings[c]; }
    __FORCEINLINE Bitboard checkers() const { return _checkers; }
    __FORCEINLINE Bitboard pin_mask() const { return _pin_mask; }
    _Position(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    __FORCEINLINE bool isCapture(Move mv) const { return mv.type_of() == EN_PASSANT || (mv.type_of() != CASTLING && piece_on(mv.to_sq()) != PieceC::NO_PIECE); }
    __FORCEINLINE bool is_capture(Move mv) const { return isCapture(mv); }
    __FORCEINLINE bool is_zeroing(Move mv) const { return isCapture(mv) || type_of(at(mv.from_sq())) == PAWN; }
    std::string fen() const;
    __FORCEINLINE uint8_t halfmoveClock() const { return current_state.halfMoveClock; }
    __FORCEINLINE uint16_t fullmoveNumber() const { return current_state.fullMoveNumber; }
    __FORCEINLINE uint8_t rule50_count() const { return current_state.halfMoveClock; }
    __FORCEINLINE CastlingRights castlingRights(Color c) const { return current_state.castlingRights & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING); }
    __FORCEINLINE CastlingRights castlingRights() const { return current_state.castlingRights; }
    __FORCEINLINE bool is_castling(Move mv) const { return (mv.type_of() & CASTLING) != 0; }
    __FORCEINLINE const HistoryEntry<PieceC> &state() const { return current_state; }
    uint64_t zobrist() const;
    __FORCEINLINE PieceC piece_at(Square sq) const { return piece_on(sq); }
    __FORCEINLINE PieceC at(Square sq) const { return piece_at(sq); }
    __FORCEINLINE Square enpassantSq() const { return ep_square(); }
    CastlingRights clean_castling_rights() const;
    void setFEN(const std::string &str);
    __FORCEINLINE void set_fen(const std::string &str) { setFEN(str); }
    Move parse_uci(std::string) const;
    Move push_uci(std::string);
    Square _valid_ep_square() const;
    template <PieceType pt> __FORCEINLINE int count() const { return popcount(pieces(pt)); }
    template <PieceType pt, Color c> __FORCEINLINE int count() const { return popcount(pieces<pt, c>()); }
    template <PieceType pt> __FORCEINLINE int count(Color c) const { return popcount(pieces<pt>(c)); }
    __FORCEINLINE int count(PieceType pt, Color c) const { return popcount(pieces(pt, c)); }
    __FORCEINLINE int ply() const { return 2 * (current_state.fullMoveNumber - 1) + (sideToMove() == BLACK); }
    bool is_insufficient_material(Color c) const;
    __FORCEINLINE bool isInsufficientMaterial(Color c) const { return is_insufficient_material(c); }
    __FORCEINLINE bool hasInsufficientMaterial(Color c) const { return is_insufficient_material(c); }
    __FORCEINLINE bool has_insufficient_material(Color c) const { return is_insufficient_material(c); }
    __FORCEINLINE bool is_insufficient_material() const { return has_insufficient_material(WHITE) && has_insufficient_material(BLACK); }
    __FORCEINLINE bool inCheck() const { return checkers() != 0; }
    __FORCEINLINE bool is_check() const { return checkers() != 0; }
    __FORCEINLINE bool has_castling_rights(Color c) const { return castlingRights(c) != 0; }
    __FORCEINLINE bool has_kingside_castling_rights(Color c) const { return (castlingRights(c) & KING_SIDE) != 0; }
    __FORCEINLINE bool has_queenside_castling_rights(Color c) const { return (castlingRights(c) & QUEEN_SIDE) != 0; }
    // Return true if a position repeats once earlier but strictly
    // after the root, or repeats twice before or at the root.
    __FORCEINLINE bool is_repetition(int ply) const { return current_state.repetition && current_state.repetition < ply; }
    // Test if it's draw of 75 move rule (that forces everyone to draw). Excludes checkmates, of course!
    __FORCEINLINE bool is_draw(int ply) const { return rule50_count() > 99 || is_repetition(ply); }
    // Tests whether there has been at least one repetition
    // of positions since the last capture or pawn move.
    __FORCEINLINE bool has_repeated() const {
        auto idx = history.size() - 1;
        int end = std::min(rule50_count(), current_state.pliesFromNull);
        while (end-- >= 4) {
            if (history[idx].repetition)
                return true;

            idx--;
        }
        return false;
    }
    __FORCEINLINE bool _is_halfmoves(int n) { return rule50_count() >= n; }
    __FORCEINLINE bool is_seventyfive_moves(int n) { return _is_halfmoves(150); }
    __FORCEINLINE bool is_fifty_moves(int n) { return _is_halfmoves(150); }
    __FORCEINLINE bool is_fivefold_repetition() { return is_repetition(5); }
    __FORCEINLINE bool is_attacked_by(Color color, Square sq, Bitboard occupied = 0) const {
        Bitboard occ_bb = occupied ? occupied : this->occ();
        return attackers_mask(color, sq, occ_bb) != 0;
    }

    __FORCEINLINE bool was_into_check() const {
        bool atk = false;
        Bitboard bb = pieces<KING>(~sideToMove());
        while (!atk && bb) {
            atk |= isAttacked((Square)pop_lsb(bb), sideToMove());
        }
        return atk != 0;
    }
    __FORCEINLINE Bitboard attackers_mask(Color color, Square square, Bitboard occupied) const {
        auto queens = pieces<QUEEN>(color);

        // using the fact that if we can attack PieceType from square, they can attack us back
        auto atks = (attacks::pawn(~color, square) & pieces<PAWN>(color));
        atks |= (attacks::knight(square) & pieces<KNIGHT>(color));
        atks |= (attacks::bishop(square, occupied) & (pieces<BISHOP>(color) | queens));
        atks |= (attacks::rook(square, occupied) & (pieces<ROOK>(color) | queens));
        atks |= (attacks::king(square) & pieces<KING>(color));

        return atks & occ(color);
    }
    __FORCEINLINE bool _attacked_for_king(Bitboard path, Bitboard occupied) const {
        Bitboard b = 0;
        while (!b && path) {
            b |= attackers_mask(~sideToMove(), static_cast<Square>(pop_lsb(path)), occupied);
        }
        return b != 0;
    }
    // Material-only key (note: Zobrist=Zpieces^Zep^Zcastling^Zturn, we just XORs the remaining, it's trivial)
    __FORCEINLINE Key material_key() const { return hash() ^ (zobrist::RandomTurn * ~sideToMove()) ^ (zobrist::RandomCastle[castlingRights()]) ^ (zobrist::RandomEP[ep_square() == SQ_NONE ? file_of(ep_square()) : FILE_NB]); }
    template <bool Strict = false> bool is_valid() const;

  private:
    template <PieceType pt> [[nodiscard]] __FORCEINLINE Bitboard pinMask(Color c, Square sq) const {
        static_assert(pt == BISHOP || pt == ROOK, "Only bishop or rook allowed!");
        Bitboard occ_opp = occ(~sideToMove());
        Bitboard occ_us = occ(sideToMove());
        Bitboard opp_sliders;
        opp_sliders = (pieces<pt>(~c) | pieces(QUEEN, ~c)) & occ_opp;

        auto pt_attacks = attacks::slider<pt>(sq, occ_opp) & opp_sliders;

        Bitboard pin = 0ull;

        while (pt_attacks) {
            const auto possible_pin = movegen::between(sq, Square(pop_lsb(pt_attacks)));
            Bitboard tmp = possible_pin & occ_us;
            bool v = popcount(tmp) == 1;
            pin |= v ? possible_pin : 0;
        }

        return pin;
    }
    void refresh_attacks();

  public:
    /** \brief Initializes the board as empty.
     *
     * \param nothing
     * \param nothing
     * \return nothing
     *
     */
    __FORCEINLINE _Position(HistoryEntry<PieceC> state) {
        // compatible!
        current_state = state;
        refresh_attacks();
    }
    __FORCEINLINE _Position(const _Position &other)
        : current_state(other.current_state), history(other.history) // calls HeapAllocatedValueList's copy constructor
    {
        refresh_attacks();
    }

    template <typename T, std::enable_if_t<(std::is_same_v<T, PolyglotPiece> || std::is_same_v<T, EnginePiece>) && !std::is_same_v<T, PieceC>, int> = 0> [[deprecated("Incompatible piece method used, you shouldn't call this")]] __FORCEINLINE _Position(HistoryEntry<T> state) {
        // incompatible!
        current_state = HistoryEntry<PieceC>();
        current_state.turn = state.turn;
        current_state.castlingRights = state.castlingRights;
        current_state.enPassant = state.enPassant;
        current_state.halfMoveClock = state.halfMoveClock;
        current_state.fullMoveNumber = state.fullMoveNumber;
        current_state.hash = state.hash;
#pragma unroll
        for (int s = 0; s < 64; ++s) {
            current_state.pieces_list[s] = make_piece<PieceC>(type_of(state.pieces_list[s]), color_of(state.pieces_list[s]));
        }
        refresh_attacks();
    }
};
using Position = _Position<EnginePiece>; // for some fun because I HATE HARDCODING
}; // namespace chess
