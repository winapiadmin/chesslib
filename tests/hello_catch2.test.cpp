#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "../position.h"
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

uint64_t perft_pawn(int depth, Position<EnginePiece, void> &p){
    ValueList<Move, 256> out;
    p.template legals<MoveGenType::PAWN>(out);
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
uint64_t perft_king(int depth, Position<EnginePiece, void> &p){
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
    REQUIRE(out.size() == 12);
    REQUIRE(position.state()._pin_mask!=0); // this was the old bug
    REQUIRE(position.state()._pin_mask & 0x8000000000000);
    REQUIRE(position.state()._pin_mask == 0x8040201000000);
}
TEST_CASE("Move making pin update") {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    pos.doMove(Move(SQ_C2, SQ_C4));
	pos.doMove(Move(SQ_A7, SQ_A5));
	pos.doMove(Move(SQ_D1, SQ_A4));
    {
		Movelist out;
        pos.legals<MoveGenType::PAWN>(out);
        REQUIRE(out.size() == 12);
        REQUIRE(pos.state()._pin_mask != 0); // this was the old bug
        REQUIRE(pos.state()._pin_mask & 0x8000000000000);
        REQUIRE(pos.state()._pin_mask == 0x8040201000000);
    }
}
template<bool EnableDiv = true, MoveGenType MGen = MoveGenType::ALL>
uint64_t perft(int depth, Position<EnginePiece, void>& p, bool isRoot = true) {
    //std::cout << p.fen() << "\n";
    if (depth == 0) {
        // No moves to generate, current position counts as one leaf
        return 1;
    }

    ValueList<Move, 256> moves;
    p.legals<MGen>(moves);

    uint64_t total = 0;

    for (const Move& m : moves) {
        p.doMove(m);

        uint64_t nodes;
        if (depth == 1) {
            // At depth 1, leaf nodes: count = 1 for each move
            nodes = 1;
        }
        else {
            nodes = perft<EnableDiv, MGen>(depth - 1, p, false);
        }

        p.undoMove();

        if (isRoot&&EnableDiv) {
            std::cout << m << ": " << nodes << '\n';
        }
        total += nodes;
    }

    if (isRoot&&EnableDiv) {
        std::cout << "Total: " << total << '\n';
    }

    return total;
}

TEST_CASE("Perft startpos") {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    REQUIRE(perft<false>(1, pos) == 20);
    REQUIRE(perft<false>(2, pos) == 400);
    REQUIRE(perft<false>(3, pos) == 8902);
	pos.doMove(Move(SQ_E2, SQ_E3));
    //REQUIRE(perft<true>(3, pos) == 13134);
    pos.doMove(Move(SQ_B7, SQ_B6));
    //REQUIRE(perft<true>(2, pos) == 622);
    pos.doMove(Move(SQ_F1, SQ_B5));
    std::cout << pos;
    REQUIRE(perft<true>(1, pos) == 18);
    //REQUIRE(perft<true>(4, pos) == 197281);
}