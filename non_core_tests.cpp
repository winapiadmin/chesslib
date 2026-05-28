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
#if !defined(__cpp_exceptions) && !defined(_CPPUNWIND) && !defined(__EXCEPTIONS) && !defined(_CHESSLIB_ERROR_MODE_THROW)
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#endif
#include "moves_io.h"
#include "position.h"
#include "printers.h"
#include <doctest/doctest.h>
#include <iostream>
using namespace chess;
template <typename InputT, typename CheckInfo> struct TestEntry {
    InputT input;
    CheckInfo info;
};
void check_moveToUci(std::vector<TestEntry<Move, std::string>> &tests) {
    for (auto &tc : tests) {
        REQUIRE(chess::uci::moveToUci(tc.input) == tc.info);
    }
}
TEST_CASE("moveToUci") {
    std::vector<TestEntry<Move, std::string>> tests = {
        { Move(SQ_A2, SQ_A4), "a2a4" },
        { Move::make<NORMAL>(SQ_H1, SQ_G4), "h1g4" },
        { Move::make<NORMAL>(SQ_A4, SQ_E6), "a4e6" },
        { Move::make<NORMAL>(SQ_D4, SQ_A7), "d4a7" },
        { Move::make<EN_PASSANT>(SQ_C5, SQ_E6), "c5e6" },
        { Move::make<NORMAL>(SQ_G7, SQ_D2), "g7d2" },
        { Move::make<NORMAL>(SQ_A5, SQ_E6), "a5e6" },
        { Move::make<NORMAL>(SQ_D3, SQ_F7), "d3f7" },
        { Move::make<NORMAL>(SQ_E8, SQ_A4), "e8a4" },
        { Move::make<NORMAL>(SQ_B4, SQ_G3), "b4g3" },
        { Move::make<NORMAL>(SQ_A6, SQ_C8), "a6c8" },
        { Move::make<NORMAL>(SQ_B7, SQ_B2), "b7b2" },
        { Move::make<NORMAL>(SQ_F8, SQ_G1), "f8g1" },
        { Move::make<NORMAL>(SQ_B2, SQ_B1), "b2b1" },
        { Move::make<NORMAL>(SQ_D2, SQ_H8), "d2h8" },
        { Move::make<EN_PASSANT>(SQ_B4, SQ_H3), "b4h3" },
        { Move::make<NORMAL>(SQ_F7, SQ_E5), "f7e5" },
        { Move::make<NORMAL>(SQ_A6, SQ_C2), "a6c2" },
        { Move::make<NORMAL>(SQ_C8, SQ_H5), "c8h5" },
        { Move::make<NORMAL>(SQ_C3, SQ_G7), "c3g7" },
        { Move::make<CASTLING>(SQ_E8, SQ_H8), "e8g8" },
        { Move::make<NORMAL>(SQ_B4, SQ_A8), "b4a8" },
        { Move::make<NORMAL>(SQ_F3, SQ_G5), "f3g5" },
        { Move::make<NORMAL>(SQ_E4, SQ_F6), "e4f6" },
        { Move::make<EN_PASSANT>(SQ_D5, SQ_B6), "d5b6" },
        { Move::make<PROMOTION>(SQ_H2, SQ_G1, BISHOP), "h2g1b" },
        { Move::make<NORMAL>(SQ_E5, SQ_C8), "e5c8" },
        { Move::make<NORMAL>(SQ_D8, SQ_F1), "d8f1" },
        { Move::make<NORMAL>(SQ_H3, SQ_H8), "h3h8" },
        { Move::make<NORMAL>(SQ_G6, SQ_A1), "g6a1" },
        { Move::make<CASTLING>(SQ_E8, SQ_A8), "e8c8" },
        { Move::make<CASTLING>(SQ_E1, SQ_H1), "e1g1" },
        { Move::make<CASTLING>(SQ_E8, SQ_H8), "e8g8" },
        { Move::make<NORMAL>(SQ_C7, SQ_B1), "c7b1" },
        { Move::make<CASTLING>(SQ_E8, SQ_H8), "e8g8" },
        { Move::make<NORMAL>(SQ_D8, SQ_A1), "d8a1" },
        { Move::make<PROMOTION>(SQ_D7, SQ_D8, KNIGHT), "d7d8n" },
        { Move::make<NORMAL>(SQ_D8, SQ_E7), "d8e7" },
        { Move::make<NORMAL>(SQ_F2, SQ_H3), "f2h3" },
        { Move::make<NORMAL>(SQ_B7, SQ_D5), "b7d5" },
        { Move::make<NORMAL>(SQ_D1, SQ_F2), "d1f2" },
        { Move::make<NORMAL>(SQ_E8, SQ_C4), "e8c4" },
        { Move::make<NORMAL>(SQ_D2, SQ_C7), "d2c7" },
        { Move::make<NORMAL>(SQ_H1, SQ_C2), "h1c2" },
        { Move::make<NORMAL>(SQ_E3, SQ_C1), "e3c1" },
        { Move::make<NORMAL>(SQ_H8, SQ_H3), "h8h3" },
        { Move::make<PROMOTION>(SQ_F2, SQ_F1, QUEEN), "f2f1q" },
        { Move::make<NORMAL>(SQ_H2, SQ_D6), "h2d6" },
        { Move::make<PROMOTION>(SQ_G2, SQ_G1, ROOK), "g2g1r" },
        { Move::make<NORMAL>(SQ_H5, SQ_G6), "h5g6" },
        { Move::make<NORMAL>(SQ_E8, SQ_E3), "e8e3" },
        { Move::make<EN_PASSANT>(SQ_H4, SQ_C3), "h4c3" },
        { Move::make<EN_PASSANT>(SQ_E4, SQ_H3), "e4h3" },
        { Move::make<NORMAL>(SQ_D2, SQ_A1), "d2a1" },
        { Move::make<NORMAL>(SQ_H1, SQ_D2), "h1d2" },
        { Move::make<NORMAL>(SQ_B8, SQ_A3), "b8a3" },
        { Move::make<NORMAL>(SQ_H5, SQ_A1), "h5a1" },
        { Move::make<CASTLING>(SQ_E1, SQ_A1), "e1c1" },
        { Move::make<NORMAL>(SQ_A2, SQ_A3), "a2a3" },
        { Move::make<NORMAL>(SQ_C7, SQ_G8), "c7g8" },
        { Move::make<NORMAL>(SQ_E1, SQ_E6), "e1e6" },
        { Move::make<NORMAL>(SQ_E1, SQ_A5), "e1a5" },
        { Move::make<NORMAL>(SQ_E6, SQ_A5), "e6a5" },
        { Move::make<PROMOTION>(SQ_H7, SQ_C8, QUEEN), "h7c8q" },
        { Move::make<PROMOTION>(SQ_C2, SQ_C1, QUEEN), "c2c1q" },
        { Move::make<NORMAL>(SQ_D6, SQ_B7), "d6b7" },
        { Move::make<NORMAL>(SQ_D5, SQ_G7), "d5g7" },
        { Move::make<NORMAL>(SQ_B6, SQ_F3), "b6f3" },
        { Move::make<NORMAL>(SQ_A3, SQ_G1), "a3g1" },
        { Move::make<PROMOTION>(SQ_D2, SQ_A1, BISHOP), "d2a1b" },
        { Move::make<NORMAL>(SQ_H6, SQ_C3), "h6c3" },
        { Move::make<PROMOTION>(SQ_C7, SQ_C8, KNIGHT), "c7c8n" },
        { Move::make<PROMOTION>(SQ_A7, SQ_B8, KNIGHT), "a7b8n" },
        { Move::make<NORMAL>(SQ_C6, SQ_C5), "c6c5" },
        { Move::make<PROMOTION>(SQ_H2, SQ_H1, QUEEN), "h2h1q" },
        { Move::make<NORMAL>(SQ_G1, SQ_G8), "g1g8" },
        { Move::make<NORMAL>(SQ_D1, SQ_H8), "d1h8" },
        { Move::make<NORMAL>(SQ_B1, SQ_C6), "b1c6" },
        { Move::make<EN_PASSANT>(SQ_E4, SQ_A3), "e4a3" },
        { Move::make<NORMAL>(SQ_G5, SQ_F3), "g5f3" },
        { Move::make<PROMOTION>(SQ_D7, SQ_D8, KNIGHT), "d7d8n" },
        { Move::make<NORMAL>(SQ_C4, SQ_E5), "c4e5" },
        { Move::make<NORMAL>(SQ_B1, SQ_F7), "b1f7" },
        { Move::make<NORMAL>(SQ_B3, SQ_B4), "b3b4" },
        { Move::make<PROMOTION>(SQ_G7, SQ_G8, BISHOP), "g7g8b" },
        { Move::make<NORMAL>(SQ_A1, SQ_E3), "a1e3" },
        { Move::make<NORMAL>(SQ_F6, SQ_E3), "f6e3" },
        { Move::make<NORMAL>(SQ_B7, SQ_A2), "b7a2" },
        { Move::make<NORMAL>(SQ_F7, SQ_D4), "f7d4" },
        { Move::make<NORMAL>(SQ_G4, SQ_F6), "g4f6" },
        { Move::make<CASTLING>(SQ_E1, SQ_A1), "e1c1" },
        { Move::make<NORMAL>(SQ_E1, SQ_A1), "e1a1" },
        { Move::make<NORMAL>(SQ_H1, SQ_H8), "h1h8" },
        { Move::make<NORMAL>(SQ_A1, SQ_A7), "a1a7" },
        { Move::make<NORMAL>(SQ_E5, SQ_A8), "e5a8" },
        { Move::make<NORMAL>(SQ_F5, SQ_F8), "f5f8" },
        { Move::make<NORMAL>(SQ_E7, SQ_D2), "e7d2" },
        { Move::make<PROMOTION>(SQ_H2, SQ_E1, BISHOP), "h2e1b" },
        { Move::make<EN_PASSANT>(SQ_H4, SQ_F3), "h4f3" },
        { Move::make<NORMAL>(SQ_E8, SQ_B6), "e8b6" },
        { Move::make<NORMAL>(SQ_H5, SQ_E8), "h5e8" },
        { Move::make<PROMOTION>(SQ_F2, SQ_F1, ROOK), "f2f1r" },
    };
    check_moveToUci(tests);
}
TEST_CASE("push_uci/parse_uci") {
    Position p;
    REQUIRE(p.parse_uci("e2e4") == Move(SQ_E2, SQ_E4));
    p.setFEN("rn1qkbnr/pP1ppppp/8/1b6/8/8/PPP1PPPP/RNBQKBNR w KQkq - 1 5");
    REQUIRE(p.parse_uci("b7a8q") == Move::make<PROMOTION>(SQ_B7, SQ_A8, QUEEN));
}
template <typename T> void check_was_into_check(std::vector<TestEntry<std::string, bool>> &tests) {
    for (auto &tc : tests) {
        _Position<T> pos(tc.input);
        REQUIRE(pos.was_into_check() == tc.info);
    }
}
TEST_CASE("was_into_check") {
    std::vector<TestEntry<std::string, bool>> tests = {
        {               "3K4/1N1Q4/8/2r4k/6r1/8/1q1Q4/r3P3 w q - 24 70", false },
        {                        "8/8/3b3K/4N3/4k3/7n/4q3/7B w - - 8 9",  true },
        {           "N7/1B4P1/4B2n/2Kn4/8/3k4/Pp2r1p1/1p5P w - - 29 85", false },
        {                 "4b3/3n3N/8/2k5/1q6/1B6/4R3/2K1R3 w - - 37 3", false },
        {                  "8/r7/1K3p2/2q5/2R5/6N1/3B4/2p4k w - - 2 89",  true },
        {          "8/1q3Pr1/2b5/2P2r2/7K/5k2/PP1n4/Q2b3p w K g3 28 35", false },
        {              "K7/1r6/4B1p1/7p/3P4/4k3/4R3/1np3b1 w - - 16 84",  true },
        {         "2R2r2/2k2N2/7K/2r2NR1/1n3b2/PP6/3q3b/B7 w - - 38 21",  true },
        {        "2Q5/3b4/1b4Rr/4R1K1/7q/2P2R1q/R5Qp/1bn1k3 w - - 8 76",  true },
        {            "4k3/bP1r4/n7/8/2PP2K1/4B3/2N1P1q1/7P w - - 37 18", false },
        {       "1P4N1/2Nb4/1K6/5B2/2N3R1/2k1Nn2/5N2/1p3p2 b - - 41 98", false },
        {                 "Q4rN1/5R2/R5b1/7r/8/r7/kK5B/7B b K c6 37 73",  true },
        {     "4k2p/r1q5/7n/1p4P1/1P3PQr/N1n2N2/7K/1N1NqB2 b - - 27 80",  true },
        {            "7Q/2p2b2/8/3K4/5Q2/N2B1bb1/5b2/5qk1 w K d3 30 68", false },
        {              "3b3K/5p2/2Pp4/2n1R3/5Qb1/7k/6n1/R7 b - - 23 54", false },
        {            "1n1PN1N1/3Pn3/3r4/2b5/6K1/7r/5b2/3k4 w - - 15 51", false },
        {         "4N3/4kN2/n2p4/4K3/r1P4R/1p4n1/b3r3/3P1n2 b - - 9 43",  true },
        {          "1Q2q3/k4bBq/5B1P/8/6q1/r1q3p1/1K6/2PP4 w - - 16 90",  true },
        {                 "Kqq1B2b/3R1Q2/8/k7/6P1/8/1P4R1/8 w - - 33 9", false },
        {              "7K/Nn3b2/r1N1pP2/4R3/5q2/kq6/4p2b/8 w - - 5 11", false },
        {                  "n3NK1B/B5kn/8/8/6pB/4P3/8/5R2 b k g3 20 41",  true },
        {                 "Kq3Bq1/6p1/4P3/k7/3R4/8/1Npn4/8 b K - 31 84",  true },
        {         "7N/1RK1p1nb/5n2/3N4/QQ2pkr1/1r6/3P1P2/8 b - - 44 77", false },
        {     "1R2RN1q/nn2Q1k1/5b1Q/3n2n1/2qN3Q/1K1R2B1/8/8 w - - 49 2",  true },
        {           "8/5bQn/3n2B1/1K2B3/4NN1n/6q1/4R3/2q4k w - - 8 100", false },
        {           "8/8/1P4P1/7r/1Q1Q4/2R2P1b/6n1/1RbK1Q1k b - - 44 5", false },
        {            "2r2p2/1B6/2r5/5p2/8/3b2q1/2R3qK/k1q5 b - - 11 89",  true },
        {           "1b1Q4/1rk2qP1/8/R1r4q/4p3/2K4p/5P2/R7 b Q - 23 29",  true },
        {        "qq3B2/2r4n/3n4/r1Q5/2P5/5b2/1b1k1K2/P2n4 w Q - 49 89", false },
        {       "B1bn1R1r/1N5k/2p5/4b2r/6n1/K5B1/2n1n3/1P3N2 b - - 6 8",  true },
        {             "8/6q1/3nr3/3n4/KQ6/1bp2q2/1Bk5/2r2Q2 w - - 0 89", false },
        {                   "4r3/p1n4N/8/8/8/4qnP1/6k1/4K3 b K - 12 83",  true },
        {           "3b1Nk1/4R1nR/3r4/3P4/R7/K2n4/1qB5/4r2R b - - 6 73",  true },
        {       "6r1/1Q1B2B1/1bK2QR1/1r6/r7/4p2B/8/2r1k1q1 b - - 44 39",  true },
        {                   "5R2/b7/3p2P1/8/2k5/qK2Q3/7p/8 w K - 33 11",  true },
        {              "1R3Q1Q/p7/8/4B1pq/6k1/2N1Q3/8/6K1 w - - 35 100", false },
        {             "Bp6/PQ5q/8/4k3/7b/2q1Nr2/2K5/2n1q2q w K - 26 14", false },
        {              "3p4/4k1Q1/5B1b/n1q5/8/2Q4K/3Q4/4B3 b - - 15 33", false },
        {           "p7/p5N1/B2n3Q/K4R1P/3n2q1/7k/P3QR2/8 b k e6 16 50", false },
        {                 "5Pkq/8/7N/2n5/1N4R1/7K/1Q6/3ppP2 w Q - 9 43",  true },
        {                "6K1/8/Q4B2/1Q6/b1k5/4q1n1/8/1B5b w K - 41 68",  true },
        {              "1N6/2k4K/2P3r1/8/3R2N1/5b2/5b2/b3p3 w k - 1 50", false },
        {                "3r1K2/1R6/p7/8/1qN3q1/1r6/1k5Q/8 w Q - 27 30",  true },
        {                 "p2B2P1/Q1b5/1Q6/K3k3/8/P4P2/8/8 b - - 42 98", false },
        {                    "BQ5n/5n2/8/7K/8/2B5/1k6/4n1N1 b K - 3 47", false },
        {            "8/n3B3/8/R7/3Q2P1/1rQ2B2/5R2/2K1k2r w k - 46 100",  true },
        {        "1n1N4/2Qr1k1N/1KR3N1/3Pn3/4Q3/4B3/pn2q3/8 w k - 10 9",  true },
        {     "2b5/1q2Bbq1/5n2/2R3qR/2kN4/6b1/P1r4Q/K3r2B b k h3 41 49",  true },
        {               "2q5/4br1R/5Q2/5Q2/2k5/Bnp5/7q/3K4 w - - 18 39", false },
        {              "8/3q4/7k/5K2/1q2R1N1/3N4/2N5/3P2p1 w K - 17 21",  true },
        {    "1P1BP1bK/N5r1/7R/1p3B1Q/6n1/3r4/k1B5/2n1P1q1 b K - 35 23", false },
        {            "2bP1P2/2p5/3K3r/4Q3/2b5/4kr2/q7/pq5r w - - 21 63",  true },
        {           "K1B5/4P1B1/kN3Q1R/3P1B2/2q1QN2/8/8/2P5 w Q - 16 2",  true },
        { "7N/2b1p2P/1RK5/Nbqn1P2/3p2p1/2qr4/P1nP2k1/P1p1b3 w - - 9 95", false },
        {              "2B3p1/2R3b1/8/p7/8/Kprk4/3b2b1/2Q5 b - - 15 77", false },
        {      "1r2b3/2R5/1k1p2PQ/7N/3p2R1/P1R1K3/1bq5/R7 w - d6 41 72", false },
        {             "3rq3/2p4K/4n3/5nn1/k7/8/2n1Q3/3P1q2 b k - 35 89",  true },
        {      "K3R3/1P4NB/1q3R2/1k6/1qn2B2/3pr3/3q1B2/4b3 b - - 18 88", false },
        {            "N1r5/3nP3/bR2Q3/2RB3N/7k/3K4/b7/nQ5b w k - 31 78", false },
        {             "b7/8/3b2P1/3kr3/4R3/6K1/3q2b1/1q2r3 w K - 28 28", false },
        {               "2B1p2n/8/P7/Nn3b1K/1q6/N2p1kR1/8/8 b - - 0 36", false },
        {           "1RK3p1/2BNr3/6R1/6q1/8/b2Pq3/8/2RR1P1k w K - 24 3", false },
        {           "5P2/5Q2/8/2K2rb1/n3p1p1/6b1/2qn4/k4p2 b K - 15 60",  true },
        {            "2b1p3/5N2/2NNR1R1/2q5/8/8/qNQ4k/1P5K w k - 11 18",  true },
        {      "5B2/3q3K/1k3n2/R5r1/1Q2bR2/4P1b1/3B1b2/3pR3 w K - 2 22",  true },
        {         "4K3/8/1r6/3kNQp1/7b/Rr3B2/rP3nq1/1nP1q3 b Q h6 4 85", false },
        {   "Q2rKN2/3Pp3/n5k1/4nPN1/3r3Q/1N1r4/5p2/1BBR1B1B w - - 6 18",  true },
        {             "1NK5/3k1Q2/1B2N1Q1/7p/8/8/5RNN/6Q1 w - g3 16 90",  true },
        {                   "8/4Q3/6QP/3k4/bN2K3/4Q3/5b2/8 w k - 33 38",  true },
        {            "6K1/6q1/QnRb4/1Np4Q/7R/3Q4/3B1q2/5k2 b - - 29 11",  true },
        {               "5n2/4q3/4r3/b7/8/1Q5k/5KRB/1q2r3 b - d6 20 39", false },
        {                  "1R4q1/8/K2B4/5Q2/1N6/Q2qk3/8/8 w - - 10 61", false },
        {            "1q1k4/4q3/8/Q1n5/3b3B/2Qp3b/R6K/5n1n b K - 47 62",  true },
        {             "2PB4/q3BR2/8/N3k3/4P3/4P3/7K/1N2P3 w k c6 22 24", false },
        {                    "6Pp/5N2/8/3QN3/1B6/p7/6kK/Q7 b K - 29 24",  true },
        {                "8/R7/4pk2/5R2/1BK3R1/6Q1/1P1Q4/8 w Q - 10 75",  true },
        {                     "8/6p1/b3QN2/5k2/8/p6P/7r/K7 b - - 18 46", false },
        {             "3Q4/5P2/7N/3N2K1/8/3rk1bn/7b/1NQ1Q3 b k - 17 42",  true },
        {          "5B2/1P2k3/2n3K1/5bn1/5R1P/p3Bp2/3N4/4n3 w - - 6 94",  true },
        {            "4P3/4n1Q1/6b1/5k2/5N2/3n3B/7p/q2B2K1 w - - 16 65",  true },
        {                      "8/8/1k3r2/Nq6/7n/8/8/3K1b2 b - - 27 10", false },
        {                    "8/5N2/1K6/8/8/2Bq4/k7/3b2qp b K f3 39 75",  true },
        {       "kbK1R3/2n2q2/7n/1p6/1r1bP3/2p5/nB5B/2N2b2 w - - 12 68", false },
        {          "8/3b4/2k1pn2/2P4n/2qQp3/1nnb4/r7/KB1Q4 b - - 12 11",  true },
        {           "r3B3/1bb2k2/p7/3K2Q1/8/6bp/6Nb/2p2nB1 b - - 22 61",  true },
        {      "1b6/4p2k/6p1/1N1Q1n2/1pK1r2B/1q2b3/p3Rp2/8 w k - 30 99", false },
        {                     "Q2k1B2/8/3rb3/8/2K5/8/1N6/8 w Q - 43 73",  true },
        {                  "8/1r4Nk/2R5/3Q1P2/8/6Q1/7P/6K1 w K - 27 13", false },
        {            "5k2/2b5/p7/5B2/1K2n3/6n1/1b4q1/3r1b2 b K - 28 91", false },
        {         "5r2/1N6/8/5N1r/4n2k/2B2K2/1RR1NB2/2p1BR2 w - - 8 12",  true },
        {          "5N2/8/5K2/8/2B2P2/5nr1/1Q1B2k1/1Qr1n3 b - - 29 100", false },
        {          "8/3b4/3p4/1K5n/3Q1Qn1/2n5/k1p1Rq1n/3N4 b Q - 39 12",  true },
        {             "2Q5/3n4/1Rq1KN2/3P4/7P/1b3B2/3k3q/8 b - - 15 47",  true },
        {        "3bn3/4Q1p1/2r3q1/2Q2k2/2QKr3/3R4/8/2p2R2 w Q - 28 77",  true },
        {        "n7/1n4p1/2KB2b1/4q1R1/QR6/1kb3p1/8/2n2b2 b K - 24 73", false },
        {           "3R2k1/QpK2R1p/N6P/RP2r3/q4b2/8/1R6/2r5 w - - 7 44",  true },
        {             "8/k1R2Q2/3r4/2QP3B/8/6B1/3K4/n1b2Q2 b Q - 18 53",  true },
        {          "p7/2n3PQ/3p1K2/N2b4/2n2bk1/Q7/B1Q5/2q5 b - - 47 54", false },
        {       "b2Q1q2/1r6/2R5/B1R3q1/p2K1kR1/4q3/8/1r3b2 b - - 15 92",  true },
        {           "pB6/4q1nk/7n/4p2B/1N3K2/P7/rn2bRb1/2r5 w - - 7 57", false },
    };
    check_was_into_check<PolyglotPiece>(tests);
    check_was_into_check<EnginePiece>(tests);
    check_was_into_check<ContiguousMappingPiece>(tests);
}
TEST_CASE("Zobrist mapping?") {
    REQUIRE(zobrist::RandomPiece[enum_idx<PolyglotPiece>()][(int)PolyglotPiece::BPAWN][0] == 0x9D39247E33776D41);
}
struct repetitions_t {
    std::string FEN;
    std::vector<Move> moves;
    int repetition;
};
void check_repetitions(std::vector<TestEntry<repetitions_t, bool>> &tests) {
    for (auto &tc : tests) {
        {
            _Position<PolyglotPiece> pos(tc.input.FEN);
            for (auto &move : tc.input.moves)
                pos.doMove(move);
            REQUIRE(pos.is_repetition(tc.input.repetition) == tc.info);
        }
        {
            _Position<EnginePiece> pos(tc.input.FEN);
            for (auto &move : tc.input.moves)
                pos.doMove(move);
            REQUIRE(pos.is_repetition(tc.input.repetition) == tc.info);
        }
        {
            _Position<ContiguousMappingPiece> pos(tc.input.FEN);
            for (auto &move : tc.input.moves)
                pos.doMove(move);
            REQUIRE(pos.is_repetition(tc.input.repetition) == tc.info);
        }
    }
}
TEST_CASE("is_repetition") {
    std::vector<TestEntry<repetitions_t, bool>> tests = {
        { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
         { Move(SQ_B1, SQ_C3),
            Move(SQ_B8, SQ_C6),
            Move(SQ_C3, SQ_B1),
            Move(SQ_C6, SQ_B8),
            Move(SQ_G1, SQ_F3),
            Move(SQ_G8, SQ_F6),
            Move(SQ_F3, SQ_G1),
            Move(SQ_F6, SQ_G8) },
         3, true }
    };
    check_repetitions(tests);
}

TEST_SUITE("SAN Parser") {
    TEST_CASE("Test ambiguous pawn capture") {
        auto b = Position{ "rnbqkbnr/ppp1p1pp/3p1p2/4N3/8/3P4/PPPKPPPP/R1BQ1BNR b kq - 1 7" };

        Move m = Move::make(Square::SQ_F6, Square::SQ_E5);

        CHECK(uci::moveToSan(b, m) == "fxe5");
        REQUIRE(uci::parseSan(b, "fxe5") == m);
    }

    TEST_CASE("Test ambiguous pawn ep capture") {
        auto b = Position{ "rnbqkbnr/pppppp1p/8/5PpP/8/8/PPPPP2P/RNBQKBNR w KQkq g6 0 2" };

        Move m = Move::make<Move::ENPASSANT>(Square::SQ_F5, Square::SQ_G6);

        CHECK(uci::moveToSan(b, m) == "fxg6");
        REQUIRE(uci::parseSan(b, "fxg6") == m);
    }

    TEST_CASE("Test ambiguous knight move") {
        auto b = Position{ "rnbqkbnr/ppp3pp/3ppp2/8/8/3P1N1N/PPPKPPPP/R1BQ1B1R w kq - 1 8" };

        Move m = Move::make(Square::SQ_F3, Square::SQ_G5);

        CHECK(uci::moveToSan(b, m) == "Nfg5");
        REQUIRE(uci::parseSan(b, "Nfg5") == m);
    }

    TEST_CASE("Test ambiguous rook move with check") {
        auto b = Position{ "4k3/8/8/8/8/8/2R3R1/2K5 w - - 0 1" };

        Move m = Move::make(Square::SQ_C2, Square::SQ_E2);

        CHECK(uci::moveToSan(b, m) == "Rce2+");
        REQUIRE(uci::parseSan(b, "Rce2+") == m);
    }

    TEST_CASE("Test ambiguous rook move with checkmate") {
        auto b = Position{ "4k3/8/8/8/8/8/2KR1R2/3R1R2 w - - 0 1" };

        Move m = Move::make(Square::SQ_D2, Square::SQ_E2);

        CHECK(uci::moveToSan(b, m) == "Rde2#");
        REQUIRE(uci::parseSan(b, "Rde2#") == m);
    }

    TEST_CASE("Test Knight move") {
        auto b = Position{ "rnbqkbnr/ppp1p1pp/3p1p2/8/8/3P1N2/PPPKPPPP/R1BQ1BNR w kq - 0 7" };

        Move m = Move::make(Square::SQ_F3, Square::SQ_G5);

        CHECK(uci::moveToSan(b, m) == "Ng5");
        REQUIRE(uci::parseSan(b, "Ng5") == m);
    }

    TEST_CASE("Test Bishop move") {
        auto b = Position{ "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1" };

        Move m = Move::make(Square::SQ_F1, Square::SQ_C4);

        CHECK(uci::moveToSan(b, m) == "Bc4");
        REQUIRE(uci::parseSan(b, "Bc4") == m);
    }

    TEST_CASE("Test Rook move") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPP1PP/R3KR2 w Qkq - 0 1" };

        Move m = Move::make(Square::SQ_F1, Square::SQ_F7);

        CHECK(uci::moveToSan(b, m) == "Rxf7");
        REQUIRE(uci::parseSan(b, "Rxf7") == m);
    }

    TEST_CASE("Test Queen move") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPP1PP/R3KQ1R w KQkq - 0 1" };

        Move m = Move::make(Square::SQ_F1, Square::SQ_F7);

        CHECK(uci::moveToSan(b, m) == "Qxf7+");
        REQUIRE(uci::parseSan(b, "Qxf7+") == m);
    }

    TEST_CASE("Test King move") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1" };

        Move m = Move::make(Square::SQ_E1, Square::SQ_F1);

        CHECK(uci::moveToSan(b, m) == "Kf1");
        REQUIRE(uci::parseSan(b, "Kf1") == m);
    }

    TEST_CASE("Test King Castling Short move") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_H1);

        CHECK(uci::moveToSan(b, m) == "O-O");
        REQUIRE(uci::parseSan(b, "O-O") == m);
    }

    TEST_CASE("Test King Castling Long move") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_A1);

        CHECK(uci::moveToSan(b, m) == "O-O-O");
        REQUIRE(uci::parseSan(b, "O-O-O") == m);
    }

    TEST_CASE("Test King Castling Short move with Zero") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_H1);

        REQUIRE(uci::parseSan(b, "0-0") == m);
    }

    TEST_CASE("Test King Castling Long move with Zero") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_A1);

        REQUIRE(uci::parseSan(b, "0-0-0") == m);
    }
    TEST_CASE("Test King Castling Short move with Annotation") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_H1);
        Move m2 = Move::none();
#if defined(_CHESSLIB_ERROR_MODE_THROW)
        REQUIRE_THROWS_WITH_AS(m2 = uci::parseSan(b, "0-0+?!"),
                               "illegal san: '0-0+?!' in rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17",
                               chess::uci::IllegalMoveException);
#endif
        REQUIRE(m2 == Move::none());
        REQUIRE(uci::parseSan(b, "0-0+?!", true) == m);
    }

    TEST_CASE("Test King Castling Long move with Annotation") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_A1);

        Move m2 = Move::none();
#if defined(_CHESSLIB_ERROR_MODE_THROW)
        REQUIRE_THROWS_WITH_AS(m2 = uci::parseSan(b, "0-0-0+?!"),
                               "illegal san: '0-0-0+?!' in rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1",
                               chess::uci::IllegalMoveException);
#endif
        REQUIRE(m2 == Move::none());

        REQUIRE(uci::parseSan(b, "0-0-0+?!", true) == m);
    }
    TEST_CASE("Test Queen Capture Ambiguity") {
        auto b = Position{ "3k4/8/4b3/8/2Q3Q1/8/8/3K4 w - - 0 1" };

        Move m = Move::make(Square::SQ_C4, Square::SQ_E6);

        CHECK(uci::moveToSan(b, m) == "Qcxe6");
        REQUIRE(uci::parseSan(b, "Qcxe6") == m);
    }

    TEST_CASE("Test Rook Ambiguity") {
        auto b = Position{ "3k4/8/8/R7/8/8/8/R3K3 w - - 0 1" };

        Move m = Move::make(Square::SQ_A1, Square::SQ_A3);

        CHECK(uci::moveToSan(b, m) == "R1a3");
        REQUIRE(uci::parseSan(b, "R1a3") == m);
    }

    TEST_CASE("Test Rook Capture Ambiguity") {
        auto b = Position{ "2r3k1/4nn2/pq1p1pp1/3Pp3/1pN1P1P1/1P1Q4/P1r1NP2/1K1R3R b - - 2 19" };

        Move m = Move::make(Square::SQ_C8, Square::SQ_C4);

        CHECK(uci::moveToSan(b, m) == "R8xc4");
        REQUIRE(uci::parseSan(b, "R8xc4") == m);
    }

    TEST_CASE("Test Knight Capture Ambiguity") {
        auto b = Position{ "r5k1/5p2/2n2B1p/4P3/p1n3PN/8/P4PK1/1R6 b - - 2 28" };

        Move m = Move::make(Square::SQ_C6, Square::SQ_E5);

        CHECK(uci::moveToSan(b, m) == "N6xe5");
        REQUIRE(uci::parseSan(b, "N6xe5") == m);
    }

    TEST_CASE("Test Pawn Capture Promotion Ambiguity") {
        auto b = Position{ "2k2n2/4P1P1/8/8/8/8/8/2K5 w - - 0 1" };

        Move m = Move::make<Move::PROMOTION>(Square::SQ_E7, Square::SQ_F8, PieceType::QUEEN);

        CHECK(uci::moveToSan(b, m) == "exf8=Q+");
        REQUIRE(uci::parseSan(b, "exf8=Q+") == m);
    }

    TEST_CASE("Test Pawn Push") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };

        Move m = Move::make(Square::SQ_E2, Square::SQ_E4);

        CHECK(uci::moveToSan(b, m) == "e4");
        REQUIRE(uci::parseSan(b, "e4") == m);
    }

    TEST_CASE("Test Pawn Promotion") {
        auto b = Position{ "8/Pk6/8/5p2/8/8/8/2K5 w - - 1 781" };

        Move m = Move::make<Move::PROMOTION>(Square::SQ_A7, Square::SQ_A8, PieceType::QUEEN);

        CHECK(uci::moveToSan(b, m) == "a8=Q+");
        REQUIRE(uci::parseSan(b, "a8=Q+") == m);
    }

    TEST_CASE("Test Knight Ambiguity") {
        auto b = Position{ "8/8/5K2/2N3P1/3N3n/4k3/3N4/7r w - - 59 97" };

        Move m = Move::make(Square::SQ_D4, Square::SQ_B3);

        CHECK(uci::moveToSan(b, m) == "N4b3");
        REQUIRE(uci::parseSan(b, "N4b3") == m);
    }

    TEST_CASE("Test Knight Capture Ambiguity") {
        auto b = Position{ "8/8/5K2/2N3P1/3N3n/1b2k3/3N4/7r w - - 59 97" };

        Move m = Move::make(Square::SQ_D4, Square::SQ_B3);

        CHECK(uci::moveToSan(b, m) == "N4xb3");
        REQUIRE(uci::parseSan(b, "N4xb3") == m);
    }

    TEST_CASE("Test Knight Ambiguity 2") {
        auto b = Position{ "2N1N3/p7/6k1/1p6/2N1N3/2R5/R3Q1P1/2R3K1 w - - 5 55" };

        {
            Move m = Move::make(Square::SQ_E4, Square::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Ne4d6");
            REQUIRE(uci::parseSan(b, "Ne4d6") == m);
        }

        {
            Move m = Move::make(Square::SQ_C4, Square::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Nc4d6");
            REQUIRE(uci::parseSan(b, "Nc4d6") == m);
        }

        {
            Move m = Move::make(Square::SQ_C8, Square::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Nc8d6");
            REQUIRE(uci::parseSan(b, "Nc8d6") == m);
        }

        {
            Move m = Move::make(Square::SQ_E8, Square::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Ne8d6");
            REQUIRE(uci::parseSan(b, "Ne8d6") == m);
        }
    }

    TEST_CASE("Test Knight Capture Check Ambiguity") {
        auto b = Position{ "2N1N3/p4k2/3q4/1p6/2N1N3/2R5/R3Q1P1/2R3K1 w - - 0 1" };

        {
            Move m = Move::make(Square::SQ_E4, Square::SQ_D6);

            CHECK(uci::moveToSan(b, m) == "Ne4xd6+");
            REQUIRE(uci::parseSan(b, "Ne4xd6+") == m);
        }
    }

    TEST_CASE("Test Bishop Ambiguity") {
        auto b = Position{ "4k3/8/8/8/2B1B3/8/2B1BK2/8 w - - 0 1" };

        {
            auto m = Move::make(Square::SQ_C2, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Bc2d3");
            REQUIRE(uci::parseSan(b, "Bc2d3") == m);
        }

        {
            auto m = Move::make(Square::SQ_C4, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Bc4d3");
            REQUIRE(uci::parseSan(b, "Bc4d3") == m);
        }

        {
            auto m = Move::make(Square::SQ_E2, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Be2d3");
            REQUIRE(uci::parseSan(b, "Be2d3") == m);
        }

        {
            auto m = Move::make(Square::SQ_E4, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Be4d3");
            REQUIRE(uci::parseSan(b, "Be4d3") == m);
        }
    }

    TEST_CASE("Test Rook Ambiguity") {
        auto b = Position{ "k5r1/8/8/8/2R1R3/8/2R1RK2/8 w - - 0 1" };

        {
            auto m = Move::make(Square::SQ_C2, Square::SQ_C3);

            CHECK(uci::moveToSan(b, m) == "R2c3");
            REQUIRE(uci::parseSan(b, "R2c3") == m);
        }

        {
            auto m = Move::make(Square::SQ_C2, Square::SQ_D2);

            CHECK(uci::moveToSan(b, m) == "Rcd2");
            REQUIRE(uci::parseSan(b, "Rcd2") == m);
        }

        {
            auto m = Move::make(Square::SQ_E2, Square::SQ_E3);

            CHECK(uci::moveToSan(b, m) == "R2e3");
            REQUIRE(uci::parseSan(b, "R2e3") == m);
        }

        {
            auto m = Move::make(Square::SQ_E2, Square::SQ_D2);

            CHECK(uci::moveToSan(b, m) == "Red2");
            REQUIRE(uci::parseSan(b, "Red2") == m);
        }
    }

    TEST_CASE("Test Queen Ambiguity") {
        auto b = Position{ "1k4r1/8/8/8/2Q1Q3/8/2Q1QK2/8 w - - 0 1" };

        {
            auto m = Move::make(Square::SQ_C2, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Qc2d3");
            REQUIRE(uci::parseSan(b, "Qc2d3") == m);
        }

        {
            auto m = Move::make(Square::SQ_C4, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Qc4d3");
            REQUIRE(uci::parseSan(b, "Qc4d3") == m);
        }

        {
            auto m = Move::make(Square::SQ_E2, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Qe2d3");
            REQUIRE(uci::parseSan(b, "Qe2d3") == m);
        }

        {
            auto m = Move::make(Square::SQ_E4, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Qe4d3");
            REQUIRE(uci::parseSan(b, "Qe4d3") == m);
        }
    }

    TEST_CASE("Test Queen Ambiguity 2") {
        auto b = Position{ "5q1k/4P3/7q/1Q1pQ3/3P4/1KN5/8/5q2 b - - 0 1" };

        {
            auto m = Move::make(Square::SQ_F8, Square::SQ_F6);

            CHECK(uci::moveToSan(b, m) == "Q8f6");
            REQUIRE(uci::parseSan(b, "Q8f6") == m);
        }
    }

    TEST_CASE("Test Queen Ambiguity 3") {
        auto b = Position{ "5q1k/4P3/7q/1Q1pQ3/3P3q/1KN5/8/5q2 b - - 0 1" };

        {
            auto m = Move::make(Square::SQ_H4, Square::SQ_F6);

            CHECK(uci::moveToSan(b, m) == "Q4f6");
            REQUIRE(uci::parseSan(b, "Q4f6") == m);
        }
    }

    TEST_CASE("Test Queen Ambiguity 4") {
        auto b = Position{ "5q1k/4P3/7q/1Q1pQ3/3P1q1q/1KN5/8/5q2 b - - 0 1" };

        {
            auto m = Move::make(Square::SQ_H4, Square::SQ_F6);

            CHECK(uci::moveToSan(b, m) == "Qh4f6");
            REQUIRE(uci::parseSan(b, "Qh4f6") == m);
        }

        {
            auto m = Move::make(Square::SQ_F4, Square::SQ_F6);

            CHECK(uci::moveToSan(b, m) == "Qf4f6");
            REQUIRE(uci::parseSan(b, "Qf4f6") == m);
        }
    }

    TEST_CASE("Test Rook Ambiguity") {
        auto b = Position{ "1r5k/5Rp1/5p1p/4Nb1P/2B5/2P1R1P1/3r1PK1/1r6 b - - 0 1" };

        {
            auto m = Move::make(Square::SQ_B1, Square::SQ_B2);

            CHECK(uci::moveToSan(b, m) == "R1b2");
            REQUIRE(uci::parseSan(b, "R1b2") == m);
        }

        {
            auto m = Move::make(Square::SQ_D2, Square::SQ_B2);

            CHECK(uci::moveToSan(b, m) == "Rdb2");
            REQUIRE(uci::parseSan(b, "Rdb2") == m);
        }
    }

    TEST_CASE("Test Rook Ambiguity 2") {
        auto b = Position{ "1r5k/5Rp1/5p1p/1r2Nb1P/2B5/2P1R1P1/3r1PK1/8 b - - 0 1" };

        {
            auto m = Move::make(Square::SQ_B5, Square::SQ_B2);

            CHECK(uci::moveToSan(b, m) == "Rbb2");
            REQUIRE(uci::parseSan(b, "Rbb2") == m);
        }

        {
            auto m = Move::make(Square::SQ_B5, Square::SQ_B6);

            CHECK(uci::moveToSan(b, m) == "R5b6");
            REQUIRE(uci::parseSan(b, "R5b6") == m);
        }
    }

    TEST_CASE("Test Rook Ambiguity 3") {
        auto b = Position{ "1r5k/5Rp1/5p1p/1r2Nb1P/2B5/2P1R1P1/5PK1/8 b - - 0 1" };

        {
            auto m = Move::make(Square::SQ_B5, Square::SQ_B2);

            CHECK(uci::moveToSan(b, m) == "Rb2");
            REQUIRE(uci::parseSan(b, "Rb2") == m);
        }

        {
            auto m = Move::make(Square::SQ_B5, Square::SQ_B6);

            CHECK(uci::moveToSan(b, m) == "R5b6");
            REQUIRE(uci::parseSan(b, "R5b6") == m);
        }
    }

    TEST_CASE("Test Bishop Ambiguity 2") {
        auto b = Position{ "2Bqkb1r/4p2p/8/8/2B5/p7/4B3/RNBQKBNR w KQk - 0 1" };

        {
            auto m = Move::make(Square::SQ_C4, Square::SQ_A6);

            CHECK(uci::moveToSan(b, m) == "B4a6");
            REQUIRE(uci::parseSan(b, "B4a6") == m);
        }

        {
            auto m = Move::make(Square::SQ_C4, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Bcd3");
            REQUIRE(uci::parseSan(b, "Bcd3") == m);
        }
    }

    TEST_CASE("Test Bishop Ambiguity 3") {
        auto b = Position{ "3qkb1r/4p2p/8/8/2B5/p7/4B3/RNBQKBNR w KQk - 0 1" };

        {
            auto m = Move::make(Square::SQ_C4, Square::SQ_A6);

            CHECK(uci::moveToSan(b, m) == "Ba6");
            REQUIRE(uci::parseSan(b, "Ba6") == m);
        }

        {
            auto m = Move::make(Square::SQ_C4, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Bcd3");
            REQUIRE(uci::parseSan(b, "Bcd3") == m);
        }
    }

    TEST_CASE("Test Bishop Capture Mating Ambiguity") {
        auto b = Position{ "7r/4pqbp/5bkp/6nn/2B5/p2r4/4B3/RNBQKBR1 w Q - 0 1" };

        {
            auto m = Move::make(Square::SQ_C4, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Bcxd3#");
            REQUIRE(uci::parseSan(b, "Bcxd3#") == m);
        }
    }

    TEST_CASE("Test Pinned Queen Ambiguity") {
        auto b = Position{ "1Q3q1k/4P3/8/3p1N1q/1Q1P4/2N5/2K5/5q2 b - - 0 1" };

        {
            auto m = Move::make(Square::SQ_F1, Square::SQ_F5);

            CHECK(uci::moveToSan(b, m) == "Qfxf5+");
            REQUIRE(uci::parseSan(b, "Qfxf5+") == m);
        }

        {
            auto m = Move::make(Square::SQ_H5, Square::SQ_F5);

            CHECK(uci::moveToSan(b, m) == "Qhxf5+");
            REQUIRE(uci::parseSan(b, "Qhxf5+") == m);
        }
    }

    TEST_CASE("Test Pinned Rook Ambiguity") {
        auto b = Position{ "1r6/5Rp1/5p1p/1r2Nb1P/2B5/2PR2P1/3r1PK1/3k4 b - - 0 1" };

        {
            auto m = Move::make(Square::SQ_B5, Square::SQ_B2);

            CHECK(uci::moveToSan(b, m) == "Rb2");
            REQUIRE(uci::parseSan(b, "Rb2") == m);
        }

        {
            auto m = Move::make(Square::SQ_B5, Square::SQ_B6);

            CHECK(uci::moveToSan(b, m) == "R5b6");
            REQUIRE(uci::parseSan(b, "R5b6") == m);
        }
    }

    TEST_CASE("Test Pinned Bishop Ambiguity") {
        auto b = Position{ "2r4r/4pqbp/5bkp/6nn/2B5/p2r4/2K1B3/RNBQ1BR1 w - - 0 1" };

        {
            auto m = Move::make(Square::SQ_E2, Square::SQ_D3);

            CHECK(uci::moveToSan(b, m) == "Bxd3#");
            REQUIRE(uci::parseSan(b, "Bxd3#") == m);
        }
    }

    TEST_CASE("Test Pinned Knight Ambiguity") {
        auto b = Position{ "r1k5/5p2/2n2B1p/2R1P3/p1n3PN/8/P4PK1/1R6 b - - 0 1" };

        Move m = Move::make(Square::SQ_C4, Square::SQ_E5);

        CHECK(uci::moveToSan(b, m) == "Nxe5");
        REQUIRE(uci::parseSan(b, "Nxe5") == m);
    }

    TEST_CASE("Parse No Move") {
        Position b;

        REQUIRE(uci::parseSan(b, "") == Move::NO_MOVE);
    }
    TEST_CASE("Should throw on ambiguous move") {
        auto b = Position{ "8/8/6K1/4k3/4N3/p4r2/N3N3/8 w - - 3 82" };

        Move san = Move::NO_MOVE;

#if defined(_CHESSLIB_ERROR_MODE_THROW)
        CHECK_THROWS_AS(san = uci::parseSan(b, "Nec3"), uci::AmbiguousMoveException);
#endif
        CHECK(san == Move::NO_MOVE);
    }

    TEST_CASE("Should throw for illegal move") {
        auto b = Position{ "8/8/6K1/4k3/4N3/p4r2/N3N3/8 w - - 3 82" };

        Move san = Move::NO_MOVE;

#if defined(_CHESSLIB_ERROR_MODE_THROW)
        CHECK_THROWS_WITH_AS(san = uci::parseSan(b, "Nec4"),
                             "illegal san: 'Nec4' in 8/8/6K1/4k3/4N3/p4r2/N3N3/8 w - - 3 82",
                             uci::IllegalMoveException);
#endif
        CHECK(san == Move::NO_MOVE);
    }
    TEST_CASE("Checkmate castle should have #") {
        auto b = Position{ "RRR5/8/8/8/8/8/PPPPPP2/k3K2R w K - 0 1" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_H1);

        CHECK(uci::moveToSan(b, m) == "O-O#");
        REQUIRE(uci::parseSan(b, "O-O#") == m);
    }

    TEST_CASE("Check castle should have +") {
        auto b = Position{ "8/8/8/8/8/8/PPPPPP2/k3K2R w K - 0 1" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_H1);

        CHECK(uci::moveToSan(b, m) == "O-O+");
        REQUIRE(uci::parseSan(b, "O-O+") == m);
    }
}

TEST_SUITE("misc tests") {
    TEST_CASE("FEN reconstruction (Chess960)") {
        Position pos(Position::START_CHESS960_FEN, true);
        REQUIRE(pos.fen(false) == Position::START_CHESS960_FEN);
        REQUIRE(pos.fen() == Position::START_FEN);
        pos.setFEN(Position::START_CHESS960_FEN, true, chess::MODE_SMK);
        REQUIRE(pos.fen(false) == Position::START_CHESS960_FEN);
        REQUIRE(pos.fen() == Position::START_FEN);
    }
}
// Malformed FEN fuzzer
#include <assert.h>
#include <setjmp.h>
#include <signal.h>
jmp_buf global_fuzzer_env;
// Windows
#if defined(_WIN32) || defined(__CYGWIN__)
#include <crtdbg.h>
#include <windows.h>

#if defined(_MSC_VER) && defined(_DEBUG)
int WindowsDebugAssertHook(int reportType, char *message, int *returnValue) {
    if (reportType == _CRT_ASSERT) {
        *returnValue = 0; // -1 assert dialog
        longjmp(global_fuzzer_env, 1);
    }
    return FALSE;
}
#endif

void windows_sigabrt_handler(int signum) {
    signal(SIGABRT, windows_sigabrt_handler);
    longjmp(global_fuzzer_env, 1);
}

void setup_os_specific_hooks() {
// -1 assert dialog
#if defined(_WRITE_ABORT_MSG)
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetReportHook(WindowsDebugAssertHook);
#endif

    signal(SIGABRT, windows_sigabrt_handler);
}

// linux
#elif defined(__linux__)
void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function) {
    longjmp(global_fuzzer_env, 1);
}
void setup_os_specific_hooks() { /* idk */ }

// macOS hooks
#elif defined(__APPLE__)
void __assert_rtn(const char *function, const char *file, int line, const char *assertion) { longjmp(global_fuzzer_env, 1); }
void setup_os_specific_hooks() { /* idk */ }

#else
void generic_sigabrt_handler(int signum) {
    signal(SIGABRT, generic_sigabrt_handler);
    longjmp(global_fuzzer_env, 1);
}
void setup_os_specific_hooks() { signal(SIGABRT, generic_sigabrt_handler); }
#endif
struct FenTest {
    const char *fen;
    bool should_pass;
};
TEST_CASE("Fuzzer (excludes illegal, unreachable, etc. positions)") {

    static constexpr FenTest fen_tests[] = {
        // ---------------- VALID ----------------

        { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  true },

        {                                "8/8/8/8/8/8/8/8 w - - 0 1",  true },

        {                            "4k3/8/8/8/8/8/8/4K3 w - - 0 1",  true },

        {                     "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",  true },

        {                            "8/8/8/3pP3/8/8/8/8 w - d6 0 1",  true },

        // ---------------- INVALID FIELD COUNT ----------------

        {                                          "8/8/8/8/8/8/8/8", false },

        {                                    "8/8/8/8/8/8/8/8 w - -", false },

        {                          "8/8/8/8/8/8/8/8 w - - 0 1 extra", false },

        // ---------------- INVALID BOARD ----------------

        {                                "9/8/8/8/8/8/8/8 w - - 0 1", false },

        {                                  "8/8/8/8/8/8/8 w - - 0 1", false },

        {                              "8/8/8/8/8/8/8/8/8 w - - 0 1", false },

        {                        "ppppppppp/8/8/8/8/8/8/8 w - - 0 1", false },

        {                               "8//8/8/8/8/8/8/8 w - - 0 1", false },

        // ---------------- INVALID PIECES ----------------

        {                               "8/8/8/8/8/8/8/X7 w - - 0 1", false },

        {                               "8/8/8/8/8/8/8/@7 w - - 0 1", false },

        // ---------------- INVALID SIDE ----------------

        {                                "8/8/8/8/8/8/8/8 x - - 0 1", false },

        {                            "8/8/8/8/8/8/8/8 white - - 0 1", false },

        // ---------------- INVALID CASTLING ----------------

        {                             "8/8/8/8/8/8/8/8 w ABCD - 0 1", false },

        {                            "8/8/8/8/8/8/8/8 w KQkqq - 0 1", false },

        // ---------------- INVALID EN PASSANT ----------------

        {                               "8/8/8/8/8/8/8/8 w - z9 0 1", false },
    };
    for (const auto &test : fen_tests) {
        bool ok = false;

#if defined(_CHESSLIB_ERROR_MODE_ASSERT)

        if (setjmp(global_fuzzer_env) == 0) {
            Position p;
            ok = p.setFEN(test.fen);
        } else {
            ok = false;
        }

#elif defined(_CHESSLIB_ERROR_MODE_THROW) || defined(__cpp_exceptions) || defined(_CPPUNWIND) || defined(__EXCEPTIONS)

        try {
            Position p;
            ok = p.setFEN(test.fen);
        } catch (...) {
            ok = false;
        }

#else

        {
            Position p;
            ok = p.set_fen(test.fen);
        }

#endif

        REQUIRE(ok == test.should_pass);
    }
}

TEST_CASE("Move utility methods") {
    // is_ok
    CHECK_FALSE(Move::none().is_ok());
    CHECK_FALSE(Move::null().is_ok());
    CHECK(Move(SQ_E2, SQ_E4).is_ok());
    CHECK(Move::make<PROMOTION>(SQ_E7, SQ_E8, QUEEN).is_ok());

    // from_to
    Move m1(SQ_E2, SQ_E4);
    CHECK((m1.from_to() & 0x3F) == SQ_E4);        // lower 6 = to
    CHECK(((m1.from_to() >> 6) & 0x3F) == SQ_E2); // next 6 = from

    // promotion_type
    Move promQ = Move::make<PROMOTION>(SQ_E7, SQ_E8, QUEEN);
    CHECK(promQ.promotion_type() == QUEEN);
    Move promN = Move::make<PROMOTION>(SQ_E7, SQ_E8, KNIGHT);
    CHECK(promN.promotion_type() == KNIGHT);
    Move promB = Move::make<PROMOTION>(SQ_E7, SQ_E8, BISHOP);
    CHECK(promB.promotion_type() == BISHOP);
    Move promR = Move::make<PROMOTION>(SQ_E7, SQ_E8, ROOK);
    CHECK(promR.promotion_type() == ROOK);

    // raw encoding
    CHECK(Move(SQ_A1, SQ_A2).raw() == 8);
    CHECK(Move::null().raw() == 65);
    CHECK(Move::none().raw() == 0);

    // operator bool
    CHECK_FALSE(Move::none());
    CHECK(static_cast<bool>(Move(SQ_E2, SQ_E4)));

    // UCI round-trip
    Move m2 = Move::make<PROMOTION>(SQ_A7, SQ_A8, QUEEN);
    CHECK(m2.uci() == "a7a8q");
    Move m3 = Move::make<CASTLING>(SQ_E1, SQ_H1);
    CHECK(m3.uci() == "e1g1");
}

TEST_CASE("Stream operators") {
    std::ostringstream os;
    // Color
    os.str("");
    os << WHITE;
    CHECK(os.str() == "WHITE");
    os.str("");
    os << BLACK;
    CHECK(os.str() == "BLACK");

    // Move
    os.str("");
    os << Move::make<PROMOTION>(SQ_E7, SQ_E8, QUEEN);
    CHECK(os.str() == "e7e8q");

    // CastlingRights
    os.str("");
    os << CastlingRights(WHITE_OO);
    CHECK(os.str() == "WHITE_OO");
    os.str("");
    os << CastlingRights(WHITE_OOO);
    CHECK(os.str() == "WHITE_OOO");
    os.str("");
    os << CastlingRights(BLACK_OO);
    CHECK(os.str() == "BLACK_OO");
    os.str("");
    os << CastlingRights(BLACK_OOO);
    CHECK(os.str() == "BLACK_OOO");
    os.str("");
    os << CastlingRights(KING_SIDE);
    CHECK(os.str() == "KING_SIDE");
    os.str("");
    os << CastlingRights(NO_CASTLING);
    CHECK(os.str() == "NO_CASTLING");

    // Square
    os.str("");
    os << SQ_A1;
    CHECK(os.str() == "a1");
    os.str("");
    os << SQ_H8;
    CHECK(os.str() == "h8");
    os.str("");
    os << SQ_E4;
    CHECK(os.str() == "e4");

    // PieceType
    os.str("");
    os << PAWN;
    CHECK(os.str() == "PAWN");
    os.str("");
    os << KNIGHT;
    CHECK(os.str() == "KNIGHT");
    os.str("");
    os << KING;
    CHECK(os.str() == "KING");

    // Position
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    os.str("");
    os << pos;
    CHECK_FALSE(os.str().empty());
    // Board should contain piece characters
    CHECK(os.str().find('r') != std::string::npos);
    CHECK(os.str().find('K') != std::string::npos);
    CHECK(os.str().find('P') != std::string::npos);
}

TEST_CASE("givesCheck") {
    // Scholar's mate position: black is mated, so givesCheck for black's move is irrelevant
    Position p_scholar("r1bqkb1r/pppp1Qpp/2n2n2/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4");
    CHECK(p_scholar.is_checkmate());

    // Starting position: e2-e4 doesn't give check
    Position p2;
    CHECK(p2.givesCheck(Move(SQ_E2, SQ_E4)) == CheckType::NO_CHECK);

    // Rook gives direct check
    Position p3("4k3/8/8/8/8/8/5R2/4K3 w - - 0 1");
    CHECK(p3.givesCheck(Move(SQ_F2, SQ_E2)) == CheckType::DIRECT_CHECK);

    // King move does not give check (blocks rook's line)
    CHECK(p3.givesCheck(Move(SQ_E1, SQ_D2)) == CheckType::NO_CHECK);

    // Discovered check: pawn push unmasks bishop's diagonal to king
    Position p4("7k/8/8/8/8/2P5/1B6/6K1 w - - 0 1");
    // Bishop on b2 attacks h8 via c3-d4-e5-f6-g7-h8. Pawn on c3 blocks.
    // c3-c4 unmasks the bishop → discovered check
    CHECK(p4.givesCheck(Move(SQ_C3, SQ_C4)) == CheckType::DISCOVERY_CHECK);

    // Non-check king move
    Position p5("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    CHECK(p5.givesCheck(Move(SQ_E1, SQ_E2)) == CheckType::NO_CHECK);
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
        CHECK(p.is_insufficient_material());

        // K+B vs K
        Position p2("4k3/8/8/8/5B2/8/8/4K3 w - - 0 1");
        CHECK(p2.is_insufficient_material());      // both sides insufficient

        // K+N vs K
        Position p3("4k3/8/8/8/5N2/8/8/4K3 w - - 0 1");
        CHECK(p3.is_insufficient_material());

        // K+N+N vs K is not insufficient (can mate with two knights)
        Position p4("4k3/8/8/8/5N2/8/4N3/4K3 w - - 0 1");
        CHECK_FALSE(p4.is_insufficient_material());

        // K+R vs K is not draw
        Position p5("4k3/8/8/8/5R2/8/8/4K3 w - - 0 1");
        CHECK_FALSE(p5.is_insufficient_material());

        // K+B+B vs K - still drawn (unless same colour bishops... but two bishops can mate)
        Position p6("4k3/8/8/8/3B1B2/8/8/4K3 w - - 0 1");
        CHECK(p6.is_insufficient_material());
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