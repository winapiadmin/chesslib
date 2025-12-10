#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include "fwd_decl.h"
#if defined(_MSC_VER)
#define UNREACHABLE() __assume(false)
#elif defined(__clang__) || defined(__GNUC__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(__cpp_lib_unreachable)
#define UNREACHABLE() std::unreachable()
#else
#define UNREACHABLE() assert(0)
#endif
#if defined(_MSC_VER)
#define ASSUME(expr) __assume(expr)
#elif defined(__clang__)
#define ASSUME(expr) __builtin_assume(expr)
#elif defined(__GNUC__)
#if __has_cpp_attribute(assume)
#define ASSUME(expr) [[assume(expr)]]
#elif __GNUC__ >= 13
#define ASSUME(cond) __attribute__((assume(cond)))
#else
#define ASSUME(cond)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    do {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
        if (!(cond))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
            UNREACHABLE();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    } while (0)
#endif
#else
#define ASSUME(cond)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    do {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
        if (!(cond))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
            UNREACHABLE();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    } while (0)
#endif

#ifdef _MSC_VER
#define __FORCEINLINE inline __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define __FORCEINLINE inline __attribute__((always_inline))
#else
#define __FORCEINLINE inline
#endif
constexpr bool is_constant_evaluated() {
#if __cpp_if_consteval >= 202106L
    if consteval {
        return true;
    }
    // both MSVC (non-comformant __cplusplus) and by-default _MSVC_LANG and other compiles with
    // conformant __cplusplus
#elif __cplusplus >= 202002L || _MSVC_LANG >= 202002L
    if (std::is_constant_evaluated())
        return true;
#elif defined(__GNUC__) // defined for both GCC and clang
    if (__builtin_is_constant_evaluated())
        return true;
#elif _MSC_VER >= 1925
    if (__builtin_is_constant_evaluated())
        return true;
#else
#error "NAWH we don't think we can detect compile time in this compiler";
#endif
    return false;
}
namespace chess {
using Bitboard = uint64_t;
using Key = uint64_t;
// clang-format off
enum Square : int8_t {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE,

    SQUARE_ZERO = 0,
    SQUARE_NB = 64
};
enum File : int8_t { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB };

enum Rank : int8_t { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB };
constexpr bool is_valid(const Rank r, const File f) { return 0 <= r && r <= 7 && 0 <= f && f <= 7; } // unsigned already fix signedness
constexpr bool is_valid(const Square s) { return 0 <= s && s < 64; }
constexpr File file_of(Square s) {
    ASSUME(0 <= s && s < 64);
    return File(s & 7);
}

constexpr Rank rank_of(Square s) {
    ASSUME(0 <= s && s < 64);
    return Rank(s >> 3);
}
constexpr Square make_sq(Rank r, File f) {
    ASSUME(0 <= r && r <= 7 && 0 <= f && f <= 7);
    return static_cast<Square>(static_cast<uint8_t>(r * 8 + f));
}
constexpr Square make_sq(File f, Rank r) {
    ASSUME(0 <= r && r <= 7 && 0 <= f && f <= 7);
    return static_cast<Square>(static_cast<uint8_t>(r * 8 + f));
}
enum Color : uint8_t { WHITE = 0, BLACK = 1, COLOR_NB = 2 };
// Toggle color
constexpr Color operator~(Color c) { return Color(c ^ BLACK); }
enum PieceType : std::int8_t { NO_PIECE_TYPE = 0, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, ALL_PIECES = 0, PIECE_TYPE_NB = 8 };
enum Direction : int8_t {
    NORTH = 8,
    EAST = 1,
    SOUTH = -NORTH,
    WEST = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST,
    DIR_NONE = 0
};
// clang-format on
inline constexpr Square relative_square(Color c, Square s) { return Square(s ^ (c * 56)); }

inline constexpr Rank relative_rank(Color c, Rank r) { return Rank(r ^ (c * 7)); }

inline constexpr Rank relative_rank(Color c, Square s) { return relative_rank(c, rank_of(s)); }

inline constexpr Direction relative_direction(Color c, Direction d) { return static_cast<Direction>(c == WHITE ? d : -d); }

inline constexpr Direction pawn_push(Color c) { return c == WHITE ? NORTH : SOUTH; }

#define ENABLE_INCR_OPERATORS_ON(T)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    constexpr T &operator++(T &d) { return d = T(int(d) + 1); }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    constexpr T &operator--(T &d) { return d = T(int(d) - 1); }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    constexpr T operator++(T &d, int) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
        T old = d;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
        ++d;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
        return old;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
    constexpr T operator--(T &d, int) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
        T old = d;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
        --d;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
        return old;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    }

ENABLE_INCR_OPERATORS_ON(PieceType)
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)

#undef ENABLE_INCR_OPERATORS_ON

constexpr Direction operator+(Direction d1, Direction d2) { return Direction(int(d1) + int(d2)); }
constexpr Direction operator*(int i, Direction d) { return Direction(i * int(d)); }

// Additional operators to add a Direction to a Square
constexpr Square operator+(Square s, Direction d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
constexpr Square &operator+=(Square &s, Direction d) { return s = s + d; }
constexpr Square &operator-=(Square &s, Direction d) { return s = s - d; }
// specifically for Polyglot (a.k.a zobrist hashing but use proper hash)
enum class PolyglotPiece : uint8_t { WPAWN = 1, WKNIGHT = 3, WBISHOP = 5, WROOK = 7, WQUEEN = 9, WKING = 11, BPAWN = 0, BKNIGHT = 2, BBISHOP = 4, BROOK = 6, BQUEEN = 8, BKING = 10, NO_PIECE = 12, PIECE_NB = 12 };
// Normal board, you can use ANY! (but comfortable for certain chess engines such as Stockfish)
enum class EnginePiece : uint8_t { NO_PIECE, WPAWN = PAWN + 0, WKNIGHT, WBISHOP, WROOK, WQUEEN, WKING, BPAWN = PAWN + 8, BKNIGHT, BBISHOP, BROOK, BQUEEN, BKING, PIECE_NB = 16 };
enum class ContiguousMappingPiece : uint8_t { WPAWN = 0, WKNIGHT = 1, WBISHOP = 2, WROOK = 3, WQUEEN = 4, WKING = 5, BPAWN = 6, BKNIGHT = 7, BBISHOP = 8, BROOK = 9, BQUEEN = 10, BKING = 11, NO_PIECE = 12, PIECE_NB = 12 };
template <typename T> constexpr size_t enum_idx() {
    if constexpr (std::is_same_v<T, PolyglotPiece>)
        return 0;
    else if constexpr (std::is_same_v<T, EnginePiece>)
        return 1;
    else if constexpr (std::is_same_v<T, ContiguousMappingPiece>)
        return 2;
    return -1;
}
// clang-format on
constexpr PieceType piece_of(PolyglotPiece p) { return p == decltype(p)::NO_PIECE ? NO_PIECE_TYPE : static_cast<PieceType>(static_cast<int>(p) / 2 + 1); }
constexpr PieceType piece_of(EnginePiece p) { return p == decltype(p)::NO_PIECE ? NO_PIECE_TYPE : static_cast<PieceType>((static_cast<int>(p) - 1) % 8 + 1); }
constexpr PieceType piece_of(ContiguousMappingPiece p) { return p == decltype(p)::NO_PIECE ? NO_PIECE_TYPE : static_cast<PieceType>(static_cast<int>(p) % 6 + 1); }
constexpr Color color_of(PolyglotPiece pt) { return static_cast<Color>((static_cast<int>(pt) + 1) % 2); }
constexpr Color color_of(EnginePiece pt) { return static_cast<Color>(static_cast<int>(pt) / static_cast<int>(EnginePiece::BPAWN)); }
constexpr Color color_of(ContiguousMappingPiece pt) { return static_cast<Color>(static_cast<uint8_t>(pt) / 6); }
template <typename T, std::enable_if_t<std::is_same_v<T, EnginePiece>, bool> = 0> constexpr EnginePiece make_piece(PieceType pt, Color c) { return static_cast<EnginePiece>((c << 3) + pt); }
template <typename T, std::enable_if_t<std::is_same_v<T, PolyglotPiece>, bool> = 0> constexpr PolyglotPiece make_piece(PieceType pt, Color c) { return static_cast<PolyglotPiece>(~c + 2 * (pt - 1)); }
template <typename T, std::enable_if_t<std::is_same_v<T, ContiguousMappingPiece>, bool> = 0> constexpr ContiguousMappingPiece make_piece(PieceType pt, Color c) { return static_cast<ContiguousMappingPiece>((static_cast<uint8_t>(pt) - 1) + 6 * static_cast<uint8_t>(c)); }
template <typename T, typename = std::enable_if_t<is_piece_enum<T>::value>> constexpr PieceType type_of(T p) { return piece_of(p); }

enum CastlingRights : int8_t {
    NO_CASTLING,
    WHITE_OO,
    WHITE_OOO = WHITE_OO << 1,
    BLACK_OO = WHITE_OO << 2,
    BLACK_OOO = WHITE_OO << 3,

    KING_SIDE = WHITE_OO | BLACK_OO,
    QUEEN_SIDE = WHITE_OOO | BLACK_OOO,
    WHITE_CASTLING = WHITE_OO | WHITE_OOO,
    BLACK_CASTLING = BLACK_OO | BLACK_OOO,
    ANY_CASTLING = WHITE_CASTLING | BLACK_CASTLING,

    CASTLING_RIGHT_NB = 16
};
constexpr CastlingRights operator&(Color c, CastlingRights cr) { return CastlingRights((c == WHITE ? WHITE_CASTLING : BLACK_CASTLING) & cr); }
// Bitwise OR assignment operator
inline CastlingRights &operator|=(CastlingRights &a, CastlingRights b) {
    a = static_cast<CastlingRights>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    return a;
}
// Bitwise OR assignment operator
inline CastlingRights operator|(CastlingRights a, CastlingRights b) { return static_cast<CastlingRights>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }

// Bitwise OR assignment operator
inline CastlingRights &operator&=(CastlingRights &a, CastlingRights b) {
    a = static_cast<CastlingRights>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    return a;
}
inline CastlingRights operator~(CastlingRights a) { return static_cast<CastlingRights>(static_cast<uint8_t>(a) ^ ANY_CASTLING); }

enum MoveType : uint16_t { NORMAL, PROMOTION = 1 << 14, EN_PASSANT = 2 << 14, CASTLING = 3 << 14 };
// A move needs 16 bits to be stored
//
// bit  0- 5: destination square (from 0 to 63)
// bit  6-11: origin square (from 0 to 63)
// bit 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
// NOTE: en passant bit is set only when a pawn can be captured
//
// Special cases are Move::none() and Move::null(). We can sneak these in because
// in any normal move the destination square and origin square are always different,
// but Move::none() and Move::null() have the same origin and destination square.
class Move {
  public:
    Move() = default;
    constexpr Move(std::uint16_t d) : data(d) {}

    constexpr Move(Square from, Square to) : data((static_cast<uint16_t>(from) << 6) | static_cast<int>(to)) {}

    template <MoveType T = NORMAL> static constexpr Move make(Square from, Square to, PieceType pt = PieceType::KNIGHT) { return Move(static_cast<int>(T) | static_cast<int>((static_cast<int>(pt) - static_cast<int>(PieceType::KNIGHT)) << 12) | (static_cast<int>(from) << 6) | static_cast<int>(to)); }

    constexpr Square from_sq() const {
        assert(is_ok());
        return Square((data >> 6) & 0x3F);
    }

    constexpr Square to_sq() const {
        assert(is_ok());
        return Square(data & 0x3F);
    }

    constexpr Square from() const { return from_sq(); }
    constexpr Square to() const { return to_sq(); }
    constexpr int from_to() const { return data & 0xFFF; }

    constexpr MoveType type_of() const { return MoveType(data & (3 << 14)); }
    constexpr MoveType typeOf() const { return type_of(); }

    constexpr PieceType promotion_type() const { return static_cast<PieceType>(((data >> 12) & 3) + static_cast<int>(PieceType::KNIGHT)); }
    constexpr PieceType promotionType() const { return promotion_type(); }
    constexpr bool is_ok() const { return none().data != data && null().data != data; }

    static constexpr Move null() { return Move(65); }
    static constexpr Move none() { return Move(0); }

    constexpr bool operator==(const Move &m) const { return data == m.data; }
    constexpr bool operator!=(const Move &m) const { return data != m.data; }

    constexpr explicit operator bool() const { return data != 0; }

    constexpr std::uint16_t raw() const { return data; }

    struct MoveHash {
        std::size_t operator()(const Move &m) const { return m.data; }
    };
    std::string uci() const;
    static constexpr std::uint16_t NO_MOVE = 0;
    static constexpr std::uint16_t NULL_MOVE = 65;
    static constexpr MoveType NORMAL = MoveType::NORMAL;
    static constexpr MoveType PROMOTION = MoveType::PROMOTION;
    static constexpr MoveType ENPASSANT = MoveType::EN_PASSANT;
    static constexpr MoveType CASTLING = MoveType::CASTLING;

  protected:
    std::uint16_t data;
};

template <typename T, typename... Ts> struct is_all_same {
    static constexpr bool value = (std::is_same_v<T, Ts> && ...);
};

template <typename... Ts> constexpr auto is_all_same_v = is_all_same<Ts...>::value;

template <typename T, std::size_t MaxSize> class ValueList {
    static_assert(MaxSize, "what are you doing with 0 items");

  public:
    using size_type = std::size_t;
    ValueList() = default;
    inline size_type size() const { return size_; }

    inline void push_back(const T &value) {
        assert(size_ < MaxSize);
        values_[size_++] = value;
    }

    inline T pop() {
        assert(size_ > 0);
        return values_[--size_]; // always safe due to mask
    }

    inline void pop_back() {
        assert(size_ > 0);
        size_--;
    }

    inline T front() const {
        assert(size_ > 0);
        return values_[0];
    }

    inline T &operator[](int index) {
        assert(index < MaxSize); // relax the conditions, it's BRANCHLESS so forgive
        return values_[index];
    }

    inline const T *begin() const { return values_; }
    inline T *data() { return values_; }
    inline const T *end() const { return values_ + size_; }
    size_type size_ = 0;

  private:
    T values_[MaxSize]{};
};

using Movelist = ValueList<Move, 256>;
constexpr int square_distance(Square a, Square b) { return std::max(std::abs(file_of(a) - file_of(b)), std::abs(rank_of(a) - rank_of(b))); }
constexpr Square parse_square(std::string_view sv) {
    if (sv.size() < 2)
        return SQ_NONE;

    char f = sv[0];
    char r = sv[1];

    if (f < 'a' || f > 'h' || r < '1' || r > '8')
        return SQ_NONE;

    return make_sq(File(f - 'a'), Rank(r - '1'));
}

constexpr PieceType parse_pt(unsigned char c) {
    const char a[] = "pnbrqk";
    int p = -1;
    (c >='A' && c<='Z') ? (c += 32) : (c); // tolower
    for (size_t i = 0; i < sizeof(a); i++) {
        if (c == a[i])
            p = i;
    }
    return static_cast<PieceType>(p + 1);
}

} // namespace chess


