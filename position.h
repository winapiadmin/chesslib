#pragma once
#include "attacks.h"
#include "bitboard.h"
#include "types.h"
#include "zobrist.h"
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
namespace chess {
#pragma pack(push, 1)
template <typename Piece> struct HistoryEntry {
    // Bitboards for each piece type (white and black)
    alignas(8) Bitboard pieces[7];
    alignas(8) Bitboard occ[COLOR_NB];
    Square kings[COLOR_NB] = { SQ_NONE };
    Color turn;                    // true if white to move
    CastlingRights castlingRights; // Castling rights bitmask
    Piece pieces_list[SQUARE_NB] = {
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE,
        Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE, Piece::NO_PIECE
    };
    // Game state information
    Square enPassant = SQ_NONE; // En passant target square
    uint8_t halfMoveClock;      // Half-move clock for 50/75-move rule
    int fullMoveNumber;         // Full-move number (starts at 1)
    bool epIncluded;
    Bitboard _pin_mask;
    Move mv;
    uint64_t hash;
    Bitboard _rook_pin;
    Bitboard _bishop_pin;
    Bitboard checkers;
    Bitboard check_mask;
    // implementation-specific implementations goes here
};
#pragma pack(pop)
template <typename T, std::size_t MaxSize> class HeapAllocatedValueList {
  public:
    using size_type = std::size_t;
    inline HeapAllocatedValueList() {
        values_ = reinterpret_cast<T *>(calloc(MaxSize, sizeof(T)));
        assert(values_);
    }
    inline ~HeapAllocatedValueList() { free(values_); }

    HeapAllocatedValueList(const HeapAllocatedValueList &other) : size_(other.size_) {
        values_ = reinterpret_cast<T *>(calloc(MaxSize, sizeof(T)));
        assert(values_);
        std::copy(other.values_, other.values_ + size_, values_);
    }

    HeapAllocatedValueList &operator=(const HeapAllocatedValueList &other) {
        if (this != &other) {
            // Allocate new memory and copy
            T *new_values = reinterpret_cast<T *>(calloc(MaxSize, sizeof(T)));
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
        values_ = reinterpret_cast<T *>(calloc(MaxSize, sizeof(T)));
        assert(values_);
        std::copy(other.values_, other.values_ + size_, values_);
    }

    HeapAllocatedValueList &operator=(HeapAllocatedValueList &&other) noexcept {
        if (this != &other) {
            free(values_);
            values_ = reinterpret_cast<T *>(calloc(MaxSize, sizeof(T)));
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
    inline T pop() {
        assert(size_ > 0);
        return values_[--size_]; // always safe due to mask
    }

    inline void pop_back() { size_ -= (size_ > 0) ? 1 : 0; }

    inline T front() const { return (size_ > 0) ? values_[0] : T{}; }

    inline T &operator[](int index) {
        assert(index < MaxSize); // relax the conditions, it's BRANCHLESS so forgive
        size_type i = static_cast<size_type>(index);
        return values_[i];
    }

    inline const T *begin() const { return values_; }
    inline const T *end() const { return values_ + size_; }
    size_type size_ = 0;

  private:
    T *values_;
};
enum class MoveGenType : uint8_t { ALL, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, CAPTURE };
template <typename PieceC = EnginePiece,
          typename = std::enable_if_t<std::is_same_v<PieceC, EnginePiece> ||
                                      std::is_same_v<PieceC, PolyglotPiece>>>
class _Position {
  private:
    static constexpr const std::array<uint64_t, 64>(*RandomPiece) =
        std::is_same_v<PieceC, PolyglotPiece> ? zobrist::RandomPiece
                                              : zobrist::RandomPiece_EnginePiece;

    HistoryEntry<PieceC> current_state;
    // Move history stack
    HeapAllocatedValueList<HistoryEntry<PieceC>, 8192>
        history; // ahh, but i hope it fulfils before I manages to find the absolute limit of a game
    // Move generation functions, but INTERNAL. (they're kind of long so i put them into a source
    // file) Pawns (fully extensively tested)
    template <Color c> void genEP(Movelist &mv) const;
    template <Color c> void genPawnDoubleMoves(Movelist &mv) const;
    template <Color c, bool capturesOnly = false> void genPawnSingleMoves(Movelist &mv) const;
    template <Color c, bool capturesOnly = false> inline void genKnightMoves(Movelist &list) const {
        Bitboard knights = pieces<KNIGHT, c>() & ~current_state._pin_mask; // yes, unconditionally.
        while (knights) {
            Square x = static_cast<Square>(pop_lsb(knights));
            Bitboard moves = attacks::knight(x) & ~occ(c);
            moves &= current_state.check_mask;
            if constexpr (capturesOnly)
                moves &= occ(~c);
            while (moves) {
                Square y = static_cast<Square>(pop_lsb(moves));
                list.push_back(Move(x, y));
            }
        }
    }
    template <Color c, bool capturesOnly = false> void genKingMoves(Movelist &mv) const;
    template <Color c, PieceType pt, bool capturesOnly = false>
    void genSlidingMoves(Movelist &mv) const;

  public:
    // Legal move generation functions
    template <MoveGenType type = MoveGenType::ALL, Color c>
    inline void legals(Movelist &out) const {
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

    void doMove(const Move &move);
    template <bool RetAll = false> inline auto undoMove() {
        current_state = history.pop();

        if constexpr (RetAll) {
            return current_state;
        }
    }

    inline void doNullMove() {
        history.push_back(current_state);
        current_state.turn = ~current_state.turn;
        current_state.hash ^= zobrist::RandomTurn;
        current_state.fullMoveNumber += (current_state.turn == WHITE);
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

    template <typename... PTypes, typename = std::enable_if_t<(std::is_integral_v<PTypes> && ...)>>
    [[nodiscard]] inline Bitboard pieces(PTypes... ptypes) const {
        return (pieces(ptypes) | ...);
    }

    template <typename... PTypes, typename = std::enable_if_t<(std::is_integral_v<PTypes> && ...)>>
    [[nodiscard]] inline Bitboard pieces(Color c, PTypes... ptypes) const {
        return (pieces(ptypes, c) | ...);
    }

    /**
     * @brief Returns the origin squares of pieces of a given color attacking a target square
     * @param board
     * @param color Attacker Color
     * @param square Attacked Square
     * @return
     */
    /** Sample code to not make myself (and others) confused:
    ```c++
    if (attackers(BLACK, E4)) {
        std::cout << "E4 is under attack by black!";
    }
    ```
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
    [[nodiscard]] inline Bitboard attackers(Color color, Square square) const {
        return attackers(color, square, occ());
    }
    // Compile-time piece type and color, runtime square
    template <PieceType pt> inline void placePiece(Square s, Color c) {
        if constexpr (pt != NO_PIECE_TYPE) {
            current_state.pieces[pt] |= 1ULL << s;
            current_state.occ[c] |= 1ULL << s;
            current_state.pieces_list[s] = make_piece<PieceC>(pt, c);
            current_state.hash ^= RandomPiece[static_cast<int>(current_state.pieces_list[s])][s];
            if constexpr (pt == KING)
                current_state.kings[c] = s;
        }
    }

    template <PieceType pt> inline void removePiece(Square s, Color c) {
        if constexpr (pt != NO_PIECE_TYPE) {
            current_state.pieces[pt] &= ~(1ULL << s);
            current_state.occ[c] &= ~(1ULL << s);
            current_state.pieces_list[s] = PieceC::NO_PIECE;
            current_state.hash ^= RandomPiece[static_cast<int>(make_piece<PieceC>(pt, c))][s];
            if constexpr (pt == KING)
                current_state.kings[c] = SQ_NONE;
        }
    }

    inline void placePiece(PieceType pt, Square s, Color c) {
        bool a = pt == KING;
        // if (pt == NO_PIECE_TYPE)
        //     return;
        Bitboard v = 1ULL << s;
        current_state.pieces[pt] |= v;
        current_state.occ[c] |= v;
        current_state.pieces_list[s] = make_piece<PieceC>(pt, c);
        current_state.hash ^= RandomPiece[static_cast<int>(current_state.pieces_list[s])][s];
        current_state.kings[c] = a ? s : current_state.kings[c];
    }

    inline void removePiece(PieceType pt, Square s, Color c) {
        bool a = pt == KING;
        if (pt == NO_PIECE_TYPE)
            return;
        Bitboard v = ~(1ULL << s);
        current_state.pieces[pt] &= v;
        current_state.occ[c] &= v;
        current_state.pieces_list[s] = PieceC::NO_PIECE;
        current_state.hash ^= RandomPiece[static_cast<int>(make_piece<PieceC>(pt, c))][s];
        current_state.kings[c] = a ? SQ_NONE : current_state.kings[c];
    }

    inline Bitboard occ(Color c) const {
        ASSUME(c != COLOR_NB);
        return current_state.occ[c];
    }
    inline Bitboard occ() const { return current_state.occ[0] | current_state.occ[1]; }
    PieceC piece_on(Square s) const {
#if !defined(_DEBUG) || defined(NDEBUG)
        return current_state.pieces_list[s];
#else
        auto p = current_state.pieces_list[s];
        auto p2 = piece_of(p);
        if (p2 == ALL_PIECES)
            return p;
        else {
            Bitboard bb = 1ULL << s;
            Bitboard _ = pieces(p2, color_of(p));
            Bitboard b = _ & bb;
            assert(b);
        }
        return p;
#endif
    }
    inline Color sideToMove() const { return current_state.turn; }
    inline uint64_t hash() const { return current_state.hash; }
    inline uint64_t key() const { return current_state.hash; }
    inline Color side_to_move() const { return current_state.turn; }
    inline Square ep_square() const { return current_state.enPassant; }
    template <PieceType pt> inline Square square(Color c) const {
        return Square(lsb(pieces<pt>(c)));
    }
    inline Square kingSq(Color c) const { return current_state.kings[c]; }
    inline Bitboard checkers() const { return current_state.checkers; }
    _Position(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    inline bool isCapture(Move mv) const {
        return mv.type_of() == EN_PASSANT ||
               (mv.type_of() != CASTLING && piece_on(mv.to_sq()) != PieceC::NO_PIECE);
    }

    std::string fen() const;
    inline int halfmoveClock() const { return current_state.halfMoveClock; }
    inline int fullmoveNumber() const { return current_state.fullMoveNumber; }
    inline int rule50_count() const { return current_state.halfMoveClock; }
    CastlingRights castlingRights(Color c) const {
        return current_state.castlingRights & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING);
    }
    CastlingRights castlingRights() const { return current_state.castlingRights; }
    inline const HistoryEntry<PieceC> &state() const { return current_state; }
    uint64_t zobrist() const;
    inline PieceC piece_at(Square sq) const { return piece_on(sq); }
    inline PieceC at(Square sq) const { return piece_at(sq); }
    inline Square enpassantSq() const { return ep_square(); }
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
    void setFEN(const std::string &str);
    inline void set_fen(const std::string &str) { setFEN(str); }
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
        : current_state(other.current_state),
          history(other.history) // calls HeapAllocatedValueList's copy constructor
    {}

    template <
        typename T,
        std::enable_if_t<(std::is_same_v<T, PolyglotPiece> || std::is_same_v<T, EnginePiece>) &&
                             !std::is_same_v<T, PieceC>,
                         int> = 0>
    [[deprecated("Incompatible piece method used, you shouldn't call this")]] inline _Position(
        HistoryEntry<T> state) {
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
            current_state.pieces_list[s] =
                make_piece<PieceC>(type_of(state.pieces_list[s]), color_of(state.pieces_list[s]));
        }
        refresh_attacks();
    }
    Move parse_uci(std::string) const;
    Move push_uci(std::string);
};
using Position = _Position<EnginePiece>; // for some fun because I HATE HARDCODING
}; // namespace chess
