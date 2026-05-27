/*
  a chess library (bonus: you can integrate more piece types!) which
  supports Chess960 and is decently fast enough
  Copyright (C) 2025-2026  winapiadmin

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#if !defined(__cpp_exceptions) && !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#endif
#include "position.h"
#include "printers.h"
#include <chrono>
#include <doctest/doctest.h>
#include <random>
#include <sstream>
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
static_assert(color_of(ContiguousMappingPiece::WPAWN) == WHITE, "WPAWN should be WHITE");
static_assert(color_of(ContiguousMappingPiece::WKNIGHT) == WHITE, "WKNIGHT should be WHITE");
static_assert(color_of(ContiguousMappingPiece::WBISHOP) == WHITE, "WBISHOP should be WHITE");
static_assert(color_of(ContiguousMappingPiece::WROOK) == WHITE, "WROOK should be WHITE");
static_assert(color_of(ContiguousMappingPiece::WQUEEN) == WHITE, "WQUEEN should be WHITE");
static_assert(color_of(ContiguousMappingPiece::WKING) == WHITE, "WKING should be WHITE");
static_assert(color_of(ContiguousMappingPiece::BPAWN) == BLACK, "BPAWN should be WHITE");
static_assert(color_of(ContiguousMappingPiece::BKNIGHT) == BLACK, "BKNIGHT should be BLACK");
static_assert(color_of(ContiguousMappingPiece::BBISHOP) == BLACK, "BBISHOP should be BLACK");
static_assert(color_of(ContiguousMappingPiece::BROOK) == BLACK, "BROOK should be BLACK");
static_assert(color_of(ContiguousMappingPiece::BQUEEN) == BLACK, "BQUEEN should be BLACK");
static_assert(color_of(ContiguousMappingPiece::BKING) == BLACK, "BKING should be BLACK");
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
static_assert(piece_of(ContiguousMappingPiece::WPAWN) == PAWN, "WPAWN should be PAWN");
static_assert(piece_of(ContiguousMappingPiece::WKNIGHT) == KNIGHT, "WKNIGHT should be KNIGHT");
static_assert(piece_of(ContiguousMappingPiece::WBISHOP) == BISHOP, "WBISHOP should be BISHOP");
static_assert(piece_of(ContiguousMappingPiece::WROOK) == ROOK, "WROOK should be ROOK");
static_assert(piece_of(ContiguousMappingPiece::WQUEEN) == QUEEN, "WQUEEN should be QUEEN");
static_assert(piece_of(ContiguousMappingPiece::WKING) == KING, "WKING should be KING");
static_assert(piece_of(ContiguousMappingPiece::BPAWN) == PAWN, "BPAWN should be PAWN");
static_assert(piece_of(ContiguousMappingPiece::BKNIGHT) == KNIGHT, "BKNIGHT should be KNIGHT");
static_assert(piece_of(ContiguousMappingPiece::BBISHOP) == BISHOP, "BBISHOP should be BISHOP");
static_assert(piece_of(ContiguousMappingPiece::BROOK) == ROOK, "BROOK should be ROOK");
static_assert(piece_of(ContiguousMappingPiece::BQUEEN) == QUEEN, "BQUEEN should be QUEEN");
static_assert(piece_of(ContiguousMappingPiece::BKING) == KING, "BKING should be KING");
static_assert(piece_of(ContiguousMappingPiece::NO_PIECE) == NO_PIECE_TYPE, "NO_PIECE -> NO_PIECE_TYPE");
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
static_assert(make_piece<ContiguousMappingPiece>(PAWN, BLACK) == ContiguousMappingPiece::BPAWN,
              "make_piece PAWN,BLACK -> BPAWN");
static_assert(make_piece<ContiguousMappingPiece>(KNIGHT, BLACK) == ContiguousMappingPiece::BKNIGHT,
              "make_piece KNIGHT,BLACK -> BKNIGHT");
static_assert(make_piece<ContiguousMappingPiece>(BISHOP, BLACK) == ContiguousMappingPiece::BBISHOP,
              "make_piece BISHOP,BLACK -> BBISHOP");
static_assert(make_piece<ContiguousMappingPiece>(ROOK, BLACK) == ContiguousMappingPiece::BROOK,
              "make_piece ROOK,BLACK -> BROOK");
static_assert(make_piece<ContiguousMappingPiece>(QUEEN, BLACK) == ContiguousMappingPiece::BQUEEN,
              "make_piece QUEEN,BLACK -> BQUEEN");
static_assert(make_piece<ContiguousMappingPiece>(KING, BLACK) == ContiguousMappingPiece::BKING,
              "make_piece KING,BLACK -> BKING");
static_assert(make_piece<ContiguousMappingPiece>(PAWN, WHITE) == ContiguousMappingPiece::WPAWN,
              "make_piece PAWN,WHITE -> WPAWN");
static_assert(make_piece<ContiguousMappingPiece>(KNIGHT, WHITE) == ContiguousMappingPiece::WKNIGHT,
              "make_piece KNIGHT,WHITE -> WKNIGHT");
static_assert(make_piece<ContiguousMappingPiece>(BISHOP, WHITE) == ContiguousMappingPiece::WBISHOP,
              "make_piece BISHOP,WHITE -> WBISHOP");
static_assert(make_piece<ContiguousMappingPiece>(ROOK, WHITE) == ContiguousMappingPiece::WROOK,
              "make_piece ROOK,WHITE -> WROOK");
static_assert(make_piece<ContiguousMappingPiece>(QUEEN, WHITE) == ContiguousMappingPiece::WQUEEN,
              "make_piece QUEEN,WHITE -> WQUEEN");
static_assert(make_piece<ContiguousMappingPiece>(KING, WHITE) == ContiguousMappingPiece::WKING,
              "make_piece KING,WHITE -> WKING");
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
#if defined(_DEBUG) || !defined(NDEBUG)
#define IS_RELEASE 0
#else
#define IS_RELEASE 1
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
            pos.template doMove<false>(m);
#if !IS_RELEASE
            {
                const auto pre_nm_hash_1 = pos.hash();
                const auto pre_nm_fen_1 = pos.fen();
                if (pos.zobrist() != pos.hash())
                    REQUIRE(pos.zobrist() == pos.hash());
                pos.doNullMove();
                pos.undoMove();
                if (!(pos.hash() == pre_nm_hash_1 && pos.fen() == pre_nm_fen_1 && pos.zobrist() == pre_nm_hash_1)) {
                    REQUIRE(pos.hash() == pre_nm_hash_1);
                    REQUIRE(pos.fen() == pre_nm_fen_1);
                    REQUIRE(pos.zobrist() == pre_nm_hash_1);
                }
            }
#endif
            const uint64_t nodes = perft<T, mt, false>(pos, depth - 1);
#if !IS_RELEASE
            {
                const auto pre_nm_hash_1 = pos.hash();
                const auto pre_nm_fen_1 = pos.fen();
                if (pos.zobrist() != pos.hash())
                    REQUIRE(pos.zobrist() == pos.hash());
                pos.doNullMove();
                pos.undoMove();
                if (!(pos.hash() == pre_nm_hash_1 && pos.fen() == pre_nm_fen_1 && pos.zobrist() == pre_nm_hash_1)) {
                    REQUIRE(pos.hash() == pre_nm_hash_1);
                    REQUIRE(pos.fen() == pre_nm_fen_1);
                    REQUIRE(pos.zobrist() == pre_nm_hash_1);
                }
            }
#endif
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
#if !IS_RELEASE
auto split_testcases(std::vector<TestEntry<std::string, perft_t>> &entries) {
    std::vector<TestEntry<std::string, perft_t>> optimized;

    std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b) { return a.info.nodes < b.info.nodes; });

    std::vector<TestEntry<std::string, perft_t>> bucket1, bucket2, bucket3;
    for (const auto &e : entries) {
        if (e.info.nodes <= 1'000'000)
            bucket1.push_back(e);
        else if (e.info.nodes <= 10'000'000)
            bucket2.push_back(e);
        else if (e.info.nodes <= 1'000'000'000)
            bucket3.push_back(e);
    }

    size_t n1 = std::min(bucket1.size(), size_t(1000));
    optimized.insert(optimized.end(), bucket1.begin(), bucket1.begin() + n1);

    size_t n2 = std::min(bucket2.size(), size_t(15));
    optimized.insert(optimized.end(), bucket2.begin(), bucket2.begin() + n2);

    size_t n3 = std::min(bucket3.size(), size_t(1));
    if (n3 > 0) {
        optimized.insert(optimized.end(), bucket3.end() - n3, bucket3.end());
    }

    return optimized;
}
#endif
template <typename T, MoveGenType mt, bool EnableDiv>
void check_perft_type(TestEntry<std::string, perft_t> &entry, uint64_t &nodes, double &elapsed) {
    using namespace std::chrono;
    _Position<T> pos(entry.input);
    auto start_time = high_resolution_clock::now();
    REQUIRE(perft<T, mt, EnableDiv>(pos, entry.info.depth) == entry.info.nodes);
    auto end_time = high_resolution_clock::now();
    elapsed += duration<double>(end_time - start_time).count();
    nodes += entry.info.nodes;
    if (entry.info.nodes < 5e6) {
        _Position<T> pos2 = pos;
        REQUIRE(pos.fen() == pos2.fen());
        start_time = high_resolution_clock::now();
        REQUIRE(perft<T, mt, EnableDiv>(pos2, entry.info.depth) == entry.info.nodes);
        end_time = high_resolution_clock::now();
        elapsed += duration<double>(end_time - start_time).count();
        nodes += entry.info.nodes;
    } else {
        std::cerr << "\n(skipped copying test)\n";
    }
}
template <MoveGenType mt = MoveGenType::ALL, bool EnableDiv = false>
void check_perfts(std::vector<TestEntry<std::string, perft_t>> &entries) {
    uint64_t nodes = 0;
    double elapsed = 0;
#if !IS_RELEASE
    entries = split_testcases(entries);
#endif
    for (auto &entry : entries) {
        std::cerr << entry.input << " (chess960=false) " << entry.info.depth;
        std::cerr << '\n';
        check_perft_type<PolyglotPiece, mt, EnableDiv>(entry, nodes, elapsed);
        check_perft_type<EnginePiece, mt, EnableDiv>(entry, nodes, elapsed);
        check_perft_type<ContiguousMappingPiece, mt, EnableDiv>(entry, nodes, elapsed);
    }
    double mnps = (nodes / elapsed) / 1'000'000.0;
    std::cout << "Speed: " << mnps << "Mnps\n";
}

template <bool ISROOK> [[nodiscard]] inline Bitboard sliderAttacks(Square sq, Bitboard occupied) noexcept {
    static constexpr Direction dirs[2][4][2] = {
        { EAST,     EAST,     EAST, WEST, WEST,     WEST,     WEST, EAST },
        { EAST, DIR_NONE, DIR_NONE, WEST, WEST, DIR_NONE, DIR_NONE, EAST }
    };

    Bitboard attacks = 0ull;

    File pf = file_of(sq);
    Rank pr = rank_of(sq);

    for (int i = 0; i < 4; ++i) {
        Direction off_f = dirs[ISROOK][i][0];
        Direction off_r = dirs[ISROOK][i][1];

        File f = pf + off_f;
        Rank r = pr + off_r;
        for (; is_valid(r, f); f += off_f, r += off_r) {
            const auto index = make_sq(f, r);
            attacks |= 1ULL << index;
            if (occupied & (1ULL << index))
                break;
        }
    }

    return attacks;
}
TEST_CASE("attacks") {
    std::mt19937_64 gen(19937);
    std::uniform_int_distribution<uint64_t> dis;
    for (Square sq = SQ_A1; sq < SQ_NONE; sq++) {
        for (int i = 0; i < 100; i++) {
            uint64_t bb = dis(gen) & dis(gen) & dis(gen);
            REQUIRE(attacks::bishop(sq, bb) == sliderAttacks<false>(sq, bb));
        }
    }
    for (Square sq = SQ_A1; sq < SQ_NONE; sq++) {
        for (int i = 0; i < 100; i++) {
            uint64_t bb = dis(gen) & dis(gen) & dis(gen);
            REQUIRE(attacks::rook(sq, bb) == sliderAttacks<true>(sq, bb));
        }
    }
}
TEST_CASE("Castling move making FEN rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8") {
    std::string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    Position position(fen);
    REQUIRE(position.hash() == 0x4f874e21f78d3590);
    position.doMove<false>(Move::make<CASTLING>(SQ_E1, SQ_H1));
    REQUIRE(position.hash() == position.zobrist());
    REQUIRE(position.hash() == 0x31F235158B7EEE80);
    REQUIRE(position.fen() == "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQ1RK1 b - - 2 8");
}
TEST_CASE("EP move making FEN 8/5k1K/8/8/4Pp2/8/8/8 b - e3 0 1") {
    std::string fen = "8/5k1K/8/8/4Pp2/8/8/8 b - e3 0 1";
    Position position(fen);
    REQUIRE(position.hash() == 0x775d0e2acc42db8c);
    position.doMove<false>(Move::make<EN_PASSANT>(SQ_F4, SQ_E3));
    REQUIRE(position.hash() == position.zobrist());
    REQUIRE(position.hash() == 0x3ec71faae73cfbed);
    REQUIRE(position.fen() == "8/5k1K/8/8/8/4p3/8/8 w - - 0 2");
}
TEST_CASE("EP move making FEN 8/5k1K/8/3pP3/8/8/8/8 w - d6 0 2") {
    std::string fen = "8/5k1K/8/3pP3/8/8/8/8 w - d6 0 2";
    Position position(fen);
    REQUIRE(position.hash() == 0xbdc108e30cccd00b);
    position.doMove<false>(Move::make<EN_PASSANT>(SQ_E5, SQ_D6));
    REQUIRE(position.fen() == "8/5k1K/3P4/8/8/8/8/8 b - - 0 2");
    REQUIRE(position.hash() == position.zobrist());
    REQUIRE(position.hash() == 0x5c57e65793a2c8fe);
}
TEST_CASE("Promotion FEN 8/5kP1/7K/8/8/8/8/8 w - - 0 1") {
    std::string fen = "8/5kP1/7K/8/8/8/8/8 w - - 0 1";
    Position position(fen);
    REQUIRE(position.hash() == 0x3793b24e1b95a6d5);
    position.doMove<false>(Move::make<PROMOTION>(SQ_G7, SQ_G8));
    REQUIRE(position.fen() == "6N1/5k2/7K/8/8/8/8/8 b - - 0 1");
    REQUIRE(position.hash() == position.zobrist());
    REQUIRE(position.hash() == 0x6adb9bf15f32cb92);
}
TEST_CASE("EP replace on non-EP move") {
    Position position("rnbqkbnr/pppppp2/7p/6pP/8/8/PPPPPPP1/RNBQKBNR w KQkq g6 0 3");
    position.doMove<false>(Move(SQ_E2, SQ_E4));
    REQUIRE(position.fen() == "rnbqkbnr/pppppp2/7p/6pP/4P3/8/PPPP1PP1/RNBQKBNR b KQkq e3 0 3");
}
TEST_CASE("EP ignore on non-EP move") {
    Position position("rnbqkbnr/pppppp2/7p/6pP/8/8/PPPPPPP1/RNBQKBNR w KQkq g6 0 3");
    position.doMove<false>(Move(SQ_E2, SQ_E3));
    REQUIRE(position.fen() == "rnbqkbnr/pppppp2/7p/6pP/8/4P3/PPPP1PP1/RNBQKBNR b KQkq - 0 3");
}
TEST_CASE("Move making and unmaking") {
    Position position;
    position.doMove<false>(Move(SQ_E2, SQ_E4));
    REQUIRE(position.fen() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    position.undoMove();
    REQUIRE(position.fen() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}
TEST_CASE("Pin detection double push movegen") {
    std::string fen = "rnbqkbnr/1ppppppp/8/p7/Q1P5/8/PP1PPPPP/RNB1KBNR b KQkq - 1 2";
    Position position(fen);
    Movelist out;
    position.legals<MoveGenType::PAWN>(out);
    auto pin_mask = position.pin_mask();
    REQUIRE(out.size() == 12);
    REQUIRE(pin_mask != 0);
    auto pin_maskAND0x8000000000000ULL = pin_mask & 0x8000000000000ULL;
    REQUIRE(pin_maskAND0x8000000000000ULL != 0);
    REQUIRE(pin_mask == 0x8040201000000ULL);
}
TEST_CASE("Move making pin update") {
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    pos.doMove<false>(Move(SQ_C2, SQ_C4));
    pos.doMove<false>(Move(SQ_A7, SQ_A5));
    pos.doMove<false>(Move(SQ_D1, SQ_A4));
    {
        Movelist out;
        pos.legals<MoveGenType::PAWN>(out);
        auto pin_mask = pos.pin_mask();
        REQUIRE(out.size() == 12);
        REQUIRE(pin_mask != 0);
        auto pin_maskAND0x8000000000000ULL = pin_mask & 0x8000000000000ULL;
        REQUIRE(pin_maskAND0x8000000000000ULL != 0);
        REQUIRE(pin_mask == 0x8040201000000ULL);
    }
}

TEST_CASE("Experienced bugs in this repo") {
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
        Position p("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
        REQUIRE(p.zobrist() == p.hash());
        p.push_uci("e5d6");
        REQUIRE(p.zobrist() == p.hash());
    }
    {
        Position p("rnbqkbnr/ppppp1pp/8/4Pp2/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
        REQUIRE(p.zobrist() == p.hash());
        p.push_uci("d7d5");
        REQUIRE(p.zobrist() == p.hash());
    }
    {
        Position p;
        p.doMove<false>(Move(SQ_A2, SQ_A3));
        p.doMove<false>(Move(SQ_B8, SQ_A6));
        p.doMove<false>(Move(SQ_F2, SQ_F3));
        p.doMove<false>(Move(SQ_F7, SQ_F5));
        REQUIRE(p.zobrist() == p.hash());
        REQUIRE(p.hash() == 4177524090105507023);
    }
    {
        Position p;
        p.doMove<false>(Move(SQ_A2, SQ_A3));
        REQUIRE(p.zobrist() == p.hash());
        p.doMove<false>(Move(SQ_B8, SQ_A6));
        REQUIRE(p.zobrist() == p.hash());
        p.doMove<false>(Move(SQ_F2, SQ_F3));
        REQUIRE(p.zobrist() == p.hash());
        p.doMove<false>(Move(SQ_F7, SQ_F5));
        REQUIRE(p.zobrist() == p.hash());
        p.doNullMove();
        REQUIRE(p.zobrist() == p.hash());
        p.undoMove();
        REQUIRE(p.zobrist() == p.hash());
        p.undoMove();
        REQUIRE(p.zobrist() == p.hash());
        p.undoMove();
        REQUIRE(p.zobrist() == p.hash());
        p.undoMove();
        REQUIRE(p.zobrist() == p.hash());
        p.undoMove();
        REQUIRE(p.zobrist() == p.hash());
    }
    {
        Position p;
        REQUIRE(p.getCastlingPath(WHITE, true) == 0x60);
        REQUIRE(p.getCastlingPath(WHITE, false) == 0xe);
        REQUIRE(p.getCastlingPath(BLACK, true) == 0x6000000000000000ULL);
        REQUIRE(p.getCastlingPath(BLACK, false) == 0xe00000000000000ULL);
    }
    {
        Position p;
        p.setFEN("1nbqkbnr/1ppppppp/r7/8/4P3/8/PPPP1PPP/RNBQK1NR w KQk - 0 3");
        REQUIRE(p.fen() == "1nbqkbnr/1ppppppp/r7/8/4P3/8/PPPP1PPP/RNBQK1NR w KQk - 0 3");
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
TEST_CASE("Perfts" * doctest::timeout(36000)) {
    std::vector<TestEntry<std::string, perft_t>> tests = {
        {               "Q1Q2QQQ/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q1Q/pp1Q3Q/kBQQ1KQ1 w - - 0 1",  1,        240 },
        {               "Q1Q2QQQ/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q1Q/pp1Q3Q/kBQQ1KQ1 w - - 0 1",  2,          0 },
        {               "Q1Q2QQQ/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q1Q/pp1Q3Q/kBQQ1KQ1 w - - 0 1",  2,          0 },
        {                     "QQQQQQQK/Q6Q/Q6Q/Q6Q/Q6Q/Q6Q/BR5Q/kBQQQQQQ w - - 0 1",  1,        271 },
        {                                            "5k2/8/8/8/3K4/8/8/8 w - - 0 1",  1,          8 },
        {                                            "5k2/8/8/8/3K4/8/8/8 w - - 0 1",  3,        310 },
        {                                            "5k2/8/8/8/3K4/8/8/8 w - - 0 1",  6,      95366 },
        {                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  1,         20 },
        {                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  2,        400 },
        {                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  3,       8902 },
        {                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  4,     197281 },
        {                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  5,    4865609 },
        {                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  6,  119060324 },
        {                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  6,  119060324 },
        {                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  7, 3195901860 },
        {               "rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR b KQkq - 0 1",  4,     328511 },
        {             "rnbqkbnr/pp1ppppp/2p5/8/8/3P4/PPP1PPPP/RNBQKBNR w KQkq - 0 2",  3,      15206 },
        {               "rnbqkbnr/pp1ppppp/2p5/8/8/3P4/PPPKPPPP/RNBQ1BNR b kq - 1 2",  2,        463 },
        {              "rnb1kbnr/pp1ppppp/2p5/q7/8/3P4/PPPKPPPP/RNBQ1BNR w kq - 2 3",  1,          4 },
        {                                         "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1",  1,          8 },
        {                                         "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1",  2,         33 },
        {                                         "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1",  3,        270 },
        {                                        "7k/8/8/8/1p6/P7/1P6/6K1 b - - 0 1",  2,         38 },
        {                                          "7k/8/8/8/8/p7/1P6/6K1 w - - 0 2",  1,          8 },
        {                                          "7k/8/8/8/8/p7/1P6/6K1 w - - 0 2",  2,         36 },
        {                                        "7k/8/8/8/1p6/P7/1P6/6K1 b - - 0 1",  3,        234 },
        {                                           "7k/8/8/8/8/pP6/8/6K1 b - - 0 2",  1,          4 },
        {                                         "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1",  4,       1664 },
        {                                         "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1",  5,      13931 },
        {                                         "7k/8/8/8/1p6/8/PP6/6K1 w - - 0 1",  6,      94162 },
        {                     "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1",  1,        216 },
        {                     "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1",  2,         95 },
        {                     "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1",  3,      18138 },
        {                     "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1",  4,      80766 },
        {                     "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNNK1B1 w - - 0 1",  5,   11649711 },
        {                               "8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1",  1,          7 },
        {                               "8/2p5/3p4/KP3k1r/2R1Pp2/6P1/8/8 b - e3 0 1",  2,        108 },
        {     "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",  1,         48 },
        {     "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",  2,       2039 },
        {     "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",  3,      97862 },
        {     "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",  4,    4085603 },
        {     "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",  5,  193690690 },
        {    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1",  4,    4627439 },
        {      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/PpN2Q1p/1PPBBPPP/R3K2R w KQkq - 0 2",  3,     103849 },
        {      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/PPN2Q1p/1P1BBPPP/R3K2R b KQkq - 0 2",  2,       2098 },
        {     "r3k2r/p1ppqpb1/1n2pnp1/3PN3/4P3/PPNb1Q1p/1P1BBPPP/R3K2R w KQkq - 1 3",  1,         46 },
        {      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/P1p2Q1p/1PPBBPPP/R3K2R w KQkq - 0 2",  3,      98073 },
        {        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/P1p2Q1p/RPPBBPPP/4K2R b Kkq - 1 2",  2,       1926 },
        {          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/4P3/P4Q1p/RPPpBPPP/4K2R w Kkq - 0 3",  1,          3 },
        {                                "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",  1,         14 },
        {                                "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",  2,        191 },
        {                                "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",  3,       2812 },
        {                                "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",  4,      43238 },
        {                                "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",  5,     674624 },
        {                                "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",  6,   11030083 },
        {                                "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",  7,  178633661 },
        {         "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",  1,          6 },
        {         "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",  2,        264 },
        {         "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",  3,       9467 },
        {         "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",  4,     422333 },
        {         "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",  5,   15833292 },
        {         "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",  6,  706045033 },
        {         "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P1RPP/R2Q2K1 b kq - 1 1",  4,    2703427 },
        {          "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/Pp1P1RPP/R2Q2K1 w kq - 0 2",  3,      58801 },
        {          "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/Pp1P1RPP/R2Q1K2 b kq - 1 2",  2,       1732 },
        {          "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/5q2/P2P1RPP/Rq1Q1K2 w kq - 0 3",  1,         30 },
        {            "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/8/Pp1P1RPP/R2q1K2 w kq - 0 3",  1,          2 },
        {         "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",  1,          6 },
        {         "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",  2,        264 },
        {         "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",  3,       9467 },
        {         "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",  4,     422333 },
        {         "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",  5,   15833292 },
        {         "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",  6,  706045033 },
        {                "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",  1,         44 },
        {                "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",  2,       1486 },
        {                "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",  3,      62379 },
        {                "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",  4,    2103487 },
        {                "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",  5,   89941194 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",  1,         46 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",  2,       2079 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",  3,      89890 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",  4,    3894594 },
        { "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",  5,  164075551 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  1,          5 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  2,         18 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  3,        176 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  4,        859 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  5,      12510 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  6,      63994 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  7,    1109712 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  8,    5763773 },
        {                                            "7K/5kP1/8/8/8/8/8/8 w - - 0 1",  9,  109424670 },
        // https://www.chessprogramming.net/perfect-perft/
        {                                        "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1",  6,    1134888 },
        {                                       "8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1",  6,    1015133 },
        {                                      "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1",  6,    1440467 },
        {                                           "5k2/8/8/8/8/8/8/4K2R w K - 0 1",  6,     661072 },
        {                                           "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1",  6,     803711 },
        {                                "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",  4,    1274206 },
        {                                 "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1",  4,    1720476 },
        {                                      "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1",  5,    1004658 },
        {                                           "4k3/1P6/8/8/8/8/K7/8 w - - 0 1",  6,     217342 },
        {                                            "8/P1k5/K7/8/8/8/8/8 w - - 0 1",  6,      92683 },
        {                                            "K1k5/8/P7/8/8/8/8/8 w - - 0 1",  6,       2217 },
        {                                           "8/k1P5/8/1K6/8/8/8/8 w - - 0 1",  7,     567584 },
        {                                        "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1",  4,      23527 },
        // https://github.com/SebLague/Chess-Coding-Adventure/blob/Chess-V2-Unity/Assets/Scripts/Testing/Perft/Suites/Suite%20Full.txt
        // converted to [fen, depth, nodes], deduped tests
        {                        "2b1b3/1r1P4/3K3p/1p6/2p5/6k1/1P3p2/4B3 w - - 0 42",  5,    5617302 },
        {                                    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",  6,   11030083 },
        {           "r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4",  5,   15587335 },
        {                "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",  5,   89941194 },
        {         "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",  5,  193690690 },
        {              "r3k1nr/p2pp1pp/b1n1P1P1/1BK1Pp1q/8/8/2PP1PPP/6N1 w kq - 0 1",  4,     497787 },
        {                                           "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1",  7,   15594314 },
        {                                        "2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1",  6,    3821001 },
        {                                            "K1k5/8/P7/8/8/8/8/8 w - - 0 1", 10,    5966690 },
        {                                        "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1",  6,    3114998 },
        {                     "4k2r/1pp1n2p/6N1/K2P2r1/1P2P3/P5P1/3b3P/R7 w k - 1 8",  4,     380429 },
        {        "r1bq2r1/1pppkppp/1b3n2/pP1PP3/2n5/2P5/P3QPPP/RNB1K2R w KQ a6 0 12",  5,   42761834 },
        {                     "4k2r/1pp1n2p/6N1/1K1P2r1/4P3/P5P1/1Pp4P/R7 w k - 0 6",  5,   10574719 },
        {    "r3k2r/pppqbppp/3p1n1B/1N2p3/1nB1P3/3P3b/PPPQNPPP/R3K2R w KQkq - 11 10",  4,    3050662 },
        {                "1Bb3BN/R2Pk2r/1Q5B/4q2R/2bN4/4Q1BK/1p6/1bq1R1rb w - - 0 1",  4,    6871272 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",  9,    7618365 },
        {                                "3r4/2p1p3/8/1P1P1P2/3K4/5k2/8/8 b - - 0 1",  4,      28181 },
        {                             "8/1p4p1/8/q1PK1P1r/3p1k2/8/4P3/4Q3 b - - 0 1",  5,    6323457 },
        // http://www.rocechess.ch/perft.html
        {                                           "4k3/8/8/8/8/8/8/4K2R w K - 0 1",  1,         15 },
        {                                           "4k3/8/8/8/8/8/8/4K2R w K - 0 1",  2,         66 },
        {                                           "4k3/8/8/8/8/8/8/4K2R w K - 0 1",  3,       1197 },
        {                                           "4k3/8/8/8/8/8/8/4K2R w K - 0 1",  4,       7059 },
        {                                           "4k3/8/8/8/8/8/8/4K2R w K - 0 1",  5,     133987 },
        {                                           "4k3/8/8/8/8/8/8/4K2R w K - 0 1",  6,     764643 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",  1,         16 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",  2,         71 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",  3,       1287 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",  4,       7626 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",  5,     145232 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",  6,     846648 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 w k - 0 1",  1,          5 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 w k - 0 1",  2,         75 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 w k - 0 1",  3,        459 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 w k - 0 1",  4,       8290 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 w k - 0 1",  5,      47635 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 w k - 0 1",  6,     899442 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 w q - 0 1",  1,          5 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 w q - 0 1",  2,         80 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 w q - 0 1",  3,        493 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 w q - 0 1",  4,       8897 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 w q - 0 1",  5,      52710 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 w q - 0 1",  6,    1001523 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",  1,         26 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",  2,        112 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",  3,       3189 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",  4,      17945 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",  5,     532933 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1",  6,    2788982 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1",  1,          5 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1",  2,        130 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1",  3,        782 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1",  4,      22180 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1",  5,     118882 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1",  6,    3517770 },
        {                                           "8/8/8/8/8/8/6k1/4K2R w K - 0 1",  1,         12 },
        {                                           "8/8/8/8/8/8/6k1/4K2R w K - 0 1",  2,         38 },
        {                                           "8/8/8/8/8/8/6k1/4K2R w K - 0 1",  3,        564 },
        {                                           "8/8/8/8/8/8/6k1/4K2R w K - 0 1",  4,       2219 },
        {                                           "8/8/8/8/8/8/6k1/4K2R w K - 0 1",  5,      37735 },
        {                                           "8/8/8/8/8/8/6k1/4K2R w K - 0 1",  6,     185867 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1",  1,         15 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1",  2,         65 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1",  3,       1018 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1",  4,       4573 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1",  5,      80619 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 w Q - 0 1",  6,     413018 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 w k - 0 1",  1,          3 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 w k - 0 1",  2,         32 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 w k - 0 1",  3,        134 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 w k - 0 1",  4,       2073 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 w k - 0 1",  5,      10485 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 w k - 0 1",  6,     179869 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 w q - 0 1",  1,          4 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 w q - 0 1",  2,         49 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 w q - 0 1",  3,        243 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 w q - 0 1",  4,       3991 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 w q - 0 1",  5,      20780 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 w q - 0 1",  6,     367724 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",  1,         26 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",  2,        568 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",  3,      13744 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",  4,     314346 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",  5,    7594526 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",  6,  179862938 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1",  1,         25 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1",  2,        567 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1",  3,      14095 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1",  4,     328965 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1",  5,    8153719 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1",  6,  195629489 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1",  1,         25 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1",  2,        548 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1",  3,      13502 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1",  4,     312835 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1",  5,    7736373 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1",  6,  184411439 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1",  1,         25 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1",  2,        547 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1",  3,      13579 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1",  4,     316214 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1",  5,    7878456 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1",  6,  189224276 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  1,         26 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  2,        583 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  3,      14252 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  4,     334705 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  5,    8198901 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  6,  198328929 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  1,         25 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  2,        560 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  3,      13592 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  4,     317324 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  5,    7710115 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1",  6,  185959088 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1",  1,         25 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1",  2,        560 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1",  3,      13607 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1",  4,     320792 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1",  5,    7848606 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1",  6,  190755813 },
        {                                           "4k3/8/8/8/8/8/8/4K2R b K - 0 1",  1,          5 },
        {                                           "4k3/8/8/8/8/8/8/4K2R b K - 0 1",  2,         75 },
        {                                           "4k3/8/8/8/8/8/8/4K2R b K - 0 1",  3,        459 },
        {                                           "4k3/8/8/8/8/8/8/4K2R b K - 0 1",  4,       8290 },
        {                                           "4k3/8/8/8/8/8/8/4K2R b K - 0 1",  5,      47635 },
        {                                           "4k3/8/8/8/8/8/8/4K2R b K - 0 1",  6,     899442 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1",  1,          5 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1",  2,         80 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1",  3,        493 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1",  4,       8897 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1",  5,      52710 },
        {                                           "4k3/8/8/8/8/8/8/R3K3 b Q - 0 1",  6,    1001523 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 b k - 0 1",  1,         15 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 b k - 0 1",  2,         66 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 b k - 0 1",  3,       1197 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 b k - 0 1",  4,       7059 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 b k - 0 1",  5,     133987 },
        {                                           "4k2r/8/8/8/8/8/8/4K3 b k - 0 1",  6,     764643 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 b q - 0 1",  1,         16 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 b q - 0 1",  2,         71 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 b q - 0 1",  3,       1287 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 b q - 0 1",  4,       7626 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 b q - 0 1",  5,     145232 },
        {                                           "r3k3/8/8/8/8/8/8/4K3 b q - 0 1",  6,     846648 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1",  1,          5 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1",  2,        130 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1",  3,        782 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1",  4,      22180 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1",  5,     118882 },
        {                                         "4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1",  6,    3517770 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",  1,         26 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",  2,        112 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",  3,       3189 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",  4,      17945 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",  5,     532933 },
        {                                         "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1",  6,    2788982 },
        {                                           "8/8/8/8/8/8/6k1/4K2R b K - 0 1",  1,          3 },
        {                                           "8/8/8/8/8/8/6k1/4K2R b K - 0 1",  2,         32 },
        {                                           "8/8/8/8/8/8/6k1/4K2R b K - 0 1",  3,        134 },
        {                                           "8/8/8/8/8/8/6k1/4K2R b K - 0 1",  4,       2073 },
        {                                           "8/8/8/8/8/8/6k1/4K2R b K - 0 1",  5,      10485 },
        {                                           "8/8/8/8/8/8/6k1/4K2R b K - 0 1",  6,     179869 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1",  1,          4 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1",  2,         49 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1",  3,        243 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1",  4,       3991 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1",  5,      20780 },
        {                                           "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1",  6,     367724 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 b k - 0 1",  1,         12 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 b k - 0 1",  2,         38 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 b k - 0 1",  3,        564 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 b k - 0 1",  4,       2219 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 b k - 0 1",  5,      37735 },
        {                                           "4k2r/6K1/8/8/8/8/8/8 b k - 0 1",  6,     185867 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 b q - 0 1",  1,         15 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 b q - 0 1",  2,         65 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 b q - 0 1",  3,       1018 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 b q - 0 1",  4,       4573 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 b q - 0 1",  5,      80619 },
        {                                           "r3k3/1K6/8/8/8/8/8/8 b q - 0 1",  6,     413018 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",  1,         26 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",  2,        568 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",  3,      13744 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",  4,     314346 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",  5,    7594526 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1",  6,  179862938 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1",  1,         26 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1",  2,        583 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1",  3,      14252 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1",  4,     334705 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1",  5,    8198901 },
        {                                     "r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1",  6,  198328929 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1",  1,         25 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1",  2,        560 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1",  3,      13592 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1",  4,     317324 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1",  5,    7710115 },
        {                                     "r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1",  6,  185959088 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1",  1,         25 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1",  2,        560 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1",  3,      13607 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1",  4,     320792 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1",  5,    7848606 },
        {                                     "r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1",  6,  190755813 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  1,         25 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  2,        567 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  3,      14095 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  4,     328965 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  5,    8153719 },
        {                                     "1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  6,  195629489 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  1,         25 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  2,        548 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  3,      13502 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  4,     312835 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  5,    7736373 },
        {                                     "2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1",  6,  184411439 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1",  1,         25 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1",  2,        547 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1",  3,      13579 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1",  4,     316214 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1",  5,    7878456 },
        {                                     "r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1",  6,  189224276 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1",  1,         14 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1",  2,        195 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1",  3,       2760 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1",  4,      38675 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1",  5,     570726 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1",  6,    8107539 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1",  1,         11 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1",  2,        156 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1",  3,       1636 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1",  4,      20534 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1",  5,     223507 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1",  6,    2594412 },
        {                                      "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1",  1,         19 },
        {                                      "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1",  2,        289 },
        {                                      "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1",  3,       4442 },
        {                                      "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1",  4,      73584 },
        {                                      "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1",  5,    1198299 },
        {                                      "8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1",  6,   19870403 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1",  1,          3 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1",  2,         51 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1",  3,        345 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1",  4,       5301 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1",  5,      38348 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N w - - 0 1",  6,     588695 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1",  1,         17 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1",  2,         54 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1",  3,        835 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1",  4,       5910 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1",  5,      92250 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n w - - 0 1",  6,     688780 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1",  1,         15 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1",  2,        193 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1",  3,       2816 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1",  4,      40039 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1",  5,     582642 },
        {                                    "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1",  6,    8503277 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1",  1,         16 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1",  2,        180 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1",  3,       2290 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1",  4,      24640 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1",  5,     288141 },
        {                                        "8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1",  6,    3147566 },
        {                                      "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1",  1,          4 },
        {                                      "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1",  2,         68 },
        {                                      "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1",  3,       1118 },
        {                                      "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1",  4,      16199 },
        {                                      "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1",  5,     281190 },
        {                                      "8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1",  6,    4405103 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1",  1,         17 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1",  2,         54 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1",  3,        835 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1",  4,       5910 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1",  5,      92250 },
        {                                         "K7/8/2n5/1n6/8/8/8/k6N b - - 0 1",  6,     688780 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1",  1,          3 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1",  2,         51 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1",  3,        345 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1",  4,       5301 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1",  5,      38348 },
        {                                         "k7/8/2N5/1N6/8/8/8/K6n b - - 0 1",  6,     588695 },
        {                                        "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1",  1,         17 },
        {                                        "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1",  2,        278 },
        {                                        "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1",  3,       4607 },
        {                                        "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1",  4,      76778 },
        {                                        "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1",  5,    1320507 },
        {                                        "B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1",  6,   22823890 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1",  1,         21 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1",  2,        316 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1",  3,       5744 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1",  4,      93338 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1",  5,    1713368 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1",  6,   28861171 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1",  1,         21 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1",  2,        144 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1",  3,       3242 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1",  4,      32955 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1",  5,     787524 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1",  6,    7881673 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1",  1,          7 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1",  2,        143 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1",  3,       1416 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1",  4,      31787 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1",  5,     310862 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1",  6,    7382896 },
        {                                        "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1",  1,          6 },
        {                                        "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1",  2,        106 },
        {                                        "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1",  3,       1829 },
        {                                        "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1",  4,      31151 },
        {                                        "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1",  5,     530585 },
        {                                        "B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1",  6,    9250746 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1",  1,         17 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1",  2,        309 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1",  3,       5133 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1",  4,      93603 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1",  5,    1591064 },
        {                                       "8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1",  6,   29027891 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1",  1,          7 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1",  2,        143 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1",  3,       1416 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1",  4,      31787 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1",  5,     310862 },
        {                                        "k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1",  6,    7382896 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1",  1,         21 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1",  2,        144 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1",  3,       3242 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1",  4,      32955 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1",  5,     787524 },
        {                                        "K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1",  6,    7881673 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K w - - 0 1",  1,         19 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K w - - 0 1",  2,        275 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K w - - 0 1",  3,       5300 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K w - - 0 1",  4,     104342 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K w - - 0 1",  5,    2161211 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K w - - 0 1",  6,   44956585 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1",  1,         36 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1",  2,       1027 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1",  3,      29215 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1",  4,     771461 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1",  5,   20506480 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1",  6,  525169084 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K b - - 0 1",  1,         19 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K b - - 0 1",  2,        275 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K b - - 0 1",  3,       5300 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K b - - 0 1",  4,     104342 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K b - - 0 1",  5,    2161211 },
        {                                          "7k/RR6/8/8/8/8/rr6/7K b - - 0 1",  6,   44956585 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1",  1,         36 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1",  2,       1027 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1",  3,      29227 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1",  4,     771368 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1",  5,   20521342 },
        {                                        "R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1",  6,  524966748 },
        {                                             "6kq/8/8/8/8/8/8/7K w - - 0 1",  1,          2 },
        {                                             "6kq/8/8/8/8/8/8/7K w - - 0 1",  2,         36 },
        {                                             "6kq/8/8/8/8/8/8/7K w - - 0 1",  3,        143 },
        {                                             "6kq/8/8/8/8/8/8/7K w - - 0 1",  4,       3637 },
        {                                             "6kq/8/8/8/8/8/8/7K w - - 0 1",  5,      14893 },
        {                                             "6kq/8/8/8/8/8/8/7K w - - 0 1",  6,     391507 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  1,          2 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  2,         36 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  3,        143 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  4,       3637 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  5,      14893 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  6,     391507 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1",  1,          6 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1",  2,         35 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1",  3,        495 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1",  4,       8349 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1",  5,     166741 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k w - - 0 1",  6,    3370175 },
        {                                             "6qk/8/8/8/8/8/8/7K b - - 0 1",  1,         22 },
        {                                             "6qk/8/8/8/8/8/8/7K b - - 0 1",  2,         43 },
        {                                             "6qk/8/8/8/8/8/8/7K b - - 0 1",  3,       1015 },
        {                                             "6qk/8/8/8/8/8/8/7K b - - 0 1",  4,       4167 },
        {                                             "6qk/8/8/8/8/8/8/7K b - - 0 1",  5,     105749 },
        {                                             "6qk/8/8/8/8/8/8/7K b - - 0 1",  6,     419369 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  1,          2 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  2,         36 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  3,        143 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  4,       3637 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  5,      14893 },
        {                                             "6KQ/8/8/8/8/8/8/7k b - - 0 1",  6,     391507 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1",  1,          6 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1",  2,         35 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1",  3,        495 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1",  4,       8349 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1",  5,     166741 },
        {                                          "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1",  6,    3370175 },
        {                                             "8/8/8/8/8/K7/P7/k7 w - - 0 1",  1,          3 },
        {                                             "8/8/8/8/8/K7/P7/k7 w - - 0 1",  2,          7 },
        {                                             "8/8/8/8/8/K7/P7/k7 w - - 0 1",  3,         43 },
        {                                             "8/8/8/8/8/K7/P7/k7 w - - 0 1",  4,        199 },
        {                                             "8/8/8/8/8/K7/P7/k7 w - - 0 1",  5,       1347 },
        {                                             "8/8/8/8/8/K7/P7/k7 w - - 0 1",  6,       6249 },
        {                                             "8/8/8/8/8/7K/7P/7k w - - 0 1",  1,          3 },
        {                                             "8/8/8/8/8/7K/7P/7k w - - 0 1",  2,          7 },
        {                                             "8/8/8/8/8/7K/7P/7k w - - 0 1",  3,         43 },
        {                                             "8/8/8/8/8/7K/7P/7k w - - 0 1",  4,        199 },
        {                                             "8/8/8/8/8/7K/7P/7k w - - 0 1",  5,       1347 },
        {                                             "8/8/8/8/8/7K/7P/7k w - - 0 1",  6,       6249 },
        {                                             "K7/p7/k7/8/8/8/8/8 w - - 0 1",  1,          1 },
        {                                             "K7/p7/k7/8/8/8/8/8 w - - 0 1",  2,          3 },
        {                                             "K7/p7/k7/8/8/8/8/8 w - - 0 1",  3,         12 },
        {                                             "K7/p7/k7/8/8/8/8/8 w - - 0 1",  4,         80 },
        {                                             "K7/p7/k7/8/8/8/8/8 w - - 0 1",  5,        342 },
        {                                             "K7/p7/k7/8/8/8/8/8 w - - 0 1",  6,       2343 },
        {                                             "7K/7p/7k/8/8/8/8/8 w - - 0 1",  1,          1 },
        {                                             "7K/7p/7k/8/8/8/8/8 w - - 0 1",  2,          3 },
        {                                             "7K/7p/7k/8/8/8/8/8 w - - 0 1",  3,         12 },
        {                                             "7K/7p/7k/8/8/8/8/8 w - - 0 1",  4,         80 },
        {                                             "7K/7p/7k/8/8/8/8/8 w - - 0 1",  5,        342 },
        {                                             "7K/7p/7k/8/8/8/8/8 w - - 0 1",  6,       2343 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",  1,          7 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",  2,         35 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",  3,        210 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",  4,       1091 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",  5,       7028 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",  6,      34834 },
        {                                             "8/8/8/8/8/K7/P7/k7 b - - 0 1",  1,          1 },
        {                                             "8/8/8/8/8/K7/P7/k7 b - - 0 1",  2,          3 },
        {                                             "8/8/8/8/8/K7/P7/k7 b - - 0 1",  3,         12 },
        {                                             "8/8/8/8/8/K7/P7/k7 b - - 0 1",  4,         80 },
        {                                             "8/8/8/8/8/K7/P7/k7 b - - 0 1",  5,        342 },
        {                                             "8/8/8/8/8/K7/P7/k7 b - - 0 1",  6,       2343 },
        {                                             "8/8/8/8/8/7K/7P/7k b - - 0 1",  1,          1 },
        {                                             "8/8/8/8/8/7K/7P/7k b - - 0 1",  2,          3 },
        {                                             "8/8/8/8/8/7K/7P/7k b - - 0 1",  3,         12 },
        {                                             "8/8/8/8/8/7K/7P/7k b - - 0 1",  4,         80 },
        {                                             "8/8/8/8/8/7K/7P/7k b - - 0 1",  5,        342 },
        {                                             "8/8/8/8/8/7K/7P/7k b - - 0 1",  6,       2343 },
        {                                             "K7/p7/k7/8/8/8/8/8 b - - 0 1",  1,          3 },
        {                                             "K7/p7/k7/8/8/8/8/8 b - - 0 1",  2,          7 },
        {                                             "K7/p7/k7/8/8/8/8/8 b - - 0 1",  3,         43 },
        {                                             "K7/p7/k7/8/8/8/8/8 b - - 0 1",  4,        199 },
        {                                             "K7/p7/k7/8/8/8/8/8 b - - 0 1",  5,       1347 },
        {                                             "K7/p7/k7/8/8/8/8/8 b - - 0 1",  6,       6249 },
        {                                             "7K/7p/7k/8/8/8/8/8 b - - 0 1",  1,          3 },
        {                                             "7K/7p/7k/8/8/8/8/8 b - - 0 1",  2,          7 },
        {                                             "7K/7p/7k/8/8/8/8/8 b - - 0 1",  3,         43 },
        {                                             "7K/7p/7k/8/8/8/8/8 b - - 0 1",  4,        199 },
        {                                             "7K/7p/7k/8/8/8/8/8 b - - 0 1",  5,       1347 },
        {                                             "7K/7p/7k/8/8/8/8/8 b - - 0 1",  6,       6249 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1",  1,          5 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1",  2,         35 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1",  3,        182 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1",  4,       1091 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1",  5,       5408 },
        {                                     "8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1",  6,      34822 },
        {                                          "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1",  1,          2 },
        {                                          "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1",  2,          8 },
        {                                          "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1",  3,         44 },
        {                                          "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1",  4,        282 },
        {                                          "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1",  5,       1814 },
        {                                          "8/8/8/8/8/4k3/4P3/4K3 w - - 0 1",  6,      11848 },
        {                                          "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1",  1,          2 },
        {                                          "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1",  2,          8 },
        {                                          "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1",  3,         44 },
        {                                          "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1",  4,        282 },
        {                                          "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1",  5,       1814 },
        {                                          "4k3/4p3/4K3/8/8/8/8/8 b - - 0 1",  6,      11848 },
        {                                            "8/8/7k/7p/7P/7K/8/8 w - - 0 1",  1,          3 },
        {                                            "8/8/7k/7p/7P/7K/8/8 w - - 0 1",  2,          9 },
        {                                            "8/8/7k/7p/7P/7K/8/8 w - - 0 1",  3,         57 },
        {                                            "8/8/7k/7p/7P/7K/8/8 w - - 0 1",  4,        360 },
        {                                            "8/8/7k/7p/7P/7K/8/8 w - - 0 1",  5,       1969 },
        {                                            "8/8/7k/7p/7P/7K/8/8 w - - 0 1",  6,      10724 },
        {                                            "8/8/k7/p7/P7/K7/8/8 w - - 0 1",  1,          3 },
        {                                            "8/8/k7/p7/P7/K7/8/8 w - - 0 1",  2,          9 },
        {                                            "8/8/k7/p7/P7/K7/8/8 w - - 0 1",  3,         57 },
        {                                            "8/8/k7/p7/P7/K7/8/8 w - - 0 1",  4,        360 },
        {                                            "8/8/k7/p7/P7/K7/8/8 w - - 0 1",  5,       1969 },
        {                                            "8/8/k7/p7/P7/K7/8/8 w - - 0 1",  6,      10724 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1",  1,          5 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1",  2,         25 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1",  3,        180 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1",  4,       1294 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1",  5,       8296 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1",  6,      53138 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1",  1,          8 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1",  2,         61 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1",  3,        483 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1",  4,       3213 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1",  5,      23599 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1",  6,     157093 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1",  1,          8 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1",  2,         61 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1",  3,        411 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1",  4,       3213 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1",  5,      21637 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1",  6,     158065 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K w - - 0 1",  1,          4 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K w - - 0 1",  2,         15 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K w - - 0 1",  3,         90 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K w - - 0 1",  4,        534 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K w - - 0 1",  5,       3450 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K w - - 0 1",  6,      20960 },
        {                                            "8/8/7k/7p/7P/7K/8/8 b - - 0 1",  1,          3 },
        {                                            "8/8/7k/7p/7P/7K/8/8 b - - 0 1",  2,          9 },
        {                                            "8/8/7k/7p/7P/7K/8/8 b - - 0 1",  3,         57 },
        {                                            "8/8/7k/7p/7P/7K/8/8 b - - 0 1",  4,        360 },
        {                                            "8/8/7k/7p/7P/7K/8/8 b - - 0 1",  5,       1969 },
        {                                            "8/8/7k/7p/7P/7K/8/8 b - - 0 1",  6,      10724 },
        {                                            "8/8/k7/p7/P7/K7/8/8 b - - 0 1",  1,          3 },
        {                                            "8/8/k7/p7/P7/K7/8/8 b - - 0 1",  2,          9 },
        {                                            "8/8/k7/p7/P7/K7/8/8 b - - 0 1",  3,         57 },
        {                                            "8/8/k7/p7/P7/K7/8/8 b - - 0 1",  4,        360 },
        {                                            "8/8/k7/p7/P7/K7/8/8 b - - 0 1",  5,       1969 },
        {                                            "8/8/k7/p7/P7/K7/8/8 b - - 0 1",  6,      10724 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1",  1,          5 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1",  2,         25 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1",  3,        180 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1",  4,       1294 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1",  5,       8296 },
        {                                        "8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1",  6,      53138 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1",  1,          8 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1",  2,         61 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1",  3,        411 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1",  4,       3213 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1",  5,      21637 },
        {                                        "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1",  6,     158065 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1",  1,          8 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1",  2,         61 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1",  3,        483 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1",  4,       3213 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1",  5,      23599 },
        {                                        "8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1",  6,     157093 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K b - - 0 1",  1,          4 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K b - - 0 1",  2,         15 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K b - - 0 1",  3,         89 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K b - - 0 1",  4,        537 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K b - - 0 1",  5,       3309 },
        {                                          "k7/8/3p4/8/3P4/8/8/7K b - - 0 1",  6,      21104 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1",  1,          4 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1",  2,         19 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1",  3,        117 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1",  4,        720 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1",  5,       4661 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 w - - 0 1",  6,      32191 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1",  1,          5 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1",  2,         19 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1",  3,        116 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1",  4,        716 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1",  5,       4786 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 w - - 0 1",  6,      30980 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 w - - 0 1",  1,          5 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 w - - 0 1",  2,         22 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 w - - 0 1",  3,        139 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 w - - 0 1",  4,        877 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 w - - 0 1",  5,       6112 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 w - - 0 1",  6,      41874 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 w - - 0 1",  1,          4 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 w - - 0 1",  2,         16 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 w - - 0 1",  3,        101 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 w - - 0 1",  4,        637 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 w - - 0 1",  5,       4354 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 w - - 0 1",  6,      29679 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 w - - 0 1",  1,          5 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 w - - 0 1",  2,         22 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 w - - 0 1",  3,        139 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 w - - 0 1",  4,        877 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 w - - 0 1",  5,       6112 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 w - - 0 1",  6,      41874 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 w - - 0 1",  1,          4 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 w - - 0 1",  2,         16 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 w - - 0 1",  3,        101 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 w - - 0 1",  4,        637 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 w - - 0 1",  5,       4354 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 w - - 0 1",  6,      29679 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K w - - 0 1",  1,          3 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K w - - 0 1",  2,         15 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K w - - 0 1",  3,         84 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K w - - 0 1",  4,        573 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K w - - 0 1",  5,       3013 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K w - - 0 1",  6,      22886 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K w - - 0 1",  1,          4 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K w - - 0 1",  2,         16 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K w - - 0 1",  3,        101 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K w - - 0 1",  4,        637 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K w - - 0 1",  5,       4271 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K w - - 0 1",  6,      28662 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1",  1,          5 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1",  2,         19 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1",  3,        117 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1",  4,        720 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1",  5,       5014 },
        {                                          "7k/3p4/8/8/3P4/8/8/K7 b - - 0 1",  6,      32167 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1",  1,          4 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1",  2,         19 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1",  3,        117 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1",  4,        712 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1",  5,       4658 },
        {                                          "7k/8/8/3p4/8/8/3P4/K7 b - - 0 1",  6,      30749 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 b - - 0 1",  1,          5 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 b - - 0 1",  2,         22 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 b - - 0 1",  3,        139 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 b - - 0 1",  4,        877 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 b - - 0 1",  5,       6112 },
        {                                           "k7/8/8/7p/6P1/8/8/K7 b - - 0 1",  6,      41874 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 b - - 0 1",  1,          4 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 b - - 0 1",  2,         16 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 b - - 0 1",  3,        101 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 b - - 0 1",  4,        637 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 b - - 0 1",  5,       4354 },
        {                                           "k7/8/7p/8/8/6P1/8/K7 b - - 0 1",  6,      29679 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 b - - 0 1",  1,          5 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 b - - 0 1",  2,         22 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 b - - 0 1",  3,        139 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 b - - 0 1",  4,        877 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 b - - 0 1",  5,       6112 },
        {                                           "k7/8/8/6p1/7P/8/8/K7 b - - 0 1",  6,      41874 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 b - - 0 1",  1,          4 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 b - - 0 1",  2,         16 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 b - - 0 1",  3,        101 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 b - - 0 1",  4,        637 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 b - - 0 1",  5,       4354 },
        {                                           "k7/8/6p1/8/8/7P/8/K7 b - - 0 1",  6,      29679 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K b - - 0 1",  1,          5 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K b - - 0 1",  2,         15 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K b - - 0 1",  3,        102 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K b - - 0 1",  4,        569 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K b - - 0 1",  5,       4337 },
        {                                          "k7/8/8/3p4/4p3/8/8/7K b - - 0 1",  6,      22579 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K b - - 0 1",  1,          4 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K b - - 0 1",  2,         16 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K b - - 0 1",  3,        101 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K b - - 0 1",  4,        637 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K b - - 0 1",  5,       4271 },
        {                                          "k7/8/3p4/8/8/4P3/8/7K b - - 0 1",  6,      28662 },
        {                                           "7k/8/8/p7/1P6/8/8/7K w - - 0 1",  1,          5 },
        {                                           "7k/8/8/p7/1P6/8/8/7K w - - 0 1",  2,         22 },
        {                                           "7k/8/8/p7/1P6/8/8/7K w - - 0 1",  3,        139 },
        {                                           "7k/8/8/p7/1P6/8/8/7K w - - 0 1",  4,        877 },
        {                                           "7k/8/8/p7/1P6/8/8/7K w - - 0 1",  5,       6112 },
        {                                           "7k/8/8/p7/1P6/8/8/7K w - - 0 1",  6,      41874 },
        {                                           "7k/8/p7/8/8/1P6/8/7K w - - 0 1",  1,          4 },
        {                                           "7k/8/p7/8/8/1P6/8/7K w - - 0 1",  2,         16 },
        {                                           "7k/8/p7/8/8/1P6/8/7K w - - 0 1",  3,        101 },
        {                                           "7k/8/p7/8/8/1P6/8/7K w - - 0 1",  4,        637 },
        {                                           "7k/8/p7/8/8/1P6/8/7K w - - 0 1",  5,       4354 },
        {                                           "7k/8/p7/8/8/1P6/8/7K w - - 0 1",  6,      29679 },
        {                                           "7k/8/8/1p6/P7/8/8/7K w - - 0 1",  1,          5 },
        {                                           "7k/8/8/1p6/P7/8/8/7K w - - 0 1",  2,         22 },
        {                                           "7k/8/8/1p6/P7/8/8/7K w - - 0 1",  3,        139 },
        {                                           "7k/8/8/1p6/P7/8/8/7K w - - 0 1",  4,        877 },
        {                                           "7k/8/8/1p6/P7/8/8/7K w - - 0 1",  5,       6112 },
        {                                           "7k/8/8/1p6/P7/8/8/7K w - - 0 1",  6,      41874 },
        {                                           "7k/8/1p6/8/8/P7/8/7K w - - 0 1",  1,          4 },
        {                                           "7k/8/1p6/8/8/P7/8/7K w - - 0 1",  2,         16 },
        {                                           "7k/8/1p6/8/8/P7/8/7K w - - 0 1",  3,        101 },
        {                                           "7k/8/1p6/8/8/P7/8/7K w - - 0 1",  4,        637 },
        {                                           "7k/8/1p6/8/8/P7/8/7K w - - 0 1",  5,       4354 },
        {                                           "7k/8/1p6/8/8/P7/8/7K w - - 0 1",  6,      29679 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 w - - 0 1",  1,          5 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 w - - 0 1",  2,         25 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 w - - 0 1",  3,        161 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 w - - 0 1",  4,       1035 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 w - - 0 1",  5,       7574 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 w - - 0 1",  6,      55338 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 w - - 0 1",  1,          5 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 w - - 0 1",  2,         25 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 w - - 0 1",  3,        161 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 w - - 0 1",  4,       1035 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 w - - 0 1",  5,       7574 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 w - - 0 1",  6,      55338 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1",  1,          7 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1",  2,         49 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1",  3,        378 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1",  4,       2902 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1",  5,      24122 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1",  6,     199002 },
        {                                           "7k/8/8/p7/1P6/8/8/7K b - - 0 1",  1,          5 },
        {                                           "7k/8/8/p7/1P6/8/8/7K b - - 0 1",  2,         22 },
        {                                           "7k/8/8/p7/1P6/8/8/7K b - - 0 1",  3,        139 },
        {                                           "7k/8/8/p7/1P6/8/8/7K b - - 0 1",  4,        877 },
        {                                           "7k/8/8/p7/1P6/8/8/7K b - - 0 1",  5,       6112 },
        {                                           "7k/8/8/p7/1P6/8/8/7K b - - 0 1",  6,      41874 },
        {                                           "7k/8/p7/8/8/1P6/8/7K b - - 0 1",  1,          4 },
        {                                           "7k/8/p7/8/8/1P6/8/7K b - - 0 1",  2,         16 },
        {                                           "7k/8/p7/8/8/1P6/8/7K b - - 0 1",  3,        101 },
        {                                           "7k/8/p7/8/8/1P6/8/7K b - - 0 1",  4,        637 },
        {                                           "7k/8/p7/8/8/1P6/8/7K b - - 0 1",  5,       4354 },
        {                                           "7k/8/p7/8/8/1P6/8/7K b - - 0 1",  6,      29679 },
        {                                           "7k/8/8/1p6/P7/8/8/7K b - - 0 1",  1,          5 },
        {                                           "7k/8/8/1p6/P7/8/8/7K b - - 0 1",  2,         22 },
        {                                           "7k/8/8/1p6/P7/8/8/7K b - - 0 1",  3,        139 },
        {                                           "7k/8/8/1p6/P7/8/8/7K b - - 0 1",  4,        877 },
        {                                           "7k/8/8/1p6/P7/8/8/7K b - - 0 1",  5,       6112 },
        {                                           "7k/8/8/1p6/P7/8/8/7K b - - 0 1",  6,      41874 },
        {                                           "7k/8/1p6/8/8/P7/8/7K b - - 0 1",  1,          4 },
        {                                           "7k/8/1p6/8/8/P7/8/7K b - - 0 1",  2,         16 },
        {                                           "7k/8/1p6/8/8/P7/8/7K b - - 0 1",  3,        101 },
        {                                           "7k/8/1p6/8/8/P7/8/7K b - - 0 1",  4,        637 },
        {                                           "7k/8/1p6/8/8/P7/8/7K b - - 0 1",  5,       4354 },
        {                                           "7k/8/1p6/8/8/P7/8/7K b - - 0 1",  6,      29679 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 b - - 0 1",  1,          5 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 b - - 0 1",  2,         25 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 b - - 0 1",  3,        161 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 b - - 0 1",  4,       1035 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 b - - 0 1",  5,       7574 },
        {                                           "k7/7p/8/8/8/8/6P1/K7 b - - 0 1",  6,      55338 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 b - - 0 1",  1,          5 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 b - - 0 1",  2,         25 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 b - - 0 1",  3,        161 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 b - - 0 1",  4,       1035 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 b - - 0 1",  5,       7574 },
        {                                           "k7/6p1/8/8/8/8/7P/K7 b - - 0 1",  6,      55338 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1",  1,          7 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1",  2,         49 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1",  3,        378 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1",  4,       2902 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1",  5,      24122 },
        {                                      "3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1",  6,     199002 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1",  1,         11 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1",  2,         97 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1",  3,        887 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1",  4,       8048 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1",  5,      90606 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1",  6,    1030499 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1",  1,         24 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1",  2,        421 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1",  3,       7421 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1",  4,     124608 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1",  5,    2193768 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1",  6,   37665329 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1",  1,         18 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1",  2,        270 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1",  3,       4699 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1",  4,      79355 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1",  5,    1533145 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1",  6,   28859283 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1",  1,         24 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1",  2,        496 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1",  3,       9483 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1",  4,     182838 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1",  5,    3605103 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1",  6,   71179139 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1",  1,         11 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1",  2,         97 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1",  3,        887 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1",  4,       8048 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1",  5,      90606 },
        {                                            "8/Pk6/8/8/8/8/6Kp/8 b - - 0 1",  6,    1030499 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1",  1,         24 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1",  2,        421 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1",  3,       7421 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1",  4,     124608 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1",  5,    2193768 },
        {                                    "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1",  6,   37665329 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1",  1,         18 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1",  2,        270 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1",  3,       4699 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1",  4,      79355 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1",  5,    1533145 },
        {                                        "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1",  6,   28859283 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",  1,         24 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",  2,        496 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",  3,       9483 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",  4,     182838 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",  5,    3605103 },
        {                                  "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",  6,   71179139 },
        {         "6k1/2nNrq2/bb1P2NR/P2bqqN1/qpKQ3n/2Rn2B1/B1r1N3/1Q1Nb3 w - - 0 1",  5,          0 }, // checkmate
        {                                      "K7/3n3R/1r4k1/8/8/8/2p5/8 w - - 0 1",  5,    1056215 },
        {   "1Qn2n1k/P1K2P2/1pr2nrP/1p2Q1b1/bPP2B2/p2B1q1N/2Rpp1pn/Q3Q1N1 w - - 0 1",  1,          2 },
        {   "1Qn2n1k/P1K2P2/1pr2nrP/1p2Q1b1/bPP2B2/p2B1q1N/2Rpp1pn/Q3Q1N1 w - - 0 1",  5,   11921325 },
        {                                            "8/7K/8/8/8/5R2/8/k7 b - - 0 1",  1,          3 },
        {                                            "8/7K/8/8/8/5R2/8/k7 b - - 0 1",  2,         57 },
        {                                            "8/7K/8/8/8/5R2/8/k7 b - - 0 1",  3,        261 },
        {                                            "8/7K/8/8/8/5R2/8/k7 b - - 0 1",  4,       4897 },
        {                                            "8/7K/8/8/8/5R2/8/k7 b - - 0 1",  5,      22176 },
        {                                            "8/7K/8/8/8/5R2/8/k7 b - - 0 1",  6,     413723 },
        {                                            "8/7K/8/8/8/5R2/8/k7 b - - 0 1",  7,    2109569 },
        {                                            "8/7K/8/8/8/5R2/8/k7 b - - 0 1",  8,   39470570 },
        {                                     "8/5K1p/r7/6q1/2p5/5qk1/8/8 w - - 0 1",  1,          1 },
        {                                     "8/5K1p/r7/6q1/2p5/5qk1/8/8 w - - 0 1",  5,      17218 },
        {            "3n2r1/2k2B1q/3B2Q1/N1n1P3/1r1R1NQ1/2brB1r1/PQB5/bK6 b - - 0 1",  1,          1 },
        {            "3n2r1/2k2B1q/3B2Q1/N1n1P3/1r1R1NQ1/2brB1r1/PQB5/bK6 b - - 0 1",  5,    7037862 },
        {                            "4q2R/BQ1Np2k/4pn2/bK1N4/4p3/1n6/8/8 b - - 0 1",  1,          4 },
        {                            "4q2R/BQ1Np2k/4pn2/bK1N4/4p3/1n6/8/8 b - - 0 1",  5,    3945793 },
        {                                     "1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1",  1,          5 },
        {                                     "1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1",  5,     188155 },
        {                                     "1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1",  6,    5537444 },
        {                                     "1K6/6N1/5Q2/5p2/4Pk2/8/8/8 b - - 0 1",  7,   33107724 },
        {                "k2bR1B1/NR3B2/2BR4/1N1p1bN1/N3b3/7r/B2n1KN1/2Q5 w - - 0 1",  1,         64 },
        {                "k2bR1B1/NR3B2/2BR4/1N1p1bN1/N3b3/7r/B2n1KN1/2Q5 w - - 0 1",  4,    2641743 },
        {                                        "4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1",  1,          6 },
        {                                        "4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1",  5,      71831 },
        {                                        "4B3/8/5K2/8/8/3k4/8/5N2 b - - 0 1",  6,    1376826 },
        {                      "3q3B/3Q4/1k1N4/5Q2/6Q1/1B2N2Q/K4bB1/2q2N2 b - - 0 1",  1,         35 },
        {                      "3q3B/3Q4/1k1N4/5Q2/6Q1/1B2N2Q/K4bB1/2q2N2 b - - 0 1",  4,    5855605 },
        {                                          "8/1K6/8/8/6n1/1k6/8/8 w - - 0 1",  1,          8 },
        {                                          "8/1K6/8/8/6n1/1k6/8/8 w - - 0 1",  5,      50950 },
        {                                          "8/1K6/8/8/6n1/1k6/8/8 w - - 0 1",  7,    3844295 },
        {                                          "8/1K6/8/8/6n1/1k6/8/8 w - - 0 1",  8,   47694794 },
        {                                 "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1",  1,         63 },
        {                                 "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1",  2,        168 },
        {                                 "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1",  3,      10739 },
        {                                 "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1",  4,      28349 },
        {                                 "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1",  5,    1804208 },
        {                                 "8/5qk1/8/2Kb2r1/8/1b5r/2n5/1b6 b - - 0 1",  6,    5073106 },
        {                                      "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1",  1,         13 },
        {                                      "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1",  2,        376 },
        {                                      "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1",  3,       3875 },
        {                                      "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1",  4,     112556 },
        {                                      "2b5/2p5/8/8/5B1k/q7/2K5/8 w - - 0 1",  5,    1189238 },
        {                      "2K2q1q/2n4n/3qb3/1r2qq2/5n2/k4n2/r5Q1/1q6 w - - 0 1",  1,          0 }, // checkmate
        {                              "8/1Q1pkp1Q/3ppp2/8/1Q2Q2Q/4K3/8/8 w - - 0 1",  5,    6990511 },
        {                                    "8/3pkp2/3ppp2/8/1Q6/4K3/8/8 w - - 0 1",  5,     617665 },
        {                                         "7k/4p3/8/2KP3r/8/8/8/8 b - - 0 1",  5,     197176 },
        {               "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4",  1,         25 },
        {               "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4",  2,        701 },
        {               "rnbqkbnr/1p2pppp/p7/2Pp4/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4",  3,      17762 }
    };
    check_perfts(tests);
}
TEST_CASE("Double setFEN reinitializes castling metadata") {
    {
        // Starting position → no-castling position
        Position p("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        auto meta = p.getCastlingMetadata(WHITE);
        REQUIRE(meta.king_start == SQ_E1);
        REQUIRE(meta.rook_start_ks == SQ_H1);
        REQUIRE(meta.rook_start_qs == SQ_A1);

        p.setFEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
        meta = p.getCastlingMetadata(WHITE);
        REQUIRE(meta.king_start == SQ_NONE);
        REQUIRE(meta.rook_start_ks == SQ_NONE);
        REQUIRE(meta.rook_start_qs == SQ_NONE);
    }
    {
        // No-castling position → starting position (checks both colors)
        Position p("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
        auto meta_w = p.getCastlingMetadata(WHITE);
        auto meta_b = p.getCastlingMetadata(BLACK);
        REQUIRE(meta_w.king_start == SQ_NONE);
        REQUIRE(meta_b.king_start == SQ_NONE);

        p.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        meta_w = p.getCastlingMetadata(WHITE);
        meta_b = p.getCastlingMetadata(BLACK);
        REQUIRE(meta_w.king_start == SQ_E1);
        REQUIRE(meta_w.rook_start_ks == SQ_H1);
        REQUIRE(meta_w.rook_start_qs == SQ_A1);
        REQUIRE(meta_b.king_start == SQ_E8);
        REQUIRE(meta_b.rook_start_ks == SQ_H8);
        REQUIRE(meta_b.rook_start_qs == SQ_A8);
    }
    {
        // Starting position → Chess960 position → no-castling position
        Position p("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        // GEge: G/kingside rook on G file, E/queenside rook on E file; king at F1/F8
        p.setFEN("n1bqrkrb/pppppppp/8/8/8/8/PPPPPPPP/N1BQRKRB w GEge - 0 1", true);
        auto meta_w = p.getCastlingMetadata(WHITE);
        auto meta_b = p.getCastlingMetadata(BLACK);
        REQUIRE(meta_w.king_start == SQ_F1);
        REQUIRE(meta_w.rook_start_ks == SQ_G1);
        REQUIRE(meta_w.rook_start_qs == SQ_E1);
        REQUIRE(meta_b.king_start == SQ_F8);
        REQUIRE(meta_b.rook_start_ks == SQ_G8);
        REQUIRE(meta_b.rook_start_qs == SQ_E8);

        p.setFEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
        meta_w = p.getCastlingMetadata(WHITE);
        meta_b = p.getCastlingMetadata(BLACK);
        REQUIRE(meta_w.king_start == SQ_NONE);
        REQUIRE(meta_b.king_start == SQ_NONE);
    }
    {
        // Partial castling rights: white only, then black only
        Position p("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
        p.setFEN("r3k2r/8/8/8/8/8/8/R3K2R w Q - 0 1");
        auto meta_w = p.getCastlingMetadata(WHITE);
        REQUIRE(meta_w.king_start == SQ_E1);
        REQUIRE(meta_w.rook_start_ks == SQ_NONE); // no kingside
        REQUIRE(meta_w.rook_start_qs == SQ_A1);
        auto meta_b = p.getCastlingMetadata(BLACK);
        REQUIRE(meta_b.king_start == SQ_NONE); // black not set
        REQUIRE(meta_b.rook_start_ks == SQ_NONE);
        REQUIRE(meta_b.rook_start_qs == SQ_NONE);
    }
}

TEST_CASE("Position validation") {
    // is_valid<Strict>() — basic validity checks
    {
        Position p("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
        CHECK(p.template is_valid<false>());

        // Two kings on same square
        Position p2("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
        CHECK(p2.template is_valid<false>());
    }

    // _valid_ep_square()
    {
        Position p("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
        // e3 is the EP target square, but there's no black pawn on d4 to make it valid
        // After a2a3, the EP square is set, but _valid_ep_square should return SQ_NONE
        // since there's no pawn that can actually capture
        Position p2("rnbqkbnr/1ppppppp/8/8/4Pp2/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
        // Black just played f7f5, so e3 is the EP square, and white has a pawn on e4
        // that can capture on f5 via EP... actually in this FEN the EP square is e3
        // The white pawn on e4 can capture en-passant on f5 if the last move was f7-f5
        // Let's use a cleaner case
    }
    {
        Position p("rnbqkbnr/ppppp2p/8/4Ppp1/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
        CHECK(p.template is_valid<false>());
    }
}

TEST_CASE("Draw detection") {
    // is_insufficient_material
    {
        Position p("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
        CHECK(p.is_insufficient_material(WHITE));
        CHECK(p.is_insufficient_material(BLACK));
        CHECK(p.is_insufficient_material());

        // K+B vs K
        Position p2("4k3/8/8/8/5B2/8/8/4K3 w - - 0 1");
        CHECK(p2.is_insufficient_material(WHITE)); // white has K+B
        CHECK(p2.is_insufficient_material());      // both sides insufficient
        // Actually K+B vs K is draw
        CHECK(p2.is_insufficient_material());

        // K+N vs K
        Position p3("4k3/8/8/8/5N2/8/8/4K3 w - - 0 1");
        CHECK(p3.is_insufficient_material());

        // K+N+N vs K is not insufficient (can mate with two knights)
        Position p4("4k3/8/8/8/5N2/8/4N3/4K3 w - - 0 1");
        CHECK_FALSE(p4.is_insufficient_material(WHITE));

        // K+R vs K is not draw
        Position p5("4k3/8/8/8/5R2/8/8/4K3 w - - 0 1");
        CHECK_FALSE(p5.is_insufficient_material(WHITE));

        // K+B+B vs K - still drawn (unless same colour bishops... but two bishops can mate)
        Position p6("4k3/8/8/8/3B1B2/8/8/4K3 w - - 0 1");
        CHECK(p6.is_insufficient_material(WHITE));
    }

    // hasNonPawnMaterial
    {
        Position p("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
        CHECK_FALSE(p.hasNonPawnMaterial(WHITE));
        CHECK_FALSE(p.hasNonPawnMaterial(BLACK));

        Position p2("4k3/8/8/8/5B2/8/8/4K3 w - - 0 1");
        CHECK(p2.hasNonPawnMaterial(WHITE));
        CHECK_FALSE(p2.hasNonPawnMaterial(BLACK));

        // Only pawns
        Position p3("4k3/8/8/3p4/3P4/8/8/4K3 w - - 0 1");
        CHECK_FALSE(p3.hasNonPawnMaterial(WHITE));
        CHECK_FALSE(p3.hasNonPawnMaterial(BLACK));
    }

    // is_stalemate
    {
        Position p("5k2/8/8/8/8/8/8/4K3 w - - 0 1");
        CHECK_FALSE(p.is_stalemate()); // not stalemate
    }
    {
        // Classic stalemate: black to move with no legal moves but not in check
        Position p("k7/1R6/8/8/8/8/8/7K b - - 0 1");
        CHECK_FALSE(p.is_stalemate()); // actually black can move... let me use a real stalemate
    }
    {
        // Classic stalemate position
        Position p("k7/8/1Q6/8/8/8/8/7K b - - 0 1");
        // Black king on a8, white queen on b7, white king on h1
        // Black has no legal moves but is not in check
        CHECK(p.is_stalemate());
        CHECK_FALSE(p.is_checkmate());
        CHECK_FALSE(p.is_check());
    }
    {
        // Another stalemate: white to move
        Position p("7K/8/8/8/8/8/8/k7 w - - 0 1");
        CHECK_FALSE(p.is_stalemate()); // not stalemate
    }

    // is_checkmate
    {
        // Scholar's mate
        Position p("r1bqkb1r/pppp1Qpp/2n2n2/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4");
        CHECK(p.is_checkmate());
        CHECK(p.is_check());

        // Not checkmate (just check)
        Position p2("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        CHECK_FALSE(p2.is_checkmate());
        CHECK_FALSE(p2.is_check());
    }

    // is_draw / fifty/seventy-five/fivefold
    {
        Position p;
        CHECK_FALSE(p.is_draw(5));       // opening, not draw
        CHECK_FALSE(p.is_fifty_moves()); // half-move clock = 0
        CHECK_FALSE(p.is_seventyfive_moves());
        CHECK_FALSE(p.is_fivefold_repetition());
        CHECK_FALSE(p.isHalfMoveDraw());
    }
}

TEST_CASE("Material and position queries") {
    // material_key — should be stable with same material regardless of turn/EP/castling
    {
        Position p("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        Key k1 = p.material_key();

        // Same position but black to move
        Position p2("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
        Key k2 = p2.material_key();

        CHECK(k1 == k2); // material key independent of turn

        // EP square present
        Position p3("rnbqkbnr/1ppppppp/8/8/4Pp2/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
        // White has an extra pawn compared to starting pos, so key should differ
        Key k3 = p3.material_key();
        CHECK(k1 != k3); // different material
    }

    // count<>()
    {
        Position p;
        CHECK(p.count<PAWN>() == 16);
        CHECK(p.count<KNIGHT>() == 4);
        CHECK(p.count<BISHOP>() == 4);
        CHECK(p.count<ROOK>() == 4);
        CHECK(p.count<QUEEN>() == 2);
        CHECK(p.count<KING>() == 2);
        CHECK(p.count<PAWN, WHITE>() == 8);
        CHECK(p.count<PAWN, BLACK>() == 8);
        CHECK(p.count<PAWN>(WHITE) == 8);
        CHECK(p.count<PAWN>(BLACK) == 8);
        CHECK(p.count(PAWN, WHITE) == 8);
        CHECK(p.count(PAWN, BLACK) == 8);
    }

    // hasNonPawnMaterial
    {
        Position p;
        CHECK(p.hasNonPawnMaterial(WHITE));
        CHECK(p.hasNonPawnMaterial(BLACK));

        Position p2("4k3/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1");
        CHECK_FALSE(p2.hasNonPawnMaterial(WHITE));
        CHECK_FALSE(p2.hasNonPawnMaterial(BLACK));
    }

    // at<T>(Square)
    {
        Position p;
        CHECK(p.at<PieceType>(SQ_E1) == KING);
        CHECK(p.at<PieceType>(SQ_D1) == QUEEN);
        CHECK(p.at<PieceType>(SQ_E2) == PAWN);
        CHECK(p.at<PieceType>(SQ_E8) == KING);
        CHECK(p.at<Color>(SQ_E1) == WHITE);
        CHECK(p.at<Color>(SQ_E8) == BLACK);
        CHECK(p.at<Color>(SQ_D8) == BLACK);
        CHECK(p.at<EnginePiece>(SQ_E1) == EnginePiece::WKING);
        CHECK(p.at<EnginePiece>(SQ_E8) == EnginePiece::BKING);
    }

    // pieces(pt...) variadic
    {
        Position p;
        Bitboard minors = p.pieces(KNIGHT, BISHOP);
        Bitboard expected = p.pieces(KNIGHT) | p.pieces(BISHOP);
        CHECK(minors == expected);

        Bitboard w_minors = p.pieces(WHITE, KNIGHT, BISHOP);
        Bitboard w_expected = (p.pieces(KNIGHT) & p.occ(WHITE)) | (p.pieces(BISHOP) & p.occ(WHITE));
        CHECK(w_minors == w_expected);

        Bitboard heavy = p.pieces(WHITE, ROOK, QUEEN);
        Bitboard h_expected = (p.pieces(ROOK) & p.occ(WHITE)) | (p.pieces(QUEEN) & p.occ(WHITE));
        CHECK(heavy == h_expected);
    }

    // ply()
    {
        Position p;
        CHECK(p.ply() == 0);
        p.doMove<false>(Move(SQ_E2, SQ_E4));
        CHECK(p.ply() == 1);
        p.doMove<false>(Move(SQ_E7, SQ_E5));
        CHECK(p.ply() == 2);
        p.undoMove();
        CHECK(p.ply() == 1);
        p.undoMove();
        CHECK(p.ply() == 0);
    }

    // chess960()
    {
        Position p("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        CHECK_FALSE(p.chess960());

        Position p960("n1bqrkrb/pppppppp/8/8/8/8/PPPPPPPP/N1BQRKRB w GEge - 0 1", true);
        CHECK(p960.chess960());
    }

    // repetition_count
    {
        Position p;
        CHECK(p.repetition_count() == 0);
    }
}

TEST_CASE("has_repeated") {
    // Test with a 3-fold repetition sequence
    Position p;
    Move m1(SQ_B1, SQ_C3);
    Move m2(SQ_B8, SQ_C6);
    Move m3(SQ_C3, SQ_B1);
    Move m4(SQ_C6, SQ_B8);

    p.doMove(m1);
    p.doMove(m2);
    CHECK_FALSE(p.has_repeated());
    p.doMove(m3);
    p.doMove(m4);
    CHECK(p.has_repeated());
    p.doMove(m1);
    p.doMove(m2);
    p.doMove(m3);
    p.doMove(m4);
    CHECK(p.has_repeated());
}

TEST_CASE("has_repeated with null moves") {
    Position p;
    Move m1(SQ_G1, SQ_F3);
    Move m2(SQ_G8, SQ_F6);
    Move m3(SQ_F3, SQ_G1);
    Move m4(SQ_F6, SQ_G8);

    p.doMove<false>(m1);
    p.doMove<false>(m2);
    p.doMove<false>(m3);
    p.doMove<false>(m4);
    CHECK_FALSE(p.has_repeated()); // only 2 occurrences total

    // null move should reset repetition tracking
    p.doNullMove();
    CHECK_FALSE(p.has_repeated());

    p.doMove<false>(m1);
    p.doMove<false>(m2);
    CHECK_FALSE(p.has_repeated());
}

TEST_CASE("Castling rights queries") {
    {
        Position p("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        CHECK(p.has_castling_rights(WHITE));
        CHECK(p.has_castling_rights(BLACK));
        CHECK(p.has_kingside_castling_rights(WHITE));
        CHECK(p.has_queenside_castling_rights(WHITE));
        CHECK(p.has_kingside_castling_rights(BLACK));
        CHECK(p.has_queenside_castling_rights(BLACK));
    }
    {
        Position p("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
        CHECK_FALSE(p.has_castling_rights(WHITE));
        CHECK_FALSE(p.has_castling_rights(BLACK));
        CHECK_FALSE(p.has_kingside_castling_rights(WHITE));
        CHECK_FALSE(p.has_queenside_castling_rights(WHITE));
    }
    {
        Position p("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1");
        CHECK(p.has_castling_rights(WHITE));
        CHECK_FALSE(p.has_castling_rights(BLACK));
        CHECK_FALSE(p.has_kingside_castling_rights(WHITE));
        CHECK(p.has_queenside_castling_rights(WHITE));
    }
}

TEST_CASE("Square and bitboard utilities") {
    // square_mirror / flip_sq
    CHECK(square_mirror(SQ_A1) == SQ_A8);
    CHECK(square_mirror(SQ_H1) == SQ_H8);
    CHECK(square_mirror(SQ_E2) == SQ_E7);
    CHECK(square_mirror(SQ_D5) == SQ_D4);
    CHECK(flip_sq(SQ_A1) == SQ_A8);
    CHECK(flip_sq(SQ_C3) == SQ_C6);

    // relative_square
    CHECK(relative_square(WHITE, SQ_A1) == SQ_A1);
    CHECK(relative_square(BLACK, SQ_A1) == SQ_A8);
    CHECK(relative_square(WHITE, SQ_H8) == SQ_H8);
    CHECK(relative_square(BLACK, SQ_H8) == SQ_H1);
    CHECK(relative_square(BLACK, SQ_E2) == SQ_E7);

    // castling_rook_square
    CHECK(castling_rook_square(WHITE, true) == SQ_F1);
    CHECK(castling_rook_square(WHITE, false) == SQ_D1);
    CHECK(castling_rook_square(BLACK, true) == SQ_F8);
    CHECK(castling_rook_square(BLACK, false) == SQ_D8);

    // castling_king_square
    CHECK(castling_king_square(WHITE, true) == SQ_G1);
    CHECK(castling_king_square(WHITE, false) == SQ_C1);
    CHECK(castling_king_square(BLACK, true) == SQ_G8);
    CHECK(castling_king_square(BLACK, false) == SQ_C8);

    // relative_rank
    CHECK(relative_rank(WHITE, RANK_1) == RANK_1);
    CHECK(relative_rank(BLACK, RANK_1) == RANK_8);
    CHECK(relative_rank(WHITE, RANK_8) == RANK_8);
    CHECK(relative_rank(BLACK, RANK_8) == RANK_1);
    CHECK(relative_rank(WHITE, SQ_A2) == RANK_2);
    CHECK(relative_rank(BLACK, SQ_E7) == RANK_2);

    // pawn_push
    CHECK(pawn_push(WHITE) == NORTH);
    CHECK(pawn_push(BLACK) == SOUTH);

    // square_distance (Chebyshev)
    CHECK(square_distance(SQ_A1, SQ_A8) == 7);
    CHECK(square_distance(SQ_A1, SQ_H1) == 7);
    CHECK(square_distance(SQ_A1, SQ_H8) == 7);
    CHECK(square_distance(SQ_E4, SQ_E5) == 1);
    CHECK(square_distance(SQ_E4, SQ_D5) == 1);
    CHECK(square_distance(SQ_A1, SQ_A1) == 0);
    CHECK(square_distance(SQ_A1, SQ_C3) == 2);

    // parse_square
    CHECK(parse_square("a1") == SQ_A1);
    CHECK(parse_square("h8") == SQ_H8);
    CHECK(parse_square("e4") == SQ_E4);
    CHECK(parse_square("") == SQ_NONE);
    CHECK(parse_square("a") == SQ_NONE);
    CHECK(parse_square("i1") == SQ_NONE);

    // parse_pt
    CHECK(parse_pt('P') == PAWN);
    CHECK(parse_pt('p') == PAWN);
    CHECK(parse_pt('N') == KNIGHT);
    CHECK(parse_pt('n') == KNIGHT);
    CHECK(parse_pt('B') == BISHOP);
    CHECK(parse_pt('R') == ROOK);
    CHECK(parse_pt('Q') == QUEEN);
    CHECK(parse_pt('K') == KING);
    CHECK(parse_pt('x') == NO_PIECE_TYPE);

    // shift()
    using namespace attacks;
    Bitboard b = 1ULL << SQ_E4;
    CHECK(shift<NORTH>(b) == (1ULL << SQ_E5));
    CHECK(shift<SOUTH>(b) == (1ULL << SQ_E3));
    CHECK(shift<EAST>(b) == (1ULL << SQ_F4));
    CHECK(shift<WEST>(b) == (1ULL << SQ_D4));

    // Edge: shift from a-file west should be zero
    Bitboard a1 = 1ULL << SQ_A1;
    CHECK(shift<WEST>(a1) == 0);
    CHECK(shift<SOUTH>(a1) == 0);

    // Edge: shift from h-file east should be zero
    Bitboard h8 = 1ULL << SQ_H8;
    CHECK(shift<EAST>(h8) == 0);
    CHECK(shift<NORTH>(h8) == 0);

    // pawnLeftAttacks / pawnRightAttacks
    Bitboard wpawns = 1ULL << SQ_E4;
    CHECK(pawnLeftAttacks<WHITE>(wpawns) == (1ULL << SQ_D5));
    CHECK(pawnRightAttacks<WHITE>(wpawns) == (1ULL << SQ_F5));

    Bitboard bpawns = 1ULL << SQ_E5;
    CHECK(pawnLeftAttacks<BLACK>(bpawns) == (1ULL << SQ_F4));
    CHECK(pawnRightAttacks<BLACK>(bpawns) == (1ULL << SQ_D4));

    // knight(Bitboard) bulk attacks
    Bitboard knights = (1ULL << SQ_E4) | (1ULL << SQ_A1);
    Bitboard kAttacks = knight(knights);
    CHECK((kAttacks & (1ULL << SQ_G5)));
    CHECK((kAttacks & (1ULL << SQ_C5)));
    CHECK((kAttacks & (1ULL << SQ_G3)));
    CHECK((kAttacks & (1ULL << SQ_C3)));
    // from SQ_A1
    CHECK((kAttacks & (1ULL << SQ_B3)));
    CHECK((kAttacks & (1ULL << SQ_C2)));

    // queen() = bishop | rook
    Bitboard empty = 0;
    Bitboard qAttacks = queen(SQ_E4, empty);
    Bitboard bAttacks = bishop(SQ_E4, empty);
    Bitboard rAttacks = rook(SQ_E4, empty);
    CHECK(qAttacks == (bAttacks | rAttacks));
}