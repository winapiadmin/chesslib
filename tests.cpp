#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "position.h"
using namespace chess;
template<int Depth, bool EnableDiv = false, MoveGenType MGen = MoveGenType::ALL>
uint64_t perft(Position& p) {
    if constexpr (Depth == 0) {
        return 1;
    }
    else if constexpr (Depth == 1) {
        ValueList<Move, 256> moves;
        p.legals<MGen>(moves);
        for (const Move& m : moves) {
            if constexpr (EnableDiv)
                std::cout << m << ": 1\n";
        }
        return moves.size();
    }
    else {
        ValueList<Move, 256> moves;
        p.legals<MGen>(moves);

        uint64_t total = 0;
        for (const Move& m : moves) {
            p.doMove(m);
            const uint64_t nodes = perft<Depth - 1, false, MGen>(p);
            p.undoMove();

            if constexpr (EnableDiv)
                std::cout << m << ": " << nodes << '\n';

            total += nodes;
        }

        if constexpr (EnableDiv)
            std::cout << "Total: " << total << '\n';

        return total;
    }
}
TEST_CASE("Castling move making FEN rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8") {
    std::string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    Position position(fen);
    REQUIRE(position.hash() == 0x4f874e21f78d3590);
    position.doMove(Move::make<CASTLING>(SQ_E1, SQ_H1));
    REQUIRE(position.hash() == 0x31F235158B7EEE80);
    REQUIRE(position.fen() == "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQ1RK1 b - - 2 8");
}
TEST_CASE("EP move making FEN 8/5k1K/8/8/4Pp2/8/8/8 b - e3 0 1") {
    std::string fen = "8/5k1K/8/8/4Pp2/8/8/8 b - e3 0 1";
    Position position(fen);
    REQUIRE(position.hash() == 0x775d0e2acc42db8c);
    position.doMove(Move::make<EN_PASSANT>(SQ_F4, SQ_E3));
    REQUIRE(position.hash() == 0x3ec71faae73cfbed);
    REQUIRE(position.fen() == "8/5k1K/8/8/8/4p3/8/8 w - - 0 2");
}
TEST_CASE("EP move making FEN 8/5k1K/8/3pP3/8/8/8/8 w - d6 0 2") {
    std::string fen = "8/5k1K/8/3pP3/8/8/8/8 w - d6 0 2";
    Position position(fen);
    REQUIRE(position.hash()==0xbdc108e30cccd00b);
    position.doMove(Move::make<EN_PASSANT>(SQ_E5, SQ_D6));
    REQUIRE(position.fen() == "8/5k1K/3P4/8/8/8/8/8 b - - 0 2");
    REQUIRE(position.hash() == 0x5c57e65793a2c8fe);
}
TEST_CASE("Promotion FEN 8/5kP1/7K/8/8/8/8/8 w - - 0 1") {
    std::string fen = "8/5kP1/7K/8/8/8/8/8 w - - 0 1";
    Position position(fen);
    REQUIRE(position.hash()==0x3793b24e1b95a6d5);
    position.doMove(Move::make<PROMOTION>(SQ_G7, SQ_G8));
    REQUIRE(position.fen() == "6N1/5k2/7K/8/8/8/8/8 b - - 0 1");
    REQUIRE(position.hash() == 0x6adb9bf15f32cb92);
}

uint64_t perft_pawn(int depth, Position &p){
    ValueList<Move, 256> out;
    p.legals<MoveGenType::PAWN>(out);
    if (depth==1) return out.size();
    uint64_t s=0;
    for (auto m:out){
        p.doMove(m);
        s+=perft_pawn(depth-1, p);
        p.undoMove();
    }
    return s;
}
TEST_CASE("Perft pawn-only startpos") {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position position(fen);
    REQUIRE(perft_pawn(5, position)==815968);
}
TEST_CASE("Perft pawn-only startpos depth 6") {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position position(fen);
    REQUIRE(perft_pawn(6, position)==11515584);
}
TEST_CASE("EP replace on non-EP move"){
    Position position("rnbqkbnr/pppppp2/7p/6pP/8/8/PPPPPPP1/RNBQKBNR w KQkq g6 0 3");
    position.doMove(Move(SQ_E2, SQ_E4));
    REQUIRE(position.fen()=="rnbqkbnr/pppppp2/7p/6pP/4P3/8/PPPP1PP1/RNBQKBNR b KQkq e3 0 3");
}
TEST_CASE("EP ignore on non-EP move"){
    Position position("rnbqkbnr/pppppp2/7p/6pP/8/8/PPPPPPP1/RNBQKBNR w KQkq g6 0 3");
    position.doMove(Move(SQ_E2, SQ_E3));
    REQUIRE(position.fen()=="rnbqkbnr/pppppp2/7p/6pP/8/4P3/PPPP1PP1/RNBQKBNR b KQkq - 0 3");
}
uint64_t perft_king(int depth, _Position<EnginePiece, void> &p){
    ValueList<Move, 256> out;
    p.legals<MoveGenType::KING>(out);
    if (depth==1) return out.size();
    uint64_t s=0;
    for (auto m:out){
        p.doMove(m);
        s+=perft_king(depth-1, p);
        p.undoMove();
    }
    return s;
}
TEST_CASE("King walk empty board wking e4 bking f8 depth 6") {
    std::string fen = "5k2/8/8/8/3K4/8/8/8 w - - 0 1";
    Position position(fen);
    REQUIRE(perft_king(6, position)==95366);
}
TEST_CASE("Pin detection double push movegen") {
	std::string fen = "rnbqkbnr/1ppppppp/8/p7/Q1P5/8/PP1PPPPP/RNB1KBNR b KQkq - 1 2";
	Position position(fen);
    ValueList<Move, 256> out;
    position.legals<MoveGenType::PAWN>(out);
    auto pin_mask = position.state()._pin_mask;
    REQUIRE(out.size() == 12);
    REQUIRE(pin_mask!=0);
    auto pin_maskAND0x8000000000000ULL = pin_mask & 0x8000000000000ULL;
    REQUIRE(pin_maskAND0x8000000000000ULL !=0);
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
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    REQUIRE(perft<1, false>(pos) == 20);
    REQUIRE(perft<2, false>(pos) == 400);
    REQUIRE(perft<3, false>(pos) == 8902);
    REQUIRE(perft<4, false>(pos) == 197281);
    REQUIRE(perft<5, false>(pos) == 4865609);
#ifndef _DEBUG
    REQUIRE(perft<6, false>(pos) == 119060324);
#endif
    pos.doMove(Move(SQ_D2, SQ_D3));
    REQUIRE(perft<4, false>(pos) == 328511);
    pos.doMove(Move(SQ_C7, SQ_C6));
    REQUIRE(perft<3, false>(pos) == 15206);
    pos.doMove(Move(SQ_E1, SQ_D2));
    REQUIRE(perft<2, false>(pos) == 463);
    pos.doMove(Move(SQ_D8, SQ_A5));
    REQUIRE(perft<1, false>(pos) == 4);
}
TEST_CASE("Perft 7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1") {
    Position pos("7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1");
    REQUIRE(perft<1, false>(pos) == 8);
    REQUIRE(perft<2, false>(pos) == 33);
    REQUIRE(perft<3, false>(pos) == 270);
    pos.doMove(Move(SQ_A2, SQ_A3)); //  -> a2a3
    REQUIRE(perft<2, false>(pos) == 38);
    pos.doMove(Move(SQ_B4, SQ_A3)); // a2a3 -> a2a3 b4a3
    REQUIRE(perft<1, false>(pos) == 8);
    pos.undoMove(); // a2a3 b4a3 -> a2a3
    REQUIRE(perft<3, false>(pos) == 234);
    pos.doMove(Move(SQ_B4, SQ_A3)); // a2a3 -> a2a3 b4a3
    REQUIRE(perft<2, false>(pos) == 36);
    pos.doMove(Move(SQ_B2, SQ_B3)); // a2a3 b4a3 -> a2a3 b4b3 b2b3
    REQUIRE(perft<1, false>(pos) == 4);
    pos.undoMove(); // a2a3 b4b3 b2b3 -> a2a3 b4a3
    pos.undoMove(); pos.undoMove(); // a2a3 b4a3 -> a2a3 -> 
    REQUIRE(perft<4, false>(pos) == 1664);
    REQUIRE(perft<6, false>(pos) == 94162);
}
TEST_CASE("Massive legal counts") {
    {
        Position pos("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1"); // #1 amount of legals for white
        REQUIRE(perft<1>(pos) == 216);
        REQUIRE(perft<2>(pos) == 95);
        REQUIRE(perft<3>(pos) == 18138);
        REQUIRE(perft<4>(pos) == 80766);
        REQUIRE(perft<5>(pos) == 11649711);
    }
}
TEST_CASE("EP 8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1") {
    Position pos("8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1");
    REQUIRE(perft<1, false>(pos) == 7);
    REQUIRE(perft<2, false>(pos) == 108);
}
TEST_CASE("kiwipete") {
    Position pos("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    REQUIRE(perft<1, false>(pos) == 48);
    REQUIRE(perft<2, false>(pos) == 2039);
    REQUIRE(perft<3, false>(pos) == 97862);
    REQUIRE(perft<4, false>(pos) == 4085603);
#ifndef _DEBUG
    REQUIRE(perft<5, false>(pos) == 193690690);
#endif
    pos.doMove(Move(SQ_A2, SQ_A3));
    REQUIRE(perft<4, false>(pos) == 4627439);
    pos.doMove(Move(SQ_B4, SQ_B3));
    REQUIRE(perft<3, false>(pos) == 103849);
    pos.doMove(Move(SQ_C2, SQ_B3));
    REQUIRE(perft<2, false>(pos) == 2098);
    pos.doMove(Move(SQ_A6, SQ_D3));
    REQUIRE(perft<1, false>(pos) == 46);
    pos.undoMove(); pos.undoMove(); pos.undoMove();
    pos.doMove(Move(SQ_B4, SQ_C3));
    REQUIRE(perft<3, false>(pos) == 98073);
    pos.doMove(Move(SQ_A1, SQ_A2));
    REQUIRE(perft<2, false>(pos) == 1926);
    pos.doMove(Move(SQ_C3, SQ_D2));
    REQUIRE(perft<1, false>(pos) == 3);
}
TEST_CASE("P[3] (chessprogramming)") {
    Position pos("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    REQUIRE(perft<1, false>(pos) == 14);
    REQUIRE(perft<2, false>(pos) == 191);
    REQUIRE(perft<3, false>(pos) == 2812);
    REQUIRE(perft<4, false>(pos) == 43238);
    REQUIRE(perft<5, false>(pos) == 674624); // the most affordable
}
TEST_CASE("P[4] (normal) (chessprogramming)") {
    Position pos("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    REQUIRE(perft<1, false>(pos) == 6);
    REQUIRE(perft<2, false>(pos) == 264);
    REQUIRE(perft<3, false>(pos) == 9467);
    REQUIRE(perft<4, false>(pos) == 422333);
    REQUIRE(perft<5, false>(pos) == 15833292); // the most affordable
    pos.doMove(Move(SQ_F1, SQ_F2));
    REQUIRE(perft<4, false>(pos) == 2703427);
    pos.doMove(Move(SQ_A3, SQ_F3));
    REQUIRE(perft<3, false>(pos) == 58801);
    pos.doMove(Move(SQ_G1, SQ_F1));
    REQUIRE(perft<2, false>(pos) == 1732);
    pos.doMove(Move::make<PROMOTION, QUEEN>(SQ_B2, SQ_B1));
    REQUIRE(perft<1, false>(pos) == 30);
    pos.undoMove();
    pos.doMove(Move(SQ_F3, SQ_D1));
    REQUIRE(perft<1, false>(pos) == 2);
}
TEST_CASE("P[4] (mirrored) (chessprogramming)") {
    Position pos("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
    REQUIRE(perft<1, false>(pos) == 6);
    REQUIRE(perft<2, false>(pos) == 264);
    REQUIRE(perft<3, false>(pos) == 9467);
    REQUIRE(perft<4, false>(pos) == 422333);
    REQUIRE(perft<5, false>(pos) == 15833292); // the most affordable
}
TEST_CASE("P[5] (chessprogramming)") {
    Position pos("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    REQUIRE(perft<1, false>(pos) == 44);
    REQUIRE(perft<2, false>(pos) == 1486);
    REQUIRE(perft<3, false>(pos) == 62379);
    REQUIRE(perft<4, false>(pos) == 2103487);
#ifndef _DEBUG
    REQUIRE(perft<5, false>(pos) == 89941194); // the most affordable
#endif
}
TEST_CASE("P[6] (chessprogramming)") {
    Position pos("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    REQUIRE(perft<1, false>(pos) == 46);
    REQUIRE(perft<2, false>(pos) == 2079);
    REQUIRE(perft<3, false>(pos) == 89890);
    REQUIRE(perft<4, false>(pos) == 3894594);
#ifndef _DEBUG
    REQUIRE(perft<5, false>(pos) == 164075551);
#endif
}
TEST_CASE("Promotion") {
    Position pos("7K/5kP1/8/8/8/8/8/8 w - - 0 1");
    REQUIRE(perft<1, false>(pos) == 5);
    REQUIRE(perft<2, false>(pos) == 18);
    REQUIRE(perft<3, false>(pos) == 176);
    REQUIRE(perft<4, false>(pos) == 859);
    REQUIRE(perft<5, false>(pos) == 12510);
    REQUIRE(perft<6, false>(pos) == 63994);
    REQUIRE(perft<7, false>(pos) == 1109712);
    REQUIRE(perft<8, false>(pos) == 5763773);
#ifndef _DEBUG
    REQUIRE(perft<9, false>(pos) == 109424670);
#endif
}
TEST_CASE("Minor position test perft") {
    //https://www.chessprogramming.net/perfect-perft/
    {
        Position pos("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1");
        REQUIRE(perft<6, false>(pos) == 1134888);
    }
    {
        Position pos("8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1");
        REQUIRE(perft<6, false>(pos) == 1015133);
    }
    {
        Position pos("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1");
        REQUIRE(perft<6, false>(pos) == 1440467);
    }
    {
        Position pos("5k2/8/8/8/8/8/8/4K2R w K - 0 1");
        REQUIRE(perft<6, false>(pos) == 661072);
    }
    {
        Position pos("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1");
        REQUIRE(perft<6, false>(pos) == 803711);
    }
    {
        Position pos("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1");
        REQUIRE(perft<4, false>(pos) == 1274206);
    }
    {
        Position pos("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1");
        REQUIRE(perft<4, false>(pos) == 1720476);
    }
    {
        Position pos("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1");
        REQUIRE(perft<6, false>(pos) == 3821001);
    }
    {
        Position pos("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1");
        REQUIRE(perft<5, false>(pos) == 1004658);
    }
    {
        Position pos("4k3/1P6/8/8/8/8/K7/8 w - - 0 1");
        REQUIRE(perft<6, false>(pos) == 217342);
    }
    {
        Position pos("8/P1k5/K7/8/8/8/8/8 w - - 0 1");
        REQUIRE(perft<6, false>(pos) == 92683);
    }
    {
        Position pos("K1k5/8/P7/8/8/8/8/8 w - - 0 1");
        REQUIRE(perft<6, false>(pos) == 2217);
    }
    {
        Position pos("8/k1P5/8/1K6/8/8/8/8 w - - 0 1");
        REQUIRE(perft<7, false>(pos) == 567584);
    }
    {
        Position pos("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1");
        REQUIRE(perft<4, false>(pos) == 23527);
    }
}
struct TestEntry {
    std::string FEN;
    int depth;
    uint64_t nodes;
};
template <typename T>
uint64_t perft(_Position<T>& pos, int depth) {
    if (depth == 0) return 1;
    Movelist moves;
    pos.legals(moves);
    uint64_t nodes = 0;
    for (auto& move : moves) {
        pos.doMove(move);
        nodes += perft(pos, depth - 1);
        pos.undoMove();
    }
    return nodes;
}
template <typename T>
void check_perfts(const std::vector<TestEntry>& entries) {
    for (auto& entry : entries) {
        _Position<T> pos(entry.FEN);
        REQUIRE(perft(pos, entry.depth) == entry.nodes);
    }
}
TEST_CASE("Random position perfts") {
    std::vector<TestEntry> positions = {
        {"6k1/2nNrq2/bb1P2NR/P2bqqN1/qpKQ3n/2Rn2B1/B1r1N3/1Q1Nb3 w - - 0 1", 5, 0}, // checkmate
        {"K7/3n3R/1r4k1/8/8/8/2p5/8 w - - 0 1", 5, 1056215},
        {"1Qn2n1k/P1K2P2/1pr2nrP/1p2Q1b1/bPP2B2/p2B1q1N/2Rpp1pn/Q3Q1N1 w - - 0 1", 1, 2},
        {"1Qn2n1k/P1K2P2/1pr2nrP/1p2Q1b1/bPP2B2/p2B1q1N/2Rpp1pn/Q3Q1N1 w - - 0 1", 5, 11921325},
        {"8/7K/8/8/8/5R2/8/k7 b - - 0 1", 1, 3},
        {"8/7K/8/8/8/5R2/8/k7 b - - 0 1", 2, 57},
        {"8/7K/8/8/8/5R2/8/k7 b - - 0 1", 3, 261},
        {"8/7K/8/8/8/5R2/8/k7 b - - 0 1", 4, 4897},
        {"8/7K/8/8/8/5R2/8/k7 b - - 0 1", 5, 22176},
        {"8/7K/8/8/8/5R2/8/k7 b - - 0 1", 6, 413723},
        {"8/7K/8/8/8/5R2/8/k7 b - - 0 1", 7, 2109569},
#ifndef _DEBUG
        {"8/7K/8/8/8/5R2/8/k7 b - - 0 1", 8, 39470570},
#endif
        {"8/5K1p/r7/6q1/2p5/5qk1/8/8 w - - 0 1", 1, 1},
        {"8/5K1p/r7/6q1/2p5/5qk1/8/8 w - - 0 1", 5, 17218},
        {"3n2r1/2k2B1q/3B2Q1/N1n1P3/1r1R1NQ1/2brB1r1/PQB5/bK6 b - - 0 1", 1, 1},
        {"3n2r1/2k2B1q/3B2Q1/N1n1P3/1r1R1NQ1/2brB1r1/PQB5/bK6 b - - 0 1", 5, 7037862},
        {"4q2R/BQ1Np2k/4pn2/bK1N4/4p3/1n6/8/8 b - - 0 1", 1, 4},
        {"4q2R/BQ1Np2k/4pn2/bK1N4/4p3/1n6/8/8 b - - 0 1", 5, 3945793},
        {"1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1", 1, 5},
        {"1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1", 5, 188155},
        {"1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1", 6, 5537444},
#ifndef _DEBUG
        {"1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1", 7, 33107724},
#endif
        {"k2bR1B1/NR3B2/2BR4/1N1p1bN1/N3b3/7r/B2n1KN1/2Q5 w - - 0 1", 1, 64},
        {"k2bR1B1/NR3B2/2BR4/1N1p1bN1/N3b3/7r/B2n1KN1/2Q5 w - - 0 1", 4, 2641743},
        {"4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1", 1, 6},
        {"4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1", 5, 71831},
        {"4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1", 6, 1376826},
        {"3q3B/3Q4/1k1N4/5Q2/6Q1/1B2N2Q/K4bB1/2q2N2 b - - 0 1", 1, 35},
        {"3q3B/3Q4/1k1N4/5Q2/6Q1/1B2N2Q/K4bB1/2q2N2 b - - 0 1", 4, 5855605},
        {"8/1K6/8/8/6n1/1k6/8/8 w - - 0 1", 1, 8},
        {"8/1K6/8/8/6n1/1k6/8/8 w - - 0 1", 5, 50950},
        {"8/1K6/8/8/6n1/1k6/8/8 w - - 0 1", 7, 3844295},
#ifndef _DEBUG
        {"8/1K6/8/8/6n1/1k6/8/8 w - - 0 1", 8, 47694794},
#endif
        {"8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 1, 63},
        {"8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 2, 168},
        {"8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 3, 10739},
        {"8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 4, 28349},
        {"8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 5, 1804208},
        {"8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1", 6, 5073106},
        {"2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 1, 13},
        {"2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 2, 376},
        {"2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 3, 3875},
        {"2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 4, 112556},
        {"2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1", 5, 1189238},
        {"2K2q1q/2n4n/3qb3/1r2qq2/5n2/k4n2/r5Q1/1q6 w - - 0 1", 1, 0} // checkmate
    };
    check_perfts<EnginePiece>(positions);
    check_perfts<PolyglotPiece>(positions);
}
TEST_CASE("Experienced bugs in this repo") {
    std::vector<TestEntry> positions = {
        {"rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 1, 25},
        {"rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 2, 701},
        {"rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4", 3, 17762}
    };
    check_perfts<EnginePiece>(positions);
    check_perfts<PolyglotPiece>(positions);
}
int main(int argc, char** argv) {
    doctest::Context ctx;
    ctx.setOption("success", true);
    ctx.setOption("duration", true);
    ctx.setOption("no-breaks", true);
    return ctx.run();
}
