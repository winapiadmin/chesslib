#include <iostream>
#include <chrono>
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
        if constexpr (EnableDiv)
            for (const Move& m : moves) {
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

int main() {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    using namespace std::chrono;
    auto start_time = high_resolution_clock::now();
    uint64_t nodes = perft<6>(pos);
    auto end_time = high_resolution_clock::now();

    double elapsed = duration<double>(end_time - start_time).count(); // seconds
    double mnps = (nodes / elapsed) / 1'000'000.0;

    std::cout << "Depth 6 perft: " << nodes << " nodes\n";
    std::cout << "Time: " << elapsed << " s\n";
    std::cout << "Speed: " << mnps << " Mnps\n";

}