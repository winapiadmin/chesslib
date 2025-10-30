#include <iostream>
#include <chrono>
#include "../position.h"
using namespace chess;
struct TTEntry {
    uint64_t hash;
    uint64_t nodes;
    int depth;
};
std::vector<TTEntry> tt;
template<int Depth, bool EnableDiv = false, MoveGenType MGen = MoveGenType::ALL>
uint64_t perft(Position& p) {
    static_assert(Depth >= 0, "Negative depth in perft template!");
    if constexpr (Depth == 0)
        return 1;   
    else if constexpr (Depth == 1) {
        ValueList<Move, 256> moves;
        p.legals<MGen>(moves);
        if constexpr (EnableDiv)
            for (const Move& m : moves)
            {
                std::cout << m << ": 1\n";
            }
        return moves.size();
    }
    else {
        const uint64_t hash = p.hash();
        TTEntry& entry = tt[hash & (tt.size() - 1)];
        if (entry.hash == hash && entry.depth == Depth)
            return entry.nodes;

        ValueList<Move, 256> moves;
        p.legals<MGen>(moves);

        uint64_t total = 0;
        for (const Move& m : moves) {
            p.doMove(m);
            const uint64_t nodes = perft<Depth - 1, false, MGen>(p);
            total += nodes;
            p.undoMove();

            if constexpr (EnableDiv)
                std::cout << m << ": " << nodes << '\n';
        }

        entry.hash = hash;
        entry.depth = Depth;
        entry.nodes = total;
        return total;
    }
}

int main() {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    tt.resize(1 << 28);
    //pos.doMove(Move(SQ_D2, SQ_D4));
    //pos.doMove(Move(SQ_A7, SQ_A6));
    //pos.doMove(Move(SQ_E1, SQ_D2));
    //pos.doMove(Move(SQ_C7, SQ_C5));
    //pos.doMove(Move(SQ_D4, SQ_C5));
    //pos.doMove(Move(SQ_D7, SQ_D5));
    using namespace std::chrono;
    auto start_time = high_resolution_clock::now();
    uint64_t nodes = perft<7, true>(pos);
    auto end_time = high_resolution_clock::now();

    double elapsed = duration<double>(end_time - start_time).count(); // seconds
    double mnps = (nodes / elapsed) / 1'000'000.0;

    std::cout << "Depth 7 perft: " << nodes << " nodes\n";
    std::cout << "Time: " << elapsed << " s\n";
    std::cout << "Speed: " << mnps << " Mnps\n";

}