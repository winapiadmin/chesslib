#pragma once
#include <array>
#include <cstdint>
namespace chess::zobrist {
extern std::array<uint64_t, 64> *RandomPiece[];
extern uint64_t RandomCastle[16];
extern uint64_t RandomEP[9];
extern uint64_t RandomTurn;
} // namespace chess::zobrist