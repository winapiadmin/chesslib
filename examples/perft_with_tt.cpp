#include <iostream>
#include <chrono>
#include "../position.h"
#include "../printers.h"
#include <vector>
using namespace chess;
//#define TT_ENABLED
struct TTEntry {
    uint64_t hash;
    uint64_t nodes;
    int depth;
};
std::vector<TTEntry> tt;
template<int Depth, bool EnableDiv = false, MoveGenType MGen = MoveGenType::ALL> uint64_t perft(Position &p) {
    static_assert(Depth >= 0, "Negative depth in perft template!");
    if constexpr (Depth == 1) {
        Movelist moves;
        p.legals<MGen>(moves);
        if constexpr (EnableDiv)
            for (const Move &m : moves) {
                std::cout << m << ": 1\n";
            }
        return moves.size();
    } else {

#ifdef TT_ENABLED
    const uint64_t hash = p.hash();
    TTEntry &entry = tt[hash & (tt.size() - 1)];
    if (entry.hash == hash && entry.depth == Depth)
        return entry.nodes;
#endif
        Movelist moves;
        p.legals<MGen>(moves);

        uint64_t total = 0;
        for (const Move &m : moves) {
            p.doMove<false>(m);
            const uint64_t nodes = perft<Depth - 1, false, MGen>(p);
            total += nodes;
            p.undoMove();

            if constexpr (EnableDiv)
                std::cout << m << ": " << nodes << '\n';
        }

#ifdef TT_ENABLED
        entry.hash = hash;
        entry.depth = Depth;
        entry.nodes = total;
#endif
        return total;
    }
}
template <bool EnableDiv = false, MoveGenType MGen = MoveGenType::ALL> uint64_t perft(Position &p, int Depth) {
    if (Depth == 1) {
        Movelist moves;
        p.legals<MGen>(moves);
        if constexpr (EnableDiv)
            for (const Move &m : moves) {
                std::cout << m << ": 1\n";
            }
        return moves.size();
    } else {
#ifdef TT_ENABLED
    const uint64_t hash = p.hash();
    TTEntry &entry = tt[hash & (tt.size() - 1)];
    if (entry.hash == hash && entry.depth == Depth)
        return entry.nodes;
#endif
        Movelist moves;
        p.legals<MGen>(moves);

        uint64_t total = 0;
        for (const Move &m : moves) {
            p.doMove<false>(m);
            const uint64_t nodes = perft<false, MGen>(p, Depth - 1);
            total += nodes;
            p.undoMove();

            if constexpr (EnableDiv)
                std::cout << m << ": " << nodes << '\n';
        }
#ifdef TT_ENABLED
        entry.hash = hash;
        entry.depth = Depth;
        entry.nodes = total;
#endif
        return total;
    }
}
int main() {
    using namespace std::chrono;
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    #ifdef TT_ENABLED
    tt.resize(1 << 28);
    #endif
    auto start_time = high_resolution_clock::now();
    uint64_t nodes = //perft(pos, 7);
    perft<7, false>(pos);
    auto end_time = high_resolution_clock::now();

    double elapsed = duration<double>(end_time - start_time).count(); // seconds
    double mnps = (nodes / elapsed) / 1'000'000.0;

    std::cout << "Depth 7 perft: " << nodes << " nodes\n";
    std::cout << "Time: " << elapsed << " s\n";
    std::cout << "Speed: " << mnps << " Mnps\n";
}