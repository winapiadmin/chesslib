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

// enums, structures, Move class taken from Stockfish
// License: https://github.com/official-stockfish/Stockfish/blob/master/COPYING.txt

#pragma once

#include "fwd_decl.h"
#include <cassert>
#include <cstdint>
#include <string>

/// @file types.h
/// @brief Core chess type definitions: squares, pieces, colours, move encoding, and ValueList.

/// @def UNREACHABLE()
/// @brief Marks code paths that should never be reached.
#if defined(_MSC_VER)
#define UNREACHABLE() __assume(false)
#elif defined(__clang__) || defined(__GNUC__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(__cpp_lib_unreachable)
#define UNREACHABLE() std::unreachable()
#else
#define UNREACHABLE() assert(0)
#endif

/// @def ASSUME(expr)
/// @brief Compiler hint that expr is always true.
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
#define ASSUME(cond)                                                                                                           \
    do {                                                                                                                       \
        if (!(cond))                                                                                                           \
            UNREACHABLE();                                                                                                     \
    } while (0)
#endif
#else
#define ASSUME(cond)                                                                                                           \
    do {                                                                                                                       \
        if (!(cond))                                                                                                           \
            UNREACHABLE();                                                                                                     \
    } while (0)
#endif

/// @brief Detect whether the current evaluation context is constant-evaluated.
/// @return true if called during compile-time evaluation.
constexpr bool is_constant_evaluated() {
#if __cpp_if_consteval >= 202106L
    if consteval {
        return true;
    }
#elif __cplusplus >= 202002L || _MSVC_LANG >= 202002L
    return std::is_constant_evaluated();
#elif defined(__GNUC__)
    return __builtin_is_constant_evaluated();
#elif _MSC_VER >= 1925
    return __builtin_is_constant_evaluated();
#else
#warning "NAWH we don't think we can detect compile time in this compiler";
#endif
    return false;
}

namespace chess {

/// @typedef Bitboard
/// @brief 64-bit bitboard representing a set of squares.
using Bitboard = uint64_t;

/// @typedef Key
/// @brief 64-bit Zobrist hash key.
using Key = uint64_t;

// clang-format off
/// @enum Square
/// @brief Board square identifiers in little-endian rank-file mapping.
/// @details SQ_A1 (0) through SQ_H8 (63). SQ_NONE is an invalid sentinel.
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

/// @enum File
/// @brief File (column) on the board: 0 = a-file, 7 = h-file.
enum File : int8_t { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB };

/// @enum Rank
/// @brief Rank (row) on the board: 0 = rank 1, 7 = rank 8.
enum Rank : int8_t { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB };

/// @brief Mirror a square vertically (rank 1 <-> rank 8).
/// @param sq Input square.
/// @return Flipped square.
constexpr Square square_mirror(const Square sq) { return static_cast<Square>(static_cast<int>(sq) ^ 56); }
/// @brief Alias for square_mirror.
constexpr Square flip_sq(const Square sq) { return square_mirror(sq); }

/// @brief Check if a rank/file pair describes a valid board square.
constexpr bool is_valid(const Rank r, const File f) { return 0 <= r && r <= 7 && 0 <= f && f <= 7; }
/// @brief Check if a square index is valid (0..63).
constexpr bool is_valid(const Square s) { return 0 <= s && s < 64; }

/// @brief Extract the file from a square.
constexpr File file_of(const Square s) {
    assert(0 <= s && s < 64);
    return static_cast<File>(s & 7);
}

/// @brief Extract the rank from a square.
constexpr Rank rank_of(const Square s) {
    assert(0 <= s && s < 64);
    return static_cast<Rank>(s >> 3);
}

/// @brief Construct a square from rank and file.
constexpr Square make_sq(const Rank r, const File f) {
    assert(0 <= r && r <= 7 && 0 <= f && f <= 7);
    return static_cast<Square>(static_cast<uint8_t>(r * 8 + f));
}

/// @brief Construct a square from file and rank (order-reversed overload).
constexpr Square make_sq(const File f, const Rank r) {
    assert(0 <= r && r <= 7 && 0 <= f && f <= 7);
    return static_cast<Square>(static_cast<uint8_t>(r * 8 + f));
}

/// @enum Color
/// @brief Side or piece colour.
enum Color : uint8_t { WHITE = 0, BLACK = 1, COLOR_NB = 2 };

/// @brief Toggle colour.
constexpr Color operator~(const Color c) { return static_cast<Color>(c ^ BLACK); }

/// @enum PieceType
/// @brief Piece type without colour information.
/// @note ALL_PIECES aliases NO_PIECE_TYPE (0).
enum PieceType : std::int8_t { NO_PIECE_TYPE = 0, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, ALL_PIECES = 0, PIECE_TYPE_NB = 8 };

/// @enum Direction
/// @brief Compass direction offsets for bitboard shifting.
enum Direction : int8_t {
    NORTH = 8,
    EAST = 1,
    SOUTH = -NORTH,
    WEST = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST,

    DOUBLE_NORTH = 2 * NORTH,
    DOUBLE_EAST = 2 * EAST,
    DOUBLE_SOUTH = 2 * SOUTH,
    DOUBLE_WEST = 2 * WEST,
    DOUBLE_NORTH_EAST = 2 * NORTH_EAST,
    DOUBLE_SOUTH_EAST = 2 * SOUTH_EAST,
    DOUBLE_SOUTH_WEST = 2 * SOUTH_WEST,
    DOUBLE_NORTH_WEST = 2 * NORTH_WEST,

    DIR_NONE = 0
};
// clang-format on

/// @brief Convert a square to the opponent's perspective (vertical mirror).
inline constexpr Square relative_square(const Color c, const Square s) { return static_cast<Square>(s ^ (c * 56)); }

/// @brief Get the rook square for a castling move.
inline constexpr Square castling_rook_square(const Color c, const bool is_king_side) {
    return relative_square(c, is_king_side ? SQ_F1 : Square::SQ_D1);
}

/// @brief Get the king square for a castling move.
inline constexpr Square castling_king_square(const Color c, const bool is_king_side) {
    return relative_square(c, is_king_side ? SQ_G1 : Square::SQ_C1);
}

/// @brief Convert a rank to the opponent's perspective.
inline constexpr Rank relative_rank(Color c, Rank r) { return Rank(r ^ (c * 7)); }

/// @brief Get the relative rank of a square for the given colour.
inline constexpr Rank relative_rank(Color c, Square s) { return relative_rank(c, rank_of(s)); }

/// @brief Flip direction for the opponent.
inline constexpr Direction relative_direction(Color c, Direction d) { return static_cast<Direction>(c == WHITE ? d : -d); }

/// @brief Get the pawn-push direction for a colour.
inline constexpr Direction pawn_push(Color c) { return c == WHITE ? NORTH : SOUTH; }

/// @brief Generate increment/decrement operators for an enum type.
#define ENABLE_INCR_OPERATORS_ON(T)                                                                                            \
    constexpr T &operator++(T &d) { return d = T(int(d) + 1); }                                                                \
    constexpr T &operator--(T &d) { return d = T(int(d) - 1); }                                                                \
    constexpr T operator++(T &d, int) {                                                                                        \
        T old = d;                                                                                                             \
        ++d;                                                                                                                   \
        return old;                                                                                                            \
    }                                                                                                                          \
    constexpr T operator--(T &d, int) {                                                                                        \
        T old = d;                                                                                                             \
        --d;                                                                                                                   \
        return old;                                                                                                            \
    }

ENABLE_INCR_OPERATORS_ON(PieceType)
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)
#undef ENABLE_INCR_OPERATORS_ON

constexpr Direction operator+(Direction d1, Direction d2) { return Direction(int(d1) + int(d2)); }
constexpr Direction operator*(int i, Direction d) { return Direction(i * int(d)); }

/// @name Square arithmetic operators
/// @{
constexpr Square operator+(Square s, Direction d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
constexpr Square &operator+=(Square &s, Direction d) { return s = s + d; }
constexpr Square &operator-=(Square &s, Direction d) { return s = s - d; }
constexpr Rank operator+(Rank s, Direction d) {
    assert(d == EAST || d == WEST || d == DIR_NONE);
    return Rank(int(s) + int(d));
}
constexpr Rank operator-(Rank s, Direction d) {
    assert(d == EAST || d == WEST || d == DIR_NONE);
    return Rank(int(s) - int(d));
}
constexpr Rank &operator+=(Rank &s, Direction d) { return s = s + d; }
constexpr Rank &operator-=(Rank &s, Direction d) { return s = s - d; }
constexpr File operator+(File s, Direction d) {
    assert(d == EAST || d == WEST || d == DIR_NONE);
    return File(int(s) + int(d));
}
constexpr File operator-(File s, Direction d) {
    assert(d == EAST || d == WEST || d == DIR_NONE);
    return File(int(s) - int(d));
}
constexpr File &operator+=(File &s, Direction d) { return s = s + d; }
constexpr File &operator-=(File &s, Direction d) { return s = s - d; }
/// @}

/// @enum PolyglotPiece
/// @brief Piece encoding compatible with Polyglot opening books.
enum class PolyglotPiece : uint8_t {
    WPAWN = 1,
    WKNIGHT = 3,
    WBISHOP = 5,
    WROOK = 7,
    WQUEEN = 9,
    WKING = 11,
    BPAWN = 0,
    BKNIGHT = 2,
    BBISHOP = 4,
    BROOK = 6,
    BQUEEN = 8,
    BKING = 10,
    NO_PIECE = 12,
    PIECE_NB = 12
};

/// @enum EnginePiece
/// @brief Default engine piece encoding with 8 values per colour.
enum class EnginePiece : uint8_t {
    NO_PIECE,
    WPAWN = PAWN + 0,
    WKNIGHT,
    WBISHOP,
    WROOK,
    WQUEEN,
    WKING,
    BPAWN = PAWN + 8,
    BKNIGHT,
    BBISHOP,
    BROOK,
    BQUEEN,
    BKING,
    PIECE_NB = 16
};

/// @enum ContiguousMappingPiece
/// @brief Compact piece encoding (0-5 = white, 6-11 = black).
enum class ContiguousMappingPiece : uint8_t {
    WPAWN = 0,
    WKNIGHT = 1,
    WBISHOP = 2,
    WROOK = 3,
    WQUEEN = 4,
    WKING = 5,
    BPAWN = 6,
    BKNIGHT = 7,
    BBISHOP = 8,
    BROOK = 9,
    BQUEEN = 10,
    BKING = 11,
    NO_PIECE = 12,
    PIECE_NB = 12
};

/// @brief Get the Zobrist array index for a piece-enum type.
/// @tparam T One of PolyglotPiece, EnginePiece, ContiguousMappingPiece.
/// @return 0, 1, or 2.
template <typename T> constexpr size_t enum_idx() {
    if constexpr (std::is_same_v<T, PolyglotPiece>)
        return 0;
    else if constexpr (std::is_same_v<T, EnginePiece>)
        return 1;
    else if constexpr (std::is_same_v<T, ContiguousMappingPiece>)
        return 2;
    return -1;
}

/// @name Piece-to-PieceType conversion
/// @{
constexpr PieceType piece_of(PolyglotPiece p) {
    return p == decltype(p)::NO_PIECE ? NO_PIECE_TYPE : static_cast<PieceType>(static_cast<int>(p) / 2 + 1);
}
constexpr PieceType piece_of(EnginePiece p) {
    return p == decltype(p)::NO_PIECE ? NO_PIECE_TYPE : static_cast<PieceType>((static_cast<int>(p) - 1) % 8 + 1);
}
constexpr PieceType piece_of(ContiguousMappingPiece p) {
    return p == decltype(p)::NO_PIECE ? NO_PIECE_TYPE : static_cast<PieceType>(static_cast<int>(p) % 6 + 1);
}
/// @}

/// @name Colour extraction
/// @{
constexpr Color color_of(PolyglotPiece pt) { return static_cast<Color>((static_cast<int>(pt) + 1) % 2); }
constexpr Color color_of(EnginePiece pt) {
    return static_cast<Color>(static_cast<int>(pt) / static_cast<int>(EnginePiece::BPAWN));
}
constexpr Color color_of(ContiguousMappingPiece pt) { return static_cast<Color>(static_cast<uint8_t>(pt) / 6); }
/// @}

/// @name Piece construction from (type, colour)
/// @{
template <typename T, std::enable_if_t<std::is_same_v<T, EnginePiece>, bool> = 0>
constexpr EnginePiece make_piece(PieceType pt, Color c) {
    return static_cast<EnginePiece>((c << 3) + pt);
}
template <typename T, std::enable_if_t<std::is_same_v<T, PolyglotPiece>, bool> = 0>
constexpr PolyglotPiece make_piece(PieceType pt, Color c) {
    return static_cast<PolyglotPiece>(~c + 2 * (pt - 1));
}
template <typename T, std::enable_if_t<std::is_same_v<T, ContiguousMappingPiece>, bool> = 0>
constexpr ContiguousMappingPiece make_piece(PieceType pt, Color c) {
    return static_cast<ContiguousMappingPiece>((static_cast<uint8_t>(pt) - 1) + 6 * static_cast<uint8_t>(c));
}
/// @}

/// @brief Extract PieceType from any piece enum (generic).
template <typename T, typename = std::enable_if_t<is_piece_enum<T>::value>> constexpr PieceType type_of(T p) {
    return piece_of(p);
}

/// @enum CastlingRights
/// @brief Bitmask of available castling rights.
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

/// @brief Intersect CastlingRights with a colour's rights.
constexpr CastlingRights operator&(Color c, CastlingRights cr) {
    return CastlingRights((c == WHITE ? WHITE_CASTLING : BLACK_CASTLING) & cr);
}

/// @brief Bitwise OR-assign for CastlingRights.
inline CastlingRights &operator|=(CastlingRights &a, CastlingRights b) {
    return a = static_cast<CastlingRights>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

/// @brief Bitwise OR for CastlingRights.
inline CastlingRights operator|(CastlingRights a, CastlingRights b) {
    return static_cast<CastlingRights>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

/// @brief Bitwise AND-assign for CastlingRights.
inline CastlingRights &operator&=(CastlingRights &a, CastlingRights b) {
    a = static_cast<CastlingRights>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    return a;
}

/// @brief Bitwise NOT for CastlingRights.
inline CastlingRights operator~(CastlingRights a) {
    return static_cast<CastlingRights>(static_cast<uint8_t>(a) ^ ANY_CASTLING);
}

/// @enum MoveType
/// @brief Move-type flags stored in the high 2 bits of a Move.
enum MoveType : uint16_t { NORMAL, PROMOTION = 1 << 14, EN_PASSANT = 2 << 14, CASTLING = 3 << 14 };

/// @class Move
/// @brief Compact 16-bit move representation.
///
/// Bit layout:
/// - bit  0-5: destination square (0-63)
/// - bit  6-11: origin square (0-63)
/// - bit 12-13: promotion piece type - KNIGHT (0-3)
/// - bit 14-15: move type flag (normal/promotion/en-passant/castling)
///
/// Special values: Move::none() (0) and Move::null() (65).
class Move {
  public:
    Move() = default;
    constexpr Move(std::uint16_t d) : data(d) {}
    constexpr Move(Square from, Square to) : data((static_cast<uint16_t>(from) << 6) | static_cast<int>(to)) {}

    /// @brief Construct a move with an explicit type and optional promotion piece.
    template <MoveType T = NORMAL> static constexpr Move make(Square from, Square to, PieceType pt = PieceType::KNIGHT) {
        return Move(static_cast<int>(T) | static_cast<int>((static_cast<int>(pt) - static_cast<int>(PieceType::KNIGHT)) << 12) |
                    (static_cast<int>(from) << 6) | static_cast<int>(to));
    }

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

    /// @brief Get the packed from|to field (lower 12 bits).
    constexpr int from_to() const { return data & 0xFFF; }

    /// @brief Get the move type.
    constexpr MoveType type_of() const { return MoveType(data & (3 << 14)); }
    constexpr bool is_ok() const { return none().data != data && null().data != data; }

    /// @brief Get the promotion piece type.
    constexpr PieceType promotion_type() const { return PieceType(((data >> 12) & 3) + KNIGHT); }

    static constexpr Move null() { return Move(65); }
    static constexpr Move none() { return Move(0); }

    constexpr bool operator==(const Move &m) const { return data == m.data; }
    constexpr bool operator!=(const Move &m) const { return data != m.data; }
    constexpr explicit operator bool() const { return data != 0; }

    /// @brief Get the raw 16-bit encoding.
    constexpr std::uint16_t raw() const { return data; }

    /// @brief Hash functor for use in unordered containers.
    struct MoveHash {
        std::size_t operator()(const Move &m) const { return m.data; }
    };

    std::string uci() const;

    /// @name Convenience constants
    /// @{
    static constexpr std::uint16_t NO_MOVE = 0;
    static constexpr std::uint16_t NULL_MOVE = 65;
    static constexpr MoveType NORMAL = MoveType::NORMAL;
    static constexpr MoveType PROMOTION = MoveType::PROMOTION;
    static constexpr MoveType ENPASSANT = MoveType::EN_PASSANT;
    static constexpr MoveType CASTLING = MoveType::CASTLING;
    /// @}

  protected:
    std::uint16_t data;
};

/// @brief Trait: check that all types in a pack are the same.
template <typename T, typename... Ts> struct is_all_same {
    static constexpr bool value = (std::is_same_v<T, Ts> && ...);
};
template <typename... Ts> constexpr auto is_all_same_v = is_all_same<Ts...>::value;

/// @class ValueList
/// @brief Stack-allocated fixed-capacity vector.
/// @tparam T Element type.
/// @tparam MaxSize Maximum number of elements.
template <typename T, std::size_t MaxSize> class ValueList {
    static_assert(MaxSize, "what are you doing with 0 items");

  public:
    using size_type = std::size_t;
    ValueList() = default;

    /// @brief Number of elements currently stored.
    inline size_type size() const { return size_; }

    /// @brief Append an element.
    inline void push_back(const T &value) {
        assert(size_ < MaxSize);
        values_[size_++] = value;
    }

    /// @brief Remove and return the last element.
    inline T pop() {
        assert(size_ > 0);
        return values_[--size_];
    }

    /// @brief Remove the last element without returning it.
    inline void pop_back() {
        assert(size_ > 0);
        size_--;
    }

    /// @brief Access the first element.
    inline T front() const {
        assert(size_ > 0);
        return values_[0];
    }

    /// @brief Indexed access.
    inline T &operator[](int index) {
        assert(index < size_);
        return values_[index];
    }

    inline const T *begin() const { return values_; }
    inline T *data() { return values_; }
    inline const T *end() const { return values_ + size_; }

    size_type size_ = 0;

  private:
    T values_[MaxSize];
};

/// @typedef Movelist
/// @brief Fixed-capacity list of up to 256 moves.
using Movelist = ValueList<Move, 256>;

/// @brief Chebyshev distance between two squares.
/// @return max(|dfile|, |drank|)
constexpr int square_distance(Square a, Square b) {
    return std::max(std::abs(file_of(a) - file_of(b)), std::abs(rank_of(a) - rank_of(b)));
}

/// @brief Parse a square from algebraic notation.
/// @param sv e.g. "e4", "a1".
/// @return Square, or SQ_NONE on parse failure.
constexpr Square parse_square(std::string_view sv) {
    if (sv.size() < 2)
        return SQ_NONE;
    char f = sv[0];
    char r = sv[1];
    if (f < 'a' || f > 'h' || r < '1' || r > '8')
        return SQ_NONE;
    return make_sq(File(f - 'a'), Rank(r - '1'));
}

/// @brief Parse a PieceType from a character.
/// @param c Character: p, n, b, r, q, k (case-insensitive).
/// @return PieceType, or NO_PIECE_TYPE on failure.
constexpr PieceType parse_pt(unsigned char c) {
    const char a[] = "pnbrqk";
    int p = -1;
    if (c >= 'A' && c <= 'Z')
        c += 32;
    for (size_t i = 0; i < sizeof(a); i++) {
        if (c == a[i])
            p = i;
    }
    return static_cast<PieceType>(p + 1);
}

} // namespace chess
