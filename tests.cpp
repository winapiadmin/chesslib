#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "position.h"
using namespace chess;
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

TEST_CASE("Perft startpos") {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    REQUIRE(perft<1, false>(pos) == 20);
    REQUIRE(perft<2, false>(pos) == 400);
    REQUIRE(perft<3, false>(pos) == 8902);
    REQUIRE(perft<4, false>(pos) == 197281);
    REQUIRE(perft<5, false>(pos) == 4865609);
    REQUIRE(perft<6, false>(pos) == 119060324);
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
TEST_CASE("Minor position test perft") {
    {
        Position pos("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        //REQUIRE(perft<5, true>(pos) == 193690690);
        pos.doMove(Move(SQ_A2, SQ_A3));
        //REQUIRE(perft<4, false>(pos) == 4627439);
        pos.doMove(Move(SQ_B4, SQ_B3));
        REQUIRE(perft<3, false>(pos) == 103849);
        pos.doMove(Move(SQ_C2, SQ_B3));
        REQUIRE(perft<2, false>(pos) == 2098);
        pos.doMove(Move(SQ_A6, SQ_D3));
        REQUIRE(perft<1, false>(pos) == 46);
    }
    /*{
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
    }*/
}
int main(int argc, char** argv) {
    doctest::Context ctx;
    ctx.setOption("success", true);
    ctx.setOption("duration", true);
    ctx.setOption("no-breaks", true);
    return ctx.run();
}