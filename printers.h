#pragma once
#include <stdint.h>
namespace chess {
class Move;
enum Color : uint8_t;
enum CastlingRights : int8_t;
enum class PolyglotPiece : uint8_t;
enum class EnginePiece : uint8_t;
enum Square : int8_t;
std::ostream &operator<<(std::ostream &os, const Move &mv);
std::ostream &operator<<(std::ostream &os, const Color &c);
std::ostream &operator<<(std::ostream &os, const CastlingRights &cr);
std::ostream &operator<<(std::ostream &os, const PolyglotPiece &p);
std::ostream &operator<<(std::ostream &os, const EnginePiece &p);
std::ostream &operator<<(std::ostream &os, const Square &sq);
template <typename T, typename>
class _Position;

template <typename PieceC = EnginePiece>
std::ostream &operator<<(std::ostream &os, const _Position<PieceC, void> &pos);
}