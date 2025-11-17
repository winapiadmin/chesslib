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
template <int Depth, typename T=EnginePiece, bool EnableDiv = false, MoveGenType MGen = MoveGenType::ALL> inline uint64_t perft(_Position<T> &p) {
    static_assert(Depth >= 0, "Negative depth in perft template!");
    if constexpr (Depth == 1) {
        Movelist moves;
        p.template legals<MGen>(moves);
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
        p.template legals<MGen>(moves);

        uint64_t total = 0;
        for (const Move &m : moves) {
            p.template doMove<false>(m);
            const uint64_t nodes = perft<Depth - 1, T, false, MGen>(p);
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
template <bool EnableDiv = false, typename T=EnginePiece, MoveGenType MGen = MoveGenType::ALL> inline uint64_t perft(_Position<T> &p, int Depth) {
    if (Depth != 1) {
#ifdef TT_ENABLED
        const uint64_t hash = p.hash();
        TTEntry &entry = tt[hash & (tt.size() - 1)];
        if (entry.hash == hash && entry.depth == Depth)
            return entry.nodes;
#endif
        Movelist moves;
        p.template legals<MGen>(moves);

        uint64_t total = 0;
        for (const Move &m : moves) {
            p.template doMove<false>(m);
            const uint64_t nodes = perft<false, T, MGen>(p, Depth - 1);
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
    } else {
        Movelist moves;
        p.template legals<MGen>(moves);
        if constexpr (EnableDiv)
            for (const Move &m : moves) {
                std::cout << m << ": 1\n";
            }
        return moves.size();
    }
}
template <int DEPTH, bool EnableDiv=false, typename T=EnginePiece> void benchmark(_Position<T> &pos) {
    using namespace std::chrono;
    #ifndef TT_ENABLED
    constexpr int N_RUNS = EnableDiv?1:5;
    #else
    constexpr int N_RUNS=1;
    #endif
    uint64_t nodes = 0;
    double totalTime = 0.0;

    std::cout << "Running " << N_RUNS << " benchmark iterations:\n";

    for (int i = 0; i < N_RUNS; ++i) {
        auto start = high_resolution_clock::now();
        nodes = perft<DEPTH, T, EnableDiv>(pos);
        auto end = high_resolution_clock::now();
        double elapsed = duration<double>(end - start).count();
        totalTime += elapsed;

        double mnps = (nodes / elapsed) / 1'000'000.0;
        std::cout << "Run " << (i + 1) << ": " << elapsed << " s, " << mnps << " Mnps\n";
    }

    double avgTime = totalTime / N_RUNS;
    double avgMnps = (nodes / avgTime) / 1'000'000.0;

    std::cout << "Average: " << avgTime << " s, " << avgMnps << " Mnps (nodes="<<(nodes/N_RUNS)<<")\n";
}


int main() {
    _Position<EnginePiece> pos;

#ifdef TT_ENABLED
    tt.resize(1 << 28);
#endif
    benchmark<7, true, EnginePiece>(pos);
    // Movelist moves;
    // pos.template legals(moves);
    // for (const Move &m : moves) {
    //     std::cout << m << ": 1\n";
    // }
}
