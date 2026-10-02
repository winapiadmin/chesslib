#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <cstring>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <limits>
#include <functional>
#include "position.h"
#include "printers.h"

using namespace chess;

/* ===================== Transposition Table ===================== */

struct TTEntry {
    uint64_t key = 0;
    uint64_t nodes = 0;
    uint8_t depth = 0;
};

class PerftTT {
public:
    PerftTT() = default;

    explicit PerftTT(size_t bits) {
        resize(bits);
    }

    void resize(size_t bits) {
        if (bits >= sizeof(size_t) * 8)
            throw std::runtime_error("TT size is too large");

        const size_t count = size_t(1) << bits;

        table.clear();
        table.resize(count);
        mask = count - 1;
    }

    inline bool probe(uint64_t key, int depth, uint64_t &out) const {
        if (table.empty())
            return false;

        const TTEntry &e = table[key & mask];

        if (e.key == key && e.depth == static_cast<uint8_t>(depth)) {
            out = e.nodes;
            return true;
        }

        return false;
    }

    inline void store(uint64_t key, int depth, uint64_t nodes) {
        if (table.empty())
            return;

        TTEntry &e = table[key & mask];

        if (e.key == 0 ||
            e.key == key ||
            depth >= static_cast<int>(e.depth)) {

            e.key = key;
            e.depth = static_cast<uint8_t>(depth);
            e.nodes = nodes;
        }
    }

    void clear() {
        for (auto &e : table)
            e.key = 0;
    }

private:
    std::vector<TTEntry> table;
    size_t mask = 0;
};

/*
 * IMPORTANT:
 *
 * There is deliberately NO global TT.
 *
 * Each worker owns its own PerftTT. The previous implementation had
 * multiple threads concurrently reading/writing the same TTEntry,
 * which was a C++ data race and therefore undefined behavior.
 */

/* ===================== Perft core ===================== */

template <int D, bool tb = true, bool div = true>
uint64_t perft(Position &pos, PerftTT *tt)
{
    static_assert(D >= 1);

    if constexpr (D == 1) {
        if constexpr (div) {
            Movelist moves;
            pos.legals(moves);
            for (const Move &m : moves)
                std::cout << m << ": 1\n";
            return moves.size();
        } else {
            CountOnlyList moves;
            pos.legals(moves);
            return moves.size();
        }
    } else {
        uint64_t key = 0;

        if constexpr (tb) {
            key = pos.hash();

            uint64_t cached;

            if (tt != nullptr && tt->probe(key, D, cached))
                return cached;
        }

        Movelist moves;
        pos.legals(moves);

        uint64_t nodes = 0;

        for (const Move &m : moves) {
            pos.doMove<false>(m);

            uint64_t child_nodes =
                perft<D - 1, tb, false>(pos, tt);

            nodes += child_nodes;

            if constexpr (div)
                std::cout << m << ": " << child_nodes << '\n';

            pos.undoMove();
        }

        if constexpr (tb) {
            if (tt != nullptr)
                tt->store(key, D, nodes);
        }

        return nodes;
    }
}

/* ===================== Runtime-depth dispatch ===================== */

template <bool tb = true>
uint64_t perft_no_div(Position &pos, int depth, PerftTT *tt)
{
    switch (depth) {
    case 0:
        return 1;

    case 1:
        return perft<1, tb, false>(pos, tt);

    case 2:
        return perft<2, tb, false>(pos, tt);

    case 3:
        return perft<3, tb, false>(pos, tt);

    case 4:
        return perft<4, tb, false>(pos, tt);

    case 5:
        return perft<5, tb, false>(pos, tt);

    case 6:
        return perft<6, tb, false>(pos, tt);

    case 7:
        return perft<7, tb, false>(pos, tt);

    case 8:
        return perft<8, tb, false>(pos, tt);

    case 9:
        return perft<9, tb, false>(pos, tt);

    case 10:
        return perft<10, tb, false>(pos, tt);

    case 11:
        return perft<11, tb, false>(pos, tt);

    case 12:
        return perft<12, tb, false>(pos, tt);

    default:
        return 0;
    }
}

template <bool tb = true>
uint64_t perftdiv(Position &pos, int depth, PerftTT *tt)
{
    switch (depth) {
    case 0:
        return 1;

    case 1:
        return perft<1, tb>(pos, tt);

    case 2:
        return perft<2, tb>(pos, tt);

    case 3:
        return perft<3, tb>(pos, tt);

    case 4:
        return perft<4, tb>(pos, tt);

    case 5:
        return perft<5, tb>(pos, tt);

    case 6:
        return perft<6, tb>(pos, tt);

    case 7:
        return perft<7, tb>(pos, tt);

    case 8:
        return perft<8, tb>(pos, tt);

    case 9:
        return perft<9, tb>(pos, tt);

    case 10:
        return perft<10, tb>(pos, tt);

    case 11:
        return perft<11, tb>(pos, tt);

    case 12:
        return perft<12, tb>(pos, tt);

    default:
        std::cout
            << "info string Unsupported depth (max 12 for this build)\n";

        return 0;
    }
}

/* ===================== Threaded perft ===================== */

struct ThreadResult {
    Move move;
    uint64_t nodes = 0;
};

struct PerftTask {
    const Position *pos;
    int depth;
    bool use_tt;
    Move move;
};

/*
 * Each worker gets its OWN TT.
 *
 * No mutex is required for TT access.
 * No atomics are required for TT entries.
 *
 * The only shared state is:
 *   - tasks      : immutable
 *   - root pos   : immutable
 *   - results    : one unique slot per task
 *   - next_idx   : atomic work distributor
 *   - print_mtx  : stdout serialization
 */
static void thread_worker(
    const std::vector<PerftTask> &tasks,
    std::vector<ThreadResult> &results,
    std::atomic<size_t> &next_idx,
    std::mutex &print_mtx,
    bool print_div,
    size_t tt_bits)
{
    PerftTT local_tt;

    if (tt_bits != 0)
        local_tt.resize(tt_bits);

    for (;;) {
        const size_t idx =
            next_idx.fetch_add(1, std::memory_order_relaxed);

        if (idx >= tasks.size())
            break;

        const PerftTask &task = tasks[idx];

        /*
         * Each worker has its own Position.
         *
         * The root Position is never modified by workers.
         */
        Position local = *task.pos;

        local.doMove<false>(task.move);

        uint64_t nodes;

        if (task.use_tt) {
            nodes = perft_no_div<true>(
                local,
                task.depth - 1,
                &local_tt
            );
        } else {
            nodes = perft_no_div<false>(
                local,
                task.depth - 1,
                nullptr
            );
        }

        /*
         * Each task owns exactly one results[idx].
         * Therefore this write does not race with another worker.
         */
        results[idx].move = task.move;
        results[idx].nodes = nodes;

        if (print_div) {
            std::lock_guard<std::mutex> lock(print_mtx);

            std::cout
                << task.move
                << ": "
                << nodes
                << '\n';
        }
    }
}

static uint64_t run_threaded_perft(
    Position &pos,
    int depth,
    size_t tt_bits,
    int threads,
    bool print_div)
{
    if (depth < 0)
        return 0;

    if (depth == 0)
        return 1;

    Movelist moves;
    pos.legals(moves);

    const size_t n_moves = moves.size();

    if (n_moves == 0)
        return 0;

    const bool use_tt = tt_bits != 0;

    std::vector<PerftTask> tasks(n_moves);

    for (size_t i = 0; i < n_moves; ++i) {
        tasks[i].pos = &pos;
        tasks[i].depth = depth;
        tasks[i].use_tt = use_tt;
        tasks[i].move = moves[i];
    }

    std::vector<ThreadResult> results(n_moves);

    std::atomic<size_t> next_idx{0};

    std::mutex print_mtx;

    const int n_threads =
        std::max(
            1,
            std::min(
                threads,
                static_cast<int>(n_moves)
            )
        );

    std::vector<std::thread> thread_pool;
    thread_pool.reserve(n_threads);

    for (int t = 0; t < n_threads; ++t) {
        thread_pool.emplace_back(
            thread_worker,
            std::cref(tasks),
            std::ref(results),
            std::ref(next_idx),
            std::ref(print_mtx),
            print_div,
            tt_bits
        );
    }

    for (auto &t : thread_pool)
        t.join();

    uint64_t total = 0;

    for (const auto &r : results)
        total += r.nodes;

    return total;
}

/* ===================== Helpers ===================== */

static unsigned hardware_threads()
{
    const unsigned n = std::thread::hardware_concurrency();
    return n ? n : 1;
}

static size_t mb_to_tt_bits(int mb)
{
    if (mb <= 0)
        return 0;

    /*
     * TTEntry may contain padding, so sizeof(TTEntry) is deliberately
     * used rather than assuming a particular entry size.
     */
    const uint64_t bytes =
        static_cast<uint64_t>(mb) * 1024ULL * 1024ULL;

    const uint64_t entries =
        bytes / sizeof(TTEntry);

    if (entries < 1)
        return 0;

    size_t bits = 0;

    /*
     * Find the largest power-of-two table that fits in the requested
     * memory.
     */
    while (bits + 1 < sizeof(size_t) * 8) {
        const uint64_t next = 1ULL << (bits + 1);

        if (next > entries)
            break;

        ++bits;
    }

    return bits;
}

/* ===================== Perft command ===================== */

static void run_perft(
    Position &pos,
    int depth,
    size_t tt_bits,
    int threads)
{
    if (depth < 0) {
        std::cout
            << "info string Depth must be non-negative\n";
        return;
    }

    try {
        const auto start =
            std::chrono::steady_clock::now();

        /*
         * For benchmark/UCI use, division output is disabled here.
         *
         * Printing every root move can dominate the benchmark.
         */
        const uint64_t nodes =
            run_threaded_perft(
                pos,
                depth,
                tt_bits,
                threads,
                true
            );

        const auto end =
            std::chrono::steady_clock::now();

        const std::chrono::duration<double> elapsed =
            end - start;

        const uint64_t milliseconds =
            static_cast<uint64_t>(
                elapsed.count() * 1000.0
            );

        const uint64_t nps =
            elapsed.count() > 0.0
                ? static_cast<uint64_t>(
                    nodes / elapsed.count()
                  )
                : 0;

        std::cout
            << "time "
            << milliseconds
            << '\n';

        std::cout
            << "nps "
            << nps
            << '\n';

        std::cout
            << "nodes searched: "
            << nodes
            << std::endl;

    } catch (const std::exception &e) {
        std::cout
            << "info string Perft error: "
            << e.what()
            << '\n';

        std::cout.flush();
    }
}

/* ===================== CLI usage ===================== */

static void print_usage(const char *prog)
{
    std::cerr
        << "Usage: " << prog << " [options]\n"
        << "  -f,--fen FEN    FEN string (default: startpos)\n"
        << "  -d,--depth N    Perft depth (default: 6)\n"
        << "  -m,--hash MB    Hash size in MB (default: 0)\n"
        << "  -z,--ttsize N   TT size in bits, raw override (default: 0)\n"
        << "  -t,--threads N  Number of threads (default: 1)\n"
        << "\n"
        << "No arguments: enter UCI mode.\n"
        << "\n"
        << "UCI commands:\n"
        << "  uci\n"
        << "  isready\n"
        << "  setoption name Threads value <N>\n"
        << "  setoption name Hash value <MB>\n"
        << "  ucinewgame\n"
        << "  position [startpos | fen <FEN>] [moves <moves>]\n"
        << "  go perft <depth> [hash <MB>] [threads <N>]\n"
        << "  quit\n";
}

/* ===================== Tokenization ===================== */

static std::vector<std::string>
split_args(const std::string &cmd)
{
    std::vector<std::string> tokens;

    std::istringstream iss(cmd);
    std::string tok;

    while (iss >> tok)
        tokens.push_back(tok);

    return tokens;
}

/* ===================== Standalone CLI ===================== */

static void uci_command_line(int argc, char **argv)
{
    std::string fen = Position::START_FEN;

    size_t tt_bits = 0;

    int depth = 6;
    int threads = 1;

    for (int i = 1; i < argc; ++i) {
        if ((std::strcmp(argv[i], "-f") == 0 ||
             std::strcmp(argv[i], "--fen") == 0) &&
            i + 1 < argc) {

            fen = argv[++i];

        } else if ((std::strcmp(argv[i], "-d") == 0 ||
                    std::strcmp(argv[i], "--depth") == 0) &&
                   i + 1 < argc) {

            depth = std::atoi(argv[++i]);

        } else if ((std::strcmp(argv[i], "-z") == 0 ||
                    std::strcmp(argv[i], "--ttsize") == 0) &&
                   i + 1 < argc) {

            tt_bits = std::stoul(argv[++i]);

        } else if ((std::strcmp(argv[i], "-m") == 0 ||
                    std::strcmp(argv[i], "--hash") == 0) &&
                   i + 1 < argc) {

            tt_bits = mb_to_tt_bits(
                std::atoi(argv[++i])
            );

        } else if ((std::strcmp(argv[i], "-t") == 0 ||
                    std::strcmp(argv[i], "--threads") == 0) &&
                   i + 1 < argc) {

            threads = std::atoi(argv[++i]);

        } else if (std::strcmp(argv[i], "-h") == 0 ||
                   std::strcmp(argv[i], "--help") == 0) {

            print_usage(argv[0]);
            return;
        }
    }

    Position pos;

    try {
        if (!pos.setFEN(fen))
            throw std::runtime_error("invalid FEN");
    }
    catch (const std::exception &e) {
        std::cerr
            << "Invalid FEN: "
            << e.what()
            << '\n';

        return;
    }

    std::cerr
        << "FEN: "
        << fen
        << '\n';

    std::cerr
        << "Depth: "
        << depth
        << " | Threads: "
        << threads
        << " | TT: ";

    if (tt_bits != 0)
        std::cerr
            << (1ULL << tt_bits)
            << " entries";
    else
        std::cerr
            << "disabled";

    std::cerr << '\n';

    const auto start =
        std::chrono::steady_clock::now();

    const uint64_t nodes =
        run_threaded_perft(
            pos,
            depth,
            tt_bits,
            threads,
            true
        );

    const auto end =
        std::chrono::steady_clock::now();

    const std::chrono::duration<double> elapsed =
        end - start;

    std::cout
        << "Perft("
        << depth
        << ") = "
        << nodes
        << '\n';

    std::cout
        << "Time = "
        << elapsed.count()
        << " s\n";

    std::cout
        << "Speed = "
        << (
            elapsed.count() > 0.0
                ? static_cast<uint64_t>(
                    nodes / elapsed.count()
                  )
                : 0
        )
        << " nps\n";
}

/* ===================== UCI ===================== */

static void uci_loop()
{
    Position pos;
    pos.setFEN(Position::START_FEN);

    /*
     * Hash is disabled by default.
     *
     * This now agrees with the CLI and the actual initial state.
     */
    size_t tt_bits = 0;

    int threads = 1;

    std::string line;

    while (std::getline(std::cin, line)) {
        const auto tokens = split_args(line);

        if (tokens.empty())
            continue;

        const std::string &cmd = tokens[0];

        /* ---------- quit ---------- */

        if (cmd == "quit") {
            break;
        }

        /* ---------- uci ---------- */

        else if (cmd == "uci") {
            std::cout
                << "id name chesslib-perft\n";

            std::cout
                << "id author winapiadmin\n";

            std::cout
                << "option name Threads "
                << "type spin "
                << "default 1 "
                << "min 1 "
                << "max 1024\n";

            std::cout
                << "option name Hash "
                << "type spin "
                << "default 0 "
                << "min 0 "
                << "max 65536\n";

            std::cout
                << "uciok\n";

            std::cout.flush();
        }

        /* ---------- isready ---------- */

        else if (cmd == "isready") {
            std::cout
                << "readyok\n";

            std::cout.flush();
        }

        /* ---------- setoption ---------- */

        else if (cmd == "setoption") {
            /*
             * setoption name <name> value <value>
             */

            size_t i =
                tokens.size() >= 2 &&
                tokens[1] == "name"
                    ? 2
                    : 1;

            std::string opt_name;

            while (i < tokens.size() &&
                   tokens[i] != "value") {

                if (!opt_name.empty())
                    opt_name += ' ';

                opt_name += tokens[i];
                ++i;
            }

            if (i < tokens.size() &&
                tokens[i] == "value") {

                ++i;
            }

            std::string opt_val;

            while (i < tokens.size()) {
                if (!opt_val.empty())
                    opt_val += ' ';

                opt_val += tokens[i];
                ++i;
            }

            try {
                if (opt_name == "Threads") {
                    const int t =
                        std::stoi(opt_val);

                    threads =
                        std::max(
                            1,
                            std::min(t, 1024)
                        );

                } else if (opt_name == "Hash") {
                    const int mb =
                        std::stoi(opt_val);

                    tt_bits =
                        mb_to_tt_bits(mb);
                }

            } catch (const std::exception &) {
                std::cout
                    << "info string Invalid setoption value\n";

                std::cout.flush();
            }
        }

        /* ---------- ucinewgame ---------- */

        else if (cmd == "ucinewgame") {
            pos.setFEN(Position::START_FEN);

            /*
             * There is no global TT anymore.
             *
             * Every perft worker creates a fresh TT for its run,
             * so there is nothing to clear here.
             */
        }

        /* ---------- position ---------- */

        else if (cmd == "position") {
            size_t i = 1;

            if (i < tokens.size() &&
                tokens[i] == "startpos") {

                pos.setFEN(Position::START_FEN);
                ++i;

            } else if (i < tokens.size() &&
                       tokens[i] == "fen") {

                ++i;

                std::string fen_str;

                /*
                 * FEN consists of six space-separated fields.
                 * Stop when "moves" begins.
                 */
                while (i < tokens.size() &&
                       tokens[i] != "moves") {

                    if (!fen_str.empty())
                        fen_str += ' ';

                    fen_str += tokens[i];
                    ++i;
                }

                if (fen_str.empty()) {
                    std::cout
                        << "info string Missing FEN value\n";

                    std::cout.flush();
                    continue;
                }

                try {
                    if (!pos.setFEN(fen_str))
                        throw std::runtime_error(
                            "invalid FEN"
                        );
                }
                catch (const std::exception &e) {
                    std::cout
                        << "info string Invalid FEN: "
                        << e.what()
                        << '\n';

                    std::cout.flush();
                    continue;
                }

            } else {
                std::cout
                    << "info string Expected 'startpos' or 'fen'\n";

                std::cout.flush();
                continue;
            }

            /* ---------- position moves ---------- */

            if (i < tokens.size() &&
                tokens[i] == "moves") {

                ++i;

                while (i < tokens.size()) {
                    try {
                        pos.push_uci(tokens[i]);
                    }
                    catch (const std::exception &e) {
                        std::cout
                            << "info string Invalid move "
                            << tokens[i]
                            << ": "
                            << e.what()
                            << '\n';

                        break;
                    }

                    ++i;
                }
            }
        }

        /* ---------- go ---------- */

        else if (cmd == "go") {
            int depth = -1;

            size_t local_tt = tt_bits;

            int local_threads = threads;

            bool perft_cmd = false;

            size_t i = 1;

            while (i < tokens.size()) {
                if (tokens[i] == "perft" &&
                    i + 1 < tokens.size()) {

                    depth =
                        std::stoi(tokens[i + 1]);

                    perft_cmd = true;

                    i += 2;

                } else if (tokens[i] == "threads" &&
                           i + 1 < tokens.size()) {

                    local_threads =
                        std::stoi(tokens[i + 1]);

                    local_threads =
                        std::max(
                            1,
                            std::min(
                                local_threads,
                                1024
                            )
                        );

                    i += 2;

                } else if (tokens[i] == "hash" &&
                           i + 1 < tokens.size()) {

                    local_tt =
                        mb_to_tt_bits(
                            std::stoi(tokens[i + 1])
                        );

                    i += 2;

                } else if (tokens[i] == "ttsize" &&
                           i + 1 < tokens.size()) {

                    local_tt =
                        std::stoul(tokens[i + 1]);

                    i += 2;

                } else {
                    /*
                     * Ignore unsupported UCI go parameters.
                     */
                    ++i;
                }
            }

            if (!perft_cmd || depth < 0) {
                std::cout
                    << "info string Usage: "
                    << "go perft <depth> "
                    << "[threads <N>] "
                    << "[hash <MB>] "
                    << "[ttsize <bits>]\n";

                std::cout.flush();
                continue;
            }

            run_perft(
                pos,
                depth,
                local_tt,
                local_threads
            );
        }

        /* ---------- unknown ---------- */

        else {
            std::cout
                << "info string Unknown command: "
                << cmd
                << '\n';

            std::cout.flush();
        }
    }
}

/* ===================== main ===================== */

int main(int argc, char **argv)
{
    if (argc > 1) {
        uci_command_line(argc, argv);
    } else {
        uci_loop();
    }

    return 0;
}
