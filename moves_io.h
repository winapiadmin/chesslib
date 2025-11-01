#pragma once
#include <stdint.h>
namespace std {
template <typename CharT> struct char_traits;

template <typename T> class allocator;

template <typename CharT, typename Traits, typename Alloc> class basic_string;

using string = basic_string<char, char_traits<char>, allocator<char>>;
} // namespace std

namespace chess {
class Move;
enum Square : int8_t;
namespace uci {
std::string moveToUci(const Move &move);

std::string squareToString(Square sq);
} // namespace uci
} // namespace chess
