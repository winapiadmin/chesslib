#define DOCTEST_CONFIG_IMPLEMENT
#ifndef __EXCEPTIONS
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#endif
#include "moves_io.h"
#include "position.h"
#include "printers.h"
#include <chrono>
#include <doctest/doctest.h>
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
        {                "3K4/1N1Q4/8/2r4k/6r1/8/1q1Q4/r3P3 w q - 24 70", false },
        {                         "8/8/3b3K/4N3/4k3/7n/4q3/7B w - - 8 9",  true },
        {            "N7/1B4P1/4B2n/2Kn4/8/3k4/Pp2r1p1/1p5P w - - 29 85", false },
        {                 "4b3/3n3N/8/2k5/1q6/1B6/4R3/2K1R3 w - g3 37 3", false },
        {                   "8/r7/1K3p2/2q5/2R5/6N1/3B4/2p4k w q - 2 89",  true },
        {           "8/1q3Pr1/2b5/2P2r2/7K/5k2/PP1n4/Q2b3p w K g3 28 35", false },
        {               "K7/1r6/4B1p1/7p/3P4/4k3/4R3/1np3b1 w - - 16 84",  true },
        {         "2R2r2/2k2N2/7K/2r2NR1/1n3b2/PP6/3q3b/B7 w q h3 38 21",  true },
        {        "2Q5/3b4/1b4Rr/4R1K1/7q/2P2R1q/R5Qp/1bn1k3 w - g3 8 76",  true },
        {            "4k3/bP1r4/n7/8/2PP2K1/4B3/2N1P1q1/7P w q a6 37 18", false },
        {        "1P4N1/2Nb4/1K6/5B2/2N3R1/2k1Nn2/5N2/1p3p2 b q - 41 98", false },
        {                  "Q4rN1/5R2/R5b1/7r/8/r7/kK5B/7B b K c6 37 73",  true },
        {     "4k2p/r1q5/7n/1p4P1/1P3PQr/N1n2N2/7K/1N1NqB2 b - f6 27 80",  true },
        {             "7Q/2p2b2/8/3K4/5Q2/N2B1bb1/5b2/5qk1 w K d3 30 68", false },
        {              "3b3K/5p2/2Pp4/2n1R3/5Qb1/7k/6n1/R7 b - a3 23 54", false },
        {             "1n1PN1N1/3Pn3/3r4/2b5/6K1/7r/5b2/3k4 w q - 15 51", false },
        {          "4N3/4kN2/n2p4/4K3/r1P4R/1p4n1/b3r3/3P1n2 b - - 9 43",  true },
        {           "1Q2q3/k4bBq/5B1P/8/6q1/r1q3p1/1K6/2PP4 w - - 16 90",  true },
        {                 "Kqq1B2b/3R1Q2/8/k7/6P1/8/1P4R1/8 w - e6 33 9", false },
        {              "7K/Nn3b2/r1N1pP2/4R3/5q2/kq6/4p2b/8 w - e3 5 11", false },
        {                   "n3NK1B/B5kn/8/8/6pB/4P3/8/5R2 b k g3 20 41",  true },
        {                  "Kq3Bq1/6p1/4P3/k7/3R4/8/1Npn4/8 b K - 31 84",  true },
        {          "7N/1RK1p1nb/5n2/3N4/QQ2pkr1/1r6/3P1P2/8 b - - 44 77", false },
        {      "1R2RN1q/nn2Q1k1/5b1Q/3n2n1/2qN3Q/1K1R2B1/8/8 w - - 49 2",  true },
        {            "8/5bQn/3n2B1/1K2B3/4NN1n/6q1/4R3/2q4k w - - 8 100", false },
        {            "8/8/1P4P1/7r/1Q1Q4/2R2P1b/6n1/1RbK1Q1k b - - 44 5", false },
        {             "2r2p2/1B6/2r5/5p2/8/3b2q1/2R3qK/k1q5 b - - 11 89",  true },
        {            "1b1Q4/1rk2qP1/8/R1r4q/4p3/2K4p/5P2/R7 b Q - 23 29",  true },
        {         "qq3B2/2r4n/3n4/r1Q5/2P5/5b2/1b1k1K2/P2n4 w Q - 49 89", false },
        {        "B1bn1R1r/1N5k/2p5/4b2r/6n1/K5B1/2n1n3/1P3N2 b q - 6 8",  true },
        {             "8/6q1/3nr3/3n4/KQ6/1bp2q2/1Bk5/2r2Q2 w - b3 0 89", false },
        {                    "4r3/p1n4N/8/8/8/4qnP1/6k1/4K3 b K - 12 83",  true },
        {            "3b1Nk1/4R1nR/3r4/3P4/R7/K2n4/1qB5/4r2R b q - 6 73",  true },
        {        "6r1/1Q1B2B1/1bK2QR1/1r6/r7/4p2B/8/2r1k1q1 b - - 44 39",  true },
        {                    "5R2/b7/3p2P1/8/2k5/qK2Q3/7p/8 w K - 33 11",  true },
        {               "1R3Q1Q/p7/8/4B1pq/6k1/2N1Q3/8/6K1 w - - 35 100", false },
        {              "Bp6/PQ5q/8/4k3/7b/2q1Nr2/2K5/2n1q2q w K - 26 14", false },
        {               "3p4/4k1Q1/5B1b/n1q5/8/2Q4K/3Q4/4B3 b q - 15 33", false },
        {            "p7/p5N1/B2n3Q/K4R1P/3n2q1/7k/P3QR2/8 b k e6 16 50", false },
        {                  "5Pkq/8/7N/2n5/1N4R1/7K/1Q6/3ppP2 w Q - 9 43",  true },
        {                 "6K1/8/Q4B2/1Q6/b1k5/4q1n1/8/1B5b w K - 41 68",  true },
        {               "1N6/2k4K/2P3r1/8/3R2N1/5b2/5b2/b3p3 w k - 1 50", false },
        {                 "3r1K2/1R6/p7/8/1qN3q1/1r6/1k5Q/8 w Q - 27 30",  true },
        {                  "p2B2P1/Q1b5/1Q6/K3k3/8/P4P2/8/8 b q - 42 98", false },
        {                     "BQ5n/5n2/8/7K/8/2B5/1k6/4n1N1 b K - 3 47", false },
        {             "8/n3B3/8/R7/3Q2P1/1rQ2B2/5R2/2K1k2r w k - 46 100",  true },
        {         "1n1N4/2Qr1k1N/1KR3N1/3Pn3/4Q3/4B3/pn2q3/8 w k - 10 9",  true },
        {      "2b5/1q2Bbq1/5n2/2R3qR/2kN4/6b1/P1r4Q/K3r2B b k h3 41 49",  true },
        {                "2q5/4br1R/5Q2/5Q2/2k5/Bnp5/7q/3K4 w - - 18 39", false },
        {               "8/3q4/7k/5K2/1q2R1N1/3N4/2N5/3P2p1 w K - 17 21",  true },
        {     "1P1BP1bK/N5r1/7R/1p3B1Q/6n1/3r4/k1B5/2n1P1q1 b K - 35 23", false },
        {            "2bP1P2/2p5/3K3r/4Q3/2b5/4kr2/q7/pq5r w - g6 21 63",  true },
        {            "K1B5/4P1B1/kN3Q1R/3P1B2/2q1QN2/8/8/2P5 w Q - 16 2",  true },
        { "7N/2b1p2P/1RK5/Nbqn1P2/3p2p1/2qr4/P1nP2k1/P1p1b3 w - d3 9 95", false },
        {              "2B3p1/2R3b1/8/p7/8/Kprk4/3b2b1/2Q5 b - b6 15 77", false },
        {       "1r2b3/2R5/1k1p2PQ/7N/3p2R1/P1R1K3/1bq5/R7 w - d6 41 72", false },
        {              "3rq3/2p4K/4n3/5nn1/k7/8/2n1Q3/3P1q2 b k - 35 89",  true },
        {       "K3R3/1P4NB/1q3R2/1k6/1qn2B2/3pr3/3q1B2/4b3 b - - 18 88", false },
        {             "N1r5/3nP3/bR2Q3/2RB3N/7k/3K4/b7/nQ5b w k - 31 78", false },
        {              "b7/8/3b2P1/3kr3/4R3/6K1/3q2b1/1q2r3 w K - 28 28", false },
        {                "2B1p2n/8/P7/Nn3b1K/1q6/N2p1kR1/8/8 b q - 0 36", false },
        {            "1RK3p1/2BNr3/6R1/6q1/8/b2Pq3/8/2RR1P1k w K - 24 3", false },
        {            "5P2/5Q2/8/2K2rb1/n3p1p1/6b1/2qn4/k4p2 b K - 15 60",  true },
        {             "2b1p3/5N2/2NNR1R1/2q5/8/8/qNQ4k/1P5K w k - 11 18",  true },
        {       "5B2/3q3K/1k3n2/R5r1/1Q2bR2/4P1b1/3B1b2/3pR3 w K - 2 22",  true },
        {          "4K3/8/1r6/3kNQp1/7b/Rr3B2/rP3nq1/1nP1q3 b Q h6 4 85", false },
        {    "Q2rKN2/3Pp3/n5k1/4nPN1/3r3Q/1N1r4/5p2/1BBR1B1B w q - 6 18",  true },
        {              "1NK5/3k1Q2/1B2N1Q1/7p/8/8/5RNN/6Q1 w - g3 16 90",  true },
        {                    "8/4Q3/6QP/3k4/bN2K3/4Q3/5b2/8 w k - 33 38",  true },
        {             "6K1/6q1/QnRb4/1Np4Q/7R/3Q4/3B1q2/5k2 b q - 29 11",  true },
        {                "5n2/4q3/4r3/b7/8/1Q5k/5KRB/1q2r3 b - d6 20 39", false },
        {                   "1R4q1/8/K2B4/5Q2/1N6/Q2qk3/8/8 w - - 10 61", false },
        {             "1q1k4/4q3/8/Q1n5/3b3B/2Qp3b/R6K/5n1n b K - 47 62",  true },
        {              "2PB4/q3BR2/8/N3k3/4P3/4P3/7K/1N2P3 w k c6 22 24", false },
        {                     "6Pp/5N2/8/3QN3/1B6/p7/6kK/Q7 b K - 29 24",  true },
        {                 "8/R7/4pk2/5R2/1BK3R1/6Q1/1P1Q4/8 w Q - 10 75",  true },
        {                     "8/6p1/b3QN2/5k2/8/p6P/7r/K7 b - f6 18 46", false },
        {              "3Q4/5P2/7N/3N2K1/8/3rk1bn/7b/1NQ1Q3 b k - 17 42",  true },
        {           "5B2/1P2k3/2n3K1/5bn1/5R1P/p3Bp2/3N4/4n3 w - - 6 94",  true },
        {             "4P3/4n1Q1/6b1/5k2/5N2/3n3B/7p/q2B2K1 w - - 16 65",  true },
        {                       "8/8/1k3r2/Nq6/7n/8/8/3K1b2 b - - 27 10", false },
        {                     "8/5N2/1K6/8/8/2Bq4/k7/3b2qp b K f3 39 75",  true },
        {        "kbK1R3/2n2q2/7n/1p6/1r1bP3/2p5/nB5B/2N2b2 w - - 12 68", false },
        {           "8/3b4/2k1pn2/2P4n/2qQp3/1nnb4/r7/KB1Q4 b - - 12 11",  true },
        {            "r3B3/1bb2k2/p7/3K2Q1/8/6bp/6Nb/2p2nB1 b - - 22 61",  true },
        {       "1b6/4p2k/6p1/1N1Q1n2/1pK1r2B/1q2b3/p3Rp2/8 w k - 30 99", false },
        {                      "Q2k1B2/8/3rb3/8/2K5/8/1N6/8 w Q - 43 73",  true },
        {                   "8/1r4Nk/2R5/3Q1P2/8/6Q1/7P/6K1 w K - 27 13", false },
        {             "5k2/2b5/p7/5B2/1K2n3/6n1/1b4q1/3r1b2 b K - 28 91", false },
        {         "5r2/1N6/8/5N1r/4n2k/2B2K2/1RR1NB2/2p1BR2 w - f6 8 12",  true },
        {           "5N2/8/5K2/8/2B2P2/5nr1/1Q1B2k1/1Qr1n3 b - - 29 100", false },
        {           "8/3b4/3p4/1K5n/3Q1Qn1/2n5/k1p1Rq1n/3N4 b Q - 39 12",  true },
        {              "2Q5/3n4/1Rq1KN2/3P4/7P/1b3B2/3k3q/8 b - - 15 47",  true },
        {         "3bn3/4Q1p1/2r3q1/2Q2k2/2QKr3/3R4/8/2p2R2 w Q - 28 77",  true },
        {         "n7/1n4p1/2KB2b1/4q1R1/QR6/1kb3p1/8/2n2b2 b K - 24 73", false },
        {            "3R2k1/QpK2R1p/N6P/RP2r3/q4b2/8/1R6/2r5 w q - 7 44",  true },
        {              "8/k1R2Q2/3r4/2QP3B/8/6B1/3K4/n1b2Q2 b Q - 18 53",  true },
        {           "p7/2n3PQ/3p1K2/N2b4/2n2bk1/Q7/B1Q5/2q5 b - - 47 54", false },
        {        "b2Q1q2/1r6/2R5/B1R3q1/p2K1kR1/4q3/8/1r3b2 b - - 15 92",  true },
        {            "pB6/4q1nk/7n/4p2B/1N3K2/P7/rn2bRb1/2r5 w - - 7 57", false },
    };
    check_was_into_check<PolyglotPiece>(tests);
    check_was_into_check<EnginePiece>(tests);
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
        _Position<PolyglotPiece> pos(tc.input.FEN);
        for (auto &move : tc.input.moves)
            pos.doMove(move);
        REQUIRE(pos.is_repetition(tc.input.repetition) == tc.info);
    }
    for (auto &tc : tests) {
        _Position<EnginePiece> pos(tc.input.FEN);
        for (auto &move : tc.input.moves)
            pos.doMove(move);
        REQUIRE(pos.is_repetition(tc.input.repetition) == tc.info);
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
    // These are removed due to illegal move (annotations aren't allowed strictly)
    TEST_CASE("Test King Castling Short move with Annotation") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_H1);
        #if defined(_DEBUG) && !defined(NDEBUG)
        REQUIRE_THROWS_WITH_AS(uci::parseSan(b, "0-0+?!"), "illegal san: '0-0+?!' in rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R w KQkq - 0 17", chess::uci::IllegalMoveException);
        #endif
        REQUIRE(uci::parseSan(b, "0-0+?!", true) == m);
    }

    TEST_CASE("Test King Castling Long move with Annotation") {
        auto b = Position{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1" };

        Move m = Move::make<Move::CASTLING>(Square::SQ_E1, Square::SQ_A1);

        #if defined(_DEBUG) && !defined(NDEBUG)
        REQUIRE_THROWS_WITH_AS(uci::parseSan(b, "0-0-0+?!"), "illegal san: '0-0-0+?!' in rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1", chess::uci::IllegalMoveException);
        #endif
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

        // CHECK(uci::moveToSan(b, m) == "Nxe5");
        REQUIRE(uci::parseSan(b, "Nxe5") == m);
    }

    TEST_CASE("Parse No Move") {
        Position b = Position();

        REQUIRE(uci::parseSan(b, "") == Move::NO_MOVE);
    }

    TEST_CASE("Should throw on ambiguous move") {
        auto b = Position{ "8/8/6K1/4k3/4N3/p4r2/N3N3/8 w - - 3 82" };

        Move san = Move::NO_MOVE;

        CHECK_THROWS_AS(san = uci::parseSan(b, "Nec3"), uci::AmbiguousMoveException);
        CHECK(san == Move::NO_MOVE);
    }

    TEST_CASE("Should throw for illegal move") {
        auto b = Position{ "8/8/6K1/4k3/4N3/p4r2/N3N3/8 w - - 3 82" };

        Move san = Move::NO_MOVE;

        CHECK_THROWS_WITH_AS(san = uci::parseSan(b, "Nec4"),
                             "illegal san: 'Nec4' in 8/8/6K1/4k3/4N3/p4r2/N3N3/8 w - - 3 82",
                             uci::IllegalMoveException);
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
TEST_CASE("Reconstruction of Position from history entry"){{
    Position p;
    Position p2(p.state());
    REQUIRE(p.fen()==p2.fen());}
    {
    _Position<PolyglotPiece> p;
    Position p2(p.state());
    _Position<PolyglotPiece> p3(p2.state());
    REQUIRE(p.fen()==p3.fen());
    REQUIRE(p.fen()==p2.fen());
    }
}
int main(int argc, char **argv) {
    doctest::Context ctx;
    ctx.setOption("success", true);
    ctx.setOption("no-breaks", true);
    return ctx.run();
}
