#pragma once
#include <cstdint>
#include <string>
#include <string_view>
namespace chess {
class Move;
enum Square : int8_t;
template <typename T, typename> class _Position;
namespace uci {
std::string moveToUci(const Move &move);

std::string squareToString(Square sq);
template <typename T, typename P = void> Move uciToMove(const _Position<T, P> &pos, std::string uci);
template <typename T, typename P = void> Move uciToMove(const _Position<T, P> &pos, std::string_view uci);
} // namespace uci
} // namespace chess
