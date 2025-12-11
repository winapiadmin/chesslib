#pragma once
#include "fwd_decl.h"
#include <cstdint>
#include <iosfwd>
#include <type_traits>
namespace chess {
std::ostream &operator<<(std::ostream &os, const Move &mv);
std::ostream &operator<<(std::ostream &os, const Color &c);
std::ostream &operator<<(std::ostream &os, const CastlingRights &cr);
template <typename PieceC, typename = std::enable_if_t<is_piece_enum<PieceC>::value>>
std::ostream &operator<<(std::ostream &os, PieceC p);
std::ostream &operator<<(std::ostream &os, const Square &sq);
template <typename T, typename> class _Position;

template <typename PieceC = EnginePiece, typename = std::enable_if_t<is_piece_enum<PieceC>::value>>
std::ostream &operator<<(std::ostream &os, const _Position<PieceC, void> &pos);
} // namespace chess