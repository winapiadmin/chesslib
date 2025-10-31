#define DOCTEST_CONFIG_IMPLEMENT
#include "position.h"
#include <doctest/doctest.h>
using namespace chess;
#if !defined(_DEBUG) || defined(NDEBUG)
#define IS_RELEASE 1
#else
#ifndef NDEBUG
#define IS_RELASE 1
#endif

struct TestEntry {
    std::string FEN;
    int depth;
    uint64_t nodes;
};
template <typename T, MoveGenType mt, bool EnableDiv = false>
uint64_t perft(_Position<T> &pos, int depth) {
    if (depth == 0) {
        return 1;
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
template <typename T, MoveGenType mt = MoveGenType::ALL, bool EnableDiv=false>
void check_perfts(const std::vector<TestEntry> &entries) {
    for (auto &entry : entries) {
        _Position<T> pos(entry.FEN);
        REQUIRE(perft<T, mt, EnableDiv>(pos, entry.depth) == entry.nodes);
    }
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
    std::vector<TestEntry> tests = {
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
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("Perft 7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1") {
    std::vector<TestEntry> tests = {
        { "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 1, 8 },
        { "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 2, 33 },
        { "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 3, 270 },
        { "7k/8/8/8/1p6/P7/1P6/6K1 b - - 0 1", 2, 38},
        { "7k/8/8/8/8/p7/1P6/6K1 w - - 0 2", 1, 8 },
        { "7k/8/8/8/8/p7/1P6/6K1 w - - 0 2", 2, 36 },
        { "7k/8/8/8/1p6/P7/1P6/6K1 b - - 0 1" , 3, 234},
        { "7k/8/8/8/8/pP6/8/6K1 b - - 0 2", 1, 4 },
        { "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 4, 1664 },
        { "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 5, 13931 },
        { "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1", 6, 94162 }
    };
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("Massive legal counts") {
    {
        std::vector<TestEntry> tests = {
            { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 1, 216 },
            { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 2, 95 },
            { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 3, 18138 },
            { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 4, 80766},
#if IS_RELEASE
            { "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1", 5, 11649711}
#endif
        };
        check_perfts<EnginePiece>(tests);
        check_perfts<PolyglotPiece>(tests);
    }
}
TEST_CASE("EP 8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1") {
    std::vector<TestEntry> tests = {
        { "8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1", 1, 7},
        { "8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1", 2, 108}
    };
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("kiwipete") {
    std::vector<TestEntry> tests = {
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

    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("P[3] (chessprogramming)") {
    std::vector<TestEntry> tests = {
        {  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 1, 14 },
        {  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 2, 191 },
        {  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 3, 2812 },
        {  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 4, 43238 },
        {  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5, 674624 },
        {  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 6, 11030083 }
    };
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("P[4] (normal) (chessprogramming)") {
    std::vector<TestEntry> tests = {
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 1,         6 },
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 2,       264 },
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 3,      9467 },
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4,    422333 },
#if IS_RELEASE
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5,  15833292 },
        //{ "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6, 706045033 },
        { "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P1RPP/R2Q2K1 b kq - 1 1", 4,   2703427 },
#endif
        {  "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/Pp1P1RPP/R2Q2K1 w kq - 0 2", 3,     58801 },
        {  "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/Pp1P1RPP/R2Q1K2 b kq - 1 2", 2,      1732 },
        {  "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/P2P1RPP/Rq1Q1K2 w kq - 0 3", 1,        30 },
        {    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/8/Pp1P1RPP/R2q1K2 w kq - 0 3", 1,         2 }
    };
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("P[4] (mirrored) (chessprogramming)") {
    std::vector<TestEntry> tests = {
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 1,         6 },
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 2,       264 },
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 3,      9467 },
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 4,    422333 },
#if IS_RELEASE
        { "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 5,  15833292 },
        //{ "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 6, 706045033 }
#endif
    };
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("P[5] (chessprogramming)") {
    std::vector<TestEntry> tests = {
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 1,        44 },
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 2,      1486 },
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 3,     62379 },
#if IS_RELEASE
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4,   2103487 },
        { "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5,  89941194 }
#endif
    };
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("P[6] (chessprogramming)") {
    std::vector<TestEntry> tests = {
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 1,        46 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 2,      2079 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3,     89890 },
#if IS_RELEASE
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4,   3894594 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 5, 164075551 }
#endif
    };
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("Promotion") {
    std::vector<TestEntry> tests = {
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
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("Minor position test perft") {
    // https://www.chessprogramming.net/perfect-perft/
    std::vector<TestEntry> tests = {
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
    check_perfts<EnginePiece>(tests);
    check_perfts<PolyglotPiece>(tests);
}
TEST_CASE("Random position perfts") {
    std::vector<TestEntry> positions = {
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
    check_perfts<EnginePiece>(positions);
    check_perfts<PolyglotPiece>(positions);
}
TEST_CASE("Experienced bugs in this repo") {
    std::vector<TestEntry> positions = {
        { "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 1,    25 },
        { "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 2,   701 },
        { "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 3, 17762 }
    };
    check_perfts<EnginePiece>(positions);
    check_perfts<PolyglotPiece>(positions);
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
    std::vector<TestEntry> tests = {
        { "rn1qkbnr/ppp1pppp/3p4/6B1/6b1/3P4/PPP1PPPP/RN1QKBNR w KQkq - 2 3", 17, 6732 },
        { "rn1qkbnr/ppp1pppp/3p4/6B1/6b1/3P4/PPP1PPPP/RN1QKBNR w KQkq - 2 3", 30, 360 },
        { "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 1, 1 }
    };
    check_perfts<EnginePiece, MoveGenType::CAPTURE>(tests);
}
int main(int argc, char **argv) {
    doctest::Context ctx;
    ctx.setOption("success", true);
    ctx.setOption("duration", true);
    ctx.setOption("no-breaks", true);
    return ctx.run();
}
