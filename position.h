#pragma once
#include "attacks.h"
#include "bitboard.h"
#include "types.h"
#include "zobrist.h"
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <cstdlib>
#include "printers.h"
#if defined(_MSC_VER)
#include <malloc.h>
#endif

namespace chess {
namespace _chess{
inline void *aligned_alloc(size_t alignment, size_t size) {
#if defined(_MSC_VER)
    return _aligned_malloc(size, alignment);
#elif defined(__APPLE__) || defined(__linux__)
    void *ptr = nullptr;
    if (posix_memalign(&ptr, alignment, size) != 0)
        return nullptr;
    return ptr;
#else
    return std::aligned_alloc(alignment, size);
#endif
}

inline void aligned_free(void *ptr) {
#if defined(_MSC_VER)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}
} // namespace _chess

template <typename Piece> struct alignas(64) HistoryEntry {
    // Bitboards for each piece type (white and black)
    Bitboard pieces[7];
    Bitboard occ[COLOR_NB];
    Color turn;                    // true if white to move
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
    // implementation-specific implementations goes here
};

template <typename T, std::size_t MaxSize> class HeapAllocatedValueList {
  private:
    constexpr static int ALIGNMENT = 64;
  public:
    using size_type = std::size_t;
    inline HeapAllocatedValueList() {
        values_ = reinterpret_cast<T *>(_chess::aligned_alloc(ALIGNMENT, MaxSize * sizeof(T)));
        assert(values_);
    }
    inline ~HeapAllocatedValueList() { _chess::aligned_free(values_); }

    HeapAllocatedValueList(const HeapAllocatedValueList &other) : size_(other.size_) {
        values_ = reinterpret_cast<T *>(_chess::aligned_alloc(ALIGNMENT, MaxSize * sizeof(T)));
        assert(values_);
        std::copy(other.values_, other.values_ + size_, values_);
    }

    HeapAllocatedValueList &operator=(const HeapAllocatedValueList &other) {
        if (this != &other) {
            // Allocate new memory and copy
            T *new_values = reinterpret_cast<T *>(_chess::aligned_alloc(ALIGNMENT, MaxSize * sizeof(T)));
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
        values_ = reinterpret_cast<T *>(_chess::aligned_alloc(ALIGNMENT, MaxSize * sizeof(T)));
        assert(values_);
        std::copy(other.values_, other.values_ + size_, values_);
    }

    HeapAllocatedValueList &operator=(HeapAllocatedValueList &&other) noexcept {
        if (this != &other) {
            free(values_);
            values_ = reinterpret_cast<T *>(_chess::aligned_alloc(ALIGNMENT, MaxSize * sizeof(T)));
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
    inline const T& pop() {
        assert(size_ > 0);
        return values_[--size_]; // always safe due to mask
    }

    inline void pop_back() { size_ -= (size_ > 0) ? 1 : 0; }

    inline const T& front() const { return (size_ > 0) ? values_[0] : T{}; }

    inline T &operator[](int index) {
        assert(index < MaxSize); // relax the conditions, it's BRANCHLESS so forgive
        size_type i = static_cast<size_type>(index);
        return values_[i];
    }

    inline const T *begin() const { return values_; }
    inline T *data() const { return values_; }
    inline const T *end() const { return values_ + size_; }
    size_type size_ = 0;

  private:
    T *values_;
};

enum class MoveGenType : uint8_t { ALL, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, CAPTURE };
template <typename PieceC = EnginePiece, typename = std::enable_if_t<std::is_same_v<PieceC, EnginePiece> || std::is_same_v<PieceC, PolyglotPiece>>> class _Position {
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
    alignas(64) PieceC pieces_list[SQUARE_NB] = { PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE,
                                                  PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE, PieceC::NO_PIECE };
    
  public:
    // Legal move generation functions
    template <MoveGenType type = MoveGenType::ALL, Color c> inline void legals(Movelist &out) const {
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
    template <MoveGenType type = MoveGenType::ALL> inline void legals(Movelist &out) const {
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
    template <bool Strict=true>
    void doMove(const Move &move);
    template <bool RetAll = false>
    inline auto undoMove() -> std::conditional_t<RetAll, HistoryEntry<PieceC> &, void> {
        // Save only the move (we'll restore the full state next)
        const Move move = current_state.mv;

        // Make sure there's something to pop
        assert(history.size_ > 0 && "undoMove called with empty history");

        // Restore previous state from history
        current_state = history.pop();

        // After restore, sideToMove() is the mover
        const Color mover = sideToMove();
        // Squares to update
        Square sqs[4];
        int n_sqs = 0;
        sqs[n_sqs++] = move.from_sq();
        sqs[n_sqs++] = move.to_sq();
        switch(auto type = move.type_of()){
            case CASTLING: {
                bool is_king_side = file_of(move.to_sq()) > file_of(move.from_sq());
                sqs[n_sqs++] = relative_square(mover, is_king_side ? SQ_G1 : SQ_C1); // king to
                sqs[n_sqs++] = relative_square(mover, is_king_side ? SQ_F1 : SQ_D1); // rook to
                break;
            }
            case EN_PASSANT: {
                sqs[n_sqs++] = static_cast<Square>(static_cast<int>(move.to_sq()) - pawn_push(mover));
                break;
            }
            default:
                break;
        }
        // Update all squares
        ASSUME(n_sqs <= 4);
        for (int i = 0; i < n_sqs; ++i) {

            const Square sq = sqs[i];
            Bitboard sq_bb = 1ULL << sq;

            if ((occ()&sq_bb)==0) {
                pieces_list[sq] = PieceC::NO_PIECE;
                continue;
            }

            // Determine color first
            Color pc = (current_state.occ[WHITE] & sq_bb) ? WHITE : BLACK;

            const Bitboard *p = current_state.pieces;
            // clang-format off
            
            PieceType pt = (p[PAWN] & sq_bb)   ? PAWN :
                           (p[KNIGHT] & sq_bb) ? KNIGHT :
                           (p[BISHOP] & sq_bb) ? BISHOP :
                           (p[ROOK] & sq_bb)   ? ROOK :
                           (p[QUEEN] & sq_bb)  ? QUEEN :
                           (p[KING] & sq_bb)   ? KING : PAWN; // fallback, though should never happen
            // clang-format on
            pieces_list[sq] = make_piece<PieceC>(pt, pc);
        }

        if constexpr (RetAll) {
            return current_state;
        }
    }

    inline void doNullMove() {
        history.push_back(current_state);
        current_state.turn = ~current_state.turn;
        current_state.hash ^= zobrist::RandomTurn;
        current_state.fullMoveNumber += (current_state.turn == WHITE);
        current_state.pliesFromNull = current_state.repetition = 0;
        current_state.mv = Move::null();
    }

    inline Bitboard pieces() const { return occ(); }
    template <PieceType pt> inline Bitboard pieces(Color c) const {
        ASSUME(c == WHITE || c == BLACK);
        if constexpr (pt == PIECE_TYPE_NB || pt == ALL_PIECES)
            return occ(c);
        return current_state.pieces[pt] & current_state.occ[c];
    }
    template <Color c> inline Bitboard pieces(PieceType pt) const {
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
    inline Bitboard pieces(PieceType pt, Color c) const {
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
    inline Bitboard pieces(PieceType pt) const {
        switch(int(pt)){
        case PIECE_TYPE_NB:
        case ALL_PIECES:
            return occ();
        default:
            return current_state.pieces[pt];
        }
    }
    template <typename... PTypes, typename = std::enable_if_t<(std::is_integral_v<PTypes> && ...)>> [[nodiscard]] inline Bitboard pieces(PTypes... ptypes) const { return (current_state.pieces[ptypes] | ...); }

    template <typename... PTypes, typename = std::enable_if_t<(std::is_integral_v<PTypes> && ...)>> [[nodiscard]] inline Bitboard pieces(Color c, PTypes... ptypes) const { return (pieces(ptypes, c) | ...); }

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

        return (attacks::pawn(~by, sq) & pieces(PieceType::PAWN, by)) || (attacks::knight(sq) & pieces(PieceType::KNIGHT, by)) || (attacks::king(sq) & pieces(PieceType::KING, by)) || (attacks::bishop(sq, occ_bb) & diag_attackers & us_bb) || (attacks::rook(sq, occ_bb) & ortho_attackers & us_bb);
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

        return (attacks::pawn(~by, sq) & pieces(PieceType::PAWN, by)) || (attacks::knight(sq) & pieces(PieceType::KNIGHT, by)) || (attacks::king(sq) & pieces(PieceType::KING, by)) || (attacks::bishop(sq, occupied) & diag_attackers) || (attacks::rook(sq, occupied) & ortho_attackers);
    }

    [[nodiscard]] inline Bitboard attackers(Color color, Square square) const { return attackers(color, square, occ()); }
    // Compile-time piece type and color, runtime square
    template <PieceType pt> inline void placePiece(Square sq, Color c) {
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

    template <PieceType pt> inline void removePiece(Square sq, Color c) {
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

    inline void placePiece(PieceType pt, Square sq, Color c) {
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

    inline void removePiece(PieceType pt, Square sq, Color c) {
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

    inline Bitboard occ(Color c) const {
        ASSUME(c != COLOR_NB);
        return current_state.occ[c];
    }
    inline Bitboard occ() const { return current_state.occ[0] | current_state.occ[1]; }
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
    inline Color sideToMove() const { return current_state.turn; }
    inline uint64_t hash() const { return current_state.hash; }
    inline uint64_t key() const { return current_state.hash; }
    inline Color side_to_move() const { return current_state.turn; }
    inline Square ep_square() const { return current_state.enPassant; }
    template <PieceType pt> inline Square square(Color c) const { return Square(lsb(pieces<pt>(c))); }
    inline Square kingSq(Color c) const { return current_state.kings[c]; }
    inline Bitboard checkers() const { return _checkers; }
    inline Bitboard pin_mask() const { return _pin_mask; }
    _Position(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    inline bool isCapture(Move mv) const { return mv.type_of() == EN_PASSANT || (mv.type_of() != CASTLING && piece_on(mv.to_sq()) != PieceC::NO_PIECE); }

    std::string fen() const;
    inline uint8_t halfmoveClock() const { return current_state.halfMoveClock; }
    inline uint16_t fullmoveNumber() const { return current_state.fullMoveNumber; }
    inline uint8_t rule50_count() const { return current_state.halfMoveClock; }
    inline CastlingRights castlingRights(Color c) const { return current_state.castlingRights & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING); }
    inline CastlingRights castlingRights() const { return current_state.castlingRights; }
    inline const HistoryEntry<PieceC> &state() const { return current_state; }
    uint64_t zobrist() const;
    inline PieceC piece_at(Square sq) const { return piece_on(sq); }
    inline PieceC at(Square sq) const { return piece_at(sq); }
    inline Square enpassantSq() const { return ep_square(); }
    CastlingRights clean_castling_rights() const;
    void setFEN(const std::string &str);
    inline void set_fen(const std::string &str) { setFEN(str); }
    Move parse_uci(std::string) const;
    Move push_uci(std::string);
    Square _valid_ep_square() const;
    template<PieceType pt>
    inline int count() const{ return popcount(pieces(pt));}
    template<PieceType pt, Color c>
    inline int count() const{ return popcount(pieces<pt, c>());}
    template<PieceType pt>
    inline int count(Color c) const{ return popcount(pieces<pt>(c));}
    inline int count(PieceType pt, Color c) const { return popcount(pieces(pt, c)); }
    // Return true if a position repeats once earlier but strictly
    // after the root, or repeats twice before or at the root.
    inline bool is_repetition(int ply) const { return current_state.repetition && current_state.repetition < ply; }
    // Test if it's draw of 75 move rule (that forces everyone to draw). Excludes checkmates, of course!
    inline bool is_draw(int ply) const { return rule50_count() > 99 || is_repetition(ply); }
    // Tests whether there has been at least one repetition
    // of positions since the last capture or pawn move.
    inline bool has_repeated() const {
        auto stc = &history[history.size()-1];
        int end = std::min(rule50_count(), current_state.pliesFromNull);
        while (end-- >= 4) {
            if (stc->repetition)
                return true;

            stc--;
        }
        return false;
    }

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
        return atk;
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
        return !!b;
    }
    template <bool Strict = false> bool is_valid() const;

  private:
    template <PieceType pt> [[nodiscard]] inline Bitboard pinMask(Color c, Square sq) const {
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
    inline _Position(HistoryEntry<PieceC> state) {
        // compatible!
        current_state = state;
        refresh_attacks();
    }
    _Position(const _Position &other)
        : current_state(other.current_state), history(other.history) // calls HeapAllocatedValueList's copy constructor
    {}

    template <typename T, std::enable_if_t<(std::is_same_v<T, PolyglotPiece> || std::is_same_v<T, EnginePiece>) && !std::is_same_v<T, PieceC>, int> = 0> [[deprecated("Incompatible piece method used, you shouldn't call this")]] inline _Position(HistoryEntry<T> state) {
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
