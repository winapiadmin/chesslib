#pragma once
#include <stdint.h>
#include <string>

namespace chess {
class Move;
enum Square : int8_t;
namespace uci {
std::string moveToUci(const Move &move);

std::string squareToString(Square sq);
} // namespace uci
} // namespace chess
