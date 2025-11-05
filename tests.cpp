#define DOCTEST_CONFIG_IMPLEMENT
#ifndef __EXCEPTIONS
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#endif
#include "moves_io.h"
#include "position.h"
#include "printers.h"
#include <chrono>
#include <doctest/doctest.h>
using namespace chess;
// --------- Color assertions ----------
static_assert(color_of(PolyglotPiece::BPAWN) == BLACK, "BPAWN should be BLACK");
static_assert(color_of(PolyglotPiece::BKNIGHT) == BLACK, "BKNIGHT should be BLACK");
static_assert(color_of(PolyglotPiece::BBISHOP) == BLACK, "BBISHOP should be BLACK");
static_assert(color_of(PolyglotPiece::BROOK) == BLACK, "BROOK should be BLACK");
static_assert(color_of(PolyglotPiece::BQUEEN) == BLACK, "BQUEEN should be BLACK");
static_assert(color_of(PolyglotPiece::BKING) == BLACK, "BKING should be BLACK");

static_assert(color_of(PolyglotPiece::WPAWN) == WHITE, "WPAWN should be WHITE");
static_assert(color_of(PolyglotPiece::WKNIGHT) == WHITE, "WKNIGHT should be WHITE");
static_assert(color_of(PolyglotPiece::WBISHOP) == WHITE, "WBISHOP should be WHITE");
static_assert(color_of(PolyglotPiece::WROOK) == WHITE, "WROOK should be WHITE");
static_assert(color_of(PolyglotPiece::WQUEEN) == WHITE, "WQUEEN should be WHITE");
static_assert(color_of(PolyglotPiece::WKING) == WHITE, "WKING should be WHITE");

// --------- PieceType assertions ----------
static_assert(piece_of(PolyglotPiece::BPAWN) == PAWN, "BPAWN -> PAWN");
static_assert(piece_of(PolyglotPiece::BKNIGHT) == KNIGHT, "BKNIGHT -> KNIGHT");
static_assert(piece_of(PolyglotPiece::BBISHOP) == BISHOP, "BBISHOP -> BISHOP");
static_assert(piece_of(PolyglotPiece::BROOK) == ROOK, "BROOK -> ROOK");
static_assert(piece_of(PolyglotPiece::BQUEEN) == QUEEN, "BQUEEN -> QUEEN");
static_assert(piece_of(PolyglotPiece::BKING) == KING, "BKING -> KING");

static_assert(piece_of(PolyglotPiece::WPAWN) == PAWN, "WPAWN -> PAWN");
static_assert(piece_of(PolyglotPiece::WKNIGHT) == KNIGHT, "WKNIGHT -> KNIGHT");
static_assert(piece_of(PolyglotPiece::WBISHOP) == BISHOP, "WBISHOP -> BISHOP");
static_assert(piece_of(PolyglotPiece::WROOK) == ROOK, "WROOK -> ROOK");
static_assert(piece_of(PolyglotPiece::WQUEEN) == QUEEN, "WQUEEN -> QUEEN");
static_assert(piece_of(PolyglotPiece::WKING) == KING, "WKING -> KING");

static_assert(piece_of(PolyglotPiece::NO_PIECE) == NO_PIECE_TYPE, "NO_PIECE -> NO_PIECE_TYPE");

// --------- Round-trip make_piece assertions ----------
static_assert(make_piece<PolyglotPiece>(PAWN, BLACK) == PolyglotPiece::BPAWN, "make_piece PAWN,BLACK -> BPAWN");
static_assert(make_piece<PolyglotPiece>(KNIGHT, BLACK) == PolyglotPiece::BKNIGHT, "make_piece KNIGHT,BLACK -> BKNIGHT");
static_assert(make_piece<PolyglotPiece>(BISHOP, BLACK) == PolyglotPiece::BBISHOP, "make_piece BISHOP,BLACK -> BBISHOP");
static_assert(make_piece<PolyglotPiece>(ROOK, BLACK) == PolyglotPiece::BROOK, "make_piece ROOK,BLACK -> BROOK");
static_assert(make_piece<PolyglotPiece>(QUEEN, BLACK) == PolyglotPiece::BQUEEN, "make_piece QUEEN,BLACK -> BQUEEN");
static_assert(make_piece<PolyglotPiece>(KING, BLACK) == PolyglotPiece::BKING, "make_piece KING,BLACK -> BKING");

static_assert(make_piece<PolyglotPiece>(PAWN, WHITE) == PolyglotPiece::WPAWN, "make_piece PAWN,WHITE -> WPAWN");
static_assert(make_piece<PolyglotPiece>(KNIGHT, WHITE) == PolyglotPiece::WKNIGHT, "make_piece KNIGHT,WHITE -> WKNIGHT");
static_assert(make_piece<PolyglotPiece>(BISHOP, WHITE) == PolyglotPiece::WBISHOP, "make_piece BISHOP,WHITE -> WBISHOP");
static_assert(make_piece<PolyglotPiece>(ROOK, WHITE) == PolyglotPiece::WROOK, "make_piece ROOK,WHITE -> WROOK");
static_assert(make_piece<PolyglotPiece>(QUEEN, WHITE) == PolyglotPiece::WQUEEN, "make_piece QUEEN,WHITE -> WQUEEN");
static_assert(make_piece<PolyglotPiece>(KING, WHITE) == PolyglotPiece::WKING, "make_piece KING,WHITE -> WKING");

// --------- Round-trip consistency ----------
static_assert(piece_of(make_piece<PolyglotPiece>(PAWN, WHITE)) == PAWN, "Round-trip piece PAWN,WHITE");
static_assert(piece_of(make_piece<PolyglotPiece>(KNIGHT, WHITE)) == KNIGHT, "Round-trip piece KNIGHT,WHITE");
static_assert(piece_of(make_piece<PolyglotPiece>(BISHOP, WHITE)) == BISHOP, "Round-trip piece BISHOP,WHITE");
static_assert(piece_of(make_piece<PolyglotPiece>(ROOK, WHITE)) == ROOK, "Round-trip piece ROOK,WHITE");
static_assert(piece_of(make_piece<PolyglotPiece>(QUEEN, WHITE)) == QUEEN, "Round-trip piece ROOK,WHITE");
static_assert(piece_of(make_piece<PolyglotPiece>(KING, WHITE)) == KING, "Round-trip piece ROOK,WHITE");

static_assert(piece_of(make_piece<PolyglotPiece>(PAWN, BLACK)) == PAWN, "Round-trip piece PAWN,BLACK");
static_assert(piece_of(make_piece<PolyglotPiece>(KNIGHT, BLACK)) == KNIGHT, "Round-trip piece KNIGHT,BLACK");
static_assert(piece_of(make_piece<PolyglotPiece>(BISHOP, BLACK)) == BISHOP, "Round-trip piece BISHOP,BLACK");
static_assert(piece_of(make_piece<PolyglotPiece>(ROOK, BLACK)) == ROOK, "Round-trip piece ROOK,BLACK");
static_assert(piece_of(make_piece<PolyglotPiece>(QUEEN, BLACK)) == QUEEN, "Round-trip piece ROOK,BLACK");
static_assert(piece_of(make_piece<PolyglotPiece>(KING, BLACK)) == KING, "Round-trip piece ROOK,BLACK");

// --------- Color assertions ----------
static_assert(color_of(PolyglotPiece::BPAWN) == BLACK, "BPAWN should be BLACK");
static_assert(color_of(PolyglotPiece::BKNIGHT) == BLACK, "BKNIGHT should be BLACK");
static_assert(color_of(PolyglotPiece::BBISHOP) == BLACK, "BBISHOP should be BLACK");
static_assert(color_of(PolyglotPiece::BROOK) == BLACK, "BROOK should be BLACK");
static_assert(color_of(PolyglotPiece::BQUEEN) == BLACK, "BQUEEN should be BLACK");
static_assert(color_of(PolyglotPiece::BKING) == BLACK, "BKING should be BLACK");

static_assert(color_of(PolyglotPiece::WPAWN) == WHITE, "WPAWN should be WHITE");
static_assert(color_of(PolyglotPiece::WKNIGHT) == WHITE, "WKNIGHT should be WHITE");
static_assert(color_of(PolyglotPiece::WBISHOP) == WHITE, "WBISHOP should be WHITE");
static_assert(color_of(PolyglotPiece::WROOK) == WHITE, "WROOK should be WHITE");
static_assert(color_of(PolyglotPiece::WQUEEN) == WHITE, "WQUEEN should be WHITE");
static_assert(color_of(PolyglotPiece::WKING) == WHITE, "WKING should be WHITE");

// --------- PieceType assertions ----------
static_assert(piece_of(EnginePiece::BPAWN) == PAWN, "BPAWN -> PAWN");
static_assert(piece_of(EnginePiece::BKNIGHT) == KNIGHT, "BKNIGHT -> KNIGHT");
static_assert(piece_of(EnginePiece::BBISHOP) == BISHOP, "BBISHOP -> BISHOP");
static_assert(piece_of(EnginePiece::BROOK) == ROOK, "BROOK -> ROOK");
static_assert(piece_of(EnginePiece::BQUEEN) == QUEEN, "BQUEEN -> QUEEN");
static_assert(piece_of(EnginePiece::BKING) == KING, "BKING -> KING");

static_assert(piece_of(EnginePiece::WPAWN) == PAWN, "WPAWN -> PAWN");
static_assert(piece_of(EnginePiece::WKNIGHT) == KNIGHT, "WKNIGHT -> KNIGHT");
static_assert(piece_of(EnginePiece::WBISHOP) == BISHOP, "WBISHOP -> BISHOP");
static_assert(piece_of(EnginePiece::WROOK) == ROOK, "WROOK -> ROOK");
static_assert(piece_of(EnginePiece::WQUEEN) == QUEEN, "WQUEEN -> QUEEN");
static_assert(piece_of(EnginePiece::WKING) == KING, "WKING -> KING");

static_assert(piece_of(EnginePiece::NO_PIECE) == NO_PIECE_TYPE, "NO_PIECE -> NO_PIECE_TYPE");

// --------- Round-trip make_piece assertions ----------
static_assert(make_piece<EnginePiece>(PAWN, BLACK) == EnginePiece::BPAWN, "make_piece PAWN,BLACK -> BPAWN");
static_assert(make_piece<EnginePiece>(KNIGHT, BLACK) == EnginePiece::BKNIGHT, "make_piece KNIGHT,BLACK -> BKNIGHT");
static_assert(make_piece<EnginePiece>(BISHOP, BLACK) == EnginePiece::BBISHOP, "make_piece BISHOP,BLACK -> BBISHOP");
static_assert(make_piece<EnginePiece>(ROOK, BLACK) == EnginePiece::BROOK, "make_piece ROOK,BLACK -> BROOK");
static_assert(make_piece<EnginePiece>(QUEEN, BLACK) == EnginePiece::BQUEEN, "make_piece QUEEN,BLACK -> BQUEEN");
static_assert(make_piece<EnginePiece>(KING, BLACK) == EnginePiece::BKING, "make_piece KING,BLACK -> BKING");

static_assert(make_piece<EnginePiece>(PAWN, WHITE) == EnginePiece::WPAWN, "make_piece PAWN,WHITE -> WPAWN");
static_assert(make_piece<EnginePiece>(KNIGHT, WHITE) == EnginePiece::WKNIGHT, "make_piece KNIGHT,WHITE -> WKNIGHT");
static_assert(make_piece<EnginePiece>(BISHOP, WHITE) == EnginePiece::WBISHOP, "make_piece BISHOP,WHITE -> WBISHOP");
static_assert(make_piece<EnginePiece>(ROOK, WHITE) == EnginePiece::WROOK, "make_piece ROOK,WHITE -> WROOK");
static_assert(make_piece<EnginePiece>(QUEEN, WHITE) == EnginePiece::WQUEEN, "make_piece QUEEN,WHITE -> WQUEEN");
static_assert(make_piece<EnginePiece>(KING, WHITE) == EnginePiece::WKING, "make_piece KING,WHITE -> WKING");

// --------- Round-trip consistency ----------
static_assert(piece_of(make_piece<EnginePiece>(PAWN, WHITE)) == PAWN, "Round-trip piece PAWN,WHITE");
static_assert(piece_of(make_piece<EnginePiece>(KNIGHT, WHITE)) == KNIGHT, "Round-trip piece KNIGHT,WHITE");
static_assert(piece_of(make_piece<EnginePiece>(BISHOP, WHITE)) == BISHOP, "Round-trip piece BISHOP,WHITE");
static_assert(piece_of(make_piece<EnginePiece>(ROOK, WHITE)) == ROOK, "Round-trip piece ROOK,WHITE");
static_assert(piece_of(make_piece<EnginePiece>(QUEEN, WHITE)) == QUEEN, "Round-trip piece ROOK,WHITE");
static_assert(piece_of(make_piece<EnginePiece>(KING, WHITE)) == KING, "Round-trip piece ROOK,WHITE");

static_assert(piece_of(make_piece<EnginePiece>(PAWN, BLACK)) == PAWN, "Round-trip piece PAWN,BLACK");
static_assert(piece_of(make_piece<EnginePiece>(KNIGHT, BLACK)) == KNIGHT, "Round-trip piece KNIGHT,BLACK");
static_assert(piece_of(make_piece<EnginePiece>(BISHOP, BLACK)) == BISHOP, "Round-trip piece BISHOP,BLACK");
static_assert(piece_of(make_piece<EnginePiece>(ROOK, BLACK)) == ROOK, "Round-trip piece ROOK,BLACK");
static_assert(piece_of(make_piece<EnginePiece>(QUEEN, BLACK)) == QUEEN, "Round-trip piece ROOK,BLACK");
static_assert(piece_of(make_piece<EnginePiece>(KING, BLACK)) == KING, "Round-trip piece ROOK,BLACK");
static_assert(make_sq(RANK_1, FILE_A) == SQ_A1, "incorrect indexing");
static_assert(make_sq(RANK_8, FILE_A) == SQ_A8, "incorrect indexing");
static_assert(make_sq(RANK_1, FILE_H) == SQ_H1, "incorrect indexing");
static_assert(file_of(SQ_H7) == FILE_H, "incorrect indexing");
static_assert(rank_of(SQ_C3) == RANK_3, "incorrect indexing");
#if defined(NDEBUG) || !defined(_DEBUG)
#define IS_RELEASE 1
#else
#define IS_RELEASE 0
#endif
struct perft_t {
    int depth;
    uint64_t nodes;
};
template <typename InputT, typename CheckInfo> struct TestEntry {
    InputT input;
    CheckInfo info;
};
template <typename T, MoveGenType mt, bool EnableDiv = false> uint64_t perft(_Position<T> &pos, int depth) {
    if (depth == 0) {
        return 1;
    } else if (depth == 1) {
        Movelist moves;
        pos.template legals<mt>(moves);
        if constexpr (EnableDiv)
            for (const Move &m : moves) {
                std::cout << m << ": 1\n";
            }
        return moves.size();
    } else {
        Movelist moves;
        pos.template legals<mt>(moves);

        uint64_t total = 0;
        for (const Move &m : moves) {
            pos.doMove(m);
            const uint64_t nodes = perft<T, mt, false>(pos, depth - 1);
            pos.undoMove();

            if constexpr (EnableDiv)
                std::cout << m << ": " << nodes << '\n';

            total += nodes;
        }

        if constexpr (EnableDiv)
            std::cout << "Total: " << total << '\n';

        return total;
    }
}
template <MoveGenType mt = MoveGenType::ALL, bool EnableDiv = false> void check_perfts(const std::vector<TestEntry<std::string, perft_t>> &entries) {
    uint64_t nodes = 0;
    double elapsed = 0;
    using namespace std::chrono;
    for (auto &entry : entries) {
        _Position<PolyglotPiece> pos(entry.input);
        auto start_time = high_resolution_clock::now();
        REQUIRE(perft<PolyglotPiece, mt, EnableDiv>(pos, entry.info.depth) == entry.info.nodes);
        auto end_time = high_resolution_clock::now();
        elapsed += duration<double>(end_time - start_time).count();
        nodes += entry.info.nodes;
    }
    for (auto &entry : entries) {
        _Position<EnginePiece> pos(entry.input);
        auto start_time = high_resolution_clock::now();
        REQUIRE(perft<EnginePiece, mt, EnableDiv>(pos, entry.info.depth) == entry.info.nodes);
        auto end_time = high_resolution_clock::now();
        elapsed += duration<double>(end_time - start_time).count();
        nodes += entry.info.nodes;
    }

    double mnps = (nodes / elapsed) / 1'000'000.0;
    std::cout << "Speed: " << mnps << "Mnps\n";
}
TEST_CASE("Castling move making FEN rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8") {
    std::string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    Position position(fen);
    REQUIRE(position.hash() == 0x4f874e21f78d3590);
    position.doMove(Move::make<CASTLING>(SQ_E1, SQ_H1));
    REQUIRE(position.hash() == position.zobrist());
    REQUIRE(position.hash() == 0x31F235158B7EEE80);
    REQUIRE(position.fen() == "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQ1RK1 b - - 2 8");
}
TEST_CASE("EP move making FEN 8/5k1K/8/8/4Pp2/8/8/8 b - e3 0 1") {
    std::string fen = "8/5k1K/8/8/4Pp2/8/8/8 b - e3 0 1";
    Position position(fen);
    REQUIRE(position.hash() == 0x775d0e2acc42db8c);
    position.doMove(Move::make<EN_PASSANT>(SQ_F4, SQ_E3));
    REQUIRE(position.hash() == position.zobrist());
    REQUIRE(position.hash() == 0x3ec71faae73cfbed);
    REQUIRE(position.fen() == "8/5k1K/8/8/8/4p3/8/8 w - - 0 2");
}
TEST_CASE("EP move making FEN 8/5k1K/8/3pP3/8/8/8/8 w - d6 0 2") {
    std::string fen = "8/5k1K/8/3pP3/8/8/8/8 w - d6 0 2";
    Position position(fen);
    REQUIRE(position.hash() == 0xbdc108e30cccd00b);
    position.doMove(Move::make<EN_PASSANT>(SQ_E5, SQ_D6));
    REQUIRE(position.fen() == "8/5k1K/3P4/8/8/8/8/8 b - - 0 2");
    REQUIRE(position.hash() == position.zobrist());
    REQUIRE(position.hash() == 0x5c57e65793a2c8fe);
}
TEST_CASE("Promotion FEN 8/5kP1/7K/8/8/8/8/8 w - - 0 1") {
    std::string fen = "8/5kP1/7K/8/8/8/8/8 w - - 0 1";
    Position position(fen);
    REQUIRE(position.hash() == 0x3793b24e1b95a6d5);
    position.doMove(Move::make<PROMOTION>(SQ_G7, SQ_G8));
    REQUIRE(position.fen() == "6N1/5k2/7K/8/8/8/8/8 b - - 0 1");
    REQUIRE(position.hash() == position.zobrist());
    REQUIRE(position.hash() == 0x6adb9bf15f32cb92);
}
TEST_CASE("Perft pawn-only startpos") {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position position(fen);
    REQUIRE(perft<EnginePiece, MoveGenType::PAWN>(position, 6) == 11515584);
    REQUIRE(perft<EnginePiece, MoveGenType::PAWN>(position, 5) == 815968);
}
TEST_CASE("EP replace on non-EP move") {
    Position position("rnbqkbnr/pppppp2/7p/6pP/8/8/PPPPPPP1/RNBQKBNR w KQkq g6 0 3");
    position.doMove(Move(SQ_E2, SQ_E4));
    REQUIRE(position.fen() == "rnbqkbnr/pppppp2/7p/6pP/4P3/8/PPPP1PP1/RNBQKBNR b KQkq e3 0 3");
}
TEST_CASE("EP ignore on non-EP move") {
    Position position("rnbqkbnr/pppppp2/7p/6pP/8/8/PPPPPPP1/RNBQKBNR w KQkq g6 0 3");
    position.doMove(Move(SQ_E2, SQ_E3));
    REQUIRE(position.fen() == "rnbqkbnr/pppppp2/7p/6pP/8/4P3/PPPP1PP1/RNBQKBNR b KQkq - 0 3");
}
TEST_CASE("King walk empty board wking e4 bking f8 depth 6") {
    std::string fen = "5k2/8/8/8/3K4/8/8/8 w - - 0 1";
    Position position(fen);
    REQUIRE(perft<EnginePiece, MoveGenType::KING>(position, 6) == 95366);
}
TEST_CASE("Pin detection double push movegen") {
    std::string fen = "rnbqkbnr/1ppppppp/8/p7/Q1P5/8/PP1PPPPP/RNB1KBNR b KQkq - 1 2";
    Position position(fen);
    Movelist out;
    position.legals<MoveGenType::PAWN>(out);
    auto pin_mask = position.state()._pin_mask;
    REQUIRE(out.size() == 12);
    REQUIRE(pin_mask != 0);
    auto pin_maskAND0x8000000000000ULL = pin_mask & 0x8000000000000ULL;
    REQUIRE(pin_maskAND0x8000000000000ULL != 0);
    REQUIRE(pin_mask == 0x8040201000000ULL);
}
TEST_CASE("Move making pin update") {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    pos.doMove(Move(SQ_C2, SQ_C4));
    pos.doMove(Move(SQ_A7, SQ_A5));
    pos.doMove(Move(SQ_D1, SQ_A4));
    {
        Movelist out;
        pos.legals<MoveGenType::PAWN>(out);
        auto pin_mask = pos.state()._pin_mask;
        REQUIRE(out.size() == 12);
        REQUIRE(pin_mask != 0);
        auto pin_maskAND0x8000000000000ULL = pin_mask & 0x8000000000000ULL;
        REQUIRE(pin_maskAND0x8000000000000ULL != 0);
        REQUIRE(pin_mask == 0x8040201000000ULL);
    }
}

TEST_CASE("Perft startpos") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        {     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1,        20 },
        {     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2,       400 },
        {     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3,      8902 },
        {     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4,    197281 },
        {     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5,   4865609 },
#if IS_RELEASE
        {     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324 },
#endif
        {   "rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR b KQkq - 0 1", 4,    328511 },
        { "rnbqkbnr/pp1ppppp/2p5/8/8/3P4/PPP1PPPP/RNBQKBNR w KQkq - 0 2", 3,     15206 },
        {   "rnbqkbnr/pp1ppppp/2p5/8/8/3P4/PPPKPPPP/RNBQ1BNR b kq - 1 2", 2,       463 },
        {  "rnb1kbnr/pp1ppppp/2p5/q7/8/3P4/PPPKPPPP/RNBQ1BNR w kq - 2 3", 1,         4 }
    };
    check_perfts(tests);
}
TEST_CASE("Perft 7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        {  "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 1,     8 },
        {  "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 2,    33 },
        {  "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 3,   270 },
        { "7k/8/8/8/1p6/P7/1P6/6K1 b - - 0 1", 2,    38 },
        {   "7k/8/8/8/8/p7/1P6/6K1 w - - 0 2", 1,     8 },
        {   "7k/8/8/8/8/p7/1P6/6K1 w - - 0 2", 2,    36 },
        { "7k/8/8/8/1p6/P7/1P6/6K1 b - - 0 1", 3,   234 },
        {    "7k/8/8/8/8/pP6/8/6K1 b - - 0 2", 1,     4 },
        {  "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 4,  1664 },
        {  "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 5, 13931 },
        {  "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 6, 94162 }
    };
    check_perfts(tests);
}
TEST_CASE("Massive legal counts") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 1,      216 },
        { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 2,       95 },
        { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 3,    18138 },
        { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 4,    80766 },
#if IS_RELEASE
        { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 5, 11649711 }
#endif
    };
    check_perfts(tests);
}
TEST_CASE("EP 8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1", 1,   7 },
        { "8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1", 2, 108 }
    };
    check_perfts(tests);
}
TEST_CASE("kiwipete") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        {  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 1,        48 },
        {  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2,      2039 },
        {  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 3,     97862 },
#if IS_RELEASE
        {  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4,   4085603 },
        {  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5, 193690690 },
        { "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1", 4,   4627439 },
#endif
        {   "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/PpN2Q1p/1PPBBPPP/R3K2R w KQkq - 0 2", 3,    103849 },
        {   "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/PPN2Q1p/1P1BBPPP/R3K2R b KQkq - 0 2", 2,      2098 },
        {  "r3k2r/p1ppqpb1/1n2pnp1/3PN3/4P3/PPNb1Q1p/1P1BBPPP/R3K2R w KQkq - 1 3", 1,        46 },
        {   "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/P1p2Q1p/1PPBBPPP/R3K2R w KQkq - 0 2", 3,     98073 },
        {     "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/P1p2Q1p/RPPBBPPP/4K2R b Kkq - 1 2", 2,      1926 },
        {       "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/P4Q1p/RPPpBPPP/4K2R w Kkq - 0 3", 1,         3 }
    };

    check_perfts(tests);
}
TEST_CASE("P[3] (chessprogramming)") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 1,       14 },
        { "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 2,      191 },
        { "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 3,     2812 },
        { "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 4,    43238 },
        { "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5,   674624 },
        { "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 6, 11030083 }
    };
    check_perfts(tests);
}
TEST_CASE("P[4] (normal) (chessprogramming)") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 1,        6 },
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 2,      264 },
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 3,     9467 },
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4,   422333 },
#if IS_RELEASE
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5, 15833292 },
        //{ "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6, 706045033 },
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P1RPP/R2Q2K1 b kq - 1 1", 4,  2703427 },
#endif
        {  "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/Pp1P1RPP/R2Q2K1 w kq - 0 2", 3,    58801 },
        {  "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/Pp1P1RPP/R2Q1K2 b kq - 1 2", 2,     1732 },
        {  "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/P2P1RPP/Rq1Q1K2 w kq - 0 3", 1,       30 },
        {    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/8/Pp1P1RPP/R2q1K2 w kq - 0 3", 1,        2 }
    };
    check_perfts(tests);
}
TEST_CASE("P[4] (mirrored) (chessprogramming)") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 1,        6 },
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 2,      264 },
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 3,     9467 },
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 4,   422333 },
#if IS_RELEASE
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 5, 15833292 },
    //{ "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 6, 706045033 }
#endif
    };
    check_perfts(tests);
}
TEST_CASE("P[5] (chessprogramming)") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 1,       44 },
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 2,     1486 },
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 3,    62379 },
#if IS_RELEASE
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4,  2103487 },
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5, 89941194 }
#endif
    };
    check_perfts(tests);
}
TEST_CASE("P[6] (chessprogramming)") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 1,        46 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 2,      2079 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3,     89890 },
#if IS_RELEASE
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4,   3894594 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 5, 164075551 }
#endif
    };
    check_perfts(tests);
}
TEST_CASE("Promotion") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 1,         5 },
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 2,        18 },
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 3,       176 },
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 4,       859 },
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 5,     12510 },
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 6,     63994 },
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 7,   1109712 },
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 8,   5763773 },
#if IS_RELEASE
        { "7K/5kP1/8/8/8/8/8/8 w - - 0 1", 9, 109424670 }
#endif
    };
    check_perfts(tests);
}
TEST_CASE("Minor position test perft") {
    // https://www.chessprogramming.net/perfect-perft/
    std::vector<TestEntry<std::string, perft_t>> tests = {
        {         "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888 },
        {        "8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1", 6, 1015133 },
        {       "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467 },
        {            "5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6,  661072 },
        {            "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 6,  803711 },
        { "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206 },
        {  "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", 4, 1720476 },
        {       "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", 5, 1004658 },
        {            "4k3/1P6/8/8/8/8/K7/8 w - - 0 1", 6,  217342 },
        {             "8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6,   92683 },
        {             "K1k5/8/P7/8/8/8/8/8 w - - 0 1", 6,    2217 },
        {            "8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7,  567584 },
        {         "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 4,   23527 }
    };
    check_perfts(tests);
}
TEST_CASE("Random position perfts") {
    std::vector<TestEntry<std::string, perft_t>> positions = {
        {       "6k1/2nNrq2/bb1P2NR/P2bqqN1/qpKQ3n/2Rn2B1/B1r1N3/1Q1Nb3 w - - 0 1", 5,        0 }, // checkmate
        {                                    "K7/3n3R/1r4k1/8/8/8/2p5/8 w - - 0 1", 5,  1056215 },
        { "1Qn2n1k/P1K2P2/1pr2nrP/1p2Q1b1/bPP2B2/p2B1q1N/2Rpp1pn/Q3Q1N1 w - - 0 1", 1,        2 },
#if IS_RELEASE
        { "1Qn2n1k/P1K2P2/1pr2nrP/1p2Q1b1/bPP2B2/p2B1q1N/2Rpp1pn/Q3Q1N1 w - - 0 1", 5, 11921325 },
#endif
        {                                          "8/7K/8/8/8/5R2/8/k7 b - - 0 1", 1,        3 },
        {                                          "8/7K/8/8/8/5R2/8/k7 b - - 0 1", 2,       57 },
        {                                          "8/7K/8/8/8/5R2/8/k7 b - - 0 1", 3,      261 },
        {                                          "8/7K/8/8/8/5R2/8/k7 b - - 0 1", 4,     4897 },
        {                                          "8/7K/8/8/8/5R2/8/k7 b - - 0 1", 5,    22176 },
        {                                          "8/7K/8/8/8/5R2/8/k7 b - - 0 1", 6,   413723 },
        {                                          "8/7K/8/8/8/5R2/8/k7 b - - 0 1", 7,  2109569 },
#if IS_RELEASE
        {                                          "8/7K/8/8/8/5R2/8/k7 b - - 0 1", 8, 39470570 },
#endif
        {                                   "8/5K1p/r7/6q1/2p5/5qk1/8/8 w - - 0 1", 1,        1 },
        {                                   "8/5K1p/r7/6q1/2p5/5qk1/8/8 w - - 0 1", 5,    17218 },
        {          "3n2r1/2k2B1q/3B2Q1/N1n1P3/1r1R1NQ1/2brB1r1/PQB5/bK6 b - - 0 1", 1,        1 },
#if IS_RELEASE
        {          "3n2r1/2k2B1q/3B2Q1/N1n1P3/1r1R1NQ1/2brB1r1/PQB5/bK6 b - - 0 1", 5,  7037862 },
#endif
        {                          "4q2R/BQ1Np2k/4pn2/bK1N4/4p3/1n6/8/8 b - - 0 1", 1,        4 },
#if IS_RELEASE
        {                          "4q2R/BQ1Np2k/4pn2/bK1N4/4p3/1n6/8/8 b - - 0 1", 5,  3945793 },
#endif
        {                                   "1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1", 1,        5 },
        {                                   "1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1", 5,   188155 },
        {                                   "1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1", 6,  5537444 },
#if IS_RELEASE
        {                                   "1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1", 7, 33107724 },
#endif
        {              "k2bR1B1/NR3B2/2BR4/1N1p1bN1/N3b3/7r/B2n1KN1/2Q5 w - - 0 1", 1,       64 },
#if IS_RELEASE
        {              "k2bR1B1/NR3B2/2BR4/1N1p1bN1/N3b3/7r/B2n1KN1/2Q5 w - - 0 1", 4,  2641743 },
#endif
        {                                      "4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1", 1,        6 },
        {                                      "4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1", 5,    71831 },
#if IS_RELEASE
        {                                      "4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1", 6,  1376826 },
#endif
        {                    "3q3B/3Q4/1k1N4/5Q2/6Q1/1B2N2Q/K4bB1/2q2N2 b - - 0 1", 1,       35 },
#if IS_RELEASE
        {                    "3q3B/3Q4/1k1N4/5Q2/6Q1/1B2N2Q/K4bB1/2q2N2 b - - 0 1", 4,  5855605 },
#endif
        {                                        "8/1K6/8/8/6n1/1k6/8/8 w - - 0 1", 1,        8 },
        {                                        "8/1K6/8/8/6n1/1k6/8/8 w - - 0 1", 5,    50950 },
#if IS_RELEASE
        {                                        "8/1K6/8/8/6n1/1k6/8/8 w - - 0 1", 7,  3844295 },
#endif
#if IS_RELEASE
        {                                        "8/1K6/8/8/6n1/1k6/8/8 w - - 0 1", 8, 47694794 },
#endif
        {                               "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 1,       63 },
        {                               "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 2,      168 },
        {                               "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 3,    10739 },
        {                               "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 4,    28349 },
#if IS_RELEASE
        {                               "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 5,  1804208 },
        {                               "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 6,  5073106 },
#endif
        {                                    "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 1,       13 },
        {                                    "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 2,      376 },
        {                                    "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 3,     3875 },
        {                                    "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 4,   112556 },
        {                                    "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 5,  1189238 },
        {                    "2K2q1q/2n4n/3qb3/1r2qq2/5n2/k4n2/r5Q1/1q6 w - - 0 1", 1,        0 }  // checkmate
    };
    check_perfts(positions);
}
TEST_CASE("Experienced bugs in this repo") {
    std::vector<TestEntry<std::string, perft_t>> positions = {
        { "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 1,    25 },
        { "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 2,   701 },
        { "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 3, 17762 }
    };
    check_perfts(positions);
    {
        Position p("rnbqkbnr/pp1p1ppp/8/2pPp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 3");
        REQUIRE(p.zobrist() == p.hash());
        REQUIRE(p.hash() == 11114434025341711937ULL);
    }
    {
        Position p("rnbqkbnr/pppp1ppp/8/3PpP2/8/8/PPP2PPP/RNBQKBNR w KQkq e6 0 2");
        REQUIRE(p.zobrist() == p.hash());
        REQUIRE(p.hash() == 11926398512926811756ULL);
    }
    {
        Position p("r4rk1/2p1qpp1/p1np1n1p/1pb1p1B1/P1B1P1b1/1PNP1N2/2P1QPPP/R4RK1 w - - 0 12");
        REQUIRE(p.zobrist() == p.hash());
        REQUIRE(p.hash() == 221975837765752100ULL);
    }
    {
        Position p;
        p.doMove(Move(SQ_A2, SQ_A3));
        p.doMove(Move(SQ_B8, SQ_A6));
        p.doMove(Move(SQ_F2, SQ_F3));
        p.doMove(Move(SQ_F7, SQ_F5));
        REQUIRE(p.zobrist() == p.hash());
        REQUIRE(p.hash() == 4177524090105507023);
    }
}
TEST_CASE("Captures only?") {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        { "rn1qkbnr/ppp1pppp/3p4/6B1/6b1/3P4/PPP1PPPP/RN1QKBNR w KQkq - 2 3", 17, 6732 },
        { "rn1qkbnr/ppp1pppp/3p4/6B1/6b1/3P4/PPP1PPPP/RN1QKBNR w KQkq - 2 3", 30,  360 },
        {                              "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1",  1,    1 }
    };
    check_perfts<MoveGenType::CAPTURE>(tests);
}
struct moveToUci_t {
    std::string destCmp;
};
void check_moveToUci_t(std::vector<TestEntry<Move, moveToUci_t>> &tests) {
    for (auto &tc : tests) {
        REQUIRE(chess::uci::moveToUci(tc.input) == tc.info.destCmp);
    }
}
TEST_CASE("moveToUci") {
    std::vector<TestEntry<Move, moveToUci_t>> tests = {
        { Move(SQ_A2, SQ_A4), "a2a4" },
        { Move::make<NORMAL>(SQ_H1, SQ_G4), "h1g4" },
        { Move::make<NORMAL>(SQ_A4, SQ_E6), "a4e6" },
        { Move::make<NORMAL>(SQ_D4, SQ_A7), "d4a7" },
        { Move::make<EN_PASSANT>(SQ_C5, SQ_E6), "c5e6" },
        { Move::make<NORMAL>(SQ_G7, SQ_D2), "g7d2" },
        { Move::make<NORMAL>(SQ_A5, SQ_E6), "a5e6" },
        { Move::make<NORMAL>(SQ_D3, SQ_F7), "d3f7" },
        { Move::make<NORMAL>(SQ_E8, SQ_A4), "e8a4" },
        { Move::make<NORMAL>(SQ_B4, SQ_G3), "b4g3" },
        { Move::make<NORMAL>(SQ_A6, SQ_C8), "a6c8" },
        { Move::make<NORMAL>(SQ_B7, SQ_B2), "b7b2" },
        { Move::make<NORMAL>(SQ_F8, SQ_G1), "f8g1" },
        { Move::make<NORMAL>(SQ_B2, SQ_B1), "b2b1" },
        { Move::make<NORMAL>(SQ_D2, SQ_H8), "d2h8" },
        { Move::make<EN_PASSANT>(SQ_B4, SQ_H3), "b4h3" },
        { Move::make<NORMAL>(SQ_F7, SQ_E5), "f7e5" },
        { Move::make<NORMAL>(SQ_A6, SQ_C2), "a6c2" },
        { Move::make<NORMAL>(SQ_C8, SQ_H5), "c8h5" },
        { Move::make<NORMAL>(SQ_C3, SQ_G7), "c3g7" },
        { Move::make<CASTLING>(SQ_E8, SQ_H8), "e8g8" },
        { Move::make<NORMAL>(SQ_B4, SQ_A8), "b4a8" },
        { Move::make<NORMAL>(SQ_F3, SQ_G5), "f3g5" },
        { Move::make<NORMAL>(SQ_E4, SQ_F6), "e4f6" },
        { Move::make<EN_PASSANT>(SQ_D5, SQ_B6), "d5b6" },
        { Move::make<PROMOTION>(SQ_H2, SQ_G1, BISHOP), "h2g1b" },
        { Move::make<NORMAL>(SQ_E5, SQ_C8), "e5c8" },
        { Move::make<NORMAL>(SQ_D8, SQ_F1), "d8f1" },
        { Move::make<NORMAL>(SQ_H3, SQ_H8), "h3h8" },
        { Move::make<NORMAL>(SQ_G6, SQ_A1), "g6a1" },
        { Move::make<CASTLING>(SQ_E8, SQ_A8), "e8c8" },
        { Move::make<CASTLING>(SQ_E1, SQ_H1), "e1g1" },
        { Move::make<CASTLING>(SQ_E8, SQ_H8), "e8g8" },
        { Move::make<NORMAL>(SQ_C7, SQ_B1), "c7b1" },
        { Move::make<CASTLING>(SQ_E8, SQ_H8), "e8g8" },
        { Move::make<NORMAL>(SQ_D8, SQ_A1), "d8a1" },
        { Move::make<PROMOTION>(SQ_D7, SQ_D8, KNIGHT), "d7d8n" },
        { Move::make<NORMAL>(SQ_D8, SQ_E7), "d8e7" },
        { Move::make<NORMAL>(SQ_F2, SQ_H3), "f2h3" },
        { Move::make<NORMAL>(SQ_B7, SQ_D5), "b7d5" },
        { Move::make<NORMAL>(SQ_D1, SQ_F2), "d1f2" },
        { Move::make<NORMAL>(SQ_E8, SQ_C4), "e8c4" },
        { Move::make<NORMAL>(SQ_D2, SQ_C7), "d2c7" },
        { Move::make<NORMAL>(SQ_H1, SQ_C2), "h1c2" },
        { Move::make<NORMAL>(SQ_E3, SQ_C1), "e3c1" },
        { Move::make<NORMAL>(SQ_H8, SQ_H3), "h8h3" },
        { Move::make<PROMOTION>(SQ_F2, SQ_F1, QUEEN), "f2f1q" },
        { Move::make<NORMAL>(SQ_H2, SQ_D6), "h2d6" },
        { Move::make<PROMOTION>(SQ_G2, SQ_G1, ROOK), "g2g1r" },
        { Move::make<NORMAL>(SQ_H5, SQ_G6), "h5g6" },
        { Move::make<NORMAL>(SQ_E8, SQ_E3), "e8e3" },
        { Move::make<EN_PASSANT>(SQ_H4, SQ_C3), "h4c3" },
        { Move::make<EN_PASSANT>(SQ_E4, SQ_H3), "e4h3" },
        { Move::make<NORMAL>(SQ_D2, SQ_A1), "d2a1" },
        { Move::make<NORMAL>(SQ_H1, SQ_D2), "h1d2" },
        { Move::make<NORMAL>(SQ_B8, SQ_A3), "b8a3" },
        { Move::make<NORMAL>(SQ_H5, SQ_A1), "h5a1" },
        { Move::make<CASTLING>(SQ_E1, SQ_A1), "e1c1" },
        { Move::make<NORMAL>(SQ_A2, SQ_A3), "a2a3" },
        { Move::make<NORMAL>(SQ_C7, SQ_G8), "c7g8" },
        { Move::make<NORMAL>(SQ_E1, SQ_E6), "e1e6" },
        { Move::make<NORMAL>(SQ_E1, SQ_A5), "e1a5" },
        { Move::make<NORMAL>(SQ_E6, SQ_A5), "e6a5" },
        { Move::make<PROMOTION>(SQ_H7, SQ_C8, QUEEN), "h7c8q" },
        { Move::make<PROMOTION>(SQ_C2, SQ_C1, QUEEN), "c2c1q" },
        { Move::make<NORMAL>(SQ_D6, SQ_B7), "d6b7" },
        { Move::make<NORMAL>(SQ_D5, SQ_G7), "d5g7" },
        { Move::make<NORMAL>(SQ_B6, SQ_F3), "b6f3" },
        { Move::make<NORMAL>(SQ_A3, SQ_G1), "a3g1" },
        { Move::make<PROMOTION>(SQ_D2, SQ_A1, BISHOP), "d2a1b" },
        { Move::make<NORMAL>(SQ_H6, SQ_C3), "h6c3" },
        { Move::make<PROMOTION>(SQ_C7, SQ_C8, KNIGHT), "c7c8n" },
        { Move::make<PROMOTION>(SQ_A7, SQ_B8, KNIGHT), "a7b8n" },
        { Move::make<NORMAL>(SQ_C6, SQ_C5), "c6c5" },
        { Move::make<PROMOTION>(SQ_H2, SQ_H1, QUEEN), "h2h1q" },
        { Move::make<NORMAL>(SQ_G1, SQ_G8), "g1g8" },
        { Move::make<NORMAL>(SQ_D1, SQ_H8), "d1h8" },
        { Move::make<NORMAL>(SQ_B1, SQ_C6), "b1c6" },
        { Move::make<EN_PASSANT>(SQ_E4, SQ_A3), "e4a3" },
        { Move::make<NORMAL>(SQ_G5, SQ_F3), "g5f3" },
        { Move::make<PROMOTION>(SQ_D7, SQ_D8, KNIGHT), "d7d8n" },
        { Move::make<NORMAL>(SQ_C4, SQ_E5), "c4e5" },
        { Move::make<NORMAL>(SQ_B1, SQ_F7), "b1f7" },
        { Move::make<NORMAL>(SQ_B3, SQ_B4), "b3b4" },
        { Move::make<PROMOTION>(SQ_G7, SQ_G8, BISHOP), "g7g8b" },
        { Move::make<NORMAL>(SQ_A1, SQ_E3), "a1e3" },
        { Move::make<NORMAL>(SQ_F6, SQ_E3), "f6e3" },
        { Move::make<NORMAL>(SQ_B7, SQ_A2), "b7a2" },
        { Move::make<NORMAL>(SQ_F7, SQ_D4), "f7d4" },
        { Move::make<NORMAL>(SQ_G4, SQ_F6), "g4f6" },
        { Move::make<CASTLING>(SQ_E1, SQ_A1), "e1c1" },
        { Move::make<NORMAL>(SQ_E1, SQ_A1), "e1a1" },
        { Move::make<NORMAL>(SQ_H1, SQ_H8), "h1h8" },
        { Move::make<NORMAL>(SQ_A1, SQ_A7), "a1a7" },
        { Move::make<NORMAL>(SQ_E5, SQ_A8), "e5a8" },
        { Move::make<NORMAL>(SQ_F5, SQ_F8), "f5f8" },
        { Move::make<NORMAL>(SQ_E7, SQ_D2), "e7d2" },
        { Move::make<PROMOTION>(SQ_H2, SQ_E1, BISHOP), "h2e1b" },
        { Move::make<EN_PASSANT>(SQ_H4, SQ_F3), "h4f3" },
        { Move::make<NORMAL>(SQ_E8, SQ_B6), "e8b6" },
        { Move::make<NORMAL>(SQ_H5, SQ_E8), "h5e8" },
        { Move::make<PROMOTION>(SQ_F2, SQ_F1, ROOK), "f2f1r" },
    };
    check_moveToUci_t(tests);
}
TEST_CASE("push_uci/parse_uci") {
    Position p;
    REQUIRE(p.parse_uci("e2e4") == Move(SQ_E2, SQ_E4));
    p.setFEN("rn1qkbnr/pP1ppppp/8/1b6/8/8/PPP1PPPP/RNBQKBNR w KQkq - 1 5");
    REQUIRE(p.parse_uci("b7a8q") == Move::make<PROMOTION>(SQ_B7, SQ_A8, QUEEN));
}
struct was_into_check_t {
    bool was_into_check;
};
template <typename T> void check_was_into_check_t(std::vector<TestEntry<std::string, was_into_check_t>> &tests) {
    for (auto &tc : tests) {
        _Position<T> pos(tc.input);
        REQUIRE(pos.was_into_check() == tc.info.was_into_check);
    }
}
TEST_CASE("was_into_check") {
    std::vector<TestEntry<std::string, was_into_check_t>> tests = {
        {                "3K4/1N1Q4/8/2r4k/6r1/8/1q1Q4/r3P3 w q - 24 70", false },
        {                         "8/8/3b3K/4N3/4k3/7n/4q3/7B w - - 8 9",  true },
        {            "N7/1B4P1/4B2n/2Kn4/8/3k4/Pp2r1p1/1p5P w - - 29 85", false },
        {                 "4b3/3n3N/8/2k5/1q6/1B6/4R3/2K1R3 w - g3 37 3", false },
        {                   "8/r7/1K3p2/2q5/2R5/6N1/3B4/2p4k w q - 2 89",  true },
        {           "8/1q3Pr1/2b5/2P2r2/7K/5k2/PP1n4/Q2b3p w K g3 28 35", false },
        {               "K7/1r6/4B1p1/7p/3P4/4k3/4R3/1np3b1 w - - 16 84",  true },
        {         "2R2r2/2k2N2/7K/2r2NR1/1n3b2/PP6/3q3b/B7 w q h3 38 21",  true },
        {        "2Q5/3b4/1b4Rr/4R1K1/7q/2P2R1q/R5Qp/1bn1k3 w - g3 8 76",  true },
        {            "4k3/bP1r4/n7/8/2PP2K1/4B3/2N1P1q1/7P w q a6 37 18", false },
        {        "1P4N1/2Nb4/1K6/5B2/2N3R1/2k1Nn2/5N2/1p3p2 b q - 41 98", false },
        {                  "Q4rN1/5R2/R5b1/7r/8/r7/kK5B/7B b K c6 37 73",  true },
        {     "4k2p/r1q5/7n/1p4P1/1P3PQr/N1n2N2/7K/1N1NqB2 b - f6 27 80",  true },
        {             "7Q/2p2b2/8/3K4/5Q2/N2B1bb1/5b2/5qk1 w K d3 30 68", false },
        {              "3b3K/5p2/2Pp4/2n1R3/5Qb1/7k/6n1/R7 b - a3 23 54", false },
        {             "1n1PN1N1/3Pn3/3r4/2b5/6K1/7r/5b2/3k4 w q - 15 51", false },
        {          "4N3/4kN2/n2p4/4K3/r1P4R/1p4n1/b3r3/3P1n2 b - - 9 43",  true },
        {           "1Q2q3/k4bBq/5B1P/8/6q1/r1q3p1/1K6/2PP4 w - - 16 90",  true },
        {                 "Kqq1B2b/3R1Q2/8/k7/6P1/8/1P4R1/8 w - e6 33 9", false },
        {              "7K/Nn3b2/r1N1pP2/4R3/5q2/kq6/4p2b/8 w - e3 5 11", false },
        {                   "n3NK1B/B5kn/8/8/6pB/4P3/8/5R2 b k g3 20 41",  true },
        {                  "Kq3Bq1/6p1/4P3/k7/3R4/8/1Npn4/8 b K - 31 84",  true },
        {          "7N/1RK1p1nb/5n2/3N4/QQ2pkr1/1r6/3P1P2/8 b - - 44 77", false },
        {      "1R2RN1q/nn2Q1k1/5b1Q/3n2n1/2qN3Q/1K1R2B1/8/8 w - - 49 2",  true },
        {            "8/5bQn/3n2B1/1K2B3/4NN1n/6q1/4R3/2q4k w - - 8 100", false },
        {            "8/8/1P4P1/7r/1Q1Q4/2R2P1b/6n1/1RbK1Q1k b - - 44 5", false },
        {             "2r2p2/1B6/2r5/5p2/8/3b2q1/2R3qK/k1q5 b - - 11 89",  true },
        {            "1b1Q4/1rk2qP1/8/R1r4q/4p3/2K4p/5P2/R7 b Q - 23 29",  true },
        {         "qq3B2/2r4n/3n4/r1Q5/2P5/5b2/1b1k1K2/P2n4 w Q - 49 89", false },
        {        "B1bn1R1r/1N5k/2p5/4b2r/6n1/K5B1/2n1n3/1P3N2 b q - 6 8",  true },
        {             "8/6q1/3nr3/3n4/KQ6/1bp2q2/1Bk5/2r2Q2 w - b3 0 89", false },
        {                    "4r3/p1n4N/8/8/8/4qnP1/6k1/4K3 b K - 12 83",  true },
        {            "3b1Nk1/4R1nR/3r4/3P4/R7/K2n4/1qB5/4r2R b q - 6 73",  true },
        {        "6r1/1Q1B2B1/1bK2QR1/1r6/r7/4p2B/8/2r1k1q1 b - - 44 39",  true },
        {                    "5R2/b7/3p2P1/8/2k5/qK2Q3/7p/8 w K - 33 11",  true },
        {               "1R3Q1Q/p7/8/4B1pq/6k1/2N1Q3/8/6K1 w - - 35 100", false },
        {              "Bp6/PQ5q/8/4k3/7b/2q1Nr2/2K5/2n1q2q w K - 26 14", false },
        {               "3p4/4k1Q1/5B1b/n1q5/8/2Q4K/3Q4/4B3 b q - 15 33", false },
        {            "p7/p5N1/B2n3Q/K4R1P/3n2q1/7k/P3QR2/8 b k e6 16 50", false },
        {                  "5Pkq/8/7N/2n5/1N4R1/7K/1Q6/3ppP2 w Q - 9 43",  true },
        {                 "6K1/8/Q4B2/1Q6/b1k5/4q1n1/8/1B5b w K - 41 68",  true },
        {               "1N6/2k4K/2P3r1/8/3R2N1/5b2/5b2/b3p3 w k - 1 50", false },
        {                 "3r1K2/1R6/p7/8/1qN3q1/1r6/1k5Q/8 w Q - 27 30",  true },
        {                  "p2B2P1/Q1b5/1Q6/K3k3/8/P4P2/8/8 b q - 42 98", false },
        {                     "BQ5n/5n2/8/7K/8/2B5/1k6/4n1N1 b K - 3 47", false },
        {             "8/n3B3/8/R7/3Q2P1/1rQ2B2/5R2/2K1k2r w k - 46 100",  true },
        {         "1n1N4/2Qr1k1N/1KR3N1/3Pn3/4Q3/4B3/pn2q3/8 w k - 10 9",  true },
        {      "2b5/1q2Bbq1/5n2/2R3qR/2kN4/6b1/P1r4Q/K3r2B b k h3 41 49",  true },
        {                "2q5/4br1R/5Q2/5Q2/2k5/Bnp5/7q/3K4 w - - 18 39", false },
        {               "8/3q4/7k/5K2/1q2R1N1/3N4/2N5/3P2p1 w K - 17 21",  true },
        {     "1P1BP1bK/N5r1/7R/1p3B1Q/6n1/3r4/k1B5/2n1P1q1 b K - 35 23", false },
        {            "2bP1P2/2p5/3K3r/4Q3/2b5/4kr2/q7/pq5r w - g6 21 63",  true },
        {            "K1B5/4P1B1/kN3Q1R/3P1B2/2q1QN2/8/8/2P5 w Q - 16 2",  true },
        { "7N/2b1p2P/1RK5/Nbqn1P2/3p2p1/2qr4/P1nP2k1/P1p1b3 w - d3 9 95", false },
        {              "2B3p1/2R3b1/8/p7/8/Kprk4/3b2b1/2Q5 b - b6 15 77", false },
        {       "1r2b3/2R5/1k1p2PQ/7N/3p2R1/P1R1K3/1bq5/R7 w - d6 41 72", false },
        {              "3rq3/2p4K/4n3/5nn1/k7/8/2n1Q3/3P1q2 b k - 35 89",  true },
        {       "K3R3/1P4NB/1q3R2/1k6/1qn2B2/3pr3/3q1B2/4b3 b - - 18 88", false },
        {             "N1r5/3nP3/bR2Q3/2RB3N/7k/3K4/b7/nQ5b w k - 31 78", false },
        {              "b7/8/3b2P1/3kr3/4R3/6K1/3q2b1/1q2r3 w K - 28 28", false },
        {                "2B1p2n/8/P7/Nn3b1K/1q6/N2p1kR1/8/8 b q - 0 36", false },
        {            "1RK3p1/2BNr3/6R1/6q1/8/b2Pq3/8/2RR1P1k w K - 24 3", false },
        {            "5P2/5Q2/8/2K2rb1/n3p1p1/6b1/2qn4/k4p2 b K - 15 60",  true },
        {             "2b1p3/5N2/2NNR1R1/2q5/8/8/qNQ4k/1P5K w k - 11 18",  true },
        {       "5B2/3q3K/1k3n2/R5r1/1Q2bR2/4P1b1/3B1b2/3pR3 w K - 2 22",  true },
        {          "4K3/8/1r6/3kNQp1/7b/Rr3B2/rP3nq1/1nP1q3 b Q h6 4 85", false },
        {    "Q2rKN2/3Pp3/n5k1/4nPN1/3r3Q/1N1r4/5p2/1BBR1B1B w q - 6 18",  true },
        {              "1NK5/3k1Q2/1B2N1Q1/7p/8/8/5RNN/6Q1 w - g3 16 90",  true },
        {                    "8/4Q3/6QP/3k4/bN2K3/4Q3/5b2/8 w k - 33 38",  true },
        {             "6K1/6q1/QnRb4/1Np4Q/7R/3Q4/3B1q2/5k2 b q - 29 11",  true },
        {                "5n2/4q3/4r3/b7/8/1Q5k/5KRB/1q2r3 b - d6 20 39", false },
        {                   "1R4q1/8/K2B4/5Q2/1N6/Q2qk3/8/8 w - - 10 61", false },
        {             "1q1k4/4q3/8/Q1n5/3b3B/2Qp3b/R6K/5n1n b K - 47 62",  true },
        {              "2PB4/q3BR2/8/N3k3/4P3/4P3/7K/1N2P3 w k c6 22 24", false },
        {                     "6Pp/5N2/8/3QN3/1B6/p7/6kK/Q7 b K - 29 24",  true },
        {                 "8/R7/4pk2/5R2/1BK3R1/6Q1/1P1Q4/8 w Q - 10 75",  true },
        {                     "8/6p1/b3QN2/5k2/8/p6P/7r/K7 b - f6 18 46", false },
        {              "3Q4/5P2/7N/3N2K1/8/3rk1bn/7b/1NQ1Q3 b k - 17 42",  true },
        {           "5B2/1P2k3/2n3K1/5bn1/5R1P/p3Bp2/3N4/4n3 w - - 6 94",  true },
        {             "4P3/4n1Q1/6b1/5k2/5N2/3n3B/7p/q2B2K1 w - - 16 65",  true },
        {                       "8/8/1k3r2/Nq6/7n/8/8/3K1b2 b - - 27 10", false },
        {                     "8/5N2/1K6/8/8/2Bq4/k7/3b2qp b K f3 39 75",  true },
        {        "kbK1R3/2n2q2/7n/1p6/1r1bP3/2p5/nB5B/2N2b2 w - - 12 68", false },
        {           "8/3b4/2k1pn2/2P4n/2qQp3/1nnb4/r7/KB1Q4 b - - 12 11",  true },
        {            "r3B3/1bb2k2/p7/3K2Q1/8/6bp/6Nb/2p2nB1 b - - 22 61",  true },
        {       "1b6/4p2k/6p1/1N1Q1n2/1pK1r2B/1q2b3/p3Rp2/8 w k - 30 99", false },
        {                      "Q2k1B2/8/3rb3/8/2K5/8/1N6/8 w Q - 43 73",  true },
        {                   "8/1r4Nk/2R5/3Q1P2/8/6Q1/7P/6K1 w K - 27 13", false },
        {             "5k2/2b5/p7/5B2/1K2n3/6n1/1b4q1/3r1b2 b K - 28 91", false },
        {         "5r2/1N6/8/5N1r/4n2k/2B2K2/1RR1NB2/2p1BR2 w - f6 8 12",  true },
        {           "5N2/8/5K2/8/2B2P2/5nr1/1Q1B2k1/1Qr1n3 b - - 29 100", false },
        {           "8/3b4/3p4/1K5n/3Q1Qn1/2n5/k1p1Rq1n/3N4 b Q - 39 12",  true },
        {              "2Q5/3n4/1Rq1KN2/3P4/7P/1b3B2/3k3q/8 b - - 15 47",  true },
        {         "3bn3/4Q1p1/2r3q1/2Q2k2/2QKr3/3R4/8/2p2R2 w Q - 28 77",  true },
        {         "n7/1n4p1/2KB2b1/4q1R1/QR6/1kb3p1/8/2n2b2 b K - 24 73", false },
        {            "3R2k1/QpK2R1p/N6P/RP2r3/q4b2/8/1R6/2r5 w q - 7 44",  true },
        {              "8/k1R2Q2/3r4/2QP3B/8/6B1/3K4/n1b2Q2 b Q - 18 53",  true },
        {           "p7/2n3PQ/3p1K2/N2b4/2n2bk1/Q7/B1Q5/2q5 b - - 47 54", false },
        {        "b2Q1q2/1r6/2R5/B1R3q1/p2K1kR1/4q3/8/1r3b2 b - - 15 92",  true },
        {            "pB6/4q1nk/7n/4p2B/1N3K2/P7/rn2bRb1/2r5 w - - 7 57", false },
    };
    check_was_into_check_t<PolyglotPiece>(tests);
    check_was_into_check_t<EnginePiece>(tests);
}
int main(int argc, char **argv) {
    doctest::Context ctx;
    ctx.setOption("success", true);
    ctx.setOption("no-breaks", true);
    return ctx.run();
}
