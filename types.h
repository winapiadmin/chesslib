#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <type_traits>
#include <vector>
#if defined(_MSC_VER)
    #define ASSUME(expr) __assume(expr)
#elif defined(__clang__) || defined(__GNUC__)
    #if __cplusplus>=202207L
        #define ASSUME(expr) [[assume(expr)]]
    #else
        #define ASSUME(expr) \
            do \
            { \
                if (!(expr)) \
                    __builtin_unreachable(); \
            } while (0)
    #endif
#else
    #define ASSUME(expr) ((void) 0)
#endif

namespace chess {
    using Bitboard = uint64_t;
    //clang-format off
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
        SQUARE_NB   = 64
    };
    enum File : int8_t {
        FILE_A,
        FILE_B,
        FILE_C,
        FILE_D,
        FILE_E,
        FILE_F,
        FILE_G,
        FILE_H,
        FILE_NB
    };

    enum Rank : int8_t {
        RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_NB
    };
    constexpr bool is_valid(const Rank r, const File f)
    {
        return 0 <= r && r <= 7 && 0 <= f && f <= 7;
    }  // unsigned already fix signedness
    constexpr File file_of(Square s)
    {
        ASSUME(0 <= s && s < 64);
        return File(s & 7);
    }

    constexpr Rank rank_of(Square s)
    {
        ASSUME(0 <= s && s < 64);
        return Rank(s >> 3);
    }
    constexpr Square make_sq(Rank r, File f)
    {
        // compile error if the user passes File,Rank instead of Rank,File (hehehehe)
        ASSUME(0 <= r && r <= 7 && 0 <= f && f <= 7);
        return static_cast<Square>(static_cast<uint8_t>(r * 8 + f));
    }
    constexpr Square make_sq(File f, Rank r)
    {
        // compile error if the user passes File,Rank instead of Rank,File (hehehehe)
        ASSUME(0 <= r && r <= 7 && 0 <= f && f <= 7);
        return static_cast<Square>(static_cast<uint8_t>(r * 8 + f));
    }
    enum Color : uint8_t {
        WHITE    = 0,
        BLACK    = 1,
        COLOR_NB = 2
    };
    // Toggle color
    constexpr Color operator~(Color c) { return Color(c ^ BLACK); }
    enum PieceType : std::int8_t {
        NO_PIECE_TYPE,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        ALL_PIECES    = 0,
        PIECE_TYPE_NB = 8
    };
    enum Direction : int8_t {
        NORTH = 8,
        EAST  = 1,
        SOUTH = -NORTH,
        WEST  = -EAST,

        NORTH_EAST = NORTH + EAST,
        SOUTH_EAST = SOUTH + EAST,
        SOUTH_WEST = SOUTH + WEST,
        NORTH_WEST = NORTH + WEST
    };
    //clang-format on
#define ENABLE_INCR_OPERATORS_ON(T) \
    constexpr T& operator++(T& d) { return d = T(int(d) + 1); } \
    constexpr T& operator--(T& d) { return d = T(int(d) - 1); } \
    constexpr T  operator++(T& d, int) \
    { \
        T old = d; \
        ++d; \
        return old; \
    } \
    constexpr T operator--(T& d, int) \
    { \
        T old = d; \
        --d; \
        return old; \
    }

    ENABLE_INCR_OPERATORS_ON(PieceType)
    ENABLE_INCR_OPERATORS_ON(Square)
    ENABLE_INCR_OPERATORS_ON(File)
    ENABLE_INCR_OPERATORS_ON(Rank)

#undef ENABLE_INCR_OPERATORS_ON

    constexpr Direction operator+(Direction d1, Direction d2)
    {
        return Direction(int(d1) + int(d2));
    }
    constexpr Direction operator*(int i, Direction d) { return Direction(i * int(d)); }

    // Additional operators to add a Direction to a Square
    constexpr Square  operator+(Square s, Direction d) { return Square(int(s) + int(d)); }
    constexpr Square  operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
    constexpr Square& operator+=(Square& s, Direction d) { return s = s + d; }
    constexpr Square& operator-=(Square& s, Direction d) { return s = s - d; }
    // specifically for Polyglot (a.k.a zobrist hashing but use proper hash)
    enum class PolyglotPiece : uint8_t {
        WPAWN    = 1,
        WKNIGHT  = 3,
        WBISHOP  = 5,
        WROOK    = 7,
        WQUEEN   = 9,
        WKING    = 11,
        BPAWN    = 0,
        BKNIGHT  = 2,
        BBISHOP  = 4,
        BROOK    = 6,
        BQUEEN   = 8,
        BKING    = 10,
        NO_PIECE = 12,
        PIECE_NB = 12
    };
    // Normal board, you can use ANY! (but comfortable for certain chess engines such as Stockfish)
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
    //clang-format on
    constexpr PieceType piece_of(PolyglotPiece p)
    {
        return static_cast<PieceType>(static_cast<int>(p) / 2);
    }
    constexpr PieceType piece_of(EnginePiece p) {
        int val = static_cast<int>(p);
        return val == 0 ? NO_PIECE_TYPE : static_cast<PieceType>((val - 1) % 8 + 1);
    }

    constexpr Color color_of(PolyglotPiece pt)
    {
        return static_cast<Color>(static_cast<int>(pt) % 2);
    }
    constexpr Color color_of(EnginePiece pt)
    {
        return static_cast<Color>(static_cast<int>(pt) / static_cast<int>(EnginePiece::BPAWN));
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, EnginePiece>, bool> = 0>
    constexpr EnginePiece make_piece(PieceType pt, Color c)
    {
        return static_cast<EnginePiece>((c << 3) + pt);
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, PolyglotPiece>, bool> = 0>
    constexpr PolyglotPiece make_piece(PieceType pt, Color c)
    {
        return static_cast<PolyglotPiece>(~c + 2 * (pt - 1));
    }
    // a little test
    static_assert(make_piece<PolyglotPiece>(PAWN, WHITE) == PolyglotPiece::WPAWN,
                  "white pawn != 1?");
    static_assert(make_piece<PolyglotPiece>(PAWN, BLACK) == PolyglotPiece::BPAWN,
                  "black pawn != 0?");
    static_assert(make_piece<PolyglotPiece>(KNIGHT, WHITE) == PolyglotPiece::WKNIGHT,
                  "white knight != 3?");
    static_assert(make_piece<PolyglotPiece>(KNIGHT, BLACK) == PolyglotPiece::BKNIGHT,
                  "black knight != 2?");
    static_assert(make_piece<EnginePiece>(KNIGHT, BLACK) == EnginePiece::BKNIGHT,
                  "black knight != 2?");
    static_assert(piece_of(EnginePiece::BBISHOP)==BISHOP, "piece_of faulty EnginePiece::BBISHOP!=BISHOP");
    static_assert(make_sq(RANK_1, FILE_A) == SQ_A1);
    static_assert(make_sq(RANK_8, FILE_A) == SQ_A8);
    static_assert(make_sq(RANK_1, FILE_H) == SQ_H1);
    static_assert(file_of(SQ_H7) == FILE_H);
    static_assert(rank_of(SQ_C3) == RANK_3);

    // end tests
    enum CastlingRights : int8_t {
        NO_CASTLING,
        WHITE_OO,
        WHITE_OOO = WHITE_OO << 1,
        BLACK_OO  = WHITE_OO << 2,
        BLACK_OOO = WHITE_OO << 3,

        KING_SIDE      = WHITE_OO | BLACK_OO,
        QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
        WHITE_CASTLING = WHITE_OO | WHITE_OOO,
        BLACK_CASTLING = BLACK_OO | BLACK_OOO,
        ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

        CASTLING_RIGHT_NB = 16
    };
    constexpr CastlingRights operator&(Color c, CastlingRights cr) {
        return CastlingRights((c == WHITE ? WHITE_CASTLING : BLACK_CASTLING) & cr);
    }
    // Bitwise OR assignment operator
    inline CastlingRights& operator|=(CastlingRights& a, CastlingRights b) {
        a = static_cast<CastlingRights>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
        return a;
    }

    // Bitwise OR assignment operator
    inline CastlingRights& operator&=(CastlingRights& a, CastlingRights b) {
        a = static_cast<CastlingRights>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
        return a;
    }
    inline CastlingRights operator~(CastlingRights a) {
        return static_cast<CastlingRights>(~static_cast<uint8_t>(a));
    }

    enum MoveType {
        NORMAL,
        PROMOTION  = 1 << 14,
        EN_PASSANT = 2 << 14,
        CASTLING   = 3 << 14
    };

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
    class Move
    {
        public:
        Move() = default;
        constexpr explicit Move(std::uint16_t d) :
            data(d)
        {
        }

        constexpr Move(Square from, Square to) :
            data((static_cast<uint16_t>(from) << 6) | static_cast<int>(to))
        {
        }

        template<MoveType T, PieceType pt = PieceType::KNIGHT>
        static constexpr Move make(Square from, Square to)
        {
            return Move(
              static_cast<int>(T)
              | static_cast<int>((static_cast<int>(pt) - static_cast<int>(PieceType::KNIGHT)) << 12)
              | (static_cast<int>(from) << 6) | static_cast<int>(to));
        }

        constexpr Square from_sq() const
        {
            assert(is_ok());
            return Square((data >> 6) & 0x3F);
        }

        constexpr Square to_sq() const
        {
            assert(is_ok());
            return Square(data & 0x3F);
        }

        constexpr int from_to() const { return data & 0xFFF; }

        constexpr MoveType type_of() const { return MoveType(data & (3 << 14)); }

        constexpr PieceType promotion_type() const
        {
            return static_cast<PieceType>(((data >> 12) & 3) + static_cast<int>(PieceType::KNIGHT));
        }

        constexpr bool is_ok() const { return none().data != data && null().data != data; }

        static constexpr Move null() { return Move(65); }
        static constexpr Move none() { return Move(0); }

        constexpr bool operator==(const Move& m) const { return data == m.data; }
        constexpr bool operator!=(const Move& m) const { return data != m.data; }

        constexpr explicit operator bool() const { return data != 0; }

        constexpr std::uint16_t raw() const { return data; }

        struct MoveHash {
            std::size_t operator()(const Move& m) const { return m.data; }
        };

        protected:
        std::uint16_t data;
    };
    constexpr std::string squareToString(int sq) {
        constexpr const char fileChars[] = "abcdefgh";
        constexpr const char rankChars[] = "12345678";

        // Assuming sq = 0..63 with 0 = a1, 1 = b1 ... up to 63 = h8
        char file = fileChars[sq % 8];
        char rank = rankChars[sq / 8];
        return std::string{file, rank};
    }

    inline std::ostream& operator<<(std::ostream& os, Move mv)
    {
        if (!mv.is_ok())
        {
            // null move
            os << "0000";
            return os;
        }
        constexpr const char PieceTypeChar[] = " pnbrqk";
        // Source square
        os << squareToString(mv.from_sq());
        // To square, special: castlings
        switch (mv.type_of())
        {
        case CASTLING :
            switch (mv.to_sq())
            {
            case SQ_H1 :
                os << "g1";  // White kingside castling
                break;
            case SQ_A1 :
                os << "c1";  // white queenside castling
                break;
            case SQ_H8 :
                os << "g8";  // black kingside castling
                break;
            case SQ_A8 :
                os << "c8";  // black queenside castling
                break;
            default :
                assert(false && "weirdly.... i didn't implement Chess960!");
            }
            break;
        case PROMOTION :
            os << squareToString(mv.to_sq());
            os << PieceTypeChar[mv.promotion_type()];
            break;
        default :
            os << squareToString(mv.to_sq());
            break;
        }
        return os;
    }
    template<typename T, typename... Ts>
    struct is_all_same {
        static constexpr bool value = (std::is_same_v<T, Ts> && ...);
    };

    template<typename... Ts>
    constexpr auto is_all_same_v = is_all_same<Ts...>::value;

    template<typename T, std::size_t MaxSize>
    class ValueList
    {
        static_assert(MaxSize, "what are you doing with 0 items");
        public:
        using size_type = std::size_t;

        inline size_type size() const { return size_; }

        inline void push_back(const T& value)
        {
            assert(size_ < MaxSize);
            values_[size_++] = value;
        }

        inline T pop()
        {
            assert(size_ > 0);
            return values_[--size_];  // always safe due to mask
        }

        inline void pop_back()
        {
            assert(size_ >0);
            size_--;
        }

        inline T front() const
        {
            assert(size_ > 0);
            return values_[0];
        }

        inline T& operator[](int index)
        {
            assert(index < MaxSize); // relax the conditions, it's BRANCHLESS so forgive
            return values_[index];
        }

        inline const T* begin() const { return values_; }
        inline const T* end() const { return values_ + size_; }
        size_type       size_ = 0;

        private:
        T         values_[MaxSize]{};
    };
    template<typename T, std::size_t MaxSize>
    class HeapAllocatedValueList
    {
        public:
        using size_type = std::size_t;
        inline HeapAllocatedValueList()
        {
            values_ = reinterpret_cast<T*>(calloc(MaxSize, sizeof(T)));
            assert(values_);
        }
        inline ~HeapAllocatedValueList()
        {
            free(values_);
        }

        inline size_type size() const { return size_; }

        inline void push_back(const T& value)
        {
            assert(size_ < MaxSize);

            values_[size_++] = value;
        }

        inline T pop()
        {
            assert(size_ > 0);
            return values_[--size_];  // always safe due to mask
        }

        inline void pop_back() { size_ -= (size_ > 0) ? 1 : 0; }

        inline T front() const { return (size_ > 0) ? values_[0] : T{}; }

        inline T& operator[](int index)
        {
            assert(index < MaxSize);  // relax the conditions, it's BRANCHLESS so forgive
            size_type i = static_cast<size_type>(index);
            return values_[i];
        }

        inline const T* begin() const { return values_; }
        inline const T* end() const { return values_ + size_; }
        size_type      size_ = 0;

        private:
        T* values_;
    };
}
