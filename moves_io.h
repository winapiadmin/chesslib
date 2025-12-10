#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <stdexcept>
namespace chess {
class Move;
enum Square : int8_t;
template <typename T, typename> class _Position;
namespace uci {
std::string moveToUci(Move move);

std::string squareToString(Square sq);
class IllegalMoveException : public std::exception{
public:
    IllegalMoveException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};
class AmbiguousMoveException : public std::exception{
public:
    AmbiguousMoveException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};
template <typename T, typename P = void> Move uciToMove(const _Position<T, P> &pos, std::string_view uci);
template <typename T, typename P = void> Move parseSan(const _Position<T, P> &pos, std::string_view uci, bool remove_illegals=false);
template <typename T, typename P = void> std::string moveToSan(const _Position<T, P> &pos, Move move, bool long_=false, bool suffix=true);
} // namespace uci
} // namespace chess
