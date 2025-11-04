#pragma once
#include <array>
#include <cstdint>
namespace chess::zobrist {
    extern const std::array<uint64_t, 64> RandomPiece[];
    extern const std::array<uint64_t, 64> RandomPiece_EnginePiece[];
    extern const uint64_t RandomCastle[];
    extern const uint64_t RandomEP[];
    extern const uint64_t RandomTurn;
}